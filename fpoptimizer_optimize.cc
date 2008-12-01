#include "fpoptimizer_grammar.hh"
#include "fpoptimizer_codetree.hh"
#include "fpoptimizer_consts.hh"

#include <algorithm>
#include <cmath>
#include <map>

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

    /* Apply the grammar to a given CodeTree */
    bool Grammar::ApplyTo(
        std::set<uint_fast64_t>& optimized_children,
        FPoptimizer_CodeTree::CodeTree& tree,
        bool child_triggered) const
    {
        bool changed_once = false;

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
                        if( ApplyTo( optimized_children, *tree.Params[a].param ) )
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
            ApplyTo( optimized_children, *tree.Parent, true );
            /* As this step may cause the tree we were passed to actually not exist,
             * don't touch the tree after this.
             *
             * FIXME: Is it even safe at all?
             */
        }

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

        CodeTreeMatch() : param_numbers(), ImmedMap(), NamedMap(), RestMap() { }
    };

    /* Apply the rule to a given CodeTree */
    bool Rule::ApplyTo(
        FPoptimizer_CodeTree::CodeTree& tree) const
    {
        const Function&      input  = pack.flist[input_index];
        const MatchedParams& params = pack.mlist[input.index];
        const MatchedParams& repl   = pack.mlist[repl_index];

        MatchedParams::CodeTreeMatch matchrec;
        if(input.Match(tree, matchrec))
        {
            switch(type)
            {
                case ReplaceParams:
                    repl.ReplaceParams(tree, params, matchrec);
                    return true;
                case ProduceNewTree:
                    repl.ReplaceTree(tree,   params, matchrec);
                    return true;
            }
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
        MatchedParams::CodeTreeMatch& match) const
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
         * FIXME: Repetition and RestHolder are not observed either, yet.
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
                    match.param_numbers.push_back(a);
                }
                // Match = no mismatch.
                return true;
            }
            case AnyParams:
            {
                if(count > n_tree_params) return false;

                std::vector<ParamMatchSnapshot> position(count);
                std::vector<bool>               used(count);

                for(size_t a=0; a<count; ++a)
                {
                    const ParamSpec& param = pack.plist[index+a];

                    position[a].snapshot  = match;
                    position[a].parampos  = 0;
                    position[a].used      = used;
                    size_t b = 0;
                backtrack:
                    for(; b<n_tree_params; ++b)
                    {
                        if(!used[b])
                        {
                            if(param.sign == tree.Params[b].sign
                            && param.Match(*tree.Params[b].param, match))
                            {
                                used[b] = true;
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
                return true;
            }
        }
        return false;
    }

    bool ParamSpec::Match(
        FPoptimizer_CodeTree::CodeTree& tree,
        MatchedParams::CodeTreeMatch& match) const
    {
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
                /* FIXME: Repetitions */
                std::map<unsigned, std::pair<uint_fast64_t, size_t> >::iterator
                    i = match.NamedMap.find(index);
                if(i != match.NamedMap.end())
                {
                    return tree.Hash == i->second.first;
                }
                match.NamedMap[index] = std::make_pair(tree.Hash, 1);
                return true;
            }
            case RestHolder:
            {
                // FIXME
                break;
            }
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
        if(transformation == Negate) result = 1.0 / result;
        return result;
    }

    void MatchedParams::ReplaceParams(
        FPoptimizer_CodeTree::CodeTree& tree,
        const MatchedParams& matcher,
        MatchedParams::CodeTreeMatch& match) const
    {
        // Replace the 0-level params indicated in "match" with the ones we have
    }

    void MatchedParams::ReplaceTree(
        FPoptimizer_CodeTree::CodeTree& tree,
        const MatchedParams& matcher,
        CodeTreeMatch& match) const
    {
        // Replace the entire tree with one indicated by our Params[0]
        // Note: The tree is still constructed using the holders indicated in "match".
    }
}
