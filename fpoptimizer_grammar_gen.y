%{
#define YYDEBUG 1

#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#include "fpoptimizer_grammar.hh"
#include "fpoptimizer_consts.hh"

#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <map>
#include <algorithm>

/* crc32 */
#include <stdint.h>
typedef uint_least32_t crc32_t;
namespace crc32 {
    enum { startvalue = 0xFFFFFFFFUL };

    /* This code constructs the CRC32 table at compile-time,
     * avoiding the need for a huge explicitly written table of magical numbers. */
    static const uint_least32_t poly = 0xEDB88320UL;
    template<uint_fast32_t crc> // One byte of a CRC32 (eight bits):
    struct b8
    {
        enum { b1 = (crc & 1) ? (poly ^ (crc >> 1)) : (crc >> 1),
               b2 = (b1  & 1) ? (poly ^ (b1  >> 1)) : (b1  >> 1),
               b3 = (b2  & 1) ? (poly ^ (b2  >> 1)) : (b2  >> 1),
               b4 = (b3  & 1) ? (poly ^ (b3  >> 1)) : (b3  >> 1),
               b5 = (b4  & 1) ? (poly ^ (b4  >> 1)) : (b4  >> 1),
               b6 = (b5  & 1) ? (poly ^ (b5  >> 1)) : (b5  >> 1),
               b7 = (b6  & 1) ? (poly ^ (b6  >> 1)) : (b6  >> 1),
               res= (b7  & 1) ? (poly ^ (b7  >> 1)) : (b7  >> 1) };
    };
    // Four values of the table
    #define B4(n) b8<n>::res,b8<n+1>::res,b8<n+2>::res,b8<n+3>::res
    // Sixteen values of the table
    #define R(n) B4(n),B4(n+4),B4(n+8),B4(n+12)
    // The whole table, index by steps of 16
    static const uint_least32_t table[256] =
    { R(0x00),R(0x10),R(0x20),R(0x30), R(0x40),R(0x50),R(0x60),R(0x70),
      R(0x80),R(0x90),R(0xA0),R(0xB0), R(0xC0),R(0xD0),R(0xE0),R(0xF0) }; 
    #undef R
    #undef B4
    uint_fast32_t update(uint_fast32_t crc, unsigned/* char */b) // __attribute__((pure))
    {
        return ((crc >> 8) /* & 0x00FFFFFF*/) ^ table[/*(unsigned char)*/(crc^b)&0xFF];
    }
    crc32_t calc_upd(crc32_t c, const unsigned char* buf, size_t size)
    {
        uint_fast32_t value = c;
        for(unsigned long p=0; p<size; ++p) value = update(value, buf[p]);
        return value;
    }
    crc32_t calc(const unsigned char* buf, size_t size)
    {
        return calc_upd(startvalue, buf, size);
    }
}



/*********/
using namespace FPoptimizer_Grammar;
using namespace FUNCTIONPARSERTYPES;

#define YY_FPoptimizerGrammarParser_MEMBERS \
    Grammar grammar; \
    virtual ~YY_FPoptimizerGrammarParser_CLASS() { }

class GrammarDumper
{
private:
    std::string GenName(const char* prefix)
    {
        static unsigned counter = 0;
        char Buf[512];
        sprintf(Buf, "%s%u", prefix,++counter);
        return Buf;
    }
private:
    std::map<std::string, size_t>    n_index;
    std::map<double,      size_t>    c_index;
    std::map<crc32_t,     size_t>    p_index;
    std::map<crc32_t,     size_t>    m_index;
    std::map<crc32_t,     size_t>    f_index;
    
    std::vector<std::string>         nlist;
    std::vector<double>              clist;
    std::vector<ParamSpec_Const>     plist;
    std::vector<MatchedParams_Const> mlist;
    std::vector<FunctionType_Const>  flist;
    std::vector<RuleType_Const>      rlist;
    std::vector<Grammar_Const>       glist;    
public:
    GrammarDumper():
        n_index(), c_index(), p_index(), m_index(), f_index(),
        nlist(),clist(),plist(),mlist(),flist(),rlist(),glist()
    {
    }
    
    std::string Dump(OpcodeType o)
    {
        switch(OPCODE(o))
        {
            case cAbs: return "cAbs";
            case cAcos: return "cAcos";
#ifndef FP_NO_ASINH
            case cAcosh: return "cAcosh";
#endif
            case cAsin: return "cAsin";
#ifndef FP_NO_ASINH
            case cAsinh: return "cAsinh";
#endif
            case cAtan: return "cAtan";
            case cAtan2: return "cAtan2";
#ifndef FP_NO_ASINH
            case cAtanh: return "cAtanh";
#endif
            case cCeil: return "cCeil";
            case cCos: return "cCos";
            case cCosh: return "cCosh";
            case cCot: return "cCot";
            case cCsc: return "cCsc";
#ifndef FP_DISABLE_EVAL
            case cEval: return "cEval";
#endif
            case cExp: return "cExp";
            case cFloor: return "cFloor";
            case cIf: return "cIf";
            case cInt: return "cInt";
            case cLog: return "cLog";
            case cLog2: return "cLog2";
            case cLog10: return "cLog10";
            case cMax: return "cMax";
            case cMin: return "cMin";
            case cPow: return "cPow";
            case cSec: return "cSec";
            case cSin: return "cSin";
            case cSinh: return "cSinh";
            case cSqrt: return "cSqrt";
            case cTan: return "cTan";
            case cTanh: return "cTanh";
            case cImmed: return "cImmed";
            case cJump: return "cJump";
            case cNeg: return "cNeg";
            case cAdd: return "cAdd";
            case cSub: return "cSub";
            case cMul: return "cMul";
            case cDiv: return "cDiv";
            case cMod: return "cMod";
            case cEqual: return "cEqual";
            case cNEqual: return "cNEqual";
            case cLess: return "cLess";
            case cLessOrEq: return "cLessOrEq";
            case cGreater: return "cGreater";
            case cGreaterOrEq: return "cGreaterOrEq";
            case cNot: return "cNot";
            case cAnd: return "cAnd";
            case cOr: return "cOr";
            case cDeg: return "cDeg";
            case cRad: return "cRad";
            case cFCall: return "cFCall";
            case cPCall: return "cPCall";
#ifdef FP_SUPPORT_OPTIMIZER
            case cVar: return "cVar";
            case cDup: return "cDup";
            case cInv: return "cInv";
            case cFetch: return "cFetch";
            case cPopNMov: return "cPopNMov";
            case cSqr: return "cSqr";
            case cRDiv: return "cRDiv";
            case cRSub: return "cRSub";
            case cNotNot: return "cNotNot";
#endif
            case cNop: return "cNop";
            case VarBegin: return "VarBegin";
        }
        switch( ParamSpec::SpecialOpcode(o) )
        {
            case ParamSpec::NumConstant:   return "ParamSpec::NumConstant  ";
            case ParamSpec::ImmedHolder:   return "ParamSpec::ImmedHolder  ";
            case ParamSpec::NamedHolder:   return "ParamSpec::NamedHolder  ";
            case ParamSpec::RestHolder:    return "ParamSpec::RestHolder   ";
            case ParamSpec::Function:      return "ParamSpec::Function     ";
          //case ParamSpec::GroupFunction: return "ParamSpec::GroupFunction";
        }
        std::stringstream tmp;
        tmp << o;
        return tmp.str();
    }
    std::string PDumpFix(const ParamSpec& p, const std::string& s)
    {
        std::string res = s;
        if(p.Negated)   
            res = "(" + res + ")->SetNegated()";
        if(p.MinimumRepeat != 1 || p.AnyRepetition)
        {
            std::stringstream tmp;
            tmp << "->SetRepeat(" << p.MinimumRepeat
                << ", " << (p.AnyRepetition ? "true" : "false")
                << ")";
            res = "(" + res + ")" + tmp.str();
        }
        return res;
    }
    
    size_t Dump(const std::string& n)
    {
        std::map<std::string, size_t>::const_iterator i = n_index.find(n);
        if(i != n_index.end()) return i->second;
        nlist.push_back(n);
        return n_index[n] = nlist.size()-1;
    }
    size_t Dump(double v)
    {
        std::map<double, size_t>::const_iterator i = c_index.find(v);
        if(i != c_index.end()) return i->second;
        clist.push_back(v);
        return c_index[v] = clist.size()-1;
    }
    
    void Dump(const std::vector<ParamSpec*>& params,
              size_t& index,
              size_t& count)
    {
        count = params.size();
        index = plist.size();
        std::vector<crc32_t> crc32list;
        for(size_t a=0; a<params.size(); ++a)
        {
            size_t pos = Dump(*params[a]);
            crc32list.push_back(crc32::calc(
                (const unsigned char*)&plist[pos],
                                sizeof(plist[pos])));
        }
        size_t candidate_begin = 0;
        bool fail = false;
        for(size_t a=0; a<count; ++a)
        {
            std::map<crc32_t, size_t>::const_iterator ppos = p_index.find(crc32list[a]);
            if(ppos == p_index.end())
            {
                /*if(a > 0 && (candidate_begin + (a-1)) == index-a)
                {
                    // REMOVED: This never seems to happen, so we can't
                    //          test it. Remove it rather than leave in
                    //          potentially buggy code.
                    //
                    // If we were inserting "abc" and the sequence
                    // just happened to be ending as "ab", undo
                    // our inserted "abc" (it would be "ababc"),
                    // and instead, insert the remainder only,
                    // so it becomes "abc".
                    fprintf(stderr, "len=%u, appending at %u\n", (unsigned)count, (unsigned)a);
                    plist.resize(index);
                    index = candidate_begin;
                    for(; a < count; ++a)
                    {
                        size_t pos = Dump(*params[a]);
                        p_index[crc32list[a]] = index + a;
                    }
                    return;
                }*/
                p_index[crc32list[a]] = index + a;
                fail = true;
            }
            else if(a == 0)
                candidate_begin = ppos->second;
            else if(ppos->second != candidate_begin + a)
                fail = true;
        }
        if(!fail)
        {
            plist.resize(index);
            index = candidate_begin;
        }
        return;
    }
    
    size_t Dump(const ParamSpec& p)
    {
        ParamSpec_Const  pitem;
        pitem.negated        = p.Negated;
        pitem.transformation = p.Transformation;
        pitem.minrepeat      = p.MinimumRepeat;
        pitem.anyrepeat      = p.AnyRepetition;
        pitem.opcode         = p.Opcode;
        switch(p.Opcode)
        {
            case ParamSpec::NumConstant:
            {
                pitem.index = Dump(p.ConstantValue);
                pitem.count = 0;
                break;
            }
            case ParamSpec::ImmedHolder:
            {
                pitem.index = p.Index;
                pitem.count = 0;
                break;
            }
            case ParamSpec::NamedHolder:
            {
                pitem.index = Dump(p.Name);
                pitem.count = p.Name.size();
                break;
            }
            case ParamSpec::RestHolder:
            {
                pitem.index = p.Index;
                pitem.count = 0;
                break;
            }
            case ParamSpec::Function:
            {
                pitem.index = Dump(*p.Func);
                pitem.count = 0;
                break;
            }
            default:
            {
                size_t i, c;
                Dump(p.Params, i, c);
                pitem.index = i;
                pitem.count = c;
                break;
            }
        }
        plist.push_back(pitem);
        return plist.size()-1;
    }
    size_t Dump(const MatchedParams& m)
    {
        MatchedParams_Const mitem;
        mitem.type  = m.Type;
        size_t i, c;
        Dump(m.Params, i, c);
        mitem.index = i;
        mitem.count = c;
        
        crc32_t crc = crc32::calc((const unsigned char*)&mitem, sizeof(mitem));
        std::map<crc32_t, size_t>::const_iterator mi = m_index.find(crc);
        if(mi != m_index.end())
            return mi->second;
        m_index[crc] = mlist.size();
        
        mlist.push_back(mitem);
        return mlist.size()-1;
    }
    size_t Dump(const FunctionType& f)
    {
        FunctionType_Const fitem;
        fitem.opcode = f.Opcode;
        fitem.index  = Dump(f.Params);

        crc32_t crc = crc32::calc((const unsigned char*)&fitem, sizeof(fitem));
        std::map<crc32_t, size_t>::const_iterator fi = f_index.find(crc);
        if(fi != f_index.end())
            return fi->second;
        f_index[crc] = flist.size();

        flist.push_back(fitem);
        return flist.size()-1;
    }
    size_t Dump(const Rule& r)
    {
        RuleType_Const ritem;
        ritem.type        = r.Type; 
        ritem.input_index = Dump(r.Input);
        ritem.repl_index  = Dump(r.Replacement);
        rlist.push_back(ritem);
        return rlist.size()-1;
    }
    size_t Dump(const Grammar& g)
    {
        Grammar_Const gitem;
        gitem.index = rlist.size();
        for(size_t a=0; a<g.rules.size(); ++a)
            Dump(g.rules[a]);
        gitem.count = g.rules.size();
        glist.push_back(gitem);
        return glist.size()-1;
    }
    
    void Flush()
    {
        std::cout <<
            "namespace\n"
            "{\n"
            "    const char* const nlist[] =\n"
            "    {\n";
        for(size_t a=0; a<nlist.size(); ++a)
        {
            std::cout <<
            "        \"" << nlist[a] << "\", /* " << a << " */\n";
        }
        std::cout <<
            "    };\n"
            "\n"
            "    const double clist[] =\n"
            "    {\n";
        for(size_t a=0; a<clist.size(); ++a)
        {
            std::cout <<
            "        ";
            std::cout.precision(50);
            if(clist[a]+2-2 != clist[a] || clist[a]+1 == clist[a])
                std::cout << "0.0 / 0.0";
            else
                std::cout << clist[a];
            std::cout << ", /* " << a << " */\n";
        }
        std::cout <<
            "    };\n"
            "\n"
            "    const ParamSpec_Const plist[] =\n"
            "    {\n";
        for(size_t a=0; a<plist.size(); ++a)
        {
            std::cout <<
            "        {" << (plist[a].negated ? "true " : "false")
                        << ", "
                        << (plist[a].transformation == ParamSpec::None    ? "ParamSpec::None  "
                         :  plist[a].transformation == ParamSpec::Negate  ? "ParamSpec::Negate"
                         :/*plist[a].transformation == ParamSpec::Invert?*/ "ParamSpec::Invert"
                           )
                        << ", "
                        << plist[a].minrepeat
                        << ", "
                        << (plist[a].anyrepeat ? "true " : "false")
                        << ", "
                        << Dump(plist[a].opcode)
                        << ", " << plist[a].count
                        << ", " << plist[a].index
                        << " }, /* " << a
                                     << " " << crc32::calc((const unsigned char*)&plist[a], sizeof(plist[a]))
                                     << " */\n";
        }
        std::cout <<
            "    };\n"
            "\n"
            "    const MatchedParams_Const mlist[] =\n"
            "    {\n";
        for(size_t a=0; a<mlist.size(); ++a)
        {
            std::cout <<
            "        {" << (mlist[a].type == MatchedParams::PositionalParams ? "MatchedParams::PositionalParams"
                         :/*mlist[a].type == MatchedParams::AnyParams      ?*/ "MatchedParams::AnyParams       "
                           )
                        << ", " << mlist[a].count
                        << ", " << mlist[a].index
                        << " }, /* " << a << " */\n";
        }
        std::cout <<
            "    };\n"
            "\n"
            "    const FunctionType_Const flist[] =\n"
            "    {\n";
        for(size_t a=0; a<flist.size(); ++a)
        {
            std::cout <<
            "        {" << Dump(flist[a].opcode) << ", " << flist[a].index
                        << " }, /* " << a << " */\n";
        }
        std::cout <<
            "    };\n"
            "\n"
            "    const RuleType_Const rlist[] =\n"
            "    {\n";
        for(size_t a=0; a<rlist.size(); ++a)
        {
            std::cout <<
            "        {" << (rlist[a].type == Rule::ProduceNewTree  ? "Rule::ProduceNewTree"
                         :/*rlist[a].type == Rule::ReplaceParams ?*/ "Rule::ReplaceParams "
                           )
                        << ", " << rlist[a].input_index
                        << ", " << rlist[a].repl_index
                        << " }, /* " << a << " */\n";
        }
        std::cout <<
            "    };\n"
            "\n"
            "    const Grammar_Const glist[] =\n"
            "    {\n";
        for(size_t a=0; a<glist.size(); ++a)
        {
            std::cout <<
            "        {" << glist[a].index << ", " << glist[a].count
                        << " }, /* " << a << " */\n";
        }
        std::cout <<
            "    };\n"
            "\n"
            "    const GrammarPack pack =\n"
            "    {\n"
            "        nlist, clist, plist, mlist, flist, rlist, glist\n"
            "    };\n"
            "}\n";
    }
};

%}

%name FPoptimizerGrammarParser
%pure_parser

%union {
    Rule*          r;
    FunctionType*  f;
    MatchedParams* p;
    ParamSpec*     a;
    
    double         num;
    std::string*   name;
    unsigned       index;
    OpcodeType     opcode;
}

%token <num>    NUMERIC_CONSTANT
%token <name>   PARAMETER_TOKEN
%token <index>  PLACEHOLDER_TOKEN
%token <index>  IMMED_TOKEN
%token <opcode> BUILTIN_FUNC_NAME
%token <opcode> OPCODE_NOTINV
%token <opcode> OPCODE_MAYBEINV
%token <opcode> GROUP_CONSTANT_OPERATOR
%token <opcode> UNARY_CONSTANT_OPERATOR
%token NEWLINE

%token SUBST_OP_COLON
%token SUBST_OP_ARROW

%type <r> substitution
%type <f> function function_notinv function_maybeinv
%type <p> paramsmatchingspec param_maybeinv_list paramlist
%type <a> maybeinv_param param paramtoken

%%
    grammar:
      grammar substitution
      {
        this->grammar.AddRule(*$2);
        delete $2;
      }
    | grammar NEWLINE
    | /* empty */
    ;

    substitution:
      function SUBST_OP_ARROW paramtoken NEWLINE
      /* Entire function is changed into the particular param */
      {
        $$ = new Rule(Rule::ProduceNewTree, *$1, $3);
        delete $1;
      }

    | function SUBST_OP_ARROW function NEWLINE
      /* Entire function changes, the paramlist is rewritten */
      /* NOTE: "p x -> o y"  is a shortcut for "p x -> (o y)"  */
      {
        $$ = new Rule(Rule::ProduceNewTree, *$1, new ParamSpec($3));
        //std::cout << GrammarDumper().Dump(*new ParamSpec($3)) << "\n";
        delete $1;
      }

    | function_maybeinv SUBST_OP_COLON  param_maybeinv_list NEWLINE
      /* The params provided are replaced with the new param_maybeinv_list */
      {
        $$ = new Rule(Rule::ReplaceParams, *$1, *$3);
        delete $1;
        delete $3;
      }

    | function_notinv   SUBST_OP_COLON  paramlist NEWLINE
      /* The params provided are replaced with the new param_maybeinv_list */
      {
        $$ = new Rule(Rule::ReplaceParams, *$1, *$3);
        delete $1;
        delete $3;
      }

    ;
    
    function:
       function_notinv
    |  function_maybeinv
    ;
 
    function_notinv:
       OPCODE_NOTINV paramsmatchingspec
       /* Match a function with opcode=opcode and the given way of matching params */
       {
         $$ = new FunctionType($1, *$2);
         delete $2;
       }
    ;

    function_maybeinv:
       OPCODE_MAYBEINV paramsmatchingspec
       /* Match a function with opcode=opcode and the given way of matching params */
       {
         $$ = new FunctionType($1, *$2);
         delete $2;
       }
    ;

    paramsmatchingspec:
       '[' param_maybeinv_list ']'  /* match this exact paramlist */
        {
          $$ = $2;
          $$->SetType(MatchedParams::PositionalParams);
        }
     |  param_maybeinv_list         /* find the specified params */
        {
          $$ = $1;
          $$->SetType(MatchedParams::AnyParams);
        }
    ;
    
    param_maybeinv_list: /* left-recursive list of 0-n params with no delimiter */
        param_maybeinv_list maybeinv_param
        {
          $$ = $1;
          $$->AddParam($2);
        }
      | /* empty */
        {
          $$ = new MatchedParams;
        }
    ;
    maybeinv_param:
       '~' param    /* negated/inverted param (negations&inversions only exist with cMul and cAdd) */
       {
         $$ = $2;
         $$->Negated = true;
       }
     | param        /* non-negated/non-inverted param */
       {
         $$ = $1;
       }
    ;

    paramlist: /* left-recursive list of 0-n params with no delimiter */
        paramlist param
        {
          $$ = $1;
          $$->AddParam($2);
        }
      | /* empty */
        {
          $$ = new MatchedParams;
        }
    ;

    param:
       paramtoken
    |  paramtoken '+'  /* In matching, matches TWO or more identical repetitions of namedparam */
                       /* In substitution, yields an immed containing the number of repetitions */
       {
         $$ = $1;
         $$->MinimumRepeat = 2;
         $$->AnyRepetition = true;
       }
    |  paramtoken '*'  /* In matching, matches ONE or more identical repetitions of namedparam */
                       /* In substitution, yields an immed containing the number of repetitions */
       {
         $$ = $1;
         $$->MinimumRepeat = 1;
         $$->AnyRepetition = true;
       }
    ;

    paramtoken:
       NUMERIC_CONSTANT         /* particular immed */
       {
         $$ = new ParamSpec($1);
       }
    |  IMMED_TOKEN              /* a placeholder for some immed */
       {
         $$ = new ParamSpec($1, 0.0);
       }
    |  PLACEHOLDER_TOKEN        /* a placeholder for all params */
       {
         $$ = new ParamSpec($1, (void*)0);
       }
    |  '(' function ')'         /* a subtree */
       {
         $$ = new ParamSpec($2);
       }
    |  GROUP_CONSTANT_OPERATOR '(' paramlist ')'    /* the literal sum/product/minimum/maximum of the provided immed-type params */
       {
         $$ = new ParamSpec($1, $3->GetParams());
         delete $3;
       }
    |  UNARY_CONSTANT_OPERATOR paramtoken           /* the negated/inverted literal value of the paramtoken */
       {
         $$ = $2;
         switch($1)
         {
           case cNeg: $$->Transformation = ParamSpec::Negate; break;
           case cInv: $$->Transformation = ParamSpec::Invert; break;
         }
       }
    |  BUILTIN_FUNC_NAME '(' paramlist ')'  /* literal logarithm/sin/etc. of the provided immed-type params */
       {
         $$ = new ParamSpec($1, $3->GetParams());
         delete $3;
       }
    |  PARAMETER_TOKEN          /* any expression */
       {
         $$ = new ParamSpec(*$1);
         delete $1;
       }
    ;

%%

void FPoptimizerGrammarParser::yyerror(char* msg)
{
    fprintf(stderr, "%s\n", msg);
    for(;;)
    {
        int c = std::fgetc(stdin);
        if(c == EOF) break;
        std::fputc(c, stderr);
    }
}

int FPoptimizerGrammarParser::yylex(yy_FPoptimizerGrammarParser_stype* lval)
{
    int c = std::fgetc(stdin);
    switch(c)
    {
        case EOF: break;
        case '#':
            while(c != EOF && c != '\n') c = std::fgetc(stdin);
            return NEWLINE;
        case '\n':
        {
            c = std::fgetc(stdin);
            std::ungetc(c, stdin);
            if(c == '[')
                return EOF;
            return NEWLINE;
        }
        case '+':
        {
            c = std::fgetc(stdin);
            std::ungetc(c, stdin);
            if(c == '(') { lval->opcode = cAdd; return GROUP_CONSTANT_OPERATOR; }
            return '+';
        }
        case '*':
        {
            c = std::fgetc(stdin);
            std::ungetc(c, stdin);
            if(c == '(') { lval->opcode = cMul; return GROUP_CONSTANT_OPERATOR; }
            return '*';
        }

        case '~':
        case '[':
        case ']':
        case '(':
        case ')':
            return c;
        case ' ':
        case '\t':
        case '\v':
        case '\r':
            return yylex(lval); // Counts as tail recursion, I hope
        case ':':
            return SUBST_OP_COLON;
        case '-':
        {
            int c2 = std::fgetc(stdin);
            if(c2 == '>')
                return SUBST_OP_ARROW;
            std::ungetc(c2, stdin);
            lval->opcode = cNeg;
            return UNARY_CONSTANT_OPERATOR;
        }
        case '/':
        {
            lval->opcode = cInv;
            return UNARY_CONSTANT_OPERATOR;
        }
        case '%': { lval->index = 0; return IMMED_TOKEN; }
        case '&': { lval->index = 1; return IMMED_TOKEN; }
        case '<':
        {
            lval->index  = 0;
            for(;;)
            {
                c = std::fgetc(stdin);
                if(c < '0' || c > '9') { std::ungetc(c, stdin); break; }
                lval->index = lval->index * 10 + (c-'0');
            }
            c = std::fgetc(stdin);
            if(c != '>') std::ungetc(c, stdin);
            return PLACEHOLDER_TOKEN;
        }
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        {
            std::string NumBuf;
            NumBuf += (char)c;
            bool had_comma = false;
            for(;;)
            {
                c = std::fgetc(stdin);
                if(c >= '0' && c <= '9')  { NumBuf += (char)c; continue; }
                if(c == '.' && !had_comma){ had_comma = true; NumBuf += (char)c; continue; }
                std::ungetc(c, stdin);
                break;
            }
            lval->num = std::strtod(NumBuf.c_str(), 0);
            return NUMERIC_CONSTANT;
        }
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
        case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
        case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
        case 'Y': case 'Z': case '_':
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
        case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
        case 's': case 't': case 'u': case 'v': case 'w': case 'x':
        case 'y': case 'z':
        {
            std::string IdBuf;
            IdBuf += (char)c;
            for(;;)
            {
                c = std::fgetc(stdin);
                if((c >= '0' && c <= '9')
                || c == '_'
                || (c >= 'a' && c <= 'z')
                || (c >= 'A' && c <= 'Z')) { IdBuf += (char)c; continue; }
                std::ungetc(c, stdin);
                break;
            }
            
            /* TODO: figure out if this is a named constant,
                     an opcode, or a parse-time function name,
                     or just an identifier
             */
            if(IdBuf == "CONSTANT_E") { lval->num = CONSTANT_E; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_RD") { lval->num = CONSTANT_RD; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_DR") { lval->num = CONSTANT_DR; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_PI") { lval->num = CONSTANT_PI; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_L2I") { lval->num = CONSTANT_L2I; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_L10I") { lval->num = CONSTANT_L10I; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_L2") { lval->num = CONSTANT_L2; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_L10") { lval->num = CONSTANT_L10; return NUMERIC_CONSTANT; }
            if(IdBuf == "NaN")
            {
                /* We generate a NaN. Anyone know a better way? */
                lval->num = 0; lval->num /= 0.0; return NUMERIC_CONSTANT;
            }
            
            if(IdBuf == "cAdd") { lval->opcode = cAdd; return OPCODE_MAYBEINV; }
            if(IdBuf == "cAnd") { lval->opcode = cAnd; return OPCODE_MAYBEINV; }
            if(IdBuf == "cMul") { lval->opcode = cMul; return OPCODE_MAYBEINV; }
            if(IdBuf == "cOr")  { lval->opcode = cOr; return OPCODE_MAYBEINV; }
            
            if(IdBuf == "cNeg") { lval->opcode = cNeg; return OPCODE_NOTINV; }
            if(IdBuf == "cSub") { lval->opcode = cSub; return OPCODE_NOTINV; }
            if(IdBuf == "cDiv") { lval->opcode = cDiv; return OPCODE_NOTINV; }
            if(IdBuf == "cMod") { lval->opcode = cMod; return OPCODE_NOTINV; }
            if(IdBuf == "cEqual") { lval->opcode = cEqual; return OPCODE_NOTINV; }
            if(IdBuf == "cNEqual") { lval->opcode = cNEqual; return OPCODE_NOTINV; }
            if(IdBuf == "cLess") { lval->opcode = cLess; return OPCODE_NOTINV; }
            if(IdBuf == "cLessOrEq") { lval->opcode = cLessOrEq; return OPCODE_NOTINV; }
            if(IdBuf == "cGreater") { lval->opcode = cGreater; return OPCODE_NOTINV; }
            if(IdBuf == "cGreaterOrEq") { lval->opcode = cGreaterOrEq; return OPCODE_NOTINV; }
            if(IdBuf == "cNot") { lval->opcode = cNot; return OPCODE_NOTINV; }
            if(IdBuf == "cNotNot") { lval->opcode = cNotNot; return OPCODE_NOTINV; }
            if(IdBuf == "cDeg")  { lval->opcode = cDeg; return OPCODE_NOTINV; }
            if(IdBuf == "cRad")  { lval->opcode = cRad; return OPCODE_NOTINV; }
            if(IdBuf == "cInv")  { lval->opcode = cInv; return OPCODE_NOTINV; }
            if(IdBuf == "cSqr")  { lval->opcode = cSqr; return OPCODE_NOTINV; }
            if(IdBuf == "cRDiv") { lval->opcode = cRDiv; return OPCODE_NOTINV; }
            if(IdBuf == "cRSub") { lval->opcode = cRSub; return OPCODE_NOTINV; }
            
            if(IdBuf[0] == 'c' && std::isupper(IdBuf[1]))
            {
                // This has a chance of being an opcode token
                std::string opcodetoken = IdBuf.substr(1);
                opcodetoken[0] = std::tolower(opcodetoken[0]);
                NamePtr nameptr(opcodetoken.c_str(), opcodetoken.size());
                const FuncDefinition* func = findFunction(nameptr);
                if(func)
                {
                    lval->opcode = func->opcode;
                    return OPCODE_NOTINV;
                }
                fprintf(stderr,
                    "Warning: Unrecognized opcode '%s' interpreted as cNop\n",
                        IdBuf.c_str());
                lval->opcode = cNop;
                return OPCODE_NOTINV;
            }
            // If it is typed entirely in capitals, it has a chance of being
            // a group token
            if(true)
            {
                std::string grouptoken = IdBuf;
                for(size_t a=0; a<grouptoken.size(); ++a)
                {
                    if(std::islower(grouptoken[a])) goto NotAGroupToken;
                    grouptoken[a] = std::tolower(grouptoken[a]);
                }
                if(1) // scope
                {
                    NamePtr nameptr(grouptoken.c_str(), grouptoken.size());
                    const FuncDefinition* func = findFunction(nameptr);
                    if(func)
                    {
                        lval->opcode = func->opcode;
                        return BUILTIN_FUNC_NAME;
                    }
                    if(IdBuf == "MOD")
                    {
                        lval->opcode = cMod;
                        return BUILTIN_FUNC_NAME;
                    }
                    
                    fprintf(stderr, "Warning: Unrecognized opcode '%s' interpreted as cNop\n",
                        IdBuf.c_str());
                    lval->opcode = cNop;
                    return BUILTIN_FUNC_NAME;
                }
            NotAGroupToken:;
            }
            // Anything else is an identifier
            lval->name = new std::string(IdBuf);
            // fprintf(stderr, "'%s' interpreted as PARAM\n", IdBuf.c_str());
            return PARAMETER_TOKEN;
        }
        default:
        {
            fprintf(stderr, "Ignoring unidentifier character '%c'\n", c);
            return yylex(lval); // tail recursion
        }
    }
    return EOF;
}

int main()
{
    Grammar Grammar_Entry;
    Grammar Grammar_Intermediate;
    Grammar Grammar_Final;
    
    std::string sectionname;

    for(;;)
    {
        FPoptimizerGrammarParser x;
        x.yyparse();
        
        std::sort(x.grammar.rules.begin(),
                  x.grammar.rules.end());
        
        if(sectionname == "ENTRY")
            Grammar_Entry = x.grammar;
        else if(sectionname == "INTERMEDIATE")
            Grammar_Intermediate = x.grammar;
        else if(sectionname == "FINAL")
            Grammar_Final = x.grammar;
        else if(!sectionname.empty())
            fprintf(stderr, "Warning: Ignored rules in unknown section '%s'\n",
                sectionname.c_str());
        
        int c = std::fgetc(stdin);
        if(c != '[') break;
        
        sectionname.clear();
        for(;;)
        {
            c = std::fgetc(stdin);
            if(c == ']' || c == EOF) break;
            sectionname += (char)c;
        }
        fprintf(stderr, "Parsing [%s]\n",
            sectionname.c_str());
    }
    
    std::cout <<
        "/* This file is automatically generated. Do not edit... */\n"
        "#include \"fpoptimizer_grammar.hh\"\n"
        "#include \"fpconfig.hh\"\n"
        "#include \"fptypes.hh\"\n"
        "\n"
        "using namespace FPoptimizer_Grammar;\n"
        "using namespace FUNCTIONPARSERTYPES;\n"
        "\n"
        "Grammar Grammar_Entry, Grammar_Intermediate, Grammar_Final;\n"
        "\n";
    
    GrammarDumper dumper;
    size_t e = dumper.Dump(Grammar_Entry);
    size_t i = dumper.Dump(Grammar_Intermediate);
    size_t f = dumper.Dump(Grammar_Final);
    
    dumper.Flush();
    
    std::cout <<
        "\n"
        "void FPoptimizer_Grammar_Init()\n"
        "{\n"
        "    Grammar_Entry.Read(pack,        " << e << ");\n"
        "    Grammar_Intermediate.Read(pack, " << i << ");\n"
        "    Grammar_Final.Read(pack,        " << f << ");\n"
        "}\n";
    
    return 0;
}
