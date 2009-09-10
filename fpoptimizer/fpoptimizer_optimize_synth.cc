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
                double value ( param.constvalue );
                tree.AddParam( new CodeTree(value) );
                break; }
            case ImmedHolder:
              { const ParamSpec_ImmedHolder& param = *(const ParamSpec_ImmedHolder*) parampair.second;
                double value ( info.GetImmedHolderValue( param.index ) );
                tree.AddParam( new CodeTree(value) );
                break; }
            case NamedHolder:
              { const ParamSpec_NamedHolder& param = *(const ParamSpec_NamedHolder*) parampair.second;
                CodeTreeP paramtree ( info.GetNamedHolderValue( param.index ) );
                tree.AddParam( paramtree );
                break; }
            case RestHolder:
              { const ParamSpec_RestHolder& param = *(const ParamSpec_RestHolder*) parampair.second;
                std::vector<CodeTreeP> trees ( info.GetRestHolderValues( param.index ) );
                for(size_t a=0; a<trees.size(); ++a)
                    tree.AddParam( trees[a] );
                break; }
            case SubFunction:
              { const ParamSpec_SubFunction& param = *(const ParamSpec_SubFunction*) parampair.second;
                CodeTreeP subtree ( new CodeTree );
                subtree->Opcode = param.data.subfunc_opcode;
                for(unsigned a=0; a < param.data.param_count; ++a)
                    SynthesizeParam( ParamSpec_Extract(param.data.param_list, a), *subtree, info );
                subtree->ConstantFolding();
                subtree->Sort();
                subtree->Recalculate_Hash_NoRecursion();
                tree.AddParam( subtree );
                break; }
            case GroupFunction:
              { const ParamSpec_GroupFunction& param = *(const ParamSpec_GroupFunction*) parampair.second;
                // This is expected to produce a cImmed. However, to simplify
                // the code, we don't bother calculating the value here.
                // Instead, we rely on ConstantFolding to do it for us.
                CodeTreeP subtree ( new CodeTree );
                subtree->Opcode = param.subfunc_opcode;
                for(unsigned a=0; a < param.param_count; ++a)
                    SynthesizeParam( ParamSpec_Extract(param.param_list, a), *subtree, info );
                subtree->ConstantFolding();
                subtree->Sort();
                subtree->Recalculate_Hash_NoRecursion();
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
                SynthesizeParam( ParamSpec_Extract(rule.repl_param_list, 0), temporary_tree, info );
                /* SynthesizeParam will add a Param into temporary_tree. */
                const CodeTreeP& source_tree = temporary_tree.Params[0];
                tree.Become(*source_tree, true, false);
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
        tree.ConstantFolding();
        tree.Sort();
        tree.Recalculate_Hash_NoRecursion();
    }
}

#endif
