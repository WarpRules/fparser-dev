#ifndef FPOptimizer_CodeTreeHH
#define FPOptimizer_CodeTreeHH

#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#ifdef FP_SUPPORT_OPTIMIZER

#include <vector>
#include <utility>

#include "fpoptimizer_hash.hh"
#include "fpoptimizer_autoptr.hh"

#ifdef FP_EPSILON
 #define NEGATIVE_MAXIMUM (-FP_EPSILON)
#else
 #define NEGATIVE_MAXIMUM (-1e-14)
#endif

namespace FPoptimizer_Grammar
{
    struct Grammar;
}

namespace FPoptimizer_ByteCode
{
    class ByteCodeSynth;
}

namespace FPoptimizer_CodeTree
{
    class CodeTree;

    struct MinMaxTree
    {
        double min,max;
        bool has_min, has_max;
        MinMaxTree() : min(),max(),has_min(false),has_max(false) { }
        MinMaxTree(double mi,double ma): min(mi),max(ma),has_min(true),has_max(true) { }
        MinMaxTree(bool,double ma): min(),max(ma),has_min(false),has_max(true) { }
        MinMaxTree(double mi,bool): min(mi),max(),has_min(true),has_max(false) { }
    };

    struct CodeTreeData;
    class CodeTree
    {
        typedef FPOPT_autoptr<CodeTreeData> DataP;
        DataP data;

    public:
    public:
        CodeTree();
        ~CodeTree();

        explicit CodeTree(double v); // produce an immed
        struct VarTag { };
        explicit CodeTree(unsigned varno, VarTag); // produce a var reference
        struct CloneTag { };
        explicit CodeTree(const CodeTree& b, CloneTag);

        /* Generates a CodeTree from the given bytecode */
        void GenerateFrom(
            const std::vector<unsigned>& byteCode,
            const std::vector<double>& immed,
            const FunctionParser::Data& data,
            bool keep_powi = false);

        void SynthesizeByteCode(
            std::vector<unsigned>& byteCode,
            std::vector<double>&   immed,
            size_t& stacktop_max);
        void SynthesizeByteCode(FPoptimizer_ByteCode::ByteCodeSynth& synth) const;

        void SetParams(const std::vector<CodeTree>& RefParams);
        void SetParamsMove(std::vector<CodeTree>& RefParams);

        CodeTree GetUniqueRef();
        // ^use this when CodeTree tmp=x; tmp.CopyOnWrite(); does not do exactly what you want

#ifdef __GXX_EXPERIMENTAL_CXX0X__
        void SetParams(std::vector<CodeTree>&& RefParams);
#endif
        void SetParam(size_t which, const CodeTree& b);
        void SetParamMove(size_t which, CodeTree& b);
        void AddParam(const CodeTree& param);
        void AddParamMove(CodeTree& param);
        void AddParams(const std::vector<CodeTree>& RefParams);
        void AddParamsMove(std::vector<CodeTree>& RefParams);
        void AddParamsMove(std::vector<CodeTree>& RefParams, size_t replacing_slot);
        void DelParam(size_t index);
        void DelParams();

        void Become(const CodeTree& b);

        inline size_t GetParamCount() const { return GetParams().size(); }
        inline CodeTree& GetParam(size_t n) { return GetParams()[n]; }
        inline const CodeTree& GetParam(size_t n) const { return GetParams()[n]; }
        inline void SetOpcode(FUNCTIONPARSERTYPES::OPCODE o);
        inline void SetFuncOpcode(FUNCTIONPARSERTYPES::OPCODE o, unsigned f);
        inline void SetVar(unsigned v);
        inline void SetImmed(double v);
        inline FUNCTIONPARSERTYPES::OPCODE GetOpcode() const;
        inline FUNCTIONPARSERTYPES::fphash_t GetHash() const;
        inline const std::vector<CodeTree>& GetParams() const;
        inline std::vector<CodeTree>& GetParams();
        inline size_t GetDepth() const;
        inline double GetImmed() const;
        inline unsigned GetVar() const;
        inline unsigned GetFuncNo() const;
        inline bool IsDefined() const { return GetOpcode() != FUNCTIONPARSERTYPES::cNop; }

        inline bool    IsImmed() const { return GetOpcode() == FUNCTIONPARSERTYPES::cImmed; }
        inline bool      IsVar() const { return GetOpcode() == FUNCTIONPARSERTYPES::cVar; }
        bool    IsLongIntegerImmed() const { return IsImmed() && GetImmed() == (double)GetLongIntegerImmed(); }
        long   GetLongIntegerImmed() const { return (long)GetImmed(); }
        bool    IsLogicalValue() const;
        inline unsigned GetRefCount() const;
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
        bool ConstantFolding_MulLogicItems();
        bool ConstantFolding_AddLogicItems();
        template<typename CondType>
        bool ConstantFolding_LogicCommon(CondType cond_type, bool is_logical);
        bool ConstantFolding_MulGrouping();
        bool ConstantFolding_AddGrouping();
        bool ConstantFolding_Assimilate();

        void Rehash(bool constantfolding = true);
        inline void Mark_Incompletely_Hashed();
        inline bool Is_Incompletely_Hashed() const;

        inline const FPoptimizer_Grammar::Grammar* GetOptimizedUsing() const;
        inline void SetOptimizedUsing(const FPoptimizer_Grammar::Grammar* g);

        bool RecreateInversionsAndNegations(bool prefer_base2 = false);
        std::vector<CodeTree> FindCommonSubExpressions();
        void FixIncompleteHashes();

        void swap(CodeTree& b) { data.swap(b.data); }
        bool IsIdenticalTo(const CodeTree& b) const;
        void CopyOnWrite();
    };

    struct CodeTreeData
    {
        int RefCount;

        /* Describing the codetree node */
        FUNCTIONPARSERTYPES::OPCODE Opcode;
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
        std::vector<CodeTree> Params;

        /* Internal operation */
        FUNCTIONPARSERTYPES::fphash_t      Hash;
        size_t        Depth;
        const FPoptimizer_Grammar::Grammar* OptimizedUsing;

        CodeTreeData();
        CodeTreeData(const CodeTreeData& b);
        explicit CodeTreeData(double i);
#ifdef __GXX_EXPERIMENTAL_CXX0X__
        CodeTreeData(CodeTreeData&& b);
#endif

        bool IsIdenticalTo(const CodeTreeData& b) const;
        void Sort();
        void Recalculate_Hash_NoRecursion();
    };

    inline void CodeTree::SetOpcode(FUNCTIONPARSERTYPES::OPCODE o)
        { data->Opcode = o; }
    inline void CodeTree::SetFuncOpcode(FUNCTIONPARSERTYPES::OPCODE o, unsigned f)
        { SetOpcode(o); data->Funcno = f; }
    inline void CodeTree::SetVar(unsigned v)
        { SetOpcode(FUNCTIONPARSERTYPES::cVar); data->Var = v; }
    inline void CodeTree::SetImmed(double v)
        { SetOpcode(FUNCTIONPARSERTYPES::cImmed); data->Value = v; }
    inline FUNCTIONPARSERTYPES::OPCODE CodeTree::GetOpcode() const { return data->Opcode; }
    inline FUNCTIONPARSERTYPES::fphash_t CodeTree::GetHash() const { return data->Hash; }
    inline const std::vector<CodeTree>& CodeTree::GetParams() const { return data->Params; }
    inline std::vector<CodeTree>& CodeTree::GetParams() { return data->Params; }
    inline size_t CodeTree::GetDepth() const { return data->Depth; }
    inline double CodeTree::GetImmed() const { return data->Value; }
    inline unsigned CodeTree::GetVar() const { return data->Var; }
    inline unsigned CodeTree::GetFuncNo() const { return data->Funcno; }

    inline const FPoptimizer_Grammar::Grammar* CodeTree::GetOptimizedUsing() const
        { return data->OptimizedUsing; }
    inline void CodeTree::SetOptimizedUsing(const FPoptimizer_Grammar::Grammar* g)
        { data->OptimizedUsing = g; }
    inline unsigned CodeTree::GetRefCount() const { return data->RefCount; }

    inline void CodeTree::Mark_Incompletely_Hashed() { data->Depth = 0; }
    inline bool CodeTree::Is_Incompletely_Hashed() const { return data->Depth == 0; }

    void DumpHashes(const FPoptimizer_CodeTree::CodeTree& tree, std::ostream& o = std::cout);
    void DumpTree(const FPoptimizer_CodeTree::CodeTree& tree, std::ostream& o = std::cout);
    void DumpTreeWithIndent(const FPoptimizer_CodeTree::CodeTree& tree, std::ostream& o = std::cout, const std::string& indent = "\\");
}

#endif

#endif
