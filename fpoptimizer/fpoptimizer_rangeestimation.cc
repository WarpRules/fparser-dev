#include "fpoptimizer_codetree.hh"
#include "fpoptimizer_consts.hh"

#include <cmath> /* for CalculateResultBoundaries() */

#ifdef FP_SUPPORT_OPTIMIZER

using namespace FUNCTIONPARSERTYPES;
using namespace FPoptimizer_CodeTree;

//#define DEBUG_SUBSTITUTIONS_extra_verbose

namespace FPoptimizer_CodeTree
{
    MinMaxTree CodeTree::CalculateResultBoundaries() const
#ifdef DEBUG_SUBSTITUTIONS_extra_verbose
    {
        MinMaxTree tmp = CalculateResultBoundaries_do();
        std::cout << "Estimated boundaries: ";
        if(tmp.has_min) std::cout << tmp.min; else std::cout << "-inf";
        std::cout << " .. ";
        if(tmp.has_max) std::cout << tmp.max; else std::cout << "+inf";
        std::cout << ": ";
        FPoptimizer_CodeTree::DumpTree(*this);
        std::cout << std::endl;
        return tmp;
    }
    MinMaxTree CodeTree::CalculateResultBoundaries_do() const
#endif
    {
        using namespace std;
        switch( GetOpcode() )
        {
            case cImmed:
                return MinMaxTree(GetImmed(), GetImmed()); // a definite value.
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
                return MinMaxTree( 0.0, 1.0 );
            }
            case cAbs:
            {
                /* cAbs always produces a positive value */
                MinMaxTree m = GetParam(0).CalculateResultBoundaries();
                if(m.has_min && m.has_max)
                {
                    if(m.min < 0.0 && m.max >= 0.0) // ex. -10..+6 or -6..+10
                    {
                        /* -x..+y: spans across zero. min=0, max=greater of |x| and |y|. */
                        double tmp = -m.min; if(tmp > m.max) m.max = tmp;
                        m.min = 0.0; m.has_min = true;
                    }
                    else if(m.min < 0.0) // ex. -10..-4
                        { double tmp = m.max; m.max = -m.min; m.min = -tmp; }
                }
                else if(!m.has_min && m.has_max && m.max < 0.0) // ex. -inf..-10
                {
                    m.min = fabs(m.max); m.has_min = true; m.has_max = false;
                }
                else if(!m.has_max && m.has_min && m.min > 0.0) // ex. +10..+inf
                {
                    m.min = fabs(m.min); m.has_min = true; m.has_max = false;
                }
                else // ex. -inf..+inf, -inf..+10, -10..+inf
                {
                    // all of these cover -inf..0, 0..+inf, or both
                    m.min = 0.0; m.has_min = true; m.has_max = false;
                }
                return m;
            }

            case cLog: /* Defined for 0.0 < x <= inf */
            {
                MinMaxTree m = GetParam(0).CalculateResultBoundaries();
                if(m.has_min) { if(m.min < 0.0) m.has_min = false; else m.min = log(m.min); } // No boundaries
                if(m.has_max) { if(m.max < 0.0) m.has_max = false; else m.max = log(m.max); }
                return m;
            }

            case cLog2: /* Defined for 0.0 < x <= inf */
            {
                MinMaxTree m = GetParam(0).CalculateResultBoundaries();
                if(m.has_min) { if(m.min < 0.0) m.has_min = false; else m.min = fp_log2(m.min); } // No boundaries
                if(m.has_max) { if(m.max < 0.0) m.has_max = false; else m.max = fp_log2(m.max); }
                return m;
            }

            case cLog10: /* Defined for 0.0 < x <= inf */
            {
                MinMaxTree m = GetParam(0).CalculateResultBoundaries();
                if(m.has_min) { if(m.min < 0.0) m.has_min = false; else m.min = fp_log10(m.min); }
                if(m.has_max) { if(m.max < 0.0) m.has_max = false; else m.max = fp_log10(m.max); }
                return m;
            }

            case cAcosh: /* defined for             1.0 <  x <= inf */
            {
                MinMaxTree m = GetParam(0).CalculateResultBoundaries();
                if(m.has_min) { if(m.min <= 1.0) m.has_min = false; else m.min = fp_acosh(m.min); } // No boundaries
                if(m.has_max) { if(m.max <= 1.0) m.has_max = false; else m.max = fp_acosh(m.max); }
                return m;
            }
            case cAsinh: /* defined for all values -inf <= x <= inf */
            {
                MinMaxTree m = GetParam(0).CalculateResultBoundaries();
                if(m.has_min) m.min = fp_asinh(m.min); // No boundaries
                if(m.has_max) m.max = fp_asinh(m.max);
                return m;
            }
            case cAtanh: /* defined for all values -inf <= x <= inf */
            {
                MinMaxTree m = GetParam(0).CalculateResultBoundaries();
                if(m.has_min) m.min = fp_atanh(m.min); // No boundaries
                if(m.has_max) m.max = fp_atanh(m.max);
                return m;
            }
            case cAcos: /* defined for -1.0 <= x < 1, results within CONSTANT_PI..0 */
            {
                /* Somewhat complicated to narrow down from this */
                /* TODO: A resourceful programmer may add it later. */
                return MinMaxTree( 0.0, CONSTANT_PI );
            }
            case cAsin: /* defined for -1.0 <= x < 1, results within -CONSTANT_PIHALF..CONSTANT_PIHALF */
            {
                /* Somewhat complicated to narrow down from this */
                /* TODO: A resourceful programmer may add it later. */
                return MinMaxTree( -CONSTANT_PIHALF, CONSTANT_PIHALF );
            }
            case cAtan: /* defined for all values -inf <= x <= inf */
            {
                MinMaxTree m = GetParam(0).CalculateResultBoundaries();
                if(m.has_min) m.min = atan(m.min); else { m.min = -CONSTANT_PIHALF; m.has_min = true; }
                if(m.has_max) m.max = atan(m.max); else { m.max =  CONSTANT_PIHALF; m.has_max = true; }
                return m;
            }
            case cAtan2: /* too complicated to estimate */
            {
                MinMaxTree p0 = GetParam(0).CalculateResultBoundaries();
                MinMaxTree p1 = GetParam(1).CalculateResultBoundaries();
                if(GetParam(0).IsImmed()
                && FloatEqual(GetParam(0).GetImmed(), 0.0))   // y == 0
                {
                    // Either 0.0 or CONSTANT_PI
                    return MinMaxTree(0.0, CONSTANT_PI);
                }
                if(GetParam(1).IsImmed()
                && FloatEqual(GetParam(1).GetImmed(), 0.0))   // x == 0
                {
                    // EIther -CONSTANT_PIHALF or +CONSTANT_PIHALF
                    return MinMaxTree(-CONSTANT_PIHALF, CONSTANT_PIHALF);
                }
                // Anything else
                /* Somewhat complicated to narrow down from this */
                /* TODO: A resourceful programmer may add it later. */
                return MinMaxTree(-CONSTANT_PI, CONSTANT_PI);
            }

            case cSin:
            case cCos:
            {
                /* Could be narrowed down from here,
                 * but it's too complicated due to
                 * the cyclic nature of the function. */
                /* TODO: A resourceful programmer may add it later. */
                return MinMaxTree(-1.0, 1.0);
            }
            case cTan:
            {
                /* Could be narrowed down from here,
                 * but it's too complicated due to
                 * the cyclic nature of the function */
                /* TODO: A resourceful programmer may add it later. */
                return MinMaxTree(); // (CONSTANT_NEG_INF, CONSTANT_POS_INF);
            }

            case cCeil:
            {
                MinMaxTree m = GetParam(0).CalculateResultBoundaries();
                m.max = std::ceil(m.max); // ceil() may increase the value, may not decrease
                return m;
            }
            case cFloor:
            {
                MinMaxTree m = GetParam(0).CalculateResultBoundaries();
                m.min = std::floor(m.min); // floor() may decrease the value, may not increase
                return m;
            }
            case cTrunc:
            {
                MinMaxTree m = GetParam(0).CalculateResultBoundaries();
                m.min = std::floor(m.min); // trunc() may either increase or decrease the value
                m.max = std::ceil(m.max); // for safety, we assume both
                return m;
            }
            case cInt:
            {
                MinMaxTree m = GetParam(0).CalculateResultBoundaries();
                m.min = std::floor(m.min); // int() may either increase or decrease the value
                m.max = std::ceil(m.max); // for safety, we assume both
                return m;
            }
            case cSinh: /* defined for all values -inf <= x <= inf */
            {
                MinMaxTree m = GetParam(0).CalculateResultBoundaries();
                if(m.has_min) m.min = sinh(m.min); // No boundaries
                if(m.has_max) m.max = sinh(m.max);
                return m;
            }
            case cTanh: /* defined for all values -inf <= x <= inf */
            {
                MinMaxTree m = GetParam(0).CalculateResultBoundaries();
                if(m.has_min) m.min = tanh(m.min); // No boundaries
                if(m.has_max) m.max = tanh(m.max);
                return m;
            }
            case cCosh: /* defined for all values -inf <= x <= inf, results within 1..inf */
            {
                MinMaxTree m = GetParam(0).CalculateResultBoundaries();
                if(m.has_min)
                {
                    if(m.has_max) // max, min
                    {
                        if(m.min >= 0.0 && m.max >= 0.0) // +x .. +y
                            { m.min = cosh(m.min); m.max = cosh(m.max); }
                        else if(m.min < 0.0 && m.max >= 0.0) // -x .. +y
                            { double tmp = cosh(m.min); m.max = cosh(m.max);
                              if(tmp > m.max) m.max = tmp;
                              m.min = 1.0; }
                        else // -x .. -y
                            { m.min = cosh(m.min); m.max = cosh(m.max);
                              std::swap(m.min, m.max); }
                    }
                    else // min, no max
                    {
                        if(m.min >= 0.0) // 0..inf -> 1..inf
                            { m.has_max = true; m.max = cosh(m.min); m.min = 1.0; }
                        else
                            { m.has_max = false; m.min = 1.0; } // Anything between 1..inf
                    }
                }
                else // no min
                {
                    m.has_min = true; m.min = 1.0; // always a lower boundary
                    if(m.has_max) // max, no min
                    {
                        m.min = cosh(m.max); // n..inf
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
                MinMaxTree res1 = GetParam(1).CalculateResultBoundaries();
                MinMaxTree res2 = GetParam(2).CalculateResultBoundaries();
                if(!res2.has_min) res1.has_min = false; else if(res2.min < res1.min) res1.min = res2.min;
                if(!res2.has_max) res1.has_max = false; else if(res2.max > res1.max) res1.max = res2.max;
                return res1;
            }

            case cMin:
            {
                bool has_unknown_min = false;
                bool has_unknown_max = false;

                MinMaxTree result;
                for(size_t a=0; a<GetParamCount(); ++a)
                {
                    MinMaxTree m = GetParam(a).CalculateResultBoundaries();
                    if(!m.has_min)
                        has_unknown_min = true;
                    else if(!result.has_min || m.min < result.min)
                        result.min = m.min;

                    if(!m.has_max)
                        has_unknown_max = true;
                    else if(!result.has_max || m.max < result.max)
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

                MinMaxTree result;
                for(size_t a=0; a<GetParamCount(); ++a)
                {
                    MinMaxTree m = GetParam(a).CalculateResultBoundaries();
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
                MinMaxTree result(0.0, 0.0);
                for(size_t a=0; a<GetParamCount(); ++a)
                {
                    MinMaxTree item = GetParam(a).CalculateResultBoundaries();

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
                    double value;

                    Value(ValueType t): valueType(t), value(0) {}
                    Value(double v): valueType(Finite), value(v) {}

                    bool isNegative() const
                    {
                        return valueType == MinusInf ||
                            (valueType == Finite && value < 0.0);
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

                MinMaxTree result(1.0, 1.0);
                for(size_t a=0; a<GetParamCount(); ++a)
                {
                    MinMaxTree item = GetParam(a).CalculateResultBoundaries();
                    if(!item.has_min && !item.has_max) return MinMaxTree(); // hopeless

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

                MinMaxTree x = GetParam(0).CalculateResultBoundaries();
                MinMaxTree y = GetParam(1).CalculateResultBoundaries();

                if(y.has_max)
                {
                    if(y.max >= 0.0)
                    {
                        if(!x.has_min || x.min < 0)
                            return MinMaxTree(-y.max, y.max);
                        else
                            return MinMaxTree(0.0, y.max);
                    }
                    else
                    {
                        if(!x.has_max || x.max >= 0)
                            return MinMaxTree(y.max, -y.max);
                        else
                            return MinMaxTree(y.max, NEGATIVE_MAXIMUM);
                    }
                }
                else
                    return MinMaxTree();
            }
            case cPow:
            {
                if(GetParam(1).IsImmed() && GetParam(1).GetImmed() == 0.0)
                {
                    // Note: This makes 0^0 evaluate into 1.
                    return MinMaxTree(1.0, 1.0); // x^0 = 1
                }
                if(GetParam(0).IsImmed() && GetParam(0).GetImmed() == 0.0)
                {
                    // Note: This makes 0^0 evaluate into 0.
                    return MinMaxTree(0.0, 0.0); // 0^x = 0
                }
                if(GetParam(0).IsImmed() && FloatEqual(GetParam(0).GetImmed(), 1.0))
                {
                    return MinMaxTree(1.0, 1.0); // 1^x = 1
                }
                if(GetParam(1).IsImmed()
                && GetParam(1).GetImmed() > 0
                && GetParam(1).IsAlwaysParity(false))
                {
                    // x ^ even_int_const always produces a non-negative value.
                    double exponent = GetParam(1).GetImmed();
                    MinMaxTree tmp = GetParam(0).CalculateResultBoundaries();
                    MinMaxTree result;
                    result.has_min = true;
                    result.min = 0;
                    if(tmp.has_min && tmp.min >= 0)
                        result.min = fp_pow(tmp.min, exponent);
                    else if(tmp.has_max && tmp.max <= 0)
                        result.min = fp_pow(tmp.max, exponent);

                    result.has_max = false;
                    if(tmp.has_min && tmp.has_max)
                    {
                        result.has_max = true;
                        result.max     = std::max(fabs(tmp.min), fabs(tmp.max));
                        result.max     = fp_pow(result.max, exponent);
                    }
                    return result;
                }

                MinMaxTree p0 = GetParam(0).CalculateResultBoundaries();
                MinMaxTree p1 = GetParam(1).CalculateResultBoundaries();
                TriTruthValue p0_positivity =
                    (p0.has_min && p0.min >= 0.0) ? IsAlways
                  : (p0.has_max && p0.max < 0.0 ? IsNever
                    : Unknown);
                TriTruthValue p1_evenness = GetParam(1).GetEvennessInfo();

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
                                if(GetParam(1).IsImmed()
                                && GetParam(1).IsAlwaysInteger(false)
                                && GetParam(1).GetImmed() >= 0.0)
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
                        double min = 0.0;
                        if(p0.has_min && p1.has_min)
                        {
                            min = pow(p0.min, p1.min);
                            if(p0.min < 0.0 && (!p1.has_max || p1.max >= 0.0) && min >= 0.0)
                                min = 0.0;
                        }
                        if(p0.has_min && p0.min >= 0.0 && p0.has_max && p1.has_max)
                        {
                            double max = pow(p0.max, p1.max);
                            if(min > max) std::swap(min, max);
                            return MinMaxTree(min, max);
                        }
                        return MinMaxTree(min, false);
                    }
                    case IsNever:
                    {
                        /* The result is always negative.
                         * TODO: Figure out whether we know the maximum value.
                         */
                        return MinMaxTree(false, NEGATIVE_MAXIMUM);
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
                CodeTree tmp;
                tmp.SetOpcode(cMul);
                tmp.AddParam(CodeTree(-1.0));
                tmp.AddParam(GetParam(0));
                return tmp.CalculateResultBoundaries();
            }
            case cSub: // converted into cMul y -1
            {
                CodeTree tmp, tmp2;
                tmp2.SetOpcode(cNeg);
                tmp2.AddParam(GetParam(1));
                tmp.SetOpcode(cAdd);
                tmp.AddParam(GetParam(0));
                tmp.AddParamMove(tmp2);
                return tmp.CalculateResultBoundaries();
            }
            case cInv: // converted into cPow x -1
            {
                CodeTree tmp;
                tmp.SetOpcode(cPow);
                tmp.AddParam(GetParam(0));
                tmp.AddParam(CodeTree(-1.0));
                return tmp.CalculateResultBoundaries();
            }
            case cDiv: // converted into cPow y -1
            {
                CodeTree tmp, tmp2;
                tmp2.SetOpcode(cInv);
                tmp2.AddParam(GetParam(1));
                tmp.SetOpcode(cMul);
                tmp.AddParam(GetParam(0));
                tmp.AddParamMove(tmp2);
                return tmp.CalculateResultBoundaries();
            }
            case cRad: // converted into cMul x CONSTANT_RD
            {
                CodeTree tmp;
                tmp.SetOpcode(cMul);
                tmp.AddParam(GetParam(0));
                tmp.AddParam(CodeTree(CONSTANT_RD));
                return tmp.CalculateResultBoundaries();
            }
            case cDeg: // converted into cMul x CONSTANT_DR
            {
                CodeTree tmp;
                tmp.SetOpcode(cMul);
                tmp.AddParam(GetParam(0));
                tmp.AddParam(CodeTree(CONSTANT_DR));
                return tmp.CalculateResultBoundaries();
            }
            case cSqr: // converted into cMul x x    or cPow x 2
            {
                CodeTree tmp;
                tmp.SetOpcode(cPow);
                tmp.AddParam(GetParam(0));
                tmp.AddParam(CodeTree(2.0));
                return tmp.CalculateResultBoundaries();
            }
            case cExp: // converted into cPow CONSTANT_E x
            {
                CodeTree tmp;
                tmp.SetOpcode(cPow);
                tmp.AddParam(CodeTree(CONSTANT_E));
                tmp.AddParam(GetParam(0));
                return tmp.CalculateResultBoundaries();
            }
            case cExp2: // converted into cPow 2 x
            {
                CodeTree tmp;
                tmp.SetOpcode(cPow);
                tmp.AddParam(CodeTree(2.0));
                tmp.AddParam(GetParam(0));
                return tmp.CalculateResultBoundaries();
            }
            case cCbrt: // converted into cPow x 0.33333333
            {
                // However, contrary to x^(1/3), this allows
                // negative values for x, and produces those
                // as well.
                MinMaxTree result = GetParam(0).CalculateResultBoundaries();
                if(result.has_min) result.min = fp_cbrt(result.min);
                if(result.has_max) result.max = fp_cbrt(result.max);
                return result;
            }
            case cSqrt: // converted into cPow x 0.5
            {
                MinMaxTree result = GetParam(0).CalculateResultBoundaries();
                if(result.has_min) result.min = result.min < 0 ? 0 : fp_sqrt(result.min);
                if(result.has_max) result.max = result.max < 0 ? 0 : fp_sqrt(result.max);
                return result;
            }
            case cRSqrt: // converted into cPow x -0.5
            {
                CodeTree tmp;
                tmp.SetOpcode(cPow);
                tmp.AddParam(GetParam(0));
                tmp.AddParam(CodeTree(-0.5));
                return tmp.CalculateResultBoundaries();
            }
            case cLog2by: // converted into cMul y CONSTANT_L2I (cLog x)
            {
                CodeTree tmp, tmp2;
                tmp2.SetOpcode(cLog2);
                tmp2.AddParam(GetParam(0));
                tmp.SetOpcode(cMul);
                tmp.AddParamMove(tmp2);
                tmp.AddParam(GetParam(1));
                return tmp.CalculateResultBoundaries();
            }
            case cCot: // converted into 1 / cTan
            {
                CodeTree tmp, tmp2;
                tmp2.SetOpcode(cTan);
                tmp2.AddParam(GetParam(0));
                tmp.SetOpcode(cInv);
                tmp.AddParamMove(tmp2);
                return tmp.CalculateResultBoundaries();
            }
            case cSec: // converted into 1 / cCos
            {
                CodeTree tmp, tmp2;
                tmp2.SetOpcode(cCos);
                tmp2.AddParam(GetParam(0));
                tmp.SetOpcode(cInv);
                tmp.AddParamMove(tmp2);
                return tmp.CalculateResultBoundaries();
            }
            case cCsc: // converted into 1 / cSin
            {
                CodeTree tmp, tmp2;
                tmp2.SetOpcode(cSin);
                tmp2.AddParam(GetParam(0));
                tmp.SetOpcode(cInv);
                tmp.AddParamMove(tmp2);
                return tmp.CalculateResultBoundaries();
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
        return MinMaxTree(); /* Cannot deduce */
    }
}

#endif
