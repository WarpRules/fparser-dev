#include "bytecodesynth.hh"
#include "codetree.hh"

#ifdef FP_SUPPORT_OPTIMIZER

using namespace FUNCTIONPARSERTYPES;
//using namespace FPoptimizer_Grammar;

//#define DEBUG_SUBSTITUTIONS_CSE

namespace
{
    using namespace FPoptimizer_CodeTree;

    class TreeCountItem
    {
        size_t n_occurrences;
        size_t n_as_cos_param;
        size_t n_as_sin_param;
    public:
        TreeCountItem() :
            n_occurrences(0),
            n_as_cos_param(0),
            n_as_sin_param(0) { }

        void AddFrom(OPCODE op)
        {
            n_occurrences += 1;
            if(op == cCos) ++n_as_cos_param;
            if(op == cSin) ++n_as_sin_param;
            if(op == cSec) ++n_as_cos_param;
            if(op == cCsc) ++n_as_sin_param;
        }

        size_t GetCSEscore() const
        {
            //size_t n_sincos = std::min(n_as_cos_param, n_as_sin_param);
            size_t result = n_occurrences;// - n_sincos;
            return result;
        }

        int NeedsSinCos() const
        {
            if(n_as_cos_param > 0 && n_as_sin_param > 0)
            {
                if(n_occurrences == n_as_cos_param + n_as_sin_param)
                    return 1;
                return 2;
            }
            return 0;
        }

        size_t MinimumDepth() const
        {
            size_t n_sincos = std::min(n_as_cos_param, n_as_sin_param);
            if(n_sincos == 0)
                return 2;
            return 1;
        }
    };

    template<typename Value_t>
    class TreeCountType:
        public std::multimap<fphash_t,  std::pair<TreeCountItem, CodeTree<Value_t> > >
    {
    };

    template<typename Value_t>
    void FindTreeCounts(
        TreeCountType<Value_t>& TreeCounts,
        const CodeTree<Value_t>& tree,
        OPCODE parent_opcode)
    {
        typename TreeCountType<Value_t>::iterator
            i = TreeCounts.lower_bound(tree.GetHash());
        bool found = false;
        for(; i != TreeCounts.end() && i->first == tree.GetHash(); ++i)
        {
            if(tree.IsIdenticalTo( i->second.second ) )
            {
                i->second.first.AddFrom(parent_opcode);
                found = true;
                break;
            }
        }
        if(!found)
        {
            TreeCountItem count;
            count.AddFrom(parent_opcode);
            TreeCounts.insert(i, std::make_pair(tree.GetHash(),
                std::make_pair(count, tree)));
        }

        for(size_t a=0; a<tree.GetParamCount(); ++a)
            FindTreeCounts(TreeCounts, tree.GetParam(a),
                           tree.GetOpcode());
    }

    struct BalanceResultType
    {
        bool BalanceGood;
        bool FoundChild;
    };

    template<typename Value_t>
    BalanceResultType IfBalanceGood(const CodeTree<Value_t>& root,
                                    const CodeTree<Value_t>& child)
    {
        if(root.IsIdenticalTo(child))
        {
            BalanceResultType result = {true,true};
            return result;
        }

        BalanceResultType result = {true,false};

        if(root.GetOpcode() == cIf
        || root.GetOpcode() == cAbsIf)
        {
            BalanceResultType cond    = IfBalanceGood(root.GetParam(0), child);
            BalanceResultType branch1 = IfBalanceGood(root.GetParam(1), child);
            BalanceResultType branch2 = IfBalanceGood(root.GetParam(2), child);

            if(cond.FoundChild || branch1.FoundChild || branch2.FoundChild)
                { result.FoundChild = true; }

            // balance is good if:
            //      branch1.found = branch2.found OR (cond.found AND cond.goodbalance)
            // AND  cond.goodbalance OR (branch1.found AND branch2.found)
            // AND  branch1.goodbalance OR (cond.found AND cond.goodbalance)
            // AND  branch2.goodbalance OR (cond.found AND cond.goodbalance)

            result.BalanceGood =
                (   (branch1.FoundChild == branch2.FoundChild)
                 || (cond.FoundChild && cond.BalanceGood) )
             && (cond.BalanceGood || (branch1.FoundChild && branch2.FoundChild))
             && (branch1.BalanceGood || (cond.FoundChild && cond.BalanceGood))
             && (branch2.BalanceGood || (cond.FoundChild && cond.BalanceGood));
        }
        else
        {
            bool has_bad_balance        = false;
            bool has_good_balance_found = false;

            // Balance is bad if one of the children has bad balance
            // Unless one of the children has good balance & found

            for(size_t b=root.GetParamCount(), a=0; a<b; ++a)
            {
                BalanceResultType tmp = IfBalanceGood(root.GetParam(a), child);
                if(tmp.FoundChild)
                    result.FoundChild = true;

                if(tmp.BalanceGood == false)
                    has_bad_balance = true;
                else if(tmp.FoundChild)
                    has_good_balance_found = true;

                // if the expression is
                //   if(x, sin(x), 0) + sin(x)
                // then sin(x) is a good subexpression
                // even though it occurs in unbalance.
            }
            if(has_bad_balance && !has_good_balance_found)
                result.BalanceGood = false;
        }
        return result;
    }

    template<typename Value_t>
    bool ContainsOtherCandidates(
        const CodeTree<Value_t>& within,
        const CodeTree<Value_t>& tree,
        const FPoptimizer_ByteCode::ByteCodeSynth<Value_t>& synth,
        const TreeCountType<Value_t>& TreeCounts)
    {
        for(size_t b=tree.GetParamCount(), a=0; a<b; ++a)
        {
            const CodeTree<Value_t>& leaf = tree.GetParam(a);

            typename TreeCountType<Value_t>::iterator synth_it;
            for(typename TreeCountType<Value_t>::const_iterator
                i = TreeCounts.begin();
                i != TreeCounts.end();
                ++i)
            {
                if(i->first != leaf.GetHash())
                    continue;

                const TreeCountItem& occ  = i->second.first;
                size_t          score     = occ.GetCSEscore();
                const CodeTree<Value_t>& candidate = i->second.second;

                // It must not yet have been synthesized
                if(synth.Find(candidate))
                    continue;

                // And it must not be a simple expression
                // Because cImmed, VarBegin are faster than cFetch
                if(leaf.GetDepth() < occ.MinimumDepth())
                    continue;

                // It must always occur at least twice
                if(score < 2)
                    continue;

                // And it must either appear on both sides
                // of a cIf, or neither
                if(IfBalanceGood(within, leaf).BalanceGood == false)
                    continue;

                return true;
            }
            if(ContainsOtherCandidates(within, leaf, synth, TreeCounts))
                return true;
        }
        return false;
    }

    template<typename Value_t>
    bool IsDescendantOf(const CodeTree<Value_t>& parent, const CodeTree<Value_t>& expr)
    {
        for(size_t a=0; a<parent.GetParamCount(); ++a)
            if(parent.GetParam(a).IsIdenticalTo(expr))
                return true;

        for(size_t a=0; a<parent.GetParamCount(); ++a)
            if(IsDescendantOf(parent.GetParam(a), expr))
                return true;

        return false;
    }

    template<typename Value_t>
    bool GoodMomentForCSE(const CodeTree<Value_t>& parent, const CodeTree<Value_t>& expr)
    {
        if(parent.GetOpcode() == cIf)
            return true;

        // Good if it's one of our direct children
        // Bad if it is a descendant of only one of our children

        for(size_t a=0; a<parent.GetParamCount(); ++a)
            if(parent.GetParam(a).IsIdenticalTo(expr))
                return true;

        size_t leaf_count = 0;
        for(size_t a=0; a<parent.GetParamCount(); ++a)
            if(IsDescendantOf(parent.GetParam(a), expr))
                ++leaf_count;

        return leaf_count != 1;
    }

}

namespace FPoptimizer_CodeTree
{
    template<typename Value_t>
    size_t CodeTree<Value_t>::SynthCommonSubExpressions(
        FPoptimizer_ByteCode::ByteCodeSynth<Value_t>& synth) const
    {
        size_t stacktop_before = synth.GetStackTop();

        /* Find common subtrees */
        TreeCountType<Value_t> TreeCounts;
        FindTreeCounts(TreeCounts, *this, GetOpcode());

    #ifdef DEBUG_SUBSTITUTIONS_CSE
        DumpHashes(*this);
    #endif

        /* Synthesize some of the most common ones */
        for(;;)
        {
            size_t best_score = 0;
            typename TreeCountType<Value_t>::iterator synth_it;
            for(typename TreeCountType<Value_t>::iterator
                j,i = TreeCounts.begin();
                i != TreeCounts.end();
                i=j)
            {
                j=i; ++j;

                const TreeCountItem& occ  = i->second.first;
                size_t          score     = occ.GetCSEscore();
                const CodeTree<Value_t>& tree = i->second.second;

    #ifdef DEBUG_SUBSTITUTIONS_CSE
                std::cout << "Score " << score << ":\n";
                DumpTreeWithIndent(tree);
    #endif

                // It must not yet have been synthesized
                if(synth.Find(tree))
                {
                    TreeCounts.erase(i);
                    continue;
                }

                // And it must not be a simple expression
                // Because cImmed, VarBegin are faster than cFetch
                if(tree.GetDepth() < occ.MinimumDepth())
                {
                    TreeCounts.erase(i);
                    continue;
                }

                // It must always occur at least twice
                if(score < 2)
                {
                    TreeCounts.erase(i);
                    continue;
                }

                // And it must either appear on both sides
                // of a cIf, or neither
                if(IfBalanceGood(*this, tree).BalanceGood == false)
                {
                    TreeCounts.erase(i);
                    continue;
                }

                // It must not contain other candidates
                if(ContainsOtherCandidates(*this, tree, synth, TreeCounts))
                {
                    // Don't erase it; it may be a proper candidate later
                    continue;
                }

                if(!GoodMomentForCSE(*this, tree))
                {
                    TreeCounts.erase(i);
                    continue;
                }

                // Is a candidate.
                score *= tree.GetDepth();
                if(score > best_score)
                    { best_score = score; synth_it = i; }
            }

            if(best_score <= 0) break; // Didn't find anything.

            const TreeCountItem& occ  = synth_it->second.first;
            const CodeTree<Value_t>& tree = synth_it->second.second;
    #ifdef DEBUG_SUBSTITUTIONS_CSE
            std::cout << "Found Common Subexpression:"; DumpTree<Value_t>(tree); std::cout << "\n";
    #endif

            int needs_sincos = occ.NeedsSinCos();
            CodeTree<Value_t> sintree, costree;
            if(needs_sincos)
            {
                sintree.AddParam(tree);
                sintree.SetOpcode(cSin);
                sintree.Rehash();
                costree.AddParam(tree);
                costree.SetOpcode(cCos);
                costree.Rehash();
                if(synth.Find(sintree) || synth.Find(costree))
                {
                    if(needs_sincos == 2)
                    {
                        // sin, cos already found, and we don't
                        // actually need _this_ tree by itself
                        TreeCounts.erase(synth_it);
                        continue;
                    }
                    needs_sincos = 0;
                }
            }

            /* Synthesize the selected tree */
            tree.SynthesizeByteCode(synth, false);
            TreeCounts.erase(synth_it);
    #ifdef DEBUG_SUBSTITUTIONS_CSE
            std::cout << "Done with Common Subexpression:"; DumpTree<Value_t>(tree); std::cout << "\n";
    #endif
            if(needs_sincos)
            {
                if(needs_sincos == 2)
                {
                    // make a duplicate of the value, since it
                    // is also needed in addition to the sin/cos.
                    synth.DoDup(synth.GetStackTop()-1);
                }
                synth.AddOperation(cSinCos, 1, 2);

                synth.StackTopIs(sintree, 1);
                synth.StackTopIs(costree, 0);
            }
        }

        return synth.GetStackTop() - stacktop_before;
    }
}

/* BEGIN_EXPLICIT_INSTANTATION */
namespace FPoptimizer_CodeTree
{
    template
    size_t CodeTree<double>::SynthCommonSubExpressions(
        FPoptimizer_ByteCode::ByteCodeSynth<double>& synth) const;
#ifdef FP_SUPPORT_FLOAT_TYPE
    template
    size_t CodeTree<float>::SynthCommonSubExpressions(
        FPoptimizer_ByteCode::ByteCodeSynth<float>& synth) const;
#endif
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
    template
    size_t CodeTree<long double>::SynthCommonSubExpressions(
        FPoptimizer_ByteCode::ByteCodeSynth<long double>& synth) const;
#endif
}
/* END_EXPLICIT_INSTANTATION */

#endif
