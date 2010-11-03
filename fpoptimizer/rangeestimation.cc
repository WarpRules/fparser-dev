#include "rangeestimation.hh"
#include "consts.hh"

#ifdef FP_SUPPORT_OPTIMIZER

using namespace FUNCTIONPARSERTYPES;
using namespace FPoptimizer_CodeTree;

//#define DEBUG_SUBSTITUTIONS_extra_verbose

namespace
{
    template<unsigned Compare> struct Comp { };
    template<>struct Comp<cLess> {
        template<typename Value_t>
        bool operator() (const Value_t& a, const Value_t& b) { return a<b; }
    };
    template<>struct Comp<cLessOrEq> {
        template<typename Value_t>
        bool operator() (const Value_t& a, const Value_t& b) { return a<=b; }
    };
    template<>struct Comp<cGreater> {
        template<typename Value_t>
        bool operator() (const Value_t& a, const Value_t& b) { return a>b; }
    };
    template<>struct Comp<cGreaterOrEq> {
        template<typename Value_t>
        bool operator() (const Value_t& a, const Value_t& b) { return a>=b; }
    };
    template<>struct Comp<cEqual> {
        template<typename Value_t>
        bool operator() (const Value_t& a, const Value_t& b) { return a==b; }
    };
    template<>struct Comp<cNEqual> {
        template<typename Value_t>
        bool operator() (const Value_t& a, const Value_t& b) { return a!=b; }
    };
}

namespace FPoptimizer_CodeTree
{
    template<typename Value_t>
    void range<Value_t>::set_abs()
    {
        if(!has_min && !has_max)                  // -inf..+inf -> +0..+inf
            { has_min = true; min = Value_t(0); }
        else if(!has_min && max < Value_t(0))     // -inf..-5   -> +5..+inf
            { has_min = true; min = -max; has_max = false; }
        else if(!has_min)                         // -inf..+5   -> +0..+inf
            { has_min = true; min = Value_t(0); has_max = false; }
        else if(min >= Value_t(0))                // +0..xx     -> +0..xx
            return;                               // +5..xx     -> +5..xx
        else if(!has_max)                         // -5..+inf   -> +0..+inf
            { has_min = true; min = Value_t(0); }
        else if(max < Value_t(0))                 // -5..-3     -> +3..+5
            { Value_t tmp(-max); max=-min; min=tmp; }
        else if(-min >= max)                      // -5..+3     -> 0..+5
            { max = -min; min = Value_t(0); }
        else                                      // -5..+7     -> +5..+7
            { min = Value_t(0); }
    }

    template<typename Value_t>
    void range<Value_t>::set_neg()
    {
        std::swap(has_min, has_max);
        std::swap(min, max);
        min = -min; max = -max;
    }

    //////

    template<typename Value_t> template<unsigned Compare>
    void range<Value_t>::set_min_if
        (const Value_t& v,
         Value_t (*const func)(Value_t),
         range<Value_t> model)
    {
        if(has_min && Comp<Compare>() (min,v))
            min = func(min);
        else
            { has_min = model.has_min; min = model.min; }
    }

    template<typename Value_t> template<unsigned Compare>
    void range<Value_t>::set_max_if
        (const Value_t& v,
         Value_t (*const func)(Value_t),
         range<Value_t> model)
    {
        if(has_max && Comp<Compare>() (max,v))
            max = func(max);
        else
            { has_max = model.has_max; max = model.max; }
    }

    template<typename Value_t> template<unsigned Compare>
    void range<Value_t>::set_min_max_if
        (const Value_t& v,
         Value_t (*const func)(Value_t),
         range<Value_t> model)
    {
        set_min_if<Compare> (v,func, model);
        set_max_if<Compare> (v,func, model);
    }

    template<typename Value_t>
    void range<Value_t>::set_min
       (Value_t (*const func)(Value_t),
         range<Value_t> model)
    {
        if(has_min)
            min = func(min);
        else
            { has_min = model.has_min; min = model.min; }
    }

    template<typename Value_t>
    void range<Value_t>::set_max
       (Value_t (*const func)(Value_t),
         range<Value_t> model)
    {
        if(has_max)
            max = func(max);
        else
            { has_max = model.has_max; max = model.max; }
    }

    template<typename Value_t>
    void range<Value_t>::set_min_max
        (Value_t (*const func)(Value_t),
         range<Value_t> model)
    {
        set_min(func, model);
        set_max(func, model);
    }

    //////

    template<typename Value_t> template<unsigned Compare>
    void range<Value_t>::set_min_if
        (const Value_t& v,
         Value_t (*const func)(const Value_t&),
         range<Value_t> model)
    {
        if(has_min && Comp<Compare>() (min,v))
            min = func(min);
        else
            { has_min = model.has_min; min = model.min; }
    }

    template<typename Value_t> template<unsigned Compare>
    void range<Value_t>::set_max_if
        (const Value_t& v,
         Value_t (*const func)(const Value_t&),
         range<Value_t> model)
    {
        if(has_max && Comp<Compare>() (max,v))
            max = func(max);
        else
            { has_max = model.has_max; max = model.max; }
    }

    template<typename Value_t> template<unsigned Compare>
    void range<Value_t>::set_min_max_if
        (const Value_t& v,
         Value_t (*const func)(const Value_t&),
         range<Value_t> model)
    {
        set_min_if<Compare> (v,func, model);
        set_max_if<Compare> (v,func, model);
    }

    template<typename Value_t>
    void range<Value_t>::set_min
       (Value_t (*const func)(const Value_t&),
         range<Value_t> model)
    {
        if(has_min)
            min = func(min);
        else
            { has_min = model.has_min; min = model.min; }
    }

    template<typename Value_t>
    void range<Value_t>::set_max
       (Value_t (*const func)(const Value_t&),
         range<Value_t> model)
    {
        if(has_max)
            max = func(max);
        else
            { has_max = model.has_max; max = model.max; }
    }

    template<typename Value_t>
    void range<Value_t>::set_min_max
        (Value_t (*const func)(const Value_t&),
         range<Value_t> model)
    {
        set_min(func, model);
        set_max(func, model);
    }

    //////////////

    template<typename Value_t>
    range<Value_t> CalculateResultBoundaries(const CodeTree<Value_t>& tree)
#ifdef DEBUG_SUBSTITUTIONS_extra_verbose
    {
        range<Value_t> tmp = CalculateResultBoundaries_do(tree);
        std::cout << "Estimated boundaries: ";
        if(tmp.has_min) std::cout << tmp.min; else std::cout << "-inf";
        std::cout << " .. ";
        if(tmp.has_max) std::cout << tmp.max; else std::cout << "+inf";
        std::cout << ": ";
        DumpTree(tree);
        std::cout << std::endl;
        return tmp;
    }
    template<typename Value_t>
    range<Value_t> CodeTree<Value_t>::CalculateResultBoundaries_do(const CodeTree<Value_t>& tree)
#endif
    {
        static const range<Value_t> pihalf_limits
            (-fp_const_pihalf<Value_t>(),
              fp_const_pihalf<Value_t>());

        static const range<Value_t> pi_limits
            (-fp_const_pi<Value_t>(),
              fp_const_pi<Value_t>());

        static const range<Value_t> abs_pi_limits
            ( Value_t(0),
              fp_const_pi<Value_t>());

        using namespace std;
        switch( tree.GetOpcode() )
        {
            case cImmed:
                return range<Value_t>(tree.GetImmed(), tree.GetImmed()); // a definite value.
            case cAnd:
            case cAbsAnd:
            case cOr:
            case cAbsOr:
            case cNot:
            case cAbsNot:
            case cNotNot:
            case cAbsNotNot:
            case cEqual:
            case cNEqual:
            case cLess:
            case cLessOrEq:
            case cGreater:
            case cGreaterOrEq:
            {
                /* These operations always produce truth values (0 or 1) */
                /* Narrowing them down is a matter of performing Constant optimization */
                return range<Value_t>( Value_t(0), Value_t(1) );
            }
            case cAbs:
            {
                /* cAbs always produces a positive value */
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                m.set_abs();
                return m;
            }

            case cLog: /* Defined for 0.0 < x <= inf */
            {
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                m.template set_min_max_if<cGreater>(Value_t(0), fp_log); // No boundaries
                return m;
            }

            case cLog2: /* Defined for 0.0 < x <= inf */
            {
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                m.template set_min_max_if<cGreater>(Value_t(0), fp_log2); // No boundaries
                return m;
            }

            case cLog10: /* Defined for 0.0 < x <= inf */
            {
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                m.template set_min_max_if<cGreater>(Value_t(0), fp_log10); // No boundaries
                return m;
            }

            case cAcosh: /* defined for             1.0 <= x <= inf */
            {
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                m.template set_min_max_if<cGreaterOrEq>(Value_t(1), fp_acosh); // No boundaries
                return m;
            }
            case cAsinh: /* defined for all values -inf <= x <= inf */
            {
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                m.set_min_max(fp_asinh); // No boundaries
                return m;
            }
            case cAtanh: /* defined for -1.0 <= x < 1, results within -inf..+inf */
            {
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                m.template set_min_if<cGreater> (Value_t(-1), fp_atanh);
                m.template set_max_if<cLess>    (Value_t( 1), fp_atanh);
                return m;
            }
            case cAcos: /* defined for -1.0 <= x <= 1, results within CONSTANT_PI..0 */
            {
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                return range<Value_t>( // Note that the range is flipped!
                    (m.has_max && (m.max) < Value_t(1))
                        ? fp_acos(m.max) : Value_t(0),
                    (m.has_min && (m.min) >= Value_t(-1))
                        ? fp_acos(m.min) : fp_const_pi<Value_t>()
                                          );
            }
            case cAsin: /* defined for -1.0 <= x < 1, results within -CONSTANT_PIHALF..CONSTANT_PIHALF */
            {
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                /* Assuming that x is never outside valid limits */
                m.template set_min_if<cGreater>(Value_t(-1), fp_asin, pihalf_limits);
                m.template set_max_if<cLess   >(Value_t( 1), fp_asin, pihalf_limits);
                return m;
            }
            case cAtan: /* defined for all values -inf <= x <= inf, results within -CONSTANT_PIHALF..CONSTANT_PIHALF */
            {
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                m.set_min_max(fp_atan, pihalf_limits);
                return m;
            }
            case cAtan2: /* too complicated to estimate */
            {
                range<Value_t> p0 = CalculateResultBoundaries( tree.GetParam(0) );
                range<Value_t> p1 = CalculateResultBoundaries( tree.GetParam(1) );
                if(tree.GetParam(0).IsImmed()
                && fp_equal(tree.GetParam(0).GetImmed(), Value_t(0)))   // y == 0
                {
                    // Either 0.0 or CONSTANT_PI
                    return abs_pi_limits;
                }
                if(tree.GetParam(1).IsImmed()
                && fp_equal(tree.GetParam(1).GetImmed(), Value_t(0)))   // x == 0
                {
                    // Either -CONSTANT_PIHALF or +CONSTANT_PIHALF
                    return pihalf_limits;
                }
                // Anything else
                /* Somewhat complicated to narrow down from this */
                /* TODO: A resourceful programmer may add it later. */
                return pi_limits;
            }

            case cSin:
            {
                /* Quite difficult to estimate due to the cyclic nature of the function. */
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                bool covers_full_cycle
                    = !m.has_min || !m.has_max
                    || (m.max - m.min) >= (fp_const_twopi<Value_t>());
                if(covers_full_cycle)
                    return range<Value_t>(Value_t(-1), Value_t(1));
                Value_t min = fp_mod(m.min, fp_const_twopi<Value_t>()); if(min<Value_t(0)) min+=fp_const_twopi<Value_t>();
                Value_t max = fp_mod(m.max, fp_const_twopi<Value_t>()); if(max<Value_t(0)) max+=fp_const_twopi<Value_t>();
                if(max < min) max += fp_const_twopi<Value_t>();
                bool covers_plus1  = (min <= fp_const_pihalf<Value_t>() && max >= fp_const_pihalf<Value_t>());
                bool covers_minus1 = (min <= Value_t(1.5)*fp_const_pi<Value_t>() && max >= Value_t(1.5)*fp_const_pi<Value_t>());
                if(covers_plus1 && covers_minus1)
                    return range<Value_t>(Value_t(-1), Value_t(1));
                if(covers_minus1)
                    return range<Value_t>(Value_t(-1), fp_max(fp_sin(min), fp_sin(max)));
                if(covers_plus1)
                    return range<Value_t>(fp_min(fp_sin(min), fp_sin(max)), Value_t(1));
                return range<Value_t>(fp_min(fp_sin(min), fp_sin(max)),
                                           fp_max(fp_sin(min), fp_sin(max)));
            }
            case cCos:
            {
                /* Quite difficult to estimate due to the cyclic nature of the function. */
                /* cos(x) = sin(pi/2 - x) = sin(x + pi/2) */
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                if(m.has_min) m.min += fp_const_pihalf<Value_t>();/*for cCos*/
                if(m.has_max) m.max += fp_const_pihalf<Value_t>();/*for cCos*/
                bool covers_full_cycle
                    = !m.has_min || !m.has_max
                    || (m.max - m.min) >= (fp_const_twopi<Value_t>());
                if(covers_full_cycle)
                    return range<Value_t>(Value_t(-1), Value_t(1));
                Value_t min = fp_mod(m.min, fp_const_twopi<Value_t>()); if(min<Value_t(0)) min+=fp_const_twopi<Value_t>();
                Value_t max = fp_mod(m.max, fp_const_twopi<Value_t>()); if(max<Value_t(0)) max+=fp_const_twopi<Value_t>();
                if(max < min) max += fp_const_twopi<Value_t>();
                bool covers_plus1  = (min <= fp_const_pihalf<Value_t>() && max >= fp_const_pihalf<Value_t>());
                bool covers_minus1 = (min <= Value_t(1.5)*fp_const_pi<Value_t>() && max >= Value_t(1.5)*fp_const_pi<Value_t>());
                if(covers_plus1 && covers_minus1)
                    return range<Value_t>(Value_t(-1), Value_t(1));
                if(covers_minus1)
                    return range<Value_t>(Value_t(-1), fp_max(fp_sin(min), fp_sin(max)));
                if(covers_plus1)
                    return range<Value_t>(fp_min(fp_sin(min), fp_sin(max)), Value_t(1));
                return range<Value_t>(fp_min(fp_sin(min), fp_sin(max)),
                                           fp_max(fp_sin(min), fp_sin(max)));
            }
            case cTan:
            {
                /* Could be narrowed down from here,
                 * but it's too complicated due to
                 * the cyclic nature of the function */
                /* TODO: A resourceful programmer may add it later. */
                return range<Value_t>(); // (CONSTANT_NEG_INF, CONSTANT_POS_INF);
            }

            case cCeil:
            {
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                m.set_max(fp_ceil); // ceil() may increase the value, may not decrease
                return m;
            }
            case cFloor:
            {
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                m.set_min(fp_floor); // floor() may decrease the value, may not increase
                return m;
            }
            case cTrunc:
            {
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                m.set_min(fp_floor); // trunc() may either increase or decrease the value
                m.set_max(fp_ceil); // for safety, we assume both
                return m;
            }
            case cInt:
            {
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                m.set_min(fp_floor); // int() may either increase or decrease the value
                m.set_max(fp_ceil); // for safety, we assume both
                return m;
            }
            case cSinh: /* defined for all values -inf <= x <= inf */
            {
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                m.set_min_max(fp_sinh); // No boundaries
                return m;
            }
            case cTanh: /* defined for all values -inf <= x <= inf, results within -1..1 */
            {
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                m.set_min_max(fp_tanh, range<Value_t> (Value_t(-1), Value_t(1)));
                return m;
            }
            case cCosh: /* defined for all values -inf <= x <= inf, results within 1..inf */
            {
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                if(m.has_min)
                {
                    if(m.has_max) // max, min
                    {
                        if(m.min >= Value_t(0) && m.max >= Value_t(0)) // +x .. +y
                            { m.min = fp_cosh(m.min); m.max = fp_cosh(m.max); }
                        else if((m.min) < Value_t(0) && m.max >= Value_t(0)) // -x .. +y
                            { Value_t tmp = fp_cosh(m.min); m.max = fp_cosh(m.max);
                              if(tmp > m.max) m.max = tmp;
                              m.min = Value_t(1); }
                        else // -x .. -y
                            { m.min = fp_cosh(m.min); m.max = fp_cosh(m.max);
                              std::swap(m.min, m.max); }
                    }
                    else // min, no max
                    {
                        if(m.min >= Value_t(0)) // 0..inf -> 1..inf
                            { m.has_max = false; m.min = fp_cosh(m.min); }
                        else
                            { m.has_max = false; m.min = Value_t(1); } // Anything between 1..inf
                    }
                }
                else // no min
                {
                    m.has_min = true; m.min = Value_t(1); // always a lower boundary
                    if(m.has_max) // max, no min
                    {
                        m.min = fp_cosh(m.max); // n..inf
                        m.has_max = false; // No upper boundary
                    }
                    else // no max, no min
                        m.has_max = false; // No upper boundary
                }
                return m;
            }

            case cIf:
            case cAbsIf:
            {
                // No guess which branch is chosen. Produce a spanning min & max.
                range<Value_t> res1 = CalculateResultBoundaries( tree.GetParam(1) );
                range<Value_t> res2 = CalculateResultBoundaries( tree.GetParam(2) );
                if(!res2.has_min) res1.has_min = false; else if(res1.has_min && (res2.min) < res1.min) res1.min = res2.min;
                if(!res2.has_max) res1.has_max = false; else if(res1.has_max && (res2.max) > res1.max) res1.max = res2.max;
                return res1;
            }

            case cMin:
            {
                bool has_unknown_min = false;
                bool has_unknown_max = false;

                range<Value_t> result;
                for(size_t a=0; a<tree.GetParamCount(); ++a)
                {
                    range<Value_t> m = CalculateResultBoundaries( tree.GetParam(a) );
                    if(!m.has_min)
                        has_unknown_min = true;
                    else if(!result.has_min || (m.min) < result.min)
                        result.min = m.min;

                    if(!m.has_max)
                        has_unknown_max = true;
                    else if(!result.has_max || (m.max) < result.max)
                        result.max = m.max;
                }
                if(has_unknown_min) result.has_min = false;
                if(has_unknown_max) result.has_max = false;
                return result;
            }
            case cMax:
            {
                bool has_unknown_min = false;
                bool has_unknown_max = false;

                range<Value_t> result;
                for(size_t a=0; a<tree.GetParamCount(); ++a)
                {
                    range<Value_t> m = CalculateResultBoundaries( tree.GetParam(a) );
                    if(!m.has_min)
                        has_unknown_min = true;
                    else if(!result.has_min || m.min > result.min)
                        result.min = m.min;

                    if(!m.has_max)
                        has_unknown_max = true;
                    else if(!result.has_max || m.max > result.max)
                        result.max = m.max;
                }
                if(has_unknown_min) result.has_min = false;
                if(has_unknown_max) result.has_max = false;
                return result;
            }
            case cAdd:
            {
                /* It's complicated. Follow the logic below. */
                /* Note: This also deals with the following opcodes:
                 *       cNeg, cSub, cRSub
                 */
                range<Value_t> result(Value_t(0), Value_t(0));
                for(size_t a=0; a<tree.GetParamCount(); ++a)
                {
                    range<Value_t> item = CalculateResultBoundaries( tree.GetParam(a) );

                    if(item.has_min) result.min += item.min;
                    else             result.has_min = false;
                    if(item.has_max) result.max += item.max;
                    else             result.has_max = false;

                    if(!result.has_min && !result.has_max) break; // hopeless
                }
                if(result.has_min && result.has_max
                && result.min > result.max) std::swap(result.min, result.max);
                return result;
            }
            case cMul:
            {
                /* It's very complicated. Follow the logic below. */
                struct Value
                {
                    enum ValueType { Finite, MinusInf, PlusInf };
                    ValueType valueType;
                    Value_t value;

                    Value(ValueType t): valueType(t), value(0) {}
                    Value(Value_t v): valueType(Finite), value(v) {}

                    bool isNegative() const
                    {
                        return valueType == MinusInf ||
                            (valueType == Finite && value < Value_t(0));
                    }

                    void operator*=(const Value& rhs)
                    {
                        if(valueType == Finite && rhs.valueType == Finite)
                            value *= rhs.value;
                        else
                            valueType = (isNegative() != rhs.isNegative() ?
                                         MinusInf : PlusInf);
                    }

                    bool operator<(const Value& rhs) const
                    {
                        return
                            (valueType == MinusInf && rhs.valueType != MinusInf) ||
                            (valueType == Finite &&
                             (rhs.valueType == PlusInf ||
                              (rhs.valueType == Finite && value < rhs.value)));
                    }
                };

                struct MultiplicationRange
                {
                    Value minValue, maxValue;

                    MultiplicationRange():
                        minValue(Value::PlusInf),
                        maxValue(Value::MinusInf) {}

                    void multiply(Value value1, const Value& value2)
                    {
                        value1 *= value2;
                        if(value1 < minValue) minValue = value1;
                        if(maxValue < value1) maxValue = value1;
                    }
                };

                range<Value_t> result(Value_t(1), Value_t(1));
                for(size_t a=0; a<tree.GetParamCount(); ++a)
                {
                    range<Value_t> item = CalculateResultBoundaries( tree.GetParam(a) );
                    if(!item.has_min && !item.has_max) return range<Value_t>(); // hopeless

                    Value minValue0 = result.has_min ? Value(result.min) : Value(Value::MinusInf);
                    Value maxValue0 = result.has_max ? Value(result.max) : Value(Value::PlusInf);
                    Value minValue1 = item.has_min ? Value(item.min) : Value(Value::MinusInf);
                    Value maxValue1 = item.has_max ? Value(item.max) : Value(Value::PlusInf);

                    MultiplicationRange range;
                    range.multiply(minValue0, minValue1);
                    range.multiply(minValue0, maxValue1);
                    range.multiply(maxValue0, minValue1);
                    range.multiply(maxValue0, maxValue1);

                    if(range.minValue.valueType == Value::Finite)
                        result.min = range.minValue.value;
                    else result.has_min = false;

                    if(range.maxValue.valueType == Value::Finite)
                        result.max = range.maxValue.value;
                    else result.has_max = false;

                    if(!result.has_min && !result.has_max) break; // hopeless
                }
                if(result.has_min && result.has_max
                && result.min > result.max) std::swap(result.min, result.max);
                return result;
            }
            case cMod:
            {
                /* TODO: The boundaries of modulo operator could be estimated better. */

                range<Value_t> x = CalculateResultBoundaries( tree.GetParam(0) );
                range<Value_t> y = CalculateResultBoundaries( tree.GetParam(1) );

                if(y.has_max)
                {
                    if(y.max >= Value_t(0))
                    {
                        if(!x.has_min || (x.min) < Value_t(0))
                            return range<Value_t>(-y.max, y.max);
                        else
                            return range<Value_t>(Value_t(0), y.max);
                    }
                    else
                    {
                        if(!x.has_max || (x.max) >= Value_t(0))
                            return range<Value_t>(y.max, -y.max);
                        else
                            return range<Value_t>(y.max, fp_const_negativezero<Value_t>());
                    }
                }
                else
                    return range<Value_t>();
            }
            case cPow:
            {
                if(tree.GetParam(1).IsImmed() && tree.GetParam(1).GetImmed() == Value_t(0))
                {
                    // Note: This makes 0^0 evaluate into 1.
                    return range<Value_t>(Value_t(1), Value_t(1)); // x^0 = 1
                }
                if(tree.GetParam(0).IsImmed() && tree.GetParam(0).GetImmed() == Value_t(0))
                {
                    // Note: This makes 0^0 evaluate into 0.
                    return range<Value_t>(Value_t(0), Value_t(0)); // 0^x = 0
                }
                if(tree.GetParam(0).IsImmed() && fp_equal(tree.GetParam(0).GetImmed(), Value_t(1)))
                {
                    return range<Value_t>(Value_t(1), Value_t(1)); // 1^x = 1
                }
                if(tree.GetParam(1).IsImmed()
                && tree.GetParam(1).GetImmed() > Value_t(0)
                && GetEvennessInfo(tree.GetParam(1)) == IsAlways)
                {
                    // x ^ even_int_const always produces a non-negative value.
                    Value_t exponent = tree.GetParam(1).GetImmed();
                    range<Value_t> tmp = CalculateResultBoundaries( tree.GetParam(0) );
                    range<Value_t> result;
                    result.has_min = true;
                    result.min = 0;
                    if(tmp.has_min && tmp.min >= Value_t(0))
                        result.min = fp_pow(tmp.min, exponent);
                    else if(tmp.has_max && tmp.max <= Value_t(0))
                        result.min = fp_pow(tmp.max, exponent);

                    result.has_max = false;
                    if(tmp.has_min && tmp.has_max)
                    {
                        result.has_max = true;
                        result.max     = fp_max(fp_abs(tmp.min), fp_abs(tmp.max));
                        result.max     = fp_pow(result.max, exponent);
                    }
                    return result;
                }

                range<Value_t> p0 = CalculateResultBoundaries( tree.GetParam(0) );
                range<Value_t> p1 = CalculateResultBoundaries( tree.GetParam(1) );
                TriTruthValue p0_positivity =
                    (p0.has_min && (p0.min) >= Value_t(0)) ? IsAlways
                  : (p0.has_max && (p0.max) < Value_t(0) ? IsNever
                    : Unknown);
                TriTruthValue p1_evenness = GetEvennessInfo(tree.GetParam(1));

                /* If param0 IsAlways, the return value is also IsAlways */
                /* If param1 is even, the return value is IsAlways */
                /* If param1 is odd, the return value is same as param0's */
                /* If param0 is negative and param1 is not integer,
                 * the return value is imaginary (assumed Unknown)
                 *
                 * Illustrated in this truth table:
                 *  P=positive, N=negative
                 *  E=even, O=odd, U=not integer
                 *  *=unknown, X=invalid (unknown), x=maybe invalid (unknown)
                 *
                 *   param1: PE PO P* NE NO N* PU NU *
                 * param0:
                 *   PE      P  P  P  P  P  P  P  P  P
                 *   PO      P  P  P  P  P  P  P  P  P
                 *   PU      P  P  P  P  P  P  P  P  P
                 *   P*      P  P  P  P  P  P  P  P  P
                 *   NE      P  N  *  P  N  *  X  X  x
                 *   NO      P  N  *  P  N  *  X  X  x
                 *   NU      P  N  *  P  N  *  X  X  x
                 *   N*      P  N  *  P  N  *  X  X  x
                 *   *       P  *  *  P  *  *  x  x  *
                 *
                 * Note: This also deals with the following opcodes:
                 *       cSqrt  (param0, PU) (x^0.5)
                 *       cRSqrt (param0, NU) (x^-0.5)
                 *       cExp   (PU, param1) (CONSTANT_E^x)
                 */
                TriTruthValue result_positivity = Unknown;
                switch(p0_positivity)
                {
                    case IsAlways:
                        // e.g.   5^x = positive.
                        result_positivity = IsAlways;
                        break;
                    case IsNever:
                    {
                        result_positivity = p1_evenness;
                        break;
                    }
                    default:
                        switch(p1_evenness)
                        {
                            case IsAlways:
                                // e.g. x^( 4) = positive
                                // e.g. x^(-4) = positive
                                result_positivity = IsAlways;
                                break;
                            case IsNever:
                                break;
                            case Unknown:
                            {
                                /* If p1 is const non-integer,
                                 * assume the result is positive
                                 * though it may be NaN instead.
                                 */
                                if(tree.GetParam(1).IsImmed()
                                && !isInteger(tree.GetParam(1).GetImmed())
                                && tree.GetParam(1).GetImmed() >= Value_t(0))
                                {
                                    result_positivity = IsAlways;
                                }
                                break;
                            }
                        }
                }
                switch(result_positivity)
                {
                    case IsAlways:
                    {
                        /* The result is always positive.
                         * Figure out whether we know the minimum value. */
                        Value_t min = Value_t(0);
                        if(p0.has_min && p1.has_min)
                        {
                            min = fp_pow(p0.min, p1.min);
                            if(p0.min < Value_t(0) && (!p1.has_max || p1.max >= Value_t(0)) && min >= Value_t(0))
                                min = Value_t(0);
                        }
                        if(p0.has_min && p0.min >= Value_t(0) && p0.has_max && p1.has_max)
                        {
                            Value_t max = fp_pow(p0.max, p1.max);
                            if(min > max) std::swap(min, max);
                            return range<Value_t>(min, max);
                        }
                        return range<Value_t>(min, false);
                    }
                    case IsNever:
                    {
                        /* The result is always negative.
                         * TODO: Figure out whether we know the maximum value.
                         */
                        return range<Value_t>(false, fp_const_negativezero<Value_t>());
                    }
                    default:
                    {
                        /* It can be negative or positive.
                         * We know nothing about the boundaries. */
                        break;
                    }
                }
                break;
            }

            /* The following opcodes are processed by GenerateFrom()
             * within fpoptimizer_bytecode_to_codetree.cc and thus
             * they will never occur in the calling context for the
             * most of the parsing context. They may however occur
             * at the late phase, so we deal with them.
             */
            case cNeg:
            {
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                m.set_neg();
                return m;
            }
            case cSub: // converted into cAdd(x, cNeg(y))
            {
                CodeTree<Value_t> tmp, tmp2;
                tmp2.SetOpcode(cNeg);
                tmp2.AddParam(tree.GetParam(1));
                tmp.SetOpcode(cAdd);
                tmp.AddParam(tree.GetParam(0));
                tmp.AddParamMove(tmp2);
                return CalculateResultBoundaries(tmp);
            }
            case cInv: // converted into cPow x -1
            {
                CodeTree<Value_t> tmp;
                tmp.SetOpcode(cPow);
                tmp.AddParam(tree.GetParam(0));
                tmp.AddParam(CodeTreeImmed(Value_t(-1)));
                return CalculateResultBoundaries(tmp);
            }
            case cDiv: // converted into cPow y -1
            {
                CodeTree<Value_t> tmp, tmp2;
                tmp2.SetOpcode(cInv);
                tmp2.AddParam(tree.GetParam(1));
                tmp.SetOpcode(cMul);
                tmp.AddParam(tree.GetParam(0));
                tmp.AddParamMove(tmp2);
                return CalculateResultBoundaries(tmp);
            }
            case cRad: // converted into cMul x CONSTANT_RD
            {
                CodeTree<Value_t> tmp;
                tmp.SetOpcode(cMul);
                tmp.AddParam(tree.GetParam(0));
                tmp.AddParam(CodeTreeImmed(fp_const_rad_to_deg<Value_t>()));
                return CalculateResultBoundaries(tmp);
            }
            case cDeg: // converted into cMul x CONSTANT_DR
            {
                CodeTree<Value_t> tmp;
                tmp.SetOpcode(cMul);
                tmp.AddParam(tree.GetParam(0));
                tmp.AddParam(CodeTreeImmed(fp_const_deg_to_rad<Value_t>()));
                return CalculateResultBoundaries(tmp);
            }
            case cSqr: // converted into cMul x x    or cPow x 2
            {
                CodeTree<Value_t> tmp;
                tmp.SetOpcode(cPow);
                tmp.AddParam(tree.GetParam(0));
                tmp.AddParam(CodeTreeImmed(Value_t(2)));
                return CalculateResultBoundaries(tmp);
            }
            case cExp: // converted into cPow CONSTANT_E x
            {
                CodeTree<Value_t> tmp;
                tmp.SetOpcode(cPow);
                tmp.AddParam(CodeTreeImmed(fp_const_e<Value_t>()));
                tmp.AddParam(tree.GetParam(0));
                return CalculateResultBoundaries(tmp);
            }
            case cExp2: // converted into cPow 2 x
            {
                CodeTree<Value_t> tmp;
                tmp.SetOpcode(cPow);
                tmp.AddParam(CodeTreeImmed(Value_t(2)));
                tmp.AddParam(tree.GetParam(0));
                return CalculateResultBoundaries(tmp);
            }
            case cCbrt: // converted into cPow x 0.33333333
            {
                // However, contrary to x^(1/3), this allows
                // negative values for x, and produces those
                // as well.
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                m.set_min_max(fp_cbrt);
                return m;
            }
            case cSqrt: // converted into cPow x 0.5
            {
                range<Value_t> m = CalculateResultBoundaries( tree.GetParam(0) );
                if(m.has_min) m.min = (m.min) < Value_t(0) ? 0 : fp_sqrt(m.min);
                if(m.has_max) m.max = (m.max) < Value_t(0) ? 0 : fp_sqrt(m.max);
                return m;
            }
            case cRSqrt: // converted into cPow x -0.5
            {
                CodeTree<Value_t> tmp;
                tmp.SetOpcode(cPow);
                tmp.AddParam(tree.GetParam(0));
                tmp.AddParam(CodeTreeImmed( Value_t(-0.5) ));
                return CalculateResultBoundaries(tmp);
            }
            case cHypot: // converted into cSqrt(cAdd(cMul(x x), cMul(y y)))
            {
                CodeTree<Value_t> xsqr, ysqr, add, sqrt;
                xsqr.AddParam(tree.GetParam(0)); xsqr.AddParam(CodeTreeImmed( Value_t(2) ));
                ysqr.AddParam(tree.GetParam(1)); ysqr.AddParam(CodeTreeImmed( Value_t(2) ));
                xsqr.SetOpcode(cPow); ysqr.SetOpcode(cPow);
                add.AddParamMove(xsqr); add.AddParamMove(ysqr);
                add.SetOpcode(cAdd); sqrt.AddParamMove(add);
                sqrt.SetOpcode(cSqrt);
                return CalculateResultBoundaries(sqrt);
            }
            case cLog2by: // converted into cMul y CONSTANT_L2I (cLog x)
            {
                CodeTree<Value_t> tmp, tmp2;
                tmp2.SetOpcode(cLog2);
                tmp2.AddParam(tree.GetParam(0));
                tmp.SetOpcode(cMul);
                tmp.AddParamMove(tmp2);
                tmp.AddParam(tree.GetParam(1));
                return CalculateResultBoundaries(tmp);
            }
            case cCot: // converted into 1 / cTan
            {
                CodeTree<Value_t> tmp, tmp2;
                tmp2.SetOpcode(cTan);
                tmp2.AddParam(tree.GetParam(0));
                tmp.SetOpcode(cInv);
                tmp.AddParamMove(tmp2);
                return CalculateResultBoundaries(tmp);
            }
            case cSec: // converted into 1 / cCos
            {
                CodeTree<Value_t> tmp, tmp2;
                tmp2.SetOpcode(cCos);
                tmp2.AddParam(tree.GetParam(0));
                tmp.SetOpcode(cInv);
                tmp.AddParamMove(tmp2);
                return CalculateResultBoundaries(tmp);
            }
            case cCsc: // converted into 1 / cSin
            {
                CodeTree<Value_t> tmp, tmp2;
                tmp2.SetOpcode(cSin);
                tmp2.AddParam(tree.GetParam(0));
                tmp.SetOpcode(cInv);
                tmp.AddParamMove(tmp2);
                return CalculateResultBoundaries(tmp);
            }
            /* The following opcodes are processed by GenerateFrom()
             * within fpoptimizer_bytecode_to_codetree.cc and thus
             * they will never occur in the calling context:
             */
                break; /* Should never occur */

            /* Opcodes that do not occur in the tree for other reasons */
            case cRDiv: // version of cDiv
            case cRSub: // version of cSub
            case cDup:
            case cFetch:
            case cPopNMov:
            case cSinCos:
            case cSinhCosh:
            case cNop:
            case cJump:
            case VarBegin:
                break; /* Should never occur */

            /* Opcodes that are completely unpredictable */
            case cPCall:
                break;
            case cFCall:
                break; // Cannot deduce
            case cEval:
                break; // Cannot deduce
        }
        return range<Value_t>(); /* Cannot deduce */
    }

    template<typename Value_t>
    TriTruthValue GetIntegerInfo(const CodeTree<Value_t>& tree)
    {
        switch(tree.GetOpcode())
        {
            case cImmed:
                return isInteger(tree.GetImmed()) ? IsAlways : IsNever;
            case cFloor:
            case cCeil:
            case cTrunc:
            case cInt:
                return IsAlways;
            case cAnd:
            case cOr:
            case cNot:
            case cNotNot:
            case cEqual:
            case cNEqual:
            case cLess:
            case cLessOrEq:
            case cGreater:
            case cGreaterOrEq:
                /* These operations always produce truth values (0 or 1) */
                return IsAlways; /* 0 and 1 are both integers */
            case cIf:
            {
                TriTruthValue a = GetIntegerInfo(tree.GetParam(1));
                TriTruthValue b = GetIntegerInfo(tree.GetParam(2));
                if(a == b) return a;
                return Unknown;
            }
            case cAdd:
            case cMul:
            {
                // It's integer if all the components are integer
                // Otherwise, unknown whether it's integer
                // A confirmed non-integer does not necessarily
                // mean the result isn't an integer, because:
                // 0.5 + 0.5 = 1.0; sqrt(2) * sqrt(2) = 2.0
                for(size_t a=tree.GetParamCount(); a-- > 0; )
                    if(GetIntegerInfo(tree.GetParam(a)) != IsAlways)
                        return Unknown;
                return IsAlways;
            }
            default:
                break;
        }
        return Unknown; /* Don't know whether it's integer. */
    }

    template<typename Value_t>
    bool IsLogicalValue(const CodeTree<Value_t>& tree)
    {
        switch(tree.GetOpcode())
        {
            case cImmed:
                return fp_equal(tree.GetImmed(), Value_t(0))
                    || fp_equal(tree.GetImmed(), Value_t(1));
            case cAnd:
            case cOr:
            case cNot:
            case cNotNot:
            case cAbsAnd:
            case cAbsOr:
            case cAbsNot:
            case cAbsNotNot:
            case cEqual:
            case cNEqual:
            case cLess:
            case cLessOrEq:
            case cGreater:
            case cGreaterOrEq:
                /* These operations always produce truth values (0 or 1) */
                return true;
            case cMul:
            {
                for(size_t a=tree.GetParamCount(); a-- > 0; )
                    if(!IsLogicalValue(tree.GetParam(a)))
                        return false;
                return true;
            }
            case cIf:
            case cAbsIf:
            {
                return IsLogicalValue(tree.GetParam(1))
                    && IsLogicalValue(tree.GetParam(2));
            }
            default:
                break;
        }
        return false; // Not a logical value.
    }
}

/* BEGIN_EXPLICIT_INSTANTATION */
#include "instantiate.hh"
namespace FPoptimizer_CodeTree
{
#define FP_INSTANTIATE(type) \
    template range<type> CalculateResultBoundaries(const CodeTree<type> &); \
    template bool IsLogicalValue(const CodeTree<type> &); \
    template TriTruthValue GetIntegerInfo(const CodeTree<type> &);
    FPOPTIMIZER_EXPLICITLY_INSTANTIATE(FP_INSTANTIATE)
#undef FP_INSTANTIATE
}
/* END_EXPLICIT_INSTANTATION */

#endif
