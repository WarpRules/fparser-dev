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

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>
#include <assert.h>

#include "crc32.hh"

#ifdef __GNUC__
# define likely(x)       __builtin_expect(!!(x), 1)
# define unlikely(x)     __builtin_expect(!!(x), 0)
#else
# define likely(x)   (x)
# define unlikely(x) (x)
#endif

/*********/
using namespace FPoptimizer_Grammar;
using namespace FUNCTIONPARSERTYPES;

class GrammarDumper;

namespace
{
    /* This function generated with make_identifier_parser.cc */
    unsigned readOpcode(const char* input)
    {
#include "fp_identifier_parser.inc"
        return 0;
    }

}

namespace GrammarData
{
    class ParamSpec;

    class MatchedParams
    {
    public:
        ParamMatchingType Type;
        std::vector<ParamSpec*> Params;
        unsigned RestHolderIndex;

    public:
        MatchedParams()                    : Type(PositionalParams), Params(), RestHolderIndex(0) { }
        MatchedParams(ParamMatchingType t) : Type(t),                Params(), RestHolderIndex(0) { }
        MatchedParams(ParamSpec* p)        : Type(PositionalParams), Params(), RestHolderIndex(0) { Params.push_back(p); }

        MatchedParams* SetType(ParamMatchingType t) { Type=t; return this; }
        MatchedParams* AddParam(ParamSpec* p) { Params.push_back(p); return this; }

        const std::vector<ParamSpec*>& GetParams() const { return Params; }

        void RecursivelySetDefaultParamMatchingType();
        bool EnsureNoRepeatedNamedHolders(std::set<unsigned>& used) const;
        bool EnsureNoRepeatedNamedHolders() const;
        bool EnsureNoVariableCoverageParams_InPositionalParamLists();

        unsigned CalcRequiredParamsCount() const;

        unsigned BuildDepMask();
        void BuildFinalDepMask();
    };

    class FunctionType
    {
    public:
        OPCODE        Opcode;
        MatchedParams Params;
    public:
        FunctionType(OPCODE o, const MatchedParams& p) : Opcode(o), Params(p) { }

        void RecursivelySetDefaultParamMatchingType()
        {
            Params.RecursivelySetDefaultParamMatchingType();
            if((Opcode == cAdd || Opcode == cMul
            || Opcode == cAnd || Opcode == cOr
            || Opcode == cAbsAnd || Opcode == cAbsOr)
            && Params.Type == PositionalParams)
                Params.Type = SelectedParams;
        }

        bool EnsureNoRepeatedNamedHolders() const
            { return Params.EnsureNoRepeatedNamedHolders(); }
    };

    class ParamSpec
    {
    public:
        unsigned DepMask;

        SpecialOpcode Opcode;      // specifies the type of the function
        union
        {
            double ConstantValue;           // for NumConstant
            unsigned Index;                 // for ParamHolder
            FunctionType* Func;             // for SubFunction
        };
        unsigned ImmedConstraint;
        bool     IsConst;                   // when SubFunction

    public:
        struct ParamHolderTag{};

        ParamSpec(FunctionType* f)
            : DepMask(),
              Opcode(SubFunction),
              Func(f),
              ImmedConstraint(0),
              IsConst(false)
        {
        }

        ParamSpec(double d)
            : DepMask(),
              Opcode(NumConstant),
              ConstantValue(d),
              ImmedConstraint(0),
              IsConst(true)
        {
        }

        ParamSpec(OPCODE o, const std::vector<ParamSpec*>& p)
            : DepMask(),
              Opcode(SubFunction),
              Func(new FunctionType(o, MatchedParams(PositionalParams))),
              ImmedConstraint(0),
              IsConst(true)
        {
            if(o == cNeg && p[0]->Opcode == NumConstant)
            {
                delete Func;
                Opcode        = NumConstant;
                ConstantValue = -p[0]->ConstantValue;
            }
            else
            {
                Func->Params.Params = p;
                /*
                if(o == cAdd && p[1]->Opcode == SubFunction
                             && p[1]->Func->Opcode == cNeg
                             && p.size() == 2)
                {
                    Func->Opcode = cSub;
                    Func->Params.Params[1] = p[1]->Func->Params.Params[0];
                } -- not done because ConstantFolding() cannot handle cSub
                */
            }
        }

        ParamSpec(unsigned i, ParamHolderTag)
            : DepMask(),
              Opcode(ParamHolder), Index(i),
              ImmedConstraint(0),
              IsConst(true)
        {
        }

/*
        // Order:
        //  NumConstant { ConstantValue }
        //  ParamHolder { Index }
        //  SubFunction { Opcode, IsConst }
        bool operator< (const ParamSpec& b) const
        {
            if(Opcode == NumConstant)
                return (b.Opcode == NumConstant)
                        ? ConstantValue < b.ConstantValue
                        : true;
            if(Opcode == ParamHolder)
                return (b.Opcode == ParamHolder)
                        ? Index < b.Index
                        : (b.Opcode == SubFunction)
                            ? true
                            : false;
            if(Opcode == SubFunction)
                return (b.Opcode == SubFunction)
                    ? (Func->Opcode != b.Func->Opcode
                         ? Func->Opcode < b.Func->Opcode
                         : IsConst < b.IsConst
                      )
                    : false;
            return false;
        }
        bool operator!= (const ParamSpec& b) const { return !operator==(b); }
        bool operator== (const ParamSpec& b) const
        {
            switch(Opcode)
            {
                case NumConstant:
                    return b.Opcode == Opcode && FloatEqual(ConstantValue, b.ConstantValue);
                case ParamHolder:
                    return b.Opcode == Opcode && ImmedConstraint == b.ImmedConstraint
                        && b.DepMask == DepMask && Index == b.Index;
                case SubFunction:
                    if(b.Opcode != SubFunction) return false;
                    if(Func->Opcode != b.Func->Opcode) return false;
                    if(ImmedConstraint != b.ImmedConstraint) return false;
                    if(DepMask != b.DepMask) return false;
                    if(IsConst != b.IsConst) return false;
                    if(Func->Params.Type != b.Func->Params.Type
                    || Func->Params.RestHolderIndex != b.Func->Params.RestHolderIndex
                    || Func->Params.Params.size() != b.Func->Params.Params.size())
                        return false;
                    for(size_t a=0; a<Func->Params.Params.size(); ++a)
                        if(*Func->Params.Params[a] != *b.Func->Params.Params[a])
                            return false;
            }
            return true;
        }
*/
        ParamSpec* SetConstraint(unsigned mask)
            { ImmedConstraint |= mask; return this; }

        unsigned BuildDepMask();

        void RecursivelySetDefaultParamMatchingType()
        {
            if(Opcode == SubFunction)
                Func->RecursivelySetDefaultParamMatchingType();
        }
        bool VerifyIsConstant()
        {
            switch(Opcode)
            {
                case NumConstant: return true;
                case ParamHolder: return ImmedConstraint & Constness_Const;
                case SubFunction:
                    if(!IsConst) return false; // subfunctions are not constant
            }
            // For const-subfunctions, all params must be const.
            for(size_t a=0; a<Func->Params.Params.size(); ++a)
                if(!Func->Params.Params[a]->VerifyIsConstant()) return false;
            return true;
        }

        bool EnsureNoRepeatedNamedHolders() const
        {
            if(Opcode != SubFunction) return true;
            MatchedParams tmp;
            tmp.Params = Func->Params.Params;
            return tmp.EnsureNoRepeatedNamedHolders();
        }

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
        bool LogicalContext;
    public:
        Rule(RuleType t, const FunctionType& f, const MatchedParams& r)
            : Type(t), Input(f), Replacement(r), LogicalContext(false)
        { }

        Rule(RuleType t, const FunctionType& f, ParamSpec* p)
            : Type(t), Input(f), Replacement(), LogicalContext(false)
        { Replacement.AddParam(p); }

        void BuildFinalDepMask()
        {
            Input.Params.BuildFinalDepMask();
            //Replacement.BuildFinalDepMask(); -- not needed, though not wrong either.
        }
        void SetLogicalContextOnly()
        {
            LogicalContext = true;
        }
    };

    class Grammar
    {
    public:
        std::vector<Rule> rules;
    public:
        Grammar(): rules() { }

        void AddRule(const Rule& r) { rules.push_back(r); }
        void BuildFinalDepMask()
        {
            for(size_t a=0; a<rules.size(); ++a)
                rules[a].BuildFinalDepMask();
        }
    };

    ////////////////////

    void MatchedParams::RecursivelySetDefaultParamMatchingType()
    {
        Type = PositionalParams;
        if(RestHolderIndex != 0)
            Type = AnyParams;

        for(size_t a=0; a<Params.size(); ++a)
            Params[a]->RecursivelySetDefaultParamMatchingType();
    }

    bool MatchedParams::EnsureNoRepeatedNamedHolders(std::set<unsigned>& used) const
    {
        for(size_t a=0; a<Params.size(); ++a)
        {
            if(Params[a]->Opcode == ParamHolder)
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

        if(RestHolderIndex != 0) return false;

        for(size_t a=0; a<Params.size(); ++a)
        {
            if(Params[a]->Opcode == SubFunction)
                if(!Params[a]->Func->Params.EnsureNoVariableCoverageParams_InPositionalParamLists())
                    return false;
        }
        return true;
    }
    unsigned MatchedParams::CalcRequiredParamsCount() const
    {
        return (unsigned)Params.size();
    }

    unsigned MatchedParams::BuildDepMask()
    {
        unsigned result = 0;
        for(size_t a=0; a<Params.size(); ++a)
            result |= Params[a]->BuildDepMask();
        return result;
    }

    void MatchedParams::BuildFinalDepMask()
    {
        unsigned all_bits = BuildDepMask();

        // For each bit that is set in all_bits, unset
        // all of them that are only set in one of the parameters.
        for(unsigned bit=1; all_bits >= bit; bit <<= 1)
            if(all_bits & bit)
            {
                unsigned count_found = 0;
                for(size_t a=0; a<Params.size(); ++a)
                {
                    unsigned param_bitmask = Params[a]->DepMask;
                    if(param_bitmask & bit) ++count_found;
                }
                if(count_found <= 1)
                {
                    for(size_t a=0; a<Params.size(); ++a)
                        Params[a]->DepMask &= ~bit;
                }
            }

        // Recurse
        for(size_t a=0; a<Params.size(); ++a)
            if(Params[a]->Opcode == SubFunction)
                Params[a]->Func->Params.BuildFinalDepMask();
    }
}

#define YY_FPoptimizerGrammarParser_MEMBERS \
    GrammarData::Grammar grammar;

std::vector<ParamSpec> plist;
std::vector<Rule>      rlist;

struct RuleComparer
{
    bool operator() (const Rule& a, const Rule& b) const
    {
        if(a.match_tree.subfunc_opcode != b.match_tree.subfunc_opcode)
            return a.match_tree.subfunc_opcode < b.match_tree.subfunc_opcode;

        // Other rules to break ties
        if(a.logical_context != b.logical_context)
            return a.logical_context < b.logical_context;

        if(a.ruletype != b.ruletype)
            return a.ruletype < b.ruletype;

        if(a.match_tree.match_type != b.match_tree.match_type)
            return a.match_tree.match_type < b.match_tree.match_type;

        if(a.match_tree.param_count != b.match_tree.param_count)
            return a.match_tree.param_count < b.match_tree.param_count;

        if(a.repl_param_count != b.repl_param_count)
            return a.repl_param_count < b.repl_param_count;

        if(a.match_tree.param_list != b.match_tree.param_list)
            return a.match_tree.param_list < b.match_tree.param_list;

        if(a.repl_param_list != b.repl_param_list)
            return a.repl_param_list < b.repl_param_list;

        return false;
    }

    bool operator() (unsigned a, unsigned b) const
    {
        return this->operator() ( rlist[a], rlist[b] );
    }
};

class GrammarDumper
{
private:
    std::string GenName(const char* prefix)
    {
        static unsigned counter = 0;
        std::ostringstream tmp;
        tmp << prefix << ++counter;
        return tmp.str();
    }
private:
    std::map<std::string, size_t> n_index;

    std::vector<std::string>        nlist;
    std::map<std::string, Grammar>  glist;
public:
    GrammarDumper():
        n_index(),
        nlist(),glist()
    {
        plist.reserve(16384);
        nlist.reserve(16);
        rlist.reserve(16384);
    }

    unsigned ConvertNamedHolderNameIntoIndex(const std::string& n)
    {
        std::map<std::string, size_t>::const_iterator i = n_index.find(n);
        if(i != n_index.end()) return i->second;
        nlist.push_back(n);
        return n_index[n] = (unsigned)(nlist.size()-1);
    }
    size_t GetNumNamedHolderNames() const { return nlist.size(); }

    void DumpParamList(const std::vector<GrammarData::ParamSpec*>& Params,
                       unsigned&       param_count,
                       unsigned&       param_list)
    {
        param_count = (unsigned)Params.size();
        param_list  = 0;
        for(unsigned a=0; a<param_count; ++a)
        {
            ParamSpec p = CreateParam(*Params[a]);

            unsigned paramno = (unsigned)plist.size();

            for(size_t b = 0; b < plist.size(); ++b)
                if(plist[b].first == p.first
                && ParamSpec_Compare(plist[b].second, p.second, p.first))
                {
                    paramno = (unsigned)b;
                    break;
                }

            if(paramno == plist.size()) plist.push_back(p);

            param_list |= paramno << (a * PARAM_INDEX_BITS);
        }
    }

    ParamSpec CreateParam(const GrammarData::ParamSpec& p)
    {
        unsigned    pcount;
        unsigned    plist;
        switch(p.Opcode)
        {
            case SubFunction:
            {
                ParamSpec_SubFunction* result = new ParamSpec_SubFunction;
                result->constraints    = p.ImmedConstraint;
                result->data.subfunc_opcode = p.Func->Opcode;
                result->data.match_type     = p.Func->Params.Type;
                DumpParamList(p.Func->Params.Params, pcount, plist);
                result->data.param_count = pcount;
                result->data.param_list  = plist;
                result->depcode        = p.DepMask;
                result->data.restholder_index = p.Func->Params.RestHolderIndex;
                if(p.IsConst)
                {
                    result->data.match_type = GroupFunction;
                    result->constraints |= Constness_Const;
                }
                return std::make_pair(SubFunction, (void*)result);
            }
            case NumConstant:
            {
                ParamSpec_NumConstant* result = new ParamSpec_NumConstant;
                result->constvalue     = p.ConstantValue;
                return std::make_pair(NumConstant, (void*)result);
            }
            case ParamHolder:
            {
                ParamSpec_ParamHolder* result = new ParamSpec_ParamHolder;
                result->constraints    = p.ImmedConstraint;
                result->index          = p.Index;
                result->depcode        = p.DepMask;
                return std::make_pair(ParamHolder, (void*)result);
            }
        }
        std::cout << "???\n";
        return std::make_pair(SubFunction, (void*) 0);
    }

    Rule CreateRule(const GrammarData::Rule& r)
    {
        unsigned min_params = r.Input.Params.CalcRequiredParamsCount();

        Rule ritem;
        memset(&ritem, 0, sizeof(ritem));
        //ritem.n_minimum_params          = min_params;
        ritem.ruletype                  = r.Type;
        ritem.logical_context           = r.LogicalContext;
        ritem.match_tree.subfunc_opcode = r.Input.Opcode;
        ritem.match_tree.match_type     = r.Input.Params.Type;
        ritem.match_tree.restholder_index = r.Input.Params.RestHolderIndex;
        unsigned         pcount;
        unsigned         plist;
        DumpParamList(r.Input.Params.Params, pcount, plist);
        ritem.match_tree.param_count = pcount;
        ritem.match_tree.param_list  = plist;

        DumpParamList(r.Replacement.Params,  pcount, plist);
        ritem.repl_param_count = pcount;
        ritem.repl_param_list  = plist;
        return ritem;
    }

    void RegisterGrammar(const std::vector<GrammarData::Grammar>& gset)
    {
        std::vector<Rule> this_rules;

        for(size_t a=0; a<gset.size(); ++a)
        {
            const GrammarData::Grammar& g = gset[a];

            for(size_t a=0; a<g.rules.size(); ++a)
            {
                if(g.rules[a].Input.Opcode == cNop) continue;
                this_rules.push_back( CreateRule(g.rules[a]) );
            }
        }

        std::sort(this_rules.begin(), this_rules.end(),
                  RuleComparer());

        for(size_t a=0; a<this_rules.size(); ++a)
        {
            const Rule& r = this_rules[a];

            // Add to global rule list, unless it's already there
            bool dup=false;
            for(size_t c=0; c<rlist.size(); ++c)
                if(memcmp(&r, &rlist[c], sizeof(r)) == 0)
                {
                    // Already in global rule list...
                    dup = true;
                    break;
                }
            if(!dup)
                rlist.push_back(r);
        }
    }

    void DumpGrammar(const std::string& grammarname,
                     const std::vector<GrammarData::Grammar>& gset)
    {
        std::vector<unsigned> rule_list;

        std::vector<Rule> this_rules;

        for(size_t a=0; a<gset.size(); ++a)
        {
            const GrammarData::Grammar& g = gset[a];

            for(size_t a=0; a<g.rules.size(); ++a)
            {
                if(g.rules[a].Input.Opcode == cNop) continue;
                this_rules.push_back( CreateRule(g.rules[a]) );
            }
        }

        std::sort(this_rules.begin(), this_rules.end(),
                  RuleComparer());

        for(size_t a=0; a<this_rules.size(); ++a)
        {
            const Rule& r = this_rules[a];

            // Add to global rule list, unless it's already there
            bool dup=false;
            for(size_t c=0; c<rlist.size(); ++c)
                if(memcmp(&r, &rlist[c], sizeof(r)) == 0)
                {
                    // Already in global rule list...
                    // Add to grammar's rule list unless it's already there
                    dup = false;
                    for(size_t b=0; b<rule_list.size(); ++b)
                        if(c == rule_list[b])
                        {
                            dup = true;
                            break;
                        }
                    if(!dup)
                    {
                        // Global duplicate, but not yet in grammar.
                        rule_list.push_back(c);
                    }
                    dup = true;
                    break;
                }
            if(!dup)
            {
                // Not in global rule list. Add there and in grammar.
                rule_list.push_back( (unsigned) rlist.size() );
                rlist.push_back(r);
            }
        }

        Grammar& gitem = glist[grammarname];

        gitem.rule_count = (unsigned) rule_list.size();

        std::sort(rule_list.begin(), rule_list.end(),
                  RuleComparer());

        for(size_t a=0; a<rule_list.size(); ++a)
            gitem.rule_list[a] = rule_list[a];
    }

    static std::string ConstraintsToString(unsigned constraints)
    {
        std::ostringstream result;
        const char* sep = "";
        static const char s[] = " | ";
        switch( ImmedConstraint_Value( constraints & ValueMask ) )
        {
            case ValueMask: case Value_AnyNum: break;
            case Value_EvenInt: result << sep << "Value_EvenInt"; sep=s; break;
            case Value_OddInt: result << sep << "Value_OddInt"; sep=s; break;
            case Value_IsInteger: result << sep << "Value_IsInteger"; sep=s; break;
            case Value_NonInteger: result << sep << "Value_NonInteger"; sep=s; break;
            case Value_Logical: result << sep << "Value_Logical"; sep=s; break;
        }
        switch( ImmedConstraint_Sign( constraints & SignMask ) )
        {
            /*case SignMask:*/ case Sign_AnySign: break;
            case Sign_Positive: result << sep << "Sign_Positive"; sep=s; break;
            case Sign_Negative: result << sep << "Sign_Negative"; sep=s; break;
            case Sign_NoIdea:   result << sep << "Sign_NoIdea"; sep=s; break;
        }
        switch( ImmedConstraint_Oneness( constraints & OnenessMask ) )
        {
            case OnenessMask: case Oneness_Any: break;
            case Oneness_One: result << sep << "Oneness_One"; sep=s; break;
            case Oneness_NotOne: result << sep << "Oneness_NotOne"; sep=s; break;
        }
        switch( ImmedConstraint_Constness( constraints & ConstnessMask ) )
        {
            /*case ConstnessMask:*/ case Oneness_Any: break;
            case Constness_Const: result << sep << "Constness_Const"; sep=s; break;
        }
        if(!*sep) result << "0";
        return result.str();
    }

    static std::string ConstValueToString(double value)
    {
        std::ostringstream result;
        result.precision(50);
        #define if_const(n) \
            if(FloatEqual(value, n)) result << #n; \
            else if(FloatEqual(value, -n)) result << "-" #n;
        if_const(CONSTANT_E)
        else if_const(CONSTANT_EI)
        else if_const(CONSTANT_2E)
        else if_const(CONSTANT_2EI)
        else if_const(CONSTANT_PI)
        else if_const(CONSTANT_L10)
        else if_const(CONSTANT_L2)
        else if_const(CONSTANT_L10I)
        else if_const(CONSTANT_L2I)
        else if_const(CONSTANT_L10B)
        else if_const(CONSTANT_L10BI)
        else if_const(CONSTANT_DR)
        else if_const(CONSTANT_RD)
        else if_const(CONSTANT_PIHALF)
        else if_const(FPOPT_NAN_CONST)
        #undef if_const
        else result << value;
        return result.str();
    }

    struct ParamCollection
    {
        std::vector<ParamSpec_ParamHolder>   plist_p;
        std::vector<ParamSpec_NumConstant>   plist_n;
        std::vector<ParamSpec_SubFunction>   plist_s;

        void Populate(const ParamSpec& param)
        {
            #define set(when, list, code) \
                case when: \
                  { for(size_t a=0; a<list.size(); ++a) \
                        if(ParamSpec_Compare(param.second, (const void*) &list[a], when)) \
                            return; \
                    list.push_back( *(ParamSpec_##when*) param.second ); \
                    code; \
                    break; }
            switch(param.first)
            {
                set(ParamHolder, plist_p, );
                set(NumConstant, plist_n, );
                set(SubFunction, plist_s,
                     ParamSpec_SubFunction* p = (ParamSpec_SubFunction*)param.second;
                     for(size_t a=0; a<p->data.param_count; ++a)
                         Populate( ParamSpec_Extract( p->data.param_list, a) );
                    );
            }
            #undef set
        }

        struct p_compare { bool operator() (
            const ParamSpec_ParamHolder& a,
            const ParamSpec_ParamHolder& b) const
        {
            if(a.index != b.index) return a.index < b.index;
            return false;
        } };
        struct n_compare { bool operator() (
            const ParamSpec_NumConstant& a,
            const ParamSpec_NumConstant& b) const
        {
            return a.constvalue < b.constvalue;
        } };
        struct s_compare { bool operator() (
            const ParamSpec_SubFunction& a,
            const ParamSpec_SubFunction& b) const
        {
            if(a.data.subfunc_opcode != b.data.subfunc_opcode)
                return a.data.subfunc_opcode < b.data.subfunc_opcode;
            if(a.data.match_type != b.data.match_type)
                return a.data.match_type < b.data.match_type;
            size_t min_param_count = a.data.param_count;
            if(b.data.param_count < min_param_count)
                min_param_count = b.data.param_count;
            for(size_t c=0; c< min_param_count; ++c)
            {
                ParamSpec aa = ParamSpec_Extract(a.data.param_list, (unsigned)c);
                ParamSpec bb = ParamSpec_Extract(b.data.param_list, (unsigned)c);
                if(aa.first != bb.first)
                    return aa.first < bb.first;
                switch(aa.first)
                {
                    case ParamHolder:
                        if(p_compare() (*(const ParamSpec_ParamHolder*)aa.second,
                                        *(const ParamSpec_ParamHolder*)bb.second))
                            return true;
                        break;
                    case NumConstant:
                        if(n_compare() (*(const ParamSpec_NumConstant*)aa.second,
                                        *(const ParamSpec_NumConstant*)bb.second))
                            return true;
                        break;
                    case SubFunction:
                        if(s_compare() (*(const ParamSpec_SubFunction*)aa.second,
                                        *(const ParamSpec_SubFunction*)bb.second))
                            return true;
                        break;
                }
            }
            if(a.data.param_count != b.data.param_count)
                return a.data.param_count < b.data.param_count;
            return false;
        } };

        void Sort()
        {
            std::stable_sort(plist_p.begin(), plist_p.end(), p_compare());
            std::stable_sort(plist_n.begin(), plist_n.end(), n_compare());
            std::stable_sort(plist_s.begin(), plist_s.end(), s_compare());
        }

        std::string ParamPtrToString(unsigned paramlist, unsigned index) const
        {
            const ParamSpec& p = ParamSpec_Extract(paramlist, index);
            if(!p.second) return "0";
            #define set(when, list, c) \
                case when: \
                    for(size_t a=0; a<list.size(); ++a) \
                        if(ParamSpec_Compare(p.second, (const void*)&list[a], when)) \
                        { \
                            std::ostringstream result; \
                            result << #c "(" << a << ")"; \
                            return result.str(); \
                        } \
                    break;
            switch(p.first)
            {
                set(ParamHolder, plist_p, P);
                set(NumConstant, plist_n, N);
                set(SubFunction, plist_s, S);
            }
            #undef set
            return "?"+FP_GetOpcodeName(p.first);
        }

        std::string ParamListToString(unsigned paramlist, unsigned paramcount) const
        {
            std::ostringstream result;
            switch(paramcount)
            {
                case 0: result << "0"; break;
                case 1: result << "P1(" << ParamPtrToString(paramlist,0) << ")"; break;
                case 2: result << "P2(" << ParamPtrToString(paramlist,0) << ','
                                                   << ParamPtrToString(paramlist,1)
                                                   << ")"; break;
                case 3: result << "P3(" << ParamPtrToString(paramlist,0) << ','
                                                     << ParamPtrToString(paramlist,1) << ','
                                                     << ParamPtrToString(paramlist,2)
                                                     << ")"; break;
                default:
                    result << "?";
            }
            //result << ',' << param_count;
            std::string res = result.str();
            while(res.size() < 24) res += ' ';
            return res;
        }

        std::string ParamHolderToString(const ParamSpec_ParamHolder& i) const
        {
            std::ostringstream result;
            result << "{" << i.index
                   << ", " << ConstraintsToString(i.constraints)
                   << ", 0x" << i.depcode
                   << "}";
            return result.str();
        }

        std::string NumConstantToString(const ParamSpec_NumConstant& i) const
        {
            std::ostringstream result;
            result << "{" << ConstValueToString(i.constvalue)
                   << "}";
            return result.str();
        }

        std::string SubFunctionDataToString(const ParamSpec_SubFunctionData& i) const
        {
            std::ostringstream result;
            result << "{"  << i.param_count
                   <<  "," << ParamListToString(i.param_list, i.param_count)
                   << ", " << FP_GetOpcodeName(i.subfunc_opcode, true)
                   << ","  << (i.match_type == PositionalParams ? "PositionalParams"
                            :  i.match_type == SelectedParams   ? "SelectedParams  "
                            :  i.match_type == AnyParams        ? "AnyParams       "
                            :/*i.match_type == GroupFunction  ?*/ "GroupFunction   "
                            )
                   << "," << i.restholder_index
                   << "}";
            return result.str();
        }

        std::string SubFunctionToString(const ParamSpec_SubFunction& i) const
        {
            std::ostringstream result;
            result << "{" << SubFunctionDataToString(i.data)
                   << ", " << ConstraintsToString(i.constraints)
                   << ", 0x" << i.depcode
                   << "}";
            return result.str();
        }
    };

    void Flush()
    {
        ParamCollection collection;
        for(size_t a=0; a<rlist.size(); ++a)
        {
            for(unsigned b=0; b < rlist[a].match_tree.param_count; ++b)
                collection.Populate( ParamSpec_Extract(rlist[a].match_tree.param_list, b) );
            for(unsigned b=0; b < rlist[a].repl_param_count; ++b)
                collection.Populate( ParamSpec_Extract(rlist[a].repl_param_list, b) );
        }
        collection.Sort();

        for(std::map<std::string, Grammar>::const_iterator
             i = glist.begin(); i != glist.end(); ++i)
            std::cout << "#define grammar_" << i->first << " grammar_" << i->first << "_tweak\n";
        std::cout <<
            "#include \"fpoptimizer_grammar.hh\"\n";
        for(std::map<std::string, Grammar>::const_iterator
             i = glist.begin(); i != glist.end(); ++i)
            std::cout << "#undef grammar_" << i->first << "\n";

        std::cout <<
            "\n"
            "using namespace FPoptimizer_Grammar;\n"
            "using namespace FUNCTIONPARSERTYPES;\n"
            "\n"
            "namespace\n"
            "{\n"
            "    const struct ParamSpec_List\n"
            "    {\n";

        std::ostringstream undef_buf;
        { std::ostringstream buf;
        std::ostringstream base;

        #define set(type, list, c) \
            std::cout << \
            "        ParamSpec_" #type " " #list "[" << collection.list.size() << "];\n" \
            "#define " #c "(n) (n" << base.str() << ")\n"; \
            base << '+' << collection.list.size(); \
            undef_buf << \
            "#undef " #c "\n"; \
            buf << \
            "        { /* " << #list << " - ParamSpec_" #type "[" << collection.list.size() << "] */\n"; \
            for(size_t a=0; a<collection.list.size(); ++a) \
            { \
                buf << "        /* " << a << "\t*/ " \
                    << collection.type##ToString(collection.list[a]) \
                    << ", /* "; \
                FPoptimizer_Grammar::DumpParam( ParamSpec(type, (const void*) &collection.list[a]), buf); \
                buf << " */\n"; \
            } \
            buf << \
            "        },\n" \
            "\n";

        set(ParamHolder,   plist_p, P)
        set(NumConstant,   plist_n, N)
        set(SubFunction,   plist_s, S)

        std::cout <<
            "    } /*PACKED_GRAMMAR_ATTRIBUTE*/ plist =\n"
            "    {\n"
            << buf.str() <<
            "    };\n"
            "}\n";
        }

        #undef set

        std::cout <<
            "namespace FPoptimizer_Grammar\n"
            "{\n";
        std::cout <<
            "    const Rule grammar_rules[" << rlist.size() << "] =\n"
            "    {\n";
        for(size_t a=0; a<rlist.size(); ++a)
        {
            std::cout <<
            "        /* " << a << ":\t";
            ParamSpec_SubFunction tmp = {rlist[a].match_tree,0,0};
            if(rlist[a].logical_context) std::cout << "@L ";
            FPoptimizer_Grammar::DumpParam( ParamSpec(SubFunction, (const void*) &tmp) );
            switch(rlist[a].ruletype)
            {
                case ProduceNewTree:
                    std::cout <<
                    "\n"
                    "         *\t->\t";
                    FPoptimizer_Grammar::DumpParam(
                        ParamSpec_Extract(rlist[a].repl_param_list, 0) );
                    break;
                case ReplaceParams: default:
                    std::cout <<
                    "\n"
                    "         *\t:\t";
                    FPoptimizer_Grammar::DumpParams( rlist[a].repl_param_list, rlist[a].repl_param_count);
                    break;
            }
            std::cout <<
            "\n"
            "         */\t\t "
                        "{"
                        << (rlist[a].ruletype == ProduceNewTree  ? "ProduceNewTree"
                         :/*rlist[a].ruletype == ReplaceParams ?*/ "ReplaceParams "
                           )
                        << ", " << (rlist[a].logical_context ? "true " : "false")
                        << ", " << rlist[a].repl_param_count
                        <<  "," << collection.ParamListToString(rlist[a].repl_param_list, rlist[a].repl_param_count)
                        << ", " << collection.SubFunctionDataToString(rlist[a].match_tree)
                        << "},\n";
        }
        std::cout <<
            "    };\n"
            << undef_buf.str()
            <<
            "\n";
        for(std::map<std::string, Grammar>::const_iterator
             i = glist.begin(); i != glist.end(); ++i)
        {
            std::cout << "    struct grammar_" << i->first << "_type\n"
                         "    {\n"
                         "        unsigned c;\n"
                         "        unsigned char l[" << i->second.rule_count << "];\n"
                         "    };\n"
                         "    extern \"C\"\n"
                         "    {\n"
                         "        grammar_" << i->first << "_type grammar_" << i->first << " =\n"
                         "        {\n"
                         "            " << i->second.rule_count << ",\n"
                         "            { ";
            for(size_t p=0; p<i->second.rule_count; ++p)
            {
                std::cout << (unsigned) i->second.rule_list[p];
                if(p+1 == i->second.rule_count) std::cout << "\n";
                else
                {
                    std::cout << ',';
                    if(p%10 == 9)
                        std::cout << "\n              ";
                }
            }
            std::cout << "    }   };  }\n";
        }
        std::cout <<
            "}\n";
    }
private:
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
    unsigned           index;
    OPCODE             opcode;
}

/* See documentation about syntax and token meanings in fpoptimizer.dat */
%token <num>       NUMERIC_CONSTANT
%token <index>     NAMEDHOLDER_TOKEN
%token <index>     RESTHOLDER_TOKEN
%token <index>     IMMEDHOLDER_TOKEN
%token <opcode>    BUILTIN_FUNC_NAME
%token <opcode>    OPCODE
%token <opcode>    UNARY_TRANSFORMATION
%token <index>     PARAM_CONSTRAINT
%token NEWLINE

%token SUBST_OP_COLON /* ':' */
%token SUBST_OP_ARROW /* '->'  */

%type <r> substitution
%type <f> function function_match
%type <p> paramlist
%type <a> param
%type <index> param_constraints

%%
    grammar:
      grammar substitution
      {
        this->grammar.AddRule(*$2);
        delete $2;
      }
    | grammar param_constraints substitution
      {
        if($2 != Value_Logical)
        {
            char msg[] = "Only @L rule constraint is allowed for now";
            yyerror(msg); YYERROR;
        }
        if($2 & Value_Logical)
            $3->SetLogicalContextOnly();
        this->grammar.AddRule(*$3);
        delete $3;
      }
    | grammar NEWLINE
    | /* empty */
    ;

    substitution:
      function_match SUBST_OP_ARROW param NEWLINE
      /* Entire function is changed into the particular param */
      {
        $3->RecursivelySetDefaultParamMatchingType();

        $$ = new GrammarData::Rule(ProduceNewTree, *$1, $3);
        delete $1;
      }

    | function_match SUBST_OP_ARROW function NEWLINE
      /* Entire function changes, the param_notinv_list is rewritten */
      /* NOTE: "p x -> o y"  is a shortcut for "p x -> (o y)"  */
      {
        GrammarData::ParamSpec* p = new GrammarData::ParamSpec($3);
        p->RecursivelySetDefaultParamMatchingType();
        /*if(!$3->Params.EnsureNoRepeatedNamedHolders())
        {
            char msg[] = "The replacement function may not specify the same variable twice";
            yyerror(msg); YYERROR;
        }*/

        $$ = new GrammarData::Rule(ProduceNewTree, *$1, p);

        //std::cout << GrammarDumper().Dump(*new GrammarData::ParamSpec($3)) << "\n";
        delete $1;
      }

    | function_match SUBST_OP_COLON  paramlist NEWLINE
      /* The params provided are replaced with the new param_maybeinv_list */
      {
        /*if($1->Params.RestHolderIndex != 0)
        {
            char msg[] = "Restholder is not valid in the outermost function when ReplaceParams is used";
            yyerror(msg); YYERROR;
        }*/
        $3->RecursivelySetDefaultParamMatchingType();
        /*if(!$3->EnsureNoRepeatedNamedHolders())
        {
            char msg[] = "The replacement function may not specify the same variable twice";
            yyerror(msg); YYERROR;
        }*/

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
               char msg[] = "Restholders such as <1>, must not occur in bracketed param lists on the matching side";
               yyerror(msg); YYERROR;
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
         $$ = new GrammarData::FunctionType($1, *$3);
         delete $3;
       }
    |  OPCODE '{' paramlist '}'
       /* Match a function with opcode=opcode,
        * and the exact parameter list in any order
        */
       {
         $$ = new GrammarData::FunctionType($1, *$3->SetType(SelectedParams));
         delete $3;
       }
    |  OPCODE paramlist
       /* Match a function with opcode=opcode and the given way of matching params */
       /* There may be more parameters, don't care about them */
       {
         $$ = new GrammarData::FunctionType($1, *$2->SetType(AnyParams));
         delete $2;
       }
    ;

    paramlist: /* left-recursive list of 0-n params with no delimiter */
        paramlist param    /* param */
        {
          $$ = $1->AddParam($2);
        }
      | paramlist RESTHOLDER_TOKEN /* a placeholder for all remaining params */
        {
          if($1->RestHolderIndex != 0)
          {
              char msg[] = "Illegal attempt to specify two restholders for the same param list";
              yyerror(msg); YYERROR;
          }
          $1->RestHolderIndex = $2;
          $$ = $1;
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
    |  IMMEDHOLDER_TOKEN param_constraints  /* a placeholder for some immed */
       {
         $$ = new GrammarData::ParamSpec($1, GrammarData::ParamSpec::ParamHolderTag());
         $$->SetConstraint($2 | Constness_Const);
       }
    |  BUILTIN_FUNC_NAME '(' paramlist ')'  /* literal logarithm/sin/etc. of the provided immed-type params -- also sum/product/minimum/maximum */
       {
         /* Verify that $3 consists of constants */
         $$ = new GrammarData::ParamSpec($1, $3->GetParams() );
         if(!$$->VerifyIsConstant())
         {
             char msg[] = "Not constant";
             yyerror(msg); YYERROR;
         }
         delete $3;
       }
    |  NAMEDHOLDER_TOKEN param_constraints /* any expression, indicated by "x", "a" etc. */
       {
         $$ = new GrammarData::ParamSpec($1 + 2, GrammarData::ParamSpec::ParamHolderTag());
         $$->SetConstraint($2);
       }
    |  '(' function ')' param_constraints    /* a subtree */
       {
         $$ = new GrammarData::ParamSpec($2);
         $$->SetConstraint($4);
       }
    |  UNARY_TRANSFORMATION param   /* the negated/inverted literal value of the param */
       {
         /* Verify that $2 is constant */
         if(!$2->VerifyIsConstant())
         {
             char msg[] = "Not constant";
             yyerror(msg); YYERROR;
         }
         std::vector<GrammarData::ParamSpec*> tmp;
         tmp.push_back($2);
         $$ = new GrammarData::ParamSpec($1, tmp);
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
enum { cVar,cFetch,cPopNMov };
#endif

void FPoptimizerGrammarParser::yyerror(char* msg) // bison++ declares msg as char*.
{
    std::cerr << msg << std::endl;
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
            if(c == '['
            || c == '$')
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
            lval->opcode = cNeg;
            return UNARY_TRANSFORMATION;
        }
        case '/':
            lval->opcode = cInv;
            return UNARY_TRANSFORMATION;

        case '=':
        {
            int c2 = std::fgetc(stdin);
            std::ungetc(c2, stdin);
            return '=';
        }
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
        case '%': { lval->index = 0; return IMMEDHOLDER_TOKEN; }
        case '&': { lval->index = 1; return IMMEDHOLDER_TOKEN; }

        case '@':
        {
            int c2 = std::fgetc(stdin);
            switch(c2)
            {
                case 'E': { lval->index = Value_EvenInt; return PARAM_CONSTRAINT; }
                case 'O': { lval->index = Value_OddInt; return PARAM_CONSTRAINT; }
                case 'I': { lval->index = Value_IsInteger; return PARAM_CONSTRAINT; }
                case 'F': { lval->index = Value_NonInteger; return PARAM_CONSTRAINT; }
                case 'L': { lval->index = Value_Logical; return PARAM_CONSTRAINT; }
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
            return RESTHOLDER_TOKEN;
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
            if(IdBuf == "cAbsNot") { lval->opcode = cAbsNot; return OPCODE; }
            if(IdBuf == "cAbsNotNot") { lval->opcode = cAbsNotNot; return OPCODE; }
            if(IdBuf == "cAbsAnd") { lval->opcode = cAbsAnd; return OPCODE; }
            if(IdBuf == "cAbsOr") { lval->opcode = cAbsOr; return OPCODE; }
            if(IdBuf == "cAbsIf") { lval->opcode = cAbsIf; return OPCODE; }
            if(IdBuf == "cDeg")  { lval->opcode = cDeg; return OPCODE; }
            if(IdBuf == "cRad")  { lval->opcode = cRad; return OPCODE; }
            if(IdBuf == "cInv")  { lval->opcode = cInv; return OPCODE; }
            if(IdBuf == "cSqr")  { lval->opcode = cSqr; return OPCODE; }
            if(IdBuf == "cRDiv") { lval->opcode = cRDiv; return OPCODE; }
            if(IdBuf == "cRSub") { lval->opcode = cRSub; return OPCODE; }
            if(IdBuf == "cRSqrt") { lval->opcode = cRSqrt; return OPCODE; }
#ifdef FP_SUPPORT_OPTIMIZER
            if(IdBuf == "cLog2by") { lval->opcode = cLog2by; return OPCODE; }
#else
            if(IdBuf == "cLog2by") { lval->opcode = cNop; return OPCODE; }
#endif

            /* Detect other function opcodes */
            if(IdBuf[0] == 'c' && std::isupper(IdBuf[1]))
            {
                // This has a chance of being an opcode token
                std::string opcodetoken = IdBuf.substr(1);
                opcodetoken[0] = std::tolower(opcodetoken[0]);
                
                unsigned nameLength = readOpcode(opcodetoken.c_str());
                if(nameLength & 0x80000000U)
                {
                    lval->opcode = FUNCTIONPARSERTYPES::OPCODE(
                        (nameLength >> 16) & 0x7FFF );
                    return OPCODE;
                }
                std::cerr <<
                    "Warning: Unrecognized opcode '" << IdBuf << "' interpreted as cNop\n";
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
                    unsigned nameLength = readOpcode(grouptoken.c_str());
                    if(nameLength & 0x80000000U)
                    {
                        lval->opcode = FUNCTIONPARSERTYPES::OPCODE(
                            (nameLength >> 16) & 0x7FFF );
                        return BUILTIN_FUNC_NAME;
                    }
                    if(IdBuf == "MOD")
                    {
                        lval->opcode = cMod;
                        return BUILTIN_FUNC_NAME;
                    }

                    std::cerr << "Warning: Unrecognized constant function '" << IdBuf
                              << "' interpreted as cNop\n";
                    lval->opcode = cNop;
                    return BUILTIN_FUNC_NAME;
                }
            NotAGroupToken:;
            }
            // Anything else is an identifier
            lval->index = dumper.ConvertNamedHolderNameIntoIndex(IdBuf);
            // std::cerr << "'" << IdBuf << "'' interpreted as PARAM\n";

            return NAMEDHOLDER_TOKEN;
        }
        default:
        {
            std::cerr << "Ignoring unidentifier character '" << char(c) << "'\n";
            return yylex(lval); // tail recursion
        }
    }
    return EOF;
}

unsigned GrammarData::ParamSpec::BuildDepMask()
{
    DepMask = 0;
    switch(Opcode)
    {
        case ParamHolder:
            DepMask |= 1 << Index;
            break;
        case SubFunction:
            DepMask = Func->Params.BuildDepMask();
            break;
        default: break;
    }
    return DepMask;
}

namespace FPoptimizer_Grammar
{
    ParamSpec ParamSpec_Extract(unsigned paramlist, unsigned index)
    {
        unsigned plist_index = (paramlist >> (index*PARAM_INDEX_BITS))
                               % (1 << PARAM_INDEX_BITS);
        return plist[plist_index];
    }
}

int main()
{
    std::map<std::string, GrammarData::Grammar> sections;

    std::string sectionname;

    for(;;)
    {
        FPoptimizerGrammarParser x;
        x.yyparse();

        x.grammar.BuildFinalDepMask();
        sections[sectionname] = x.grammar;

        int c = std::fgetc(stdin);
        if(c != '[')
        {
            std::ungetc(c, stdin);
            break;
        }

        sectionname.clear();
        for(;;)
        {
            c = std::fgetc(stdin);
            if(c == ']' || c == EOF) break;
            sectionname += (char)c;
        }
        std::cerr << "Parsing [" << sectionname << "]\n";
    }

    std::map<std::string, std::vector<std::string> > grammar_components;
    sectionname = "";
    for(;;)
    {
        int c = std::fgetc(stdin);
        if(c == ' ' || c == '\t' || c == '\r' || c == '\n') continue;
        if(c == '#')
            { do { c = std::fgetc(stdin); } while(!(c == '\n' || c == EOF));
              continue; }
        if(c == '$')
        {
            sectionname = "";
            for(;;)
            {
                c = std::fgetc(stdin);
                if(c == EOF) break;
                if(c == ' ' || c == '\t' || c == '\r' || c == '\n') break;
                if(c == ':') break;
                sectionname += char(c);
            }
            std::cerr << "Parsing $" << sectionname << "\n";
            continue;
        }
        if((c >= 'A' && c <= 'Z') || c == '_' || (c >= '0' && c <= '9'))
        {
            std::string componentname;
            for(;;)
            {
                if(c == EOF) break;
                if(c == ' ' || c == '\t' || c == '\r' || c == '\n') break;
                componentname += char(c);
                c = std::fgetc(stdin);
            }
            std::cerr << "- Has [" << componentname << "]\n";
            grammar_components[sectionname].push_back(componentname);
            //dumper.AddRulesFrom(sections[componentname]);
        }
        else break;
    }

    std::cout <<
        "/* This file is automatically generated. Do not edit... */\n"
        "#include \"fpoptimizer_consts.hh\"\n"
        "#include \"fpconfig.hh\"\n"
        "#include \"fptypes.hh\"\n"
        "#include <algorithm>\n"
        "\n"
        "#define P1(a) a\n"
        "#define P2(a,b) (P1(a) | (b << PARAM_INDEX_BITS))\n"
        "#define P3(a,b,c) (P2(a,b) | (c << (PARAM_INDEX_BITS*2)))\n"
        "\n";

    std::vector<GrammarData::Grammar> components;
    for(std::map<std::string, std::vector<std::string> >::const_iterator
        i = grammar_components.begin();
        i != grammar_components.end();
        ++i)
    {
        for(size_t a=0; a<i->second.size(); ++a)
            components.push_back(sections[ i->second[a] ]);
    }
    dumper.RegisterGrammar(components);

    for(std::map<std::string, std::vector<std::string> >::const_iterator
        i = grammar_components.begin();
        i != grammar_components.end();
        ++i)
    {
        components.clear();
        for(size_t a=0; a<i->second.size(); ++a)
            components.push_back(sections[ i->second[a] ]);
        dumper.DumpGrammar(i->first, components);
    }
    dumper.Flush();

    std::cout <<
        "#undef P1\n"
        "#undef P2\n"
        "#undef P3\n"
        "\n"
        "namespace FPoptimizer_Grammar\n"
        "{\n"
        "    ParamSpec ParamSpec_Extract(unsigned paramlist, unsigned index)\n"
        "    {\n"
        "        index = (paramlist >> (index * PARAM_INDEX_BITS)) % (1 << PARAM_INDEX_BITS);\n"
        "        const unsigned p_begin = 0;\n"
        "        const unsigned n_begin = p_begin + sizeof(plist.plist_p)/sizeof(*plist.plist_p);\n"
        "        const unsigned s_begin = n_begin + sizeof(plist.plist_n)/sizeof(*plist.plist_n);\n"
        "      /*const unsigned     end = s_begin + sizeof(plist.plist_s)/sizeof(*plist.plist_s);*/\n"
        "        if(index < s_begin)\n"
        "        {\n"
        "            if(index < n_begin)\n"
        "                return ParamSpec(ParamHolder,(const void*)&plist.plist_p[index-p_begin]);\n"
        "            else\n"
        "                return ParamSpec(NumConstant,(const void*)&plist.plist_n[index-n_begin]);\n"
        "        }\n"
        "        else\n"
        "            return ParamSpec(SubFunction,(const void*)&plist.plist_s[index-s_begin]);\n"
        "    }\n"
        "}\n";

    return 0;
}
