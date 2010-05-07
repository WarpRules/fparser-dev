#include "codetree.hh"

#ifdef FP_SUPPORT_OPTIMIZER

#include "bytecodesynth.hh"
#include "rangeestimation.hh"
#include "optimize.hh" // For DEBUG_SUBSTITUTIONS

using namespace FUNCTIONPARSERTYPES;
//using namespace FPoptimizer_Grammar;

//#define DEBUG_POWI

#if defined(__x86_64) || !defined(FP_SUPPORT_CBRT)
# define CBRT_IS_SLOW
#endif

#if defined(DEBUG_POWI) || defined(DEBUG_SUBSTITUTIONS)
#include <cstdio>
#endif

namespace FPoptimizer_ByteCode
{
    extern const unsigned char powi_table[256];
}
namespace
{
    using namespace FPoptimizer_CodeTree;

    template<typename Value_t>
    bool IsOptimizableUsingPowi(long immed, long penalty = 0)
    {
        FPoptimizer_ByteCode::ByteCodeSynth<Value_t> synth;
        synth.PushVar(VarBegin);
        // Ignore the size generated by subtree
        size_t bytecodesize_backup = synth.GetByteCodeSize();
        FPoptimizer_ByteCode::AssembleSequence(immed,
            FPoptimizer_ByteCode::SequenceOpcodes<Value_t>::MulSequence, synth);

        size_t bytecode_grow_amount = synth.GetByteCodeSize() - bytecodesize_backup;

        return bytecode_grow_amount < size_t(MAX_POWI_BYTECODE_LENGTH - penalty);
    }

    template<typename Value_t>
    void ChangeIntoRootChain(
        CodeTree<Value_t>& tree,
        bool inverted,
        long sqrt_count,
        long cbrt_count)
    {
        while(cbrt_count > 0)
        {
            CodeTree<Value_t> tmp;
            tmp.SetOpcode(cCbrt);
            tmp.AddParamMove(tree);
            tmp.Rehash();
            tree.swap(tmp);
            --cbrt_count;
        }
        while(sqrt_count > 0)
        {
            CodeTree<Value_t> tmp;
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
            CodeTree<Value_t> tmp;
            tmp.SetOpcode(cInv);
            tmp.AddParamMove(tree);
            tree.swap(tmp);
        }
    }

    template<typename Value_t>
    struct RootPowerTable
    {
        static const Value_t RootPowers[(1+4)*(1+3)];
    };
    template<typename Value_t>
    const Value_t RootPowerTable<Value_t>::RootPowers[(1+4)*(1+3)] =
    {
        // (sqrt^n(x))
        Value_t(1),
        Value_t(1) / Value_t(2),
        Value_t(1) / Value_t(2*2),
        Value_t(1) / Value_t(2*2*2),
        Value_t(1) / Value_t(2*2*2*2),
        // cbrt^1(sqrt^n(x))
        Value_t(1) / Value_t(3),
        Value_t(1) / Value_t(3*2),
        Value_t(1) / Value_t(3*2*2),
        Value_t(1) / Value_t(3*2*2*2),
        Value_t(1) / Value_t(3*2*2*2*2),
        // cbrt^2(sqrt^n(x))
        Value_t(1) / Value_t(3*3),
        Value_t(1) / Value_t(3*3*2),
        Value_t(1) / Value_t(3*3*2*2),
        Value_t(1) / Value_t(3*3*2*2*2),
        Value_t(1) / Value_t(3*3*2*2*2*2),
        // cbrt^3(sqrt^n(x))
        Value_t(1) / Value_t(3*3*3),
        Value_t(1) / Value_t(3*3*3*2),
        Value_t(1) / Value_t(3*3*3*2*2),
        Value_t(1) / Value_t(3*3*3*2*2*2),
        Value_t(1) / Value_t(3*3*3*2*2*2*2)
    };

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
        static const int      MaxOp  = 5;

        typedef int factor_t;
        typedef long cost_t;
        typedef long int_exponent_t;

        struct PowiResult
        {
            PowiResult() :
                n_int_sqrt(0),
                n_int_cbrt(0),
                sep_list(),
                resulting_exponent(0) { }

            int n_int_sqrt; // totals
            int n_int_cbrt; // totals
            int sep_list[MaxSep]; // action list. Each element is (n_sqrt + MaxOp * n_cbrt).
            int_exponent_t resulting_exponent;
        };

        template<typename Value_t>
        static PowiResult CreatePowiResult(Value_t exponent)
        {
            PowiResult result;

            factor_t best_factor = FindIntegerFactor(exponent);
            if(best_factor == 0)
            {
        #ifdef DEBUG_POWI
                printf("no factor found for %Lg\n", (long double)exponent);
        #endif
                return result; // Unoptimizable
            }

            result.resulting_exponent = MultiplyAndMakeLong(exponent, best_factor);
            cost_t best_cost =
                EvaluateFactorCost(best_factor, 0, 0, 0)
              + CalculatePowiFactorCost( result.resulting_exponent );
            int s_count = 0;
            int c_count = 0;
            int mul_count = 0;

        #ifdef DEBUG_POWI
            printf("orig = %Lg\n", (long double) exponent);
            printf("plain factor = %d, cost %ld\n", (int) best_factor, (long) best_cost);
        #endif

            for(unsigned n_s=0; n_s<MaxSep; ++n_s)
            {
                int best_selected_sep = 0;
                cost_t best_sep_cost  = best_cost;
                factor_t best_sep_factor = best_factor;
                for(int s=1; s<MaxOp*4; ++s)
                {
#ifdef CBRT_IS_SLOW
                    if(s >= MaxOp) break;
                    // When cbrt is implemented through exp and log,
                    // there is no advantage over exp(log()), so don't support it.
#endif
                    int n_sqrt = s%MaxOp;
                    int n_cbrt = s/MaxOp;
                    if(n_sqrt + n_cbrt > 4) continue;

                    Value_t changed_exponent = exponent;
                    changed_exponent -= RootPowerTable<Value_t>::RootPowers[s];

                    factor_t factor = FindIntegerFactor(changed_exponent);
                    if(factor != 0)
                    {
                        int_exponent_t int_exponent = MultiplyAndMakeLong(changed_exponent, factor);
                        cost_t cost =
                            EvaluateFactorCost(factor, s_count + n_sqrt, c_count + n_cbrt, mul_count + 1)
                          + CalculatePowiFactorCost(int_exponent);

        #ifdef DEBUG_POWI
                        printf("Candidate sep %u (%d*sqrt %d*cbrt)factor = %d, cost %ld (for %Lg to %ld)\n",
                            s, n_sqrt, n_cbrt, factor,
                            (long) cost,
                            (long double) changed_exponent,
                            (long) int_exponent);
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

        #ifdef DEBUG_POWI
                printf("CHOSEN sep %u (%d*sqrt %d*cbrt)factor = %d, cost %ld, exponent %Lg->%Lg\n",
                       best_selected_sep,
                       best_selected_sep % MaxOp,
                       best_selected_sep / MaxOp,
                       best_sep_factor, best_sep_cost,
                       (long double)(exponent),
                       (long double)(exponent-RootPowerTable<Value_t>::RootPowers[best_selected_sep]));
        #endif
                result.sep_list[n_s] = best_selected_sep;
                exponent -= RootPowerTable<Value_t>::RootPowers[best_selected_sep];
                s_count += best_selected_sep % MaxOp;
                c_count += best_selected_sep / MaxOp;
                best_cost   = best_sep_cost;
                best_factor = best_sep_factor;
                mul_count += 1;
            }

            result.resulting_exponent = MultiplyAndMakeLong(exponent, best_factor);
        #ifdef DEBUG_POWI
            printf("resulting exponent is %ld (from exponent=%Lg, best_factor=%Lg)\n",
                result.resulting_exponent,
                (long double) exponent,
                (long double) best_factor);
        #endif
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
        static cost_t CalculatePowiFactorCost(int_exponent_t int_exponent)
        {
            static std::map<int_exponent_t, cost_t> cache;
            if(int_exponent < 0)
            {
                cost_t cost = 22; // division cost
                return cost + CalculatePowiFactorCost(-int_exponent);
            }
            std::map<int_exponent_t,cost_t>::iterator i = cache.lower_bound(int_exponent);
            if(i != cache.end() && i->first == int_exponent)
                return i->second;
            std::pair<int_exponent_t, cost_t> result(int_exponent, 0.0);
            cost_t& cost = result.second;

            while(int_exponent > 1)
            {
                int factor = 0;
                if(int_exponent < 256)
                {
                    factor = FPoptimizer_ByteCode::powi_table[int_exponent];
                    if(factor & 128) factor &= 127; else factor = 0;
                    if(factor & 64) factor = -(factor&63) - 1;
                }
                if(factor)
                {
                    cost += CalculatePowiFactorCost(factor);
                    int_exponent /= factor;
                    continue;
                }
                if(!(int_exponent & 1))
                {
                    int_exponent /= 2;
                    cost += 6; // sqr
                }
                else
                {
                    cost += 7; // dup+mul
                    int_exponent -= 1;
                }
            }

            cache.insert(i, result);
            return cost;
        }

        template<typename Value_t>
        static int_exponent_t MultiplyAndMakeLong(const Value_t& value, factor_t factor)
        {
            return makeLongInteger( value * Value_t(factor) );
        }

        // Find the integer that "value" must be multiplied
        // with to produce an integer...
        // Consisting of factors 2 and 3 only.
        template<typename Value_t>
        static bool MakesInteger(const Value_t& value, factor_t factor)
        {
            /* Does value, multiplied by factor, result in an integer? */
            Value_t v = value * Value_t(factor);
            return isLongInteger(v);
            /*
            Value_t diff = fp_abs(v - fp_int(v));
            //printf("factor %d: v=%.20f, diff=%.20f\n", factor,v, diff);
            return diff < Value_t(1e-9l);
            */
        }

        template<typename Value_t>
        static factor_t FindIntegerFactor(const Value_t& value)
        {
            factor_t factor = (2*2*2*2);
#ifdef CBRT_IS_SLOW
            // When cbrt is implemented through exp and log,
            // there is no advantage over exp(log()), so don't support it.
#else
            factor *= (3*3*3);
#endif
            factor_t result = 0;
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

        static int EvaluateFactorCost(int factor, int s, int c, int nmuls)
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
    template<typename Value_t>
    bool CodeTree<Value_t>::RecreateInversionsAndNegations(bool prefer_base2)
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
                std::vector<CodeTree<Value_t> > div_params;
                CodeTree<Value_t> found_log2, found_log2by;

                if(true)
                {
                    /* This lengthy bit of code
                     * changes log2(x)^3 * 5
                     * to      log2by(x, 5^(1/3)) ^ 3
                     * which is better for runtime
                     * than    log2by(x,1)^3 * 5
                     */
                    bool found_log2_on_exponent = false;
                    Value_t log2_exponent = 0;
                    for(size_t a = GetParamCount(); a-- > 0; )
                    {
                        const CodeTree<Value_t>& powgroup = GetParam(a);
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
                        Value_t immeds = 1.0;
                        for(size_t a = GetParamCount(); a-- > 0; )
                        {
                            const CodeTree<Value_t>& powgroup = GetParam(a);
                            if(powgroup.IsImmed())
                            {
                                immeds *= powgroup.GetImmed();
                                DelParam(a);
                            }
                        }
                        for(size_t a = GetParamCount(); a-- > 0; )
                        {
                            CodeTree<Value_t>& powgroup = GetParam(a);
                            if(powgroup.GetOpcode() == cPow
                            && powgroup.GetParam(0).GetOpcode() == cLog2
                            && powgroup.GetParam(1).IsImmed())
                            {
                                CodeTree<Value_t>& log2 = powgroup.GetParam(0);
                                log2.CopyOnWrite();
                                log2.SetOpcode(cLog2by);
                                log2.AddParam( CodeTreeImmed(
                                    fp_pow(immeds, Value_t(1) / log2_exponent) ) );
                                log2.Rehash();
                                break;
                            }
                        }
                    }
                }

                for(size_t a = GetParamCount(); a-- > 0; )
                {
                    const CodeTree<Value_t>& powgroup = GetParam(a);

                    if(powgroup.GetOpcode() == cPow
                    && powgroup.GetParam(1).IsImmed())
                    {
                        const CodeTree<Value_t>& exp_param = powgroup.GetParam(1);
                        Value_t exponent = exp_param.GetImmed();
                        if(fp_equal(exponent, Value_t(-1)))
                        {
                            CopyOnWrite();
                            div_params.push_back(GetParam(a).GetParam(0));
                            DelParam(a); // delete the pow group
                        }
                        else if(exponent < 0 && isInteger(exponent))
                        {
                            CodeTree<Value_t> edited_powgroup;
                            edited_powgroup.SetOpcode(cPow);
                            edited_powgroup.AddParam(powgroup.GetParam(0));
                            edited_powgroup.AddParam(CodeTreeImmed( -exponent ));
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

                    CodeTree<Value_t> divgroup;
                    divgroup.SetOpcode(cMul);
                    divgroup.SetParamsMove(div_params);
                    divgroup.Rehash(); // will reduce to div_params[0] if only one item
                    CodeTree<Value_t> mulgroup;
                    mulgroup.SetOpcode(cMul);
                    mulgroup.SetParamsMove(GetParams());
                    mulgroup.Rehash(); // will reduce to 1.0 if none remained in this cMul
                    if(mulgroup.IsImmed() && fp_equal(mulgroup.GetImmed(), Value_t(1)))
                    {
                        SetOpcode(cInv);
                        AddParamMove(divgroup);
                    }
                    /*else if(mulgroup.IsImmed() && fp_equal(mulgroup.GetImmed(), Value_t(-1)))
                    {
                        CodeTree<Value_t> invgroup;
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
                    CodeTree<Value_t> mulgroup;
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
                    CodeTree<Value_t> mulgroup;
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
                std::vector<CodeTree<Value_t> > sub_params;

                for(size_t a = GetParamCount(); a-- > 0; )
                    if(GetParam(a).GetOpcode() == cMul)
                    {
                        bool is_signed = false; // if the mul group has a -1 constant...

                    Recheck_RefCount_Mul:;
                        CodeTree<Value_t>& mulgroup = GetParam(a);
                        bool needs_cow = GetRefCount() > 1;

                        for(size_t b=mulgroup.GetParamCount(); b-- > 0; )
                        {
                            if(mulgroup.GetParam(b).IsImmed())
                            {
                                Value_t factor = mulgroup.GetParam(b).GetImmed();
                                if(fp_equal(factor, Value_t(-1)))
                                {
                                    if(needs_cow) { CopyOnWrite(); goto Recheck_RefCount_Mul; }
                                    mulgroup.CopyOnWrite();
                                    mulgroup.DelParam(b);
                                    is_signed = !is_signed;
                                }
                                else if(fp_equal(factor, Value_t(-2)))
                                {
                                    if(needs_cow) { CopyOnWrite(); goto Recheck_RefCount_Mul; }
                                    mulgroup.CopyOnWrite();
                                    mulgroup.DelParam(b);
                                    mulgroup.AddParam( CodeTreeImmed( Value_t(2) ) );
                                    is_signed = !is_signed;
                                }
                            }
                        }
                        if(is_signed)
                        {
                            mulgroup.Rehash();
                            sub_params.push_back(mulgroup);
                            DelParam(a);
                        }
                    }
                    else if(GetParam(a).GetOpcode() == cDiv)
                    {
                        bool is_signed = false;

                    Recheck_RefCount_Div:;
                        CodeTree<Value_t>& divgroup = GetParam(a);
                        bool needs_cow = GetRefCount() > 1;

                        if(divgroup.GetParam(0).IsImmed())
                        {
                            if(fp_equal(divgroup.GetParam(0).GetImmed(), Value_t(-1)))
                            {
                                if(needs_cow) { CopyOnWrite(); goto Recheck_RefCount_Div; }
                                divgroup.CopyOnWrite();
                                divgroup.DelParam(0);
                                divgroup.SetOpcode(cInv);
                                is_signed = !is_signed;
                            }
                        }
                        if(is_signed)
                        {
                            if(needs_cow) { CopyOnWrite(); goto Recheck_RefCount_Div; }
                            divgroup.Rehash();
                            sub_params.push_back(divgroup);
                            DelParam(a);
                        }
                    }
                    else if(GetParam(a).GetOpcode() == cRDiv)
                    {
                        bool is_signed = false;

                    Recheck_RefCount_RDiv:;
                        CodeTree<Value_t>& divgroup = GetParam(a);
                        bool needs_cow = GetRefCount() > 1;

                        if(divgroup.GetParam(1).IsImmed())
                        {
                            if(fp_equal(divgroup.GetParam(1).GetImmed(), Value_t(-1)))
                            {
                                if(needs_cow) { CopyOnWrite(); goto Recheck_RefCount_RDiv; }
                                divgroup.CopyOnWrite();
                                divgroup.DelParam(1);
                                divgroup.SetOpcode(cInv);
                                is_signed = !is_signed;
                            }
                        }
                        if(is_signed)
                        {
                            if(needs_cow) { CopyOnWrite(); goto Recheck_RefCount_RDiv; }
                            divgroup.Rehash();
                            sub_params.push_back(divgroup);
                            DelParam(a);
                        }
                    }
                if(!sub_params.empty())
                {
                  #ifdef DEBUG_SUBSTITUTIONS
                    printf("Will make a Sub conversion in:\n"); fflush(stdout);
                    DumpTreeWithIndent(*this);
                  #endif
                    CodeTree<Value_t> subgroup;
                    subgroup.SetOpcode(cAdd);
                    subgroup.SetParamsMove(sub_params);
                    subgroup.Rehash(); // will reduce to sub_params[0] if only one item
                    CodeTree<Value_t> addgroup;
                    addgroup.SetOpcode(cAdd);
                    addgroup.SetParamsMove(GetParams());
                    addgroup.Rehash(); // will reduce to 0.0 if none remained in this cAdd
                    if(addgroup.IsImmed() && fp_equal(addgroup.GetImmed(), Value_t(0)))
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
                                CodeTree<Value_t> innersub;
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
                  #ifdef DEBUG_SUBSTITUTIONS
                    printf("After Sub conversion:\n"); fflush(stdout);
                    DumpTreeWithIndent(*this);
                  #endif
                }
                break;
            }
            case cPow:
            {
                const CodeTree<Value_t>& p0 = GetParam(0);
                const CodeTree<Value_t>& p1 = GetParam(1);
                if(p1.IsImmed())
                {
                    if(p1.GetImmed() != Value_t(0)
                    && !isInteger(p1.GetImmed()))
                    {
                        PowiResolver::PowiResult
                            r = PowiResolver::CreatePowiResult(fp_abs(p1.GetImmed()));

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
                            printf("Will resolve powi %Lg as powi(chain(%d,%d),%ld)",
                                (long double) fp_abs(p1.GetImmed()),
                                r.n_int_sqrt,
                                r.n_int_cbrt,
                                r.resulting_exponent);
                            for(unsigned n=0; n<PowiResolver::MaxSep; ++n)
                            {
                                if(r.sep_list[n] == 0) break;
                                int n_sqrt = r.sep_list[n] % PowiResolver::MaxOp;
                                int n_cbrt = r.sep_list[n] / PowiResolver::MaxOp;
                                printf("*chain(%d,%d)", n_sqrt,n_cbrt);
                            }
                            printf("\n");
                        #endif

                            CodeTree<Value_t> source_tree = GetParam(0);

                            CodeTree<Value_t> pow_item = source_tree;
                            pow_item.CopyOnWrite();
                            ChangeIntoRootChain(pow_item,
                                signed_chain,
                                r.n_int_sqrt,
                                r.n_int_cbrt);
                            pow_item.Rehash();

                            CodeTree<Value_t> pow;
                            if(r.resulting_exponent != 1)
                            {
                                pow.SetOpcode(cPow);
                                pow.AddParamMove(pow_item);
                                pow.AddParam(CodeTreeImmed( Value_t(r.resulting_exponent) ));
                            }
                            else
                                pow.swap(pow_item);

                            CodeTree<Value_t> mul;
                            mul.SetOpcode(cMul);
                            mul.AddParamMove(pow);

                            for(unsigned n=0; n<PowiResolver::MaxSep; ++n)
                            {
                                if(r.sep_list[n] == 0) break;
                                int n_sqrt = r.sep_list[n] % PowiResolver::MaxOp;
                                int n_cbrt = r.sep_list[n] / PowiResolver::MaxOp;

                                CodeTree<Value_t> mul_item = source_tree;
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
                && (!p1.IsImmed()
                 || !isLongInteger(p1.GetImmed())
                 || !IsOptimizableUsingPowi<Value_t>( makeLongInteger(p1.GetImmed()) )))
                {
                    if(p0.IsImmed() && p0.GetImmed() > 0.0)
                    {
                        // Convert into cExp or Exp2.
                        //    x^y = exp(log(x) * y) =
                        //    Can only be done when x is positive, though.
                        if(prefer_base2)
                        {
                            Value_t mulvalue = fp_log2( p0.GetImmed() );
                            if(fp_equal(mulvalue, Value_t(1)))
                            {
                                // exp2(1)^x becomes exp2(x)
                                DelParam(0);
                            }
                            else
                            {
                                // exp2(4)^x becomes exp2(4*x)
                                CodeTree<Value_t> exponent;
                                exponent.SetOpcode(cMul);
                                exponent.AddParam( CodeTreeImmed( mulvalue ) );
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
                            Value_t mulvalue = fp_log( p0.GetImmed() );
                            if(fp_equal(mulvalue, Value_t(1)))
                            {
                                // exp(1)^x becomes exp(x)
                                DelParam(0);
                            }
                            else
                            {
                                // exp(4)^x becomes exp(4*x)
                                CodeTree<Value_t> exponent;
                                exponent.SetOpcode(cMul);
                                exponent.AddParam( CodeTreeImmed( mulvalue ) );
                                exponent.AddParam(p1);
                                exponent.Rehash();
                                SetParamMove(0, exponent);
                                DelParam(1);
                            }
                            SetOpcode(cExp);
                            changed = true;
                        }
                    }
                    else if(GetPositivityInfo(p0) == IsAlways)
                    {
                        if(prefer_base2)
                        {
                            CodeTree<Value_t> log;
                            log.SetOpcode(cLog2);
                            log.AddParam(p0);
                            log.Rehash();
                            CodeTree<Value_t> exponent;
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
                            CodeTree<Value_t> log;
                            log.SetOpcode(cLog);
                            log.AddParam(p0);
                            log.Rehash();
                            CodeTree<Value_t> exponent;
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
}

/* BEGIN_EXPLICIT_INSTANTATION */
namespace FPoptimizer_CodeTree
{
    template
    bool CodeTree<double>::RecreateInversionsAndNegations(bool prefer_base2);
#ifdef FP_SUPPORT_FLOAT_TYPE
    template
    bool CodeTree<float>::RecreateInversionsAndNegations(bool prefer_base2);
#endif
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
    template
    bool CodeTree<long double>::RecreateInversionsAndNegations(bool prefer_base2);
#endif
}
/* END_EXPLICIT_INSTANTATION */

#endif
