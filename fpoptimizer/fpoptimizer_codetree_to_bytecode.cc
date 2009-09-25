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

#ifndef FP_GENERATING_POWI_TABLE
static const unsigned MAX_POWI_BYTECODE_LENGTH = 15;
#else
static const unsigned MAX_POWI_BYTECODE_LENGTH = 999;
#endif
static const unsigned MAX_MULI_BYTECODE_LENGTH = 3;

namespace
{
    using namespace FPoptimizer_CodeTree;

    bool AssembleSequence(
                  const CodeTree& tree, long count,
                  const FPoptimizer_ByteCode::SequenceOpCode& sequencing,
                  FPoptimizer_ByteCode::ByteCodeSynth& synth,
                  size_t max_bytecode_grow_length);
}

namespace
{
    typedef
        std::map<fphash_t,  std::pair<size_t, CodeTree> >
        TreeCountType;
    typedef
        std::multimap<fphash_t, CodeTree>
        DoneTreesType;

    void FindTreeCounts(TreeCountType& TreeCounts, const CodeTree& tree)
    {
        TreeCountType::iterator i = TreeCounts.lower_bound(tree.GetHash());
        if(i != TreeCounts.end()
        && tree.GetHash() == i->first
        && tree.IsIdenticalTo( i->second.second ) )
            i->second.first += 1;
        else
            TreeCounts.insert(i, std::make_pair(tree.GetHash(), std::make_pair(size_t(1), tree)));

        for(size_t a=0; a<tree.GetParamCount(); ++a)
            FindTreeCounts(TreeCounts, tree.GetParam(a));
    }

    void RememberRecursivelyHashList(DoneTreesType& hashlist,
                                     const CodeTree& tree)
    {
        hashlist.insert( std::make_pair(tree.GetHash(), tree) );
        for(size_t a=0; a<tree.GetParamCount(); ++a)
            RememberRecursivelyHashList(hashlist, tree.GetParam(a));
    }
    bool RecreateInversionsAndNegations(CodeTree& tree)
    {
        tree.BeginChanging();

        bool changed = false;

        for(size_t a=0; a<tree.GetParamCount(); ++a)
            if(RecreateInversionsAndNegations(tree.GetParam(a)))
                changed = true;

        switch(tree.GetOpcode()) // Recreate inversions and negations
        {
            case cMul:
            {
                std::vector<CodeTree> div_params;

                for(size_t a = tree.GetParamCount(); a-- > 0; )
                    if(tree.GetParam(a).GetOpcode() == cPow
                    && tree.GetParam(a).GetParam(1).IsImmed()
                    && FloatEqual(tree.GetParam(a).GetParam(1).GetImmed(), -1.0))
                    {
                        div_params.push_back(tree.GetParam(a).GetParam(0));
                        tree.DelParam(a);
                        changed = true;
                    }
                if(!div_params.empty())
                {
                    CodeTree divgroup;
                    divgroup.BeginChanging();
                    divgroup.SetOpcode(cMul);
                    divgroup.SetParamsMove(div_params);
                    divgroup.ConstantFolding();
                    divgroup.FinishChanging();
                    CodeTree mulgroup;
                    mulgroup.BeginChanging();
                    mulgroup.SetOpcode(cMul);
                    mulgroup.SetParamsMove(tree.GetParams());
                    mulgroup.ConstantFolding();
                    mulgroup.FinishChanging();
                    if(mulgroup.IsImmed() && FloatEqual(mulgroup.GetImmed(), 1.0))
                        tree.SetOpcode(cInv);
                    else
                    {
                        tree.SetOpcode(cDiv);
                        tree.AddParam(mulgroup);
                    }
                    tree.AddParam(divgroup);
                }
                break;
            }
            case cAdd:
            {
                std::vector<CodeTree> sub_params;

                for(size_t a = tree.GetParamCount(); a-- > 0; )
                    if(tree.GetParam(a).GetOpcode() == cMul)
                    {
                        bool is_signed = false;
                        // if the mul group has a -1 constant...
                        bool subchanged = false;
                        CodeTree mulgroup = tree.GetParam(a);
                        for(size_t b=mulgroup.GetParamCount(); b-- > 0; )
                            if(mulgroup.GetParam(b).IsImmed()
                            && FloatEqual(mulgroup.GetParam(b).GetImmed(), -1.0))
                            {
                                if(!subchanged) mulgroup.BeginChanging();
                                mulgroup.DelParam(b);
                                is_signed = !is_signed;
                                subchanged = true;
                            }
                        if(subchanged)
                        {
                            mulgroup.ConstantFolding();
                            mulgroup.FinishChanging();
                            changed = true;
                        }
                        if(is_signed)
                        {
                            sub_params.push_back(mulgroup); // this mul group
                            tree.DelParam(a);
                            changed = true;
                        }
                    }
                if(!sub_params.empty())
                {
                    CodeTree subgroup;
                    subgroup.BeginChanging();
                    subgroup.SetOpcode(cAdd);
                    subgroup.SetParamsMove(sub_params);
                    subgroup.ConstantFolding();
                    subgroup.FinishChanging();
                    CodeTree addgroup;
                    addgroup.BeginChanging();
                    addgroup.SetOpcode(cAdd);
                    addgroup.SetParamsMove(tree.GetParams());
                    addgroup.ConstantFolding();
                    addgroup.FinishChanging();
                    if(addgroup.IsImmed() && FloatEqual(addgroup.GetImmed(), 0.0))
                        tree.SetOpcode(cNeg);
                    else
                    {
                        tree.SetOpcode(cSub);
                        tree.AddParam(addgroup);
                    }
                    tree.AddParam(subgroup);
                }
                break;
            }
            default: break;
        }
        if(changed)
        {
        #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "BEGIN CONSTANTFOLDING: ";
            FPoptimizer_Grammar::DumpTree(tree);
            std::cout << "\n";
        #endif
            tree.ConstantFolding();
        #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "END CONSTANTFOLDING:   ";
            FPoptimizer_Grammar::DumpTree(tree);
            std::cout << "\n";
        #endif
            tree.FinishChanging();
            return true;
        }
        return false;
    }
}

namespace FPoptimizer_CodeTree
{
    void CodeTree::SynthesizeByteCode(
        std::vector<unsigned>& ByteCode,
        std::vector<double>&   Immed,
        size_t& stacktop_max)
    {
    #ifdef DEBUG_SUBSTITUTIONS
        std::cout << "Making bytecode for:       "; FPoptimizer_Grammar::DumpTree(*this); std::cout << "\n";
    #endif
        RecreateInversionsAndNegations(*this);
    #ifdef DEBUG_SUBSTITUTIONS
        std::cout << "After recreating inv/neg:  "; FPoptimizer_Grammar::DumpTree(*this); std::cout << "\n";
    #endif

        FPoptimizer_ByteCode::ByteCodeSynth synth;

        /* Find common subtrees */
        TreeCountType TreeCounts;
        FindTreeCounts(TreeCounts, *this);

        /* Synthesize some of the most common ones */
        DoneTreesType AlreadyDoneTrees;
    FindMore: ;
        size_t best_score = 0;
        TreeCountType::const_iterator synth_it;
        for(TreeCountType::const_iterator
            i = TreeCounts.begin();
            i != TreeCounts.end();
            ++i)
        {
            size_t score = i->second.first;
            // It must always occur at least twice
            if(score < 2) continue;
            // And it must not be a simple expression
            if(i->second.second.GetDepth() < 2) CandSkip: continue;
            // And it must not yet have been synthesized
            DoneTreesType::const_iterator j = AlreadyDoneTrees.lower_bound(i->first);
            for(; j != AlreadyDoneTrees.end() && j->first == i->first; ++j)
            {
                if(j->second.IsIdenticalTo(i->second.second))
                    goto CandSkip;
            }
            // Is a candidate.
            score *= i->second.second.GetDepth();
            if(score > best_score)
                { best_score = score; synth_it = i; }
        }
        if(best_score > 0)
        {
    #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "Found Common Subexpression:"; FPoptimizer_Grammar::DumpTree(*synth_it->second.second); std::cout << "\n";
    #endif
            /* Synthesize the selected tree */
            synth_it->second.second.SynthesizeByteCode(synth);
            /* Add the tree and all its children to the AlreadyDoneTrees list,
             * to prevent it from being re-synthesized
             */
            RememberRecursivelyHashList(AlreadyDoneTrees, synth_it->second.second);
            goto FindMore;
        }

    #ifdef DEBUG_SUBSTITUTIONS
        std::cout << "Actually synthesizing:     "; FPoptimizer_Grammar::DumpTree(*this); std::cout << "\n";
    #endif
        /* Then synthesize the actual expression */
        SynthesizeByteCode(synth);
      #ifndef FP_DISABLE_EVAL
        /* Ensure that the expression result is
         * the only thing that remains in the stack
         */
        /* Removed: Fparser does not seem to care! */
        /* But if cEval is supported, it still needs to be done. */
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

                            CodeTree tmp;
                            tmp.Become(*this);
                            tmp.BeginChanging();
                            tmp.DelParam(a);
                            tmp.ConstantFolding();
                            tmp.FinishChanging();

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
                            synth.PushImmed(0);
                            break;
                        case cMul:
                        case cAnd:
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

                if(p1.IsImmed() && p1.GetImmed() == 0.5)
                {
                    p0.SynthesizeByteCode(synth);
                    synth.AddOperation(cSqrt, 1);
                }
                else if(p1.IsImmed() && p1.GetImmed() == -0.5)
                {
                    p0.SynthesizeByteCode(synth);
                    synth.AddOperation(cRSqrt, 1);
                }
                /*
                else if(p0.IsImmed() && p0.GetImmed() == CONSTANT_E)
                {
                    p1.SynthesizeByteCode(synth);
                    synth.AddOperation(cExp, 1);
                }
                else if(p0.IsImmed() && p0.GetImmed() == CONSTANT_EI)
                {
                    p1.SynthesizeByteCode(synth);
                    synth.AddOperation(cNeg, 1);
                    synth.AddOperation(cExp, 1);
                }
                */
                else if(!p1.IsLongIntegerImmed()
                || !AssembleSequence( /* Optimize integer exponents */
                        p0, p1.GetLongIntegerImmed(),
                        FPoptimizer_ByteCode::MulSequence,
                        synth,
                        MAX_POWI_BYTECODE_LENGTH)
                  )
                {
                    if(p0.IsImmed() && p0.GetImmed() > 0.0)
                    {
                        // Convert into cExp or Exp2.
                        //    x^y = exp(log(x) * y) =
                        //    Can only be done when x is positive, though.
                        double mulvalue = std::log( p0.GetImmed() );
                        const CodeTree& p1backup = p1;
                        CodeTree p1 = p1backup;
                        if(p1.GetOpcode() == cMul)
                        {
                            p1.BeginChanging();
                            // Neat, we can delegate the multiplication to the child
                            p1.AddParam( CodeTree(mulvalue) );
                            p1.ConstantFolding();
                            p1.FinishChanging();
                            mulvalue = 1.0;
                        }

                        // If the exponent needs multiplication, multiply it
                        if(
                      #ifdef FP_EPSILON
                          fabs(mulvalue - (double)(long)mulvalue) <= FP_EPSILON
                      #else
                          mulvalue == (double)(long)mulvalue
                      #endif
                        && AssembleSequence(p1, (long)mulvalue,
                                            FPoptimizer_ByteCode::AddSequence, synth,
                                            MAX_MULI_BYTECODE_LENGTH))
                        {
                            // Done with a dup/add sequence, cExp
                            synth.AddOperation(cExp, 1);
                        }
                        /* - disabled cExp2 optimizations for now, because it
                         *   turns out that glibc for at least x86_64 has a
                         *   particularly stupid exp2() implementation that
                         *   is _slower_ than exp() or even pow(2,x)
                         *
                        else if(
                          #ifndef FP_SUPPORT_EXP2
                           #ifdef FP_EPSILON
                            fabs(mulvalue - CONSTANT_L2) <= FP_EPSILON
                           #else
                            mulvalue == CONSTANT_L2
                           #endif
                          #else
                            true
                          #endif
                            )
                        {
                            // Do with cExp2; in all likelihood it's never slower than cExp.
                            mulvalue *= CONSTANT_L2I;
                            if(
                          #ifdef FP_EPSILON
                              fabs(mulvalue - (double)(long)mulvalue) <= FP_EPSILON
                          #else
                              mulvalue == (double)(long)mulvalue
                          #endif
                            && AssembleSequence(*p1, (long)mulvalue,
                                                FPoptimizer_ByteCode::AddSequence, synth,
                                                MAX_MULI_BYTECODE_LENGTH))
                            {
                                // Done with a dup/add sequence, cExp2
                                synth.AddOperation(cExp2, 1);
                            }
                            else
                            {
                                // Do with cMul and cExp2
                                p1.SynthesizeByteCode(synth);
                                synth.PushImmed(mulvalue);
                                synth.AddOperation(cMul, 2);
                                synth.AddOperation(cExp2, 1);
                            }
                        }*/
                        else
                        {
                            // Do with cMul and cExp
                            p1.SynthesizeByteCode(synth);
                            synth.PushImmed(mulvalue);
                            synth.AddOperation(cMul, 2);
                            synth.AddOperation(cExp, 1);
                        }
                    }
                    else
                    {
                        p0.SynthesizeByteCode(synth);
                        p1.SynthesizeByteCode(synth);
                        synth.AddOperation(GetOpcode(), 2); // Create a vanilla cPow.
                    }
                }
                break;
            }
            case cIf:
            {
                size_t ofs;
                // If the parameter amount is != 3, we're screwed.
                GetParam(0).SynthesizeByteCode(synth); // expression
                synth.SynthIfStep1(ofs);
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
