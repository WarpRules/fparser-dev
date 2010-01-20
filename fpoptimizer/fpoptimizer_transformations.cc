#include "fpoptimizer_bytecodesynth.hh"
#include "fpoptimizer_codetree.hh"
#include "fpoptimizer_consts.hh"

#ifdef FP_SUPPORT_OPTIMIZER

using namespace FUNCTIONPARSERTYPES;
//using namespace FPoptimizer_Grammar;

//#define DEBUG_POWI
//#define DEBUG_SUBSTITUTIONS_CSE

#if defined(__x86_64) || !defined(FP_SUPPORT_CBRT)
# define CBRT_IS_SLOW
#endif


namespace FPoptimizer_ByteCode
{
    extern const unsigned char powi_table[256];
}
namespace
{
    using namespace FPoptimizer_CodeTree;

    class TreeCountItem
    {
        size_t n_occurrences;
        size_t n_as_cos_param;
        size_t n_as_sin_param;
    public:
        TreeCountItem() :
            n_occurrences(0),
            n_as_cos_param(0),
            n_as_sin_param(0) { }

        void AddFrom(OPCODE op)
        {
            n_occurrences += 1;
            if(op == cCos) ++n_as_cos_param;
            if(op == cSin) ++n_as_sin_param;
            if(op == cSec) ++n_as_cos_param;
            if(op == cCsc) ++n_as_sin_param;
        }

        size_t GetCSEscore() const
        {
            //size_t n_sincos = std::min(n_as_cos_param, n_as_sin_param);
            size_t result = n_occurrences;// - n_sincos;
            return result;
        }

        int NeedsSinCos() const
        {
            if(n_as_cos_param > 0 && n_as_sin_param > 0)
            {
                if(n_occurrences == n_as_cos_param + n_as_sin_param)
                    return 1;
                return 2;
            }
            return 0;
        }

        size_t MinimumDepth() const
        {
            size_t n_sincos = std::min(n_as_cos_param, n_as_sin_param);
            if(n_sincos == 0)
                return 2;
            return 1;
        }
    };

    typedef
        std::multimap<fphash_t,  std::pair<TreeCountItem, CodeTree> >
        TreeCountType;

    /* TODO: Do SinCos optimizations, for example sin(x^2)+cos(x^2) */

    void FindTreeCounts(
        TreeCountType& TreeCounts, const CodeTree& tree,
        OPCODE parent_opcode)
    {
        TreeCountType::iterator i = TreeCounts.lower_bound(tree.GetHash());
        bool found = false;
        for(; i != TreeCounts.end() && i->first == tree.GetHash(); ++i)
        {
            if(tree.IsIdenticalTo( i->second.second ) )
            {
                i->second.first.AddFrom(parent_opcode);
                found = true;
                break;
            }
        }
        if(!found)
        {
            TreeCountItem count;
            count.AddFrom(parent_opcode);
            TreeCounts.insert(i, std::make_pair(tree.GetHash(),
                std::make_pair(count, tree)));
        }

        for(size_t a=0; a<tree.GetParamCount(); ++a)
            FindTreeCounts(TreeCounts, tree.GetParam(a),
                           tree.GetOpcode());
    }

    struct BalanceResultType
    {
        bool BalanceGood;
        bool FoundChild;
    };
    BalanceResultType IfBalanceGood(const CodeTree& root, const CodeTree& child)
    {
        if(root.IsIdenticalTo(child))
        {
            BalanceResultType result = {true,true};
            return result;
        }

        BalanceResultType result = {true,false};

        if(root.GetOpcode() == cIf
        || root.GetOpcode() == cAbsIf)
        {
            BalanceResultType cond    = IfBalanceGood(root.GetParam(0), child);
            BalanceResultType branch1 = IfBalanceGood(root.GetParam(1), child);
            BalanceResultType branch2 = IfBalanceGood(root.GetParam(2), child);

            if(cond.FoundChild || branch1.FoundChild || branch2.FoundChild)
                { result.FoundChild = true; }

            // balance is good if:
            //      branch1.found = branch2.found OR (cond.found AND cond.goodbalance)
            // AND  cond.goodbalance OR (branch1.found AND branch2.found)
            // AND  branch1.goodbalance OR (cond.found AND cond.goodbalance)
            // AND  branch2.goodbalance OR (cond.found AND cond.goodbalance)

            result.BalanceGood =
                (   (branch1.FoundChild == branch2.FoundChild)
                 || (cond.FoundChild && cond.BalanceGood) )
             && (cond.BalanceGood || (branch1.FoundChild && branch2.FoundChild))
             && (branch1.BalanceGood || (cond.FoundChild && cond.BalanceGood))
             && (branch2.BalanceGood || (cond.FoundChild && cond.BalanceGood));
        }
        else
        {
            bool has_bad_balance        = false;
            bool has_good_balance_found = false;

            // Balance is bad if one of the children has bad balance
            // Unless one of the children has good balance & found

            for(size_t b=root.GetParamCount(), a=0; a<b; ++a)
            {
                BalanceResultType tmp = IfBalanceGood(root.GetParam(a), child);
                if(tmp.FoundChild)
                    result.FoundChild = true;

                if(tmp.BalanceGood == false)
                    has_bad_balance = true;
                else if(tmp.FoundChild)
                    has_good_balance_found = true;

                // if the expression is
                //   if(x, sin(x), 0) + sin(x)
                // then sin(x) is a good subexpression
                // even though it occurs in unbalance.
            }
            if(has_bad_balance && !has_good_balance_found)
                result.BalanceGood = false;
        }
        return result;
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

    void ChangeIntoRootChain(
        CodeTree& tree,
        bool inverted,
        long sqrt_count,
        long cbrt_count)
    {
        while(cbrt_count > 0)
        {
            CodeTree tmp;
            tmp.SetOpcode(cCbrt);
            tmp.AddParamMove(tree);
            tmp.Rehash();
            tree.swap(tmp);
            --cbrt_count;
        }
        while(sqrt_count > 0)
        {
            CodeTree tmp;
            tmp.SetOpcode(cSqrt);
            if(inverted)
            {
                tmp.SetOpcode(cRSqrt);
                inverted = false;
            }
            tmp.AddParamMove(tree);
            tmp.Rehash();
            tree.swap(tmp);
            --sqrt_count;
        }
        if(inverted)
        {
            CodeTree tmp;
            tmp.SetOpcode(cInv);
            tmp.AddParamMove(tree);
            tree.swap(tmp);
        }
    }

    double CalculatePowiFactorCost(long abs_int_exponent)
    {
        static std::map<long, double> cache;
        std::map<long,double>::iterator i = cache.lower_bound(abs_int_exponent);
        if(i != cache.end() && i->first == abs_int_exponent)
            return i->second;
        std::pair<long, double> result(abs_int_exponent, 0.0);
        double& cost = result.second;

        while(abs_int_exponent > 1)
        {
            int factor = 0;
            if(abs_int_exponent < 256)
            {
                factor = FPoptimizer_ByteCode::powi_table[abs_int_exponent];
                if(factor & 128) factor &= 127; else factor = 0;
                if(factor & 64) factor = -(factor&63) - 1;
            }
            if(factor)
            {
                cost += CalculatePowiFactorCost(factor);
                abs_int_exponent /= factor;
                continue;
            }
            if(!(abs_int_exponent & 1))
            {
                abs_int_exponent /= 2;
                cost += 3; // sqr
            }
            else
            {
                cost += 3.5; // dup+mul
                abs_int_exponent -= 1;
            }
        }

        cache.insert(i, result);
        return cost;
    }

    struct PowiResolver
    {
        /* Any exponentiation can be turned into one of these:
         *
         *   x^y  -> sqrt(x)^(y*2)         = x Sqrt       y*2  Pow
         *   x^y  -> cbrt(x)^(y*3)         = x Cbrt       y*3  Pow
         *   x^y  -> rsqrt(x)^(y*-2)       = x RSqrt     y*-2  Pow
         *   x^y  -> x^(y-1/2) * sqrt(x)   = x Sqrt   x y-0.5  Pow Mul
         *   x^y  -> x^(y-1/3) * cbrt(x)   = x Cbrt   x y-0.33 Pow Mul
         *   x^y  -> x^(y+1/2) * rsqrt(x)  = x Sqrt   x y+0.5  Pow Mul
         *   x^y  -> inv(x)^(-y)           = x Inv      -y     Pow
         *
         * These rules can be applied recursively.
         * The goal is to find the optimal chain of operations
         * that results in the least number of sqrt,cbrt operations;
         * an integer value of y, and that the integer is as close
         * to zero as possible.
         */
        static const unsigned MaxSep = 4;

        struct PowiResult
        {
            PowiResult() :
                n_int_sqrt(0),
                n_int_cbrt(0),
                resulting_exponent(0),
                sep_list() { }

            int n_int_sqrt;
            int n_int_cbrt;
            long resulting_exponent;
            int sep_list[MaxSep];
        };

        PowiResult CreatePowiResult(double exponent) const
        {
            static const double RootPowers[(1+4)*(1+3)] =
            {
                // (sqrt^n(x))
                1.0,
                1.0 / (2),
                1.0 / (2*2),
                1.0 / (2*2*2),
                1.0 / (2*2*2*2),
                // cbrt^1(sqrt^n(x))
                1.0 / (3),
                1.0 / (3*2),
                1.0 / (3*2*2),
                1.0 / (3*2*2*2),
                1.0 / (3*2*2*2*2),
                // cbrt^2(sqrt^n(x))
                1.0 / (3*3),
                1.0 / (3*3*2),
                1.0 / (3*3*2*2),
                1.0 / (3*3*2*2*2),
                1.0 / (3*3*2*2*2*2),
                // cbrt^3(sqrt^n(x))
                1.0 / (3*3*3),
                1.0 / (3*3*3*2),
                1.0 / (3*3*3*2*2),
                1.0 / (3*3*3*2*2*2),
                1.0 / (3*3*3*2*2*2*2)
            };

            PowiResult result;

            int best_factor = FindIntegerFactor(exponent);
            if(best_factor == 0)
            {
        #ifdef DEBUG_POWI
            printf("no factor found for %g\n", exponent);
        #endif
                return result; // Unoptimizable
            }

            double best_cost = EvaluateFactorCost(best_factor, 0, 0, 0)
                             + CalculatePowiFactorCost(long(exponent*best_factor));
            int s_count = 0;
            int c_count = 0;
            int mul_count = 0;

        #ifdef DEBUG_POWI
            printf("orig = %g\n", exponent);
            printf("plain factor = %d, cost %g\n", best_factor, best_cost);
        #endif

            for(unsigned n_s=0; n_s<MaxSep; ++n_s)
            {
                int best_selected_sep = 0;
                double best_sep_cost     = best_cost;
                int best_sep_factor   = best_factor;
                for(int s=1; s<5*4; ++s)
                {
#ifdef CBRT_IS_SLOW
                    if(s >= 5) break;
                    // When cbrt is implemented through exp and log,
                    // there is no advantage over exp(log()), so don't support it.
#endif
                    int n_sqrt = s%5;
                    int n_cbrt = s/5;
                    if(n_sqrt + n_cbrt > 4) continue;

                    double changed_exponent = exponent;
                    changed_exponent -= RootPowers[s];

                    int factor = FindIntegerFactor(changed_exponent);
                    if(factor != 0)
                    {
                        double cost = EvaluateFactorCost
                            (factor, s_count + n_sqrt, c_count + n_cbrt, mul_count + 1)
                          + CalculatePowiFactorCost(long(changed_exponent*factor));

        #ifdef DEBUG_POWI
                        printf("%d sqrt %d cbrt factor = %d, cost %g\n",
                            n_sqrt, n_cbrt, factor, cost);
        #endif
                        if(cost < best_sep_cost)
                        {
                            best_selected_sep = s;
                            best_sep_factor   = factor;
                            best_sep_cost     = cost;
                        }
                    }
                }
                if(!best_selected_sep) break;

                result.sep_list[n_s] = best_selected_sep;
                exponent -= RootPowers[best_selected_sep];
                s_count += best_selected_sep % 5;
                c_count += best_selected_sep / 5;
                best_cost   = best_sep_cost;
                best_factor = best_sep_factor;
                mul_count += 1;
            }

            result.resulting_exponent = (long) (exponent * best_factor + 0.5);
            while(best_factor % 2 == 0)
            {
                ++result.n_int_sqrt;
                best_factor /= 2;
            }
            while(best_factor % 3 == 0)
            {
                ++result.n_int_cbrt;
                best_factor /= 3;
            }
            return result;
        }

    private:
        // Find the integer that "value" must be multiplied
        // with to produce an integer...
        // Consisting of factors 2 and 3 only.
        bool MakesInteger(double value, int factor) const
        {
            double v = value * double(factor);
            double diff = fabs(v - (double)(long)(v+0.5));
            //printf("factor %d: v=%.20f, diff=%.20f\n", factor,v, diff);
            return diff < 1e-9;
        }
        int FindIntegerFactor(double value) const
        {
            int factor = (2*2*2*2);
#ifdef CBRT_IS_SLOW
            // When cbrt is implemented through exp and log,
            // there is no advantage over exp(log()), so don't support it.
#else
            factor *= (3*3*3);
#endif
            int result = 0;
            if(MakesInteger(value, factor))
            {
                result = factor;
                while((factor % 2) == 0 && MakesInteger(value, factor/2))
                    result = factor /= 2;
                while((factor % 3) == 0 && MakesInteger(value, factor/3))
                    result = factor /= 3;
            }
#ifdef CBRT_IS_SLOW
            if(result == 0)
            {
                /* Note: Even if we allow one cbrt,
                 *        cbrt(cbrt(x)) still gets turned into
                 *        exp(log(x)*0.111111)
                 *        which gives an error when x < 0...
                 *        should we use a special system here?
                 *        i.e. exp(log(-5)*y)
                 *      =      -exp(log(5)*y)
                 *        except when y is an even integer,
                 *      when  = exp(log(5)*y)
                 * We use a custom fp_pow() function
                 * in order to handle these situations.
                 */
                if(MakesInteger(value, 3)) return 3; // single cbrt opcode
            }
#endif
            return result;
        }

        int EvaluateFactorCost(int factor, int s, int c, int nmuls) const
        {
            const int sqrt_cost = 6;
#ifdef CBRT_IS_SLOW
            const int cbrt_cost = 25;
#else
            const int cbrt_cost = 8;
#endif
            int result = s * sqrt_cost + c * cbrt_cost;
            while(factor % 2 == 0) { factor /= 2; result += sqrt_cost; }
            while(factor % 3 == 0) { factor /= 3; result += cbrt_cost; }
            result += nmuls;
            return result;
        }
    };
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
            case cPow:
            {
                const CodeTree& p0 = GetParam(0);
                const CodeTree& p1 = GetParam(1);
                if(p1.IsImmed())
                {
                    if(p1.GetImmed() != 0.0 && !p1.IsLongIntegerImmed())
                    {
                        PowiResolver::PowiResult
                            r = PowiResolver().CreatePowiResult(fabs(p1.GetImmed()));

                        if(r.resulting_exponent != 0)
                        {
                            bool signed_chain = false;

                            if(p1.GetImmed() < 0
                            && r.sep_list[0] == 0
                            && r.n_int_sqrt > 0)
                            {
                                // If one of the internal sqrts can be changed into rsqrt
                                signed_chain = true;
                            }

                        #ifdef DEBUG_POWI
                            printf("Will resolve powi %g as powi(chain(%d,%d),%ld)",
                                fabs(p1.GetImmed()),
                                r.n_int_sqrt,
                                r.n_int_cbrt,
                                r.resulting_exponent);
                            for(unsigned n=0; n<PowiResolver::MaxSep; ++n)
                            {
                                if(r.sep_list[n] == 0) break;
                                int n_sqrt = r.sep_list[n] % 5;
                                int n_cbrt = r.sep_list[n] / 5;
                                printf("*chain(%d,%d)", n_sqrt,n_cbrt);
                            }
                            printf("\n");
                        #endif

                            CodeTree source_tree = GetParam(0);

                            CodeTree pow_item = source_tree;
                            pow_item.CopyOnWrite();
                            ChangeIntoRootChain(pow_item,
                                signed_chain,
                                r.n_int_sqrt,
                                r.n_int_cbrt);
                            pow_item.Rehash();

                            CodeTree pow;
                            if(r.resulting_exponent != 1)
                            {
                                pow.SetOpcode(cPow);
                                pow.AddParamMove(pow_item);
                                pow.AddParam(CodeTree( double(r.resulting_exponent) ));
                            }
                            else
                                pow.swap(pow_item);

                            CodeTree mul;
                            mul.SetOpcode(cMul);
                            mul.AddParamMove(pow);

                            for(unsigned n=0; n<PowiResolver::MaxSep; ++n)
                            {
                                if(r.sep_list[n] == 0) break;
                                int n_sqrt = r.sep_list[n] % 5;
                                int n_cbrt = r.sep_list[n] / 5;

                                CodeTree mul_item = source_tree;
                                mul_item.CopyOnWrite();
                                ChangeIntoRootChain(mul_item, false, n_sqrt, n_cbrt);
                                mul_item.Rehash();
                                mul.AddParamMove(mul_item);
                            }

                            if(p1.GetImmed() < 0 && !signed_chain)
                            {
                                mul.Rehash();
                                SetOpcode(cInv);
                                SetParamMove(0, mul);
                                DelParam(1);
                            }
                            else
                            {
                                SetOpcode(cMul);
                                SetParamsMove(mul.GetParams());
                            }
                        #ifdef DEBUG_POWI
                            DumpTreeWithIndent(*this);
                        #endif
                            changed = true;
                            break;
                        }
                    }
                }
                if(GetOpcode() == cPow
                && (!p1.IsLongIntegerImmed()
                 || !IsOptimizableUsingPowi(p1.GetLongIntegerImmed())))
                {
                    if(p0.IsImmed() && p0.GetImmed() > 0.0)
                    {
                        // Convert into cExp or Exp2.
                        //    x^y = exp(log(x) * y) =
                        //    Can only be done when x is positive, though.
                        if(prefer_base2)
                        {
                            double mulvalue = fp_log2( p0.GetImmed() );
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
                            changed = true;
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
                            changed = true;
                        }
                    }
                    else if(p0.IsAlwaysSigned(true))
                    {
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
                            changed = true;
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
                            changed = true;
                        }
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

    bool ContainsOtherCandidates(
        const CodeTree& within,
        const CodeTree& tree,
        const FPoptimizer_ByteCode::ByteCodeSynth& synth,
        const TreeCountType& TreeCounts)
    {
        for(size_t b=tree.GetParamCount(), a=0; a<b; ++a)
        {
            const CodeTree& leaf = tree.GetParam(a);

            TreeCountType::iterator synth_it;
            for(TreeCountType::const_iterator
                i = TreeCounts.begin();
                i != TreeCounts.end();
                ++i)
            {
                if(i->first != leaf.GetHash())
                    continue;

                const TreeCountItem& occ  = i->second.first;
                size_t          score     = occ.GetCSEscore();
                const CodeTree& candidate = i->second.second;

                // It must not yet have been synthesized
                if(synth.Find(candidate))
                    continue;

                // And it must not be a simple expression
                // Because cImmed, VarBegin are faster than cFetch
                if(leaf.GetDepth() < occ.MinimumDepth())
                    continue;

                // It must always occur at least twice
                if(score < 2)
                    continue;

                // And it must either appear on both sides
                // of a cIf, or neither
                if(IfBalanceGood(within, leaf).BalanceGood == false)
                    continue;

                return true;
            }
            if(ContainsOtherCandidates(within, leaf, synth, TreeCounts))
                return true;
        }
        return false;
    }

    bool IsDescendantOf(const CodeTree& parent, const CodeTree& expr)
    {
        for(size_t a=0; a<parent.GetParamCount(); ++a)
            if(parent.GetParam(a).IsIdenticalTo(expr))
                return true;

        for(size_t a=0; a<parent.GetParamCount(); ++a)
            if(IsDescendantOf(parent.GetParam(a), expr))
                return true;

        return false;
    }

    bool GoodMomentForCSE(const CodeTree& parent, const CodeTree& expr)
    {
        if(parent.GetOpcode() == cIf)
            return true;

        // Good if it's one of our direct children
        // Bad if it is a descendant of only one of our children

        for(size_t a=0; a<parent.GetParamCount(); ++a)
            if(parent.GetParam(a).IsIdenticalTo(expr))
                return true;

        size_t leaf_count = 0;
        for(size_t a=0; a<parent.GetParamCount(); ++a)
            if(IsDescendantOf(parent.GetParam(a), expr))
                ++leaf_count;

        return leaf_count != 1;
    }

    size_t CodeTree::SynthCommonSubExpressions(
        FPoptimizer_ByteCode::ByteCodeSynth& synth) const
    {
        size_t stacktop_before = synth.GetStackTop();

        /* Find common subtrees */
        TreeCountType TreeCounts;
        FindTreeCounts(TreeCounts, *this, GetOpcode());

    #ifdef DEBUG_SUBSTITUTIONS_CSE
        DumpHashes(*this);
    #endif

        /* Synthesize some of the most common ones */
        for(;;)
        {
            size_t best_score = 0;
            TreeCountType::iterator synth_it;
            for(TreeCountType::iterator
                j,i = TreeCounts.begin();
                i != TreeCounts.end();
                i=j)
            {
                j=i; ++j;

                const TreeCountItem& occ  = i->second.first;
                size_t          score     = occ.GetCSEscore();
                const CodeTree& tree = i->second.second;

    #ifdef DEBUG_SUBSTITUTIONS_CSE
                std::cout << "Score " << score << ":\n";
                DumpTreeWithIndent(tree);
    #endif

                // It must not yet have been synthesized
                if(synth.Find(tree))
                {
                    TreeCounts.erase(i);
                    continue;
                }

                // And it must not be a simple expression
                // Because cImmed, VarBegin are faster than cFetch
                if(tree.GetDepth() < occ.MinimumDepth())
                {
                    TreeCounts.erase(i);
                    continue;
                }

                // It must always occur at least twice
                if(score < 2)
                {
                    TreeCounts.erase(i);
                    continue;
                }

                // And it must either appear on both sides
                // of a cIf, or neither
                if(IfBalanceGood(*this, tree).BalanceGood == false)
                {
                    TreeCounts.erase(i);
                    continue;
                }

                // It must not contain other candidates
                if(ContainsOtherCandidates(*this, tree, synth, TreeCounts))
                {
                    // Don't erase it; it may be a proper candidate later
                    continue;
                }

                if(!GoodMomentForCSE(*this, tree))
                {
                    TreeCounts.erase(i);
                    continue;
                }

                // Is a candidate.
                score *= tree.GetDepth();
                if(score > best_score)
                    { best_score = score; synth_it = i; }
            }

            if(best_score <= 0) break; // Didn't find anything.

            const TreeCountItem& occ  = synth_it->second.first;
            const CodeTree& tree = synth_it->second.second;
    #ifdef DEBUG_SUBSTITUTIONS_CSE
            std::cout << "Found Common Subexpression:"; DumpTree(tree); std::cout << "\n";
    #endif

            int needs_sincos = occ.NeedsSinCos();
            CodeTree sintree, costree;
            if(needs_sincos)
            {
                sintree.AddParam(tree);
                sintree.SetOpcode(cSin);
                sintree.Rehash();
                costree.AddParam(tree);
                costree.SetOpcode(cCos);
                costree.Rehash();
                if(synth.Find(sintree) || synth.Find(costree))
                {
                    if(needs_sincos == 2)
                    {
                        // sin, cos already found, and we don't
                        // actually need _this_ tree by itself
                        TreeCounts.erase(synth_it);
                        continue;
                    }
                    needs_sincos = 0;
                }
            }

            /* Synthesize the selected tree */
            tree.SynthesizeByteCode(synth, false);
            TreeCounts.erase(synth_it);
    #ifdef DEBUG_SUBSTITUTIONS_CSE
            std::cout << "Done with Common Subexpression:"; DumpTree(tree); std::cout << "\n";
    #endif
            if(needs_sincos)
            {
                if(needs_sincos == 2)
                {
                    // make a duplicate of the value, since it
                    // is also needed in addition to the sin/cos.
                    synth.DoDup(synth.GetStackTop()-1);
                }
                synth.AddOperation(cSinCos, 1, 2);

                synth.StackTopIs(sintree, 1);
                synth.StackTopIs(costree, 0);
            }
        }

        return synth.GetStackTop() - stacktop_before;
    }
}

#endif
