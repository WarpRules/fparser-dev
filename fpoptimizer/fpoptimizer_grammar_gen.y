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
        bool EnsureNoRepeatedNamedHolders(std::set<unsigned>& used) const;
        bool EnsureNoRepeatedNamedHolders() const;
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

        bool EnsureNoRepeatedNamedHolders() const
            { return Params.EnsureNoRepeatedNamedHolders(); }
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
        unsigned ImmedConstraint;
        std::vector<ParamSpec*> Params;

    public:
        struct NamedHolderTag{};
        struct ImmedHolderTag{};
        struct RestHolderTag{};

        ParamSpec(FunctionType* f)
            : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
              Opcode(SubFunction), Func(f),          ImmedConstraint(0), Params()
              {
              }

        ParamSpec(double d)
            : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
              Opcode(NumConstant), ConstantValue(d), ImmedConstraint(0), Params() { }

        ParamSpec(OpcodeType o, const std::vector<ParamSpec*>& p)
            : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
              Opcode(o),                             ImmedConstraint(0), Params(p) { }

        ParamSpec(unsigned i, NamedHolderTag)
            : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
              Opcode(NamedHolder), Index(i),         ImmedConstraint(0), Params() { }

        ParamSpec(unsigned i, ImmedHolderTag)
            : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
              Opcode(ImmedHolder), Index(i),         ImmedConstraint(0), Params() { }

        ParamSpec(unsigned i, RestHolderTag)
            : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
              Opcode(RestHolder),  Index(i),         ImmedConstraint(0), Params() { }

        ParamSpec* SetNegated()                      { Negated=true; return this; }
        ParamSpec* SetRepeat(unsigned min, bool any) { MinimumRepeat=min; AnyRepetition=any; return this; }
        ParamSpec* SetTransformation(TransformationType t)
            { Transformation = t; return this; }
        ParamSpec* SetConstraint(unsigned mask)
            { ImmedConstraint |= mask; return this; }

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

        bool EnsureNoRepeatedNamedHolders() const
        {
            MatchedParams tmp;
            tmp.Params = Params;
            return tmp.EnsureNoRepeatedNamedHolders();
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

    bool MatchedParams::EnsureNoRepeatedNamedHolders(std::set<unsigned>& used) const
    {
        for(size_t a=0; a<Params.size(); ++a)
        {
            if(Params[a]->Opcode == NamedHolder
            && (Params[a]->MinimumRepeat == 1 && !Params[a]->AnyRepetition))
            {
                unsigned index = Params[a]->Index;
                std::set<unsigned>::iterator i = used.lower_bound(index);
                if(i != used.end() && *i == index)
                    return false;
                used.insert(i, index);
            }
            if(Params[a]->Opcode == SubFunction)
                if(!Params[a]->Func->Params.EnsureNoRepeatedNamedHolders(used))
                    return false;
        }
        return true;
    }

    bool MatchedParams::EnsureNoRepeatedNamedHolders() const
    {
        std::set<unsigned> used;
        return EnsureNoRepeatedNamedHolders(used);
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
        if(ImmedConstraint != b.ImmedConstraint) return false;
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
        if(ImmedConstraint != b.ImmedConstraint) return ImmedConstraint < b.ImmedConstraint;
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
    std::multimap<crc32_t,size_t>    p_index;
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

    ParamSpec CreateParamSpec(const GrammarData::ParamSpec& p)
    {
        ParamSpec  pitem;
        memset(&pitem, 0, sizeof(pitem));
        pitem.sign           = p.Negated;
        pitem.transformation = p.Transformation;
        pitem.minrepeat      = p.MinimumRepeat;
        pitem.anyrepeat      = p.AnyRepetition;
        pitem.opcode         = p.Opcode;
        size_t count = p.ImmedConstraint; // note: stored in "count"
        size_t index = 0;
        switch(p.Opcode)
        {
            case NumConstant:
            {
                index = DumpConstant(p.ConstantValue);
                break;
            }
            case NamedHolder:
            case ImmedHolder:
            case RestHolder:
            {
                index = p.Index;
                break;
            }
            case SubFunction:
            {
                index = DumpFunction(*p.Func);
                break;
            }
            default:
            {
                std::pair<size_t,size_t> r = DumpParamList(p.Params);
                index = r.first;
                count = r.second;
                break;
            }
        }
        pitem.index = index;
        pitem.count = count;

        /* These assertions catch mis-sized bitfields */
        assert(pitem.sign == p.Negated);
        assert(pitem.transformation == p.Transformation);
        assert(pitem.minrepeat == p.MinimumRepeat);
        assert(pitem.anyrepeat == p.AnyRepetition);
        assert(pitem.opcode == p.Opcode);
        assert(pitem.index == index);
        assert(pitem.count == count);
        return pitem;
    }

    size_t DumpName(const std::string& n)
    {
        std::map<std::string, size_t>::const_iterator i = n_index.find(n);
        if(i != n_index.end()) return i->second;
        nlist.push_back(n);
        return n_index[n] = nlist.size()-1;
    }
    size_t DumpConstant(double v)
    {
        std::map<double, size_t>::const_iterator i = c_index.find(v);
        if(i != c_index.end()) return i->second;
        clist.push_back(v);
        return c_index[v] = clist.size()-1;
    }

    std::pair<size_t/*index*/, size_t/*count*/>
        DumpParamList(const std::vector<GrammarData::ParamSpec*>& params)
    {
        if(params.empty())
        {
            return std::pair<size_t, size_t> (0,0); // "nothing" can be found anywhere!
        }

        const size_t count = params.size();

        std::vector<ParamSpec> pitems;
        pitems.reserve(count);
        for(size_t a=0; a<count; ++a)
            pitems.push_back( CreateParamSpec(*params[a]) );

        const crc32_t first_crc = crc32::calc( (const unsigned char*) &pitems[0], sizeof(pitems[0]) );

        /* Find a position within plist[] where to insert pitems[] */

        const size_t old_plist_size = plist.size();

        size_t decided_position = old_plist_size;
        size_t n_missing        = count;

        std::multimap<crc32_t, size_t>::const_iterator ppos = p_index.lower_bound( first_crc );
        for(; ppos != p_index.end() && ppos->first == first_crc; ++ppos)
        {
            size_t candidate_position = ppos->second;
            size_t n_candidate_items  = count;
            if(candidate_position + count > old_plist_size)
                n_candidate_items = old_plist_size - candidate_position;
            size_t n_missing_here = count - n_candidate_items;

            /* Using std::equal() ensures that we don't get crc collisions. */
            /* However, we cast to (const unsigned char*) because
             * our ParamSpec does not have operator== implemented.
             */
            if(std::equal(
                (const unsigned char*) &pitems[0],
                (const unsigned char*) (&pitems[0]+count),
                (const unsigned char*) &plist[candidate_position]))
            {
                /* Found a match */
                n_missing        = n_missing_here;
                decided_position = candidate_position;
                break;
            }
        }

        /* Insert those items that are missing */
        size_t source_offset = count - n_missing;
        plist.reserve(decided_position + count);
        for(size_t a=0; a<n_missing; ++a)
        {
            const ParamSpec& pitem = pitems[a + source_offset];
            const crc32_t crc = crc32::calc( (const unsigned char*) &pitem, sizeof(pitem) );
            p_index.insert( std::make_pair(crc, plist.size()) );
            plist.push_back(pitem);
        }
        return std::pair<size_t, size_t> (decided_position, count);
    }

    size_t DumpMatchedParams(const GrammarData::MatchedParams& m)
    {
        MatchedParams mitem;
        memset(&mitem, 0, sizeof(mitem));
        mitem.type    = m.Type;
        mitem.balance = m.Balance;
        std::pair<size_t,size_t> r = DumpParamList(m.Params);
        mitem.index = r.first;
        mitem.count = r.second;

        /* These assertions catch mis-sized bitfields */
        assert(mitem.type == m.Type);
        assert(mitem.balance == m.Balance);
        assert(mitem.index == r.first);
        assert(mitem.count == r.second);
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
    Function CreateFunction(const GrammarData::FunctionType& f)
    {
        Function fitem;
        memset(&fitem, 0, sizeof(fitem));
        size_t index = DumpMatchedParams(f.Params);
        fitem.opcode = f.Opcode;
        fitem.index  = index;
        /* These assertions catch mis-sized bitfields */
        assert(fitem.opcode == f.Opcode);
        assert(fitem.index  == index);
        return fitem;
    }
    size_t DumpFunction(const GrammarData::FunctionType& f)
    {
        Function fitem = CreateFunction(f);
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
    size_t DumpRule(const GrammarData::Rule& r)
    {
        Rule ritem;
        ritem.type             = r.Type;
        ritem.func             = CreateFunction(r.Input);
        size_t repl_index = DumpMatchedParams(r.Replacement);
        size_t min_params = r.Input.Params.CalcRequiredParamsCount();
        ritem.repl_index       = repl_index;
        ritem.n_minimum_params = min_params;
        /* These assertions catch mis-sized bitfields */
        assert(ritem.type == r.Type);
        assert(ritem.repl_index == repl_index);
        assert(ritem.n_minimum_params == min_params);
        rlist.push_back(ritem);
        return rlist.size()-1;
    }
    size_t DumpGrammar(const GrammarData::Grammar& g)
    {
        Grammar gitem;
        gitem.index = rlist.size();
        gitem.count = 0;
        for(size_t a=0; a<g.rules.size(); ++a)
        {
            if(g.rules[a].Input.Opcode == cNop) continue;
            DumpRule(g.rules[a]);
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
                        << FP_GetOpcodeName(plist[a].opcode, true)
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
                        << ", ";
            switch(plist[a].opcode)
            {
                case NumConstant:
                case RestHolder:
                case SubFunction:
                default:
                    std::cout << plist[a].count;
                    break;
                case ImmedHolder:
                case NamedHolder:
                {
                    const char* sep = "";
                    static const char s[] = " | ";
                    switch( ImmedConstraint_Value( plist[a].count & ValueMask ) )
                    {
                        case ValueMask: case Value_AnyNum: break;
                        case Value_EvenInt: std::cout << sep << "Value_EvenInt"; sep=s; break;
                        case Value_OddInt: std::cout << sep << "Value_OddInt"; sep=s; break;
                        case Value_IsInteger: std::cout << sep << "Value_IsInteger"; sep=s; break;
                        case Value_NonInteger: std::cout << sep << "Value_NonInteger"; sep=s; break;
                    }
                    switch( ImmedConstraint_Sign( plist[a].count & SignMask ) )
                    {
                        /*case SignMask:*/ case Sign_AnySign: break;
                        case Sign_Positive: std::cout << sep << "Sign_Positive"; sep=s; break;
                        case Sign_Negative: std::cout << sep << "Sign_Negative"; sep=s; break;
                        case Sign_NoIdea:   std::cout << sep << "Sign_NoIdea"; sep=s; break;
                    }
                    switch( ImmedConstraint_Oneness( plist[a].count & OnenessMask ) )
                    {
                        case OnenessMask: case Oneness_Any: break;
                        case Oneness_One: std::cout << sep << "Oneness_One"; sep=s; break;
                        case Oneness_NotOne: std::cout << sep << "Oneness_NotOne"; sep=s; break;
                    }
                    if(!*sep) std::cout << "0";
                    break;
                }
            }
            std::cout   << ",\t" << plist[a].index
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
            "        {" << FP_GetOpcodeName(flist[a].opcode, true)
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
                        << ",\t{ " << FP_GetOpcodeName(rlist[a].func.opcode, true)
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
    /* Note: Because bison's token type is an union or a simple type,
     *       anything that has constructors and destructors must be
     *       carried behind pointers here.
     */
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

/* See documentation about syntax and token meanings in fpoptimizer.dat */
%token <num>       NUMERIC_CONSTANT
%token <name>      PARAMETER_TOKEN
%token <index>     PLACEHOLDER_TOKEN
%token <index>     IMMED_TOKEN
%token <opcode>    BUILTIN_FUNC_NAME
%token <opcode>    OPCODE
%token <transform> UNARY_TRANSFORMATION
%token <index>     PARAM_CONSTRAINT
%token NEWLINE

%token SUBST_OP_COLON /* '->' */
%token SUBST_OP_ARROW /* ':'  */
%token BALANCE_POS    /* '=+' */
%token BALANCE_EQUAL  /* '==' */
%token BALANCE_NEG    /* '=-' */

%type <r> substitution
%type <f> function function_match
%type <p> paramlist paramlist_loop
%type <a> param expression_param immed_param subtree_param
%type <index> param_constraints

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
        /*if(!$3->Params.EnsureNoRepeatedNamedHolders())
        {
            yyerror("The replacement function may not specify the same variable twise"); YYERROR;
        }*/

        $$ = new GrammarData::Rule(ProduceNewTree, *$1, new GrammarData::ParamSpec($3));

        //std::cout << GrammarDumper().Dump(*new GrammarData::ParamSpec($3)) << "\n";
        delete $1;
      }

    | function_match SUBST_OP_COLON  paramlist NEWLINE
      /* The params provided are replaced with the new param_maybeinv_list */
      {
        $3->RecursivelySetParamMatchingType(PositionalParams);
        /*if(!$3->EnsureNoRepeatedNamedHolders())
        {
            yyerror("The replacement function may not specify the same variable twise"); YYERROR;
        }*/

        if($1->Opcode != cAnd && $1->Opcode != cOr)
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
         if($1 != cAnd && $1 != cOr)
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
         if($1 != cAnd && $1 != cOr)
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
         if($1 != cAnd && $1 != cOr)
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
        paramlist_loop '~' param /* negated/inverted param */
        {
          $$ = $1->AddParam($3->SetNegated());
        }
      | paramlist_loop param    /* normal param */
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
    |  immed_param
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
    |  expression_param '+'     /* any expression, indicated by "x", "a" etc. */
       {
         /* In matching, matches TWO or more identical repetitions of namedparam */
         /* In substitution, yields an immed containing the number of repetitions */
         $$ = $1->SetRepeat(2, true);
       }
    |  expression_param '*'     /* any expression, indicated by "x", "a" etc. */
       {
         /* In matching, matches TWO or more identical repetitions of namedparam */
         /* In substitution, yields an immed containing the number of repetitions */
         $$ = $1->SetRepeat(1, true);
       }
    |  subtree_param            /* a subtree */
    |  PLACEHOLDER_TOKEN        /* a placeholder for all params */
       {
         $$ = new GrammarData::ParamSpec($1, GrammarData::ParamSpec::RestHolderTag());
       }
    ;

    expression_param:
       PARAMETER_TOKEN param_constraints /* any expression, indicated by "x", "a" etc. */
       {
         unsigned nameindex = dumper.DumpName(*$1);
         $$ = new GrammarData::ParamSpec(nameindex, GrammarData::ParamSpec::NamedHolderTag());
         delete $1;
         $$->SetConstraint($2);
       }
    ;

    immed_param:
       IMMED_TOKEN param_constraints  /* a placeholder for some immed */
       {
         $$ = new GrammarData::ParamSpec($1, GrammarData::ParamSpec::ImmedHolderTag());
         $$->SetConstraint($2);
       }
    ;

    subtree_param:
       '(' function ')' param_constraints    /* a subtree */
       {
         $$ = new GrammarData::ParamSpec($2);
         $$->SetConstraint($4);
       }
   ;

    param_constraints: /* List of possible constraints to the given param, eg. odd,int,etc */
       param_constraints PARAM_CONSTRAINT
       {
         $$ = $1 | $2;
       }
    |  /* empty */
       {
         $$ = 0;
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

        case '@':
        {
            int c2 = std::fgetc(stdin);
            switch(c2)
            {
                case 'E': { lval->index = Value_EvenInt; return PARAM_CONSTRAINT; }
                case 'O': { lval->index = Value_OddInt; return PARAM_CONSTRAINT; }
                case 'I': { lval->index = Value_IsInteger; return PARAM_CONSTRAINT; }
                case 'F': { lval->index = Value_NonInteger; return PARAM_CONSTRAINT; }
                case 'P': { lval->index = Sign_Positive; return PARAM_CONSTRAINT; }
                case 'N': { lval->index = Sign_Negative; return PARAM_CONSTRAINT; }
                case 'Q': { lval->index = Sign_NoIdea; return PARAM_CONSTRAINT; }
                case '1': { lval->index = Oneness_One; return PARAM_CONSTRAINT; }
                case 'M': { lval->index = Oneness_NotOne; return PARAM_CONSTRAINT; }
            }
            std::ungetc(c2, stdin);
            return '@';
        }
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
            if(IdBuf == "CONSTANT_EI") { lval->num = CONSTANT_EI; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_2E") { lval->num = CONSTANT_2E; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_2EI") { lval->num = CONSTANT_2EI; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_RD") { lval->num = CONSTANT_RD; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_DR") { lval->num = CONSTANT_DR; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_PI") { lval->num = CONSTANT_PI; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_PIHALF") { lval->num = CONSTANT_PIHALF; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_L2I") { lval->num = CONSTANT_L2I; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_L10I") { lval->num = CONSTANT_L10I; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_L2") { lval->num = CONSTANT_L2; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_L10") { lval->num = CONSTANT_L10; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_L10B") { lval->num = CONSTANT_L10B; return NUMERIC_CONSTANT; }
            if(IdBuf == "CONSTANT_L10BI") { lval->num = CONSTANT_L10BI; return NUMERIC_CONSTANT; }
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
    GrammarData::Grammar Grammar_Basic;
    GrammarData::Grammar Grammar_Entry;
    GrammarData::Grammar Grammar_Intermediate;
    GrammarData::Grammar Grammar_Final1;
    GrammarData::Grammar Grammar_Final2;

    std::string sectionname;

    for(;;)
    {
        FPoptimizerGrammarParser x;
        x.yyparse();

        if(sectionname == "BASIC")
            Grammar_Basic = x.grammar;
        else if(sectionname == "ENTRY")
            Grammar_Entry = x.grammar;
        else if(sectionname == "INTERMEDIATE")
            Grammar_Intermediate = x.grammar;
        else if(sectionname == "FINAL1")
            Grammar_Final1 = x.grammar;
        else if(sectionname == "FINAL2")
            Grammar_Final2 = x.grammar;
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

    std::sort(Grammar_Entry.rules.begin(), Grammar_Entry.rules.end());

    Grammar_Intermediate.rules.insert(
       Grammar_Intermediate.rules.end(),
       Grammar_Basic.rules.begin(),
       Grammar_Basic.rules.end());

    std::sort(Grammar_Intermediate.rules.begin(), Grammar_Intermediate.rules.end());

    Grammar_Final1.rules.insert(
       Grammar_Final1.rules.end(),
       Grammar_Basic.rules.begin(),
       Grammar_Basic.rules.end());

    std::sort(Grammar_Final1.rules.begin(), Grammar_Final1.rules.end());

    std::sort(Grammar_Final2.rules.begin(), Grammar_Final2.rules.end());

    std::cout <<
        "/* This file is automatically generated. Do not edit... */\n"
        "#include \"fpoptimizer_grammar.hh\"\n"
        "#include \"fpconfig.hh\"\n"
        "#include \"fptypes.hh\"\n"
        "\n"
        "using namespace FPoptimizer_Grammar;\n"
        "using namespace FUNCTIONPARSERTYPES;\n"
        "\n";

    /*size_t e = */dumper.DumpGrammar(Grammar_Entry);
    /*size_t i = */dumper.DumpGrammar(Grammar_Intermediate);
    /*size_t f = */dumper.DumpGrammar(Grammar_Final1);
    /*size_t f = */dumper.DumpGrammar(Grammar_Final2);

    dumper.Flush();

    return 0;
}
