#include "fpoptimizer_codetree.hh"
#include "fpoptimizer_grammar.hh"

#ifdef FP_SUPPORT_OPTIMIZER

#include <map>
#include <vector>
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
        std::map<unsigned, std::vector<CodeTree> > restholder_matches;
        std::map<unsigned, CodeTree> paramholder_matches;
        std::vector<unsigned> matched_params;
    public:
        /* These functions save data from matching */
        bool SaveOrTestRestHolder(
            unsigned restholder_index,
            const std::vector<CodeTree>& treelist)
        {
            std::map<unsigned, std::vector<CodeTree> >::iterator
                i = restholder_matches.lower_bound(restholder_index);
            if(i == restholder_matches.end() || i->first != restholder_index)
                { restholder_matches.insert(i, std::make_pair(restholder_index, treelist) );
                  return true; }
            if(treelist.size() != i->second.size())
                return false;
            for(size_t a=0; a<treelist.size(); ++a)
                if(!treelist[a].IsIdenticalTo(i->second[a]))
                    return false;
            return true;
        }

        void SaveRestHolder(
            unsigned restholder_index,
            std::vector<CodeTree>& treelist)
        {
            restholder_matches[restholder_index].swap(treelist);
        }

        bool SaveOrTestParamHolder(
            unsigned paramholder_index,
            const CodeTree& treeptr)
        {
            std::map<unsigned, CodeTree>::iterator
                i = paramholder_matches.lower_bound(paramholder_index);
            if(i == paramholder_matches.end() || i->first != paramholder_index)
                { paramholder_matches.insert(i, std::make_pair(paramholder_index, treeptr));
                  return true; }
            return treeptr.IsIdenticalTo(i->second);
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
            std::map<unsigned, CodeTree>::const_iterator i = paramholder_matches.find(paramholder_index);
            if(i == paramholder_matches.end() || i->first != paramholder_index)
            {
                static const CodeTree dummytree;
                return dummytree;
            }
            return i->second;
        }

        const CodeTree& GetParamHolderValue( unsigned paramholder_index ) const
            { return paramholder_matches.find(paramholder_index)->second; }

        bool HasRestHolder(unsigned restholder_index) const
            { return restholder_matches.find(restholder_index)
                     != restholder_matches.end(); }

        const std::vector<CodeTree>& GetRestHolderValues( unsigned restholder_index ) const
        {
            static const std::vector<CodeTree> empty_result;
            std::map<unsigned, std::vector<CodeTree> >::const_iterator
                i = restholder_matches.find(restholder_index);
            if(i != restholder_matches.end())
                return i->second;
            return empty_result;
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
