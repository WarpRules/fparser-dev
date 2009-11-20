#include "fpoptimizer_grammar.hh"
#include "fpoptimizer_opcodename.hh"
#include "fpoptimizer_optimize.hh"

#ifdef DEBUG_SUBSTITUTIONS

#include <sstream>
#include <cstring>

using namespace FUNCTIONPARSERTYPES;
using namespace FPoptimizer_Grammar;
using namespace FPoptimizer_CodeTree;
using namespace FPoptimizer_Optimize;

namespace FPoptimizer_Grammar
{
    void DumpMatch(const Rule& rule,
                   const CodeTree& tree,
                   const MatchInfo& info,
                   bool DidMatch,
                   std::ostream& o)
    {
        DumpMatch(rule,tree,info,DidMatch?"Found match":"Found mismatch",o);
    }

    void DumpMatch(const Rule& rule,
                   const CodeTree& tree,
                   const MatchInfo& info,
                   const char* whydump,
                   std::ostream& o)
    {
        static const char ParamHolderNames[][2] = {"%","&","x","y","z","a","b","c"};

        o << whydump
          << " (rule " << (&rule - grammar_rules) << ")"
          << ":\n"
            "  Pattern    : ";
        { ParamSpec tmp;
          tmp.first = SubFunction;
          ParamSpec_SubFunction tmp2;
          tmp2.data = rule.match_tree;
          tmp.second = (const void*) &tmp2;
          DumpParam(tmp, o);
        }
        o << "\n"
            "  Replacement: ";
        DumpParams(rule.repl_param_list, rule.repl_param_count, o);
        o << "\n";

        o <<
            "  Tree       : ";
        DumpTree(tree, o);
        o << "\n";
        if(!std::strcmp(whydump,"Found match")) DumpHashes(tree, o);

        for(size_t a=0; a<info.paramholder_matches.size(); ++a)
        {
            if(!info.paramholder_matches[a].IsDefined()) continue;
            o << "           " << ParamHolderNames[a] << " = ";
            DumpTree(info.paramholder_matches[a], o);
            o << "\n";
        }

        for(size_t b=0; b<info.restholder_matches.size(); ++b)
        {
            if(!info.restholder_matches[b].first) continue;
            for(size_t a=0; a<info.restholder_matches[b].second.size(); ++a)
            {
                o << "         <" << b << "> = ";
                DumpTree(info.restholder_matches[b].second[a], o);
                o << std::endl;
            }
        }
        o << std::flush;
    }

}

#endif
