#include <string>
#include <vector>

namespace FPoptimizer_Grammar
{
    typedef unsigned OpcodeType;

    class FunctionType;

    class ParamSpec
    {
    public:
        bool Negated;    // true means for: cAdd:-x; cMul:1/x; cAnd/cOr: !x; other: invalid

        enum
        {
            None,    // default
            Negate,  // 0-x
            Invert   // 1/x
        } Transformation;

        unsigned MinimumRepeat; // default 1
        bool AnyRepetition;     // false: max=minimum; true: max=infinite
    private:
        // From parser, not from grammar:
        double EvalValue;// this is the value used for arithmetics

    public:
        virtual ~ParamSpec() { }
    };
    // Holds a particular value (syntax-time constant):
    class ParamSpec_NumConstant: public ParamSpec
    {
    private:
        double ConstantValue;
    public:
        ParamSpec_NumConstant(double c) : ParamSpec(), ConstantValue(c) { }
    };
    // Holds a particular immed:
    class ParamSpec_ImmedHolder: public ParamSpec
    {
    private:
        unsigned Index;
    public:
        ParamSpec_ImmedHolder(unsigned i) : ParamSpec(), Index(i) { }
    };
    // Holds a particular named param (of any kind):
    class ParamSpec_NamedHolder: public ParamSpec
    {
    private:
        std::string Name;
    public:
        ParamSpec_NamedHolder(const std::string& n) : ParamSpec(), Name(n) { }
    };
    // Holds anything else
    class ParamSpec_RestHolder: public ParamSpec
    {
    private:
        unsigned Index;
    public:
        ParamSpec_RestHolder(unsigned i) : ParamSpec(), Index(i) { }
    };
    // Holds an opcode and the params
    class ParamSpec_Function: public ParamSpec
    {
    private:
        FunctionType* func; // FIXME: has a pointer
    public:
        ParamSpec_Function(FunctionType* f): ParamSpec(), func(f) { }
    };
    // For parse-time functions:
    class ParamSpec_GroupFunction: public ParamSpec
    {
    private:
        OpcodeType Opcode;              // specifies the type of the function
        std::vector<ParamSpec*> Params; // EvalValues thereof are used for calculation
    public:
        ParamSpec_GroupFunction(OpcodeType o,
                                const std::vector<ParamSpec*>& p) 
                               : ParamSpec(), Opcode(o), Params(p) { }
    };

    class MatchedParams
    {
    public:
        enum TypeType
        {
            PositionalParams,
            AnyParams
        };
    private:
        TypeType Type;

        std::vector<ParamSpec*> Params;
    public:
        MatchedParams()             : Type(), Params() { }
        MatchedParams(ParamSpec* p) : Type(), Params() { Params.push_back(p); }
        void SetType(TypeType t) { Type=t; }
        void AddParam(ParamSpec* p) { Params.push_back(p); }
        
        const std::vector<ParamSpec*>& GetParams() const { return Params; }
    };

    class FunctionType
    {
    private:
        OpcodeType    Opcode;
        MatchedParams Params;
    public:
        FunctionType(OpcodeType o, const MatchedParams& p)
            : Opcode(o), Params(p) { }
    };

    class Rule
    {
    public:
        enum TypeType
        {
            ProduceNewTree, // replace self with the first (and only) from replaced_param
            ReplaceParams   // replace indicate params with replaced_params
        };
    private:
        TypeType Type;

        FunctionType  Input;
        MatchedParams Replacement; // length should be 1 if ProduceNewTree is used
    public:
        Rule(TypeType t, const FunctionType& f, const MatchedParams& r)
            : Type(t), Input(f), Replacement(r) { }
    };

    class Grammar
    {
    private:
        std::vector<Rule> rules;
    public:
        void AddRule(const Rule& r) { rules.push_back(r); }
    };
}
