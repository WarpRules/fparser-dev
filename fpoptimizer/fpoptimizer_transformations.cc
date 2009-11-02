#include "fpoptimizer_bytecodesynth.hh"
#include "fpoptimizer_codetree.hh"
#include "fpoptimizer_consts.hh"

#ifdef FP_SUPPORT_OPTIMIZER

using namespace FUNCTIONPARSERTYPES;
//using namespace FPoptimizer_Grammar;

namespace
{
    using namespace FPoptimizer_CodeTree;

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

    bool IsOptimizableUsingPowi(long immed, long penalty = 0)
    {
        FPoptimizer_ByteCode::ByteCodeSynth synth;
        synth.PushVar(0);
        // Ignore the size generated by subtree
        size_t bytecodesize_backup = synth.GetByteCodeSize();
        FPoptimizer_ByteCode::AssembleSequence(immed, FPoptimizer_ByteCode::MulSequence, synth);

        size_t bytecode_grow_amount = synth.GetByteCodeSize() - bytecodesize_backup;

        return bytecode_grow_amount < size_t(MAX_POWI_BYTECODE_LENGTH - penalty);
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
                CodeTree found_log2, found_log2by;

                if(true)
                {
                    /* This lengthy bit of code
                     * changes log2(x)^3 * 5
                     * to      log2by(x, 5^(1/3)) ^ 3
                     * which is better for runtime
                     * than    log2by(x,1)^3 * 5
                     */
                    bool found_log2_on_exponent = false;
                    double log2_exponent = 0;
                    for(size_t a = GetParamCount(); a-- > 0; )
                    {
                        const CodeTree& powgroup = GetParam(a);
                        if(powgroup.GetOpcode() == cPow
                        && powgroup.GetParam(0).GetOpcode() == cLog2
                        && powgroup.GetParam(1).IsImmed())
                        {
                            // Found log2 on exponent
                            found_log2_on_exponent = true;
                            log2_exponent = powgroup.GetParam(1).GetImmed();
                            break;
                        }
                    }
                    if(found_log2_on_exponent)
                    {
                        double immeds = 1.0;
                        for(size_t a = GetParamCount(); a-- > 0; )
                        {
                            const CodeTree& powgroup = GetParam(a);
                            if(powgroup.IsImmed())
                            {
                                immeds *= powgroup.GetImmed();
                                DelParam(a);
                            }
                        }
                        for(size_t a = GetParamCount(); a-- > 0; )
                        {
                            CodeTree& powgroup = GetParam(a);
                            if(powgroup.GetOpcode() == cPow
                            && powgroup.GetParam(0).GetOpcode() == cLog2
                            && powgroup.GetParam(1).IsImmed())
                            {
                                CodeTree& log2 = powgroup.GetParam(0);
                                log2.CopyOnWrite();
                                log2.SetOpcode(cLog2by);
                                log2.AddParam( CodeTree( fp_pow(immeds, 1.0 / log2_exponent) ) );
                                log2.Rehash();
                                break;
                            }
                        }
                    }
                }

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
                    else if(powgroup.GetOpcode() == cLog2by && !found_log2by.IsDefined())
                    {
                        found_log2by = powgroup;
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
                    /*else if(mulgroup.IsImmed() && FloatEqual(mulgroup.GetImmed(), -1.0))
                    {
                        CodeTree invgroup;
                        invgroup.SetOpcode(cInv);
                        invgroup.AddParamMove(divgroup);
                        invgroup.Rehash();
                        SetOpcode(cNeg);
                        AddParamMove(invgroup);
                    }*/
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
                    mulgroup.SetOpcode(GetOpcode());
                    mulgroup.SetParamsMove(GetParams());
                    mulgroup.Rehash();
                    while(mulgroup.RecreateInversionsAndNegations(prefer_base2))
                        mulgroup.FixIncompleteHashes();
                    SetOpcode(cLog2by);
                    AddParamMove(found_log2);
                    AddParamMove(mulgroup);
                    changed = true;
                }
                if(found_log2by.IsDefined())
                {
                    CodeTree mulgroup;
                    mulgroup.SetOpcode(cMul);
                    mulgroup.AddParamMove(found_log2by.GetParam(1));
                    mulgroup.AddParamsMove(GetParams());
                    mulgroup.Rehash();
                    while(mulgroup.RecreateInversionsAndNegations(prefer_base2))
                        mulgroup.FixIncompleteHashes();
                    DelParams();
                    SetOpcode(cLog2by);
                    AddParamMove(found_log2by.GetParam(0));
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
                    else if(GetParam(a).GetOpcode() == cDiv)
                    {
                        bool is_signed = false;
                        CodeTree& divgroup = GetParam(a);
                        if(divgroup.GetParam(0).IsImmed())
                        {
                            if(FloatEqual(divgroup.GetParam(0).GetImmed(), -1.0))
                            {
                                divgroup.CopyOnWrite();
                                divgroup.DelParam(0);
                                divgroup.SetOpcode(cInv);
                                is_signed = !is_signed;
                            }
                        }
                        if(is_signed)
                        {
                            divgroup.Rehash();
                            sub_params.push_back(divgroup);
                            CopyOnWrite();
                            DelParam(a);
                        }
                    }
                    else if(GetParam(a).GetOpcode() == cRDiv)
                    {
                        bool is_signed = false;
                        CodeTree& divgroup = GetParam(a);
                        if(divgroup.GetParam(1).IsImmed())
                        {
                            if(FloatEqual(divgroup.GetParam(1).GetImmed(), -1.0))
                            {
                                divgroup.CopyOnWrite();
                                divgroup.DelParam(1);
                                divgroup.SetOpcode(cInv);
                                is_signed = !is_signed;
                            }
                        }
                        if(is_signed)
                        {
                            divgroup.Rehash();
                            sub_params.push_back(divgroup);
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
#if 0
            case cLog:
            {
                if(prefer_base2)
                {
                    CodeTree log2;
                    log2.SetOpcode(cLog2),
                    log2.SetParamsMove(GetParams());
                    log2.Rehash();
                    SetOpcode(cMul);
                    AddParamMove(log2);
                    AddParam(CodeTree(CONSTANT_L2));
                    changed = true;
                }
                break;
            }
            case cLog10:
            {
                if(prefer_base2)
                {
                    CodeTree log2;
                    log2.SetOpcode(cLog2),
                    log2.SetParamsMove(GetParams());
                    log2.Rehash();
                    SetOpcode(cMul);
                    AddParamMove(log2);
                    AddParam(CodeTree(CONSTANT_L10B));
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
                    changed = true;
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
                    changed = true;
                }
                break;
            }

            case cSinh:
            {
                if(prefer_base2) // sin(x) = (exp(x) - 1/exp(x)) * 0.5
                {
                    CodeTree exp;
                    exp.SetOpcode(cExp); exp.AddParam(GetParam(0)); exp.Rehash();
                    CodeTree exp_inv;
                    exp_inv.SetOpcode(cInv); exp_inv.AddParam(exp); exp_inv.Rehash();
                    CodeTree sub;
                    sub.SetOpcode(cSub);
                    sub.AddParamMove(exp);
                    sub.AddParamMove(exp_inv);
                    sub.Rehash();
                    SetOpcode(cMul);
                    SetParamMove(0, sub);
                    AddParam(CodeTree(0.5));
                    changed = true;
                }
                break;
            }

            case cCosh:
            {
                if(prefer_base2) // sin(x) = (exp(x) + 1/exp(x)) * 0.5
                {
                    CodeTree exp;
                    exp.SetOpcode(cExp); exp.AddParam(GetParam(0)); exp.Rehash();
                    CodeTree exp_inv;
                    exp_inv.SetOpcode(cInv); exp_inv.AddParam(exp); exp_inv.Rehash();
                    CodeTree sub;
                    sub.SetOpcode(cAdd);
                    sub.AddParamMove(exp);
                    sub.AddParamMove(exp_inv);
                    sub.Rehash();
                    SetOpcode(cMul);
                    SetParamMove(0, sub);
                    AddParam(CodeTree(0.5));
                    changed = true;
                }
                break;
            }

            case cTanh:
            {
                if(prefer_base2)
                {
                    // tanh(x) = sinh(x) / cosh(x)
                    //         = (exp(2*x)-1) / (exp(2*x)+1)
                    CodeTree xdup;
                    xdup.SetOpcode(cAdd);
                    xdup.AddParam(GetParam(0));
                    xdup.AddParam(GetParam(0));
                    xdup.Rehash();
                    CodeTree exp;
                    exp.SetOpcode(cExp); exp.AddParamMove(xdup); exp.Rehash();
                    CodeTree m1,p1;
                    m1.SetOpcode(cAdd); m1.AddParam(exp); m1.AddParam(CodeTree(-1.0)); m1.Rehash();
                    p1.SetOpcode(cAdd); p1.AddParam(exp); p1.AddParam(CodeTree( 1.0)); p1.Rehash();

                    SetOpcode(cDiv);
                    SetParamMove(0, m1);
                    AddParamMove(p1);
                    changed = true;
                }
                break;
            }

            case cAtanh:
            {
                if(prefer_base2)
                {
                    // atanh(x) = log( (1+x) / (1-x)) * 0.5
                    //          = log2by( (1+x) / (1-x), 0.5 * CONSTANT_L2 )
                    CodeTree p1, m1;
                    p1.SetOpcode(cAdd); p1.AddParam(CodeTree(1.0)); p1.AddParam(GetParam(0)); p1.Rehash();
                    m1.SetOpcode(cSub); m1.AddParam(CodeTree(1.0)); m1.AddParam(GetParam(0)); m1.Rehash();
                    CodeTree div;
                    div.SetOpcode(cDiv);
                    div.AddParamMove(p1);
                    div.AddParamMove(m1);
                    div.Rehash();
                    SetOpcode(cLog2by);
                    SetParamMove(0, div);
                    AddParam( CodeTree(0.5 * CONSTANT_L2) );
                    changed = true;
                }
                break;
            }

            case cAsinh:
            {
                if(prefer_base2)
                {
                    // asinh(x) = log(x + sqrt(x*x + 1))
                    CodeTree xsqr;
                    xsqr.SetOpcode(cSqr); xsqr.AddParam(GetParam(0)); xsqr.Rehash();
                    CodeTree xsqrp1;
                    xsqrp1.SetOpcode(cAdd); xsqrp1.AddParamMove(xsqr);
                    xsqrp1.AddParam(CodeTree(1.0)); xsqrp1.Rehash();
                    CodeTree sqrt;
                    sqrt.SetOpcode(cSqrt); sqrt.AddParamMove(xsqrp1); sqrt.Rehash();
                    CodeTree add;
                    add.SetOpcode(cAdd); add.SetParamsMove(GetParams());
                    add.AddParamMove(sqrt);
                    SetOpcode(cLog);
                    AddParamMove(add);
                    changed = true;
                }
                break;
            }

            case cAcosh:
            {
                if(prefer_base2)
                {
                    // acosh(x) = log(x + sqrt(x*x - 1))
                    CodeTree xsqr;
                    xsqr.SetOpcode(cSqr); xsqr.AddParam(GetParam(0)); xsqr.Rehash();
                    CodeTree xsqrp1;
                    xsqrp1.SetOpcode(cAdd); xsqrp1.AddParamMove(xsqr);
                    xsqrp1.AddParam(CodeTree(-1.0)); xsqrp1.Rehash();
                    CodeTree sqrt;
                    sqrt.SetOpcode(cSqrt); sqrt.AddParamMove(xsqrp1); sqrt.Rehash();
                    CodeTree add;
                    add.SetOpcode(cAdd); add.SetParamsMove(GetParams());
                    add.AddParamMove(sqrt);
                    SetOpcode(cLog);
                    AddParamMove(add);
                    changed = true;
                }
                break;
            }
#endif
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
                        && IsIntegerConst(inverse_exponent)
                        && (int)inverse_exponent != 1
                        && (int)inverse_exponent != -1)
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

    std::vector<CodeTree> CodeTree::FindCommonSubExpressions()
    {
        std::vector<CodeTree> result;

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
            std::cout << "Found Common Subexpression:"; DumpTree(synth_it->second.second); std::cout << "\n";
    #endif
            /* Synthesize the selected tree */
            result.push_back(synth_it->second.second);
            /* Add the tree and all its children to the AlreadyDoneTrees list,
             * to prevent it from being re-synthesized
             */
            RememberRecursivelyHashList(AlreadyDoneTrees, synth_it->second.second);
            goto FindMore;
        }

        return result;
    }
}

#endif
