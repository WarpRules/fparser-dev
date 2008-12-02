#include <stdint.h>
#include <vector>

#include "fpconfig.hh"
#include "fparser.hh"

namespace FPoptimizer_CodeTree
{
    class CodeTreeParserData;
    class CodeTree;

    class CodeTreeP
    {
    public:
        CodeTreeP()                   : p(0)   { }
        CodeTreeP(CodeTree*        b) : p(b)   { Birth(); }
        CodeTreeP(const CodeTreeP& b) : p(&*b) { Birth(); }

        inline CodeTree& operator* () const { return *p; }
        inline CodeTree* operator->() const { return p; }

        CodeTreeP& operator= (CodeTree*        b) { Set(b); return *this; }
        CodeTreeP& operator= (const CodeTreeP& b) { Set(&*b); return *this; }

        ~CodeTreeP() { Forget(); }

    private:
        inline static void Have(CodeTree* p2);
        inline void Forget();
        inline void Birth();
        inline void Set(CodeTree* p2);
    private:
        CodeTree* p;
    };

    class CodeTree
    {
        friend class CodeTreeParserData;
        friend class CodeTreeP;

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
        struct Param
        {
            CodeTreeP param; // param node
            bool      sign;  // true = negated or inverted

            Param()                           : param(),  sign()  {}
            Param(CodeTree*        p, bool s) : param(p), sign(s) {}
            Param(const CodeTreeP& p, bool s) : param(p), sign(s) {}
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
        std::vector<Param> Params;

        /* Internal operation */
        uint_fast64_t Hash;
        size_t        Depth;
        CodeTree*     Parent;
    public:
        CodeTree();
        ~CodeTree();

        /* Generates a CodeTree from the given bytecode */
        static CodeTreeP GenerateFrom(
            const std::vector<unsigned>& byteCode,
            const std::vector<double>& immed,
            const FunctionParser::Data& data);

        class ByteCodeSynth;
        void SynthesizeByteCode(
            std::vector<unsigned>& byteCode,
            std::vector<double>&   immed,
            size_t& stacktop_max);
        void SynthesizeByteCode(ByteCodeSynth& synth);

        /* Regenerates the hash.
         * child_triggered=false: Recurse to children
         * child_triggered=true:  Recurse to parents
         */
        void Rehash(bool child_triggered);
        void Recalculate_Hash_NoRecursion();

        void Sort();
        void Sort_Recursive();

        void SetParams(const std::vector<Param>& RefParams);
        void AddParam(const Param& param);
        void DelParam(size_t index);

        /* Clones the tree. (For parameter duplication) */
        CodeTree* Clone();

        bool    IsImmed() const;
        double GetImmed() const { return Value; }
        bool    IsLongIntegerImmed() const { return IsImmed() && GetImmed() == (double)GetLongIntegerImmed(); }
        double GetLongIntegerImmed() const { return (long)GetImmed(); }
        bool      IsVar() const;
        unsigned GetVar() const { return Var; }

        void NegateImmed() { if(IsImmed()) Value = -Value;       }
        void InvertImmed() { if(IsImmed()) Value = 1.0 / Value;  }
        void NotTheImmed() { if(IsImmed()) Value = Value == 0.0; }

    private:
        void ConstantFolding();

    private:
        CodeTree(const CodeTree&);
        CodeTree& operator=(const CodeTree&);
    };

    inline void CodeTreeP::Forget()
    {
        if(!p) return;
        p->RefCount -= 1;
        if(!p->RefCount) delete p;
        //assert(p->RefCount >= 0);
    }
    inline void CodeTreeP::Have(CodeTree* p2)
    {
        if(p2) ++(p2->RefCount);
    }
    inline void CodeTreeP::Birth()
    {
        Have(p);
    }
    inline void CodeTreeP::Set(CodeTree* p2)
    {
        Have(p2);
        Forget();
        p = p2;
    }
}
