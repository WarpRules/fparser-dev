#include "fpoptimizer_grammar.hh"
#include "fpoptimizer_codetree.hh"
#include "fpoptimizer_consts.hh"
#include "fpoptimizer_opcodename.hh"

#include <stdio.h>

#include <algorithm>
#include <cmath>
#include <map>
#include <assert.h>

#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#ifdef FP_SUPPORT_OPTIMIZER

using namespace FUNCTIONPARSERTYPES;

//#define DEBUG_SUBSTITUTIONS

#ifdef DEBUG_SUBSTITUTIONS
#include <sstream>
namespace FPoptimizer_Grammar
{
    void DumpTree(const FPoptimizer_CodeTree::CodeTree& tree, std::ostream& o = std::cout);
    void DumpHashes(const FPoptimizer_CodeTree::CodeTree& tree);
}
#endif

namespace
{
    /* I have heard that std::equal_range() is practically worthless
     * due to the insane limitation that the two parameters for Comp() must
     * be of the same type. Hence we must reinvent the wheel and implement
     * our own here. This is practically identical to the one from
     * GNU libstdc++, except rewritten. -Bisqwit
     */
    template<typename It, typename T, typename Comp>
    std::pair<It, It>
    MyEqualRange(It first, It last, const T& val, Comp comp)
    {
        size_t len = last-first;
        while(len > 0)
        {
            size_t half = len/2;
            It middle(first); middle += half;
            if(comp(*middle, val))
            {
                first = middle;
                ++first;
                len = len - half - 1;
            }
            else if(comp(val, *middle))
            {
                len = half;
            }
            else
            {
                // The following implements this:
                // // left = lower_bound(first, middle, val, comp);
                It left(first);
              {///
                It& first2 = left;
                It last2(middle);
                size_t len2 = last2-first2;
                while(len2 > 0)
                {
                    size_t half2 = len2 / 2;
                    It middle2(first2); middle2 += half2;
                    if(comp(*middle2, val))
                    {
                        first2 = middle2;
                        ++first2;
                        len2 = len2 - half2 - 1;
                    }
                    else
                        len2 = half2;
                }
                // left = first2;  - not needed, already happens due to reference
              }///
                first += len;
                // The following implements this:
                // // right = upper_bound(++middle, first, val, comp);
                It right(++middle);
              {///
                It& first2 = right;
                It& last2 = first;
                size_t len2 = last2-first2;
                while(len2 > 0)
                {
                    size_t half2 = len2 / 2;
                    It middle2(first2); middle2 += half2;
                    if(comp(val, *middle2))
                        len2 = half2;
                    else
                    {
                        first2 = middle2;
                        ++first2;
                        len2 = len2 - half2 - 1;
                    }
                }
                // right = first2;  - not needed, already happens due to reference
              }///
                return std::pair<It,It> (left,right);
            }
        }
        return std::pair<It,It> (first,first);
    }
}

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
                        bool had_sign = Params[a].sign;

                        if(Opcode == cMul && had_sign)
                        {
                            int n_divisions_old = 1;
                            int n_divisions_new = 0;

                            for(size_t b=0; b<tree->Params.size(); ++b)
                                if(tree->Params[b].sign)
                                    ++n_divisions_old;
                                else
                                    ++n_divisions_new;

                            if(n_divisions_new > n_divisions_old)
                            {
                                // Don't assimilate if the number of divisions increases
                                // in the process.  x/(y*z) is better than x/y/z
                                continue;
                            }
                        }

                        Params.erase(Params.begin()+a);
                        for(size_t b=0; b<tree->Params.size(); ++b)
                            AddParam( Param(tree->Params[b].param,
                                            had_sign ^ tree->Params[b].sign) );
                    }
                break;
            }
            case cAnd:
            case cOr:
            {
                /* If the list contains another list of the same kind, assimilate it */
                for(size_t a=Params.size(); a-- > 0; )
                    if(Params[a].param->Opcode == Opcode
                    && Params[a].sign == false)
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
                Opcode = Params[which_param].param->Opcode;
                Var    = Params[which_param].param->Var;
                Value  = Params[which_param].param->Value;
                Params.swap(Params[which_param].param->Params);
                for(size_t a=0; a<Params.size(); ++a)
                    Params[a].param->Parent = this;
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
                        if(!Params[a].sign) goto ReplaceTreeWithZero;
                        all_values_are_nonzero = false;
                    }
                    else if( (p.has_max && p.max <= -0.5)
                          || (p.has_min && p.min >= 0.5)) // |x| >= 0.5  = nonzero
                    {
                        if(Params[a].sign) goto ReplaceTreeWithZero;
                    }
                    else
                        all_values_are_nonzero = false;
                }
                if(all_values_are_nonzero) goto ReplaceTreeWithOne;
                if(Params.size() == 1 && !Params[0].sign)
                {
                    // Replace self with the single operand
                    Opcode = Params[0].sign ? cNot : cNotNot;
                    Params[0].sign = false;
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
                        if(Params[a].sign) goto ReplaceTreeWithOne;
                    }
                    else if( (p.has_max && p.max <= -0.5)
                          || (p.has_min && p.min >= 0.5)) // |x| >= 0.5  = nonzero
                    {
                        if(!Params[a].sign) goto ReplaceTreeWithOne;
                        all_values_are_zero = false;
                    }
                    else
                        all_values_are_zero = false;
                }
                if(all_values_are_zero) goto ReplaceTreeWithZero;
                if(Params.size() == 1)
                {
                    // Replace self with the single operand
                    Opcode = Params[0].sign ? cNot : cNotNot;
                    Params[0].sign = false;
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
                    if(Params[a].sign) immed = 1.0 / immed;
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
                        AddParam( Param(new CodeTree(mul_immed_sum), false) );
                }
                if(Params.size() == 1 && !Params[0].sign)
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
                    if(Params[a].sign) immed = -immed;
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
                        AddParam( Param(new CodeTree(immed_sum), false) );
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
                    AddParam( Param(new CodeTree(-1.0), false) );
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
                        AddParam(Param(subtree, false));
                        /* Now:
                         * this    = p * Abs(x*y)
                         */
                        if(!neg_set.empty())
                        {
                            for(size_t a=0; a<neg_set.size(); ++a)
                                AddParam(neg_set[a]);
                            AddParam( Param(new CodeTree(-1.0), false) );
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
                    AddParam(Param(subtree, false)); // we = atan(y/x)
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
            case cCot: // converted into cMul ~(cTan x)
            case cSec: // converted into cMul ~(cCos x)
            case cCsc: // converted into cMul ~(cSin x)
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
#         ifndef FP_DISABLE_EVAL
            case cEval:
#endif
                break;
        }
        /*
        if(Parent)
            Parent->ConstantFolding();

        */
    }
}

namespace FPoptimizer_Grammar
{
    static double GetPackConst(size_t index)
    {
        double res = pack.clist[index];
    #if 0
        if(res == FPOPT_NAN_CONST)
        {
        #ifdef NAN
            return NAN;
        #else
            return 0.0; // Should be 0.0/0.0, but some compilers don't like that
        #endif
        }
    #endif
        return res;
    }

    /* A helper for std::equal_range */
    struct OpcodeRuleCompare
    {
        bool operator() (const FPoptimizer_CodeTree::CodeTree& tree, const Rule& rule) const
        {
            /* If this function returns true, len=half.
             */

            if(tree.Opcode != rule.func.opcode)
                return tree.Opcode < rule.func.opcode;

            if(tree.Params.size() < rule.n_minimum_params)
            {
                // Tree has fewer params than required?
                return true; // Failure
            }
            return false;
        }
        bool operator() (const Rule& rule, const FPoptimizer_CodeTree::CodeTree& tree) const
        {
            /* If this function returns true, rule will be excluded from the equal_range
             */

            if(rule.func.opcode != tree.Opcode)
                return rule.func.opcode < tree.Opcode;

            if(rule.n_minimum_params < tree.Params.size())
            {
                // Tree has more params than the pattern has?
                switch(pack.mlist[rule.func.index].type)
                {
                    case PositionalParams:
                    case SelectedParams:
                        return true; // Failure
                    case AnyParams:
                        return false; // Not a failure
                }
            }
            return false;
        }
    };

#ifdef DEBUG_SUBSTITUTIONS
    static const char ImmedHolderNames[4][2]  = {"%","&"};
    static const char NamedHolderNames[10][2] = {"x","y","z","a","b","c","d","e","f","g"};
#endif

    /* Apply the grammar to a given CodeTree */
    bool Grammar::ApplyTo(
        FPoptimizer_CodeTree::CodeTree& tree,
        bool recursion) const
    {
        bool changed = false;

        recursion=recursion;

        if(tree.OptimizedUsing != this)
        {
            /* First optimize all children */
            tree.ConstantFolding();

            for(size_t a=0; a<tree.Params.size(); ++a)
            {
                if( ApplyTo( *tree.Params[a].param, true ) )
                {
                    changed = true;
                }
            }

            if(changed)
            {
                // Give the parent node a rerun at optimization
                return true;
            }

            /* Figure out which rules _may_ match this tree */
            typedef const Rule* ruleit;

            std::pair<ruleit, ruleit> range
                = MyEqualRange(pack.rlist + this->index,
                               pack.rlist + this->index + this->count,
                               tree,
                               OpcodeRuleCompare());

#ifdef DEBUG_SUBSTITUTIONS
            std::cout << "Input (Grammar #"
                      << (this - pack.glist)
                      << ", " << FP_GetOpcodeName(tree.Opcode)
                      << "[" << tree.Params.size()
                      << "], rules "
                      << (range.first - pack.rlist)
                      << ".."
                      << (range.second - pack.rlist)
                      << ": ";
            DumpTree(tree);
            std::cout << "\n" << std::flush;
#endif

            while(range.first != range.second)
            {
                /* Check if this rule matches */
                if(range.first->ApplyTo(tree))
                {
                    changed = true;
                    break;
                }
                ++range.first;
            }

#ifdef DEBUG_SUBSTITUTIONS
            std::cout << (changed ? "Changed." : "No changes.");
            std::cout << "\n" << std::flush;
#endif

            if(!changed)
            {
                tree.OptimizedUsing = this;
            }
        }
        else
        {
#ifdef DEBUG_SUBSTITUTIONS
            std::cout << "Already optimized:  ";
            DumpTree(tree);
            std::cout << "\n" << std::flush;
#endif
        }

#ifdef DEBUG_SUBSTITUTIONS
        if(!recursion)
        {
            std::cout << "Output: ";
            DumpTree(tree);
            std::cout << "\n" << std::flush;
        }
#endif
        return changed;
    }

    /* Store information about a potential match,
     * in order to iterate through candidates
     */
    struct MatchedParams::CodeTreeMatch
    {
        // Which parameters were matched -- these will be replaced if AnyParams are used
        std::vector<size_t> param_numbers;

        // Which values were saved for ImmedHolders?
        std::map<unsigned, double> ImmedMap;
        // Which codetrees were saved for each NameHolder?
            struct NamedItem
            {
                fphash_t hash;
                size_t   n_synthesized;

                NamedItem(): hash(),n_synthesized(0) { }
                explicit NamedItem(fphash_t h): hash(h),n_synthesized(0) { }
            };
        std::map<unsigned, NamedItem> NamedMap;
        // Which codetrees were saved for each RestHolder?
        std::map<unsigned,
          std::vector<fphash_t> > RestMap;

        // Examples of each codetree
        std::map<fphash_t, FPoptimizer_CodeTree::CodeTreeP> trees;

        CodeTreeMatch() : param_numbers(), ImmedMap(), NamedMap(), RestMap() { }
    };

#ifdef DEBUG_SUBSTITUTIONS
    void DumpMatch(const Function& input,
                   const FPoptimizer_CodeTree::CodeTree& tree,
                   const MatchedParams& replacement,
                   const MatchedParams::CodeTreeMatch& matchrec,
                   bool DidMatch=true);
    void DumpFunction(const Function& input);
    void DumpParam(const ParamSpec& p);
    void DumpParams(const MatchedParams& mitem);
#endif

    /* Apply the rule to a given CodeTree */
    bool Rule::ApplyTo(
        FPoptimizer_CodeTree::CodeTree& tree) const
    {
        const Function&      input  = func;
        const MatchedParams& repl   = pack.mlist[repl_index];

        if(input.opcode == tree.Opcode)
        {
            for(unsigned long match_index=0; ; ++match_index)
            {
                MatchedParams::CodeTreeMatch matchrec;
                MatchResultType mr =
                    pack.mlist[input.index].Match(tree, matchrec,match_index, false);
                if(!mr.found && mr.has_more) continue;
                if(!mr.found) break;

    #ifdef DEBUG_SUBSTITUTIONS
                DumpMatch(input, tree, repl, matchrec);
    #endif

                const MatchedParams& params = pack.mlist[input.index];
                switch(type)
                {
                    case ReplaceParams:
                        repl.ReplaceParams(tree, params, matchrec);
    #ifdef DEBUG_SUBSTITUTIONS
                        std::cout << "  ParmReplace: ";
                        DumpTree(tree);
                        std::cout << "\n" << std::flush;
                        DumpHashes(tree);
    #endif
                        return true;
                    case ProduceNewTree:
                        repl.ReplaceTree(tree,   params, matchrec);
    #ifdef DEBUG_SUBSTITUTIONS
                        std::cout << "  TreeReplace: ";
                        DumpTree(tree);
                        std::cout << "\n" << std::flush;
                        DumpHashes(tree);
    #endif
                        return true;
                }
                break; // should be unreachable
            }
        }
        #ifdef DEBUG_SUBSTITUTIONS
        // Report mismatch
        MatchedParams::CodeTreeMatch matchrec;
        DumpMatch(input, tree, repl, matchrec, false);
        #endif
        return false;
    }


    /* Match the given function to the given CodeTree.
     */
    MatchResultType Function::Match(
        FPoptimizer_CodeTree::CodeTree& tree,
        MatchedParams::CodeTreeMatch& match,
        unsigned long match_index) const
    {
        if(opcode != tree.Opcode) return NoMatch;
        return pack.mlist[index].Match(tree, match, match_index, true);
    }


    /* This struct is used by MatchedParams::Match() for backtracking. */
    struct ParamMatchSnapshot
    {
        MatchedParams::CodeTreeMatch snapshot;
                                    // Snapshot of the state so far
        size_t            parampos; // Which position was last chosen?
        std::vector<bool> used;     // Which params were allocated?

        size_t            matchpos;
    };

    /* Match the given list of ParamSpecs using the given ParamMatchingType
     * to the given CodeTree.
     * The CodeTree is already assumed to be a function type
     * -- i.e. it is assumed that the caller has tested the Opcode of the tree.
     */
    MatchResultType MatchedParams::Match(
        FPoptimizer_CodeTree::CodeTree& tree,
        MatchedParams::CodeTreeMatch& match,
        unsigned long match_index,
        bool recursion) const
    {
        /*        match_index is a feature for backtracking.
         *
         *        For example,
         *          cMul (cAdd x) (cAdd x)
         *        Applied to:
         *          (a+b)*(c+b)
         *
         *        Match (cAdd x) to (a+b) may first capture "a" into "x",
         *        and then Match(cAdd x) for (c+b) will fail,
         *        because there's no "a" there.
         *
         *        However, match_index can be used to indicate that the
         *        _second_ matching will be used, so that "b" will be
         *        captured into "x".
         */


        /* First, check if the tree has any chances of matching... */
        /* Figure out what we need. */
        struct Needs
        {
            struct Needs_Pol
            {
                int SubTrees; // This many subtrees
                int Others;   // This many others (namedholder)
                unsigned SubTreesDetail[VarBegin]; // This many subtrees of each opcode type

                Needs_Pol(): SubTrees(0), Others(0), SubTreesDetail()
                {
                }
            } polarity[2]; // 0=positive, 1=negative

            int Immeds;      // This many immeds

            Needs(): polarity(), Immeds() { }
        } NeedList;

        // Figure out what we need
        size_t minimum_need = 0;
        for(unsigned a=0; a<count; ++a)
        {
            const ParamSpec& param = pack.plist[index+a];
            Needs::Needs_Pol& needs = NeedList.polarity[param.sign];
            switch(param.opcode)
            {
                case SubFunction:
                    needs.SubTrees += 1;
                    assert( pack.flist[param.index].opcode < VarBegin );
                    needs.SubTreesDetail[ pack.flist[param.index].opcode ] += 1;
                    ++minimum_need;
                    break;
                case NumConstant:
                case ImmedHolder:
                default: // GroupFunction:
                    NeedList.Immeds += 1;
                    ++minimum_need;
                    break;
                case NamedHolder:
                    needs.Others += 1;
                    ++minimum_need;
                    break;
                case RestHolder:
                    break;
            }
        }
        if(tree.Params.size() < minimum_need)
        {
            // Impossible to satisfy
            return NoMatch;
        }

        // Figure out what we have (note: we already assume that the opcode of the tree matches!)
        for(size_t a=0; a<tree.Params.size(); ++a)
        {
            Needs::Needs_Pol& needs = NeedList.polarity[tree.Params[a].sign];
            unsigned opcode = tree.Params[a].param->Opcode;
            switch(opcode)
            {
                case cImmed:
                    if(NeedList.Immeds > 0) NeedList.Immeds -= 1;
                    else needs.Others -= 1;
                    break;
                case cVar:
                case cFCall:
                case cPCall:
                    needs.Others -= 1;
                    break;
                default:
                    assert( opcode < VarBegin );
                    if(needs.SubTrees > 0
                    && needs.SubTreesDetail[opcode] > 0)
                    {
                        needs.SubTrees -= 1;
                        needs.SubTreesDetail[opcode] -= 1;
                    }
                    else needs.Others -= 1;
            }
        }

        // Check whether all needs were satisfied
        if(NeedList.Immeds > 0
        || NeedList.polarity[0].SubTrees > 0
        || NeedList.polarity[0].Others > 0
        || NeedList.polarity[1].SubTrees > 0
        || NeedList.polarity[1].Others > 0)
        {
            // Something came short, impossible to satisfy.
            return NoMatch;
        }

        if(type != AnyParams)
        {
            if(NeedList.Immeds < 0
            || NeedList.polarity[0].SubTrees < 0
            || NeedList.polarity[0].Others < 0
            || NeedList.polarity[1].SubTrees < 0
            || NeedList.polarity[1].Others < 0
            || count != tree.Params.size())
            {
                // Something was too much.
                return NoMatch;
            }
        }

        /* Which transformation to do on the operand if a sign is present */
        TransformationType transf = None;
        switch(tree.Opcode)
        {
            case cAdd: transf = Negate; break;
            case cMul: transf = Invert; break;
            case cAnd:
            case cOr:  transf = NotThe; break;
        }

        switch(type)
        {
            case PositionalParams:
            {
                /*DumpTree(tree);
                std::cout << "<->";
                DumpParams(*this);
                std::cout << " -- ";*/

                std::vector<MatchPositionSpec<CodeTreeMatch> > specs;
                specs.reserve(count);
                //fprintf(stderr, "Enter loop %lu\n", match_index);
                for(unsigned a=0; a<count; ++a)
                {
                    specs.resize(a+1);

                PositionalParamsMatchingLoop:;
                    // Match this parameter.
                    MatchResultType mr = pack.plist[index+a].Match(
                        *tree.Params[a].param, match,
                        tree.Params[a].sign ? transf : None,
                        specs[a].roundno);

                    specs[a].done = !mr.has_more;

                    // If it was not found, backtrack...
                    if(!mr.found)
                    {
                    LoopThisRound:
                        while(specs[a].done)
                        {
                            // Backtrack
                            if(a <= 0) return NoMatch; //
                            specs.resize(a);
                            --a;
                            match = specs[a].data;
                        }
                        ++specs[a].roundno;
                        goto PositionalParamsMatchingLoop;
                    }
                    // If found...
                    if(!recursion)
                        match.param_numbers.push_back(a);
                    specs[a].data = match;

                    if(a == count-1U && match_index > 0)
                    {
                        // Skip this match
                        --match_index;
                        goto LoopThisRound;
                    }
                }
                /*std::cout << " yay?\n";*/
                // Match = no mismatch.
                bool final_try = true;
                for(unsigned a=0; a<count; ++a)
                    if(!specs[a].done) { final_try = false; break; }
                //fprintf(stderr, "Exit  loop %lu\n", match_index);
                return MatchResultType(true, !final_try);
            }
            case AnyParams:
            case SelectedParams:
            {
                const size_t n_tree_params = tree.Params.size();

                unsigned N_PositiveRestHolders = 0;
                unsigned N_NegativeRestHolders = 0;
                for(unsigned a=0; a<count; ++a)
                {
                    const ParamSpec& param = pack.plist[index+a];
                    if(param.opcode == RestHolder)
                    {
                        if(param.sign)
                            ++N_NegativeRestHolders;
                        else
                            ++N_PositiveRestHolders;
                    }
                }

                bool HasRestHolders = N_PositiveRestHolders || N_NegativeRestHolders;

                #ifdef DEBUG_SUBSTITUTIONS
                if((type == AnyParams) && recursion && !HasRestHolders)
                {
                    std::cout << "Recursed AnyParams with no RestHolders?\n";
                    DumpParams(*this);
                }
                #endif

                if(!HasRestHolders && recursion && count != n_tree_params)
                {
                    /*DumpTree(tree);
                    std::cout << "<->";
                    DumpParams(*this);
                    std::cout << " -- fail due to recursion&&count!=n_tree_params";*/
                    return NoMatch; // Impossible match.
                }

                /*std::cout << "Matching ";
                DumpTree(tree); std::cout << " with ";
                DumpParams(*this);
                std::cout << " , match_index=" << match_index << "\n" << std::flush;*/

                std::vector<ParamMatchSnapshot> position(count);
                std::vector<bool>               used(n_tree_params);

                unsigned p=0;

                for(; p<count; ++p)
                {
                    position[p].snapshot  = match;
                    position[p].parampos  = 0;
                    position[p].matchpos  = 0;
                    position[p].used      = used;

                    //fprintf(stderr, "posA: p=%u count=%u\n", p, count);

                backtrack:
                  {
                    if(pack.plist[index+p].opcode == RestHolder)
                    {
                        // RestHolders always match. They're filled afterwards.
                        position[p].parampos = n_tree_params;
                        position[p].matchpos = 0;
                        continue;
                    }

                    size_t whichparam = position[p].parampos;
                    size_t whichmatch = position[p].matchpos;

                    /* a          = param index in the syntax specification
                     * whichparam = param index in the tree received from parser
                     */

                    /*fprintf(stderr, "posB: p=%u, whichparam=%lu, whichmatch=%lu\n",
                        p,whichparam,whichmatch);*/
                    while(whichparam < n_tree_params)
                    {
                        if(used[whichparam])
                        {
                        NextParamNumber:
                            ++whichparam;
                            whichmatch = 0;
                            continue;
                        NextMatchNumber:
                            ++whichmatch;
                        }

                        /*std::cout << "Maybe [" << p << "]:";
                        DumpParam(pack.plist[index+p]);
                        std::cout << " <-> ";
                        if(tree.Params[whichparam].sign) std::cout << '~';
                        DumpTree(*tree.Params[whichparam].param);
                        std::cout << "...?\n" << std::flush;*/

                        MatchResultType mr = pack.plist[index+p].Match(
                            *tree.Params[whichparam].param, match,
                            tree.Params[whichparam].sign ? transf : None,
                            whichmatch);

                        /*std::cout << "In ";
                        DumpTree(tree); std::cout << std::flush;
                        fprintf(stderr, ", trying param %lu, match %lu (matchindex %lu); got %s,%s: ",
                            whichparam,whichmatch, match_index,
                            mr.found?"found":"not found",
                            mr.has_more?"more":"no more"); fflush(stderr);
                        DumpParam(pack.plist[index+p]); std::cout << "\n" << std::flush;*/

                        if(!mr.found)
                        {
                            if(!mr.has_more) goto NextParamNumber;
                            goto NextMatchNumber;
                        }

                        /*std::cout << "woo... " << a << ", " << b << "\n";*/
                        /* NamedHolders require a special treatment,
                         * because a repetition count may be issued
                         * for them.
                         */
                        if(pack.plist[index+p].opcode == NamedHolder)
                        {
                            // Verify the MinRepeat & AnyRepeat case
                            /*fprintf(stderr, "Got repeat %u, needs %u\n", HadRepeat,MinRepeat);*/
                            used[whichparam] = true;
                            if(!recursion) match.param_numbers.push_back(whichparam);
                        }
                        else
                        {
                            used[whichparam] = true;
                            if(!recursion) match.param_numbers.push_back(whichparam);
                        }
                        position[p].parampos = mr.has_more ? whichparam : (whichparam+1);
                        position[p].matchpos = mr.has_more ? (whichmatch+1) : 0;
                        goto ok;
                    }

                    /*DumpParam(param);
                    std::cout << " didn't match anything in ";
                    DumpTree(tree);
                    std::cout << "\n";*/
                  }

                    // No match for this param, try backtracking.
                DiscardedThisAttempt:
                    while(p > 0)
                    {
                        --p;
                        ParamMatchSnapshot& prevpos = position[p];
                        if(prevpos.parampos < n_tree_params)
                        {
                            // Try another combination.
                            match = prevpos.snapshot;
                            used  = prevpos.used;
                            goto backtrack;
                        }
                    }
                    // If we cannot backtrack, break. No possible match.
                    /*if(!recursion)
                        std::cout << "Drats!\n";*/
                    if(match_index == 0)
                        return NoMatch;
                    break;
                ok:;
                    /*if(!recursion)
                        std::cout << "Match for param " << a << " at " << b << std::endl;*/

                    if(p == count-1U && match_index > 0)
                    {
                        // Skip this match
                        --match_index;
                        goto DiscardedThisAttempt;
                    }
                }
                /*fprintf(stderr, "End loop, match_index=%lu\n", match_index); fflush(stderr);*/

                /* We got a match. */

                // If the rule cares about the balance of
                // negative restholdings versus positive restholdings,
                // verify them.
                if(balance != BalanceDontCare)
                {
                    unsigned n_pos_restholdings = 0;
                    unsigned n_neg_restholdings = 0;

                    for(unsigned a=0; a<count; ++a)
                    {
                        const ParamSpec& param = pack.plist[index+a];
                        if(param.opcode == RestHolder)
                        {
                            for(size_t b=0; b<n_tree_params; ++b)
                                if(tree.Params[b].sign == param.sign && !used[b])
                                {
                                    if(param.sign)
                                        n_neg_restholdings += 1;
                                    else
                                        n_pos_restholdings += 1;
                                }
                        }
                    }
                    switch(balance)
                    {
                        case BalanceMoreNeg:
                            if(n_neg_restholdings <= n_pos_restholdings) return NoMatch;
                            break;
                        case BalanceMorePos:
                            if(n_pos_restholdings <= n_neg_restholdings) return NoMatch;
                            break;
                        case BalanceEqual:
                            if(n_pos_restholdings != n_neg_restholdings) return NoMatch;
                            break;
                        case BalanceDontCare: ;
                    }
                }

                unsigned pos_rest_remain = N_PositiveRestHolders;
                unsigned neg_rest_remain = N_NegativeRestHolders;

                // Verify if we have RestHolder constraints.
                for(unsigned a=0; a<count; ++a)
                {
                    const ParamSpec& param = pack.plist[index+a];
                    if(param.opcode == RestHolder)
                    {
                        std::map<unsigned, std::vector<fphash_t> >::iterator
                            i = match.RestMap.lower_bound(param.index);

                        if(i != match.RestMap.end() && i->first == param.index)
                        {
                            unsigned& n_remaining_restholders_of_this_kind =
                                param.sign ? neg_rest_remain : pos_rest_remain;
                            /*fprintf(stderr, "Does restholder %u match in", param.index);
                            fflush(stderr); DumpTree(tree); std::cout << "? " << std::flush;*/

                            const std::vector<fphash_t>& RefRestList = i->second;
                            for(size_t r=0; r<RefRestList.size(); ++r)
                            {
                                for(size_t b=0; b<n_tree_params; ++b)
                                    if(tree.Params[b].sign == param.sign
                                    && !used[b]
                                    && tree.Params[b].param->Hash == RefRestList[r]
                                    && tree.Params[b].param->IsIdenticalTo(
                                        * match.trees.find(RefRestList[r])->second )
                                      )
                                    {
                                        used[b] = true;
                                        goto SatisfiedRestHolder;
                                    }
                                // Unsatisfied RestHolder constraint
                                /*fprintf(stderr, "- no\n");*/
                                p=count-1;
                                goto DiscardedThisAttempt;
                            SatisfiedRestHolder:;
                            }
                            --n_remaining_restholders_of_this_kind;
                            /*fprintf(stderr, "- yes\n");*/
                        }
                    }
                }

                // Now feed any possible RestHolders the remaining parameters.
                bool more_restholder_options = false;
                for(unsigned a=0; a<count; ++a)
                {
                    const ParamSpec& param = pack.plist[index+a];
                    if(param.opcode == RestHolder)
                    {
                        std::map<unsigned, std::vector<fphash_t> >::iterator
                            i = match.RestMap.lower_bound(param.index);
                        if(i != match.RestMap.end() && i->first == param.index) continue;

                        std::vector<fphash_t>& RestList = match.RestMap[param.index]; // mark it up

                        unsigned& n_remaining_restholders_of_this_kind =
                            param.sign ? neg_rest_remain : pos_rest_remain;

                        unsigned n_remaining_params = 0;
                        for(size_t b=0; b<n_tree_params; ++b)
                            if(tree.Params[b].sign == param.sign && !used[b])
                                ++n_remaining_params;

                        /*fprintf(stderr, "[index %lu] For restholder %u, %u remains, %u remaining of kind\n",
                            match_index,
                            (unsigned)param.index, (unsigned)n_remaining_params,
                            (unsigned)n_remaining_restholders_of_this_kind);
                            fflush(stderr);*/

                        if(n_remaining_params > 0)
                        {
                            if(n_remaining_params > 8) n_remaining_params = 8;
                            unsigned n_remaining_combinations = 1 << n_remaining_params;

                            unsigned n_options = n_remaining_restholders_of_this_kind > 1
                                ? n_remaining_combinations
                                : 1;
                            size_t selection = n_remaining_combinations - 1;
                            if(n_options > 1)
                            {
                                --n_options;
                                selection = match_index % (n_options); ++selection;
                                match_index /= n_options;
                            }
                            if(selection+1 < n_options) more_restholder_options = true;

                            /*fprintf(stderr, "- selected %u/%u\n", selection, n_options); fflush(stderr);*/

                            unsigned matchbit = 1;
                            for(size_t b=0; b<n_tree_params; ++b)
                                if(tree.Params[b].sign == param.sign && !used[b])
                                {
                                    if(selection & matchbit)
                                    {
                                        /*fprintf(stderr, "- uses param %lu\n", b);*/
                                        if(!recursion)
                                            match.param_numbers.push_back(b);
                                        fphash_t hash = tree.Params[b].param->Hash;
                                        RestList.push_back(hash);
                                        match.trees.insert(
                                            std::make_pair(hash, tree.Params[b].param) );

                                        used[b] = true;
                                    }
                                    if(matchbit < 0x80U) matchbit <<= 1;
                                }
                        }
                        --n_remaining_restholders_of_this_kind;
                    }
                }
                /*std::cout << "Returning match for ";
                DumpTree(tree);
                std::cout << "\n               with ";
                DumpParams(*this); std::cout << std::flush;
                fprintf(stderr, ", %s hope for more (now %lu)\n",
                    more_restholder_options ? "with" : "without", match_index); fflush(stderr);*/
                return more_restholder_options ? FoundSomeMatch : FoundLastMatch;
            }
        }
        return NoMatch;
    }

    MatchResultType ParamSpec::Match(
        FPoptimizer_CodeTree::CodeTree& tree,
        MatchedParams::CodeTreeMatch& match,
        TransformationType transf,
        unsigned long match_index) const
    {
        assert(opcode != RestHolder); // RestHolders are supposed to be handled by the caller

        switch(OpcodeType(opcode))
        {
            case NumConstant:
            {
                if(!tree.IsImmed()) return NoMatch;
                double res = tree.GetImmed();
                double res2 = GetPackConst(index);
                if(transf == Negate) res2 = -res2;
                if(transf == Invert) res2 = 1/res2;
                if(transf == NotThe) res2 = res2 != 0;
                /*std::cout << std::flush;
                fprintf(stderr, "Comparing %.20f and %.20f\n", res, res2);
                fflush(stderr);*/
                if(!FloatEqual(res, res2)) return NoMatch;
                return FoundLastMatch; // Previously unknown NumConstant, good
            }
            case ImmedHolder:
            {
                if(!tree.IsImmed()) return NoMatch;
                double res = tree.GetImmed();

                switch( ImmedConstraint_Value(count & ValueMask) )
                {
                    case ValueMask: break;
                    case Value_AnyNum: break;
                    case Value_EvenInt:
                        if(!FloatEqual(res, (double)(long)(res))) return NoMatch;
                        if( (long)(res) % 2 != 0) return NoMatch;
                        break;
                    case Value_OddInt:
                        if(!FloatEqual(res, (double)(long)(res))) return NoMatch;
                        if( (long)(res) % 2 == 0) return NoMatch;
                        break;
                    case Value_IsInteger:
                        if(!FloatEqual(res, (double)(long)(res))) return NoMatch;
                        break;
                    case Value_NonInteger:
                        if(FloatEqual(res, (double)(long)(res))) return NoMatch;
                        break;
                }
                switch( ImmedConstraint_Sign(count & SignMask) )
                {
                    /*case SignMask: break;*/
                    case Sign_AnySign: break;
                    case Sign_Positive:
                        if(res < 0.0)  return NoMatch;
                        break;
                    case Sign_Negative:
                        if(res >= 0.0) return NoMatch;
                        break;
                    case Sign_NoIdea:
                        return NoMatch;
                }
                switch( ImmedConstraint_Oneness(count & OnenessMask) )
                {
                    case OnenessMask: break;
                    case Oneness_Any: break;
                    case Oneness_One:
                        if(!FloatEqual(fabs(res), 1.0)) return NoMatch;
                        break;
                    case Oneness_NotOne:
                        if(FloatEqual(fabs(res), 1.0)) return NoMatch;
                        break;
                }

                std::map<unsigned, double>::iterator
                    i = match.ImmedMap.lower_bound(index);
                if(i != match.ImmedMap.end() && i->first == index)
                {
                    double res2 = i->second;
                    if(transf == Negate) res2 = -res2;
                    if(transf == Invert) res2 = 1/res2;
                    if(transf == NotThe) res2 = res2 != 0;
                    /*std::cout << std::flush;
                    fprintf(stderr, "Comparing %.20f and %.20f\n", res, res2);
                    fflush(stderr);*/
                    return FloatEqual(res, res2) ? FoundLastMatch : NoMatch;
                }
                if(sign != (transf != None)) return NoMatch;

                match.ImmedMap.insert(i, std::make_pair((unsigned)index, res));
                return FoundLastMatch; // Previously unknown ImmedHolder, good
            }
            case NamedHolder:
            {
                if(sign != (transf != None)) return NoMatch;
                std::map<unsigned, MatchedParams::CodeTreeMatch::NamedItem>::iterator
                    i = match.NamedMap.lower_bound(index);
                if(i != match.NamedMap.end() && i->first == index)
                {
                    /*fprintf(stderr, "NamedHolder found: %16lX -- tested against %16lX\n", i->second.first, tree.Hash);*/
                    if(tree.Hash == i->second.hash
                    && tree.IsIdenticalTo(* match.trees.find(i->second.hash)->second)
                      )
                        return FoundLastMatch;
                    else
                        return NoMatch;
                }

                switch( ImmedConstraint_Value(count & ValueMask) )
                {
                    case ValueMask: break;
                    case Value_AnyNum: break;
                    case Value_EvenInt:
                        if(!tree.IsAlwaysParity(false)) return NoMatch;
                        break;
                    case Value_OddInt:
                        if(!tree.IsAlwaysParity(true)) return NoMatch;
                        break;
                    case Value_IsInteger:
                        if(!tree.IsAlwaysInteger()) return NoMatch;
                        break;
                    case Value_NonInteger:
                        if(tree.IsAlwaysInteger()) return NoMatch;
                        break;
                }
                switch( ImmedConstraint_Sign(count & SignMask) )
                {
                    /*case SignMask: break;*/
                    case Sign_AnySign: break;
                    case Sign_Positive:
                        if(!tree.IsAlwaysSigned(true)) return NoMatch;
                        break;
                    case Sign_Negative:
                        if(!tree.IsAlwaysSigned(false)) return NoMatch;
                        break;
                    case Sign_NoIdea:
                        if(tree.IsAlwaysSigned(false)) return NoMatch;
                        if(tree.IsAlwaysSigned(true)) return NoMatch;
                        break;
                }
                switch( ImmedConstraint_Oneness(count & OnenessMask) )
                {
                    case OnenessMask: break;
                    case Oneness_Any: break;
                    case Oneness_One:    return NoMatch;
                    case Oneness_NotOne: return NoMatch;
                }

                match.NamedMap.insert(i,
                    std::make_pair(index,
                        MatchedParams::CodeTreeMatch::NamedItem(tree.Hash) ));
                match.trees.insert(std::make_pair(tree.Hash, &tree));
                return FoundLastMatch; // Previously unknown NamedHolder, good
            }
            case RestHolder:
            {
                break;
            }
            case SubFunction:
            {
                if(sign != (transf != None)) return NoMatch;

                switch( ImmedConstraint_Value(count & ValueMask) )
                {
                    case ValueMask: break;
                    case Value_AnyNum: break;
                    case Value_EvenInt:
                        if(!tree.IsAlwaysParity(false)) return NoMatch;
                        break;
                    case Value_OddInt:
                        if(!tree.IsAlwaysParity(true)) return NoMatch;
                        break;
                    case Value_IsInteger:
                        if(!tree.IsAlwaysInteger()) return NoMatch;
                        break;
                    case Value_NonInteger:
                        if(tree.IsAlwaysInteger()) return NoMatch;
                        break;
                }
                switch( ImmedConstraint_Sign(count & SignMask) )
                {
                    /*case SignMask: break;*/
                    case Sign_AnySign: break;
                    case Sign_Positive:
                        if(!tree.IsAlwaysSigned(true)) return NoMatch;
                        break;
                    case Sign_Negative:
                        if(!tree.IsAlwaysSigned(false)) return NoMatch;
                        break;
                    case Sign_NoIdea:
                        if(tree.IsAlwaysSigned(false)) return NoMatch;
                        if(tree.IsAlwaysSigned(true)) return NoMatch;
                        break;
                }
                switch( ImmedConstraint_Oneness(count & OnenessMask) )
                {
                    case OnenessMask: break;
                    case Oneness_Any: break;
                    case Oneness_One:    return NoMatch;
                    case Oneness_NotOne: return NoMatch;
                }

                return pack.flist[index].Match(tree, match, match_index);
            }
            default: // means groupfunction. No ImmedConstraint
            {
                if(!tree.IsImmed()) return NoMatch;
                double res = tree.GetImmed();
                double res2;
                if(!GetConst(match, res2)) return NoMatch;
                if(transf == Negate) res2 = -res2;
                if(transf == Invert) res2 = 1/res2;
                if(transf == NotThe) res2 = res2 != 0;
                /*std::cout << std::flush;
                fprintf(stderr, "Comparing %.20f and %.20f\n", res, res2);
                fflush(stderr);*/
                return FloatEqual(res, res2) ? FoundLastMatch : NoMatch;
            }
        }
        return NoMatch;
    }

    bool ParamSpec::GetConst(
        const MatchedParams::CodeTreeMatch& match,
        double& result) const
    {
        switch(OpcodeType(opcode))
        {
            case NumConstant:
                result = GetPackConst(index);
                break;
            case ImmedHolder:
            {
                std::map<unsigned, double>::const_iterator
                    i = match.ImmedMap.find(index);
                if(i == match.ImmedMap.end()) return false; // impossible
                result = i->second;
                //fprintf(stderr, "immedholder: %.20f\n", result);
                break;
            }
            case NamedHolder:
            {
                // Not enumerable
                return false;
            }
            case RestHolder:
            {
                // Not enumerable
                return false;
            }
            case SubFunction:
            {
                // Not enumerable
                return false;
            }
            default:
            {
                switch(OPCODE(opcode))
                {
                    case cAdd:
                        result=0;
                        for(unsigned p=0; p<count; ++p)
                        {
                            double tmp;
                            if(!pack.plist[index+p].GetConst(match, tmp)) return false;
                            result += tmp;
                        }
                        break;
                    case cMul:
                        result=1;
                        for(unsigned p=0; p<count; ++p)
                        {
                            double tmp;
                            if(!pack.plist[index+p].GetConst(match, tmp)) return false;
                            result *= tmp;
                        }
                        break;
                    case cMin:
                        for(unsigned p=0; p<count; ++p)
                        {
                            double tmp;
                            if(!pack.plist[index+p].GetConst(match, tmp)) return false;
                            if(p == 0 || tmp < result) result = tmp;
                        }
                        break;
                    case cMax:
                        for(unsigned p=0; p<count; ++p)
                        {
                            double tmp;
                            if(!pack.plist[index+p].GetConst(match, tmp)) return false;
                            if(p == 0 || tmp > result) result = tmp;
                        }
                        break;
                    case cSin: if(!pack.plist[index].GetConst(match, result))return false;
                               result = std::sin(result); break;
                    case cCos: if(!pack.plist[index].GetConst(match, result))return false;
                               result = std::cos(result); break;
                    case cTan: if(!pack.plist[index].GetConst(match, result))return false;
                               result = std::tan(result); break;
                    case cAsin: if(!pack.plist[index].GetConst(match, result))return false;
                                result = std::asin(result); break;
                    case cAcos: if(!pack.plist[index].GetConst(match, result))return false;
                                result = std::acos(result); break;
                    case cAtan: if(!pack.plist[index].GetConst(match, result))return false;
                                result = std::atan(result); break;
                    case cSinh: if(!pack.plist[index].GetConst(match, result))return false;
                                result = std::sinh(result); break;
                    case cCosh: if(!pack.plist[index].GetConst(match, result))return false;
                                result = std::cosh(result); break;
                    case cTanh: if(!pack.plist[index].GetConst(match, result))return false;
                                 result = std::tanh(result); break;

                    case cAsinh: if(!pack.plist[index].GetConst(match, result))return false;
                                 result = fp_asinh(result); break;
                    case cAcosh: if(!pack.plist[index].GetConst(match, result))return false;
                                 result = fp_acosh(result); break;
                    case cAtanh: if(!pack.plist[index].GetConst(match, result))return false;
                                 result = fp_atanh(result); break;

                    case cCeil: if(!pack.plist[index].GetConst(match, result))return false;
                                result = std::ceil(result); break;
                    case cFloor: if(!pack.plist[index].GetConst(match, result))return false;
                                 result = std::floor(result); break;
                    case cLog: if(!pack.plist[index].GetConst(match, result))return false;
                               result = std::log(result); break;
                    case cExp: if(!pack.plist[index].GetConst(match, result))return false;
                               result = std::exp(result); break;
                    case cExp2: if(!pack.plist[index].GetConst(match, result))return false;
                               result = std::pow(2.0, result); break;
                    case cLog2: if(!pack.plist[index].GetConst(match, result))return false;
                                result = std::log(result) * CONSTANT_L2I;
                                //result = std::log2(result);
                                break;
                    case cLog10: if(!pack.plist[index].GetConst(match, result))return false;
                                 result = std::log10(result); break;
                    case cAbs: if(!pack.plist[index].GetConst(match, result))return false;
                               result = std::fabs(result); break;
                    case cNeg: if(!pack.plist[index].GetConst(match, result))return false;
                               result = -result; break;
                    case cInv: if(!pack.plist[index].GetConst(match, result))return false;
                               result = 1.0 / result; break;
                    case cNot: if(!pack.plist[index].GetConst(match, result))return false;
                               result = FloatEqual(result, 0.0); break;
                    case cPow:
                    {
                        if(!pack.plist[index+0].GetConst(match, result))return false;
                        double tmp;
                        if(!pack.plist[index+1].GetConst(match, tmp))return false;
                        result = std::pow(result, tmp);
                        //fprintf(stderr, "pow result: %.20f\n", result);
                        break;
                    }
                    case cMod:
                    {
                        if(!pack.plist[index+0].GetConst(match, result))return false;
                        double tmp;
                        if(!pack.plist[index+1].GetConst(match, tmp))return false;
                        result = std::fmod(result, tmp);
                        break;
                    }
                    default:
                        fprintf(stderr, "Unknown macro opcode: %s\n",
                            FP_GetOpcodeName(opcode).c_str());
                        return false;
                }
            }
        }
        return true;
    }

    void MatchedParams::SynthesizeTree(
        FPoptimizer_CodeTree::CodeTree& tree,
        const MatchedParams& matcher,
        MatchedParams::CodeTreeMatch& match) const
    {
        for(unsigned a=0; a<count; ++a)
        {
            const ParamSpec& param = pack.plist[index+a];
            if(param.opcode == RestHolder)
            {
                // Add children directly to this tree
                param.SynthesizeTree(tree, matcher, match);
            }
            else
            {
                FPoptimizer_CodeTree::CodeTree* subtree = new FPoptimizer_CodeTree::CodeTree;
                param.SynthesizeTree(*subtree, matcher, match);
                subtree->ConstantFolding();
                subtree->Sort();
                subtree->Recalculate_Hash_NoRecursion(); // rehash this, but not the children, nor the parent
                FPoptimizer_CodeTree::CodeTree::Param p(subtree, param.sign) ;
                tree.AddParam(p);
            }
        }
    }

    void MatchedParams::ReplaceParams(
        FPoptimizer_CodeTree::CodeTree& tree,
        const MatchedParams& matcher,
        MatchedParams::CodeTreeMatch& match) const
    {
        // Replace the 0-level params indicated in "match" with the ones we have

        // First, construct the tree recursively using the "match" info
        SynthesizeTree(tree, matcher, match);

        // Remove the indicated params
        std::sort(match.param_numbers.begin(), match.param_numbers.end());
        for(size_t a=match.param_numbers.size(); a-->0; )
        {
            size_t num = match.param_numbers[a];
            tree.DelParam(num);
        }

        tree.ConstantFolding();

        tree.Sort();
        tree.Rehash(true); // rehash this and its parents, but not its children
    }

    void MatchedParams::ReplaceTree(
        FPoptimizer_CodeTree::CodeTree& tree,
        const MatchedParams& matcher,
        CodeTreeMatch& match) const
    {
        // Replace the entire tree with one indicated by our Params[0]
        // Note: The tree is still constructed using the holders indicated in "match".
        std::vector<FPoptimizer_CodeTree::CodeTree::Param> OldParams = tree.Params;
        tree.Params.clear();
        pack.plist[index].SynthesizeTree(tree, matcher, match);

        tree.ConstantFolding();

        tree.Sort();
        tree.Rehash(true);  // rehash this and its parents, but not its children
    }

    /* Synthesizes a new tree based on the given information
     * in ParamSpec. Assume the tree is empty, don't deallocate
     * anything. Don't touch Hash, Parent.
     */
    void ParamSpec::SynthesizeTree(
        FPoptimizer_CodeTree::CodeTree& tree,
        const MatchedParams& matcher,
        MatchedParams::CodeTreeMatch& match) const
    {
        switch(SpecialOpcode(opcode))
        {
            case RestHolder:
            {
                std::map<unsigned, std::vector<fphash_t> >
                    ::const_iterator i = match.RestMap.find(index);

                assert(i != match.RestMap.end());

                /*std::cout << std::flush;
                fprintf(stderr, "Restmap %u, sign %d, size is %u -- params %u\n",
                    (unsigned) i->first, sign, (unsigned) i->second.size(),
                    (unsigned) tree.Params.size());*/

                for(size_t a=0; a<i->second.size(); ++a)
                {
                    fphash_t hash = i->second[a];

                    std::map<fphash_t, FPoptimizer_CodeTree::CodeTreeP>
                        ::const_iterator j = match.trees.find(hash);

                    assert(j != match.trees.end());

                    FPoptimizer_CodeTree::CodeTree* subtree = j->second->Clone();
                    FPoptimizer_CodeTree::CodeTree::Param p(subtree, sign);
                    tree.AddParam(p);
                }
                /*fprintf(stderr, "- params size became %u\n", (unsigned)tree.Params.size());
                fflush(stderr);*/
                break;
            }
            case SubFunction:
            {
                const Function& fitem = pack.flist[index];
                tree.Opcode = fitem.opcode;
                const MatchedParams& mitem = pack.mlist[fitem.index];
                mitem.SynthesizeTree(tree, matcher, match);
                break;
            }
            case NamedHolder:
            {
                /* Literal parameter */
                std::map<unsigned, MatchedParams::CodeTreeMatch::NamedItem>
                    ::iterator i = match.NamedMap.find(index);

                assert(i != match.NamedMap.end());

                fphash_t hash = i->second.hash;

                std::map<fphash_t, FPoptimizer_CodeTree::CodeTreeP>
                    ::const_iterator j = match.trees.find(hash);

                assert(j != match.trees.end());

                tree.Opcode = j->second->Opcode;
                switch(tree.Opcode)
                {
                    case cImmed: tree.Value = j->second->Value; break;
                    case cVar:   tree.Var   = j->second->Var;  break;
                    case cFCall:
                    case cPCall: tree.Funcno = j->second->Funcno; break;
                }

                /* Note: SetParams() will Clone() all the given params.
                 *       This is considered appropriate, because the
                 *       same NamedHolder may be synthesized in multiple
                 *       trees.
                 *       Example of such rule:
                 *         asinh(x) -> log2(x + (x^2 + 1)^0.5) * CONSTANT_L2
                 *       We use n_synthesized here to limit the cloning only
                 *       to successive invokations of the same tree. The first
                 *       instance is simply assigned. This is safe, because the
                 *       tree from which it was brought, will not be used anymore.
                 */
                tree.SetParams(j->second->Params, i->second.n_synthesized++ > 0);
                break;
            }
            case NumConstant:
            case ImmedHolder:
            default:
                tree.Opcode = cImmed;
                GetConst(match, tree.Value); // note: return value is ignored
                // FIXME: Should we check ImmedConstraints here?
                break;
        }
    }

#ifdef DEBUG_SUBSTITUTIONS
    void DumpParam(const ParamSpec& p)
    {
        //std::cout << "/*p" << (&p-pack.plist) << "*/";

        if(p.sign) std::cout << '~';

        bool has_constraint = false;
        switch(SpecialOpcode(p.opcode))
        {
            case NumConstant: std::cout << GetPackConst(p.index); break;
            case ImmedHolder: has_constraint = true; std::cout << ImmedHolderNames[p.index]; break;
            case NamedHolder: has_constraint = true; std::cout << NamedHolderNames[p.index]; break;
            case RestHolder: std::cout << '<' << p.index << '>'; break;
            case SubFunction: DumpFunction(pack.flist[p.index]); break;
            default:
            {
                std::string opcode = FP_GetOpcodeName(p.opcode).substr(1);
                for(size_t a=0; a<opcode.size(); ++a) opcode[a] = std::toupper(opcode[a]);
                std::cout << opcode << '(';
                for(unsigned a=0; a<p.count; ++a)
                {
                    if(a > 0) std::cout << ' ';
                    DumpParam(pack.plist[p.index+a]);
                }
                std::cout << " )";
            }
        }
        if(has_constraint)
        {
            switch( ImmedConstraint_Value(p.count & ValueMask) )
            {
                case ValueMask: break;
                case Value_AnyNum: break;
                case Value_EvenInt:   std::cout << "@E"; break;
                case Value_OddInt:    std::cout << "@O"; break;
                case Value_IsInteger: std::cout << "@I"; break;
                case Value_NonInteger:std::cout << "@F"; break;
            }
            switch( ImmedConstraint_Sign(p.count & SignMask) )
            {
                case SignMask: break;
                case Sign_AnySign: break;
                case Sign_Positive:   std::cout << "@P"; break;
                case Sign_Negative:   std::cout << "@N"; break;
            }
            switch( ImmedConstraint_Oneness(p.count & OnenessMask) )
            {
                case OnenessMask: break;
                case Oneness_Any: break;
                case Oneness_One:     std::cout << "@1"; break;
                case Oneness_NotOne:  std::cout << "@M"; break;
            }
        }
    }

    void DumpParams(const MatchedParams& mitem)
    {
        //std::cout << "/*m" << (&mitem-pack.mlist) << "*/";

        if(mitem.type == PositionalParams) std::cout << '[';
        if(mitem.type == SelectedParams) std::cout << '{';

        for(unsigned a=0; a<mitem.count; ++a)
        {
            std::cout << ' ';
            DumpParam(pack.plist[mitem.index + a]);
        }

        switch(mitem.balance)
        {
            case BalanceMorePos: std::cout << " =+"; break;
            case BalanceMoreNeg: std::cout << " =-"; break;
            case BalanceEqual:   std::cout << " =="; break;
            case BalanceDontCare: break;
        }

        if(mitem.type == PositionalParams) std::cout << " ]";
        if(mitem.type == SelectedParams) std::cout << " }";
    }

    void DumpFunction(const Function& fitem)
    {
        //std::cout << "/*f" << (&fitem-pack.flist) << "*/";

        std::cout << '(' << FP_GetOpcodeName(fitem.opcode);
        DumpParams(pack.mlist[fitem.index]);
        std::cout << ')';
    }
    void DumpMatch(const Function& input,
                   const FPoptimizer_CodeTree::CodeTree& tree,
                   const MatchedParams& replacement,
                   const MatchedParams::CodeTreeMatch& matchrec,
                   bool DidMatch)
    {
        std::cout <<
            "Found " << (DidMatch ? "match" : "mismatch") << ":\n"
            "  Pattern    : ";
        DumpFunction(input);
        std::cout << "\n"
            "  Replacement: ";
        DumpParams(replacement);
        std::cout << "\n";

        std::cout <<
            "  Tree       : ";
        DumpTree(tree);
        std::cout << "\n";
        if(DidMatch) DumpHashes(tree);

        for(std::map<unsigned, MatchedParams::CodeTreeMatch::NamedItem>::const_iterator
            i = matchrec.NamedMap.begin(); i != matchrec.NamedMap.end(); ++i)
        {
            std::cout << "           " << NamedHolderNames[i->first] << " = ";
            DumpTree(*matchrec.trees.find(i->second.hash)->second);
            std::cout << "\n";
        }

        for(std::map<unsigned, double>::const_iterator
            i = matchrec.ImmedMap.begin(); i != matchrec.ImmedMap.end(); ++i)
        {
            std::cout << "           " << ImmedHolderNames[i->first] << " = ";
            std::cout << i->second << std::endl;
        }

        for(std::map<unsigned, std::vector<fphash_t> >::const_iterator
            i = matchrec.RestMap.begin(); i != matchrec.RestMap.end(); ++i)
        {
            for(size_t a=0; a<i->second.size(); ++a)
            {
                fphash_t hash = i->second[a];
                std::cout << "         <" << i->first << "> = ";
                DumpTree(*matchrec.trees.find(hash)->second);
                std::cout << std::endl;
            }
            if(i->second.empty())
                std::cout << "         <" << i->first << "> = <empty>\n";
        }
        std::cout << std::flush;
    }
    void DumpHashes(const FPoptimizer_CodeTree::CodeTree& tree,
                    std::map<fphash_t, std::set<std::string> >& done)
    {
        for(size_t a=0; a<tree.Params.size(); ++a)
            DumpHashes(*tree.Params[a].param, done);

        std::stringstream buf;
        DumpTree(tree, buf);
        done[tree.Hash].insert(buf.str());
    }
    void DumpHashes(const FPoptimizer_CodeTree::CodeTree& tree)
    {
        std::map<fphash_t, std::set<std::string> > done;
        DumpHashes(tree, done);

        for(std::map<fphash_t, std::set<std::string> >::const_iterator
            i = done.begin();
            i != done.end();
            ++i)
        {
            const std::set<std::string>& flist = i->second;
            if(flist.size() != 1) std::cout << "ERROR - HASH COLLISION?\n";
            for(std::set<std::string>::const_iterator
                j = flist.begin();
                j != flist.end();
                ++j)
            {
                //std::cout << '[' << std::hex << i->first << ']' << std::dec;
                std::cout << ": " << *j << "\n";
            }
        }
    }
    void DumpTree(const FPoptimizer_CodeTree::CodeTree& tree, std::ostream& o)
    {
        //o << "/*" << tree.Depth << "*/";
        const char* sep2 = "";
        //o << '[' << std::hex << tree.Hash << ']' << std::dec;
        switch(tree.Opcode)
        {
            case cImmed: o << tree.Value; return;
            case cVar:   o << "Var" << tree.Var; return;
            case cAdd: sep2 = " +"; break;
            case cMul: sep2 = " *"; break;
            case cAnd: sep2 = " &"; break;
            case cOr: sep2 = " |"; break;
            case cPow: sep2 = " ^"; break;
            default:
                o << FP_GetOpcodeName(tree.Opcode);
                if(tree.Opcode == cFCall || tree.Opcode == cPCall)
                    o << ':' << tree.Funcno;
        }
        o << '(';
        if(tree.Params.size() <= 1 && *sep2) o << (sep2+1) << ' ';
        for(size_t a=0; a<tree.Params.size(); ++a)
        {
            if(a > 0) o << ' ';
            if(tree.Params[a].sign) o << '~';

            DumpTree(*tree.Params[a].param, o);

            if(tree.Params[a].param->Parent != &tree)
            {
                o << "(?parent?)";
            }

            if(a+1 < tree.Params.size()) o << sep2;
        }
        o << ')';
    }
#endif
}

#endif
