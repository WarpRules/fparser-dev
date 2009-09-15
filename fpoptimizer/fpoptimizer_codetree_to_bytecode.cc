#include <cmath>
#include <list>
#include <cassert>

#include "fpoptimizer_codetree.hh"
#include "fptypes.hh"
#include "fpoptimizer_consts.hh"
#include "fpoptimizer_bytecodesynth.hh"

#ifdef FP_SUPPORT_OPTIMIZER

using namespace FUNCTIONPARSERTYPES;
//using namespace FPoptimizer_Grammar;

#ifndef FP_GENERATING_POWI_TABLE
static const unsigned MAX_POWI_BYTECODE_LENGTH = 15;
#else
static const unsigned MAX_POWI_BYTECODE_LENGTH = 999;
#endif
static const unsigned MAX_MULI_BYTECODE_LENGTH = 3;

//#define DEBUG_SUBSTITUTIONS

#ifdef DEBUG_SUBSTITUTIONS
namespace FPoptimizer_Grammar
{
    void DumpTree(const FPoptimizer_CodeTree::CodeTree& tree, std::ostream& o = std::cout);
}
#endif

namespace
{
    using namespace FPoptimizer_CodeTree;

    bool AssembleSequence(
                  CodeTree& tree, long count,
                  const FPoptimizer_ByteCode::SequenceOpCode& sequencing,
                  FPoptimizer_ByteCode::ByteCodeSynth& synth,
                  size_t max_bytecode_grow_length);
}

namespace
{
    typedef
        std::map<fphash_t,  std::pair<size_t, CodeTreeP> >
        TreeCountType;
    typedef
        std::multimap<fphash_t, CodeTreeP>
        DoneTreesType;

    void FindTreeCounts(TreeCountType& TreeCounts, CodeTreeP tree)
    {
        TreeCountType::iterator i = TreeCounts.lower_bound(tree->Hash);
        if(i != TreeCounts.end()
        && tree->Hash == i->first
        && tree->IsIdenticalTo( * i->second.second ) )
            i->second.first += 1;
        else
            TreeCounts.insert(i, std::make_pair(tree->Hash, std::make_pair(size_t(1), tree)));

        for(size_t a=0; a<tree->Params.size(); ++a)
            FindTreeCounts(TreeCounts, tree->Params[a]);
    }

    void RememberRecursivelyHashList(DoneTreesType& hashlist,
                                     const CodeTreeP& tree)
    {
        hashlist.insert( std::make_pair(tree->Hash, tree) );
        for(size_t a=0; a<tree->Params.size(); ++a)
            RememberRecursivelyHashList(hashlist, tree->Params[a]);
    }
    void RecreateInversionsAndNegations(CodeTree& tree)
    {
        for(size_t a=0; a<tree.Params.size(); ++a)
            RecreateInversionsAndNegations(*tree.Params[a]);

        bool changed = false;
        switch(tree.Opcode) // Recreate inversions and negations
        {
            case cMul:
            {
                std::vector<CodeTreeP> div_params;

                for(size_t a = tree.Params.size(); a-- > 0; )
                    if(tree.Params[a]->Opcode == cPow
                    && tree.Params[a]->Params[1]->IsImmed()
                    && FloatEqual(tree.Params[a]->Params[1]->GetImmed(), -1.0))
                    {
                        div_params.push_back(tree.Params[a]->Params[0]);
                        tree.Params.erase(tree.Params.begin()+a);
                        changed = true;
                    }
                if(tree.Params.empty() && div_params.size() == 1)
                {
                    tree.Opcode = cInv;
                    tree.AddParam(div_params[0]);
                }
                else if(div_params.size() ==  1)
                {
                    CodeTreeP divgroup(new CodeTree);
                    divgroup->Opcode = cInv;
                    divgroup->SetParams(div_params);
                    tree.AddParam(divgroup);
                }
                else if(!div_params.empty())
                {
                    CodeTreeP divgroup(new CodeTree);
                    divgroup->Opcode = cMul;
                    divgroup->SetParams(div_params);
                    divgroup->ConstantFolding();
                    divgroup->Sort();
                    divgroup->Recalculate_Hash_NoRecursion();
                    CodeTreeP mulgroup(new CodeTree);
                    mulgroup->Opcode = cMul;
                    mulgroup->SetParams(tree.Params);
                    mulgroup->ConstantFolding();
                    mulgroup->Sort();
                    mulgroup->Recalculate_Hash_NoRecursion();
                    tree.Params.clear();
                    if(mulgroup->IsImmed() && FloatEqual(mulgroup->GetImmed(), 1.0))
                        tree.Opcode = cInv;
                    else
                    {
                        tree.Opcode = cDiv;
                        tree.AddParam(mulgroup);
                    }
                    tree.AddParam(divgroup);
                }
                break;
            }
            case cAdd:
            {
                std::vector<CodeTreeP> sub_params;

                for(size_t a = tree.Params.size(); a-- > 0; )
                    if(tree.Params[a]->Opcode == cMul)
                    {
                        bool is_signed = false;
                        // if the mul group has a -1 constant...
                        bool subchanged = false;
                        CodeTree& mulgroup = *tree.Params[a];
                        for(size_t b=mulgroup.Params.size(); b-- > 0; )
                            if(mulgroup.Params[b]->IsImmed()
                            && FloatEqual(mulgroup.Params[b]->GetImmed(), -1.0))
                            {
                                mulgroup.Params.erase(mulgroup.Params.begin()+b);
                                is_signed = !is_signed;
                                subchanged = true;
                            }
                        if(subchanged)
                        {
                            mulgroup.ConstantFolding();
                            mulgroup.Sort();
                            mulgroup.Recalculate_Hash_NoRecursion();
                            changed = true;
                        }
                        if(is_signed)
                        {
                            sub_params.push_back(tree.Params[a]);
                            tree.Params.erase(tree.Params.begin()+a);
                            changed = true;
                        }
                    }
                if(tree.Params.empty() && sub_params.size() == 1)
                {
                    tree.Opcode = cNeg;
                    tree.AddParam(sub_params[0]);
                }
                else if(sub_params.size() ==  1)
                {
                    CodeTreeP subgroup(new CodeTree);
                    subgroup->Opcode = cNeg;
                    subgroup->SetParams(sub_params);
                    tree.AddParam(subgroup);
                }
                else if(!sub_params.empty())
                {
                    CodeTreeP subgroup(new CodeTree);
                    subgroup->Opcode = cAdd;
                    subgroup->SetParams(sub_params);
                    subgroup->ConstantFolding();
                    subgroup->Sort();
                    subgroup->Recalculate_Hash_NoRecursion();
                    CodeTreeP addgroup(new CodeTree);
                    addgroup->Opcode = cAdd;
                    addgroup->SetParams(tree.Params);
                    addgroup->ConstantFolding();
                    addgroup->Sort();
                    addgroup->Recalculate_Hash_NoRecursion();
                    tree.Params.clear();
                    if(addgroup->IsImmed() && FloatEqual(addgroup->GetImmed(), 0.0))
                        tree.Opcode = cNeg;
                    else
                    {
                        tree.Opcode = cSub;
                        tree.AddParam(addgroup);
                    }
                    tree.AddParam(subgroup);
                }
            }
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
            tree.Sort();
            tree.Rehash(true);
        }
    }
}

namespace FPoptimizer_CodeTree
{
    void CodeTree::SynthesizeByteCode(
        std::vector<unsigned>& ByteCode,
        std::vector<double>&   Immed,
        size_t& stacktop_max)
    {
        RecreateInversionsAndNegations(*this);

        FPoptimizer_ByteCode::ByteCodeSynth synth;

        /* Find common subtrees */
        TreeCountType TreeCounts;
        FindTreeCounts(TreeCounts, this);

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
            if(i->second.second->Depth < 2) CandSkip: continue;
            // And it must not yet have been synthesized
            DoneTreesType::const_iterator j = AlreadyDoneTrees.lower_bound(i->first);
            for(; j != AlreadyDoneTrees.end() && j->first == i->first; ++j)
            {
                if(j->second->IsIdenticalTo(*i->second.second))
                    goto CandSkip;
            }
            // Is a candidate.
            score *= i->second.second->Depth;
            if(score > best_score)
                { best_score = score; synth_it = i; }
        }
        if(best_score > 0)
        {
            /* Synthesize the selected tree */
            synth_it->second.second->SynthesizeByteCode(synth);
            /* Add the tree and all its children to the AlreadyDoneTrees list,
             * to prevent it from being re-synthesized
             */
            RememberRecursivelyHashList(AlreadyDoneTrees, synth_it->second.second);
            goto FindMore;
        }

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

    void CodeTree::SynthesizeByteCode(FPoptimizer_ByteCode::ByteCodeSynth& synth)
    {
        // If the synth can already locate our operand in the stack,
        // never mind synthesizing it again, just dup it.
        /* FIXME: Possible hash collisions. */
        if(synth.FindAndDup(Hash))
        {
            return;
        }

        switch(Opcode)
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
                /*
                // Try to ensure that Immeds don't have a sign
                for(size_t a=0; a<Params.size(); ++a)
                {
                    CodeTreeP& param = Params[a];
                    if(Params[a].sign && param->IsImmed())
                        switch(Opcode)
                        {
                            case cAdd: param->NegateImmed(); Params[a].sign=false; break;
                            case cMul: if(param->GetImmed() == 0.0) break;
                                       param->InvertImmed(); Params[a].sign=false; break;
                            case cAnd:
                            case cOr:  param->NotTheImmed(); Params[a].sign=false; break;
                        }
                }
                */
                if(Opcode == cMul) // Special treatment for cMul sequences
                {
                    // If the paramlist contains an Immed, and that Immed
                    // fits in a long-integer, try to synthesize it
                    // as add-sequences instead.
                    for(size_t a=0; a<Params.size(); ++a)
                    {
                        if(Params[a]->IsLongIntegerImmed())
                        {
                            CodeTreeP param = Params[a];

                            long value = param->GetLongIntegerImmed();
                            Params.erase(Params.begin()+a);

                            bool success = AssembleSequence(
                                *this, value, FPoptimizer_ByteCode::AddSequence,
                                synth,
                                MAX_MULI_BYTECODE_LENGTH);

                            // Readd the token so that we don't need
                            // to deal with allocationd/deallocation here.
                            Params.insert(Params.begin()+a, param);

                            if(success)
                            {
                                // this tree was treated just fine
                                synth.StackTopIs(Hash);
                                return;
                            }
                        }
                    }
                }

                int n_stacked = 0;
                for(size_t a=0; a<Params.size(); ++a)
                {
                    CodeTreeP const & param = Params[a];

                    param->SynthesizeByteCode(synth);
                    ++n_stacked;

                    if(n_stacked > 1)
                    {
                        // Cumulate at the earliest opportunity.
                        synth.AddOperation(Opcode, 2); // stack state: -2+1 = -1
                        n_stacked = n_stacked - 2 + 1;
                    }
                }
                if(n_stacked == 0)
                {
                    // Uh, we got an empty cAdd/cMul/whatever...
                    // Synthesize a default value.
                    // This should never happen.
                    switch(Opcode)
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
                    }
                    ++n_stacked;
                }
                assert(n_stacked == 1);
                break;
            }
            case cPow:
            {
                const CodeTreeP& p0 = Params[0];
                const CodeTreeP& p1 = Params[1];

                if(p1->IsImmed() && p1->GetImmed() == 0.5)
                {
                    p0->SynthesizeByteCode(synth);
                    synth.AddOperation(cSqrt, 1);
                }
                else if(p1->IsImmed() && p1->GetImmed() == -0.5)
                {
                    p0->SynthesizeByteCode(synth);
                    synth.AddOperation(cRSqrt, 1);
                }
                /*
                else if(p0->IsImmed() && p0->GetImmed() == CONSTANT_E)
                {
                    p1->SynthesizeByteCode(synth);
                    synth.AddOperation(cExp, 1);
                }
                else if(p0->IsImmed() && p0->GetImmed() == CONSTANT_EI)
                {
                    p1->SynthesizeByteCode(synth);
                    synth.AddOperation(cNeg, 1);
                    synth.AddOperation(cExp, 1);
                }
                */
                else if(!p1->IsLongIntegerImmed()
                || !AssembleSequence( /* Optimize integer exponents */
                        *p0, p1->GetLongIntegerImmed(),
                        FPoptimizer_ByteCode::MulSequence,
                        synth,
                        MAX_POWI_BYTECODE_LENGTH)
                  )
                {
                    if(p0->IsImmed() && p0->GetImmed() > 0.0)
                    {
                        // Convert into cExp or Exp2.
                        //    x^y = exp(log(x) * y) =
                        //    Can only be done when x is positive, though.
                        double mulvalue = std::log( p0->GetImmed() );

                        if(p1->Opcode == cMul)
                        {
                            // Neat, we can delegate the multiplication to the child
                            p1->AddParam( new CodeTree(mulvalue) );
                            p1->ConstantFolding();
                            p1->Sort();
                            p1->Recalculate_Hash_NoRecursion();
                            mulvalue = 1.0;
                        }

                        // If the exponent needs multiplication, multiply it
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
                                p1->SynthesizeByteCode(synth);
                                synth.PushImmed(mulvalue);
                                synth.AddOperation(cMul, 2);
                                synth.AddOperation(cExp2, 1);
                            }
                        }*/
                        else
                        {
                            // Do with cMul and cExp
                            p1->SynthesizeByteCode(synth);
                            synth.PushImmed(mulvalue);
                            synth.AddOperation(cMul, 2);
                            synth.AddOperation(cExp, 1);
                        }
                    }
                    else
                    {
                        p0->SynthesizeByteCode(synth);
                        p1->SynthesizeByteCode(synth);
                        synth.AddOperation(Opcode, 2); // Create a vanilla cPow.
                    }
                }
                break;
            }
            case cIf:
            {
                size_t ofs;
                // If the parameter amount is != 3, we're screwed.
                Params[0]->SynthesizeByteCode(synth); // expression
                synth.SynthIfStep1(ofs);
                Params[1]->SynthesizeByteCode(synth); // true branch
                synth.SynthIfStep2(ofs);
                Params[2]->SynthesizeByteCode(synth); // false branch
                synth.SynthIfStep3(ofs);
                break;
            }
            case cFCall:
            {
                // If the parameter count is invalid, we're screwed.
                for(size_t a=0; a<Params.size(); ++a)
                    Params[a]->SynthesizeByteCode(synth);
                synth.AddOperation(Opcode, (unsigned) Params.size());
                synth.AddOperation(Funcno, 0, 0);
                break;
            }
            case cPCall:
            {
                // If the parameter count is invalid, we're screwed.
                for(size_t a=0; a<Params.size(); ++a)
                    Params[a]->SynthesizeByteCode(synth);
                synth.AddOperation(Opcode, (unsigned) Params.size());
                synth.AddOperation(Funcno, 0, 0);
                break;
            }
            default:
            {
                // If the parameter count is invalid, we're screwed.
                for(size_t a=0; a<Params.size(); ++a)
                    Params[a]->SynthesizeByteCode(synth);
                synth.AddOperation(Opcode, (unsigned) Params.size());
                break;
            }
        }
        synth.StackTopIs(Hash);
    }
}

namespace
{
    bool AssembleSequence(
        CodeTree& tree, long count,
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
