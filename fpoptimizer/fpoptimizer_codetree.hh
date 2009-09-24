#ifndef FPOptimizer_CodeTreeHH
#define FPOptimizer_CodeTreeHH

#include <vector>
#include <utility>

#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#include "fpoptimizer_hash.hh"
#include "fpoptimizer_autoptr.hh"

#ifdef FP_EPSILON
 #define NEGATIVE_MAXIMUM (-FP_EPSILON)
#else
 #define NEGATIVE_MAXIMUM (-1e-14)
#endif

namespace FPoptimizer_Grammar
{
    class Grammar;
}

namespace FPoptimizer_ByteCode
{
    class ByteCodeSynth;
}

namespace FPoptimizer_CodeTree
{
    class CodeTreeParserData;
    class CodeTree;

    typedef FPOPT_autoptr<CodeTree> CodeTreeP;

    class CodeTree
    {
        friend class CodeTreeParserData;
        friend class FPOPT_autoptr<CodeTree>;

        int RefCount;

    public:
        /* Describing the codetree node */
        unsigned Opcode;
        union
        {
            double   Value;   // In case of cImmed: value of the immed
            unsigned Var;     // In case of cVar:   variable number
            unsigned Funcno;  // In case of cFCall or cPCall
        };

        // Parameters for the function
        //  These use the sign:
        //   For cAdd: operands to add together (0 to n)
        //             sign indicates that the value is negated before adding (0-x)
        //   For cMul: operands to multiply together (0 to n)
        //             sign indicates that the value is inverted before multiplying (1/x)
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
        std::vector<CodeTreeP> Params;

        /* Internal operation */
        FUNCTIONPARSERTYPES::fphash_t      Hash;
        size_t        Depth;
        CodeTree*     Parent;
        const FPoptimizer_Grammar::Grammar* OptimizedUsing;
    public:
        CodeTree();
        ~CodeTree();

        explicit CodeTree(double v); // produce an immed

        /* Generates a CodeTree from the given bytecode */
        static CodeTreeP GenerateFrom(
            const std::vector<unsigned>& byteCode,
            const std::vector<double>& immed,
            const FunctionParser::Data& data);

        void SynthesizeByteCode(
            std::vector<unsigned>& byteCode,
            std::vector<double>&   immed,
            size_t& stacktop_max);
        void SynthesizeByteCode(FPoptimizer_ByteCode::ByteCodeSynth& synth);

        /* Regenerates the hash.
         * child_triggered=false: Recurse to children
         * child_triggered=true:  Recurse to parents
         */
        void Rehash(bool child_triggered);
        void Rehash_Parents();
        void Recalculate_Hash_NoRecursion();

        void Sort();
        void Sort_Recursive();

        void SetParams(const std::vector<CodeTreeP>& RefParams, bool do_clone = true);
        void SetParamsMove(std::vector<CodeTreeP>& RefParams);
#ifdef __GXX_EXPERIMENTAL_CXX0X__
        void SetParams(std::vector<CodeTreeP>&& RefParams);
#endif
        void AddParam(const CodeTreeP& param);
        void AddParamMove(CodeTreeP& param);
        void DelParam(size_t index);

        /* Clones the tree. (For parameter duplication) */
        CodeTreeP Clone();
        void Become(CodeTree& b, bool thrash_original, bool do_clone = true);

        bool    IsImmed() const;
        double GetImmed() const { return Value; }
        bool    IsLongIntegerImmed() const { return IsImmed() && GetImmed() == (double)GetLongIntegerImmed(); }
        long   GetLongIntegerImmed() const { return (long)GetImmed(); }
        bool      IsVar() const;
        unsigned GetVar() const { return Var; }
        bool    IsLogicalValue() const;

        void NegateImmed() { if(IsImmed()) Value = -Value;       }
        void InvertImmed() { if(IsImmed()) Value = 1.0 / Value;  }
        void NotTheImmed() { if(IsImmed()) Value = Value == 0.0; }

        struct MinMaxTree
        {
            double min,max;
            bool has_min, has_max;
            MinMaxTree() : min(),max(),has_min(false),has_max(false) { }
            MinMaxTree(double mi,double ma): min(mi),max(ma),has_min(true),has_max(true) { }
            MinMaxTree(bool,double ma): min(),max(ma),has_min(false),has_max(true) { }
            MinMaxTree(double mi,bool): min(mi),max(),has_min(true),has_max(false) { }
        };
        /* This function calculates the minimum and maximum values
         * of the tree's result. If an estimate cannot be made,
         * has_min/has_max are indicated as false.
         */
        MinMaxTree CalculateResultBoundaries_do() const;
        MinMaxTree CalculateResultBoundaries() const;

        enum TriTruthValue { IsAlways, IsNever, Unknown };
        TriTruthValue GetEvennessInfo() const;

        bool IsAlwaysSigned(bool positive) const;
        bool IsAlwaysParity(bool odd) const
            { return GetEvennessInfo() == (odd?IsNever:IsAlways); }
        bool IsAlwaysInteger() const;

        void ConstantFolding();
        void ConstantFolding_FromLogicalParent();
        bool ConstantFolding_AndLogic();
        bool ConstantFolding_OrLogic();
        bool ConstantFolding_LogicCommon(bool is_or);
        bool ConstantFolding_MulGrouping();
        bool ConstantFolding_AddGrouping();

        bool IsIdenticalTo(const CodeTree& b) const;

    private:
        CodeTree(const CodeTree&);
        CodeTree& operator=(const CodeTree&);
    };
}

#endif
