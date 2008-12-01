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
#include <assert.h>

#include "crc32.hh"

/*********/
using namespace FPoptimizer_Grammar;
using namespace FUNCTIONPARSERTYPES;

class GrammarDumper;

namespace GrammarData
{
    class FunctionType;
    
    class ParamSpec
    {
    public:
        bool Negated;    // true means for: cAdd:-x; cMul:1/x; cAnd/cOr: !x; other: invalid

        TransformationType Transformation;

        unsigned MinimumRepeat; // default 1
        bool AnyRepetition;     // false: max=minimum; true: max=infinite

        OpcodeType Opcode;      // specifies the type of the function
        union
        {
            double ConstantValue;           // for NumConstant
            unsigned Index;                 // for ImmedHolder, RestHolder, NamedHolder
            FunctionType* Func;             // for Function
        };
        std::vector<ParamSpec*> Params;

    public:
        struct NamedHolderTag{};
        struct ImmedHolderTag{};
        struct RestHolderTag{};

        ParamSpec(FunctionType* f)
            : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
              Opcode(SubFunction), Func(f),          Params()
              {
              }

        ParamSpec(double d)
            : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
              Opcode(NumConstant), ConstantValue(d), Params() { }

        ParamSpec(OpcodeType o, const std::vector<ParamSpec*>& p)
            : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
              Opcode(o),                             Params(p) { }

        ParamSpec(unsigned i, NamedHolderTag)
            : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
              Opcode(NamedHolder), Index(i),         Params() { }

        ParamSpec(unsigned i, ImmedHolderTag)
            : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
              Opcode(ImmedHolder), Index(i),         Params() { }

        ParamSpec(unsigned i, RestHolderTag)
            : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
              Opcode(RestHolder),  Index(i),         Params() { }

        ParamSpec* SetNegated()                      { Negated=true; return this; }
        ParamSpec* SetRepeat(unsigned min, bool any) { MinimumRepeat=min; AnyRepetition=any; return this; }
        ParamSpec* SetTransformation(TransformationType t)
            { Transformation = t; return this; }

        bool operator== (const ParamSpec& b) const;
        bool operator< (const ParamSpec& b) const;

    private:
        ParamSpec(const ParamSpec&);
        ParamSpec& operator= (const ParamSpec&);
    };

    class MatchedParams
    {
    public:
        ParamMatchingType Type;
        std::vector<ParamSpec*> Params;

    public:
        MatchedParams()                    : Type(PositionalParams), Params() { }
        MatchedParams(ParamMatchingType t) : Type(t),                Params() { }
        MatchedParams(ParamSpec* p)        : Type(PositionalParams), Params() { Params.push_back(p); }

        MatchedParams* SetType(ParamMatchingType t) { Type=t; return this; }
        MatchedParams* AddParam(ParamSpec* p) { Params.push_back(p); return this; }

        const std::vector<ParamSpec*>& GetParams() const { return Params; }

        bool operator== (const MatchedParams& b) const;
        bool operator< (const MatchedParams& b) const;
    };

    class FunctionType
    {
    public:
        OpcodeType    Opcode;
        MatchedParams Params;
    public:
        FunctionType(OpcodeType o, const MatchedParams& p) : Opcode(o), Params(p) { }

        bool operator== (const FunctionType& b) const
        {
            return Opcode == b.Opcode && Params == b.Params;
        }
        bool operator< (const FunctionType& b) const
        {
            if(Opcode != b.Opcode) return Opcode < b.Opcode;
            return Params < b.Params;
        }
    };

    class Rule
    {
    public:
        friend class GrammarDumper;
        RuleType Type;

        FunctionType  Input;
        MatchedParams Replacement; // length should be 1 if ProduceNewTree is used
    public:
        Rule(RuleType t, const FunctionType& f, const MatchedParams& r)
            : Type(t), Input(f), Replacement(r) { }
        Rule(RuleType t, const FunctionType& f, ParamSpec* p)
            : Type(t), Input(f), Replacement() { Replacement.AddParam(p); }

        bool operator< (const Rule& b) const
        {
            return Input < b.Input;
        }
    };

    class Grammar
    {
    public:
        std::vector<Rule> rules;
    public:
        Grammar(): rules() { }

        void AddRule(const Rule& r) { rules.push_back(r); }
    };
    
    ////////////////////

    bool ParamSpec::operator== (const ParamSpec& b) const
    {
        if(Negated != b.Negated) return false;
        if(Transformation != b.Transformation) return false;
        if(MinimumRepeat != b.MinimumRepeat) return false;
        if(AnyRepetition != b.AnyRepetition) return false;
        if(Opcode != b.Opcode) return false;
        switch(Opcode)
        {
            case NumConstant:
                return ConstantValue == b.ConstantValue;
            case ImmedHolder:
            case RestHolder:
            case NamedHolder:
                return Index == b.Index;
            case SubFunction:
                return *Func == *b.Func;
            default:
                if(Params.size() != b.Params.size()) return false;
                for(size_t a=0; a<Params.size(); ++a)
                    if(!(*Params[a] == *b.Params[a]))
                        return false;
                break;
        }
        return true;
    }

    bool ParamSpec::operator< (const ParamSpec& b) const
    {
        if(Negated != b.Negated) return Negated < b.Negated;
        if(Transformation != b.Transformation) return Transformation < b.Transformation;
        if(MinimumRepeat != b.MinimumRepeat) return MinimumRepeat < b.MinimumRepeat;
        if(AnyRepetition != b.AnyRepetition) return AnyRepetition < b.AnyRepetition;
        if(Opcode != b.Opcode) return Opcode < b.Opcode;
        switch(Opcode)
        {
            case NumConstant:
                return ConstantValue < b.ConstantValue;
            case ImmedHolder:
            case RestHolder:
            case NamedHolder:
                return Index < b.Index;
            case SubFunction:
                return *Func < *b.Func;
            default:
                if(Params.size() != b.Params.size()) return Params.size() > b.Params.size();
                for(size_t a=0; a<Params.size(); ++a)
                    if(!(*Params[a] == *b.Params[a]))
                        return *Params[a] < *b.Params[a];
                break;
        }
        return false;
    }

    bool MatchedParams::operator== (const MatchedParams& b) const
    {
        if(Type != b.Type) return false;
        if(Params.size() != b.Params.size()) return false;
        for(size_t a=0; a<Params.size(); ++a)
            if(!(*Params[a] == *b.Params[a]))
                return false;
        return true;
    }

    bool MatchedParams::operator< (const MatchedParams& b) const
    {
        if(Type !=  b.Type) return Type;
        if(Params.size() != b.Params.size()) return Params.size() > b.Params.size();
        for(size_t a=0; a < Params.size(); ++a)
            if(!(*Params[a] == *b.Params[a]))
                return *Params[a] < *b.Params[a];
        return false;
    }
}

#define YY_FPoptimizerGrammarParser_MEMBERS \
    GrammarData::Grammar grammar;

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

    std::vector<std::string>   nlist;
    std::vector<double>        clist;
    std::vector<ParamSpec>     plist;
    std::vector<MatchedParams> mlist;
    std::vector<Function>      flist;
    std::vector<Rule>          rlist;
    std::vector<Grammar>       glist;
public:
    GrammarDumper():
        n_index(), c_index(), p_index(), m_index(), f_index(),
        nlist(),clist(),plist(),mlist(),flist(),rlist(),glist()
    {
    }

    std::string Dump(OpcodeType o)
    {
#if 1
        /* Symbolic meanings for the opcodes? */
        const char* p = 0;
        switch(OPCODE(o))
        {
            case cAbs: p = "cAbs"; break;
            case cAcos: p = "cAcos"; break;
#ifndef FP_NO_ASINH
            case cAcosh: p = "cAcosh"; break;
#endif
            case cAsin: p = "cAsin"; break;
#ifndef FP_NO_ASINH
            case cAsinh: p = "cAsinh"; break;
#endif
            case cAtan: p = "cAtan"; break;
            case cAtan2: p = "cAtan2"; break;
#ifndef FP_NO_ASINH
            case cAtanh: p = "cAtanh"; break;
#endif
            case cCeil: p = "cCeil"; break;
            case cCos: p = "cCos"; break;
            case cCosh: p = "cCosh"; break;
            case cCot: p = "cCot"; break;
            case cCsc: p = "cCsc"; break;
#ifndef FP_DISABLE_EVAL
            case cEval: p = "cEval"; break;
#endif
            case cExp: p = "cExp"; break;
            case cFloor: p = "cFloor"; break;
            case cIf: p = "cIf"; break;
            case cInt: p = "cInt"; break;
            case cLog: p = "cLog"; break;
            case cLog2: p = "cLog2"; break;
            case cLog10: p = "cLog10"; break;
            case cMax: p = "cMax"; break;
            case cMin: p = "cMin"; break;
            case cPow: p = "cPow"; break;
            case cSec: p = "cSec"; break;
            case cSin: p = "cSin"; break;
            case cSinh: p = "cSinh"; break;
            case cSqrt: p = "cSqrt"; break;
            case cTan: p = "cTan"; break;
            case cTanh: p = "cTanh"; break;
            case cImmed: p = "cImmed"; break;
            case cJump: p = "cJump"; break;
            case cNeg: p = "cNeg"; break;
            case cAdd: p = "cAdd"; break;
            case cSub: p = "cSub"; break;
            case cMul: p = "cMul"; break;
            case cDiv: p = "cDiv"; break;
            case cMod: p = "cMod"; break;
            case cEqual: p = "cEqual"; break;
            case cNEqual: p = "cNEqual"; break;
            case cLess: p = "cLess"; break;
            case cLessOrEq: p = "cLessOrEq"; break;
            case cGreater: p = "cGreater"; break;
            case cGreaterOrEq: p = "cGreaterOrEq"; break;
            case cNot: p = "cNot"; break;
            case cAnd: p = "cAnd"; break;
            case cOr: p = "cOr"; break;
            case cDeg: p = "cDeg"; break;
            case cRad: p = "cRad"; break;
            case cFCall: p = "cFCall"; break;
            case cPCall: p = "cPCall"; break;
#ifdef FP_SUPPORT_OPTIMIZER
            case cVar: p = "cVar"; break;
            case cDup: p = "cDup"; break;
            case cInv: p = "cInv"; break;
            case cFetch: p = "cFetch"; break;
            case cPopNMov: p = "cPopNMov"; break;
            case cSqr: p = "cSqr"; break;
            case cRDiv: p = "cRDiv"; break;
            case cRSub: p = "cRSub"; break;
            case cNotNot: p = "cNotNot"; break;
#endif
            case cNop: p = "cNop"; break;
            case VarBegin: p = "VarBegin"; break;
        }
        switch( SpecialOpcode(o) )
        {
            case NumConstant:   p = "NumConstant"; break;
            case ImmedHolder:   p = "ImmedHolder"; break;
            case NamedHolder:   p = "NamedHolder"; break;
            case RestHolder:    p = "RestHolder"; break;
            case SubFunction:   p = "SubFunction"; break;
          //case GroupFunction: p = "GroupFunction"; break;
        }
        std::stringstream tmp;
        assert(p);
        tmp << p;
        while(tmp.str().size() < 12) tmp << ' ';
        return tmp.str();
#else
        /* Just numeric meanings */
        std::stringstream tmp;
        tmp << o;
        while(tmp.str().size() < 5) tmp << ' ';
        return tmp.str();
#endif
    }
    std::string PDumpFix(const GrammarData::ParamSpec& p, const std::string& s)
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

    void Dump(const std::vector<GrammarData::ParamSpec*>& params,
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

    size_t Dump(const GrammarData::ParamSpec& p)
    {
        ParamSpec  pitem;
        pitem.sign           = p.Negated;
        pitem.transformation = p.Transformation;
        pitem.minrepeat      = p.MinimumRepeat;
        pitem.anyrepeat      = p.AnyRepetition;
        pitem.opcode         = p.Opcode;
        switch(p.Opcode)
        {
            case NumConstant:
            {
                pitem.index = Dump(p.ConstantValue);
                pitem.count = 0;
                break;
            }
            case NamedHolder:
            case ImmedHolder:
            case RestHolder:
            {
                pitem.index = p.Index;
                pitem.count = 0;
                break;
            }
            case SubFunction:
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
    size_t Dump(const GrammarData::MatchedParams& m)
    {
        MatchedParams mitem;
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
    size_t Dump(const GrammarData::FunctionType& f)
    {
        Function fitem;
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
    size_t Dump(const GrammarData::Rule& r)
    {
        Rule ritem;
        ritem.type        = r.Type;
        ritem.input_index = Dump(r.Input);
        ritem.repl_index  = Dump(r.Replacement);
        rlist.push_back(ritem);
        return rlist.size()-1;
    }
    size_t Dump(const GrammarData::Grammar& g)
    {
        Grammar gitem;
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
            "    const ParamSpec plist[] =\n"
            "    {\n";
        for(size_t a=0; a<plist.size(); ++a)
        {
            std::cout <<
            "        {"
                        << Dump(plist[a].opcode)
                        << ", "
                        << (plist[a].sign ? "true " : "false")
                        << ", "
                        << (plist[a].transformation == None    ? "None  "
                         :  plist[a].transformation == Negate  ? "Negate"
                         :/*plist[a].transformation == Invert?*/ "Invert"
                           )
                        << ", "
                        << plist[a].minrepeat
                        << ", "
                        << (plist[a].anyrepeat ? "true " : "false")
                        << ", " << plist[a].count
                        << ",\t" << plist[a].index
                        << "\t}, /* " << a;
            if(plist[a].opcode == NamedHolder)
                std::cout << " \"" << nlist[plist[a].index] << "\"";
            else
                std::cout << "    ";
            std::cout << "\t*/\n";
        }
        std::cout <<
            "    };\n"
            "\n"
            "    const MatchedParams mlist[] =\n"
            "    {\n";
        for(size_t a=0; a<mlist.size(); ++a)
        {
            std::cout <<
            "        {" << (mlist[a].type == PositionalParams ? "PositionalParams"
                         :/*mlist[a].type == AnyParams      ?*/ "AnyParams       "
                           )
                        << ", " << mlist[a].count
                        << ", " << mlist[a].index
                        << " }, /* " << a << " */\n";
        }
        std::cout <<
            "    };\n"
            "\n"
            "    const Function flist[] =\n"
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
            "    const Rule rlist[] =\n"
            "    {\n";
        for(size_t a=0; a<rlist.size(); ++a)
        {
            std::cout <<
            "        {" << (rlist[a].type == ProduceNewTree  ? "ProduceNewTree"
                         :/*rlist[a].type == ReplaceParams ?*/ "ReplaceParams "
                           )
                        << ", " << rlist[a].input_index
                        << ", " << rlist[a].repl_index
                        << " }, /* " << a << " */\n";
        }
        std::cout <<
            "    };\n"
            "}\n"
            "\n"
            "namespace FPoptimizer_Grammar\n"
            "{\n"
            "    const GrammarPack pack =\n"
            "    {\n"
            "        clist, plist, mlist, flist, rlist,\n"
            "        {\n";
        for(size_t a=0; a<glist.size(); ++a)
        {
            std::cout <<
            "            {" << glist[a].index << ", " << glist[a].count
                        << " }, /* " << a << " */\n";
        }
        std::cout <<
            "        }\n"
            "    };\n"
            "}\n";
    }
};

static GrammarDumper dumper;

%}

%name FPoptimizerGrammarParser
%pure_parser

%union {
    GrammarData::Rule*          r;
    GrammarData::FunctionType*  f;
    GrammarData::MatchedParams* p;
    GrammarData::ParamSpec*     a;

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
%token <opcode> UNARY_CONSTANT_NEGATE
%token <opcode> UNARY_CONSTANT_INVERT
%token NEWLINE

%token SUBST_OP_COLON
%token SUBST_OP_ARROW

%type <r> substitution
%type <f> function             function_notinv             function_maybeinv
%type <f> function_fixedparams function_notinv_fixedparams function_maybeinv_fixedparams
%type <p> params_maybeinv_list_maybefixed param_maybeinv_list_nobrackets
%type <p> params_notinv_list_maybefixed   param_notinv_list_nobrackets param_notinv_list_nobrackets_numerable
%type <a> maybeinv_param param numerable_param

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
      function SUBST_OP_ARROW param NEWLINE
      /* Entire function is changed into the particular param */
      {
        $$ = new GrammarData::Rule(ProduceNewTree, *$1, $3);
        delete $1;
      }

    | function SUBST_OP_ARROW function NEWLINE
      /* Entire function changes, the param_notinv_list is rewritten */
      /* NOTE: "p x -> o y"  is a shortcut for "p x -> (o y)"  */
      {
        $$ = new GrammarData::Rule(ProduceNewTree, *$1, new GrammarData::ParamSpec($3));
        //std::cout << GrammarDumper().Dump(*new ParamSpec($3)) << "\n";
        delete $1;
      }

    | function_maybeinv SUBST_OP_COLON  param_maybeinv_list_nobrackets NEWLINE
      /* The params provided are replaced with the new param_maybeinv_list */
      {
        $$ = new GrammarData::Rule(ReplaceParams, *$1, *$3);
        delete $1;
        delete $3;
      }

    | function_notinv   SUBST_OP_COLON  param_notinv_list_nobrackets NEWLINE
      /* The params provided are replaced with the new param_maybeinv_list */
      {
        $$ = new GrammarData::Rule(ReplaceParams, *$1, *$3);
        delete $1;
        delete $3;
      }

    ;

    /**/
    
    function:
       function_notinv
    |  function_maybeinv
    ;
    function_notinv:
       OPCODE_NOTINV params_notinv_list_maybefixed
       /* Match a function with opcode=opcode and the given way of matching params */
       {
         $$ = new GrammarData::FunctionType($1, *$2);
         delete $2;
       }
    ;
    function_maybeinv:
       OPCODE_MAYBEINV params_maybeinv_list_maybefixed
       /* Match a function with opcode=opcode and the given way of matching params */
       {
         $$ = new GrammarData::FunctionType($1, *$2);
         delete $2;
       }
    ;

    /**/
    
    function_fixedparams:
       function_notinv_fixedparams
    |  function_maybeinv_fixedparams
    ;
    function_notinv_fixedparams:
       OPCODE_NOTINV '[' param_notinv_list_nobrackets ']'
       /* Match a function with opcode=opcode and the given way of matching params */
       {
         $$ = new GrammarData::FunctionType($1, *$3);
         delete $3;
       }
    ;
    function_maybeinv_fixedparams:
       OPCODE_MAYBEINV '[' param_maybeinv_list_nobrackets ']'
       /* Match a function with opcode=opcode and the given way of matching params */
       {
         $$ = new GrammarData::FunctionType($1, *$3);
         delete $3;
       }
    ;

    /**/
    
    params_maybeinv_list_maybefixed:
       '[' param_maybeinv_list_nobrackets ']'  /* match this exact param_notinv_list */
        { $$ = $2 }
     |  param_maybeinv_list_nobrackets         /* find the specified params */
        {
          $$ = $1->SetType(AnyParams);
        }
    ;

    params_notinv_list_maybefixed:
       '[' param_notinv_list_nobrackets ']'  /* match this exact param_notinv_list */
        { $$ = $2 }
     |  param_notinv_list_nobrackets         /* find the specified params */
        {
          $$ = $1->SetType(AnyParams);
        }
    ;

    /**/
    
    param_maybeinv_list_nobrackets: /* left-recursive list of 0-n params with no delimiter */
        param_maybeinv_list_nobrackets maybeinv_param
        {
          $$ = $1->AddParam($2);
        }
      | /* empty */
        {
          $$ = new GrammarData::MatchedParams;
        }
    ;
    
    param_notinv_list_nobrackets: /* left-recursive list of 0-n params with no delimiter */
        param_notinv_list_nobrackets param
        {
          $$ = $1->AddParam($2);
        }
      | /* empty */
        {
          $$ = new GrammarData::MatchedParams;
        }
    ;

    param_notinv_list_nobrackets_numerable:
        param_notinv_list_nobrackets numerable_param
        {
          $$ = $1->AddParam($2);
        }
      | /* empty */
        {
          $$ = new GrammarData::MatchedParams;
        }
    ;

    /**/
    
    maybeinv_param:
       '~' param    /* negated/inverted param (negations&inversions only exist with cMul and cAdd) */
       {
         $$ = $2->SetNegated();
       }
     | param        /* non-negated/non-inverted param */
    ;

    /**/
    
    param:
       numerable_param
    |  PLACEHOLDER_TOKEN        /* a placeholder for all params */
       {
         $$ = new GrammarData::ParamSpec($1, GrammarData::ParamSpec::RestHolderTag());
       }
    |  PARAMETER_TOKEN          /* any expression, indicated by "x", "a" etc. */
       {
         unsigned nameindex = dumper.Dump(*$1);
         $$ = new GrammarData::ParamSpec(nameindex, GrammarData::ParamSpec::NamedHolderTag());
         delete $1;
       }
    |  '(' function ')'         /* a subtree */
       {
         $$ = new GrammarData::ParamSpec($2);
       }
    ;
    
    numerable_param:
       NUMERIC_CONSTANT         /* particular immed */
       {
         $$ = new GrammarData::ParamSpec($1);
       }
    |  IMMED_TOKEN              /* a placeholder for some immed */
       {
         $$ = new GrammarData::ParamSpec($1, GrammarData::ParamSpec::ImmedHolderTag());
       }
    |  BUILTIN_FUNC_NAME '(' param_notinv_list_nobrackets_numerable ')'  /* literal logarithm/sin/etc. of the provided immed-type params -- also sum/product/minimum/maximum */
       {
         $$ = new GrammarData::ParamSpec($1, $3->GetParams());
         delete $3;
       }
    |  UNARY_CONSTANT_NEGATE numerable_param   /* the negated literal value of the param */
       {
         $$ = $2->SetTransformation(Negate);
       }
    |  UNARY_CONSTANT_INVERT numerable_param   /* the inverted literal value of the param */
       {
         $$ = $2->SetTransformation(Invert);
       }
    |  PARAMETER_TOKEN '+'       /* any expression, indicated by "x", "a" etc. */
       {
         /* In matching, matches TWO or more identical repetitions of namedparam */
         /* In substitution, yields an immed containing the number of repetitions */
         unsigned nameindex = dumper.Dump(*$1);
         $$ = (new GrammarData::ParamSpec(nameindex, GrammarData::ParamSpec::NamedHolderTag()))
            ->SetRepeat(2, true);
         delete $1;
       }
    |  PARAMETER_TOKEN '*'       /* any expression, indicated by "x", "a" etc. */
       {
         /* In matching, matches TWO or more identical repetitions of namedparam */
         /* In substitution, yields an immed containing the number of repetitions */
         unsigned nameindex = dumper.Dump(*$1);
         $$ = (new GrammarData::ParamSpec(nameindex, GrammarData::ParamSpec::NamedHolderTag()))
            ->SetRepeat(1, true);
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
            if(c == '(') { lval->opcode = cAdd; return BUILTIN_FUNC_NAME; }
            return '+';
        }
        case '*':
        {
            c = std::fgetc(stdin);
            std::ungetc(c, stdin);
            if(c == '(') { lval->opcode = cMul; return BUILTIN_FUNC_NAME; }
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
            return UNARY_CONSTANT_NEGATE;
        }
        case '/':
            return UNARY_CONSTANT_INVERT;
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

                    fprintf(stderr, "Warning: Unrecognized constant function '%s' interpreted as cNop\n",
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
    GrammarData::Grammar Grammar_Entry;
    GrammarData::Grammar Grammar_Intermediate;
    GrammarData::Grammar Grammar_Final;

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
        "\n";

    /*size_t e = */dumper.Dump(Grammar_Entry);
    /*size_t i = */dumper.Dump(Grammar_Intermediate);
    /*size_t f = */dumper.Dump(Grammar_Final);

    dumper.Flush();

    return 0;
}
