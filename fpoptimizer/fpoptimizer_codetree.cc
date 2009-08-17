#include <cmath>
#include <list>
#include <algorithm>

#include <cmath> /* for CalculateResultBoundaries() */

#include "fpoptimizer_codetree.hh"
#include "fptypes.hh"

#include "fpoptimizer_consts.hh"

#ifdef FP_SUPPORT_OPTIMIZER

using namespace FUNCTIONPARSERTYPES;
//using namespace FPoptimizer_Grammar;


namespace FPoptimizer_CodeTree
{
    CodeTree::CodeTree()
        : RefCount(0), Opcode(), Params(), Hash(), Depth(1), Parent(), OptimizedUsing(0)
    {
    }

    CodeTree::CodeTree(double i)
        : RefCount(0), Opcode(cImmed), Params(), Hash(), Depth(1), Parent(), OptimizedUsing(0)
    {
        Value = i;
    }

    CodeTree::~CodeTree()
    {
    }

    void CodeTree::Rehash(
        bool child_triggered)
    {
        /* If we were triggered by a parent, recurse to children */
        if(!child_triggered)
        {
            for(size_t a=0; a<Params.size(); ++a)
                Params[a].param->Rehash(false);
        }

        Recalculate_Hash_NoRecursion();

        /* If we were triggered by a child, recurse to the parent */
        if(child_triggered && Parent)
        {
            //assert(Parent->RefCount > 0);
            Parent->Rehash(true);
        }
    }

    struct ParamComparer
    {
        bool operator() (const CodeTree::Param& a, const CodeTree::Param& b) const
        {
            if(a.param->Depth != b.param->Depth)
                return a.param->Depth > b.param->Depth;
            if(a.sign != b.sign) return a.sign < b.sign;
            return a.param->Hash < b.param->Hash;
        }
    };

    void CodeTree::Sort()
    {
        /* If the tree is commutative, order the parameters
         * in a set order in order to make equality tests
         * efficient in the optimizer
         */
        switch(Opcode)
        {
            case cAdd:
            case cMul:
            case cMin:
            case cMax:
            case cAnd:
            case cOr:
            case cEqual:
            case cNEqual:
                std::sort(Params.begin(), Params.end(), ParamComparer());
                break;
            case cLess:
                if(ParamComparer() (Params[1], Params[0]))
                    { std::swap(Params[0], Params[1]); Opcode = cGreater; }
                break;
            case cLessOrEq:
                if(ParamComparer() (Params[1], Params[0]))
                    { std::swap(Params[0], Params[1]); Opcode = cGreaterOrEq; }
                break;
            case cGreater:
                if(ParamComparer() (Params[1], Params[0]))
                    { std::swap(Params[0], Params[1]); Opcode = cLess; }
                break;
            case cGreaterOrEq:
                if(ParamComparer() (Params[1], Params[0]))
                    { std::swap(Params[0], Params[1]); Opcode = cLessOrEq; }
                break;
        }
    }

    void CodeTree::Sort_Recursive()
    {
        Sort();
        for(size_t a=0; a<Params.size(); ++a)
            Params[a].param->Sort_Recursive();
        Recalculate_Hash_NoRecursion();
    }

    void CodeTree::Recalculate_Hash_NoRecursion()
    {
        fphash_t NewHash = Opcode * FPHASH_CONST(0x3A83A83A83A83A0);
        Depth = 1;
        switch(Opcode)
        {
            case cImmed:
                // FIXME: not portable - we're casting double* into uint_least64_t*
                if(Value != 0.0)
                    NewHash ^= *(fphash_t*)&Value;
                break; // no params
            case cVar:
                NewHash ^= (Var<<24) | (Var>>24);
                break; // no params
            case cFCall: case cPCall:
                NewHash ^= (Funcno<<24) | (Funcno>>24);
                /* passthru */
            default:
            {
                size_t MaxChildDepth = 0;
                for(size_t a=0; a<Params.size(); ++a)
                {
                    if(Params[a].param->Depth > MaxChildDepth)
                        MaxChildDepth = Params[a].param->Depth;

                    NewHash += (1+Params[a].sign)*FPHASH_CONST(0x2492492492492492);
                    NewHash *= FPHASH_CONST(1099511628211);
                    //assert(&*Params[a].param != this);
                    NewHash += Params[a].param->Hash;
                }
                Depth += MaxChildDepth;
            }
        }
        if(Hash != NewHash)
        {
            Hash = NewHash;
            OptimizedUsing = 0;
        }
    }

    CodeTree* CodeTree::Clone()
    {
        CodeTree* result = new CodeTree;
        result->Opcode = Opcode;
        switch(Opcode)
        {
            case cImmed:
                result->Value  = Value;
                break;
            case cVar:
                result->Var = Var;
                break;
            case cFCall: case cPCall:
                result->Funcno = Funcno;
                break;
        }
        result->SetParams(Params);
        result->Hash   = Hash;
        result->Depth  = Depth;
        //assert(Parent->RefCount > 0);
        result->Parent = Parent;
        return result;
    }

    void CodeTree::AddParam(const Param& param)
    {
        Params.push_back(param);
        Params.back().param->Parent = this;
    }

    void CodeTree::SetParams(const std::vector<Param>& RefParams)
    {
        Params = RefParams;
        /**
        *** Note: The only reason we need to CLONE the children here
        ***       is because they must have the correct Parent field.
        ***       The Parent is required because of backward-recursive
        ***       hash regeneration. Is there any way around this?
        */

        for(size_t a=0; a<Params.size(); ++a)
        {
            Params[a].param = Params[a].param->Clone();
            Params[a].param->Parent = this;
        }
    }

    void CodeTree::DelParam(size_t index)
    {
        Params.erase(Params.begin() + index);
    }

    CodeTree::MinMaxTree CodeTree::CalculateResultBoundaries() const
    {
        using namespace std;

        switch( (OPCODE) Opcode)
        {
            case cImmed:
                return MinMaxTree(Value, Value); // a definite value.
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
            {
                /* These operations always produce truth values (0 or 1) */
                /* Narrowing them down is a matter of performing Constant optimization */
                return MinMaxTree( 0.0, 1.0 );
            }
            case cAbs:
            {
                /* cAbs always produces a positive value */
                MinMaxTree m = Params[0].param->CalculateResultBoundaries();
                if(m.min < 0.0 && m.max >= 0.0)
                    { double tmp = -m.min; if(tmp > m.max) m.max = tmp;
                    m.min = 0.0; }
                else if(m.min < 0.0)
                    { double tmp = m.max; m.max = -m.min; m.min = -tmp; }
                return m;
            }

            case cLog: /* Defined for 0.0 < x <= inf */
            {
                MinMaxTree m = Params[0].param->CalculateResultBoundaries();
                if(m.has_min) { if(m.min < 0.0) m.has_min = false; else m.min = log(m.min); } // No boundaries
                if(m.has_max) { if(m.max < 0.0) m.has_max = false; else m.max = log(m.max); }
                return m;
            }

#         ifndef FP_NO_ASINH
            case cAcosh: /* defined for             1.0 <  x <= inf */
            {
                MinMaxTree m = Params[0].param->CalculateResultBoundaries();
                if(m.has_min) { if(m.min <= 1.0) m.has_min = false; else m.min = acosh(m.min); } // No boundaries
                if(m.has_max) { if(m.max <= 1.0) m.has_max = false; else m.max = acosh(m.max); }
                return m;
            }
            case cAsinh: /* defined for all values -inf <= x <= inf */
            {
                MinMaxTree m = Params[0].param->CalculateResultBoundaries();
                if(m.has_min) m.min = asinh(m.min); // No boundaries
                if(m.has_max) m.max = asinh(m.max);
                return m;
            }
            case cAtanh: /* defined for all values -inf <= x <= inf */
            {
                MinMaxTree m = Params[0].param->CalculateResultBoundaries();
                if(m.has_min) m.min = atanh(m.min); // No boundaries
                if(m.has_max) m.max = atanh(m.max);
                return m;
            }
#         endif
            case cAcos: /* defined for -1.0 <= x < 1, results within CONSTANT_PI..0 */
            {
                /* Somewhat complicated to narrow down from this */
                return MinMaxTree( 0.0, CONSTANT_PI );
            }
            case cAsin: /* defined for -1.0 <= x < 1, results within -CONSTANT_PIHALF..CONSTANT_PIHALF */
            {
                /* Somewhat complicated to narrow down from this */
                /* A resourceful programmer may add it later. */
                return MinMaxTree( -CONSTANT_PIHALF, CONSTANT_PIHALF );
            }
            case cAtan: /* defined for all values -inf <= x <= inf */
            {
                MinMaxTree m = Params[0].param->CalculateResultBoundaries();
                if(m.has_min) m.min = atan(m.min); else { m.min = -CONSTANT_PIHALF; m.has_min = true; }
                if(m.has_max) m.max = atan(m.max); else { m.max =  CONSTANT_PIHALF; m.has_max = true; }
                return m;
            }
            case cAtan2: /* too complicated to estimate */
            {
                /* Somewhat complicated to narrow down from this */
                /* A resourceful programmer may add it later. */
                return MinMaxTree(-CONSTANT_PI, CONSTANT_PI);
            }

            case cSin:
            case cCos:
            {
                /* Could be narrowed down from here,
                 * but it's too complicated due to
                 * the cyclic nature of the function. */
                /* A resourceful programmer may add it later. */
                return MinMaxTree(-1.0, 1.0);
            }
            case cTan:
            {
                /* Could be narrowed down from here,
                 * but it's too complicated due to
                 * the cyclic nature of the function */
                /* A resourceful programmer may add it later. */
                return MinMaxTree(); // (CONSTANT_NEG_INF, CONSTANT_POS_INF);
            }

            case cCeil:
            {
                MinMaxTree m = Params[0].param->CalculateResultBoundaries();
                m.max = std::ceil(m.max); // ceil() may increase the value, may not decrease
                return m;
            }
            case cFloor:
            {
                MinMaxTree m = Params[0].param->CalculateResultBoundaries();
                m.min = std::floor(m.min); // floor() may decrease the value, may not increase
                return m;
            }
            case cInt:
            {
                MinMaxTree m = Params[0].param->CalculateResultBoundaries();
                m.min = std::floor(m.min); // int() may either increase or decrease the value
                m.max = std::ceil(m.max); // for safety, we assume both
                return m;
            }
            case cSinh: /* defined for all values -inf <= x <= inf */
            {
                MinMaxTree m = Params[0].param->CalculateResultBoundaries();
                if(m.has_min) m.min = sinh(m.min); // No boundaries
                if(m.has_max) m.max = sinh(m.max);
                return m;
            }
            case cTanh: /* defined for all values -inf <= x <= inf */
            {
                MinMaxTree m = Params[0].param->CalculateResultBoundaries();
                if(m.has_min) m.min = tanh(m.min); // No boundaries
                if(m.has_max) m.max = tanh(m.max);
                return m;
            }
            case cCosh: /* defined for all values -inf <= x <= inf, results within 1..inf */
            {
                MinMaxTree m = Params[0].param->CalculateResultBoundaries();
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
                        m.max = cosh(m.max); // 1..inf
                    else // no max, no min
                        m.has_max = false; // No upper boundary
                }
                return m;
            }

            case cIf:
            {
                // No guess which branch is chosen. Produce a spanning min & max.
                MinMaxTree res1 = Params[1].param->CalculateResultBoundaries();
                MinMaxTree res2 = Params[2].param->CalculateResultBoundaries();
                if(!res2.has_min) res1.has_min = false; else if(res2.min < res1.min) res1.min = res2.min;
                if(!res2.has_max) res2.has_max = false; else if(res2.max > res1.max) res2.max = res2.max;
                return res1;
            }

            case cMin:
            {
                bool has_unknown_min = false;
                bool has_unknown_max = false;

                MinMaxTree result;
                for(size_t a=0; a<Params.size(); ++a)
                {
                    MinMaxTree m = Params[a].param->CalculateResultBoundaries();
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
                for(size_t a=0; a<Params.size(); ++a)
                {
                    MinMaxTree m = Params[a].param->CalculateResultBoundaries();
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
                for(size_t a=0; a<Params.size(); ++a)
                {
                    const Param& p = Params[a];
                    MinMaxTree item = p.param->CalculateResultBoundaries();

                    if(p.sign) // subtraction
                    {
                        /* FIXME: is this right? */
                        if(item.has_min) result.max -= item.min;
                        else             result.has_max = false;
                        if(item.has_max) result.min -= item.max;
                        else             result.has_min = false;
                    }
                    else // addition
                    {
                        if(item.has_min) result.min += item.min;
                        else             result.has_min = false;
                        if(item.has_max) result.max += item.max;
                        else             result.has_max = false;
                    }
                    if(!result.has_min && !result.has_max) break; // hopeless
                }
                if(result.has_min && result.has_max
                && result.min > result.max) std::swap(result.min, result.max);
                return result;
            }
            case cMul:
            {
                /* It's complicated. Follow the logic below. */
                /* Note: This also deals with the following opcodes:
                 *       cInv, cDiv, cRDiv, cRad, cDeg, cSqr
                 *       cCot, Sec, cCsc, cLog2, cLog10
                 */
                MinMaxTree result(1.0, 1.0);
                for(size_t a=0; a<Params.size(); ++a)
                {
                    const Param& p = Params[a];
                    MinMaxTree item = p.param->CalculateResultBoundaries();

                    if(p.sign) // division
                    {
                        /* FIXME: is this right? */
                        if(item.has_min) result.max /= item.min;
                        else             result.has_max = false;
                        if(item.has_max) result.min /= item.max;
                        else             result.has_min = false;
                    }
                    else // multiplication
                    {
                        /* FIXME: is this right? */
                        if(item.has_min) result.min *= item.min;
                        else             result.has_min = false;
                        if(item.has_max) result.max *= item.max;
                        else             result.has_max = false;
                    }
                    if(!result.has_min && !result.has_max) break; // hopeless
                }
                if(result.has_min && result.has_max
                && result.min > result.max) std::swap(result.min, result.max);
                return result;
            }
            case cMod:
            {
                /* TODO: The boundaries of modulo operator could be estimated better. */

                MinMaxTree x = Params[0].param->CalculateResultBoundaries();
                MinMaxTree y = Params[1].param->CalculateResultBoundaries();

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
                            return MinMaxTree(y.max, -0.0);
                    }
                }
                else
                    return MinMaxTree();
            }
          #if 0
            case cPow:
            {
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
                switch(Params[0].param->GetPositivityInfo())
                {
                    case IsAlways:
                        // e.g.   5^x = positive.
                        return IsAlways;
                    case IsNever:
                        return Params[1].param->GetEvennessInfo();
                        // ^Simplifies the code below, same meaning
                        /* switch(Params[1].param->GetEvennessInfo())
                        {
                            case IsAlways:
                                // e.g. (-5)^( 4) = positive
                                // e.g. (-5)^(-4) = positive
                                return IsAlways;
                            case IsNever:
                                // e.g. (-5)^( 3) = negative
                                // e.g. (-5)^(-3) = negative
                                return IsNever;
                        } */
                    default:
                        switch(Params[1].param->GetEvennessInfo())
                        {
                            case IsAlways:
                                // e.g. x^( 4) = positive
                                // e.g. x^(-4) = positive
                                return IsAlways;
                            default:
                                break;
                        }
                }
                break;
            }
          #endif


            /* The following opcodes are processed by GenerateFrom()
             * within fpoptimizer_bytecode_to_codetree.cc and thus
             * they will never occur in the calling context:
             */
            case cNeg: // converted into cAdd ~x
            case cInv: // converted into cMul ~x
            case cDiv: // converted into cMul ~x
            case cRDiv: // similar to above
            case cSub: // converted into cAdd ~x
            case cRSub: // similar to above
            case cRad: // converted into cMul x CONSTANT_RD
            case cDeg: // converted into cMul x CONSTANT_DR
            case cSqr: // converted into cMul x x
            case cExp: // converted into cPow CONSTANT_E x
            case cSqrt: // converted into cPow x 0.5
            case cRSqrt: // converted into cPow x -0.5
            case cCot: // converted into cMul ~(cTan x)
            case cSec: // converted into cMul ~(cCos x)
            case cCsc: // converted into cMul ~(cSin x)
            case cLog2: // converted into cMul CONSTANT_L2I (cLog x)
            case cLog10: // converted into cMul CONSTANT_L10I (cLog x)
                break; /* Should never occur */

            /* Opcodes that do not occur in the tree for other reasons */
            case cDup:
            case cFetch:
            case cPopNMov:
            case cNop:
            case cJump:
            case VarBegin:
                break; /* Should never occur */

            /* Opcodes that are completely unpredictable */
            case cVar:
            case cPCall:
            case cFCall:
#         ifndef FP_DISABLE_EVAL
            case cEval:
#endif
                break; // Cannot deduce


            //default:
                break;
        }
        return MinMaxTree(); /* Cannot deduce */
    }

    /* Indicates whether we know whether the result is positive or negative. */
    /* Note: zero is assumed positive */
    CodeTree::TriTruthValue CodeTree::GetPositivityInfo() const
    {
        switch( (OPCODE) Opcode)
        {
            case cImmed:
                return (Value >= 0.0) ? IsAlways : IsNever;
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
                return IsAlways; /* 0 and 1 are both positive */
            case cAbs:
                /* cAbs always produces a positive value */
                return IsAlways;

            /* cLog produces positive values if the input is >= 1, negative if < 1
             * could include it here if we had a boundary mechanism...
             */

            case cAcos:
                return IsAlways; /* 0..pi */
#         ifndef FP_NO_ASINH
            case cAcosh:
                return IsAlways; /* 0..infinity */
#         endif
            case cCosh:
                return IsAlways; /* positive */

            case cCeil:
            case cFloor:
            case cInt:
            case cSinh:
            case cTanh:
#         ifndef FP_NO_ASINH
            case cAtanh:
#endif
                /* For these unary functions, the return value
                 * happens to be exactly as positive as the input value
                 */
                return Params[0].param->GetPositivityInfo();
            case cMin:
            {
                /* IsNever  if one of the values is also IsNever */
                /* IsAlways if all values are IsAlways */
                /* Otherwise unknown */
                bool all_are_positive = true;
                for(size_t a=0; a<Params.size(); ++a)
                    switch(Params[a].param->GetPositivityInfo())
                    {
                        case IsNever:
                            return IsNever;
                        case IsAlways:
                            break;
                        default:
                            all_are_positive = false;
                    }
                return all_are_positive ? IsAlways : Unknown;
            }
            case cMax:
            {
                /* IsNever  if all values are IsNever */
                /* IsAlways if one of the values is also IsAlways */
                /* Otherwise unknown */
                bool all_are_negative = true;
                for(size_t a=0; a<Params.size(); ++a)
                    switch(Params[a].param->GetPositivityInfo())
                    {
                        case IsAlways:
                            return IsAlways;
                        case IsNever:
                            break;
                        default:
                            all_are_negative = false;
                    }
                return all_are_negative ? IsNever : Unknown;
            }
            case cPow:
            {
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
                switch(Params[0].param->GetPositivityInfo())
                {
                    case IsAlways:
                        // e.g.   5^x = positive.
                        return IsAlways;
                    case IsNever:
                        return Params[1].param->GetEvennessInfo();
                        // ^Simplifies the code below, same meaning
                        /* switch(Params[1].param->GetEvennessInfo())
                        {
                            case IsAlways:
                                // e.g. (-5)^( 4) = positive
                                // e.g. (-5)^(-4) = positive
                                return IsAlways;
                            case IsNever:
                                // e.g. (-5)^( 3) = negative
                                // e.g. (-5)^(-3) = negative
                                return IsNever;
                        } */
                    default:
                        switch(Params[1].param->GetEvennessInfo())
                        {
                            case IsAlways:
                                // e.g. x^( 4) = positive
                                // e.g. x^(-4) = positive
                                return IsAlways;
                            default:
                                break;
                        }
                }
                break;
            }
            case cMul:
            {
                /* If there are Unknowns, return value is Unknown */
                /* Otherwise the return value is the xor of number of IsNevers */
                /* ~-bits (which indicate dividing operations) are irrelevant. */
                /* Note: This also deals with the following opcodes:
                 *       cInv, cDiv, cRDiv, cRad, cDeg, cSqr
                 *       cCot, Sec, cCsc, cLog2, cLog10
                 */
                bool decidedly_positive = true;
                for(size_t a=0; a<Params.size(); ++a)
                    switch(Params[a].param->GetPositivityInfo())
                    {
                        case Unknown:
                            return Unknown;
                        case IsNever:
                            decidedly_positive = !decidedly_positive;
                            break;
                        default:
                            break;
                    }
                return decidedly_positive ? IsAlways : IsNever;
            }
            case cAdd:
            {
                /* It's complicated. Follow the logic below. */
                /* Note: This also deals with the following opcodes:
                 *       cNeg, cSub, cRSub
                 */
                unsigned num_negatives = 0, num_positives = 0;
                for(size_t a=0; a<Params.size(); ++a)
                {
                    const Param& p = Params[a];
                    if(p.sign) // subtraction
                        switch(p.param->GetPositivityInfo())
                        {
                            case IsAlways:
                                ++num_negatives; // -positive = negative
                                break;
                            case IsNever:
                                ++num_positives; // -negative = positive
                                break;
                            default:
                                return Unknown; // could be anything
                        }
                    else // addition
                        switch(p.param->GetPositivityInfo())
                        {
                            case IsAlways:
                                ++num_positives;
                                break;
                            case IsNever:
                                ++num_negatives;
                                break;
                            default:
                                return Unknown; // could be anything
                        }
                    /* If both positives and negatives are present,
                     * the result can be either.
                     */
                    if(num_positives && num_negatives)
                        return Unknown; // could by anything
                }
                return num_negatives ? IsNever : IsAlways;
            }

            /* The following opcodes are processed by GenerateFrom()
             * within fpoptimizer_bytecode_to_codetree.cc and thus
             * they will never occur in the calling context:
             */
            case cNeg: // converted into cAdd ~x
            case cInv: // converted into cMul ~x
            case cDiv: // converted into cMul ~x
            case cRDiv: // similar to above
            case cSub: // converted into cAdd ~x
            case cRSub: // similar to above
            case cRad: // converted into cMul x CONSTANT_RD
            case cDeg: // converted into cMul x CONSTANT_DR
            case cSqr: // converted into cMul x x
            case cExp: // converted into cPow CONSTANT_E x
            case cSqrt: // converted into cPow x 0.5
            case cRSqrt: // converted into cPow x -0.5
            case cCot: // converted into cMul ~(cTan x)
            case cSec: // converted into cMul ~(cCos x)
            case cCsc: // converted into cMul ~(cSin x)
            case cLog2: // converted into cMul CONSTANT_L2I (cLog x)
            case cLog10: // converted into cMul CONSTANT_L10I (cLog x)
                break; /* Should never occur */

            default:
                break;
        }
        return Unknown; /* Cannot deduce */
    }

    /* Is the value of this tree definitely odd(true) or even(false)? */
    CodeTree::TriTruthValue CodeTree::GetEvennessInfo() const
    {
        if(!IsImmed()) return Unknown;
        if(!IsLongIntegerImmed()) return Unknown;
        return (GetLongIntegerImmed() & 1) ? IsNever : IsAlways;
    }

    bool CodeTree::IsAlwaysInteger() const
    {
        switch( (OPCODE) Opcode)
        {
            case cImmed:
                return IsLongIntegerImmed();
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
                return true; /* 0 and 1 are both integers */
            default:
                break;
        }
        return false; /* Don't know whether it's integer. */
    }
}

#endif
