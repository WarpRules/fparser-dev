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

    void DumpParam(const ParamSpec& parampair)
    {
        //std::cout << "/*p" << (&p-pack.plist) << "*/";
        unsigned constraints = 0;
        switch(parampair.first)
        {
            case NumConstant:
              { const ParamSpec_NumConstant& param = *(const ParamSpec_NumConstant*) parampair.second;
                std::cout << param.constvalue; break; }
            case ImmedHolder:
              { const ParamSpec_ImmedHolder& param = *(const ParamSpec_ImmedHolder*) parampair.second;
                std::cout << ImmedHolderNames[param.index];
                constraints = param.constraints;
                break; }
            case NamedHolder:
              { const ParamSpec_NamedHolder& param = *(const ParamSpec_NamedHolder*) parampair.second;
                std::cout << NamedHolderNames[param.index];
                constraints = param.constraints;
                break; }
            case RestHolder:
              { const ParamSpec_RestHolder& param = *(const ParamSpec_RestHolder*) parampair.second;
                std::cout << '<' << param.index << '>';
                break; }
            case SubFunction:
              { const ParamSpec_SubFunction& param = *(const ParamSpec_SubFunction*) parampair.second;
                constraints = param.constraints;
                std::cout << '(' << FP_GetOpcodeName(param.data.subfunc_opcode) << ' ';
                if(param.data.match_type == PositionalParams) std::cout << '[';
                if(param.data.match_type == SelectedParams) std::cout << '{';
                DumpParams(param.data.param_list, param.data.param_count);
                if(param.data.match_type == PositionalParams) std::cout << " ]";
                if(param.data.match_type == SelectedParams) std::cout << " }";
                std::cout << ')';
                break; }
            case GroupFunction:
              { const ParamSpec_GroupFunction& param = *(const ParamSpec_GroupFunction*) parampair.second;
                constraints = param.constraints;
                std::string opcode = FP_GetOpcodeName(param.subfunc_opcode).substr(1);
                for(size_t a=0; a<opcode.size(); ++a) opcode[a] = std::toupper(opcode[a]);
                std::cout << opcode << '(';
                DumpParams(param.param_list, param.param_count);
                std::cout << " )"; }
        }
        switch( ImmedConstraint_Value(constraints & ValueMask) )
        {
            case ValueMask: break;
            case Value_AnyNum: break;
            case Value_EvenInt:   std::cout << "@E"; break;
            case Value_OddInt:    std::cout << "@O"; break;
            case Value_IsInteger: std::cout << "@I"; break;
            case Value_NonInteger:std::cout << "@F"; break;
        }
        switch( ImmedConstraint_Sign(constraints & SignMask) )
        {
            case SignMask: break;
            case Sign_AnySign: break;
            case Sign_Positive:   std::cout << "@P"; break;
            case Sign_Negative:   std::cout << "@N"; break;
        }
        switch( ImmedConstraint_Oneness(constraints & OnenessMask) )
        {
            case OnenessMask: break;
            case Oneness_Any: break;
            case Oneness_One:     std::cout << "@1"; break;
            case Oneness_NotOne:  std::cout << "@M"; break;
        }
    }
    void DumpParams(unsigned paramlist, unsigned count)
    {
        for(unsigned a=0; a<count; ++a)
        {
            if(a > 0) std::cout << ' ';
            const ParamSpec& param = ParamSpec_Extract(paramlist,a);
            DumpParam(param, o);
            unsigned depcode = ParamSpec_GetDepCode(param);
            if(depcode != 0)
                o << "@D" << depcode;
        }
    }
    void DumpMatch(const Rule& rule,
                   const CodeTree& tree,
                   const MatchInfo& info,
                   bool DidMatch)
    {
        std::cout <<
            "Found " << (DidMatch ? "match" : "mismatch") << ":\n"
            "  Pattern    : ";
        { ParamSpec tmp;
          tmp.first = SubFunction;
          ParamSpec_SubFunction tmp2;
          tmp2.data = rule.match_tree;
          tmp.second = (const void*) &tmp2;
          DumpParam(tmp);
        }
        std::cout << "\n"
            "  Replacement: ";
        DumpParams(rule.repl_param_list, rule.repl_param_count);
        std::cout << "\n";

        std::cout <<
            "  Tree       : ";
        DumpTree(tree);
        std::cout << "\n";
        if(DidMatch) DumpHashes(tree);

        for(std::map<unsigned, CodeTreeP>::const_iterator
            i = info.namedholder_matches.begin();
            i != info.namedholder_matches.end();
            ++i)
        {
            std::cout << "           " << NamedHolderNames[i->first] << " = ";
            DumpTree(*i->second);
            std::cout << "\n";
        }

        for(std::map<unsigned, double>::const_iterator
            i = info.immedholder_matches.begin();
            i != info.immedholder_matches.end();
            ++i)
        {
            std::cout << "           " << ImmedHolderNames[i->first] << " = ";
            std::cout << i->second << std::endl;
        }

        for(std::multimap<unsigned, CodeTreeP>::const_iterator
            i = info.restholder_matches.begin();
            i != info.restholder_matches.end();
            ++i)
        {
            std::cout << "         <" << i->first << "> = ";
            DumpTree(*i->second);
            std::cout << std::endl;
        }
        std::cout << std::flush;
    }

    void DumpHashes(const CodeTree& tree,
                    std::map<fphash_t, std::set<std::string> >& done)
    {
        for(size_t a=0; a<tree.Params.size(); ++a)
            DumpHashes(*tree.Params[a], done);

        std::ostringstream buf;
        DumpTree(tree, buf);
        done[tree.Hash].insert(buf.str());
    }
    void DumpHashes(const CodeTree& tree, std::ostream& o)
    {
        std::map<fphash_t, std::set<std::string> > done;
        DumpHashes(tree, done);

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

