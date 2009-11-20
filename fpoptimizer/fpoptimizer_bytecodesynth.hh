#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#ifdef FP_SUPPORT_OPTIMIZER

#include <vector>
#include <utility>

#include "fpoptimizer_codetree.hh"

#ifndef FP_GENERATING_POWI_TABLE
enum { MAX_POWI_BYTECODE_LENGTH = 20 };
#else
enum { MAX_POWI_BYTECODE_LENGTH = 999 };
#endif
enum { MAX_MULI_BYTECODE_LENGTH = 3 };

namespace FPoptimizer_ByteCode
{
    class ByteCodeSynth
    {
    public:
        ByteCodeSynth()
            : ByteCode(), Immed(), StackTop(0), StackMax(0)
        {
            /* estimate the initial requirements as such */
            ByteCode.reserve(64);
            Immed.reserve(8);
            StackState.reserve(16);
        }

        void Pull(std::vector<unsigned>& bc,
                  std::vector<double>&   imm,
                  size_t& StackTop_max)
        {
            ByteCode.swap(bc);
            Immed.swap(imm);
            StackTop_max = StackMax;
        }

        size_t GetByteCodeSize() const { return ByteCode.size(); }
        size_t GetStackTop()     const { return StackTop; }

        void PushVar(unsigned varno)
        {
            ByteCode.push_back(varno);
            SetStackTop(StackTop+1);
        }

        void PushImmed(double immed)
        {
            using namespace FUNCTIONPARSERTYPES;
            ByteCode.push_back(cImmed);
            Immed.push_back(immed);
            SetStackTop(StackTop+1);
        }

        void StackTopIs(const FPoptimizer_CodeTree::CodeTree& tree)
        {
            if(StackTop > 0)
            {
                StackState[StackTop-1].first = true;
                StackState[StackTop-1].second = tree;
            }
        }

        void EatNParams(unsigned eat_count)
        {
            SetStackTop(StackTop - eat_count);
        }

        void ProducedNParams(unsigned produce_count)
        {
            SetStackTop(StackTop + produce_count);
        }

        void AddOperation(unsigned opcode, unsigned eat_count, unsigned produce_count = 1)
        {
            EatNParams(eat_count);

            using namespace FUNCTIONPARSERTYPES;

            if(!ByteCode.empty() && opcode == cMul && ByteCode.back() == cDup)
                ByteCode.back() = cSqr;
            else
                ByteCode.push_back(opcode);

            ProducedNParams(produce_count);
        }

        void DoPopNMov(size_t targetpos, size_t srcpos)
        {
            using namespace FUNCTIONPARSERTYPES;
            ByteCode.push_back(cPopNMov);
            ByteCode.push_back( (unsigned) targetpos);
            ByteCode.push_back( (unsigned) srcpos);

            SetStackTop(srcpos+1);
            StackState[targetpos] = StackState[srcpos];
            SetStackTop(targetpos+1);
        }

        void DoDup(size_t src_pos)
        {
            using namespace FUNCTIONPARSERTYPES;
            if(src_pos == StackTop-1)
            {
                ByteCode.push_back(cDup);
            }
            else
            {
                ByteCode.push_back(cFetch);
                ByteCode.push_back( (unsigned) src_pos);
            }
            SetStackTop(StackTop + 1);
            StackState[StackTop-1] = StackState[src_pos];
        }

        size_t FindPos(const FPoptimizer_CodeTree::CodeTree& tree) const
        {
            /*std::cout << "Stack state now(" << StackTop << "):\n";
            for(size_t a=0; a<StackTop; ++a)
            {
                std::cout << a << ": ";
                if(StackState[a].first)
                    DumpTree(StackState[a].second);
                else
                    std::cout << "?";
                std::cout << "\n";
            }*/
            for(size_t a=StackTop; a-->0; )
                if(StackState[a].first && StackState[a].second.IsIdenticalTo(tree))
                    return a;
            return ~size_t(0);
        }

        bool Find(const FPoptimizer_CodeTree::CodeTree& tree) const
        {
            return FindPos(tree) != ~size_t(0);
        }

        bool FindAndDup(const FPoptimizer_CodeTree::CodeTree& tree)
        {
            size_t pos = FindPos(tree);
            if(pos != ~size_t(0))
            {
                DoDup(pos);
                return true;
            }
            return false;
        }

        struct IfData
        {
            size_t ofs;
        };

        void SynthIfStep1(IfData& ifdata, FUNCTIONPARSERTYPES::OPCODE op)
        {
            using namespace FUNCTIONPARSERTYPES;
            SetStackTop(StackTop-1); // the If condition was popped.

            ifdata.ofs = ByteCode.size();
            ByteCode.push_back(op);
            ByteCode.push_back(0); // code index
            ByteCode.push_back(0); // Immed index
        }
        void SynthIfStep2(IfData& ifdata)
        {
            using namespace FUNCTIONPARSERTYPES;
            SetStackTop(StackTop-1); // ignore the pushed then-branch result.

            ByteCode[ifdata.ofs+1] = unsigned( ByteCode.size()+2 );
            ByteCode[ifdata.ofs+2] = unsigned( Immed.size()      );

            ifdata.ofs = ByteCode.size();
            ByteCode.push_back(cJump);
            ByteCode.push_back(0); // code index
            ByteCode.push_back(0); // Immed index
        }
        void SynthIfStep3(IfData& ifdata)
        {
            using namespace FUNCTIONPARSERTYPES;
            SetStackTop(StackTop-1); // ignore the pushed else-branch result.

            ByteCode[ifdata.ofs+1] = unsigned( ByteCode.size()-1 );
            ByteCode[ifdata.ofs+2] = unsigned( Immed.size()      );

            SetStackTop(StackTop+1); // one or the other was pushed.

            /* Threading jumps:
             * If there are any cJumps that point
             * to the cJump instruction we just changed,
             * change them to point to this target as well.
             * This screws up PrintByteCode() majorly.
             */
            for(size_t a=0; a<ifdata.ofs; ++a)
            {
                if(ByteCode[a]   == cJump
                && ByteCode[a+1] == ifdata.ofs-1)
                {
                    ByteCode[a+1] = unsigned( ByteCode.size()-1 );
                    ByteCode[a+2] = unsigned( Immed.size()      );
                }
                switch(ByteCode[a])
                {
                    case cAbsIf:
                    case cIf:
                    case cJump:
                    case cPopNMov: a += 2; break;
                    case cFCall:
                    case cPCall:
                    case cFetch: a += 1; break;
                    default: break;
                }
            }
        }

    protected:
        void SetStackTop(size_t value)
        {
            StackTop = value;
            if(StackTop > StackMax)
            {
                StackMax = StackTop;
                StackState.resize(StackMax);
            }
        }

    private:
        std::vector<unsigned> ByteCode;
        std::vector<double>   Immed;

        std::vector<
            std::pair<bool/*known*/, FPoptimizer_CodeTree::CodeTree/*tree*/>
                   > StackState;
        size_t StackTop;
        size_t StackMax;
    };

    struct SequenceOpCode;
    extern const SequenceOpCode AddSequence; /* Multiplication implemented with adds */
    extern const SequenceOpCode MulSequence; /* Exponentiation implemented with muls */

    /* Generate a sequence that multiplies or exponentifies the
     * last operand in the stack by the given constant integer
     * amount (positive or negative).
     */
    void AssembleSequence(
        long count,
        const SequenceOpCode& sequencing,
        ByteCodeSynth& synth);
}

#endif
