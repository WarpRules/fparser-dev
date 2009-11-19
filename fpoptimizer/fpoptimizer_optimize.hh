#include "fpoptimizer_codetree.hh"
#include "fpoptimizer_grammar.hh"

#ifdef FP_SUPPORT_OPTIMIZER

#include <vector>
#include <utility>
#include <iostream>

//#define DEBUG_SUBSTITUTIONS

namespace FPoptimizer_Optimize
{
    using namespace FPoptimizer_Grammar;
    using namespace FPoptimizer_CodeTree;
    using namespace FUNCTIONPARSERTYPES;

    /* This struct collects information regarding the matching process so far */
    class MatchInfo
    {
    public:
        std::vector<std::pair<bool,std::vector<CodeTree>
                             > > restholder_matches;
        std::vector<CodeTree> paramholder_matches;
        std::vector<unsigned> matched_params;
    public:
        /* These functions save data from matching */
        bool SaveOrTestRestHolder(
            unsigned restholder_index,
            const std::vector<CodeTree>& treelist)
        {
            if(restholder_matches.size() <= restholder_index)
            {
                restholder_matches.resize(restholder_index+1);
                restholder_matches[restholder_index].first  = true;
                restholder_matches[restholder_index].second = treelist;
                return true;
            }
            if(restholder_matches[restholder_index].first == false)
            {
                restholder_matches[restholder_index].first  = true;
                restholder_matches[restholder_index].second = treelist;
                return true;
            }
            const std::vector<CodeTree>& found =
                restholder_matches[restholder_index].second;
            if(treelist.size() != found.size())
                return false;
            for(size_t a=0; a<treelist.size(); ++a)
                if(!treelist[a].IsIdenticalTo(found[a]))
                    return false;
            return true;
        }

        void SaveRestHolder(
            unsigned restholder_index,
            std::vector<CodeTree>& treelist)
        {
            if(restholder_matches.size() <= restholder_index)
                restholder_matches.resize(restholder_index+1);
            restholder_matches[restholder_index].first = true;
            restholder_matches[restholder_index].second.swap(treelist);
        }

        bool SaveOrTestParamHolder(
            unsigned paramholder_index,
            const CodeTree& treeptr)
        {
            if(paramholder_matches.size() <= paramholder_index)
            {
                paramholder_matches.reserve(paramholder_index+1);
                paramholder_matches.resize(paramholder_index);
                paramholder_matches.push_back(treeptr);
                return true;
            }
            if(!paramholder_matches[paramholder_index].IsDefined())
            {
                paramholder_matches[paramholder_index] = treeptr;
                return true;
            }
            return treeptr.IsIdenticalTo(paramholder_matches[paramholder_index]);
        }

        void SaveMatchedParamIndex(unsigned index)
        {
            matched_params.push_back(index);
        }

        /* These functions retrieve the data from matching
         * for use when synthesizing the resulting tree.
         */
        const CodeTree& GetParamHolderValueIfFound( unsigned paramholder_index ) const
        {
            static const CodeTree dummytree;
            if(paramholder_matches.size() <= paramholder_index)
                return dummytree;
            return paramholder_matches[paramholder_index];
        }

        const CodeTree& GetParamHolderValue( unsigned paramholder_index ) const
            { return paramholder_matches[paramholder_index]; }

        bool HasRestHolder(unsigned restholder_index) const
            { return restholder_matches.size() > restholder_index
                  && restholder_matches[restholder_index].first == true; }

        const std::vector<CodeTree>& GetRestHolderValues( unsigned restholder_index ) const
        {
            static const std::vector<CodeTree> empty_result;
            if(restholder_matches.size() <= restholder_index)
                return empty_result;
            return restholder_matches[restholder_index].second;
        }

        const std::vector<unsigned>& GetMatchedParamIndexes() const
            { return matched_params; }

        /* */
        void swap(MatchInfo& b)
        {
            restholder_matches.swap(b.restholder_matches);
            paramholder_matches.swap(b.paramholder_matches);
            matched_params.swap(b.matched_params);
        }
        MatchInfo& operator=(const MatchInfo& b)
        {
            restholder_matches = b.restholder_matches;
            paramholder_matches = b.paramholder_matches;
            matched_params = b.matched_params;
            return *this;
        }
    };

    class MatchPositionSpecBase;

    // For iterating through match candidates
    typedef FPOPT_autoptr<MatchPositionSpecBase> MatchPositionSpecBaseP;

    class MatchPositionSpecBase
    {
    public:
        int RefCount;
    public:
        MatchPositionSpecBase() : RefCount(0) { }
        virtual ~MatchPositionSpecBase() { }
    };
    struct MatchResultType
    {
        bool found;
        MatchPositionSpecBaseP specs;

        MatchResultType(bool f) : found(f), specs() { }
        MatchResultType(bool f,
                        const MatchPositionSpecBaseP& s) : found(f), specs(s) { }
    };

    /* Synthesize the given grammatic rule's replacement into the codetree */
    void SynthesizeRule(
        const Rule& rule,
        CodeTree& tree,
        MatchInfo& info);

    /* Test the given parameter to a given CodeTree */
    MatchResultType TestParam(
        const ParamSpec& parampair,
        const CodeTree& tree,
        const MatchPositionSpecBaseP& start_at,
        MatchInfo& info);

    /* Test the list of parameters to a given CodeTree */
    MatchResultType TestParams(
        const ParamSpec_SubFunctionData& model_tree,
        const CodeTree& tree,
        const MatchPositionSpecBaseP& start_at,
        MatchInfo& info,
        bool TopLevel);

    bool ApplyGrammar(const Grammar& grammar,
                      FPoptimizer_CodeTree::CodeTree& tree,
                      bool from_logical_context = false);
    void ApplyGrammars(FPoptimizer_CodeTree::CodeTree& tree);

    bool IsLogisticallyPlausibleParamsMatch(
        const ParamSpec_SubFunctionData& params,
        const CodeTree& tree);
}

namespace FPoptimizer_Grammar
{
    void DumpMatch(const Rule& rule,
                   const FPoptimizer_CodeTree::CodeTree& tree,
                   const FPoptimizer_Optimize::MatchInfo& info,
                   bool DidMatch,
                   std::ostream& o = std::cout);
    void DumpMatch(const Rule& rule,
                   const FPoptimizer_CodeTree::CodeTree& tree,
                   const FPoptimizer_Optimize::MatchInfo& info,
                   const char* whydump,
                   std::ostream& o = std::cout);
}

#endif
