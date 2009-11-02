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

        for(std::map<unsigned, CodeTree>::const_iterator
            i = info.paramholder_matches.begin();
            i != info.paramholder_matches.end();
            ++i)
        {
            o << "           " << ParamHolderNames[i->first] << " = ";
            DumpTree(i->second, o);
            o << "\n";
        }

        for(std::map<unsigned, std::vector<CodeTree> >::const_iterator
            i = info.restholder_matches.begin();
            i != info.restholder_matches.end();
            ++i)
        {
            for(size_t a=0; a<i->second.size(); ++a)
            {
                o << "         <" << i->first << "> = ";
                DumpTree(i->second[a], o);
                o << std::endl;
            }
        }
        o << std::flush;
    }

}

#endif
