#include "fpoptimizer_codetree.hh"
#include "fpoptimizer_grammar.hh"

#include <map>
#include <vector>

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
        std::map<unsigned, double>    immedholder_matches;
        std::map<unsigned, CodeTreeP> namedholder_matches;
        std::vector<unsigned> matched_params;
    public:
        /* These functions save data from matching */
        void SaveRestHolderMatch(unsigned restholder_index, const CodeTreeP& treeptr)
        {
            restholder_matches.insert( std::make_pair(restholder_index, treeptr) );
        }

        bool SaveOrTestImmedHolder(unsigned immedholder_index, double value)
        {
            std::map<unsigned, double>::iterator i = immedholder_matches.lower_bound(immedholder_index);
            if(i == immedholder_matches.end() || i->first != immedholder_index)
                { immedholder_matches.insert(i, std::make_pair(immedholder_index, value));
                  return true; }
            return FloatEqual(i->second, value);
        }

        bool SaveOrTestNamedHolder(unsigned namedholder_index, const CodeTreeP& treeptr)
        {
            std::map<unsigned, CodeTreeP>::iterator i = namedholder_matches.lower_bound(namedholder_index);
            if(i == namedholder_matches.end() || i->first != namedholder_index)
                { namedholder_matches.insert(i, std::make_pair(namedholder_index, treeptr));
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
        double    GetImmedHolderValue( unsigned immedholder_index ) const
            { return immedholder_matches.find(immedholder_index)->second; }

        bool GetImmedHolderValueIfFound( unsigned immedholder_index, double& value ) const
        {
            std::map<unsigned, double>::const_iterator i = immedholder_matches.find(immedholder_index);
            if(i == immedholder_matches.end() || i->first != immedholder_index) return false;
            value = i->second;
            return true;
        }

        CodeTreeP GetNamedHolderValue( unsigned namedholder_index ) const
            { return namedholder_matches.find(namedholder_index)->second->Clone(); }

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
            immedholder_matches.swap(b.immedholder_matches);
            namedholder_matches.swap(b.namedholder_matches);
            matched_params.swap(b.matched_params);
        }
        MatchInfo& operator=(const MatchInfo& b)
        {
            restholder_matches = b.restholder_matches;
            immedholder_matches = b.immedholder_matches;
            namedholder_matches = b.namedholder_matches;
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

#ifdef DEBUG_SUBSTITUTIONS
namespace FPoptimizer_Grammar
{
    void DumpTree(const FPoptimizer_CodeTree::CodeTree& tree, std::ostream& o = std::cout);
    void DumpHashes(const FPoptimizer_CodeTree::CodeTree& tree, std::ostream& o = std::cout);

    void DumpParam(const ParamSpec& p);
    void DumpParams(unsigned paramlist, unsigned count);
    void DumpMatch(const Rule& rule,
                   const FPoptimizer_CodeTree::CodeTree& tree,
                   const FPoptimizer_Optimize::MatchInfo& info,
                   bool DidMatch);
}
#endif
