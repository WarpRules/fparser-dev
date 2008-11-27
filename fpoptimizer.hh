#include <stdint.h>

/*
 First read: fpoptimizer-plan.txt
*/

namespace FPoptimizer_Grammar
{
    typedef unsigned OpcodeType;

    class FunctionType;

    class ParamSpec
    {
    private:
        bool Negated;    // true means for: cAdd:-x; cMul:1/x; cAnd/cOr: !x; other: invalid

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
    };
    // Holds a particular immed:
    class ParamSpec_ImmedHolder: public ParamSpec
    {
    private:
        enum
        {
            None,    // default
            Negate,  // 0-x
            Invert   // 1/x
        } Transformation;
        unsigned Index;
    };
    // Holds a particular named param (of any kind):
    class ParamSpec_NamedHolder: public ParamSpec
    {
    private:
        unsigned MinimumRepeat; // default 1
        bool AnyRepetition;     // false: max=minimum; true: max=infinite
        std::string Name;
    };
    // Holds anything else
    class ParamSpec_RestHolder: public ParamSpec
    {
    private:
        unsigned Index;
    };
    // Holds an opcode and the params
    class ParamSpec_Function: public ParamSpec
    {
    private:
        FunctionType* func; // FIXME: has a pointer
    public:
    };
    // For parse-time functions:
    class ParamSpec_GroupFunction: public ParamSpec
    {
    private:
        OpcodeType Opcode;              // specifies the type of the function
        std::vector<ParamSpec*> Params; // EvalValues thereof are used for calculation
    };

    class MatchedParams
    {
    private:
        enum
        {
            PositionalParams
            AnyParams,
        } Type;

        std::vector<ParamSpec> Params;
    };

    class FunctionType
    {
    private:
        OpcodeType    Opcode;
        MatchedParams MatchedParams;
    };

    class Rule
    {
    private:
        enum
        {
            ProduceNewTree, // replace self with the first (and only) from replaced_param
            ReplaceParams   // replace indicate params with replaced_params
        } Type;

        FunctionType  Input;
        MatchedParams Replacement; // length should be 1 if ProduceNewTree is used
    };

    class Grammar
    {
    private:
        std::vector<Rule> rules;
    };
}

namespace FPoptimizer_CodeTree
{
    class CodeTree;

    class CodeTree
    {
    private:
        /* Describing the codetree node */
        unsigned Opcode;
        union
        {
            double   Value;   // In case of cImmed: value of the immed
            unsigned Var;     // In case of cVar:   variable number
            unsigned Funcno;  // In case of cFCall or cPCall
        };
        struct Param
        {
            CodeTree* param; // param node
            bool      sign;  // true = negated or inverted
        };
        // Parameters for the function
        //  These use the sign:
        //   For cAdd: operands to add together (0 to n)
        //             sign indicates that the value is negated before adding (0-x)
        //   For cMul: operands to multiply together (0 to n)
        //             sign indicates that the value is inverted before adding (1/x)
        //   For cAnd: operands to bitwise-and together (0 to n)
        //             sign indicates that the value is inverted before anding (!x)
        //   For cOr:  operands to bitwise-or together (0 to n)
        //             sign indicates that the value is inverted before orring (!x)
        //  These don't use the sign (sign is always false):
        //   For cMin: operands to select the minimum of
        //   For cMax: operands to select the maximum of
        //   For cImmed, not used
        //   For cVar,   not used
        //   For cIf:  operand 1 = condition, operand 2 = yes-branch, operand 3 = no-branch
        //   For anything else: the parameters required by the operation/function
        std::vector<Param> Params;
    private:
        /* Internal operation */
        uint_fast64_t Hash;
        CodeTree*     Parent;
    };
}
