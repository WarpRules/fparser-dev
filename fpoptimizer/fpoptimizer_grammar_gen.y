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

struct RuleComparer
{
    bool operator() (const Rule& a, const Rule& b)
    {
        if(a.match_tree.subfunc_opcode != b.match_tree.subfunc_opcode)
            return a.match_tree.subfunc_opcode < b.match_tree.subfunc_opcode;
        if(a.n_minimum_params != b.n_minimum_params)
            return a.n_minimum_params < b.n_minimum_params;
        // Other rules to break ties
        if(a.ruletype != b.ruletype)
            return a.ruletype < b.ruletype;
        if(a.match_tree.match_type != b.match_tree.match_type)
            return a.match_tree.match_type < b.match_tree.match_type;
        if(a.match_tree.param_list != b.match_tree.param_list)
            return a.match_tree.param_list < b.match_tree.param_list;
        if(a.match_tree.param_count != b.match_tree.param_count)
            return a.match_tree.param_count < b.match_tree.param_count;
        if(a.repl_param_list != b.repl_param_list)
            return a.repl_param_list < b.repl_param_list;
        if(a.repl_param_count != b.repl_param_count)
            return a.repl_param_count < b.repl_param_count;
        return false;
    }
};

namespace GrammarData
{
    class ParamSpec;

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

        void RecursivelySetParamMatchingType(ParamMatchingType t);
        bool EnsureNoRepeatedNamedHolders(std::set<unsigned>& used) const;
        bool EnsureNoRepeatedNamedHolders() const;
        bool EnsureNoVariableCoverageParams_InPositionalParamLists();
        bool EnsureNoRepeatedRestHolders();
        bool EnsureNoRepeatedRestHolders(std::set<unsigned>& used);

        size_t CalcRequiredParamsCount() const;

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
        unsigned DepMask;

        SpecialOpcode Opcode;      // specifies the type of the function
        union
        {
            double ConstantValue;           // for NumConstant
            unsigned Index;                 // for ImmedHolder, RestHolder, NamedHolder
            FunctionType* Func;             // for SubFunction
            OPCODE GroupFuncOpcode;         // for GroupFunction
        };
        unsigned ImmedConstraint;
        std::vector<ParamSpec*> Params;

    public:
        struct NamedHolderTag{};
        struct ImmedHolderTag{};
        struct RestHolderTag{};

        ParamSpec(FunctionType* f)
            : DepMask(),
              Opcode(SubFunction), Func(f),          ImmedConstraint(0), Params()
              {
              }

        ParamSpec(double d)
            : DepMask(),
              Opcode(NumConstant), ConstantValue(d), ImmedConstraint(0), Params() { }

        ParamSpec(OPCODE o, const std::vector<ParamSpec*>& p)
            : DepMask(),
              Opcode(GroupFunction),    ImmedConstraint(0), Params(p)
        {
            GroupFuncOpcode = o;
        }

        ParamSpec(unsigned i, NamedHolderTag)
            : DepMask(),
              Opcode(NamedHolder), Index(i),         ImmedConstraint(0), Params() { }

        ParamSpec(unsigned i, ImmedHolderTag)
            : DepMask(),
              Opcode(ImmedHolder), Index(i),         ImmedConstraint(0), Params() { }

        ParamSpec(unsigned i, RestHolderTag)
            : DepMask(),
              Opcode(RestHolder),  Index(i),         ImmedConstraint(0), Params() { }

        ParamSpec* SetConstraint(unsigned mask)
            { ImmedConstraint |= mask; return this; }

        unsigned BuildDepMask();

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
                case NamedHolder: return false;
                case RestHolder: return false; // <1> is not constant
                case SubFunction: return false; // subfunctions are not constant
                case GroupFunction: break;
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

        void BuildFinalDepMask()
        {
            Input.Params.BuildFinalDepMask();
            //Replacement.BuildFinalDepMask(); -- not needed, though not wrong either.
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

    void MatchedParams::RecursivelySetParamMatchingType(ParamMatchingType t)
    {
        for(size_t a=0; a<Params.size(); ++a)
            Params[a]->RecursivelySetParamMatchingType(t);
    }

    bool MatchedParams::EnsureNoRepeatedNamedHolders(std::set<unsigned>& used) const
    {
        for(size_t a=0; a<Params.size(); ++a)
        {
            if(Params[a]->Opcode == NamedHolder)
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

            if(Params[a]->Opcode == SubFunction)
                if(!Params[a]->Func->Params.EnsureNoVariableCoverageParams_InPositionalParamLists())
                    return false;
        }
        return true;
    }
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
    size_t MatchedParams::CalcRequiredParamsCount() const
    {
        size_t res = 0;
        for(size_t a=0; a<Params.size(); ++a)
        {
            if(Params[a]->Opcode == RestHolder)
                continue; // Completely optional
            res += 1;
        }
        return res;
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
    std::map<std::string, size_t> n_index;

    std::vector<std::string>     nlist;
    std::vector<Rule>            rlist;
    std::vector<Grammar>         glist;
public:
    GrammarDumper():
        n_index(),
        nlist(),rlist(),glist()
    {
        plist.reserve(16384);
        nlist.reserve(16);
        rlist.reserve(16384);
        glist.reserve(16);
    }

    size_t ConvertNamedHolderNameIntoIndex(const std::string& n)
    {
        std::map<std::string, size_t>::const_iterator i = n_index.find(n);
        if(i != n_index.end()) return i->second;
        nlist.push_back(n);
        return n_index[n] = nlist.size()-1;
    }
    size_t GetNumNamedHolderNames() const { return nlist.size(); }

    void DumpParamList(const std::vector<GrammarData::ParamSpec*>& Params,
                       unsigned&       param_count,
                       unsigned&       param_list)
    {
        param_count = Params.size();
        param_list  = 0;
        ParamSpec pp[3];
        for(unsigned a=0; a<param_count; ++a)
            pp[a] = CreateParam(*Params[a]);
        for(unsigned a=0; a<param_count; ++a)
        {
            ParamSpec p = pp[a];

            unsigned paramno = plist.size();

            for(size_t b = 0; b < plist.size(); ++b)
                if(plist[b].first == p.first
                && ParamSpec_Compare(plist[b].second, p.second, p.first))
                {
                    paramno = b;
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
                return std::make_pair(SubFunction, (void*)result);
            }
            case NumConstant:
            {
                ParamSpec_NumConstant* result = new ParamSpec_NumConstant;
                result->constvalue     = p.ConstantValue;
                return std::make_pair(NumConstant, (void*)result);
            }
            case NamedHolder:
            {
                ParamSpec_NamedHolder* result = new ParamSpec_NamedHolder;
                result->constraints    = p.ImmedConstraint;
                result->index          = p.Index;
                result->depcode        = p.DepMask;
                return std::make_pair(NamedHolder, (void*)result);
            }
            case ImmedHolder:
            {
                ParamSpec_ImmedHolder* result = new ParamSpec_ImmedHolder;
                result->constraints    = p.ImmedConstraint;
                result->index          = p.Index;
                result->depcode        = p.DepMask;
                return std::make_pair(ImmedHolder, (void*)result);
            }
            case RestHolder:
            {
                ParamSpec_RestHolder* result = new ParamSpec_RestHolder;
                result->index          = p.Index;
                return std::make_pair(RestHolder, (void*)result);
            }
            default:
            {
                ParamSpec_GroupFunction* result = new ParamSpec_GroupFunction;
                result->constraints    = p.ImmedConstraint;
                result->subfunc_opcode = p.GroupFuncOpcode;
                DumpParamList(p.Params, pcount, plist);
                result->param_count = pcount;
                result->param_list  = plist;
                result->param_count = pcount;
                result->depcode        = p.DepMask;
                return std::make_pair(GroupFunction, (void*)result);
            }
        }
        std::cout << "???\n";
        return std::make_pair(GroupFunction, (void*) 0);
    }

    Rule CreateRule(const GrammarData::Rule& r)
    {
        size_t min_params = r.Input.Params.CalcRequiredParamsCount();

        Rule ritem;
        memset(&ritem, 0, sizeof(ritem));
        ritem.n_minimum_params          = min_params;
        ritem.ruletype                  = r.Type;
        ritem.match_tree.subfunc_opcode = r.Input.Opcode;
        ritem.match_tree.match_type     = r.Input.Params.Type;
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
    void DumpGrammar(const GrammarData::Grammar& g)
    {
        Grammar gitem;
        gitem.rule_count = 0;
        for(size_t a=0; a<g.rules.size(); ++a)
        {
            if(g.rules[a].Input.Opcode == cNop) continue;
            rlist.push_back( CreateRule(g.rules[a]) );
            ++gitem.rule_count;
        }
        std::sort(rlist.end() - gitem.rule_count,
                  rlist.end(),
                  RuleComparer());

        gitem.rule_begin = &rlist[rlist.size() - gitem.rule_count];

        glist.push_back(gitem);
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
        if(!*sep) result << "0";
        return result.str();
    }

    static std::string ConstValueToString(double value)
    {
        std::ostringstream result;
        result.precision(50);
        #define if_const(n) \
            if(FloatEqual(value, n)) result << #n;
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
        std::vector<ParamSpec_ImmedHolder>   plist_i;
        std::vector<ParamSpec_NumConstant>   plist_n;
        std::vector<ParamSpec_NamedHolder>   plist_a;
        std::vector<ParamSpec_RestHolder>    plist_r;
        std::vector<ParamSpec_SubFunction>   plist_s;
        std::vector<ParamSpec_GroupFunction> plist_g;

        const ParamSpec& DecodeParam(unsigned list, unsigned index) const
        {
            unsigned plist_index = (list >> (index*PARAM_INDEX_BITS))
                                   % (1 << PARAM_INDEX_BITS);
            return plist[plist_index];
        }
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
                set(ImmedHolder, plist_i, );
                set(NumConstant, plist_n, );
                set(NamedHolder, plist_a, );
                set(RestHolder,  plist_r, );
                set(SubFunction, plist_s,
                     ParamSpec_SubFunction* p = (ParamSpec_SubFunction*)param.second;
                     for(size_t a=0; a<p->data.param_count; ++a)
                         Populate( DecodeParam( p->data.param_list, a) );
                    );
                set(GroupFunction, plist_g,
                     ParamSpec_GroupFunction* p = (ParamSpec_GroupFunction*)param.second;
                     for(size_t a=0; a<p->param_count; ++a)
                         Populate( DecodeParam( p->param_list, a) );
                    );
            }
            #undef set
        }

        std::string ParamPtrToString(unsigned paramlist, unsigned index) const
        {
            const ParamSpec& p = DecodeParam(paramlist, index);
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
                set(ImmedHolder, plist_i, I);
                set(NumConstant, plist_n, N);
                set(NamedHolder, plist_a, A);
                set(RestHolder,  plist_r, R);
                set(SubFunction, plist_s, S);
                set(GroupFunction, plist_g, G);
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

        std::string ImmedHolderToString(const ParamSpec_ImmedHolder& i) const
        {
            std::ostringstream result;
            result << "{" << i.index
                   << ", 0x" << i.depcode
                   << ", " << ConstraintsToString(i.constraints)
                   << "}";
            return result.str();
        }

        std::string NamedHolderToString(const ParamSpec_NamedHolder& i) const
        {
            std::ostringstream result;
            result << "{" << i.index
                   << ", 0x" << i.depcode
                   << ", " << ConstraintsToString(i.constraints)
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

        std::string RestHolderToString(const ParamSpec_RestHolder& i) const
        {
            std::ostringstream result;
            result << "{" << i.index
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
                            :/*i.match_type == AnyParams      ?*/ "AnyParams       "
                            )
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

        std::string GroupFunctionToString(const ParamSpec_GroupFunction& i) const
        {
            std::ostringstream result;
            result << "{" << i.param_count
                   <<  "," << ParamListToString(i.param_list, i.param_count)
                   << ", " << FP_GetOpcodeName(i.subfunc_opcode, true)
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
                collection.Populate( collection.DecodeParam(rlist[a].match_tree.param_list, b) );
            for(unsigned b=0; b < rlist[a].repl_param_count; ++b)
                collection.Populate( collection.DecodeParam(rlist[a].repl_param_list, b) );
        }

        std::cout <<
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
                    << ",\n"; \
            } \
            buf << \
            "        },\n" \
            "\n";

        set(ImmedHolder,   plist_i, I)
        set(NumConstant,   plist_n, N)
        set(NamedHolder,   plist_a, A)
        set(RestHolder,    plist_r, R)
        set(SubFunction,   plist_s, S)
        set(GroupFunction, plist_g, G)

        std::cout <<
            "    } /*PACKED_GRAMMAR_ATTRIBUTE*/ plist =\n"
            "    {\n"
            << buf.str() <<
            "    };\n";
        }

        #undef set

        std::cout <<
            "    const Rule rlist[" << rlist.size() << "] =\n"
            "    {\n";
        for(size_t a=0; a<rlist.size(); ++a)
        {
            if(a > 0)
                for(size_t g=0; g<glist.size(); ++g)
                    if(glist[g].rule_begin == &rlist[a])
                        std::cout <<
                        "        /*********************/\n";

            std::cout <<
            "        /* " << a << "\t*/ "
                        << "{"
                        << (rlist[a].ruletype == ProduceNewTree  ? "ProduceNewTree"
                         :/*rlist[a].ruletype == ReplaceParams ?*/ "ReplaceParams "
                           )
                        << ", " << rlist[a].repl_param_count
                        <<  "," << collection.ParamListToString(rlist[a].repl_param_list, rlist[a].repl_param_count)
                        << ", " << collection.SubFunctionDataToString(rlist[a].match_tree)
                        << ", " << rlist[a].n_minimum_params
                        << "},\n";
        }
        std::cout <<
            "    };\n"
            "}\n"
            "\n"
            "namespace FPoptimizer_Grammar\n"
            "{\n"
            "    const GrammarPack pack =\n"
            "    {\n"
            "        {\n";
        for(size_t a=0; a<glist.size(); ++a)
        {
            std::cout <<
            "            /* " << a << " */\t"
                      << "{ &rlist[" << (glist[a].rule_begin - &rlist[0]) << "]"
                        << ", " << glist[a].rule_count
                        << " },\n";
        }
        std::cout <<
            "        }\n"
            "    };\n"
            "}\n"
            << undef_buf.str();
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

%token SUBST_OP_COLON /* '->' */
%token SUBST_OP_ARROW /* ':'  */

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
               yyerror("Restholders such as <1> or ~<2>, must not occur in bracketed param lists on the matching side"); YYERROR;
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
/**/
         if(!$3->EnsureNoRepeatedRestHolders())
         {
             yyerror("RestHolders such as <1> or ~<2> must not be repeated in a rule; make matching too difficult"); YYERROR;
         }
/**/
         $$ = new GrammarData::FunctionType($1, *$3);
         delete $3;
       }
    |  OPCODE '{' paramlist '}'
       /* Match a function with opcode=opcode,
        * and the exact parameter list in any order
        */
       {
/**/
         if(!$3->EnsureNoRepeatedRestHolders())
         {
             yyerror("RestHolders such as <1> or ~<2> must not be repeated in a rule; make matching too difficult"); YYERROR;
         }
/**/
         $$ = new GrammarData::FunctionType($1, *$3->SetType(SelectedParams));
         delete $3;
       }
    |  OPCODE paramlist
       /* Match a function with opcode=opcode and the given way of matching params */
       /* There may be more parameters, don't care about them */
       {
/**/
         if(!$2->EnsureNoRepeatedRestHolders())
         {
             yyerror("RestHolders such as <1> or ~<2> must not be repeated in a rule; make matching too difficult"); YYERROR;
         }
/**/
         $$ = new GrammarData::FunctionType($1, *$2->SetType(AnyParams));
         delete $2;
       }
    ;

    paramlist: /* left-recursive list of 0-n params with no delimiter */
        paramlist param    /* param */
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
    |  IMMEDHOLDER_TOKEN param_constraints  /* a placeholder for some immed */
       {
         $$ = new GrammarData::ParamSpec($1, GrammarData::ParamSpec::ImmedHolderTag());
         $$->SetConstraint($2);
       }
    |  BUILTIN_FUNC_NAME '(' paramlist ')'  /* literal logarithm/sin/etc. of the provided immed-type params -- also sum/product/minimum/maximum */
       {
         /* Verify that $3 consists of constants */
         $$ = new GrammarData::ParamSpec($1, $3->GetParams() );
         if(!$$->VerifyIsConstant())
         {
             yyerror("Not constant"); YYERROR;
         }
         delete $3;
       }
    |  NAMEDHOLDER_TOKEN param_constraints /* any expression, indicated by "x", "a" etc. */
       {
         $$ = new GrammarData::ParamSpec($1, GrammarData::ParamSpec::NamedHolderTag());
         $$->SetConstraint($2);
       }
    |  '(' function ')' param_constraints    /* a subtree */
       {
         $$ = new GrammarData::ParamSpec($2);
         $$->SetConstraint($4);
       }
    |  RESTHOLDER_TOKEN        /* a placeholder for all remaining params of given polarity */
       {
         $$ = new GrammarData::ParamSpec($1, GrammarData::ParamSpec::RestHolderTag());
       }
    |  UNARY_TRANSFORMATION param   /* the negated/inverted literal value of the param */
       {
         /* Verify that $2 is constant */
         if(!$2->VerifyIsConstant())
         {
             yyerror("Not constant"); YYERROR;
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
            lval->index = dumper.ConvertNamedHolderNameIntoIndex(IdBuf);
            // fprintf(stderr, "'%s' interpreted as PARAM\n", IdBuf.c_str());

            return NAMEDHOLDER_TOKEN;
        }
        default:
        {
            fprintf(stderr, "Ignoring unidentifier character '%c'\n", c);
            return yylex(lval); // tail recursion
        }
    }
    return EOF;
}

unsigned GrammarData::ParamSpec::BuildDepMask()
{
    const unsigned NamedHolderShift = 0;
    const unsigned ImmedHolderShift = dumper.GetNumNamedHolderNames();
    DepMask = 0;
    switch(Opcode)
    {
        case NamedHolder:
            DepMask |= 1 << (Index + NamedHolderShift);
            break;
        case ImmedHolder:
            DepMask |= 1 << (Index + ImmedHolderShift);
            break;
        case SubFunction:
            DepMask = Func->Params.BuildDepMask();
            break;
        case GroupFunction:
            for(size_t a=0; a<Params.size(); ++a)
                DepMask |= Params[a]->BuildDepMask();
            break;
        default: break;
    }
    return DepMask;
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

    Grammar_Basic.BuildFinalDepMask();
    Grammar_Entry.BuildFinalDepMask();
    Grammar_Intermediate.BuildFinalDepMask();
    Grammar_Final1.BuildFinalDepMask();
    Grammar_Final2.BuildFinalDepMask();

    Grammar_Intermediate.rules.insert(
       Grammar_Intermediate.rules.end(),
       Grammar_Basic.rules.begin(),
       Grammar_Basic.rules.end());

    Grammar_Final1.rules.insert(
       Grammar_Final1.rules.end(),
       Grammar_Basic.rules.begin(),
       Grammar_Basic.rules.end());

    std::cout <<
        "/* This file is automatically generated. Do not edit... */\n"
        "#include \"fpoptimizer_grammar.hh\"\n"
        "#include \"fpoptimizer_consts.hh\"\n"
        "#include \"fpconfig.hh\"\n"
        "#include \"fptypes.hh\"\n"
        "#include <algorithm>\n"
        "\n"
        "using namespace FPoptimizer_Grammar;\n"
        "using namespace FUNCTIONPARSERTYPES;\n"
        "\n";

    std::cout <<
        "#define P1(a) a\n"
        "#define P2(a,b) (P1(a) | (b << PARAM_INDEX_BITS))\n"
        "#define P3(a,b,c) (P2(a,b) | (c << (PARAM_INDEX_BITS*2)))\n"
        "\n";

    /*size_t e = */dumper.DumpGrammar(Grammar_Entry);
    /*size_t i = */dumper.DumpGrammar(Grammar_Intermediate);
    /*size_t f = */dumper.DumpGrammar(Grammar_Final1);
    /*size_t f = */dumper.DumpGrammar(Grammar_Final2);

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
        "        const unsigned i_begin = 0;\n"
        "        const unsigned n_begin = i_begin + sizeof(plist.plist_i)/sizeof(*plist.plist_i);\n"
        "        const unsigned a_begin = n_begin + sizeof(plist.plist_n)/sizeof(*plist.plist_n);\n"
        "        const unsigned r_begin = a_begin + sizeof(plist.plist_a)/sizeof(*plist.plist_a);\n"
        "        const unsigned s_begin = r_begin + sizeof(plist.plist_r)/sizeof(*plist.plist_r);\n"
        "        const unsigned g_begin = s_begin + sizeof(plist.plist_s)/sizeof(*plist.plist_s);\n"
        "      /*const unsigned     end = g_begin + sizeof(plist.plist_g)/sizeof(*plist.plist_g);*/\n"
        "        if(index < r_begin)\n"
        "        {\n"
        "            if(index < n_begin)\n"
        "                return ParamSpec(ImmedHolder,(const void*)&plist.plist_i[index-i_begin]);\n"
        "            else if(index < a_begin)\n"
        "                return ParamSpec(NumConstant,(const void*)&plist.plist_n[index-n_begin]);\n"
        "            else\n"
        "                return ParamSpec(NamedHolder,(const void*)&plist.plist_a[index-a_begin]);\n"
        "        }\n"
        "        else if(index < s_begin)\n"
        "            return ParamSpec(RestHolder,(const void*)&plist.plist_r[index-r_begin]);\n"
        "        else if(index < g_begin)\n"
        "            return ParamSpec(SubFunction,(const void*)&plist.plist_s[index-s_begin]);\n"
        "        else\n"
        "            return ParamSpec(GroupFunction,(const void*)&plist.plist_g[index-g_begin]);\n"
        "    }\n"
        "}\n";

    return 0;
}
