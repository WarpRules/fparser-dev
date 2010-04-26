#include <cmath>
#include <list>
#include <cassert>

#include "codetree.hh"
#include "fptypes.hh"
#include "consts.hh"
#include "optimize.hh"
#include "bytecodesynth.hh"

#ifdef FP_SUPPORT_OPTIMIZER

using namespace FUNCTIONPARSERTYPES;
//using namespace FPoptimizer_Grammar;

namespace
{
    using namespace FPoptimizer_CodeTree;

    template<typename Value_t>
    bool AssembleSequence(
                  const CodeTree<Value_t>& tree, long count,
                  const FPoptimizer_ByteCode::SequenceOpCode<Value_t>& sequencing,
                  FPoptimizer_ByteCode::ByteCodeSynth<Value_t>& synth,
                  size_t max_bytecode_grow_length);
}

namespace FPoptimizer_CodeTree
{
    template<typename Value_t>
    void CodeTree<Value_t>::SynthesizeByteCode(
        std::vector<unsigned>& ByteCode,
        std::vector<Value_t>&   Immed,
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
        std::cout << "Actually synthesizing, after recreating inv/neg:\n";
        DumpTreeWithIndent(*this);
    #endif

        FPoptimizer_ByteCode::ByteCodeSynth<Value_t> synth;

        /* Then synthesize the actual expression */
        SynthesizeByteCode(synth, false);
        /* The "false" parameters tells SynthesizeByteCode
         * that at the outermost synthesizing level, it does
         * not matter if leftover temps are left in the stack.
         */
        synth.Pull(ByteCode, Immed, stacktop_max);
    }

    template<typename Value_t>
    void CodeTree<Value_t>::SynthesizeByteCode(
        FPoptimizer_ByteCode::ByteCodeSynth<Value_t>& synth,
        bool MustPopTemps) const
    {
        // If the synth can already locate our operand in the stack,
        // never mind synthesizing it again, just dup it.
        if(synth.FindAndDup(*this))
        {
            return;
        }
        if(GetOpcode() == cSec)
        {
            CodeTree invtree; invtree.SetParams(GetParams());
            invtree.SetOpcode(cCos); invtree.Rehash(false);
            if(synth.FindAndDup(invtree)) { synth.AddOperation(cInv,1,1); return; }
        }
        if(GetOpcode() == cSin)
        {
            CodeTree invtree; invtree.SetParams(GetParams());
            invtree.SetOpcode(cCsc); invtree.Rehash(false);
            if(synth.FindAndDup(invtree)) { synth.AddOperation(cInv,1,1); return; }
        }
        if(GetOpcode() == cCsc)
        {
            CodeTree invtree; invtree.SetParams(GetParams());
            invtree.SetOpcode(cSin); invtree.Rehash(false);
            if(synth.FindAndDup(invtree)) { synth.AddOperation(cInv,1,1); return; }
        }
        if(GetOpcode() == cCos)
        {
            CodeTree invtree; invtree.SetParams(GetParams());
            invtree.SetOpcode(cSec); invtree.Rehash(false);
            if(synth.FindAndDup(invtree)) { synth.AddOperation(cInv,1,1); return; }
        }

        size_t n_subexpressions_synthesized = SynthCommonSubExpressions(synth);

        switch(GetOpcode())
        {
            case VarBegin:
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
                    bool did_muli = false;
                    for(size_t a=0; a<GetParamCount(); ++a)
                    {
                        if(GetParam(a).IsLongIntegerImmed())
                        {
                            long value = GetParam(a).GetLongIntegerImmed();

                            CodeTree tmp(*this, CodeTree::CloneTag());
                            tmp.DelParam(a);
                            tmp.Rehash();
                            if(AssembleSequence(
                                tmp, value,
                                FPoptimizer_ByteCode::SequenceOpcodes<Value_t>::AddSequence,
                                synth,
                                MAX_MULI_BYTECODE_LENGTH))
                            {
                                did_muli = true;
                                break;
                            }
                        }
                    }
                    if(did_muli)
                        break; // done
                }

                // If any of the params is currently a copy of
                // the stack topmost item, treat it first.
                int n_stacked = 0;
                std::vector<bool> done( GetParamCount() , false );
                CodeTree synthed_tree;
                synthed_tree.SetOpcode(GetOpcode());
                for(;;)
                {
                    bool found = false;
                    for(size_t a=0; a<GetParamCount(); ++a)
                    {
                        if(done[a]) continue;
                        if(synth.IsStackTop(GetParam(a)))
                        {
                            found = true;
                            done[a] = true;
                            GetParam(a).SynthesizeByteCode(synth);
                            synthed_tree.AddParam(GetParam(a));
                            if(++n_stacked > 1)
                            {
                                // Cumulate at the earliest opportunity.
                                synth.AddOperation(GetOpcode(), 2); // stack state: -2+1 = -1
                                synthed_tree.Rehash(false);
                                synth.StackTopIs(synthed_tree);
                                n_stacked = n_stacked - 2 + 1;
                            }
                        }
                    }
                    if(!found) break;
                }

                for(size_t a=0; a<GetParamCount(); ++a)
                {
                    if(done[a]) continue;
                    GetParam(a).SynthesizeByteCode(synth);
                    synthed_tree.AddParam(GetParam(a));
                    if(++n_stacked > 1)
                    {
                        // Cumulate at the earliest opportunity.
                        synth.AddOperation(GetOpcode(), 2); // stack state: -2+1 = -1
                        synthed_tree.Rehash(false);
                        synth.StackTopIs(synthed_tree);
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
                        FPoptimizer_ByteCode::SequenceOpcodes<Value_t>::MulSequence,
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
                // Assume that the parameter count is 3 as it should.
                typename FPoptimizer_ByteCode::ByteCodeSynth<Value_t>::IfData ifdata;

                GetParam(0).SynthesizeByteCode(synth); // expression

                synth.SynthIfStep1(ifdata, GetOpcode());

                GetParam(1).SynthesizeByteCode(synth); // true branch

                synth.SynthIfStep2(ifdata);

                GetParam(2).SynthesizeByteCode(synth); // false branch

                synth.SynthIfStep3(ifdata);
                break;
            }
            case cFCall:
            case cPCall:
            {
                // Assume that the parameter count is as it should.
                for(size_t a=0; a<GetParamCount(); ++a)
                    GetParam(a).SynthesizeByteCode(synth);
                synth.AddOperation(GetOpcode(), (unsigned) GetParamCount());
                synth.AddOperation(GetFuncNo(), 0, 0);
                break;
            }
            default:
            {
                // Assume that the parameter count is as it should.
                for(size_t a=0; a<GetParamCount(); ++a)
                    GetParam(a).SynthesizeByteCode(synth);
                synth.AddOperation(GetOpcode(), (unsigned) GetParamCount());
                break;
            }
        }

        // Tell the synthesizer which tree was just produced in the stack
        synth.StackTopIs(*this);

        // If we added subexpressions, peel them off the stack now
        if(MustPopTemps && n_subexpressions_synthesized > 0)
        {
            size_t top = synth.GetStackTop();
            synth.DoPopNMov(top-1-n_subexpressions_synthesized, top-1);
        }
    }
}

namespace
{
    template<typename Value_t>
    bool AssembleSequence(
        const CodeTree<Value_t>& tree, long count,
        const FPoptimizer_ByteCode::SequenceOpCode<Value_t>& sequencing,
        FPoptimizer_ByteCode::ByteCodeSynth<Value_t>& synth,
        size_t max_bytecode_grow_length)
    {
        if(count != 0)
        {
            FPoptimizer_ByteCode::ByteCodeSynth<Value_t> backup = synth;

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

// Explicitly instantiate types
namespace FPoptimizer_CodeTree
{
    template void CodeTree<double>::SynthesizeByteCode(
        std::vector<unsigned>& ByteCode,
        std::vector<double>&   Immed,
        size_t& stacktop_max);
#ifdef FP_SUPPORT_FLOAT_TYPE
    template void CodeTree<float>::SynthesizeByteCode(
        std::vector<unsigned>& ByteCode,
        std::vector<float>&   Immed,
        size_t& stacktop_max);
#endif
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
    template void CodeTree<long double>::SynthesizeByteCode(
        std::vector<unsigned>& ByteCode,
        std::vector<long double>&   Immed,
        size_t& stacktop_max);
#endif
}

#endif
