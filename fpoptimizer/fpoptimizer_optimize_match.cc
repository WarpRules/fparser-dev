#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#ifdef FP_SUPPORT_OPTIMIZER

#include <algorithm>
#include <assert.h>
#include <cmath>

#include <memory> /* for auto_ptr */

#include "fpoptimizer_grammar.hh"
#include "fpoptimizer_optimize.hh"

using namespace FUNCTIONPARSERTYPES;
using namespace FPoptimizer_Grammar;
using namespace FPoptimizer_CodeTree;
using namespace FPoptimizer_Optimize;

namespace
{
    /* Test the given constraints to a given CodeTree */
    bool TestImmedConstraints(unsigned bitmask, CodeTreeP& tree)
    {
        switch(bitmask & ValueMask)
        {
            case Value_AnyNum: case ValueMask: break;
            case Value_EvenInt:
                if(tree->GetEvennessInfo() != CodeTree::IsAlways)
                    return false;
                break;
            case Value_OddInt:
                if(tree->GetEvennessInfo() != CodeTree::IsNever)
                    return false;
                break;
            case Value_IsInteger:
                if(!tree->IsAlwaysInteger()) return false;
                break;
            case Value_NonInteger:
                if(tree->IsAlwaysInteger()) return false;
                break;
        }
        switch(bitmask & SignMask)
        {
            case Sign_AnySign: /*case SignMask:*/ break;
            case Sign_Positive:
                if(!tree->IsAlwaysSigned(true)) return false;
                break;
            case Sign_Negative:
                if(!tree->IsAlwaysSigned(false)) return false;
                break;
            case Sign_NoIdea:
                if(tree->IsAlwaysSigned(true)) return false;
                if(tree->IsAlwaysSigned(false)) return false;
                break;
        }
        switch(bitmask & OnenessMask)
        {
            case Oneness_Any: case OnenessMask: break;
            case Oneness_One:
                if(!tree->IsImmed()) return false;
                if(!FloatEqual(fabs(tree->GetImmed()), 1.0)) return false;
                break;
            case Oneness_NotOne:
                if(!tree->IsImmed()) return false;
                if(FloatEqual(fabs(tree->GetImmed()), 1.0)) return false;
                break;
        }
        return true;
    }

    /* Test the list of parameters to a given CodeTree */
    /* A helper function which simply checks whether the
     * basic shape of the tree matches what we are expecting
     * i.e. given number of numeric constants, etc.
     */
    bool IsLogisticallyPlausibleParamsMatch(
        const ParamSpec_SubFunctionData& params,
        const CodeTree& tree)
    {
        /* First, check if the tree has any chances of matching... */
        /* Figure out what we need. */
        struct Needs
        {
            int SubTrees; // This many subtrees
            int Others;   // This many others (namedholder)
            unsigned SubTreesDetail[VarBegin]; // This many subtrees of each opcode type

            int Immeds;      // This many immeds

            Needs(): SubTrees(0), Others(0), SubTreesDetail(), Immeds() { }
        } NeedList;

        // Figure out what we need
        unsigned minimum_need = 0;
        for(unsigned a = 0; a < params.param_count; ++a)
        {
            const ParamSpec& parampair = ParamSpec_Extract(params.param_list, a);
            switch(parampair.first)
            {
                case SubFunction:
                {
                    const ParamSpec_SubFunction& param = *(const ParamSpec_SubFunction*) parampair.second;
                    NeedList.SubTrees += 1;
                    assert( param.data.subfunc_opcode < VarBegin );
                    NeedList.SubTreesDetail[ param.data.subfunc_opcode ] += 1;
                    ++minimum_need;
                    break;
                }
                case NumConstant:
                case ImmedHolder:
                case GroupFunction:
                    NeedList.Immeds += 1;
                    ++minimum_need;
                    break;
                case NamedHolder:
                    NeedList.Others += 1;
                    ++minimum_need;
                    break;
                case RestHolder:
                    break;
            }
        }
        if(tree.Params.size() < minimum_need)
        {
            // Impossible to satisfy
            return false;
        }

        // Figure out what we have (note: we already assume that the opcode of the tree matches!)
        for(size_t a=0; a<tree.Params.size(); ++a)
        {
            unsigned opcode = tree.Params[a]->Opcode;
            switch(opcode)
            {
                case cImmed:
                    if(NeedList.Immeds > 0) NeedList.Immeds -= 1;
                    else NeedList.Others -= 1;
                    break;
                case cVar:
                case cFCall:
                case cPCall:
                    NeedList.Others -= 1;
                    break;
                default:
                    assert( opcode < VarBegin );
                    if(NeedList.SubTrees > 0
                    && NeedList.SubTreesDetail[opcode] > 0)
                    {
                        NeedList.SubTrees -= 1;
                        NeedList.SubTreesDetail[opcode] -= 1;
                    }
                    else NeedList.Others -= 1;
            }
        }

        // Check whether all needs were satisfied
        if(NeedList.Immeds > 0
        || NeedList.SubTrees > 0
        || NeedList.Others > 0)
        {
            // Something came short, impossible to satisfy.
            return false;
        }

        if(params.match_type != AnyParams)
        {
            if(NeedList.Immeds < 0
            || NeedList.SubTrees < 0
            || NeedList.Others < 0/*
            || params.count != tree.Params.size() - already checked*/)
            {
                // Something was too much.
                return false;
            }
        }
        return true;
    }

    /* Construct CodeTree from a GroupFunction, hopefully evaluating to a constant value */
    CodeTreeP CalculateGroupFunction(
        const ParamSpec& parampair,
        const MatchInfo& info)
    {
        using namespace std;

        switch( parampair.first )
        {
            case NumConstant:
              { const ParamSpec_NumConstant& param = *(const ParamSpec_NumConstant*) parampair.second;
                return new CodeTree( param.constvalue ); }
            case ImmedHolder:
            {
                const ParamSpec_ImmedHolder& param = *(const ParamSpec_ImmedHolder*) parampair.second;
                double value = 0.0;
                if(info.GetImmedHolderValueIfFound( param.index, value ))
                    return new CodeTree(value);
                break; // The immed is not defined
            }
            case NamedHolder:// Note: Never occurs within GroupFunction
            case RestHolder: // Note: Never occurs within GroupFunction
            case SubFunction: // Note: Never occurs within GroupFunction
                break;
            case GroupFunction:
            {
                const ParamSpec_GroupFunction& param = *(const ParamSpec_GroupFunction*) parampair.second;
                /* Synthesize a CodeTree which will take care of
                 * constant-folding our expression. It will also
                 * indicate whether the result is, in fact,
                 * a constant at all. */
                CodeTreeP result = new CodeTree;
                result->Opcode = param.subfunc_opcode;
                for(unsigned a=0; a<param.param_count; ++a)
                    result->AddParam(
                            CalculateGroupFunction(
                                ParamSpec_Extract(param.param_list, a), info)
                                    );
                result->ConstantFolding();
                /* Don't run Sort() or Recalculate_Hash_NoRecursion()
                 * here - these are not used in the execution paths
                 * that involve CalculateGroupFunction().
                 */
                return result;
            }
        }
        // Issue an un-calculatable tree.
        CodeTreeP result = new CodeTree;
        result->Opcode = cVar;
        result->Var    = 999;
        return result;
    }
}

namespace FPoptimizer_Optimize
{
    /* Test the given parameter to a given CodeTree */
    MatchResultType TestParam(
        const ParamSpec& parampair,
        CodeTreeP& tree,
        const MatchPositionSpecBaseP& start_at,
        MatchInfo& info)
    {
        /* What kind of param are we expecting */
        switch( parampair.first )
        {
            case NumConstant: /* A particular numeric value */
            {
                const ParamSpec_NumConstant& param = *(const ParamSpec_NumConstant*) parampair.second;
                if(!tree->IsImmed()) return false;
                return FloatEqual(tree->GetImmed(), param.constvalue);
            }
            case ImmedHolder: /* A constant value */
            {
                const ParamSpec_ImmedHolder& param = *(const ParamSpec_ImmedHolder*) parampair.second;
                if(!tree->IsImmed()) return false;
                if(!TestImmedConstraints(param.constraints, tree)) return false;
                return info.SaveOrTestImmedHolder( param.index, tree->GetImmed() );
            }
            case NamedHolder: /* A subtree */
            {
                const ParamSpec_NamedHolder& param = *(const ParamSpec_NamedHolder*) parampair.second;
                if(!TestImmedConstraints(param.constraints, tree)) return false;
                return info.SaveOrTestNamedHolder(param.index, tree);
            }
            case RestHolder: // Note: Never occurs
                return true;
            case SubFunction: /* A subtree conforming these specs */
            {
                const ParamSpec_SubFunction& param = *(const ParamSpec_SubFunction*) parampair.second;
                if(!&*start_at)
                {
                    if(!TestImmedConstraints(param.constraints, tree)) return false;
                    if(tree->Opcode != param.data.subfunc_opcode) return false;
                }
                return TestParams(param.data, *tree, start_at, info, false);
            }
            case GroupFunction: /* A constant value acquired from this formula */
            {
                const ParamSpec_GroupFunction& param = *(const ParamSpec_GroupFunction*) parampair.second;
                if(!tree->IsImmed()) return false;
                /* Construct the formula */
                CodeTreeP  grammar_func = CalculateGroupFunction(parampair, info);
                /* Evaluate it and compare */
                if(!grammar_func->IsImmed()) return false;
                if(!TestImmedConstraints(param.constraints, tree)) return false;
                return FloatEqual(tree->GetImmed(), grammar_func->GetImmed());
            }
        }
        return false;
    }

    struct PositionalParams_Rec
    {
        MatchPositionSpecBaseP start_at; /* child's start_at */
        MatchInfo              info;     /* backup of "info" at start */

        PositionalParams_Rec(): start_at(), info() { }
    };
    class MatchPositionSpec_PositionalParams
        : public MatchPositionSpecBase,
          public std::vector<PositionalParams_Rec>
    {
    public:
        explicit MatchPositionSpec_PositionalParams(size_t n)
            : MatchPositionSpecBase(),
              std::vector<PositionalParams_Rec> (n)
              { }
    };

    struct AnyWhere_Rec
    {
        MatchPositionSpecBaseP start_at; /* child's start_at */
        AnyWhere_Rec() : start_at() { }
    };
    class MatchPositionSpec_AnyWhere
        : public MatchPositionSpecBase,
          public std::vector<AnyWhere_Rec>
    {
    public:
        unsigned trypos;   /* which param index to try next */

        explicit MatchPositionSpec_AnyWhere(size_t n)
            : MatchPositionSpecBase(),
              std::vector<AnyWhere_Rec> (n),
              trypos(0)
              { }
    };

    MatchResultType TestParam_AnyWhere(
        const ParamSpec& parampair,
        CodeTree& tree,
        const MatchPositionSpecBaseP& start_at,
        MatchInfo&         info,
        std::vector<bool>& used,
        bool TopLevel)
    {
        FPOPT_autoptr<MatchPositionSpec_AnyWhere> position;
        unsigned a;
        if(&*start_at)
        {
            position = (MatchPositionSpec_AnyWhere*) &*start_at;
            a = position->trypos;
            goto retry_anywhere_2;
        }
        else
        {
            position = new MatchPositionSpec_AnyWhere(tree.Params.size());
            a = 0;
        }
        for(; a < tree.Params.size(); ++a)
        {
            if(used[a]) continue;

        retry_anywhere:
          { MatchResultType r = TestParam(
                parampair,
                tree.Params[a],
                (*position)[a].start_at,
                info);

            (*position)[a].start_at = r.specs;
            if(r.found)
            {
                used[a]               = true; // matched
                if(TopLevel) info.SaveMatchedParamIndex(a);

                position->trypos = a; // in case of backtrack, try a again
                return MatchResultType(true, &*position);
            } }
        retry_anywhere_2:
            if(&*(*position)[a].start_at) // is there another try?
            {
                goto retry_anywhere;
            }
            // no, move on
        }
        return false;
    }

    struct AnyParams_Rec
    {
        MatchPositionSpecBaseP start_at; /* child's start_at */
        MatchInfo              info;     /* backup of "info" at start */
        std::vector<bool>      used;     /* which params are remaining */

        explicit AnyParams_Rec(size_t nparams)
            : start_at(), info(), used(nparams) { }
    };
    class MatchPositionSpec_AnyParams
        : public MatchPositionSpecBase,
          public std::vector<AnyParams_Rec>
    {
    public:
        explicit MatchPositionSpec_AnyParams(size_t n, size_t m)
            : MatchPositionSpecBase(),
              std::vector<AnyParams_Rec> (n, AnyParams_Rec(m))
              { }
    };

    /* Test the list of parameters to a given CodeTree */
    MatchResultType TestParams(
        const ParamSpec_SubFunctionData& model_tree,
        CodeTree& tree,
        const MatchPositionSpecBaseP& start_at,
        MatchInfo& info,
        bool TopLevel)
    {
        /* When PositionalParams or SelectedParams, verify that
         * the number of parameters is exactly as expected.
         */
        if(model_tree.match_type != AnyParams)
        {
            if(model_tree.param_count != tree.Params.size())
                return false;
        }

        /* Verify that the tree basically conforms the shape we are expecting */
        /* This test is not necessary; it may just save us some work. */
        if(!IsLogisticallyPlausibleParamsMatch(model_tree, tree))
        {
            return false;
        }

        /* Verify each parameter that they are found in the tree as expected. */
        switch(model_tree.match_type)
        {
            case PositionalParams:
            {
                /* Simple: Test all given parameters in succession. */
                FPOPT_autoptr<MatchPositionSpec_PositionalParams> position;
                unsigned a;
                if(&*start_at)
                {
                    position = (MatchPositionSpec_PositionalParams*) &*start_at;
                    a = model_tree.param_count - 1;
                    goto retry_positionalparams_2;
                }
                else
                {
                    position = new MatchPositionSpec_PositionalParams(model_tree.param_count);
                    a = 0;
                }

                for(; a < model_tree.param_count; ++a)
                {
                    (*position)[a].info = info;
                retry_positionalparams:
                  { MatchResultType r = TestParam(
                        ParamSpec_Extract(model_tree.param_list, a),
                        tree.Params[a],
                        (*position)[a].start_at,
                        info);

                    (*position)[a].start_at = r.specs;
                    if(r.found)
                    {
                        continue;
                  } }
                retry_positionalparams_2:
                    // doesn't match
                    if(&*(*position)[a].start_at) // is there another try?
                    {
                        info = (*position)[a].info;
                        goto retry_positionalparams;
                    }
                    // no, backtrack
                    if(a > 0)
                    {
                        --a;
                        goto retry_positionalparams_2;
                    }
                    // cannot backtrack
                    info = (*position)[0].info;
                    return false;
                }
                if(TopLevel)
                    for(unsigned a = 0; a < model_tree.param_count; ++a)
                        info.SaveMatchedParamIndex(a);
                return MatchResultType(true, &*position);
            }
            case SelectedParams:
                // same as AnyParams, except that model_tree.count==tree.Params.size()
                //                       and that there are no RestHolders
            case AnyParams:
            {
                /* Ensure that all given parameters are found somewhere, in any order */

                ParamSpec parampair;
                FPOPT_autoptr<MatchPositionSpec_AnyParams> position;
                std::vector<bool> used( tree.Params.size() );
                std::vector<unsigned> depcodes( model_tree.param_count );
                std::vector<unsigned> test_order( model_tree.param_count );
                for(unsigned a=0; a<model_tree.param_count; ++a)
                {
                    const ParamSpec parampair = ParamSpec_Extract(model_tree.param_list, a);
                    depcodes[a] = ParamSpec_GetDepCode(parampair);
                }
                { unsigned b=0;
                for(unsigned a=0; a<model_tree.param_count; ++a)
                    if(depcodes[a] != 0)
                        test_order[b++] = a;
                for(unsigned a=0; a<model_tree.param_count; ++a)
                    if(depcodes[a] == 0)
                        test_order[b++] = a;
                }
            anyparams_restart:;
                unsigned a;
                if(&*start_at)
                {
                    position = (MatchPositionSpec_AnyParams*) &*start_at;
                    a = model_tree.param_count - 1;
                    parampair = ParamSpec_Extract(model_tree.param_list, test_order[a]);
                    goto retry_anyparams_2;
                }
                else
                {
                    position = new MatchPositionSpec_AnyParams(model_tree.param_count,
                                                               tree.Params.size());
                    a = 0;
                    (*position)[0].info   = info;
                    (*position)[0].used   = used;
                }
                // Match all but restholders
                for(; a < model_tree.param_count; ++a)
                {
                    parampair = ParamSpec_Extract(model_tree.param_list, test_order[a]);
                    if(parampair.first == RestHolder)
                        continue;

                    if(a > 0) // this test is not necessary, but it saves from doing
                    {         // duplicate work, because [0] was already saved above.
                        (*position)[a].info   = info;
                        (*position)[a].used   = used;
                    }
                retry_anyparams:
                  { MatchResultType r = TestParam_AnyWhere(
                        parampair,
                        tree,
                        (*position)[a].start_at,
                        info,
                        used,
                        TopLevel);
                    (*position)[a].start_at = r.specs;
                    if(r.found)
                    {
                        continue;
                  } }
                retry_anyparams_2:
                    // doesn't match
                    if(&*(*position)[a].start_at) // is there another try?
                    {
                        info = (*position)[a].info;
                        used = (*position)[a].used;
                        goto retry_anyparams;
                    }
                    // no, backtrack
                retry_anyparams_3:
                    if(a > 0)
                    {
                        --a;
                        parampair = ParamSpec_Extract(model_tree.param_list, test_order[a]);
                        if(parampair.first == RestHolder)
                            goto retry_anyparams_3;
                        goto retry_anyparams_2;
                    }
                    // cannot backtrack
                    info = (*position)[0].info;
                    return false;
                }
                // Capture anything remaining in restholders
                for(a = 0; a < model_tree.param_count; ++a)
                {
                    const ParamSpec parampair = ParamSpec_Extract(model_tree.param_list, a);
                    if(parampair.first == RestHolder)
                        for(unsigned b = 0; b < tree.Params.size(); ++b)
                        {
                            if(used[b]) continue; // Ignore subtrees that were already used
                            // Save this tree to this restholder

                            const ParamSpec_RestHolder& param = *(const ParamSpec_RestHolder*) parampair.second;
                            info.SaveRestHolderMatch(param.index,
                                                     tree.Params[b]);
                            used[b] = true;
                            if(TopLevel) info.SaveMatchedParamIndex(b);
                        }
                }
                return MatchResultType(true, &*position);
            }
        }
        return false; // doesn't match
    }
}

#endif
