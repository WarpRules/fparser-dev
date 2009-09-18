#include "fpoptimizer_codetree.hh"
#include "fpoptimizer_grammar.hh"

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
        std::multimap<unsigned, CodeTreeP> restholder_matches;
        std::map<unsigned, CodeTreeP> paramholder_matches;
        std::vector<unsigned> matched_params;
    public:
        /* These functions save data from matching */
        void SaveRestHolderMatch(unsigned restholder_index, const CodeTreeP& treeptr)
        {
            restholder_matches.insert( std::make_pair(restholder_index, treeptr) );
        }

        bool SaveOrTestParamHolder(unsigned paramholder_index, const CodeTreeP& treeptr)
        {
            std::map<unsigned, CodeTreeP>::iterator i = paramholder_matches.lower_bound(paramholder_index);
            if(i == paramholder_matches.end() || i->first != paramholder_index)
                { paramholder_matches.insert(i, std::make_pair(paramholder_index, treeptr));
                  return true; }
            return treeptr->IsIdenticalTo(*i->second);
        }

        void SaveMatchedParamIndex(unsigned index)
        {
            matched_params.push_back(index);
        }

        /* These functions retrieve the data from matching
         * for use when synthesizing the resulting tree.
         * It is expected that when synthesizing codetrees,
         * it will use Clone() from the original tree.
         */
        CodeTreeP GetParamHolderValueIfFound( unsigned paramholder_index ) const
        {
            std::map<unsigned, CodeTreeP>::const_iterator i = paramholder_matches.find(paramholder_index);
            if(i == paramholder_matches.end() || i->first != paramholder_index)
                return 0;
            return i->second;
        }

        CodeTreeP GetParamHolderValue( unsigned paramholder_index ) const
            { return paramholder_matches.find(paramholder_index)->second->Clone(); }

        std::vector<CodeTreeP> GetRestHolderValues( unsigned restholder_index ) const
        {
            std::vector<CodeTreeP> result;
            for(std::multimap<unsigned, CodeTreeP>::const_iterator
                i = restholder_matches.lower_bound(restholder_index);
                i != restholder_matches.end() && i->first == restholder_index;
                ++i)
                result.push_back(i->second->Clone());
            return result;
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

    /* Synthesize the given grammatic parameter into the codetree */
    void SynthesizeParam(
        const ParamSpec& parampair,
        CodeTree& tree,
        MatchInfo& info);

    void SynthesizeRule(
        const Rule& rule,
        CodeTree& tree,
        MatchInfo& info);

    /* Test the given parameter to a given CodeTree */
    MatchResultType TestParam(
        const ParamSpec& parampair,
        CodeTreeP& tree,
        const MatchPositionSpecBaseP& start_at,
        MatchInfo& info);

    /* Test the list of parameters to a given CodeTree */
    MatchResultType TestParams(
        const ParamSpec_SubFunctionData& model_tree,
        CodeTree& tree,
        const MatchPositionSpecBaseP& start_at,
        MatchInfo& info,
        bool TopLevel);
}

namespace FPoptimizer_Grammar
{
    void DumpTree(const FPoptimizer_CodeTree::CodeTree& tree, std::ostream& o = std::cout);
    void DumpHashes(const FPoptimizer_CodeTree::CodeTree& tree, std::ostream& o = std::cout);
    void DumpMatch(const Rule& rule,
                   const FPoptimizer_CodeTree::CodeTree& tree,
                   const FPoptimizer_Optimize::MatchInfo& info,
                   bool DidMatch,
                   std::ostream& o = std::cout);
}
