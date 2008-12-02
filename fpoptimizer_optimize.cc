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

//#define DEBUG_SUBSTITUTIONS

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
        bool operator() (const FPoptimizer_CodeTree::CodeTree& tree, const Rule& rule) const
        {
            /* If this function returns true, len=half.
             */

            if(tree.Opcode != rule.func.opcode)
                return tree.Opcode < rule.func.opcode;

            if(tree.Params.size() < rule.n_minimum_params)
            {
                // Tree has fewer params than required?
                return true; // Failure
            }
            return false;
        }
        bool operator() (const Rule& rule, const FPoptimizer_CodeTree::CodeTree& tree) const
        {
            /* If this function returns true, rule will be excluded from the equal_range
             */

            if(rule.func.opcode != tree.Opcode)
                return rule.func.opcode < tree.Opcode;

            if(rule.n_minimum_params < tree.Params.size())
            {
                // Tree has more params than the pattern has?
                switch(pack.mlist[rule.func.index].type)
                {
                    case PositionalParams:
                        return true; // Failure
                    case AnyParams:
                        return false; // Not a failure
                }
            }
            return false;
        }
    };

#ifdef DEBUG_SUBSTITUTIONS
    void DumpTree(const FPoptimizer_CodeTree::CodeTree& tree);
    static const char ImmedHolderNames[2][2] = {"%","&"};
    static const char NamedHolderNames[6][2] = {"x","y","z","a","b","c"};
#endif

    /* Apply the grammar to a given CodeTree */
    bool Grammar::ApplyTo(
        std::set<uint_fast64_t>& optimized_children,
        FPoptimizer_CodeTree::CodeTree& tree,
        bool recursion) const
    {
        bool changed = false;

#ifdef DEBUG_SUBSTITUTIONS
        if(!recursion)
        {
            std::cout << "Input:  ";
            DumpTree(tree);
            std::cout << "\n";
        }
#endif
        std::set<uint_fast64_t>::iterator
            i = optimized_children.lower_bound(tree.Hash);
        if(i == optimized_children.end() || *i != tree.Hash)
        {
            /* First optimize all children */
            for(size_t a=0; a<tree.Params.size(); ++a)
            {
                if( ApplyTo( optimized_children, *tree.Params[a].param, true ) )
                {
                    changed = true;
                }
            }

            /* Figure out which rules _may_ match this tree */
            typedef const Rule* ruleit;

            std::pair<ruleit, ruleit> range
                = std::equal_range(pack.rlist + index,
                                   pack.rlist + index + count,
                                   tree,
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

            if(!changed)
                optimized_children.insert(i, tree.Hash);
        }

#ifdef DEBUG_SUBSTITUTIONS
        if(!recursion)
        {
            std::cout << "Output: ";
            DumpTree(tree);
            std::cout << "\n";
        }
#endif
        return changed;
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
        std::map<uint_fast64_t, FPoptimizer_CodeTree::CodeTreeP> trees;

        CodeTreeMatch() : param_numbers(), ImmedMap(), NamedMap(), RestMap() { }
    };

#ifdef DEBUG_SUBSTITUTIONS
    void DumpMatch(const Function& input,
                   const FPoptimizer_CodeTree::CodeTree& tree,
                   const MatchedParams& replacement,
                   const MatchedParams::CodeTreeMatch& matchrec,
                   bool DidMatch=true);
    void DumpFunction(const Function& input);
    void DumpParam(const ParamSpec& p);
    void DumpParams(const MatchedParams& mitem);
#endif

    /* Apply the rule to a given CodeTree */
    bool Rule::ApplyTo(
        FPoptimizer_CodeTree::CodeTree& tree) const
    {
        const Function&      input  = func;
        const MatchedParams& repl   = pack.mlist[repl_index];

        MatchedParams::CodeTreeMatch matchrec;
        if(input.opcode == tree.Opcode
        && pack.mlist[input.index].Match(tree, matchrec, false))
        {
#ifdef DEBUG_SUBSTITUTIONS
            DumpMatch(input, tree, repl, matchrec);
#endif

            const MatchedParams& params = pack.mlist[input.index];
            switch(type)
            {
                case ReplaceParams:
                    repl.ReplaceParams(tree, params, matchrec);
#ifdef DEBUG_SUBSTITUTIONS
                    std::cout << "  ParmReplace: ";
                    DumpTree(tree);
                    std::cout << "\n";
#endif
                    return true;
                case ProduceNewTree:
                    repl.ReplaceTree(tree,   params, matchrec);
#ifdef DEBUG_SUBSTITUTIONS
                    std::cout << "  TreeReplace: ";
                    DumpTree(tree);
                    std::cout << "\n";
#endif
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
         */


        /* First, check if the tree has any chances of matching... */
        /* Figure out what we need. */
        struct Needs
        {
            struct Needs_Pol
            {
                int Immeds;
                int SubTrees;
                int Others;
                unsigned SubTreesDetail[VarBegin];

                Needs_Pol(): Immeds(0), SubTrees(0), Others(0), SubTreesDetail()
                {
                }
            } polarity[2]; // 0=positive, 1=negative
        } NeedList;

        // Figure out what we need
        for(unsigned a=0; a<count; ++a)
        {
            const ParamSpec& param = pack.plist[index+a];
            Needs::Needs_Pol& needs = NeedList.polarity[param.sign];
            switch(param.opcode)
            {
                case SubFunction:
                    needs.SubTrees += 1;
                    assert( pack.flist[param.index].opcode < VarBegin );
                    needs.SubTreesDetail[ pack.flist[param.index].opcode ] += 1;
                    break;
                case NumConstant:
                case ImmedHolder:
                default:
                    needs.Immeds += 1;
                    break;
                case NamedHolder:
                    needs.Others += param.minrepeat;
                    break;
                case RestHolder:
                    break;
            }
        }

        // Figure out what we have (note: we already assume that the opcode of the tree matches!)
        for(size_t a=0; a<tree.Params.size(); ++a)
        {
            Needs::Needs_Pol& needs = NeedList.polarity[tree.Params[a].sign];
            unsigned opcode = tree.Params[a].param->Opcode;
            switch(opcode)
            {
                case cImmed:
                    if(needs.Immeds > 0) needs.Immeds -= 1;
                    else needs.Others -= 1;
                    break;
                case cVar:
                case cFCall:
                case cPCall:
                    needs.Others -= 1;
                    break;
                default:
                    assert( opcode < VarBegin );
                    if(needs.SubTrees > 0
                    && needs.SubTreesDetail[opcode] > 0)
                    {
                        needs.SubTrees -= 1;
                        needs.SubTreesDetail[opcode] -= 1;
                    }
                    else needs.Others -= 1;
            }
        }

        // Check whether all needs were satisfied
        if(NeedList.polarity[0].Immeds > 0
        || NeedList.polarity[0].SubTrees > 0
        || NeedList.polarity[0].Others > 0
        || NeedList.polarity[1].Immeds > 0
        || NeedList.polarity[1].SubTrees > 0
        || NeedList.polarity[1].Others > 0)
        {
            // Something came short.
            return false;
        }

        switch(type)
        {
            case PositionalParams:
            {
                /*DumpTree(tree);
                std::cout << "<->";
                DumpParams(*this);
                std::cout << " -- ";*/

                if(NeedList.polarity[0].Immeds < 0
                || NeedList.polarity[0].SubTrees < 0
                || NeedList.polarity[0].Others < 0
                || NeedList.polarity[1].Immeds < 0
                || NeedList.polarity[1].SubTrees < 0
                || NeedList.polarity[1].Others < 0
                || count != tree.Params.size())
                {
                    // Something was too much.
                    return false;
                }

                for(unsigned a=0; a<count; ++a)
                {
                    const ParamSpec& param = pack.plist[index+a];
                    if(param.sign != tree.Params[a].sign
                    || !param.Match(*tree.Params[a].param, match))
                    {
                        /*std::cout << " drats at " << a << "!\n";*/
                        return false;
                    }
                    if(!recursion)
                        match.param_numbers.push_back(a);
                }
                /*std::cout << " yay?\n";*/
                // Match = no mismatch.
                return true;
            }
            case AnyParams:
            {
                const size_t n_tree_params = tree.Params.size();

                bool HasRestHolders = false;
                for(unsigned a=0; a<count; ++a)
                {
                    const ParamSpec& param = pack.plist[index+a];
                    if(param.opcode == RestHolder) { HasRestHolders = true; break; }
                }

                if(!HasRestHolders && recursion && count != n_tree_params)
                {
                    /*DumpTree(tree);
                    std::cout << "<->";
                    DumpParams(*this);
                    std::cout << " -- fail due to recursion&&count!=n_tree_params";*/
                    return false;
                }

                std::vector<ParamMatchSnapshot> position(count);
                std::vector<bool>               used(count);

                for(unsigned a=0; a<count; ++a)
                {
                    position[a].snapshot  = match;
                    position[a].parampos  = 0;
                    position[a].used      = used;

                    size_t b = 0;
                backtrack:
                    const ParamSpec& param = pack.plist[index+a];

                    if(param.opcode == RestHolder)
                    {
                        // RestHolders always match. They're filled afterwards.
                        continue;
                    }

                    for(; b<n_tree_params; ++b)
                    {
                        if(!used[b])
                        {
                            /*std::cout << "Maybe [" << a << "]:";
                            DumpParam(param);
                            std::cout << " <-> ";
                            if(tree.Params[b].sign) std::cout << '~';
                            DumpTree(*tree.Params[b].param);
                            std::cout << "...?\n";*/

                            if(param.sign == tree.Params[b].sign
                            && param.Match(*tree.Params[b].param, match))
                            {
                                /*std::cout << "woo... " << a << ", " << b << "\n";*/
                                /* NamedHolders require a special treatment,
                                 * because a repetition count may be issued
                                 * for them.
                                 */
                                if(param.opcode == NamedHolder)
                                {
                                    // Verify the MinRepeat & AnyRepeat case
                                    unsigned MinRepeat = param.minrepeat;
                                    bool AnyRepeat     = param.anyrepeat;
                                    unsigned HadRepeat = 1;

                                    for(size_t c = b+1;
                                        c < n_tree_params && (HadRepeat < MinRepeat || AnyRepeat);
                                        ++c)
                                    {
                                        if(tree.Params[c].param->Hash == tree.Params[b].param->Hash
                                        && tree.Params[c].sign == param.sign)
                                        {
                                            ++HadRepeat;
                                        }
                                    }
                                    if(HadRepeat < MinRepeat)
                                        continue; // No sufficient repeat count here

                                    used[b] = true;
                                    if(!recursion) match.param_numbers.push_back(b);

                                    HadRepeat = 1;
                                    for(size_t c = b+1;
                                        c < n_tree_params && (HadRepeat < MinRepeat || AnyRepeat);
                                        ++c)
                                    {
                                        if(tree.Params[c].param->Hash == tree.Params[b].param->Hash
                                        && tree.Params[c].sign == param.sign)
                                        {
                                            ++HadRepeat;
                                            used[c] = true;
                                            if(!recursion) match.param_numbers.push_back(c);
                                        }
                                    }
                                    if(AnyRepeat)
                                        match.NamedMap[param.index].second = HadRepeat;
                                    position[a].parampos = b+1;
                                    goto ok;
                                }

                                used[b] = true;
                                if(!recursion) match.param_numbers.push_back(b);
                                position[a].parampos = b+1;
                                goto ok;
                            }
                        }
                    }

                    /*DumpParam(param);
                    std::cout << " didn't match anything in ";
                    DumpTree(tree);
                    std::cout << "\n";*/

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
                    /*if(!recursion)
                        std::cout << "Drats!\n";*/
                    return false;
                ok:;
                    /*if(!recursion)
                        std::cout << "Match for param " << a << " at " << b << std::endl;*/
                }
                // Match = no mismatch.

                // Now feed any possible RestHolders the remaining parameters.
                for(unsigned a=0; a<count; ++a)
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
                std::map<unsigned, double>::iterator
                    i = match.ImmedMap.lower_bound(index);
                if(i != match.ImmedMap.end() && i->first == index)
                    return res == i->second;
                match.ImmedMap.insert(i, std::make_pair((unsigned)index, res));
                return true;
            }
            case NamedHolder:
            {
                std::map<unsigned, std::pair<uint_fast64_t, size_t> >::iterator
                    i = match.NamedMap.lower_bound(index);
                if(i != match.NamedMap.end() && i->first == index)
                {
                    return tree.Hash == i->second.first;
                }
                match.NamedMap.insert(i, std::make_pair(index, std::make_pair(tree.Hash, 1)));
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

    void MatchedParams::SynthesizeTree(
        FPoptimizer_CodeTree::CodeTree& tree,
        const MatchedParams& matcher,
        MatchedParams::CodeTreeMatch& match) const
    {
        for(unsigned a=0; a<count; ++a)
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
                subtree->Sort();
                subtree->Recalculate_Hash_NoRecursion(); // rehash this, but not the children, nor the parent
                FPoptimizer_CodeTree::CodeTree::Param p(subtree, param.sign) ;
                tree.AddParam(p);
            }
        }
    }

    void MatchedParams::ReplaceParams(
        FPoptimizer_CodeTree::CodeTree& tree,
        const MatchedParams& matcher,
        MatchedParams::CodeTreeMatch& match) const
    {
        // Replace the 0-level params indicated in "match" with the ones we have

        // First, construct the tree recursively using the "match" info
        SynthesizeTree(tree, matcher, match);

        // Remove the indicated params
        std::sort(match.param_numbers.begin(), match.param_numbers.end());
        for(size_t a=match.param_numbers.size(); a-->0; )
        {
            size_t num = match.param_numbers[a];
            tree.DelParam(num);
        }
        tree.Sort();
        tree.Rehash(true); // rehash this and its parents, but not its children
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

        tree.Sort();
        tree.Rehash(true);  // rehash this and its parents, but not its children
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

                    std::map<uint_fast64_t, FPoptimizer_CodeTree::CodeTreeP>
                        ::const_iterator j = match.trees.find(hash);

                    assert(j != match.trees.end());

                    FPoptimizer_CodeTree::CodeTree* subtree = j->second->Clone();
                    FPoptimizer_CodeTree::CodeTree::Param p(subtree, sign);
                    tree.AddParam(p);
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
                mitem.SynthesizeTree(tree, matcher, match);
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

                    std::map<uint_fast64_t, FPoptimizer_CodeTree::CodeTreeP>
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

                    tree.SetParams(j->second->Params);
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

#ifdef DEBUG_SUBSTITUTIONS
    void DumpParam(const ParamSpec& p)
    {
        //std::cout << "/*p" << (&p-pack.plist) << "*/";

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
            case cAnd: sep2 = " &"; break;
            case cOr: sep2 = " |"; break;
            default:
                std::cout << FP_GetOpcodeName(tree.Opcode);
                if(tree.Opcode == cFCall || tree.Opcode == cPCall)
                    std::cout << ':' << tree.Funcno;
        }
        std::cout << '(';
        if(tree.Params.size() <= 1 && *sep2) std::cout << (sep2+1) << ' ';
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
            std::cout << "           " << NamedHolderNames[i->first] << " = ";
            DumpTree(*matchrec.trees.find(i->second.first)->second);
            std::cout << " (" << i->second.second << " matches)\n";
        }

        for(std::map<unsigned, double>::const_iterator
            i = matchrec.ImmedMap.begin(); i != matchrec.ImmedMap.end(); ++i)
        {
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
#endif
}
