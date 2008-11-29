#include <string>
#include <vector>

namespace FPoptimizer_Grammar
{
    typedef unsigned OpcodeType;

    class FunctionType;
    struct GrammarPack;

    class ParamSpec
    {
    public:
        bool Negated;    // true means for: cAdd:-x; cMul:1/x; cAnd/cOr: !x; other: invalid

        enum TransformationType
        {
            None,    // default
            Negate,  // 0-x
            Invert   // 1/x
        } Transformation;

        unsigned MinimumRepeat; // default 1
        bool AnyRepetition;     // false: max=minimum; true: max=infinite
        
        enum SpecialOpcode
        {
            NumConstant = 0xFFFB, // Holds a particular value (syntax-time constant)
            ImmedHolder,          // Holds a particular immed
            NamedHolder,          // Holds a particular named param (of any kind)
            RestHolder,           // Holds anything else
            Function              // Holds an opcode and the params
          //GroupFunction         // For parse-time functions
        };
        
        OpcodeType Opcode;      // specifies the type of the function
        union
        {
            double ConstantValue;           // for NumConstant
            unsigned Index;                 // for ImmedHolder, RestHolder
            FunctionType* Func;             // for Function
        };
        std::vector<ParamSpec*> Params; // EvalValues thereof are used for calculation
                                        // when GroupFunction is used
        std::string Name;               // for NamedHolder
    protected:
        // From parser, not from grammar:
        double EvalValue;// this is the value used for arithmetics
    
    public:
        ParamSpec(FunctionType* f);      // Function
        ParamSpec(double d);             // NumConstant
        ParamSpec(const std::string& n); // NamedHolder
        ParamSpec(OpcodeType o, const std::vector<ParamSpec*>& p); // GroupFunction
        ParamSpec(unsigned i, double); // ImmedHolder
        ParamSpec(unsigned i, void*);  // RestHolder
        
        ParamSpec* SetNegated()
            { Negated=true; return this; }
        ParamSpec* SetRepeat(unsigned min, bool any)
            { MinimumRepeat=min; AnyRepetition=any; return this; }

        ParamSpec(const GrammarPack& pack, size_t offs);
        
        bool operator== (const ParamSpec& b) const;
        bool operator< (const ParamSpec& b) const;
        
    private:
        ParamSpec(const ParamSpec&);
        ParamSpec& operator= (const ParamSpec&);
    };
    
    class MatchedParams
    {
    public:
        enum TypeType
        {
            PositionalParams,
            AnyParams
        };
    public:
        TypeType Type;

        std::vector<ParamSpec*> Params;
    public:
        MatchedParams()             : Type(), Params() { }
        MatchedParams(TypeType t)   : Type(t), Params() { }
        MatchedParams(ParamSpec* p) : Type(), Params() { Params.push_back(p); }
        
        void SetType(TypeType t) { Type=t; }
        void AddParam(ParamSpec* p) { Params.push_back(p); }
        
        const std::vector<ParamSpec*>& GetParams() const { return Params; }

        MatchedParams(const GrammarPack& pack, size_t offs);
        
        bool operator== (const MatchedParams& b) const;
        bool operator< (const MatchedParams& b) const;

    };

    class FunctionType
    {
    public:
        OpcodeType    Opcode;
        MatchedParams Params;
    public:
        FunctionType(OpcodeType o, const MatchedParams& p)
            : Opcode(o), Params(p) { }

        FunctionType(const GrammarPack& pack, size_t offs);
        
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
        enum TypeType
        {
            ProduceNewTree, // replace self with the first (and only) from replaced_param
            ReplaceParams   // replace indicate params with replaced_params
        };
    public:
        friend class GrammarDumper;
        TypeType Type;

        FunctionType  Input;
        MatchedParams Replacement; // length should be 1 if ProduceNewTree is used
    public:
        Rule(TypeType t, const FunctionType& f, const MatchedParams& r)
            : Type(t), Input(f), Replacement(r) { }
        Rule(TypeType t, const FunctionType& f, ParamSpec* p)
            : Type(t), Input(f), Replacement() { Replacement.AddParam(p); }

        Rule(const GrammarPack& pack, size_t offs);
        
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
        void Read(const GrammarPack& pack, size_t offs);
    };

    #ifdef __GNUC__
    # define GRAMMAR_PACK_STRUCT __attribute__((packed))
    #else
    # define GRAMMAR_PACK_STRUCT
    #endif
    
    /* These are the versions of those above classes, that can be
     * statically initialized (used in fpoptimizer_grammar_init.cc,
     * which is generated by fpoptimizer_grammar_gen,
     * which is generated by bison++ from fpoptimizer_grammar_gen.y.
     */
    
    struct ParamSpec_Const
    {
        bool     negated : 1;
        ParamSpec::TransformationType
           transformation : 3;
        unsigned minrepeat : 3;
        bool     anyrepeat : 1;
        OpcodeType opcode : 16;
        unsigned count : 8;
        unsigned index : 16;
    } GRAMMAR_PACK_STRUCT;
    struct MatchedParams_Const
    {
        MatchedParams::TypeType type : 8;
        unsigned count : 8;
        unsigned index : 16;
    } GRAMMAR_PACK_STRUCT;
    struct FunctionType_Const
    {
        OpcodeType opcode : 16;
        unsigned   index  : 16;
    } GRAMMAR_PACK_STRUCT;
    struct RuleType_Const
    {
        Rule::TypeType   type        : 4;
        unsigned         input_index : 14;
        unsigned         repl_index  : 14;
    } GRAMMAR_PACK_STRUCT;
    struct Grammar_Const
    {
        unsigned index : 8;
        unsigned count : 8;
    } GRAMMAR_PACK_STRUCT;
    
    #undef GRAMMAR_PACK_STRUCT
    
    struct GrammarPack
    {
        const char* const*          nlist;
        const double*               clist;
        const ParamSpec_Const*      plist;
        const MatchedParams_Const*  mlist;
        const FunctionType_Const*   flist;
        const RuleType_Const*       rlist;
        const Grammar_Const*        glist;
    };
}

extern FPoptimizer_Grammar::Grammar  Grammar_Entry;
extern FPoptimizer_Grammar::Grammar  Grammar_Intermediate;
extern FPoptimizer_Grammar::Grammar  Grammar_Final;
extern void FPoptimizer_Grammar_Init();
