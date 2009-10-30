#include "fpoptimizer_grammar.hh"
#include "fpoptimizer_opcodename.hh"
#include "fpoptimizer_optimize.hh"

#ifdef DEBUG_SUBSTITUTIONS

#include <sstream>

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
        static const char ParamHolderNames[][2] = {"%","&","x","y","z","a","b","c"};

        o <<
            "Found " << (DidMatch ? "match" : "mismatch")
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
        if(DidMatch) DumpHashes(tree, o);

        for(std::map<unsigned, CodeTree>::const_iterator
            i = info.paramholder_matches.begin();
            i != info.paramholder_matches.end();
            ++i)
        {
            o << "           " << ParamHolderNames[i->first] << " = ";
            DumpTree(i->second, o);
            o << "\n";
        }

        for(std::multimap<unsigned, CodeTree>::const_iterator
            i = info.restholder_matches.begin();
            i != info.restholder_matches.end();
            ++i)
        {
            o << "         <" << i->first << "> = ";
            DumpTree(i->second, o);
            o << std::endl;
        }
        o << std::flush;
    }

}

#endif
