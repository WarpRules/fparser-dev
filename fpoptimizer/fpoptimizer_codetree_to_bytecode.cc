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
        std::multimap<fphash_t,  std::pair<size_t, CodeTree> >
        TreeCountType;
    typedef
        std::multimap<fphash_t, CodeTree>
        DoneTreesType;

    void FindTreeCounts(TreeCountType& TreeCounts, const CodeTree& tree)
    {
        TreeCountType::iterator i = TreeCounts.lower_bound(tree.GetHash());
        for(; i != TreeCounts.end() && i->first == tree.GetHash(); ++i)
        {
            if(tree.IsIdenticalTo( i->second.second ) )
            {
                i->second.first += 1;
                goto found;
        }   }
        TreeCounts.insert(i, std::make_pair(tree.GetHash(), std::make_pair(size_t(1), tree)));
    found:
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

    #ifdef DEBUG_SUBSTITUTIONS
    CodeTree* root;
    #endif

    bool IsOptimizableUsingPowi(long immed, long penalty = 0)
    {
        FPoptimizer_ByteCode::ByteCodeSynth synth;
        return AssembleSequence(CodeTree(0, CodeTree::VarTag()),
                                immed,
                                FPoptimizer_ByteCode::MulSequence,
                                synth,
                                MAX_POWI_BYTECODE_LENGTH - penalty);
    }

    void ChangeIntoSqrtChain(CodeTree& tree, long sqrt_chain)
    {
        long abs_sqrt_chain = sqrt_chain < 0 ? -sqrt_chain : sqrt_chain;
        while(abs_sqrt_chain > 2)
        {
            CodeTree tmp;
            tmp.SetOpcode(cSqrt);
            tmp.AddParamMove(tree.GetParam(0));
            tmp.Rehash();
            tree.SetParamMove(0, tmp);
            abs_sqrt_chain /= 2;
        }
        tree.DelParam(1);
        tree.SetOpcode(sqrt_chain < 0 ? cRSqrt : cSqrt);
    }
}

namespace FPoptimizer_CodeTree
{
    bool CodeTree::RecreateInversionsAndNegations(bool prefer_base2)
    {
        bool changed = false;

        for(size_t a=0; a<GetParamCount(); ++a)
            if(GetParam(a).RecreateInversionsAndNegations(prefer_base2))
                changed = true;

        if(changed)
        {
        exit_changed:
            Mark_Incompletely_Hashed();
            return true;
        }

        switch(GetOpcode()) // Recreate inversions and negations
        {
            case cMul:
            {
                std::vector<CodeTree> div_params;
                CodeTree found_log2;
                for(size_t a = GetParamCount(); a-- > 0; )
                {
                    const CodeTree& powgroup = GetParam(a);
                    if(powgroup.GetOpcode() == cPow
                    && powgroup.GetParam(1).IsImmed())
                    {
                        const CodeTree& exp_param = powgroup.GetParam(1);
                        double exponent = exp_param.GetImmed();
                        if(FloatEqual(exponent, -1.0))
                        {
                            CopyOnWrite();
                            div_params.push_back(GetParam(a).GetParam(0));
                            DelParam(a); // delete the pow group
                        }
                        else if(exponent < 0 && IsIntegerConst(exponent))
                        {
                            CodeTree edited_powgroup;
                            edited_powgroup.SetOpcode(cPow);
                            edited_powgroup.AddParam(powgroup.GetParam(0));
                            edited_powgroup.AddParam(CodeTree(-exponent));
                            edited_powgroup.Rehash();
                            div_params.push_back(edited_powgroup);
                            CopyOnWrite();
                            DelParam(a); // delete the pow group
                        }
                    }
                    else if(powgroup.GetOpcode() == cLog2 && !found_log2.IsDefined())
                    {
                        found_log2 = powgroup.GetParam(0);
                        CopyOnWrite();
                        DelParam(a);
                    }
                }
                if(!div_params.empty())
                {
                    changed = true;

                    CodeTree divgroup;
                    divgroup.SetOpcode(cMul);
                    divgroup.SetParamsMove(div_params);
                    divgroup.Rehash(); // will reduce to div_params[0] if only one item
                    CodeTree mulgroup;
                    mulgroup.SetOpcode(cMul);
                    mulgroup.SetParamsMove(GetParams());
                    mulgroup.Rehash(); // will reduce to 1.0 if none remained in this cMul
                    if(mulgroup.IsImmed() && FloatEqual(mulgroup.GetImmed(), 1.0))
                    {
                        SetOpcode(cInv);
                        AddParamMove(divgroup);
                    }
                    else
                    {
                        if(mulgroup.GetDepth() >= divgroup.GetDepth())
                        {
                            SetOpcode(cDiv);
                            AddParamMove(mulgroup);
                            AddParamMove(divgroup);
                        }
                        else
                        {
                            SetOpcode(cRDiv);
                            AddParamMove(divgroup);
                            AddParamMove(mulgroup);
                        }
                    }
                }
                if(found_log2.IsDefined())
                {
                    CodeTree mulgroup;
                    mulgroup.SetOpcode(cMul);
                    mulgroup.SetParamsMove(GetParams());
                    mulgroup.Rehash();
                    SetOpcode(cLog2by);
                    AddParamMove(found_log2);
                    AddParamMove(mulgroup);
                    changed = true;
                }
                break;
            }
            case cAdd:
            {
                std::vector<CodeTree> sub_params;

                for(size_t a = GetParamCount(); a-- > 0; )
                    if(GetParam(a).GetOpcode() == cMul)
                    {
                        bool is_signed = false; // if the mul group has a -1 constant...

                        CodeTree& mulgroup = GetParam(a);

                        for(size_t b=mulgroup.GetParamCount(); b-- > 0; )
                        {
                            if(mulgroup.GetParam(b).IsImmed())
                            {
                                double factor = mulgroup.GetParam(b).GetImmed();
                                if(FloatEqual(factor, -1.0))
                                {
                                    mulgroup.CopyOnWrite();
                                    mulgroup.DelParam(b);
                                    is_signed = !is_signed;
                                }
                                else if(FloatEqual(factor, -2.0))
                                {
                                    mulgroup.CopyOnWrite();
                                    mulgroup.DelParam(b);
                                    mulgroup.AddParam( CodeTree(2.0) );
                                    is_signed = !is_signed;
                                }
                            }
                        }
                        if(is_signed)
                        {
                            mulgroup.Rehash();
                            sub_params.push_back(mulgroup);
                            CopyOnWrite();
                            DelParam(a);
                        }
                    }
                if(!sub_params.empty())
                {
                    CodeTree subgroup;
                    subgroup.SetOpcode(cAdd);
                    subgroup.SetParamsMove(sub_params);
                    subgroup.Rehash(); // will reduce to sub_params[0] if only one item
                    CodeTree addgroup;
                    addgroup.SetOpcode(cAdd);
                    addgroup.SetParamsMove(GetParams());
                    addgroup.Rehash(); // will reduce to 0.0 if none remained in this cAdd
                    if(addgroup.IsImmed() && FloatEqual(addgroup.GetImmed(), 0.0))
                    {
                        SetOpcode(cNeg);
                        AddParamMove(subgroup);
                    }
                    else
                    {
                        if(addgroup.GetDepth() == 1)
                        {
                            /* 5 - (x+y+z) is best expressed as rsub(x+y+z, 5);
                             * this has lowest stack usage.
                             * This is identified by addgroup having just one member.
                             */
                            SetOpcode(cRSub);
                            AddParamMove(subgroup);
                            AddParamMove(addgroup);
                        }
                        else if(subgroup.GetOpcode() == cAdd)
                        {
                            /* a+b-(x+y+z) is expressed as a+b-x-y-z.
                             * Making a long chain of cSubs is okay, because the
                             * cost of cSub is the same as the cost of cAdd.
                             * Thus we get the lowest stack usage.
                             * This approach cannot be used for cDiv.
                             */
                            SetOpcode(cSub);
                            AddParamMove(addgroup);
                            AddParamMove(subgroup.GetParam(0));
                            for(size_t a=1; a<subgroup.GetParamCount(); ++a)
                            {
                                CodeTree innersub;
                                innersub.SetOpcode(cSub);
                                innersub.SetParamsMove(GetParams());
                                innersub.Rehash(false);
                                //DelParams();
                                AddParamMove(innersub);
                                AddParamMove(subgroup.GetParam(a));
                            }
                        }
                        else
                        {
                            SetOpcode(cSub);
                            AddParamMove(addgroup);
                            AddParamMove(subgroup);
                        }
                    }
                }
                break;
            }
            case cLog:
            {
                if(prefer_base2)
                {
                    SetOpcode(cLog2);
                    CodeTree mul;
                    mul.SetOpcode(cMul);
                    mul.AddParamMove(*this);
                    mul.AddParam(CodeTree(CONSTANT_L2));
                    Become(mul);
                    changed = true;
                }
                break;
            }
            case cLog10:
            {
                if(prefer_base2)
                {
                    SetOpcode(cLog2);
                    CodeTree mul;
                    mul.SetOpcode(cMul);
                    mul.AddParamMove(*this);
                    mul.AddParam(CodeTree(CONSTANT_L10B));
                    Become(mul);
                    changed = true;
                }
                break;
            }
            case cExp:
            {
                if(prefer_base2)
                {
                    CodeTree p0 = GetParam(0), mul;
                    mul.SetOpcode(cMul);
                    // exp(x) -> exp2(x*CONSTANT_L2I)
                    if(p0.GetOpcode() == cLog2by)
                    {
                        // exp(log2by(x,y)) -> exp2(log2(x)*y*CONSTANT_L2I)
                        // This so that y*CONSTANT_L2I gets a chance for
                        // constant folding. log2by() is regenerated thereafter.
                        p0.CopyOnWrite();
                        mul.AddParamMove(p0.GetParam(1));
                        p0.DelParam(1);
                        p0.SetOpcode(cLog2);
                        p0.Rehash();
                    }
                    mul.AddParamMove(p0);
                    mul.AddParam(CodeTree(CONSTANT_L2I));
                    mul.Rehash();
                    SetOpcode(cExp2);
                    SetParamMove(0, mul);
                    changed = true;
                }
                break;
            }
            case cAsin:
            {
                if(prefer_base2) // asin(x) = atan2(x, sqrt(1-x*x))
                {
                    CodeTree p0 = GetParam(0);
                    CodeTree op_a;
                    op_a.SetOpcode(cSqr); op_a.AddParam(p0);
                    op_a.Rehash();
                    CodeTree op_c;
                    op_c.SetOpcode(cSub); op_c.AddParam(CodeTree(1.0)); op_c.AddParamMove(op_a);
                    op_c.Rehash();
                    CodeTree op_d;
                    op_d.SetOpcode(cSqrt); op_d.AddParamMove(op_c);
                    op_d.Rehash();
                    SetOpcode(cAtan2); DelParams(); AddParamMove(p0); AddParamMove(op_d);
                }
                break;
            }
            case cAcos:
            {
                if(prefer_base2) // acos(x) = atan2(sqrt(1-x*x), x)
                {
                    CodeTree p0 = GetParam(0);
                    CodeTree op_a;
                    op_a.SetOpcode(cSqr); op_a.AddParam(p0);
                    op_a.Rehash();
                    CodeTree op_c;
                    op_c.SetOpcode(cSub); op_c.AddParam(CodeTree(1.0)); op_c.AddParamMove(op_a);
                    op_c.Rehash();
                    CodeTree op_d;
                    op_d.SetOpcode(cSqrt); op_d.AddParamMove(op_c);
                    op_d.Rehash();
                    SetOpcode(cAtan2); DelParams(); AddParamMove(op_d); AddParamMove(p0);
                }
                break;
            }
            case cPow:
            {
                const CodeTree& p0 = GetParam(0);
                const CodeTree& p1 = GetParam(1);
                if(p1.IsImmed())
                {
                    if(p1.GetImmed() != 0.0 && !p1.IsLongIntegerImmed())
                    {
                        double inverse_exponent = 1.0 / p1.GetImmed();
                        if(inverse_exponent >= -16.0 && inverse_exponent <= 16.0
                        && IsIntegerConst(inverse_exponent))
                        {
                            long sqrt_chain = (long) inverse_exponent;
                            long abs_sqrt_chain = sqrt_chain < 0 ? -sqrt_chain : sqrt_chain;
                            if((abs_sqrt_chain & (abs_sqrt_chain-1)) == 0) // 2, 4, 8 or 16
                            {
                                ChangeIntoSqrtChain(*this, sqrt_chain);
                                changed = true;
                                break;
                            }
                        }
                    }
                    if(!p1.IsLongIntegerImmed())
                    {
                        // x^1.5 is sqrt(x^3)
                        for(int sqrt_count=1; sqrt_count<=4; ++sqrt_count)
                        {
                            double with_sqrt_exponent = p1.GetImmed() * (1 << sqrt_count);
                            if(IsIntegerConst(with_sqrt_exponent))
                            {
                                long int_sqrt_exponent = (long)with_sqrt_exponent;
                                if(int_sqrt_exponent < 0)
                                    int_sqrt_exponent = -int_sqrt_exponent;
                                if(IsOptimizableUsingPowi(int_sqrt_exponent, sqrt_count))
                                {
                                    long sqrt_chain = 1 << sqrt_count;
                                    if(with_sqrt_exponent < 0) sqrt_chain = -sqrt_chain;

                                    CodeTree tmp;
                                    tmp.AddParamMove(GetParam(0));
                                    tmp.AddParam(CodeTree());
                                    ChangeIntoSqrtChain(tmp, sqrt_chain);
                                    tmp.Rehash();
                                    SetParamMove(0, tmp);
                                    SetParam(1, CodeTree(p1.GetImmed() * (double)sqrt_chain));
                                    changed = true;
                                }
                                break;
                            }
                        }
                    }
                }
                if(!p1.IsLongIntegerImmed()
                || !IsOptimizableUsingPowi(p1.GetLongIntegerImmed()))
                {
                    if(p0.IsImmed() && p0.GetImmed() > 0.0)
                    {
                        // Convert into cExp or Exp2.
                        //    x^y = exp(log(x) * y) =
                        //    Can only be done when x is positive, though.
                        if(prefer_base2)
                        {
                            double mulvalue = std::log( p0.GetImmed() ) * CONSTANT_L2I;
                            if(mulvalue == 1.0)
                            {
                                // exp2(1)^x becomes exp2(x)
                                DelParam(0);
                            }
                            else
                            {
                                // exp2(4)^x becomes exp2(4*x)
                                CodeTree exponent;
                                exponent.SetOpcode(cMul);
                                exponent.AddParam( CodeTree( mulvalue ) );
                                exponent.AddParam(p1);
                                exponent.Rehash();
                                SetParamMove(0, exponent);
                                DelParam(1);
                            }
                            SetOpcode(cExp2);
                        }
                        else
                        {
                            double mulvalue = std::log( p0.GetImmed() );
                            if(mulvalue == 1.0)
                            {
                                // exp(1)^x becomes exp(x)
                                DelParam(0);
                            }
                            else
                            {
                                // exp(4)^x becomes exp(4*x)
                                CodeTree exponent;
                                exponent.SetOpcode(cMul);
                                exponent.AddParam( CodeTree( mulvalue ) );
                                exponent.AddParam(p1);
                                exponent.Rehash();
                                SetParamMove(0, exponent);
                                DelParam(1);
                            }
                            SetOpcode(cExp);
                        }
                        changed = true;
                    }
                    else if(p1.IsImmed() && !p1.IsLongIntegerImmed())
                    {
                        // x^y can be safely converted into exp(y * log(x))
                        // when y is _not_ integer, because we know that x >= 0.
                        // Otherwise either expression will give a NaN or inf.
                        if(prefer_base2)
                        {
                            CodeTree log;
                            log.SetOpcode(cLog2);
                            log.AddParam(p0);
                            log.Rehash();
                            CodeTree exponent;
                            exponent.SetOpcode(cMul);
                            exponent.AddParam(p1);
                            exponent.AddParamMove(log);
                            exponent.Rehash();
                            SetOpcode(cExp2);
                            SetParamMove(0, exponent);
                            DelParam(1);
                        }
                        else
                        {
                            CodeTree log;
                            log.SetOpcode(cLog);
                            log.AddParam(p0);
                            log.Rehash();
                            CodeTree exponent;
                            exponent.SetOpcode(cMul);
                            exponent.AddParam(p1);
                            exponent.AddParamMove(log);
                            exponent.Rehash();
                            SetOpcode(cExp);
                            SetParamMove(0, exponent);
                            DelParam(1);
                        }
                        changed = true;
                    }
                }
                break;
            }

            default: break;
        }

        if(changed)
            goto exit_changed;

        return changed;
    }

    void CodeTree::SynthesizeByteCode(
        std::vector<unsigned>& ByteCode,
        std::vector<double>&   Immed,
        size_t& stacktop_max)
    {
    #ifdef DEBUG_SUBSTITUTIONS
        std::cout << "Making bytecode for:\n";
        FPoptimizer_Grammar::DumpTreeWithIndent(*this); root=this;
    #endif
        while(RecreateInversionsAndNegations())
        {
        #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "One change issued, produced:\n";
            FPoptimizer_Grammar::DumpTreeWithIndent(*root);
        #endif
            FixIncompleteHashes();
        }
    #ifdef DEBUG_SUBSTITUTIONS
        std::cout << "After recreating inv/neg:  "; FPoptimizer_Grammar::DumpTree(*this); std::cout << "\n";
    #endif

        FPoptimizer_ByteCode::ByteCodeSynth synth;

      { // begin scope for TreeCounts, AlreadyDoneTrees
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
            const fphash_t& hash = i->first;
            size_t         score = i->second.first;
            const CodeTree& tree = i->second.second;
            // It must always occur at least twice
            if(score < 2) continue;
            // And it must not be a simple expression
            if(tree.GetDepth() < 2) CandSkip: continue;
            // And it must not yet have been synthesized
            DoneTreesType::const_iterator j = AlreadyDoneTrees.lower_bound(hash);
            for(; j != AlreadyDoneTrees.end() && j->first == hash; ++j)
            {
                if(j->second.IsIdenticalTo(tree))
                    goto CandSkip;
            }
            // Is a candidate.
            score *= tree.GetDepth();
            if(score > best_score)
                { best_score = score; synth_it = i; }
        }
        if(best_score > 0)
        {
    #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "Found Common Subexpression:"; FPoptimizer_Grammar::DumpTree(synth_it->second.second); std::cout << "\n";
    #endif
            /* Synthesize the selected tree */
            synth_it->second.second.SynthesizeByteCode(synth);
            /* Add the tree and all its children to the AlreadyDoneTrees list,
             * to prevent it from being re-synthesized
             */
            RememberRecursivelyHashList(AlreadyDoneTrees, synth_it->second.second);
            goto FindMore;
        }
      } // end scope for TreeCounts, AlreadyDoneTrees

    #ifdef DEBUG_SUBSTITUTIONS
        std::cout << "Actually synthesizing:\n";
        FPoptimizer_Grammar::DumpTreeWithIndent(*this);
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
