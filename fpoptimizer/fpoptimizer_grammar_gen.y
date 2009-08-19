%{
#define YYDEBUG 1
#define YYERROR_VERBOSE 1
#define YY_FPoptimizerGrammarParser_ERROR_VERBOSE 1
#include <string.h> // for error reporting

#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#include "fpoptimizer_grammar.hh"
#include "fpoptimizer_consts.hh"
#include "fpoptimizer_opcodename.hh"

#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>
#include <assert.h>

#include "crc32.hh"

/*********/
using namespace FPoptimizer_Grammar;
using namespace FUNCTIONPARSERTYPES;

class GrammarDumper;

namespace GrammarData
{
    class ParamSpec;

    class MatchedParams
    {
    public:
        ParamMatchingType Type;
        SignBalanceType   Balance;
        std::vector<ParamSpec*> Params;

    public:
        MatchedParams()                    : Type(PositionalParams), Balance(BalanceDontCare), Params() { }
        MatchedParams(ParamMatchingType t) : Type(t),                Balance(BalanceDontCare), Params() { }
        MatchedParams(ParamSpec* p)        : Type(PositionalParams), Balance(BalanceDontCare), Params() { Params.push_back(p); }

        MatchedParams* SetType(ParamMatchingType t) { Type=t; return this; }
        MatchedParams* SetBalance(SignBalanceType b) { Balance=b; return this; }
        MatchedParams* AddParam(ParamSpec* p) { Params.push_back(p); return this; }

        const std::vector<ParamSpec*>& GetParams() const { return Params; }

        void RecursivelySetParamMatchingType(ParamMatchingType t);
        bool EnsureNoInversions();
        bool EnsureNoVariableCoverageParams_InPositionalParamLists();
/*
        bool EnsureNoRepeatedRestHolders();
        bool EnsureNoRepeatedRestHolders(std::set<unsigned>& used);
*/

        bool operator== (const MatchedParams& b) const;
        bool operator< (const MatchedParams& b) const;

        size_t CalcRequiredParamsCount() const;
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

        void RecursivelySetParamMatchingType(ParamMatchingType t)
        {
            Params.Type = t;
            Params.RecursivelySetParamMatchingType(t);
        }
    };

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
            FunctionType* Func;             // for SubFunction
        };
        ConstraintType     Constraint;
        std::vector<ParamSpec*> Params;

    public:
        struct NamedHolderTag{};
        struct ImmedHolderTag{};
        struct RestHolderTag{};

        ParamSpec(FunctionType* f)
            : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
              Opcode(SubFunction), Func(f),          Constraint(AnyValue), Params()
              {
              }

        ParamSpec(double d)
            : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
              Opcode(NumConstant), ConstantValue(d), Constraint(AnyValue), Params() { }

        ParamSpec(OpcodeType o, const std::vector<ParamSpec*>& p)
            : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
              Opcode(o),                             Constraint(AnyValue), Params(p) { }

        ParamSpec(unsigned i, NamedHolderTag)
            : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
              Opcode(NamedHolder), Index(i),         Constraint(AnyValue), Params() { }

        ParamSpec(unsigned i, ImmedHolderTag)
            : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
              Opcode(ImmedHolder), Index(i),         Constraint(AnyValue), Params() { }

        ParamSpec(unsigned i, RestHolderTag)
            : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
              Opcode(RestHolder),  Index(i),         Constraint(AnyValue), Params() { }

        ParamSpec* SetNegated()                      { Negated=true; return this; }
        ParamSpec* SetRepeat(unsigned min, bool any) { MinimumRepeat=min; AnyRepetition=any; return this; }
        ParamSpec* SetTransformation(TransformationType t)
            { Transformation = t; return this; }
        ParamSpec* SetConstraint(ConstraintType c)
            { Constraint = c; return this; }

        void RecursivelySetParamMatchingType(ParamMatchingType t)
        {
            for(size_t a=0; a<Params.size(); ++a)
                Params[a]->RecursivelySetParamMatchingType(t);
            if(Opcode == SubFunction)
                Func->RecursivelySetParamMatchingType(t);
        }
        bool VerifyIsConstant()
        {
            switch(SpecialOpcode(Opcode))
            {
                case NumConstant: return true;
                case ImmedHolder: return true;
                case NamedHolder: return AnyRepetition; // x+ is constant, x is not
                case RestHolder: return false; // <1> is not constant
                case SubFunction: return false; // subfunctions are not constant
            }
            // For GroupFunctions, all params must be const.
            for(size_t a=0; a<Params.size(); ++a)
                if(!Params[a]->VerifyIsConstant()) return false;
            return true;
        }

        bool operator== (const ParamSpec& b) const;
        bool operator< (const ParamSpec& b) const;

    private:
        ParamSpec(const ParamSpec&);
        ParamSpec& operator= (const ParamSpec&);
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

    void MatchedParams::RecursivelySetParamMatchingType(ParamMatchingType t)
    {
        for(size_t a=0; a<Params.size(); ++a)
            Params[a]->RecursivelySetParamMatchingType(t);
    }

    bool MatchedParams::EnsureNoInversions()
    {
        for(size_t a=0; a<Params.size(); ++a)
            if(Params[a]->Negated)
                return false;
        return true;
    }

    bool MatchedParams::EnsureNoVariableCoverageParams_InPositionalParamLists()
    {
        if(Type != PositionalParams
        && Type != SelectedParams) return true;

        for(size_t a=0; a<Params.size(); ++a)
        {
            if(Params[a]->Opcode == RestHolder)
                return false;
            if(Params[a]->MinimumRepeat != 1
            || Params[a]->AnyRepetition)
                return false;

            if(Params[a]->Opcode == SubFunction)
                if(!Params[a]->Func->Params.EnsureNoVariableCoverageParams_InPositionalParamLists())
                    return false;
        }
        return true;
    }
/*
    bool MatchedParams::EnsureNoRepeatedRestHolders()
    {
        std::set<unsigned> Used_RestHolders;
        return EnsureNoRepeatedRestHolders(Used_RestHolders);
    }

    bool MatchedParams::EnsureNoRepeatedRestHolders(std::set<unsigned>& used)
    {
        for(size_t a=0; a<Params.size(); ++a)
        {
            switch(SpecialOpcode(Params[a]->Opcode))
            {
                case RestHolder:
                    if(used.find(Params[a]->Index) != used.end()) return false;
                    used.insert(Params[a]->Index);
                    break;
                case NumConstant:
                case ImmedHolder:
                case NamedHolder:
                    break;
                case SubFunction:
                    if(!Params[a]->Func->Params.EnsureNoRepeatedRestHolders(used))
                        return false;
                    break;
                default: // GroupFunction:
                    break;
            }
        }
        return true;
    }
*/
    size_t MatchedParams::CalcRequiredParamsCount() const
    {
        size_t res = 0;
        for(size_t a=0; a<Params.size(); ++a)
        {
            if(Params[a]->Opcode == RestHolder)
                continue; // Completely optional
            res += Params[a]->MinimumRepeat;
        }
        return res;
    }

    bool ParamSpec::operator== (const ParamSpec& b) const
    {
        if(Negated != b.Negated) return false;
        if(Transformation != b.Transformation) return false;
        if(MinimumRepeat != b.MinimumRepeat) return false;
        if(AnyRepetition != b.AnyRepetition) return false;
        if(Constraint != b.Constraint) return false;
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
        if(Constraint != b.Constraint) return Constraint < b.Constraint;
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
        size_t a_req =   CalcRequiredParamsCount();
        size_t b_req = b.CalcRequiredParamsCount();
        if(a_req != b_req) return false;

        if(Type != b.Type) return false;

        if(Params.size() != b.Params.size()) return false;
        for(size_t a=0; a<Params.size(); ++a)
            if(!(*Params[a] == *b.Params[a]))
                return false;
        return true;
    }

    bool MatchedParams::operator< (const MatchedParams& b) const
    {
        size_t a_req =   CalcRequiredParamsCount();
        size_t b_req = b.CalcRequiredParamsCount();
        if(a_req != b_req) return a_req < b_req;

        if(Type !=  b.Type) return Type < b.Type;

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

    std::string Dump(unsigned o)
    {
        return FP_GetOpcodeName(o, true);
    }
    std::string PDumpFix(const GrammarData::ParamSpec& p, const std::string& s)
    {
        std::string res = s;
        if(p.Negated)
            res = "(" + res + ")->SetNegated()";
        if(p.MinimumRepeat != 1 || p.AnyRepetition)
        {
            std::ostringstream tmp;
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
        std::vector<crc32_t> crc32list;
        std::vector<ParamSpec> pitems;
        pitems.reserve(params.size());
        for(size_t a=0; a<params.size(); ++a)
        {
            pitems.push_back(Dump(*params[a]));
        }
        count = params.size();
        index = plist.size();
        plist.reserve(plist.size() + pitems.size());
        for(size_t a=0; a<pitems.size(); ++a)
        {
            size_t pos = plist.size();
            plist.push_back(pitems[a]);
            crc32list.push_back(crc32::calc(
                (const unsigned char*)&plist[pos],
                                sizeof(plist[pos])));
        }
      #if 1
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
      #endif
        return;
    }

    ParamSpec Dump(const GrammarData::ParamSpec& p)
    {
        ParamSpec  pitem;
        pitem.sign           = p.Negated;
        pitem.transformation = p.Transformation;
        pitem.constraint     = p.Constraint;
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
        return pitem;
    }
    size_t Dump(const GrammarData::MatchedParams& m)
    {
        MatchedParams mitem;
        mitem.type    = m.Type;
        mitem.balance = m.Balance;
        size_t i, c;
        Dump(m.Params, i, c);
        mitem.index = i;
        mitem.count = c;
      #if 1
        crc32_t crc = crc32::calc((const unsigned char*)&mitem, sizeof(mitem));
        std::map<crc32_t, size_t>::const_iterator mi = m_index.find(crc);
        if(mi != m_index.end())
            return mi->second;
        m_index[crc] = mlist.size();
      #endif
        mlist.push_back(mitem);
        return mlist.size()-1;
    }
    size_t Dump(const GrammarData::FunctionType& f)
    {
        Function fitem;
        fitem.opcode = f.Opcode;
        fitem.index  = Dump(f.Params);
      #if 1
        crc32_t crc = crc32::calc((const unsigned char*)&fitem, sizeof(fitem));
        std::map<crc32_t, size_t>::const_iterator fi = f_index.find(crc);
        if(fi != f_index.end())
            return fi->second;
        f_index[crc] = flist.size();
      #endif
        flist.push_back(fitem);
        return flist.size()-1;
    }
    size_t Dump(const GrammarData::Rule& r)
    {
        Rule ritem;
        ritem.type        = r.Type;
        ritem.func.opcode = r.Input.Opcode;
        ritem.func.index  = Dump(r.Input.Params);
        ritem.repl_index  = Dump(r.Replacement);
        ritem.n_minimum_params = r.Input.Params.CalcRequiredParamsCount();
        rlist.push_back(ritem);
        return rlist.size()-1;
    }
    size_t Dump(const GrammarData::Grammar& g)
    {
        Grammar gitem;
        gitem.index = rlist.size();
        gitem.count = 0;
        for(size_t a=0; a<g.rules.size(); ++a)
        {
            if(g.rules[a].Input.Opcode == cNop) continue;
            Dump(g.rules[a]);
            ++gitem.count;
        }
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
                std::cout << "FPOPT_NAN_CONST";
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
                        << Dump(OpcodeType(plist[a].opcode))
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
                        << ",\t"
                        << (plist[a].constraint == AnyValue ? "AnyValue"
                         :  plist[a].constraint == Positive ? "Positive"
                         :  plist[a].constraint == Negative ? "Negative"
                         :  plist[a].constraint == Even     ? "Even    "
                         :  plist[a].constraint == NonEven  ? "NonEven "
                         :/*plist[a].constraint == Odd    ?*/ "Odd     "
                           )
                        << " }, /* " << a;
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
                         :  mlist[a].type == SelectedParams   ? "SelectedParams  "
                         :/*mlist[a].type == AnyParams      ?*/ "AnyParams       "
                           )
                        << ", "
                        << (mlist[a].balance == BalanceMoreNeg    ? "BalanceMoreNeg "
                         :  mlist[a].balance == BalanceMorePos    ? "BalanceMorePos "
                         :  mlist[a].balance == BalanceEqual      ? "BalanceEqual   "
                         :/*mlist[a].balance == BalanceDontCare ?*/ "BalanceDontCare"
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
            "        {" << Dump(OpcodeType(flist[a].opcode))
                        << ", " << flist[a].index
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
            "        {" << rlist[a].n_minimum_params
                        << ", "
                        << (rlist[a].type == ProduceNewTree  ? "ProduceNewTree"
                         :/*rlist[a].type == ReplaceParams ?*/ "ReplaceParams "
                           )
                        << ",    " << rlist[a].repl_index
                        << ",\t{ " << Dump(OpcodeType(rlist[a].func.opcode))
                        <<   ", " << rlist[a].func.index
                        <<  " } }, /* " << a << " */\n";
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
private:
  /*
    void DumpParam(const ParamSpec& p)
    {
        //std::cout << "/""*p" << (&p-plist) << "*""/";

        static const char ImmedHolderNames[2][2] = {"%","&"};
        static const char NamedHolderNames[6][2] = {"x","y","z","a","b","c"};

        if(p.sign) std::cout << '~';
        if(p.transformation == Negate) std::cout << '-';
        if(p.transformation == Invert) std::cout << '/';

        switch(SpecialOpcode(p.opcode))
        {
            case NumConstant: std::cout << clist[p.index]; break;
            case ImmedHolder: std::cout << ImmedHolderNames[p.index]; break;
            case NamedHolder: std::cout << NamedHolderNames[p.index]; break;
            case RestHolder: std::cout << '<' << p.index << '>'; break;
            case SubFunction: DumpFunction(flist[p.index]); break;
            default:
            {
                std::string opcode = FP_GetOpcodeName(p.opcode).substr(1);
                for(size_t a=0; a<opcode.size(); ++a) opcode[a] = std::toupper(opcode[a]);
                std::cout << opcode << '(';
                for(unsigned a=0; a<p.count; ++a)
                {
                    if(a > 0) std::cout << ' ';
                    DumpParam(plist[p.index+a]);
                }
                std::cout << " )";
            }
        }
        if(p.anyrepeat && p.minrepeat==1) std::cout << '*';
        if(p.anyrepeat && p.minrepeat==2) std::cout << '+';
    }

    void DumpParams(const MatchedParams& mitem)
    {
        //std::cout << "/""*m" << (&mitem-mlist) << "*""/";

        if(mitem.type == PositionalParams) std::cout << '[';

        for(unsigned a=0; a<mitem.count; ++a)
        {
            std::cout << ' ';
            DumpParam(plist[mitem.index + a]);
        }

        if(mitem.type == PositionalParams) std::cout << " ]";
    }

    void DumpFunction(const Function& fitem)
    {
        //std::cout << "/""*f" << (&fitem-flist) << "*""/";

        std::cout << '(' << FP_GetOpcodeName(fitem.opcode);
        DumpParams(mlist[fitem.index]);
        std::cout << ')';
    }
  */
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

    double             num;
    std::string*       name;
    unsigned           index;
    OpcodeType         opcode;
    TransformationType transform;
}

%token <num>       NUMERIC_CONSTANT
%token <name>      PARAMETER_TOKEN
%token <name>      POSITIVE_PARAM_TOKEN
%token <name>      NEGATIVE_PARAM_TOKEN
%token <name>      EVEN_PARAM_TOKEN
%token <name>      NONEVEN_PARAM_TOKEN
%token <name>      ODD_PARAM_TOKEN
%token <index>     PLACEHOLDER_TOKEN
%token <index>     IMMED_TOKEN
%token <opcode>    BUILTIN_FUNC_NAME
%token <opcode>    OPCODE
%token <transform> UNARY_TRANSFORMATION
%token NEWLINE

%token SUBST_OP_COLON
%token SUBST_OP_ARROW
%token BALANCE_POS
%token BALANCE_EQUAL
%token BALANCE_NEG

%type <r> substitution
%type <f> function function_match
%type <p> paramlist paramlist_loop
%type <a> param
%type <a> expression_param

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
      function_match SUBST_OP_ARROW param NEWLINE
      /* Entire function is changed into the particular param */
      {
        $3->RecursivelySetParamMatchingType(PositionalParams);

        $$ = new GrammarData::Rule(ProduceNewTree, *$1, $3);
        delete $1;
      }

    | function_match SUBST_OP_ARROW function NEWLINE
      /* Entire function changes, the param_notinv_list is rewritten */
      /* NOTE: "p x -> o y"  is a shortcut for "p x -> (o y)"  */
      {
        $3->RecursivelySetParamMatchingType(PositionalParams);

        $$ = new GrammarData::Rule(ProduceNewTree, *$1, new GrammarData::ParamSpec($3));

        //std::cout << GrammarDumper().Dump(*new GrammarData::ParamSpec($3)) << "\n";
        delete $1;
      }

    | function_match SUBST_OP_COLON  paramlist NEWLINE
      /* The params provided are replaced with the new param_maybeinv_list */
      {
        $3->RecursivelySetParamMatchingType(PositionalParams);

        if($1->Opcode != cAdd && $1->Opcode != cMul && $1->Opcode != cAnd && $1->Opcode != cOr)
        {
            /* If function opcode is "notinv", verify that $23 has no inversions */
            if(!$3->EnsureNoInversions())
            {
                yyerror("Can have no inversions"); YYERROR;
            }
        }

        $$ = new GrammarData::Rule(ReplaceParams, *$1, *$3);
        delete $1;
        delete $3;
      }
    ;

    function_match:
       function
       {
           if(!$1->Params.EnsureNoVariableCoverageParams_InPositionalParamLists())
           {
               yyerror("Variable coverage parameters, such as x* or <1>, must not occur in bracketed param lists on the matching side"); YYERROR;
           }
           $$ = $1;
       }
    ;

    function:
       OPCODE '[' paramlist ']'
       /* Match a function with opcode=opcode,
        * and the exact parameter list as specified
        */
       {
         if($1 != cAdd && $1 != cMul && $1 != cAnd && $1 != cOr)
         {
             /* If function opcode is "notinv", verify that $3 has no inversions */
             if(!$3->EnsureNoInversions())
             {
                 yyerror("Can have no inversions"); YYERROR;
             }
         }
/*
         if(!$3->EnsureNoRepeatedRestHolders())
         {
             yyerror("RestHolders such as <1> must not be repeated in a rule; make matching too difficult"); YYERROR;
         }
*/
         $$ = new GrammarData::FunctionType($1, *$3);
         delete $3;
       }
    |  OPCODE '{' paramlist '}'
       /* Match a function with opcode=opcode,
        * and the exact parameter list in any order
        */
       {
         if($1 != cAdd && $1 != cMul && $1 != cAnd && $1 != cOr)
         {
             /* If function opcode is "notinv", verify that $3 has no inversions */
             if(!$3->EnsureNoInversions())
             {
                 yyerror("Can have no inversions"); YYERROR;
             }
         }
/*
         if(!$3->EnsureNoRepeatedRestHolders())
         {
             yyerror("RestHolders such as <1> must not be repeated in a rule; make matching too difficult"); YYERROR;
         }
*/
         $$ = new GrammarData::FunctionType($1, *$3->SetType(SelectedParams));
         delete $3;
       }
    |  OPCODE paramlist
       /* Match a function with opcode=opcode and the given way of matching params */
       /* There may be more parameters, don't care about them */
       {
         if($1 != cAdd && $1 != cMul && $1 != cAnd && $1 != cOr)
         {
             /* If function opcode is "notinv", verify that $2 has no inversions */
             if(!$2->EnsureNoInversions())
             {
                 yyerror("Can have no inversions"); YYERROR;
             }
         }
/*
         if(!$2->EnsureNoRepeatedRestHolders())
         {
             yyerror("RestHolders such as <1> must not be repeated in a rule; make matching too difficult"); YYERROR;
         }
*/
         $$ = new GrammarData::FunctionType($1, *$2->SetType(AnyParams));
         delete $2;
       }
    ;

    paramlist:
        paramlist_loop
      | paramlist_loop BALANCE_POS   { $$ = $1->SetBalance(BalanceMorePos); }
      | paramlist_loop BALANCE_NEG   { $$ = $1->SetBalance(BalanceMoreNeg); }
      | paramlist_loop BALANCE_EQUAL { $$ = $1->SetBalance(BalanceEqual); }
    ;

    paramlist_loop: /* left-recursive list of 0-n params with no delimiter */
        paramlist_loop '~' param
        {
          $$ = $1->AddParam($3->SetNegated());
        }
      | paramlist_loop param
        {
          $$ = $1->AddParam($2);
        }
      | /* empty */
        {
          $$ = new GrammarData::MatchedParams;
        }
    ;

    param:
       NUMERIC_CONSTANT         /* particular immed */
       {
         $$ = new GrammarData::ParamSpec($1);
       }
    |  IMMED_TOKEN              /* a placeholder for some immed */
       {
         $$ = new GrammarData::ParamSpec($1, GrammarData::ParamSpec::ImmedHolderTag());
       }
    |  BUILTIN_FUNC_NAME '(' paramlist_loop ')'  /* literal logarithm/sin/etc. of the provided immed-type params -- also sum/product/minimum/maximum */
       {
         /* Verify that $3 contains no inversions */
         if(!$3->EnsureNoInversions())
         {
             yyerror("Can have no inversions"); YYERROR;
         }
         /* Verify that $3 consists of constants */
         $$ = new GrammarData::ParamSpec($1, $3->GetParams());
         if(!$$->VerifyIsConstant())
         {
             yyerror("Not constant"); YYERROR;
         }
         delete $3;
       }
    |  UNARY_TRANSFORMATION param   /* the negated/inverted literal value of the param */
       {
         /* Verify that $2 is constant */
         if(!$2->VerifyIsConstant())
         {
             yyerror("Not constant"); YYERROR;
         }
         $$ = $2->SetTransformation($1);
       }
    |  expression_param         /* any expression, indicated by "x", "a" etc. */
       {
         $$ = $1;
       }
    |  expression_param '+'       /* any expression, indicated by "x", "a" etc. */
       {
         /* In matching, matches TWO or more identical repetitions of namedparam */
         /* In substitution, yields an immed containing the number of repetitions */
         $$ = $1->SetRepeat(2, true);
       }
    |  expression_param '*'       /* any expression, indicated by "x", "a" etc. */
       {
         /* In matching, matches TWO or more identical repetitions of namedparam */
         /* In substitution, yields an immed containing the number of repetitions */
         $$ = $1->SetRepeat(1, true);
       }
    |  '(' function ')'         /* a subtree */
       {
         $$ = new GrammarData::ParamSpec($2);
       }
    |  PLACEHOLDER_TOKEN        /* a placeholder for all params */
       {
         $$ = new GrammarData::ParamSpec($1, GrammarData::ParamSpec::RestHolderTag());
       }
    ;

    expression_param:
       PARAMETER_TOKEN          /* any expression, indicated by "x", "a" etc. */
       {
         unsigned nameindex = dumper.Dump(*$1);
         $$ = new GrammarData::ParamSpec(nameindex, GrammarData::ParamSpec::NamedHolderTag());
         delete $1;
       }
     | POSITIVE_PARAM_TOKEN
       {
         unsigned nameindex = dumper.Dump(*$1);
         $$ = (new GrammarData::ParamSpec(nameindex, GrammarData::ParamSpec::NamedHolderTag()))
                ->SetConstraint(Positive);
         delete $1;
       }
     | NEGATIVE_PARAM_TOKEN
       {
         unsigned nameindex = dumper.Dump(*$1);
         $$ = (new GrammarData::ParamSpec(nameindex, GrammarData::ParamSpec::NamedHolderTag()))
                ->SetConstraint(Negative);
         delete $1;
       }
     | EVEN_PARAM_TOKEN
       {
         unsigned nameindex = dumper.Dump(*$1);
         $$ = (new GrammarData::ParamSpec(nameindex, GrammarData::ParamSpec::NamedHolderTag()))
                ->SetConstraint(Even);
         delete $1;
       }
     | NONEVEN_PARAM_TOKEN
       {
         unsigned nameindex = dumper.Dump(*$1);
         $$ = (new GrammarData::ParamSpec(nameindex, GrammarData::ParamSpec::NamedHolderTag()))
                ->SetConstraint(NonEven);
         delete $1;
       }
     | ODD_PARAM_TOKEN
       {
         unsigned nameindex = dumper.Dump(*$1);
         $$ = (new GrammarData::ParamSpec(nameindex, GrammarData::ParamSpec::NamedHolderTag()))
                ->SetConstraint(Odd);
         delete $1;
       }
    ;
%%

#ifndef FP_SUPPORT_OPTIMIZER
enum { cVar,cDup,cInv,cFetch,cPopNMov,cSqr,cRDiv,cRSub,cNotNot,cRSqrt };
#endif

void FPoptimizerGrammarParser::yyerror(char* msg)
{
    fprintf(stderr, "%s\n", msg);
    for(;;)
    {
        int c = std::fgetc(stdin);
        if(c == EOF) break;
        std::fputc(c, stderr);
    }
    exit(1);
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
        case '-':
        {
            int c2 = std::fgetc(stdin);
            if(c2 == '>') return SUBST_OP_ARROW;
            std::ungetc(c2, stdin);
            if(c2 >= '0' && c2 <= '9')
            {
                goto GotNumeric;
            }
            lval->transform = Negate;
            return UNARY_TRANSFORMATION;
        }
        case '/':
            lval->transform = Invert;
            return UNARY_TRANSFORMATION;

        case '=':
        {
            int c2 = std::fgetc(stdin);
            if(c2 == '-') return BALANCE_NEG;
            if(c2 == '+') return BALANCE_POS;
            if(c2 == '=') return BALANCE_EQUAL;
            std::ungetc(c2, stdin);
            return '=';
        }
        case '~':
        case '[': case '{':
        case ']': case '}':
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
        GotNumeric:;
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

            /* This code figures out if this is a named constant,
               an opcode, or a parse-time function name,
               or just an identifier
             */

            /* Detect named constants */
            if(IdBuf == "CONSTANT_E") { lval->num = CONSTANT_E; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_RD") { lval->num = CONSTANT_RD; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_DR") { lval->num = CONSTANT_DR; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_PI") { lval->num = CONSTANT_PI; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_PIHALF") { lval->num = CONSTANT_PIHALF; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_L2I") { lval->num = CONSTANT_L2I; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_L10I") { lval->num = CONSTANT_L10I; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_L2") { lval->num = CONSTANT_L2; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_L10") { lval->num = CONSTANT_L10; return NUMERIC_CONSTANT; }
            if(IdBuf == "NaN") { lval->num = FPOPT_NAN_CONST; return NUMERIC_CONSTANT; }

            /* Detect opcodes */
            if(IdBuf == "cAdd") { lval->opcode = cAdd; return OPCODE; }
            if(IdBuf == "cAnd") { lval->opcode = cAnd; return OPCODE; }
            if(IdBuf == "cMul") { lval->opcode = cMul; return OPCODE; }
            if(IdBuf == "cOr")  { lval->opcode = cOr; return OPCODE; }

            if(IdBuf == "cNeg") { lval->opcode = cNeg; return OPCODE; }
            if(IdBuf == "cSub") { lval->opcode = cSub; return OPCODE; }
            if(IdBuf == "cDiv") { lval->opcode = cDiv; return OPCODE; }
            if(IdBuf == "cMod") { lval->opcode = cMod; return OPCODE; }
            if(IdBuf == "cEqual") { lval->opcode = cEqual; return OPCODE; }
            if(IdBuf == "cNEqual") { lval->opcode = cNEqual; return OPCODE; }
            if(IdBuf == "cLess") { lval->opcode = cLess; return OPCODE; }
            if(IdBuf == "cLessOrEq") { lval->opcode = cLessOrEq; return OPCODE; }
            if(IdBuf == "cGreater") { lval->opcode = cGreater; return OPCODE; }
            if(IdBuf == "cGreaterOrEq") { lval->opcode = cGreaterOrEq; return OPCODE; }
            if(IdBuf == "cNot") { lval->opcode = cNot; return OPCODE; }
            if(IdBuf == "cNotNot") { lval->opcode = cNotNot; return OPCODE; }
            if(IdBuf == "cDeg")  { lval->opcode = cDeg; return OPCODE; }
            if(IdBuf == "cRad")  { lval->opcode = cRad; return OPCODE; }
            if(IdBuf == "cInv")  { lval->opcode = cInv; return OPCODE; }
            if(IdBuf == "cSqr")  { lval->opcode = cSqr; return OPCODE; }
            if(IdBuf == "cRDiv") { lval->opcode = cRDiv; return OPCODE; }
            if(IdBuf == "cRSub") { lval->opcode = cRSub; return OPCODE; }
            if(IdBuf == "cRSqrt") { lval->opcode = cRSqrt; return OPCODE; }

            /* Detect other function opcodes */
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
                    return OPCODE;
                }
                fprintf(stderr,
                    "Warning: Unrecognized opcode '%s' interpreted as cNop\n",
                        IdBuf.c_str());
                lval->opcode = cNop;
                return OPCODE;
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

            if(IdBuf == "p" || IdBuf == "q")
                return POSITIVE_PARAM_TOKEN;
            if(IdBuf == "m" || IdBuf == "n")
                return NEGATIVE_PARAM_TOKEN;
            if(IdBuf == "e" || IdBuf == "f")
                return EVEN_PARAM_TOKEN;
            if(IdBuf == "g" || IdBuf == "h")
                return NONEVEN_PARAM_TOKEN;
            if(IdBuf == "o" || IdBuf == "r")
                return ODD_PARAM_TOKEN;
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
