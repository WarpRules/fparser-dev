#include <cmath>
#include <list>
#include <cassert>

#include "fpoptimizer_codetree.hh"
#include "fptypes.hh"
#include "fpoptimizer_consts.hh"
#include "fpoptimizer_bytecodesynth.hh"
#include "fpoptimizer_optimize.hh"

#ifdef FP_SUPPORT_OPTIMIZER

using namespace FUNCTIONPARSERTYPES;
//using namespace FPoptimizer_Grammar;

namespace
{
    using namespace FPoptimizer_CodeTree;

    bool AssembleSequence(
                  const CodeTree& tree, long count,
                  const FPoptimizer_ByteCode::SequenceOpCode& sequencing,
                  FPoptimizer_ByteCode::ByteCodeSynth& synth,
                  size_t max_bytecode_grow_length);
}

namespace FPoptimizer_CodeTree
{
    void CodeTree::SynthesizeByteCode(
        std::vector<unsigned>& ByteCode,
        std::vector<double>&   Immed,
        size_t& stacktop_max)
    {
    #ifdef DEBUG_SUBSTITUTIONS
        std::cout << "Making bytecode for:\n";
        DumpTreeWithIndent(*this);
    #endif
        while(RecreateInversionsAndNegations())
        {
        #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "One change issued, produced:\n";
            DumpTreeWithIndent(*this);
        #endif
            FixIncompleteHashes();
        }
    #ifdef DEBUG_SUBSTITUTIONS
        std::cout << "After recreating inv/neg:  "; DumpTree(*this); std::cout << "\n";
    #endif

        std::vector<CodeTree> subexpressions ( FindCommonSubExpressions() );
        FPoptimizer_ByteCode::ByteCodeSynth synth;

        for(size_t a=0; a<subexpressions.size(); ++a)
        {
            subexpressions[a].SynthesizeByteCode(synth);
        }

    #ifdef DEBUG_SUBSTITUTIONS
        std::cout << "Actually synthesizing:\n";
        DumpTreeWithIndent(*this);
    #endif
        /* Then synthesize the actual expression */
        SynthesizeByteCode(synth);
      #if 0
        /* Ensure that the expression result is
         * the only thing that remains in the stack
         */
        /* Removed: Fparser does not seem to care! */
        /* Seems that it is not required even when cEval is supported. */
        if(synth.GetStackTop() > 1)
            synth.DoPopNMov(0, synth.GetStackTop()-1);
      #endif
        synth.Pull(ByteCode, Immed, stacktop_max);
    }

    void CodeTree::SynthesizeByteCode(FPoptimizer_ByteCode::ByteCodeSynth& synth) const
    {
        // If the synth can already locate our operand in the stack,
        // never mind synthesizing it again, just dup it.
        if(synth.FindAndDup(*this))
        {
            return;
        }

        switch(GetOpcode())
        {
            case cVar:
                synth.PushVar(GetVar());
                break;
            case cImmed:
                synth.PushImmed(GetImmed());
                break;
            case cAdd:
            case cMul:
            case cMin:
            case cMax:
            case cAnd:
            case cOr:
            case cAbsAnd:
            case cAbsOr:
            {
                if(GetOpcode() == cMul) // Special treatment for cMul sequences
                {
                    // If the paramlist contains an Immed, and that Immed
                    // fits in a long-integer, try to synthesize it
                    // as add-sequences instead.
                    for(size_t a=0; a<GetParamCount(); ++a)
                    {
                        if(GetParam(a).IsLongIntegerImmed())
                        {
                            long value = GetParam(a).GetLongIntegerImmed();

                            CodeTree tmp(*this, CodeTree::CloneTag());
                            tmp.DelParam(a);
                            tmp.Rehash();
                            bool success = AssembleSequence(
                                tmp, value, FPoptimizer_ByteCode::AddSequence,
                                synth,
                                MAX_MULI_BYTECODE_LENGTH);

                            if(success)
                            {
                                // this tree was treated just fine
                                synth.StackTopIs(*this);
                                return;
                            }
                        }
                    }
                }

                int n_stacked = 0;
                for(size_t a=0; a<GetParamCount(); ++a)
                {
                    GetParam(a).SynthesizeByteCode(synth);
                    ++n_stacked;

                    if(n_stacked > 1)
                    {
                        // Cumulate at the earliest opportunity.
                        synth.AddOperation(GetOpcode(), 2); // stack state: -2+1 = -1
                        n_stacked = n_stacked - 2 + 1;
                    }
                }
                if(n_stacked == 0)
                {
                    // Uh, we got an empty cAdd/cMul/whatever...
                    // Synthesize a default value.
                    // This should never happen.
                    switch(GetOpcode())
                    {
                        case cAdd:
                        case cOr:
                        case cAbsOr:
                            synth.PushImmed(0);
                            break;
                        case cMul:
                        case cAnd:
                        case cAbsAnd:
                            synth.PushImmed(1);
                            break;
                        case cMin:
                        case cMax:
                            //synth.PushImmed(NaN);
                            synth.PushImmed(0);
                            break;
                        default:
                            break;
                    }
                    ++n_stacked;
                }
                assert(n_stacked == 1);
                break;
            }
            case cPow:
            {
                const CodeTree& p0 = GetParam(0);
                const CodeTree& p1 = GetParam(1);

                if(!p1.IsLongIntegerImmed()
                || !AssembleSequence( /* Optimize integer exponents */
                        p0, p1.GetLongIntegerImmed(),
                        FPoptimizer_ByteCode::MulSequence,
                        synth,
                        MAX_POWI_BYTECODE_LENGTH)
                  )
                {
                    p0.SynthesizeByteCode(synth);
                    p1.SynthesizeByteCode(synth);
                    synth.AddOperation(GetOpcode(), 2); // Create a vanilla cPow.
                }
                break;
            }
            case cIf:
            case cAbsIf:
            {
                size_t ofs;
                // If the parameter amount is != 3, we're screwed.
                GetParam(0).SynthesizeByteCode(synth); // expression
                synth.SynthIfStep1(ofs, GetOpcode());
                GetParam(1).SynthesizeByteCode(synth); // true branch
                synth.SynthIfStep2(ofs);
                GetParam(2).SynthesizeByteCode(synth); // false branch
                synth.SynthIfStep3(ofs);
                break;
            }
            case cFCall:
            case cPCall:
            {
                // If the parameter count is invalid, we're screwed.
                for(size_t a=0; a<GetParamCount(); ++a)
                    GetParam(a).SynthesizeByteCode(synth);
                synth.AddOperation(GetOpcode(), (unsigned) GetParamCount());
                synth.AddOperation(GetFuncNo(), 0, 0);
                break;
            }
            default:
            {
                // If the parameter count is invalid, we're screwed.
                for(size_t a=0; a<GetParamCount(); ++a)
                    GetParam(a).SynthesizeByteCode(synth);
                synth.AddOperation(GetOpcode(), (unsigned) GetParamCount());
                break;
            }
        }
        synth.StackTopIs(*this);
    }
}

namespace
{
    bool AssembleSequence(
        const CodeTree& tree, long count,
        const FPoptimizer_ByteCode::SequenceOpCode& sequencing,
        FPoptimizer_ByteCode::ByteCodeSynth& synth,
        size_t max_bytecode_grow_length)
    {
        if(count != 0)
        {
            FPoptimizer_ByteCode::ByteCodeSynth backup = synth;

            tree.SynthesizeByteCode(synth);

            // Ignore the size generated by subtree
            size_t bytecodesize_backup = synth.GetByteCodeSize();

            FPoptimizer_ByteCode::AssembleSequence(count, sequencing, synth);

            size_t bytecode_grow_amount = synth.GetByteCodeSize() - bytecodesize_backup;
            if(bytecode_grow_amount > max_bytecode_grow_length)
            {
                synth = backup;
                return false;
            }
            return true;
        }
        else
        {
            FPoptimizer_ByteCode::AssembleSequence(count, sequencing, synth);
            return true;
        }
    }
}

#endif
