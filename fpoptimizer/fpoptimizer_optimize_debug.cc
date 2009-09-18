#include "fpoptimizer_grammar.hh"
#include "fpoptimizer_consts.hh"
#include "fpoptimizer_opcodename.hh"
#include "fpoptimizer_optimize.hh"

#include <sstream>

#ifdef DEBUG_SUBSTITUTIONS
using namespace FUNCTIONPARSERTYPES;
using namespace FPoptimizer_Grammar;
using namespace FPoptimizer_CodeTree;
using namespace FPoptimizer_Optimize;

namespace FPoptimizer_Grammar
{
    static const char ImmedHolderNames[][2]  = {"%","&"};
    static const char NamedHolderNames[][2] = {"x","y","z","a","b","c","d","e","f","g"};

    void DumpMatch(const Rule& rule,
                   const CodeTree& tree,
                   const MatchInfo& info,
                   bool DidMatch,
                   std::ostream& o)
    {
        o <<
            "Found " << (DidMatch ? "match" : "mismatch") << ":\n"
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

        for(std::map<unsigned, CodeTreeP>::const_iterator
            i = info.paramholder_matches.begin();
            i != info.paramholder_matches.end();
            ++i)
        {
            o << "           " << NamedHolderNames[i->first] << " = ";
            DumpTree(*i->second, o);
            o << "\n";
        }

        for(std::multimap<unsigned, CodeTreeP>::const_iterator
            i = info.restholder_matches.begin();
            i != info.restholder_matches.end();
            ++i)
        {
            o << "         <" << i->first << "> = ";
            DumpTree(*i->second, o);
            o << std::endl;
        }
        o << std::flush;
    }

    void DumpHashes(const CodeTree& tree,
                    std::map<fphash_t, std::set<std::string> >& done,
                    std::ostream& o)
    {
        for(size_t a=0; a<tree.Params.size(); ++a)
            DumpHashes(*tree.Params[a], done, o);

        std::ostringstream buf;
        DumpTree(tree, buf);
        done[tree.Hash].insert(buf.str());
    }
    void DumpHashes(const CodeTree& tree, std::ostream& o)
    {
        std::map<fphash_t, std::set<std::string> > done;
        DumpHashes(tree, done, o);

        for(std::map<fphash_t, std::set<std::string> >::const_iterator
            i = done.begin();
            i != done.end();
            ++i)
        {
            const std::set<std::string>& flist = i->second;
            if(flist.size() != 1) o << "ERROR - HASH COLLISION?\n";
            for(std::set<std::string>::const_iterator
                j = flist.begin();
                j != flist.end();
                ++j)
            {
                o << '[' << std::hex << i->first.hash1
                              << ',' << i->first.hash2
                              << ']' << std::dec;
                o << ": " << *j << "\n";
            }
        }
    }
    void DumpTree(const CodeTree& tree, std::ostream& o)
    {
        //o << "/*" << tree.Depth << "*/";
        const char* sep2 = "";
        /*
        o << '[' << std::hex << tree.Hash.hash1
                      << ',' << tree.Hash.hash2
                      << ']' << std::dec;
        */
        switch(tree.Opcode)
        {
            case cImmed: o << tree.Value; return;
            case cVar:   o << "Var" << (tree.Var - VarBegin); return;
            case cAdd: sep2 = " +"; break;
            case cMul: sep2 = " *"; break;
            case cAnd: sep2 = " &"; break;
            case cOr: sep2 = " |"; break;
            case cPow: sep2 = " ^"; break;
            default:
                o << FP_GetOpcodeName(tree.Opcode);
                if(tree.Opcode == cFCall || tree.Opcode == cPCall)
                    o << ':' << tree.Funcno;
        }
        o << '(';
        if(tree.Params.size() <= 1 && *sep2) o << (sep2+1) << ' ';
        for(size_t a=0; a<tree.Params.size(); ++a)
        {
            if(a > 0) o << ' ';

            DumpTree(*tree.Params[a], o);

            if(tree.Params[a]->Parent != &tree)
            {
                o << "(?parent?)";
            }

            if(a+1 < tree.Params.size()) o << sep2;
        }
        o << ')';
    }
}
#endif

