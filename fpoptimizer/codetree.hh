#ifndef FPOptimizer_CodeTreeHH
#define FPOptimizer_CodeTreeHH

#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#ifdef FP_SUPPORT_OPTIMIZER

#include <vector>
#include <utility>

#include "hash.hh"
#include "../lib/autoptr.hh"

namespace FPoptimizer_Grammar
{
    struct Grammar;
}

namespace FPoptimizer_ByteCode
{
    template<typename Value_t>
    class ByteCodeSynth;
}

namespace FPoptimizer_CodeTree
{
    template<typename Value_t>
    class CodeTree;

    template<typename Value_t>
    struct MinMaxTree
    {
        Value_t min,max;
        bool has_min, has_max;
        MinMaxTree() : min(),max(),has_min(false),has_max(false) { }
        MinMaxTree(Value_t mi,Value_t ma): min(mi),max(ma),has_min(true),has_max(true) { }
        MinMaxTree(bool,Value_t ma): min(),max(ma),has_min(false),has_max(true) { }
        MinMaxTree(Value_t mi,bool): min(mi),max(),has_min(true),has_max(false) { }
    };

    template<typename Value_t>
    struct CodeTreeData;

    template<typename Value_t>
    class CodeTree
    {
        typedef FPOPT_autoptr<CodeTreeData<Value_t> > DataP;
        DataP data;

    public:
        CodeTree();
        ~CodeTree();

        explicit CodeTree(const Value_t& v); // produce an immed
        struct VarTag { };
        explicit CodeTree(unsigned varno, VarTag); // produce a var reference
        struct CloneTag { };
        explicit CodeTree(const CodeTree& b, CloneTag);

        /* Generates a CodeTree from the given bytecode */
        void GenerateFrom(
            const std::vector<unsigned>& byteCode,
            const std::vector<Value_t>& immed,
            const typename FunctionParserBase<Value_t>::Data& data,
            bool keep_powi = false);

        void GenerateFrom(
            const std::vector<unsigned>& byteCode,
            const std::vector<Value_t>& immed,
            const typename FunctionParserBase<Value_t>::Data& data,
            const std::vector<CodeTree>& var_trees,
            bool keep_powi = false);

        void SynthesizeByteCode(
            std::vector<unsigned>& byteCode,
            std::vector<Value_t>&   immed,
            size_t& stacktop_max);

        void SynthesizeByteCode(
            FPoptimizer_ByteCode::ByteCodeSynth<Value_t>& synth,
            bool MustPopTemps=true) const;

        size_t SynthCommonSubExpressions(
            FPoptimizer_ByteCode::ByteCodeSynth<Value_t>& synth) const;

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
        inline void SetImmed(Value_t v);
        inline FUNCTIONPARSERTYPES::OPCODE GetOpcode() const;
        inline FUNCTIONPARSERTYPES::fphash_t GetHash() const;
        inline const std::vector<CodeTree>& GetParams() const;
        inline std::vector<CodeTree>& GetParams();
        inline size_t GetDepth() const;
        inline const Value_t& GetImmed() const;
        inline unsigned GetVar() const;
        inline unsigned GetFuncNo() const;
        inline bool IsDefined() const { return GetOpcode() != FUNCTIONPARSERTYPES::cNop; }

        inline bool    IsImmed() const { return GetOpcode() == FUNCTIONPARSERTYPES::cImmed; }
        inline bool      IsVar() const { return GetOpcode() == FUNCTIONPARSERTYPES::VarBegin; }
        bool    IsLongIntegerImmed() const { return IsImmed() && GetImmed() == (Value_t)GetLongIntegerImmed(); }
        long   GetLongIntegerImmed() const { return (long)GetImmed(); }
        bool    IsLogicalValue() const;
        inline unsigned GetRefCount() const;
        /* This function calculates the minimum and maximum values
         * of the tree's result. If an estimate cannot be made,
         * has_min/has_max are indicated as false.
         */
        MinMaxTree<Value_t> CalculateResultBoundaries_do() const;
        MinMaxTree<Value_t> CalculateResultBoundaries() const;

        enum TriTruthValue { IsAlways, IsNever, Unknown };
        TriTruthValue GetEvennessInfo() const;

        bool IsAlwaysSigned(bool positive) const;
        bool IsAlwaysParity(bool odd) const
            { return GetEvennessInfo() == (odd?IsNever:IsAlways); }
        bool IsAlwaysInteger(bool integer) const;

        void ConstantFolding();
        bool ConstantFolding_AndLogic();
        bool ConstantFolding_OrLogic();
        bool ConstantFolding_MulLogicItems();
        bool ConstantFolding_AddLogicItems();
        bool ConstantFolding_IfOperations();
        bool ConstantFolding_PowOperations();
        bool ConstantFolding_ComparisonOperations();
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
        void FixIncompleteHashes();

        void swap(CodeTree& b) { data.swap(b.data); }
        bool IsIdenticalTo(const CodeTree& b) const;
        void CopyOnWrite();
    };

    template<typename Value_t>
    struct CodeTreeData
    {
        int RefCount;

        /* Describing the codetree node */
        FUNCTIONPARSERTYPES::OPCODE Opcode;
        union
        {
            Value_t   Value;   // In case of cImmed:   value of the immed
            unsigned Var;     // In case of VarBegin: variable number
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
        //   For VarBegin, not used
        //   For cIf:  operand 1 = condition, operand 2 = yes-branch, operand 3 = no-branch
        //   For anything else: the parameters required by the operation/function
        std::vector<CodeTree<Value_t> > Params;

        /* Internal operation */
        FUNCTIONPARSERTYPES::fphash_t      Hash;
        size_t        Depth;
        const FPoptimizer_Grammar::Grammar* OptimizedUsing;

        CodeTreeData();
        CodeTreeData(const CodeTreeData& b);
        explicit CodeTreeData(const Value_t& i);
#ifdef __GXX_EXPERIMENTAL_CXX0X__
        CodeTreeData(CodeTreeData&& b);
#endif

        bool IsIdenticalTo(const CodeTreeData& b) const;
        void Sort();
        void Recalculate_Hash_NoRecursion();
    };

    template<typename Value_t>
    inline void CodeTree<Value_t>::SetOpcode(FUNCTIONPARSERTYPES::OPCODE o)
        { data->Opcode = o; }
    template<typename Value_t>
    inline void CodeTree<Value_t>::SetFuncOpcode(FUNCTIONPARSERTYPES::OPCODE o, unsigned f)
        { SetOpcode(o); data->Funcno = f; }
    template<typename Value_t>
    inline void CodeTree<Value_t>::SetVar(unsigned v)
        { SetOpcode(FUNCTIONPARSERTYPES::VarBegin); data->Var = v; }
    template<typename Value_t>
    inline void CodeTree<Value_t>::SetImmed(Value_t v)
        { SetOpcode(FUNCTIONPARSERTYPES::cImmed); data->Value = v; }
    template<typename Value_t>
    inline FUNCTIONPARSERTYPES::OPCODE CodeTree<Value_t>::GetOpcode() const { return data->Opcode; }
    template<typename Value_t>
    inline FUNCTIONPARSERTYPES::fphash_t CodeTree<Value_t>::GetHash() const { return data->Hash; }
    template<typename Value_t>
    inline const std::vector<CodeTree<Value_t> >& CodeTree<Value_t>::GetParams() const { return data->Params; }
    template<typename Value_t>
    inline std::vector<CodeTree<Value_t> >& CodeTree<Value_t>::GetParams() { return data->Params; }
    template<typename Value_t>
    inline size_t CodeTree<Value_t>::GetDepth() const { return data->Depth; }
    template<typename Value_t>
    inline const Value_t& CodeTree<Value_t>::GetImmed() const { return data->Value; }
    template<typename Value_t>
    inline unsigned CodeTree<Value_t>::GetVar() const { return data->Var; }
    template<typename Value_t>
    inline unsigned CodeTree<Value_t>::GetFuncNo() const { return data->Funcno; }

    template<typename Value_t>
    inline const FPoptimizer_Grammar::Grammar* CodeTree<Value_t>::GetOptimizedUsing() const
        { return data->OptimizedUsing; }
    template<typename Value_t>
    inline void CodeTree<Value_t>::SetOptimizedUsing(const FPoptimizer_Grammar::Grammar* g)
        { data->OptimizedUsing = g; }
    template<typename Value_t>
    inline unsigned CodeTree<Value_t>::GetRefCount() const { return data->RefCount; }

    template<typename Value_t>
    inline void CodeTree<Value_t>::Mark_Incompletely_Hashed() { data->Depth = 0; }
    template<typename Value_t>
    inline bool CodeTree<Value_t>::Is_Incompletely_Hashed() const { return data->Depth == 0; }

#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
    template<typename Value_t>
    void DumpHashes(const CodeTree<Value_t>& tree, std::ostream& o = std::cout);

    template<typename Value_t>
    void DumpTree(const CodeTree<Value_t>& tree, std::ostream& o = std::cout);

    template<typename Value_t>
    void DumpTreeWithIndent(const CodeTree<Value_t>& tree, std::ostream& o = std::cout, const std::string& indent = "\\");
#endif
}

#endif

#endif
