#include "fpoptimizer_codetree.hh"
#include "fpoptimizer_consts.hh"

#include <cmath> /* for CalculateResultBoundaries() */

#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#ifdef FP_SUPPORT_OPTIMIZER

using namespace FUNCTIONPARSERTYPES;

//#define DEBUG_SUBSTITUTIONS

#ifdef DEBUG_SUBSTITUTIONS
namespace FPoptimizer_Grammar
{
    void DumpTree(const FPoptimizer_CodeTree::CodeTree& tree, std::ostream& o = std::cout);
}
#endif

namespace FPoptimizer_CodeTree
{
    void CodeTree::ConstantFolding()
    {
        using namespace std;

        // Insert here any hardcoded constant-folding optimizations
        // that you want to be done whenever a new subtree is generated.
        /* Not recursive. */

        double const_value = 1.0;
        size_t which_param = 0;

        if(Opcode != cImmed)
        {
            MinMaxTree p = CalculateResultBoundaries();
            if(p.has_min && p.has_max && p.min == p.max)
            {
                // Replace us with this immed
                const_value = p.min;
                goto ReplaceTreeWithConstValue;
            }
        }


        /* Sub-list assimilation prepass */
        switch( (OPCODE) Opcode)
        {
            case cAdd:
            case cMul:
            case cMin:
            case cMax:
            {
                /* If the list contains another list of the same kind, assimilate it */
                for(size_t a=Params.size(); a-- > 0; )
                    if(Params[a].param->Opcode == Opcode)
                    {
                        // Assimilate its children and remove it
                        CodeTreeP tree = Params[a].param;

                        Params.erase(Params.begin()+a);
                        for(size_t b=0; b<tree->Params.size(); ++b)
                            AddParam( Param(tree->Params[b].param) );
                    }
                break;
            }
            case cAnd:
            case cOr:
            {
                /* If the list contains another list of the same kind, assimilate it */
                for(size_t a=Params.size(); a-- > 0; )
                    if(Params[a].param->Opcode == Opcode)
                    {
                        // Assimilate its children and remove it
                        CodeTreeP tree = Params[a].param;
                        Params.erase(Params.begin()+a);
                        for(size_t b=0; b<tree->Params.size(); ++b)
                            AddParam(tree->Params[b]);
                    }
                break;
            }
            default: break;
        }

        /* Constant folding */
        switch( (OPCODE) Opcode)
        {
            case cImmed:
                break; // nothing to do
            case cVar:
                break; // nothing to do

            ReplaceTreeWithOne:
                const_value = 1.0;
                goto ReplaceTreeWithConstValue;
            ReplaceTreeWithZero:
                const_value = 0.0;
            ReplaceTreeWithConstValue:
              #ifdef DEBUG_SUBSTITUTIONS
                std::cout << "Replacing "; FPoptimizer_Grammar::DumpTree(*this);
                std::cout << " with const value " << const_value << "\n";
              #endif
                Params.clear();
                Opcode = cImmed;
                Value  = const_value;
                break;
            ReplaceTreeWithParam0:
                which_param = 0;
            ReplaceTreeWithParam:
              #ifdef DEBUG_SUBSTITUTIONS
                std::cout << "Before replace: "; FPoptimizer_Grammar::DumpTree(*this);
                std::cout << "\n";
              #endif
                Become(*Params[which_param].param, true, false);
              #ifdef DEBUG_SUBSTITUTIONS
                std::cout << "After replace: "; FPoptimizer_Grammar::DumpTree(*this);
                std::cout << "\n";
              #endif
                break;

            case cAnd:
            {
                // If the and-list contains an expression that evaluates to approx. zero,
                // the whole list evaluates to zero.
                // If all expressions within the and-list evaluate to approx. nonzero,
                // the whole list evaluates to one.
                bool all_values_are_nonzero = true;
                for(size_t a=0; a<Params.size(); ++a)
                {
                    MinMaxTree p = Params[a].param->CalculateResultBoundaries();
                    if(p.has_min && p.has_max
                    && p.min > -0.5 && p.max < 0.5) // -0.5 < x < 0.5 = zero
                    {
                        goto ReplaceTreeWithZero;
                    }
                    else if( (p.has_max && p.max <= -0.5)
                          || (p.has_min && p.min >= 0.5)) // |x| >= 0.5  = nonzero
                    {
                    }
                    else
                        all_values_are_nonzero = false;
                }
                if(all_values_are_nonzero) goto ReplaceTreeWithOne;
                if(Params.size() == 1)
                {
                    // Replace self with the single operand
                    Opcode = cNotNot;
                }
                if(Params.empty()) goto ReplaceTreeWithZero;
                break;
            }
            case cOr:
            {
                // If the or-list contains an expression that evaluates to approx. nonzero,
                // the whole list evaluates to one.
                // If all expressions within the and-list evaluate to approx. zero,
                // the whole list evaluates to zero.
                bool all_values_are_zero = true;
                for(size_t a=0; a<Params.size(); ++a)
                {
                    MinMaxTree p = Params[a].param->CalculateResultBoundaries();
                    if(p.has_min && p.has_max
                    && p.min > -0.5 && p.max < 0.5) // -0.5 < x < 0.5 = zero
                    {
                    }
                    else if( (p.has_max && p.max <= -0.5)
                          || (p.has_min && p.min >= 0.5)) // |x| >= 0.5  = nonzero
                    {
                        goto ReplaceTreeWithOne;
                    }
                    else
                        all_values_are_zero = false;
                }
                if(all_values_are_zero) goto ReplaceTreeWithZero;
                if(Params.size() == 1)
                {
                    // Replace self with the single operand
                    Opcode = cNotNot;
                }
                if(Params.empty()) goto ReplaceTreeWithOne;
                break;
            }
            case cNot:
            {
                // If the sub-expression evaluates to approx. zero, yield one.
                // If the sub-expression evaluates to approx. nonzero, yield zero.
                MinMaxTree p = Params[0].param->CalculateResultBoundaries();
                if(p.has_min && p.has_max
                && p.min > -0.5 && p.max < 0.5) // -0.5 < x < 0.5 = zero
                {
                    goto ReplaceTreeWithOne;
                }
                else if( (p.has_max && p.max <= -0.5)
                      || (p.has_min && p.min >= 0.5)) // |x| >= 0.5  = nonzero
                    goto ReplaceTreeWithZero;
                break;
            }
            case cNotNot:
            {
                // If the sub-expression evaluates to approx. zero, yield zero.
                // If the sub-expression evaluates to approx. nonzero, yield one.
                MinMaxTree p = Params[0].param->CalculateResultBoundaries();
                if(p.has_min && p.has_max
                && p.min > -0.5 && p.max < 0.5) // -0.5 < x < 0.5 = zero
                {
                    goto ReplaceTreeWithZero;
                }
                else if( (p.has_max && p.max <= -0.5)
                      || (p.has_min && p.min >= 0.5)) // |x| >= 0.5  = nonzero
                    goto ReplaceTreeWithOne;
                break;
            }
            case cIf:
            {
                // If the sub-expression evaluates to approx. zero, yield param3.
                // If the sub-expression evaluates to approx. nonzero, yield param2.
                MinMaxTree p = Params[0].param->CalculateResultBoundaries();
                if(p.has_min && p.has_max
                && p.min > -0.5 && p.max < 0.5) // -0.5 < x < 0.5 = zero
                {
                    which_param = 2;
                    goto ReplaceTreeWithParam;
                }
                else if( (p.has_max && p.max <= -0.5)
                      || (p.has_min && p.min >= 0.5)) // |x| >= 0.5  = nonzero
                {
                    which_param = 1;
                    goto ReplaceTreeWithParam;
                }
                break;
            }
            case cMul:
            {
            NowWeAreMulGroup: ;
                // If one sub-expression evalutes to exact zero, yield zero.
                double mul_immed_sum = 1.0;
                size_t n_mul_immeds = 0; bool needs_resynth=false;
                for(size_t a=0; a<Params.size(); ++a)
                {
                    if(!Params[a].param->IsImmed()) continue;
                    // ^ Only check constant values
                    double immed = Params[a].param->GetImmed();
                    if(FloatEqual(immed, 0.0)) goto ReplaceTreeWithZero;
                    if(FloatEqual(immed, 1.0)) needs_resynth = true;
                    mul_immed_sum *= immed; ++n_mul_immeds;
                }
                // Merge immeds.
                if(n_mul_immeds > 1) needs_resynth = true;
                if(needs_resynth)
                {
                    // delete immeds and add new ones
                    //std::cout << "cMul: Will add new immed " << mul_immed_sum << "\n";
                    for(size_t a=Params.size(); a-->0; )
                        if(Params[a].param->IsImmed())
                        {
                            //std::cout << " - For that, deleting immed " << Params[a].param->GetImmed();
                            //std::cout << "\n";
                            Params.erase(Params.begin()+a);
                        }
                    if(!FloatEqual(mul_immed_sum, 1.0))
                        AddParam( Param(new CodeTree(mul_immed_sum) ) );
                }
                if(Params.size() == 1)
                {
                    // Replace self with the single operand
                    goto ReplaceTreeWithParam0;
                }
                if(Params.empty()) goto ReplaceTreeWithOne;
                break;
            }
            case cAdd:
            {
                double immed_sum = 0.0;
                size_t n_immeds = 0; bool needs_resynth=false;
                for(size_t a=0; a<Params.size(); ++a)
                {
                    if(!Params[a].param->IsImmed()) continue;
                    // ^ Only check constant values
                    double immed = Params[a].param->GetImmed();
                    if(FloatEqual(immed, 0.0)) needs_resynth = true;
                    immed_sum += immed; ++n_immeds;
                }
                // Merge immeds.
                if(n_immeds > 1) needs_resynth = true;
                if(needs_resynth)
                {
                    // delete immeds and add new ones
                    //std::cout << "cAdd: Will add new immed " << immed_sum << "\n";
                    //std::cout << "In: "; FPoptimizer_Grammar::DumpTree(*this);
                    //std::cout << "\n";

                    for(size_t a=Params.size(); a-->0; )
                        if(Params[a].param->IsImmed())
                        {
                            //std::cout << " - For that, deleting immed " << Params[a].param->GetImmed();
                            //std::cout << "\n";
                            Params.erase(Params.begin()+a);
                        }
                    if(!FloatEqual(immed_sum, 0.0))
                        AddParam( Param(new CodeTree(immed_sum)) );
                }
                if(Params.size() == 1)
                {
                    // Replace self with the single operand
                    goto ReplaceTreeWithParam0;
                }
                if(Params.empty()) goto ReplaceTreeWithZero;
                break;
            }
            case cMin:
            {
                /* Goal: If there is any pair of two operands, where
                 * their ranges form a disconnected set, i.e. as below:
                 *     xxxxx
                 *            yyyyyy
                 * Then remove the larger one.
                 *
                 * Algorithm: 1. figure out the smallest maximum of all operands.
                 *            2. eliminate all operands where their minimum is
                 *               larger than the selected maximum.
                 */
                MinMaxTree smallest_maximum;
                for(size_t a=0; a<Params.size(); ++a)
                {
                    MinMaxTree p = Params[a].param->CalculateResultBoundaries();
                    if(p.has_max && (!smallest_maximum.has_max || p.max < smallest_maximum.max))
                    {
                        smallest_maximum.max = p.max;
                        smallest_maximum.has_max = true;
                }   }
                if(smallest_maximum.has_max)
                    for(size_t a=Params.size(); a-- > 0; )
                    {
                        MinMaxTree p = Params[a].param->CalculateResultBoundaries();
                        if(p.has_min && p.min > smallest_maximum.max)
                            Params.erase(Params.begin() + a);
                    }
                //fprintf(stderr, "Remains: %u\n", (unsigned)Params.size());
                if(Params.size() == 1)
                {
                    // Replace self with the single operand
                    goto ReplaceTreeWithParam0;
                }
                break;
            }
            case cMax:
            {
                /* Goal: If there is any pair of two operands, where
                 * their ranges form a disconnected set, i.e. as below:
                 *     xxxxx
                 *            yyyyyy
                 * Then remove the smaller one.
                 *
                 * Algorithm: 1. figure out the biggest minimum of all operands.
                 *            2. eliminate all operands where their maximum is
                 *               smaller than the selected minimum.
                 */
                MinMaxTree biggest_minimum;
                for(size_t a=0; a<Params.size(); ++a)
                {
                    MinMaxTree p = Params[a].param->CalculateResultBoundaries();
                    if(p.has_min && (!biggest_minimum.has_min || p.min > biggest_minimum.min))
                    {
                        biggest_minimum.min = p.min;
                        biggest_minimum.has_min = true;
                }   }
                if(biggest_minimum.has_min)
                {
                    //fprintf(stderr, "Removing all where max < %g\n", biggest_minimum.min);
                    for(size_t a=Params.size(); a-- > 0; )
                    {
                        MinMaxTree p = Params[a].param->CalculateResultBoundaries();
                        if(p.has_max && p.max < biggest_minimum.min)
                        {
                            //fprintf(stderr, "Removing %g\n", p.max);
                            Params.erase(Params.begin() + a);
                        }
                    }
                }
                //fprintf(stderr, "Remains: %u\n", (unsigned)Params.size());
                if(Params.size() == 1)
                {
                    // Replace self with the single operand
                    goto ReplaceTreeWithParam0;
                }
                break;
            }

            case cEqual:
            {
                /* If we know the two operands' ranges don't overlap, we get zero.
                 * The opposite is more complex and is done in .dat code.
                 */
                MinMaxTree p0 = Params[0].param->CalculateResultBoundaries();
                MinMaxTree p1 = Params[1].param->CalculateResultBoundaries();
                if((p0.has_max && p1.has_min && p1.min > p0.max)
                || (p1.has_max && p0.has_min && p0.min > p1.max))
                    goto ReplaceTreeWithZero;
                break;
            }

            case cNEqual:
            {
                /* If we know the two operands' ranges don't overlap, we get one.
                 * The opposite is more complex and is done in .dat code.
                 */
                MinMaxTree p0 = Params[0].param->CalculateResultBoundaries();
                MinMaxTree p1 = Params[1].param->CalculateResultBoundaries();
                if((p0.has_max && p1.has_min && p1.min > p0.max)
                || (p1.has_max && p0.has_min && p0.min > p1.max))
                    goto ReplaceTreeWithOne;
                break;
            }

            case cLess:
            {
                MinMaxTree p0 = Params[0].param->CalculateResultBoundaries();
                MinMaxTree p1 = Params[1].param->CalculateResultBoundaries();
                if(p0.has_max && p1.has_min && p0.max < p1.min)
                    goto ReplaceTreeWithOne; // We know p0 < p1
                if(p1.has_max && p0.has_min && p1.max <= p0.min)
                    goto ReplaceTreeWithZero; // We know p1 >= p0
                break;
            }

            case cLessOrEq:
            {
                MinMaxTree p0 = Params[0].param->CalculateResultBoundaries();
                MinMaxTree p1 = Params[1].param->CalculateResultBoundaries();
                if(p0.has_max && p1.has_min && p0.max <= p1.min)
                    goto ReplaceTreeWithOne; // We know p0 <= p1
                if(p1.has_max && p0.has_min && p1.max < p0.min)
                    goto ReplaceTreeWithZero; // We know p1 > p0
                break;
            }

            case cGreater:
            {
                // Note: Eq case not handled
                MinMaxTree p0 = Params[0].param->CalculateResultBoundaries();
                MinMaxTree p1 = Params[1].param->CalculateResultBoundaries();
                if(p0.has_max && p1.has_min && p0.max <= p1.min)
                    goto ReplaceTreeWithZero; // We know p0 <= p1
                if(p1.has_max && p0.has_min && p1.max < p0.min)
                    goto ReplaceTreeWithOne; // We know p1 > p0
                break;
            }

            case cGreaterOrEq:
            {
                // Note: Eq case not handled
                MinMaxTree p0 = Params[0].param->CalculateResultBoundaries();
                MinMaxTree p1 = Params[1].param->CalculateResultBoundaries();
                if(p0.has_max && p1.has_min && p0.max < p1.min)
                    goto ReplaceTreeWithZero; // We know p0 < p1
                if(p1.has_max && p0.has_min && p1.max <= p0.min)
                    goto ReplaceTreeWithOne; // We know p1 >= p0
                break;
            }

            case cAbs:
            {
                /* If we know the operand is always positive, cAbs is redundant.
                 * If we know the operand is always negative, use actual negation.
                 */
                MinMaxTree p0 = Params[0].param->CalculateResultBoundaries();
                if(p0.has_min && p0.min >= 0.0)
                    goto ReplaceTreeWithParam0;
                if(p0.has_max && p0.max <= NEGATIVE_MAXIMUM)
                {
                    /* abs(negative) = negative*-1 */
                    Opcode = cMul;
                    AddParam( Param(new CodeTree(-1.0)) );
                    /* The caller of ConstantFolding() will do Sort() and Rehash() next.
                     * Thus, no need to do it here. */
                    /* We were changed into a cMul group. Do cMul folding. */
                    goto NowWeAreMulGroup;
                }
                /* If the operand is a cMul group, find elements
                 * that are always positive and always negative,
                 * and move them out, e.g. abs(p*n*x*y) = p*(-n)*abs(x*y)
                 */
                if(Params[0].param->Opcode == cMul)
                {
                    CodeTree& p = *Params[0].param;
                    std::vector<Param> pos_set;
                    std::vector<Param> neg_set;
                    for(size_t a=0; a<p.Params.size(); ++a)
                    {
                        p0 = p.Params[a].param->CalculateResultBoundaries();
                        if(p0.has_min && p0.min >= 0.0)
                            { pos_set.push_back(p.Params[a]); }
                        if(p0.has_max && p0.max <= NEGATIVE_MAXIMUM)
                            { neg_set.push_back(p.Params[a]); }
                    }
                #ifdef DEBUG_SUBSTITUTIONS
                    std::cout << "Abs: mul group has " << pos_set.size()
                              << " pos, " << neg_set.size() << "neg\n";
                #endif
                    if(!pos_set.empty() || !neg_set.empty())
                    {
                #ifdef DEBUG_SUBSTITUTIONS
                        std::cout << "AbsReplace-Before: ";
                        FPoptimizer_Grammar::DumpTree(*this);
                        std::cout << "\n" << std::flush;
                        FPoptimizer_Grammar::DumpHashes(*this);
                #endif
                        for(size_t a=p.Params.size(); a-- > 0; )
                        {
                            p0 = p.Params[a].param->CalculateResultBoundaries();
                            if((p0.has_min && p0.min >= 0.0)
                            || (p0.has_max && p0.max <= NEGATIVE_MAXIMUM))
                                p.Params.erase(p.Params.begin() + a);

                            /* Here, p*n*x*y -> x*y.
                             * p is saved in pos_set[]
                             * n is saved in neg_set[]
                             */
                        }
                        p.ConstantFolding();
                        p.Sort();

                        CodeTreeP subtree = new CodeTree;
                        p.Parent = &*subtree;
                        subtree->Opcode = cAbs;
                        subtree->Params.swap(Params);
                        subtree->ConstantFolding();
                        subtree->Sort();
                        subtree->Rehash(false); // hash it and its children.

                        /* Now:
                         * subtree = Abs(x*y)
                         * this    = Abs()
                         */

                        Opcode = cMul;
                        for(size_t a=0; a<pos_set.size(); ++a)
                            AddParam(pos_set[a]);
                        AddParam(Param(subtree));
                        /* Now:
                         * this    = p * Abs(x*y)
                         */
                        if(!neg_set.empty())
                        {
                            for(size_t a=0; a<neg_set.size(); ++a)
                                AddParam(neg_set[a]);
                            AddParam( Param(new CodeTree(-1.0)) );
                            /* Now:
                             * this = p * n * -1 * Abs(x*y)
                             */
                        }
                #ifdef DEBUG_SUBSTITUTIONS
                        std::cout << "AbsReplace-After: ";
                        FPoptimizer_Grammar::DumpTree(*this);
                        std::cout << "\n" << std::flush;
                        FPoptimizer_Grammar::DumpHashes(*this);
                #endif
                        /* We were changed into a cMul group. Do cMul folding. */
                        goto NowWeAreMulGroup;
                    }
                }
                break;
            }

            #define HANDLE_UNARY_CONST_FUNC(funcname) \
                if(Params[0].param->IsImmed()) \
                    { const_value = funcname(Params[0].param->GetImmed()); \
                      goto ReplaceTreeWithConstValue; }

            case cLog:   HANDLE_UNARY_CONST_FUNC(log); break;
            case cAcosh: HANDLE_UNARY_CONST_FUNC(fp_acosh); break;
            case cAsinh: HANDLE_UNARY_CONST_FUNC(fp_asinh); break;
            case cAtanh: HANDLE_UNARY_CONST_FUNC(fp_atanh); break;
            case cAcos: HANDLE_UNARY_CONST_FUNC(acos); break;
            case cAsin: HANDLE_UNARY_CONST_FUNC(asin); break;
            case cAtan: HANDLE_UNARY_CONST_FUNC(atan); break;
            case cCosh: HANDLE_UNARY_CONST_FUNC(cosh); break;
            case cSinh: HANDLE_UNARY_CONST_FUNC(sinh); break;
            case cTanh: HANDLE_UNARY_CONST_FUNC(tanh); break;
            case cSin: HANDLE_UNARY_CONST_FUNC(sin); break;
            case cCos: HANDLE_UNARY_CONST_FUNC(cos); break;
            case cTan: HANDLE_UNARY_CONST_FUNC(tan); break;
            case cCeil: HANDLE_UNARY_CONST_FUNC(ceil); break;
            case cFloor: HANDLE_UNARY_CONST_FUNC(floor); break;
            case cInt:
                if(Params[0].param->IsImmed())
                    { const_value = floor(Params[0].param->GetImmed() + 0.5);
                      goto ReplaceTreeWithConstValue; }
                break;
            case cLog2:
                if(Params[0].param->IsImmed())
                    { const_value = log(Params[0].param->GetImmed()) * CONSTANT_L2I;
                      goto ReplaceTreeWithConstValue; }
                break;
            case cLog10:
                if(Params[0].param->IsImmed())
                    { const_value = log(Params[0].param->GetImmed()) * CONSTANT_L10I;
                      goto ReplaceTreeWithConstValue; }
                break;

            case cAtan2:
            {
                /* Range based optimizations for (y,x):
                 * If y is +0 and x <= -0, +pi is returned
                 * If y is -0 and x <= -0, -pi is returned (assumed never happening)
                 * If y is +0 and x >= +0, +0 is returned
                 * If y is -0 and x >= +0, -0 is returned  (assumed never happening)
                 * If x is +-0 and y < 0, -pi/2 is returned
                 * If x is +-0 and y > 0, +pi/2 is returned
                 * Otherwise, perform constant folding when available
                 * If we know x <> 0, convert into atan(y / x)
                 *   TODO: Figure out whether the above step is wise
                 *         It allows e.g. atan2(6*x, 3*y) -> atan(2*x/y)
                 *         when we know y != 0
                 */
                MinMaxTree p0 = Params[0].param->CalculateResultBoundaries();
                MinMaxTree p1 = Params[1].param->CalculateResultBoundaries();
                if(p0.has_min && p0.has_max && p0.min == 0.0)
                {
                    if(p1.has_max && p1.max < 0)
                        { const_value = CONSTANT_PI; goto ReplaceTreeWithConstValue; }
                    if(p1.has_max && p1.max >= 0.0)
                        { const_value = p0.min; goto ReplaceTreeWithConstValue; }
                }
                if(p1.has_min && p1.has_max && p1.min == 0.0)
                {
                    if(p0.has_max && p0.max < 0)
                        { const_value = -CONSTANT_PIHALF; goto ReplaceTreeWithConstValue; }
                    if(p0.has_min && p0.min > 0)
                        { const_value =  CONSTANT_PIHALF; goto ReplaceTreeWithConstValue; }
                }
                if(Params[0].param->IsImmed()
                && Params[1].param->IsImmed())
                    { const_value = atan2(Params[0].param->GetImmed(),
                                          Params[1].param->GetImmed());
                      goto ReplaceTreeWithConstValue; }
              #if 0
                if((p1.has_min && p1.min > 0.0)
                || (p1.has_max && p1.max < NEGATIVE_MAXIMUM))
                {
                    // Convert into a division
                    CodeTreeP subtree = new CodeTree;
                    Params[1].sign = true; /* FIXME: Not appropriate anymore */
                    for(size_t a=0; a<Params.size(); ++a)
                        Params[a].param->Parent = &*subtree;
                    subtree->Opcode = cMul;
                    subtree->Params.swap(Params); // subtree = y/x
                    subtree->ConstantFolding();
                    subtree->Sort();
                    subtree->Rehash(false);
                    Opcode = cAtan;
                    AddParam(Param(subtree)); // we = atan(y/x)
                }
              #endif
                break;
            }

            case cPow:
            {
                if(Params[0].param->IsImmed()
                && Params[1].param->IsImmed())
                    { const_value = pow(Params[0].param->GetImmed(),
                                        Params[1].param->GetImmed());
                      goto ReplaceTreeWithConstValue; }
                if(Params[1].param->IsImmed()
                && Params[1].param->GetImmed() == 1.0)
                {
                    // x^1 = x
                    goto ReplaceTreeWithParam0;
                }
                if(Params[0].param->IsImmed()
                && Params[0].param->GetImmed() == 1.0)
                {
                    // 1^x = 1
                    goto ReplaceTreeWithOne;
                }
                break;
            }

            case cMod:
            {
                /* Can more be done than this? */
                if(Params[0].param->IsImmed()
                && Params[1].param->IsImmed())
                    { const_value = fmod(Params[0].param->GetImmed(),
                                         Params[1].param->GetImmed());
                      goto ReplaceTreeWithConstValue; }
                break;
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
            case cExp2: // converted into cPow 2.0 x
            case cSqrt: // converted into cPow x 0.5
            case cRSqrt: // converted into cPow x -0.5
            case cCot: // converted into cMul (cPow (cTan x) -1)
            case cSec: // converted into cMul (cPow (cCos x) -1)
            case cCsc: // converted into cMul (cPow (cSin x) -1)
                break; /* Should never occur */

            /* Opcodes that do not occur in the tree for other reasons */
            case cDup:
            case cFetch:
            case cPopNMov:
            case cNop:
            case cJump:
            case VarBegin:
                break; /* Should never occur */
            /* Opcodes that we can't do anything about */
            case cPCall:
            case cFCall:
            case cEval:
                break;
        }
        /*
        if(Parent)
            Parent->ConstantFolding();

        */
    }

    CodeTree::MinMaxTree CodeTree::CalculateResultBoundaries() const
#ifdef DEBUG_SUBSTITUTIONS
    {
        MinMaxTree tmp = CalculateResultBoundaries_do();
        std::cout << std::flush;
        fprintf(stderr, "Estimated boundaries: %g%s .. %g%s: ",
            tmp.min, tmp.has_min?"":"(unknown)",
            tmp.max, tmp.has_max?"":"(unknown)");
        fflush(stderr);
        FPoptimizer_Grammar::DumpTree(*this);
        std::cout << std::flush;
        fprintf(stderr, " \n");
        fflush(stderr);
        return tmp;
    }
    CodeTree::MinMaxTree CodeTree::CalculateResultBoundaries_do() const
#endif
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
                MinMaxTree m = Params[0].param->CalculateResultBoundaries();
                if(m.has_min) { if(m.min < 0.0) m.has_min = false; else m.min = log(m.min); } // No boundaries
                if(m.has_max) { if(m.max < 0.0) m.has_max = false; else m.max = log(m.max); }
                return m;
            }

            case cLog2: /* Defined for 0.0 < x <= inf */
            {
                MinMaxTree m = Params[0].param->CalculateResultBoundaries();
                if(m.has_min) { if(m.min < 0.0) m.has_min = false; else m.min = log(m.min)*CONSTANT_L2I; } // No boundaries
                if(m.has_max) { if(m.max < 0.0) m.has_max = false; else m.max = log(m.max)*CONSTANT_L2I; }
                return m;
            }

            case cAcosh: /* defined for             1.0 <  x <= inf */
            {
                MinMaxTree m = Params[0].param->CalculateResultBoundaries();
                if(m.has_min) { if(m.min <= 1.0) m.has_min = false; else m.min = fp_acosh(m.min); } // No boundaries
                if(m.has_max) { if(m.max <= 1.0) m.has_max = false; else m.max = fp_acosh(m.max); }
                return m;
            }
            case cAsinh: /* defined for all values -inf <= x <= inf */
            {
                MinMaxTree m = Params[0].param->CalculateResultBoundaries();
                if(m.has_min) m.min = fp_asinh(m.min); // No boundaries
                if(m.has_max) m.max = fp_asinh(m.max);
                return m;
            }
            case cAtanh: /* defined for all values -inf <= x <= inf */
            {
                MinMaxTree m = Params[0].param->CalculateResultBoundaries();
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
                MinMaxTree m = Params[0].param->CalculateResultBoundaries();
                if(m.has_min) m.min = atan(m.min); else { m.min = -CONSTANT_PIHALF; m.has_min = true; }
                if(m.has_max) m.max = atan(m.max); else { m.max =  CONSTANT_PIHALF; m.has_max = true; }
                return m;
            }
            case cAtan2: /* too complicated to estimate */
            {
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
            {
                // No guess which branch is chosen. Produce a spanning min & max.
                MinMaxTree res1 = Params[1].param->CalculateResultBoundaries();
                MinMaxTree res2 = Params[2].param->CalculateResultBoundaries();
                if(!res2.has_min) res1.has_min = false; else if(res2.min < res1.min) res1.min = res2.min;
                if(!res2.has_max) res1.has_max = false; else if(res2.max > res1.max) res1.max = res2.max;
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
                /* It's complicated. Follow the logic below. */
                /* Note: This also deals with the following opcodes:
                 *       cInv, cDiv, cRDiv, cRad, cDeg, cSqr
                 *       cCot, Sec, cCsc, cLog2, cLog10
                 */

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
                for(size_t a=0; a<Params.size(); ++a)
                {
                    const Param& p = Params[a];
                    MinMaxTree item = p.param->CalculateResultBoundaries();
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
                            return MinMaxTree(y.max, NEGATIVE_MAXIMUM);
                    }
                }
                else
                    return MinMaxTree();
            }
            case cPow:
            {
                if(Params[1].param->IsImmed() && FloatEqual(Params[1].param->GetImmed(), 0.0))
                {
                    // Note: This makes 0^0 evaluate into 1.
                    return MinMaxTree(1.0, 1.0); // x^0 = 1
                }
                if(Params[0].param->IsImmed() && FloatEqual(Params[0].param->GetImmed(), 0.0))
                {
                    // Note: This makes 0^0 evaluate into 0.
                    return MinMaxTree(0.0, 0.0); // 0^x = 0
                }
                if(Params[0].param->IsImmed() && FloatEqual(Params[0].param->GetImmed(), 1.0))
                {
                    return MinMaxTree(1.0, 1.0); // 1^x = 1
                }

                MinMaxTree p0 = Params[0].param->CalculateResultBoundaries();
                MinMaxTree p1 = Params[1].param->CalculateResultBoundaries();
                TriTruthValue p0_positivity =
                    (p0.has_min && p0.has_max)
                        ? ( (p0.min >= 0.0 && p0.max >= 0.0) ? IsAlways
                          : (p0.min <  0.0 && p0.max <  0.0) ? IsNever
                          : Unknown)
                        : Unknown;
                TriTruthValue p1_evenness = Params[1].param->GetEvennessInfo();

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
                                if(Params[1].param->IsImmed()
                                && !Params[1].param->IsAlwaysInteger()
                                && Params[1].param->GetImmed() >= 0.0)
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
             * they will never occur in the calling context:
             */
            case cNeg: // converted into cMul x -1
            case cInv: // converted into cPow x -1
            case cDiv: // converted into cPow y -1
            case cRDiv: // similar to above
            case cSub: // converted into cMul y -1
            case cRSub: // similar to above
            case cRad: // converted into cMul x CONSTANT_RD
            case cDeg: // converted into cMul x CONSTANT_DR
            case cSqr: // converted into cMul x x
            case cExp: // converted into cPow CONSTANT_E x
            case cExp2: // converted into cPow 2 x
            case cSqrt: // converted into cPow x 0.5
            case cRSqrt: // converted into cPow x -0.5
            case cCot: // converted into cMul (cPow (cTan x) -1)
            case cSec: // converted into cMul (cPow (cCos x) -1)
            case cCsc: // converted into cMul (cPow (cSin x) -1)
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
            case cEval:
                break; // Cannot deduce


            //default:
                break;
        }
        return MinMaxTree(); /* Cannot deduce */
    }
}

#endif
