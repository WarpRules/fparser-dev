#include "fpoptimizer_grammar.hh"
#include "fpoptimizer_codetree.hh"
#include "fpoptimizer_consts.hh"
#include "fpoptimizer_opcodename.hh"

#include <algorithm>
#include <cmath>
#include <map>
#include <assert.h>

#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"
using namespace FUNCTIONPARSERTYPES;

namespace FPoptimizer_CodeTree
{
    void CodeTree::ConstantFolding()
    {
        // Insert here any hardcoded constant-folding optimizations
        // that you want to be done at bytecode->codetree conversion time.
    }
}

namespace FPoptimizer_Grammar
{
    /* A helper for std::equal_range */
    struct OpcodeRuleCompare
    {
        bool operator() (unsigned opcode, const Rule& rule) const
        {
            return opcode < pack.flist[rule.input_index].opcode;
        }
        bool operator() (const Rule& rule, unsigned opcode) const
        {
            return pack.flist[rule.input_index].opcode < opcode;
        }
    };

    void DumpTree(const FPoptimizer_CodeTree::CodeTree& tree);

    /* Apply the grammar to a given CodeTree */
    bool Grammar::ApplyTo(
        std::set<uint_fast64_t>& optimized_children,
        FPoptimizer_CodeTree::CodeTree& tree,
        bool child_triggered,
        bool recursion) const
    {
        bool changed_once = false;

        /*if(!recursion)
        {
            std::cout << "Input: ";
            DumpTree(tree);
            std::cout << "\n";
        }*/

        if(optimized_children.find(tree.Hash) == optimized_children.end())
        {
            for(;;)
            {
                bool changed = false;

                if(!child_triggered)
                {
                    /* First optimize all children */
                    for(size_t a=0; a<tree.Params.size(); ++a)
                    {
                        if( ApplyTo( optimized_children, *tree.Params[a].param, false, true ) )
                        {
                            changed = true;
                        }
                    }
                }

                /* Figure out which rules _may_ match this tree */
                typedef const Rule* ruleit;

                std::pair<ruleit, ruleit> range
                    = std::equal_range(pack.rlist + index,
                                       pack.rlist + index + count,
                                       tree.Opcode,
                                       OpcodeRuleCompare());

                while(range.first < range.second)
                {
                    /* Check if this rule matches */
                    if(range.first->ApplyTo(tree))
                    {
                        changed = true;
                        break;
                    }
                    ++range.first;
                }

                if(!changed) break;

                // If we had a change at this point, mark up that we had one, and try again
                changed_once = true;
            }

            optimized_children.insert(tree.Hash);
        }

        /* If any changes whatsoever were done, recurse the optimization to parents */
        if((child_triggered || changed_once) && tree.Parent)
        {
            //ApplyTo( optimized_children, *tree.Parent, true , true);

            /* As this step may cause the tree we were passed to actually not exist,
             * don't touch the tree after this.
             *
             * FIXME: Is it even safe at all?
             */
        }

        /*if(!recursion)
        {
            std::cout << "Output: ";
            DumpTree(tree);
            std::cout << "\n";
        }*/

        return changed_once;
    }

    /* Store information about a potential match,
     * in order to iterate through candidates
     */
    struct MatchedParams::CodeTreeMatch
    {
        // Which parameters were matched -- these will be replaced if AnyParams are used
        std::vector<size_t> param_numbers;

        // Which values were saved for ImmedHolders?
        std::map<unsigned, double> ImmedMap;
        // Which codetrees were saved for each NameHolder? And how many?
        std::map<unsigned, std::pair<uint_fast64_t, size_t> > NamedMap;
        // Which codetrees were saved for each RestHolder?
        std::map<unsigned,
          std::vector<uint_fast64_t> > RestMap;

        // Examples of each codetree
        std::map<uint_fast64_t, FPoptimizer_CodeTree::CodeTree*> trees;

        CodeTreeMatch() : param_numbers(), ImmedMap(), NamedMap(), RestMap() { }
    };

    void DumpMatch(const Function& input,
                   const FPoptimizer_CodeTree::CodeTree& tree,
                   const MatchedParams& replacement,
                   const MatchedParams::CodeTreeMatch& matchrec,
                   bool DidMatch=true);
    void DumpFunction(const Function& input);
    void DumpParam(const ParamSpec& p);
    void DumpParams(const MatchedParams& mitem);

    /* Apply the rule to a given CodeTree */
    bool Rule::ApplyTo(
        FPoptimizer_CodeTree::CodeTree& tree) const
    {
        const Function&      input  = pack.flist[input_index];
        const MatchedParams& repl   = pack.mlist[repl_index];

        MatchedParams::CodeTreeMatch matchrec;
        if(input.opcode == tree.Opcode
        && pack.mlist[input.index].Match(tree, matchrec, false))
        {
            DumpMatch(input, tree, repl, matchrec);

            const MatchedParams& params = pack.mlist[input.index];
            switch(type)
            {
                case ReplaceParams:
                    repl.ReplaceParams(tree, params, matchrec);
                    std::cout << "  Produced(.): ";
                    DumpTree(tree);
                    std::cout << "\n";
                    return true;
                case ProduceNewTree:
                    repl.ReplaceTree(tree,   params, matchrec);
                    std::cout << "  Produced(*): ";
                    DumpTree(tree);
                    std::cout << "\n";
                    return true;
            }
        }
        else
        {
            //DumpMatch(input, tree, repl, matchrec, false);
        }
        return false;
    }


    /* Match the given function to the given CodeTree.
     */
    bool Function::Match(
        FPoptimizer_CodeTree::CodeTree& tree,
        MatchedParams::CodeTreeMatch& match) const
    {
        return opcode == tree.Opcode
            && pack.mlist[index].Match(tree, match);
    }


    /* This struct is used by MatchedParams::Match() for backtracking. */
    struct ParamMatchSnapshot
    {
        MatchedParams::CodeTreeMatch snapshot;
                                    // Snapshot of the state so far
        size_t            parampos; // Which position was last chosen?
        std::vector<bool> used;     // Which params were allocated?
    };

    /* Match the given list of ParamSpecs using the given ParamMatchingType
     * to the given CodeTree.
     * The CodeTree is already assumed to be a function type
     * -- i.e. it is assumed that the caller has tested the Opcode of the tree.
     */
    bool MatchedParams::Match(
        FPoptimizer_CodeTree::CodeTree& tree,
        MatchedParams::CodeTreeMatch& match,
        bool recursion) const
    {
        const size_t n_tree_params = tree.Params.size();

        /* FIXME: This algorithm still does not cover the possibility
         *        that the ParamSpec needs backtracking.
         *
         *        For example,
         *          cMul (cAdd x) (cAdd x)
         *        Applied to:
         *          (a+b)*(c+b)
         *
         *        Match (cAdd x) to (a+b) may first capture "a" into "x",
         *        and then Match(cAdd x) for (c+b) will fail,
         *        because there's no "a" there.
         *
         * FIXME: Repetition are not observed either, yet.
         */
        switch(type)
        {
            case PositionalParams:
            {
                if(count != n_tree_params) return false;
                for(size_t a=0; a<count; ++a)
                {
                    const ParamSpec& param = pack.plist[index+a];
                    if(param.sign != tree.Params[a].sign) return false;
                    if(!param.Match(*tree.Params[a].param, match)) return false;
                    if(!recursion)
                        match.param_numbers.push_back(a);
                }
                // Match = no mismatch.
                return true;
            }
            case AnyParams:
            {
                if(count > n_tree_params) return false;
                if(recursion && count != n_tree_params) return false;

                std::vector<ParamMatchSnapshot> position(count);
                std::vector<bool>               used(count);

                for(size_t a=0; a<count; ++a)
                {
                    const ParamSpec& param = pack.plist[index+a];

                    position[a].snapshot  = match;
                    position[a].parampos  = 0;
                    position[a].used      = used;

                    if(param.opcode == RestHolder)
                    {
                        // RestHolders always match. They're filled afterwards.
                        continue;
                    }

                    size_t b = 0;
                backtrack:
                    for(; b<n_tree_params; ++b)
                    {
                        if(!used[b])
                        {
                            if(param.sign != tree.Params[b].sign) continue;

                            if(param.Match(*tree.Params[b].param, match))
                            {
                                used[b] = true;
                                if(!recursion)
                                    match.param_numbers.push_back(b);
                                position[a].parampos = b+1;
                                goto ok;
                            }
                        }
                    }
                    // No match for this param, try backtracking.
                    while(a > 0)
                    {
                        --a;
                        ParamMatchSnapshot& prevpos = position[a];
                        if(prevpos.parampos < n_tree_params)
                        {
                            // Try another combination.
                            b     = prevpos.parampos;
                            match = prevpos.snapshot;
                            used  = prevpos.used;
                            goto backtrack;
                        }
                    }
                    // If we cannot backtrack, break. No possible match.
                    return false;
                ok:;
                }
                // Match = no mismatch.

                // Now feed any possible RestHolders the remaining parameters.
                for(size_t a=0; a<count; ++a)
                {
                    const ParamSpec& param = pack.plist[index+a];
                    if(param.opcode == RestHolder)
                    {
                        std::vector<uint_fast64_t>& RestList
                            = match.RestMap[param.index]; // mark it up

                        for(size_t b=0; b<n_tree_params; ++b)
                            if(tree.Params[b].sign == param.sign && !used[b])
                            {
                                if(!recursion)
                                    match.param_numbers.push_back(b);
                                uint_fast64_t hash = tree.Params[b].param->Hash;
                                RestList.push_back(hash);
                                match.trees.insert(
                                    std::make_pair(hash, tree.Params[b].param) );
                            }
                    }
                }
                return true;
            }
        }
        return false;
    }

    bool ParamSpec::Match(
        FPoptimizer_CodeTree::CodeTree& tree,
        MatchedParams::CodeTreeMatch& match) const
    {
        assert(opcode != RestHolder); // RestHolders are supposed to be handler by the caller

        switch(OpcodeType(opcode))
        {
            case NumConstant:
            {
                if(!tree.IsImmed()) return false;
                double res = tree.GetImmed();
                if(transformation == Negate) res = -res;
                if(transformation == Invert) res = 1/res;
                if(res != pack.clist[index]) return false;
                return true;
            }
            case ImmedHolder:
            {
                if(!tree.IsImmed()) return false;
                double res = tree.GetImmed();
                if(transformation == Negate) res = -res;
                if(transformation == Invert) res = 1/res;
                std::map<unsigned, double>::const_iterator
                    i = match.ImmedMap.find(index);
                if(i != match.ImmedMap.end())
                    return res == i->second;
                match.ImmedMap[index] = res;
                return true;
            }
            case NamedHolder:
            {
                if(minrepeat >= 2) return false;

                /* FIXME: Repetitions */
                std::map<unsigned, std::pair<uint_fast64_t, size_t> >::iterator
                    i = match.NamedMap.find(index);
                if(i != match.NamedMap.end())
                {
                    return tree.Hash == i->second.first;
                }
                match.NamedMap[index] = std::make_pair(tree.Hash, 1);
                match.trees.insert(std::make_pair(tree.Hash, &tree));
                return true;
            }
            case RestHolder:
                break;
            case SubFunction:
            {
                return pack.flist[index].Match(tree, match);
            }
            default:
            {
                if(!tree.IsImmed()) return false;
                double res = tree.GetImmed();
                bool impossible = false;
                if(res == GetConst(match, impossible))
                    return !impossible;
                return false;
            }
        }
        return false;
    }

    double ParamSpec::GetConst(
        MatchedParams::CodeTreeMatch& match,
        bool& impossible) const
    {
        double result = 1;
        switch(OpcodeType(opcode))
        {
            case NumConstant:
                result = pack.clist[index];
                break;
            case ImmedHolder:
            {
                std::map<unsigned, double>::const_iterator
                    i = match.ImmedMap.find(index);
                if(i == match.ImmedMap.end()) { impossible=true; return 1; }
                result = i->second;
                break;
            }
            case NamedHolder:
            {
                std::map<unsigned, std::pair<uint_fast64_t, size_t> >::const_iterator
                    i = match.NamedMap.find(index);
                if(i == match.NamedMap.end()) { impossible=true; return 1; }
                result = i->second.second;
                break;
            }
            case RestHolder:
            {
                // Not enumerable
                impossible = true;
                return 1;
            }
            case SubFunction:
            {
                // Not enumerable
                impossible = true;
                return 1;
            }
            default:
            {
                switch(OPCODE(opcode))
                {
                    case cAdd:
                        result=0;
                        for(unsigned p=0; p<count; ++p)
                            result += pack.plist[index+p].GetConst(match, impossible);
                        break;
                    case cMul:
                        result=1;
                        for(unsigned p=0; p<count; ++p)
                            result *= pack.plist[index+p].GetConst(match, impossible);
                        break;
                    case cMin:
                        for(unsigned p=0; p<count; ++p)
                        {
                            double tmp = pack.plist[index+p].GetConst(match, impossible);
                            if(p == 0 || tmp < result) result = tmp;
                        }
                        break;
                    case cMax:
                        for(unsigned p=0; p<count; ++p)
                        {
                            double tmp = pack.plist[index+p].GetConst(match, impossible);
                            if(p == 0 || tmp > result) result = tmp;
                        }
                        break;
                    case cSin: result = std::sin( pack.plist[index].GetConst(match, impossible) ); break;
                    case cCos: result = std::cos( pack.plist[index].GetConst(match, impossible) ); break;
                    case cTan: result = std::tan( pack.plist[index].GetConst(match, impossible) ); break;
                    case cAsin: result = std::asin( pack.plist[index].GetConst(match, impossible) ); break;
                    case cAcos: result = std::acos( pack.plist[index].GetConst(match, impossible) ); break;
                    case cAtan: result = std::atan( pack.plist[index].GetConst(match, impossible) ); break;
                    case cSinh: result = std::sinh( pack.plist[index].GetConst(match, impossible) ); break;
                    case cCosh: result = std::cosh( pack.plist[index].GetConst(match, impossible) ); break;
                    case cTanh: result = std::tanh( pack.plist[index].GetConst(match, impossible) ); break;
#ifndef FP_NO_ASINH
                    case cAsinh: result = std::asinh( pack.plist[index].GetConst(match, impossible) ); break;
                    case cAcosh: result = std::acosh( pack.plist[index].GetConst(match, impossible) ); break;
                    case cAtanh: result = std::atanh( pack.plist[index].GetConst(match, impossible) ); break;
#endif
                    case cCeil: result = std::ceil( pack.plist[index].GetConst(match, impossible) ); break;
                    case cFloor: result = std::floor( pack.plist[index].GetConst(match, impossible) ); break;
                    case cLog: result = std::log( pack.plist[index].GetConst(match, impossible) ); break;
                    case cLog2:
                        result = std::log( pack.plist[index].GetConst(match, impossible) ) * CONSTANT_L2I;
                        //result = std::log2( pack.plist[index].GetConst(match, impossible) );
                        break;
                    case cLog10: result = std::log10( pack.plist[index].GetConst(match, impossible) ); break;
                    case cPow: result = std::pow( pack.plist[index+0].GetConst(match, impossible),
                                                  pack.plist[index+1].GetConst(match, impossible) ); break;
                    case cMod: result = std::fmod( pack.plist[index+0].GetConst(match, impossible),
                                                   pack.plist[index+1].GetConst(match, impossible) ); break;
                    default:
                        impossible = true;
                }
            }
        }
        if(transformation == Negate) result = -result;
        if(transformation == Invert) result = 1.0 / result;
        return result;
    }

    void MatchedParams::ReplaceParams(
        FPoptimizer_CodeTree::CodeTree& tree,
        const MatchedParams& matcher,
        MatchedParams::CodeTreeMatch& match) const
    {
        // Replace the 0-level params indicated in "match" with the ones we have

        // First, construct the tree recursively using the "match" info

        for(size_t a=0; a<count; ++a)
        {
            const ParamSpec& param = pack.plist[index+a];
            if(param.opcode == RestHolder)
            {
                // Add children directly to this tree
                param.SynthesizeTree(tree, matcher, match);
            }
            else
            {
                FPoptimizer_CodeTree::CodeTree* subtree = new FPoptimizer_CodeTree::CodeTree;
                param.SynthesizeTree(*subtree, matcher, match);
                subtree->Parent = &tree;
                subtree->Recalculate_Hash_NoRecursion();
                tree.Params.push_back( FPoptimizer_CodeTree::CodeTree::Param(subtree, param.sign) );
            }
        }

        // Remove the indicated params
        std::sort(match.param_numbers.begin(), match.param_numbers.end());
        for(size_t a=match.param_numbers.size(); a-->0; )
        {
            size_t num = match.param_numbers[a];
            delete tree.Params[num].param;
            tree.Params.erase(tree.Params.begin() + num);
        }
        tree.Sort();
        //tree.Recalculate_Hash_NoRecursion();
        tree.Rehash(true); // rehash this and its parents
    }

    void MatchedParams::ReplaceTree(
        FPoptimizer_CodeTree::CodeTree& tree,
        const MatchedParams& matcher,
        CodeTreeMatch& match) const
    {
        // Replace the entire tree with one indicated by our Params[0]
        // Note: The tree is still constructed using the holders indicated in "match".
        std::vector<FPoptimizer_CodeTree::CodeTree::Param> OldParams = tree.Params;
        tree.Params.clear();
        pack.plist[index].SynthesizeTree(tree, matcher, match);
        for(size_t a=0; a<OldParams.size(); ++a)
            delete OldParams[a].param;

        tree.Sort();
        //tree.Recalculate_Hash_NoRecursion();
        tree.Rehash(true);  // rehash this and its parents
    }

    /* Synthesizes a new tree based on the given information
     * in ParamSpec. Assume the tree is empty, don't deallocate
     * anything. Don't touch Hash, Parent.
     */
    void ParamSpec::SynthesizeTree(
        FPoptimizer_CodeTree::CodeTree& tree,
        const MatchedParams& matcher,
        MatchedParams::CodeTreeMatch& match) const
    {
        switch(ParamMatchingType(opcode))
        {
            case RestHolder:
            {
                std::map<unsigned, std::vector<uint_fast64_t> >
                    ::const_iterator i = match.RestMap.find(index);

                assert(i != match.RestMap.end());

                /*std::cout << std::flush;
                fprintf(stderr, "Restmap %u, sign %d, size is %u -- params %u\n",
                    (unsigned) i->first, sign, (unsigned) i->second.size(),
                    (unsigned) tree.Params.size());*/

                for(size_t a=0; a<i->second.size(); ++a)
                {
                    uint_fast64_t hash = i->second[a];

                    std::map<uint_fast64_t, FPoptimizer_CodeTree::CodeTree*>
                        ::const_iterator j = match.trees.find(hash);

                    assert(j != match.trees.end());

                    FPoptimizer_CodeTree::CodeTree::Param p(j->second->Clone(), sign);
                    p.param->Parent = &tree;
                    tree.Params.push_back(p);
                }
                /*fprintf(stderr, "- params size became %u\n", (unsigned)tree.Params.size());
                fflush(stderr);*/
                break;
            }
            case SubFunction:
            {
                const Function& fitem = pack.flist[index];
                tree.Opcode = fitem.opcode;
                const MatchedParams& mitem = pack.mlist[fitem.index];
                for(unsigned a=0; a<mitem.count; ++a)
                {
                    const ParamSpec& param = pack.plist[mitem.index + a];
                    if(param.opcode == RestHolder)
                    {
                        // Add children directly to this tree
                        param.SynthesizeTree(tree, matcher, match);
                    }
                    else
                    {
                        FPoptimizer_CodeTree::CodeTree* subtree = new FPoptimizer_CodeTree::CodeTree;
                        param.SynthesizeTree(*subtree, matcher, match);
                        subtree->Sort();
                        subtree->Recalculate_Hash_NoRecursion();
                        FPoptimizer_CodeTree::CodeTree::Param p(subtree, sign^param.sign) ;
                        p.param->Parent = &tree;
                        tree.Params.push_back(p);
                    }
                }
                break;
            }
            case NamedHolder:
                if(!anyrepeat && minrepeat == 1)
                {
                    /* Literal parameter */
                    std::map<unsigned, std::pair<uint_fast64_t, size_t> >
                        ::const_iterator i = match.NamedMap.find(index);

                    assert(i != match.NamedMap.end());

                    uint_fast64_t hash = i->second.first;

                    std::map<uint_fast64_t, FPoptimizer_CodeTree::CodeTree*>
                        ::const_iterator j = match.trees.find(hash);

                    assert(j != match.trees.end());

                    tree.Opcode = j->second->Opcode;
                    switch(tree.Opcode)
                    {
                        case cImmed: tree.Value = j->second->Value; break;
                        case cVar:   tree.Var   = j->second->Var;  break;
                        case cFCall:
                        case cPCall: tree.Funcno = j->second->Funcno; break;
                    }
                    tree.Params = j->second->Params;
                    for(size_t a=0; a<tree.Params.size(); ++a)
                    {
                        tree.Params[a].param = tree.Params[a].param->Clone();
                        tree.Params[a].param->Parent = &tree;
                    }
                    break;
                }
                // passthru; x+ is synthesized as the number, not as the tree
            case NumConstant:
            case ImmedHolder:
            default:
                tree.Opcode = cImmed;
                bool impossible = false;
                tree.Value  = GetConst(match, impossible);
                break;
        }
    }

    void DumpParam(const ParamSpec& p)
    {
        //std::cout << "/*p" << (&p-pack.plist) << "*/";

        static const char ImmedHolderNames[2][2] = {"%","&"};
        static const char NamedHolderNames[6][2] = {"x","y","z","a","b","c"};

        if(p.sign) std::cout << '~';
        if(p.transformation == Negate) std::cout << '-';
        if(p.transformation == Invert) std::cout << '/';

        switch(SpecialOpcode(p.opcode))
        {
            case NumConstant: std::cout << pack.clist[p.index]; break;
            case ImmedHolder: std::cout << ImmedHolderNames[p.index]; break;
            case NamedHolder: std::cout << NamedHolderNames[p.index]; break;
            case RestHolder: std::cout << '<' << p.index << '>'; break;
            case SubFunction: DumpFunction(pack.flist[p.index]); break;
            default:
            {
                std::string opcode = FP_GetOpcodeName(p.opcode).substr(1);
                for(size_t a=0; a<opcode.size(); ++a) opcode[a] = std::toupper(opcode[a]);
                std::cout << opcode << '(';
                for(unsigned a=0; a<p.count; ++a)
                {
                    if(a > 0) std::cout << ' ';
                    DumpParam(pack.plist[p.index+a]);
                }
                std::cout << " )";
            }
        }
        if(p.anyrepeat && p.minrepeat==1) std::cout << '*';
        if(p.anyrepeat && p.minrepeat==2) std::cout << '+';
    }

    void DumpParams(const MatchedParams& mitem)
    {
        //std::cout << "/*m" << (&mitem-pack.mlist) << "*/";

        if(mitem.type == PositionalParams) std::cout << '[';

        for(unsigned a=0; a<mitem.count; ++a)
        {
            std::cout << ' ';
            DumpParam(pack.plist[mitem.index + a]);
        }

        if(mitem.type == PositionalParams) std::cout << " ]";
    }

    void DumpFunction(const Function& fitem)
    {
        //std::cout << "/*f" << (&fitem-pack.flist) << "*/";

        std::cout << '(' << FP_GetOpcodeName(fitem.opcode);
        DumpParams(pack.mlist[fitem.index]);
        std::cout << ')';
    }
    void DumpTree(const FPoptimizer_CodeTree::CodeTree& tree)
    {
        //std::cout << "/*" << tree.Depth << "*/";
        const char* sep2 = "";
        switch(tree.Opcode)
        {
            case cImmed: std::cout << tree.Value; return;
            case cVar:   std::cout << "Var" << tree.Var; return;
            case cAdd: sep2 = " +"; break;
            case cMul: sep2 = " *"; break;
            case cAnd: sep2 = " &&"; break;
            case cOr: sep2 = " or"; break;
            default:
                std::cout << FP_GetOpcodeName(tree.Opcode);
                if(tree.Opcode == cFCall || tree.Opcode == cPCall)
                    std::cout << ':' << tree.Funcno;
        }
        std::cout << '(';
        if(tree.Params.size() <= 1) std::cout << (sep2+1) << ' ';
        for(size_t a=0; a<tree.Params.size(); ++a)
        {
            if(a > 0) std::cout << ' ';
            if(tree.Params[a].sign) std::cout << '~';

            DumpTree(*tree.Params[a].param);

            if(tree.Params[a].param->Parent != &tree)
            {
                std::cout << "(?""?""?))";
            }

            if(a+1 < tree.Params.size()) std::cout << sep2;
        }
        std::cout << ')';
    }
    void DumpMatch(const Function& input,
                   const FPoptimizer_CodeTree::CodeTree& tree,
                   const MatchedParams& replacement,
                   const MatchedParams::CodeTreeMatch& matchrec,
                   bool DidMatch)
    {
        std::cout <<
            "Found " << (DidMatch ? "match" : "mismatch") << ":\n"
            "  Pattern    : ";
        DumpFunction(input);
        std::cout << "\n"
            "  Replacement: ";
        DumpParams(replacement);
        std::cout << "\n";

        std::cout <<
            "  Tree       : ";
        DumpTree(tree);
        std::cout << "\n";

        for(std::map<unsigned, std::pair<uint_fast64_t, size_t> >::const_iterator
            i = matchrec.NamedMap.begin(); i != matchrec.NamedMap.end(); ++i)
        {
            static const char NamedHolderNames[6][2] = {"x","y","z","a","b","c"};
            std::cout << "           " << NamedHolderNames[i->first] << " = ";
            DumpTree(*matchrec.trees.find(i->second.first)->second);
            std::cout << std::endl;
        }

        for(std::map<unsigned, double>::const_iterator
            i = matchrec.ImmedMap.begin(); i != matchrec.ImmedMap.end(); ++i)
        {
            static const char ImmedHolderNames[2][2] = {"%","&"};
            std::cout << "           " << ImmedHolderNames[i->first] << " = ";
            std::cout << i->second << std::endl;
        }

        for(std::map<unsigned, std::vector<uint_fast64_t> >::const_iterator
            i = matchrec.RestMap.begin(); i != matchrec.RestMap.end(); ++i)
        {
            for(size_t a=0; a<i->second.size(); ++a)
            {
                uint_fast64_t hash = i->second[a];
                std::cout << "         <" << i->first << "> = ";
                DumpTree(*matchrec.trees.find(hash)->second);
                std::cout << std::endl;
            }
            if(i->second.empty())
                std::cout << "         <" << i->first << "> = <empty>\n";
        }
    }
}
