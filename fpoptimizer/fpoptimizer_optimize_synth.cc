#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#ifdef FP_SUPPORT_OPTIMIZER

#include <algorithm>

#include "fpoptimizer_optimize.hh"

namespace FPoptimizer_Optimize
{
    /* Synthesize the given grammatic parameter into the codetree */
    void SynthesizeParam(
        const ParamSpec& parampair,
        CodeTree& tree,
        MatchInfo& info)
    {
        switch( parampair.first )
        {
            case NumConstant:
              { const ParamSpec_NumConstant& param = *(const ParamSpec_NumConstant*) parampair.second;
                tree.AddParam( CodeTree( param.constvalue ) );
                break; }
            case ParamHolder:
              { const ParamSpec_ParamHolder& param = *(const ParamSpec_ParamHolder*) parampair.second;
                tree.AddParam( info.GetParamHolderValue( param.index ) );
                break; }
            case SubFunction:
              { const ParamSpec_SubFunction& param = *(const ParamSpec_SubFunction*) parampair.second;
                CodeTree subtree;
                subtree.BeginChanging();
                subtree.SetOpcode( param.data.subfunc_opcode );
                for(unsigned a=0; a < param.data.param_count; ++a)
                    SynthesizeParam( ParamSpec_Extract(param.data.param_list, a), subtree, info );
                if(param.data.restholder_index != 0)
                {
                    std::vector<CodeTree> trees ( info.GetRestHolderValues( param.data.restholder_index ) );
                    for(size_t a=0; a<trees.size(); ++a)
                        subtree.AddParam( trees[a] );
                }
                subtree.FinishChanging();
                tree.AddParam( subtree );
                break; }
        }
    }

    void SynthesizeRule(
        const Rule& rule,
        CodeTree& tree,
        MatchInfo& info)
    {
        switch(rule.ruletype)
        {
            case ProduceNewTree:
            {
                CodeTree temporary_tree;
                temporary_tree.BeginChanging();
                SynthesizeParam( ParamSpec_Extract(rule.repl_param_list, 0), temporary_tree, info );
                /* SynthesizeParam will add a Param into temporary_tree. */
                tree.Become(temporary_tree.GetParam(0)); // does not need BeginChanging()
                // does not need ConstantFolding; we assume source_tree is already optimized
                break;
            }
            case ReplaceParams:
            {
                /* Delete the matched parameters from the source tree */
                std::vector<unsigned> list = info.GetMatchedParamIndexes();
                std::sort(list.begin(), list.end());

                for(size_t a=list.size(); a-->0; )
                    tree.DelParam( list[a] );

                /* Synthesize the replacement params */
                for(unsigned a=0; a < rule.repl_param_count; ++a)
                    SynthesizeParam( ParamSpec_Extract(rule.repl_param_list, a), tree, info );
                break;
            }
        }
    }
}

#endif
