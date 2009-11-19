#include "fpoptimizer_codetree.hh"
#include "fpoptimizer_optimize.hh"
#include "fpoptimizer_consts.hh"

#include <cmath> /* for CalculateResultBoundaries() */
#include <algorithm>

#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#ifdef FP_SUPPORT_OPTIMIZER

using namespace FUNCTIONPARSERTYPES;
using namespace FPoptimizer_CodeTree;

#define FP_MUL_COMBINE_EXPONENTS

#ifdef _MSC_VER
#include <float.h>
#define isinf(x) (!_finite(x))
#endif

namespace
{
    bool IsLogicalTrueValue(const MinMaxTree& p, bool abs)
    {
        if(p.has_min && p.min >= 0.5) return true;
        if(!abs && p.has_max && p.max <= -0.5) return true;
        return false;
    }
    bool IsLogicalFalseValue(const MinMaxTree& p, bool abs)
    {
        if(abs)
            return p.has_max && p.max < 0.5;
        else
            return p.has_min && p.has_max
               && p.min > -0.5 && p.max < 0.5;
    }
    int GetLogicalValue(const MinMaxTree& p, bool abs)
    {
        if(IsLogicalTrueValue(p, abs)) return 1;
        if(IsLogicalFalseValue(p, abs)) return 0;
        return -1;
    }

    struct ComparisonSet /* For optimizing And, Or */
    {
        static const int Lt_Mask = 0x1; // 1=less
        static const int Eq_Mask = 0x2; // 2=equal
        static const int Le_Mask = 0x3; // 1+2 = Less or Equal
        static const int Gt_Mask = 0x4; // 4=greater
        static const int Ne_Mask = 0x5; // 4+1 = Greater or Less, i.e. Not equal
        static const int Ge_Mask = 0x6; // 4+2 = Greater or Equal
        static int Swap_Mask(int m) { return (m&Eq_Mask)
                                  | ((m&Lt_Mask) ? Gt_Mask : 0)
                                  | ((m&Gt_Mask) ? Lt_Mask : 0); }
        struct Comparison
        {
            CodeTree a;
            CodeTree b;
            int relationship;
        };
        std::vector<Comparison> relationships;
        struct Item
        {
            CodeTree value;
            bool negated;
        };
        std::vector<Item> plain_set;
        int const_offset;

        enum RelationshipResult
        {
            Ok,
            BecomeZero,
            BecomeOne,
            Suboptimal
        };
        enum ConditionType
        {
            cond_or,
            cond_and,
            cond_mul,
            cond_add
        };

        ComparisonSet():
            relationships(),
            plain_set(),
            const_offset(0)
        {
        }

        RelationshipResult AddItem(const CodeTree& a, bool negated, ConditionType type)
        {
            for(size_t c=0; c<plain_set.size(); ++c)
                if(plain_set[c].value.IsIdenticalTo(a))
                {
                    if(negated != plain_set[c].negated)
                    {
                        switch(type)
                        {
                            case cond_or:
                                return BecomeOne;
                            case cond_add:
                                plain_set.erase(plain_set.begin() + c);
                                const_offset += 1;
                                return Suboptimal;
                            case cond_and:
                            case cond_mul:
                                return BecomeZero;
                        }
                    }
                    return Suboptimal;
                }
            Item pole;
            pole.value   = a;
            pole.negated = negated;
            plain_set.push_back(pole);
            return Ok;
        }

        RelationshipResult AddRelationship(CodeTree a, CodeTree b, int reltype, ConditionType type)
        {
            switch(type)
            {
                case cond_or:
                    if(reltype == 7) return BecomeOne;
                    break;
                case cond_add:
                    if(reltype == 7) { const_offset += 1; return Suboptimal; }
                    break;
                case cond_and:
                case cond_mul:
                    if(reltype == 0) return BecomeZero;
                    break;
            }

            if(!(a.GetHash() < b.GetHash()))
            {
                a.swap(b);
                reltype = Swap_Mask(reltype);
            }

            for(size_t c=0; c<relationships.size(); ++c)
            {
                if(relationships[c].a.IsIdenticalTo(a)
                && relationships[c].b.IsIdenticalTo(b))
                {
                    switch(type)
                    {
                        case cond_or:
                        {
                            int newrel = relationships[c].relationship | reltype;
                            if(newrel == 7) return BecomeOne;
                            relationships[c].relationship = newrel;
                            break;
                        }
                        case cond_and:
                        case cond_mul:
                        {
                            int newrel = relationships[c].relationship & reltype;
                            if(newrel == 0) return BecomeZero;
                            relationships[c].relationship = newrel;
                            break;
                        }
                        case cond_add:
                        {
                            int newrel_or  = relationships[c].relationship | reltype;
                            int newrel_and = relationships[c].relationship & reltype;
                            if(newrel_or  == 5 // < + >
                            && newrel_and == 0)
                            {
                                // (x<y) + (x>y) = x!=y
                                relationships[c].relationship = Ne_Mask;
                                return Suboptimal;
                            }
                            if(newrel_or  == 7
                            && newrel_and == 0)
                            {
                                // (x<y) + (x>=y) = 1
                                // (x<=y) + (x>y) = 1
                                // (x=y) + (x!=y) = 1
                                const_offset += 1;
                                relationships.erase(relationships.begin()+c);
                                return Suboptimal;
                            }
                            if(newrel_or  == 7
                            && newrel_and == Eq_Mask)
                            {
                                // (x<=y) + (x>=y) = 1 + (x=y)
                                relationships[c].relationship = Eq_Mask;
                                const_offset += 1;
                                return Suboptimal;
                            }
                            continue;
                        }
                    }
                    return Suboptimal;
                }
            }
            Comparison comp;
            comp.a = a;
            comp.b = b;
            comp.relationship = reltype;
            relationships.push_back(comp);
            return Ok;
        }
    };

    struct CollectionSet /* For optimizing Add,  Mul */
    {
        struct Collection
        {
            CodeTree value;
            CodeTree factor;
            bool factor_needs_rehashing;

            Collection() : value(),factor(), factor_needs_rehashing(false) { }
            Collection(const CodeTree& v, const CodeTree& f)
                : value(v), factor(f), factor_needs_rehashing(false) { }
        };
        std::multimap<fphash_t, Collection> collections;

        enum CollectionResult
        {
            Ok,
            Suboptimal
        };

        typedef std::multimap<fphash_t, Collection>::iterator PositionType;

        PositionType FindIdenticalValueTo(const CodeTree& value)
        {
            fphash_t hash = value.GetHash();
            for(PositionType
                i = collections.lower_bound(hash);
                i != collections.end() && i->first == hash;
                ++i)
            {
                if(value.IsIdenticalTo(i->second.value))
                    return i;
            }
            return collections.end();
        }
        bool Found(const PositionType& b) { return b != collections.end(); }

        CollectionResult AddCollectionTo(const CodeTree& factor,
                                         const PositionType& into_which)
        {
            Collection& c = into_which->second;
            if(c.factor_needs_rehashing)
                c.factor.AddParam(factor);
            else
            {
                CodeTree add;
                add.SetOpcode(cAdd);
                add.AddParamMove(c.factor);
                add.AddParam(factor);
                c.factor.swap(add);
                c.factor_needs_rehashing = true;
            }
            return Suboptimal;
        }

        CollectionResult AddCollection(const CodeTree& value, const CodeTree& factor)
        {
            const fphash_t hash = value.GetHash();
            PositionType i = collections.lower_bound(hash);
            for(; i != collections.end() && i->first == hash; ++i)
            {
                if(i->second.value.IsIdenticalTo(value))
                    return AddCollectionTo(factor, i);
            }
            collections.insert(
                i,
                std::make_pair( hash, Collection(value, factor) ) );
            return Ok;
        }

        CollectionResult AddCollection(const CodeTree& a)
        {
            return AddCollection(a, CodeTree(1.0) );
        }
    };

    struct Select2ndRev
    {
        template<typename T>
        inline bool operator() (const T& a, const T& b) const
        {
            return a.second > b.second;
        }
    };
    struct Select1st
    {
        template<typename T1, typename T2>
        inline bool operator() (const std::pair<T1,T2>& a,
                                const std::pair<T1,T2>& b) const
        {
            return a.first < b.first;
        }

        template<typename T1, typename T2>
        inline bool operator() (const std::pair<T1,T2>& a, T1 b) const
        {
            return a.first < b;
        }

        template<typename T1, typename T2>
        inline bool operator() (T1 a, const std::pair<T1,T2>& b) const
        {
            return a < b.first;
        }
    };

    bool IsEvenIntegerConst(double v)
    {
        return IsIntegerConst(v) && ((long)v % 2) == 0;
    }

    struct ConstantExponentCollection
    {
        typedef std::pair<double, std::vector<CodeTree> > ExponentInfo;
        std::vector<ExponentInfo> data;

        void MoveToSet_Unique(double exponent, std::vector<CodeTree>& source_set)
        {
            data.push_back( std::pair<double, std::vector<CodeTree> >
                            (exponent, std::vector<CodeTree>() ) );
            data.back().second.swap(source_set);
        }
        void MoveToSet_NonUnique(double exponent, std::vector<CodeTree>& source_set)
        {
            std::vector<ExponentInfo>::iterator i
                = std::lower_bound(data.begin(), data.end(), exponent, Select1st());
            if(i != data.end() && i->first == exponent)
            {
                i->second.insert(i->second.end(), source_set.begin(), source_set.end());
            }
            else
            {
                //MoveToSet_Unique(exponent, source_set);
                data.insert(i,  std::pair<double, std::vector<CodeTree> >
                                (exponent, source_set) );
            }
        }

        bool Optimize()
        {
            /* TODO: Group them such that:
             *
             *      x^3 *         z^2 becomes (x*z)^2 * x^1
             *      x^3 * y^2.5 * z^2 becomes (x*z*y)^2 * y^0.5 * x^1
             *                    rather than (x*y*z)^2 * (x*y)^0.5 * x^0.5
             *
             *      x^4.5 * z^2.5     becomes (z * x)^2.5 * x^2
             *                        becomes (x*z*x)^2 * (z*x)^0.5
             *                        becomes (z*x*x*z*x)^0.5 * (z*x*x)^1.5 -- buzz, bad.
             *
             */
            bool changed = false;
            std::sort( data.begin(), data.end(), Select1st() );
        redo:
            /* Supposed algorithm:
             * For the smallest pair of data[] where the difference
             * between the two is a "neat value" (x*16 is positive integer),
             * do the combining as indicated above.
             */
            /*
             * NOTE: Hanged in Testbed test P44, looping the following
             *       (Var0 ^ 0.75) * ((1.5 * Var0) ^ 1.0)
             *     = (Var0 ^ 1.75) *  (1.5         ^ 1.0)
             *       Fixed by limiting to cases where (exp_a != 1.0).
             *
             * NOTE: Converting (x*z)^0.5 * x^16.5
             *              into x^17 * z^0.5
             *       is handled by code within CollectMulGroup().
             *       However, bacause it is prone for infinite looping,
             *       the use of "IsIdenticalTo(before)" is added at the
             *       end of ConstantFolding_MulGrouping().
             *
             *       This algorithm could make it into (x*z*x)^0.5 * x^16,
             *       but this is wrong, for it falsely includes x^evenint.. twice.
             */
            for(size_t a=0; a<data.size(); ++a)
            {
                double exp_a = data[a].first;
                if(FloatEqual(exp_a, 1.0)) continue;
                for(size_t b=a+1; b<data.size(); ++b)
                {
                    double exp_b = data[b].first;
                    double exp_diff = exp_b - exp_a;
                    if(exp_diff >= fabs(exp_a)) break;
                    double exp_diff_still_probable_integer = exp_diff * 16.0;
                    if(IsIntegerConst(exp_diff_still_probable_integer)
                    && !(IsIntegerConst(exp_b) && !IsIntegerConst(exp_diff))
                      )
                    {
                        /* When input is x^3 * z^2,
                         * exp_a = 2
                         * a_set = z
                         * exp_b = 3
                         * b_set = x
                         * exp_diff = 3-2 = 1
                         */
                        std::vector<CodeTree>& a_set = data[a].second;
                        std::vector<CodeTree>& b_set = data[b].second;
          #ifdef DEBUG_SUBSTITUTIONS
                        std::cout << "Before ConstantExponentCollection iteration:\n";
                        Dump(std::cout);
          #endif
                        if(IsIntegerConst(exp_b)
                        && IsEvenIntegerConst(exp_b)
                        //&& !IsEvenIntegerConst(exp_diff)
                        && !IsEvenIntegerConst(exp_diff+exp_a))
                        {
                            CodeTree tmp2;
                            tmp2.SetOpcode(cMul);
                            tmp2.SetParamsMove(b_set);
                            tmp2.Rehash();
                            CodeTree tmp;
                            tmp.SetOpcode(cAbs);
                            tmp.AddParamMove(tmp2);
                            tmp.Rehash();
                            b_set.resize(1);
                            b_set[0].swap(tmp);
                        }

                        a_set.insert(a_set.end(), b_set.begin(), b_set.end());

                        std::vector<CodeTree> b_copy = b_set;
                        data.erase(data.begin() + b);
                        MoveToSet_NonUnique(exp_diff, b_copy);
                        changed = true;

          #ifdef DEBUG_SUBSTITUTIONS
                        std::cout << "After ConstantExponentCollection iteration:\n";
                        Dump(std::cout);
          #endif
                        goto redo;
                    }
                }
            }
            return changed;
        }

    #ifdef DEBUG_SUBSTITUTIONS
        void Dump(std::ostream& out)
        {
            for(size_t a=0; a<data.size(); ++a)
            {
                out.precision(12);
                out << data[a].first << ": ";
                for(size_t b=0; b<data[a].second.size(); ++b)
                {
                    if(b > 0) out << '*';
                    DumpTree(data[a].second[b], out);
                }
                out << std::endl;
            }
        }
    #endif

    };

    struct RangeComparisonData
    {
        enum Decision { MakeFalse=0, MakeTrue=1, MakeNEqual=2, MakeEqual=3, Unchanged=4 };

        Decision if_identical; // What to do when operands are identical
        Decision if_always[4]; // What to do if Always <, <=, >, >=

        Decision Analyze(const CodeTree& a, const CodeTree& b) const
        {
            if(a.IsIdenticalTo(b))
                return if_identical;

            MinMaxTree p0 = a.CalculateResultBoundaries();
            MinMaxTree p1 = b.CalculateResultBoundaries();
            if(p0.has_max && p1.has_min)
            {
                if(p0.max <  p1.min)// && if_always[0] != Unchanged)
                    return if_always[0]; // p0 < p1
                if(p0.max <= p1.min)// && if_always[1] != Unchanged)
                    return if_always[1]; // p0 <= p1
            }
            if(p0.has_min && p1.has_max)
            {
                if(p0.min >  p1.max)// && if_always[2] != Unchanged)
                    return if_always[2]; // p0 > p1
                if(p0.min >= p1.max)// && if_always[3] != Unchanged)
                    return if_always[3]; // p0 >= p1
            }
            return Unchanged;
        }
    };

}

namespace FPoptimizer_CodeTree
{
    template<typename CondType> /* ComparisonSet::ConditionType */
    bool CodeTree::ConstantFolding_LogicCommon(CondType cond_type, bool is_logical)
    {
        bool should_regenerate = false;
        ComparisonSet comp;
        for(size_t a=0; a<GetParamCount(); ++a)
        {
            ComparisonSet::RelationshipResult change = ComparisonSet::Ok;
            const CodeTree& atree = GetParam(a);
            switch(atree.GetOpcode())
            {
                case cEqual:
                    change = comp.AddRelationship(atree.GetParam(0), atree.GetParam(1), ComparisonSet::Eq_Mask, cond_type);
                    break;
                case cNEqual:
                    change = comp.AddRelationship(atree.GetParam(0), atree.GetParam(1), ComparisonSet::Ne_Mask, cond_type);
                    break;
                case cLess:
                    change = comp.AddRelationship(atree.GetParam(0), atree.GetParam(1), ComparisonSet::Lt_Mask, cond_type);
                    break;
                case cLessOrEq:
                    change = comp.AddRelationship(atree.GetParam(0), atree.GetParam(1), ComparisonSet::Le_Mask, cond_type);
                    break;
                case cGreater:
                    change = comp.AddRelationship(atree.GetParam(0), atree.GetParam(1), ComparisonSet::Gt_Mask, cond_type);
                    break;
                case cGreaterOrEq:
                    change = comp.AddRelationship(atree.GetParam(0), atree.GetParam(1), ComparisonSet::Ge_Mask, cond_type);
                    break;
                case cNot:
                    change = comp.AddItem(atree.GetParam(0), true, cond_type);
                    break;
                case cNotNot:
                    change = comp.AddItem(atree.GetParam(0), false, cond_type);
                    break;
                default:
                    if(is_logical || atree.IsLogicalValue())
                        change = comp.AddItem(atree, false, cond_type);
            }
            switch(change)
            {
            ReplaceTreeWithZero:
                    data = new CodeTreeData(0.0);
                    return true;
            ReplaceTreeWithOne:
                    data = new CodeTreeData(1.0);
                    return true;
                case ComparisonSet::Ok: // ok
                    break;
                case ComparisonSet::BecomeZero: // whole set was invalidated
                    goto ReplaceTreeWithZero;
                case ComparisonSet::BecomeOne: // whole set was validated
                    goto ReplaceTreeWithOne;
                case ComparisonSet::Suboptimal: // something was changed
                    should_regenerate = true;
                    break;
            }
        }
        if(should_regenerate)
        {
          #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "Before ConstantFolding_LogicCommon: "; DumpTree(*this);
            std::cout << "\n";
          #endif

            if(is_logical)
            {
                DelParams(); // delete all params
            }
            else
            {
                // Delete only logical params
                for(size_t a=GetParamCount(); a-- > 0; )
                {
                    const CodeTree& atree = GetParam(a);
                    if(atree.IsLogicalValue())
                        DelParam(a);
                }
            }

            for(size_t a=0; a<comp.plain_set.size(); ++a)
            {
                if(comp.plain_set[a].negated)
                {
                    CodeTree r;
                    r.SetOpcode(cNot);
                    r.AddParamMove(comp.plain_set[a].value);
                    r.Rehash();
                    AddParamMove(r);
                }
                else if(!is_logical)
                {
                    CodeTree r;
                    r.SetOpcode(cNotNot);
                    r.AddParamMove(comp.plain_set[a].value);
                    r.Rehash();
                    AddParamMove(r);
                }
                else
                    AddParamMove(comp.plain_set[a].value);
            }
            for(size_t a=0; a<comp.relationships.size(); ++a)
            {
                CodeTree r;
                r.SetOpcode(cNop); // dummy
                switch(comp.relationships[a].relationship)
                {
                    case ComparisonSet::Lt_Mask: r.SetOpcode( cLess ); break;
                    case ComparisonSet::Eq_Mask: r.SetOpcode( cEqual ); break;
                    case ComparisonSet::Gt_Mask: r.SetOpcode( cGreater ); break;
                    case ComparisonSet::Le_Mask: r.SetOpcode( cLessOrEq ); break;
                    case ComparisonSet::Ne_Mask: r.SetOpcode( cNEqual ); break;
                    case ComparisonSet::Ge_Mask: r.SetOpcode( cGreaterOrEq ); break;
                }
                r.AddParamMove(comp.relationships[a].a);
                r.AddParamMove(comp.relationships[a].b);
                r.Rehash();
                AddParamMove(r);
            }
            if(comp.const_offset != 0)
                AddParam( CodeTree( double(comp.const_offset) ) );
          #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "After ConstantFolding_LogicCommon: "; DumpTree(*this);
            std::cout << "\n";
          #endif
            return true;
        }
        /*
        Note: One thing this does not yet do, is to detect chains
              such as x=y & y=z & x=z, which could be optimized
              to x=y & x=z.
        */
        return false;
    }

    bool CodeTree::ConstantFolding_AndLogic()
    {
        return ConstantFolding_LogicCommon( ComparisonSet::cond_and, true );
    }
    bool CodeTree::ConstantFolding_OrLogic()
    {
        return ConstantFolding_LogicCommon( ComparisonSet::cond_or, true );
    }
    bool CodeTree::ConstantFolding_AddLogicItems()
    {
        return ConstantFolding_LogicCommon( ComparisonSet::cond_add, false );
    }
    bool CodeTree::ConstantFolding_MulLogicItems()
    {
        return ConstantFolding_LogicCommon( ComparisonSet::cond_mul, false );
    }

    static CodeTree CollectMulGroup_Item(
        CodeTree& value,
        bool& has_highlevel_opcodes)
    {
        switch(value.GetOpcode())
        {
            case cPow:
            {
                CodeTree exponent = value.GetParam(1);
                value.Become( value.GetParam(0) );
                return exponent;
            }
            case cSqrt:
                value.Become( value.GetParam(0) );
                has_highlevel_opcodes = true;
                return CodeTree(0.5);
            case cRSqrt:
                value.Become( value.GetParam(0) );
                has_highlevel_opcodes = true;
                return CodeTree(-0.5);
            case cInv:
                value.Become( value.GetParam(0) );
                has_highlevel_opcodes = true;
                return CodeTree(-1.0);
            default: break;
        }
        return CodeTree(1.0);
    }

    static void CollectMulGroup(
        CollectionSet& mul, const CodeTree& tree, const CodeTree& factor,
        bool& should_regenerate,
        bool& has_highlevel_opcodes
    )
    {
        for(size_t a=0; a<tree.GetParamCount(); ++a)
        {
            CodeTree value(tree.GetParam(a));

            CodeTree exponent ( CollectMulGroup_Item(value, has_highlevel_opcodes) );

            if(!factor.IsImmed() || factor.GetImmed() != 1.0)
            {
                CodeTree new_exp;
                new_exp.SetOpcode(cMul);
                new_exp.AddParam( exponent );
                new_exp.AddParam( factor );
                new_exp.Rehash();
                exponent.swap( new_exp );
            }
        #if 0 /* FIXME: This does not work */
            if(value.GetOpcode() == cMul)
            {
                if(1)
                {
                    // Avoid erroneously converting
                    //          (x*z)^0.5 * z^2
                    // into     x^0.5 * z^2.5
                    // It should be x^0.5 * abs(z)^2.5, but this is not a good conversion.
                    bool exponent_is_even = exponent.IsImmed() && IsEvenIntegerConst(exponent.GetImmed());

                    for(size_t b=0; b<value.GetParamCount(); ++b)
                    {
                        bool tmp=false;
                        CodeTree val(value.GetParam(b));
                        CodeTree exp(CollectMulGroup_Item(val, tmp));
                        if(exponent_is_even
                        || (exp.IsImmed() && IsEvenIntegerConst(exp.GetImmed())))
                        {
                            CodeTree new_exp;
                            new_exp.SetOpcode(cMul);
                            new_exp.AddParam(exponent);
                            new_exp.AddParamMove(exp);
                            new_exp.ConstantFolding();
                            if(!new_exp.IsImmed() || !IsEvenIntegerConst(new_exp.GetImmed()))
                            {
                                goto cannot_adopt_mul;
                            }
                        }
                    }
                }
                CollectMulGroup(mul, value, exponent,
                                should_regenerate,
                                has_highlevel_opcodes);
            }
            else cannot_adopt_mul:
        #endif
            {
                if(mul.AddCollection(value, exponent) == CollectionSet::Suboptimal)
                    should_regenerate = true;
            }
        }
    }

    bool CodeTree::ConstantFolding_MulGrouping()
    {
        bool has_highlevel_opcodes = false;
        bool should_regenerate = false;
        CollectionSet mul;

        CollectMulGroup(mul, *this, CodeTree(1.0),
                        should_regenerate,
                        has_highlevel_opcodes);

        typedef std::pair<CodeTree/*exponent*/,
                          std::vector<CodeTree>/*base value (mul group)*/
                         > exponent_list;
        typedef std::multimap<fphash_t,/*exponent hash*/
                              exponent_list> exponent_map;
        exponent_map by_exponent;

        for(CollectionSet::PositionType
            j = mul.collections.begin();
            j != mul.collections.end();
            ++j)
        {
            CodeTree& value = j->second.value;
            CodeTree& exponent = j->second.factor;
            if(j->second.factor_needs_rehashing) exponent.Rehash();
            const fphash_t exponent_hash = exponent.GetHash();

            exponent_map::iterator i = by_exponent.lower_bound(exponent_hash);
            for(; i != by_exponent.end() && i->first == exponent_hash; ++i)
                if(i->second.first.IsIdenticalTo(exponent))
                {
                    if(!exponent.IsImmed() || !FloatEqual(exponent.GetImmed(), 1.0))
                        should_regenerate = true;
                    i->second.second.push_back(value);
                    goto skip_b;
                }
            by_exponent.insert(i, std::make_pair(exponent_hash,
                std::make_pair(exponent,
                               std::vector<CodeTree> (size_t(1), value)
                              )));
        skip_b:;
        }

    #ifdef FP_MUL_COMBINE_EXPONENTS
        ConstantExponentCollection by_float_exponent;
        for(exponent_map::iterator
            j,i = by_exponent.begin();
            i != by_exponent.end();
            i=j)
        {
            j=i; ++j;
            exponent_list& list = i->second;
            if(list.first.IsImmed())
            {
                double exponent = list.first.GetImmed();
                if(!(exponent == 0.0))
                    by_float_exponent.MoveToSet_Unique(exponent, list.second);
                by_exponent.erase(i);
            }
        }
        if(by_float_exponent.Optimize())
            should_regenerate = true;
    #endif

        if(should_regenerate)
        {
            CodeTree before = *this;
            before.CopyOnWrite();

          #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "Before ConstantFolding_MulGrouping: "; DumpTree(before);
            std::cout << "\n";
          #endif
            DelParams();

            /* Group by exponents */
            /* First handle non-constant exponents */
            for(exponent_map::iterator
                i = by_exponent.begin();
                i != by_exponent.end();
                ++i)
            {
                exponent_list& list = i->second;
        #ifndef FP_MUL_COMBINE_EXPONENTS
                if(list.first.IsImmed())
                {
                    double exponent = list.first.GetImmed();
                    if(exponent == 0.0) continue;
                    if(FloatEqual(exponent, 1.0))
                    {
                        AddParamsMove(list.second);
                        continue;
                    }
                }
        #endif
                CodeTree mul;
                mul.SetOpcode(cMul);
                mul.SetParamsMove( list.second);
                mul.Rehash();

                if(has_highlevel_opcodes && list.first.IsImmed())
                {
                    if(list.first.GetImmed() == 0.5)
                    {
                        CodeTree sqrt;
                        sqrt.SetOpcode(cSqrt);
                        sqrt.AddParamMove(mul);
                        sqrt.Rehash();
                        AddParamMove(sqrt);
                        continue;
                    }
                    if(list.first.GetImmed() == -0.5)
                    {
                        CodeTree rsqrt;
                        rsqrt.SetOpcode(cRSqrt);
                        rsqrt.AddParamMove(mul);
                        rsqrt.Rehash();
                        AddParamMove(rsqrt);
                        continue;
                    }
                    if(list.first.GetImmed() == -1.0)
                    {
                        CodeTree inv;
                        inv.SetOpcode(cInv);
                        inv.AddParamMove(mul);
                        inv.Rehash();
                        AddParamMove(inv);
                        continue;
                    }
                }
                CodeTree pow;
                pow.SetOpcode(cPow);
                pow.AddParamMove(mul);
                pow.AddParamMove( list.first );
                pow.Rehash();
                AddParamMove(pow);
            }
        #ifdef FP_MUL_COMBINE_EXPONENTS
            by_exponent.clear();
            /* Then handle constant exponents */
            for(size_t a=0; a<by_float_exponent.data.size(); ++a)
            {
                double exponent = by_float_exponent.data[a].first;
                if(FloatEqual(exponent, 1.0))
                {
                    AddParamsMove(by_float_exponent.data[a].second);
                    continue;
                }
                CodeTree mul;
                mul.SetOpcode(cMul);
                mul.SetParamsMove( by_float_exponent.data[a].second );
                mul.Rehash();
                CodeTree pow;
                pow.SetOpcode(cPow);
                pow.AddParamMove(mul);
                pow.AddParam( CodeTree( exponent ) );
                pow.Rehash();
                AddParamMove(pow);
            }
        #endif
          #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "After ConstantFolding_MulGrouping: "; DumpTree(*this);
            std::cout << "\n";
          #endif
            // return true;
            return !IsIdenticalTo(before); // avoids infinite looping
        }
        return false;
    }

    bool CodeTree::ConstantFolding_AddGrouping()
    {
        bool should_regenerate = false;
        CollectionSet add;
        for(size_t a=0; a<GetParamCount(); ++a)
        {
            if(GetParam(a).GetOpcode() == cMul) continue;
            if(add.AddCollection(GetParam(a)) == CollectionSet::Suboptimal)
                should_regenerate = true;
            // This catches x + x and x - x
        }
        std::vector<bool> remaining ( GetParamCount() );
        size_t has_mulgroups_remaining = 0;
        for(size_t a=0; a<GetParamCount(); ++a)
        {
            const CodeTree& mulgroup = GetParam(a);
            if(mulgroup.GetOpcode() == cMul)
            {
                // This catches x + y*x*z, producing x*(1 + y*z)
                //
                // However we avoid changing 7 + 7*x into 7*(x+1),
                // because it may lead us into producing code such
                // as 20*x + 50*(x+1) + 10, which would be much
                // better expressed as 70*x + 60, and converting
                // back to that format would be needlessly hairy.
                for(size_t b=0; b<mulgroup.GetParamCount(); ++b)
                {
                    if(mulgroup.GetParam(b).IsImmed()) continue;
                    CollectionSet::PositionType c
                        = add.FindIdenticalValueTo(mulgroup.GetParam(b));
                    if(add.Found(c))
                    {
                        CodeTree tmp(mulgroup, CodeTree::CloneTag());
                        tmp.DelParam(b);
                        tmp.Rehash();
                        add.AddCollectionTo(tmp, c);
                        should_regenerate = true;
                        goto done_a;
                    }
                }
                remaining[a]  = true;
                has_mulgroups_remaining += 1;
            done_a:;
            }
        }

        if(has_mulgroups_remaining > 0)
        {
            if(has_mulgroups_remaining > 1) // is it possible to find a duplicate?
            {
                std::vector< std::pair<CodeTree, size_t> > occurance_counts;
                std::multimap<fphash_t, size_t> occurance_pos;
                bool found_dup = false;
                for(size_t a=0; a<GetParamCount(); ++a)
                    if(remaining[a])
                    {
                        // This catches x*a + x*b, producing x*(a+b)
                        for(size_t b=0; b<GetParam(a).GetParamCount(); ++b)
                        {
                            const CodeTree& p = GetParam(a).GetParam(b);
                            const fphash_t   p_hash = p.GetHash();
                            for(std::multimap<fphash_t, size_t>::const_iterator
                                i = occurance_pos.lower_bound(p_hash);
                                i != occurance_pos.end() && i->first == p_hash;
                                ++i)
                            {
                                if(occurance_counts[i->second].first.IsIdenticalTo(p))
                                {
                                    occurance_counts[i->second].second += 1;
                                    found_dup = true;
                                    goto found_mulgroup_item_dup;
                                }
                            }
                            occurance_counts.push_back(std::make_pair(p, size_t(1)));
                            occurance_pos.insert(std::make_pair(p_hash, occurance_counts.size()-1));
                        found_mulgroup_item_dup:;
                        }
                    }
                if(found_dup)
                {
                    // Find the "x" to group by
                    CodeTree group_by; { size_t max = 0;
                    for(size_t p=0; p<occurance_counts.size(); ++p)
                        if(occurance_counts[p].second <= 1)
                            occurance_counts[p].second = 0;
                        else
                        {
                            occurance_counts[p].second *= occurance_counts[p].first.GetDepth();
                            if(occurance_counts[p].second > max)
                                { group_by = occurance_counts[p].first; max = occurance_counts[p].second; }
                        } }
                    // Collect the items for adding in the group (a+b)
                    CodeTree group_add;
                    group_add.SetOpcode(cAdd);

        #ifdef DEBUG_SUBSTITUTIONS
                    std::cout << "Duplicate across some trees: ";
                    DumpTree(group_by);
                    std::cout << " in ";
                    DumpTree(*this);
                    std::cout << "\n";
        #endif
                    for(size_t a=0; a<GetParamCount(); ++a)
                        if(remaining[a])
                            for(size_t b=0; b<GetParam(a).GetParamCount(); ++b)
                                if(group_by.IsIdenticalTo(GetParam(a).GetParam(b)))
                                {
                                    CodeTree tmp(GetParam(a), CodeTree::CloneTag());
                                    tmp.DelParam(b);
                                    tmp.Rehash();
                                    group_add.AddParamMove(tmp);
                                    remaining[a] = false;
                                    break;
                                }
                    group_add.Rehash();
                    CodeTree group;
                    group.SetOpcode(cMul);
                    group.AddParamMove(group_by);
                    group.AddParamMove(group_add);
                    group.Rehash();
                    add.AddCollection(group);
                    should_regenerate = true;
                }
            }

            // all remaining mul-groups.
            for(size_t a=0; a<GetParamCount(); ++a)
                if(remaining[a])
                {
                    if(add.AddCollection(GetParam(a)) == CollectionSet::Suboptimal)
                        should_regenerate = true;
                }
        }

        if(should_regenerate)
        {
          #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "Before ConstantFolding_AddGrouping: "; DumpTree(*this);
            std::cout << "\n";
          #endif
            DelParams();

            for(CollectionSet::PositionType
                j = add.collections.begin();
                j != add.collections.end();
                ++j)
            {
                CodeTree& value = j->second.value;
                CodeTree& coeff = j->second.factor;
                if(j->second.factor_needs_rehashing) coeff.Rehash();

                if(coeff.IsImmed())
                {
                    if(coeff.GetImmed() == 0.0)
                        continue;
                    if(FloatEqual(coeff.GetImmed(), 1.0))
                    {
                        AddParamMove(value);
                        continue;
                    }
                }
                CodeTree mul;
                mul.SetOpcode(cMul);
                mul.AddParamMove(value);
                mul.AddParamMove(coeff);
                mul.Rehash();
                AddParamMove(mul);
            }
          #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "After ConstantFolding_AddGrouping: "; DumpTree(*this);
            std::cout << "\n";
          #endif
            return true;
        }
        return false;
    }

    bool CodeTree::ConstantFolding_IfOperations()
    {
        // If the If() condition begins with a cNot,
        // remove the cNot and swap the branches.
        for(;;)
        {
            if(GetParam(0).GetOpcode() == cNot)
                { SetOpcode(cIf); }
            else if(GetParam(0).GetOpcode() == cAbsNot)
                { SetOpcode(cAbsIf); }
            else break;
            GetParam(0).Become( GetParam(0).GetParam(0) );
            GetParam(1).swap(GetParam(2));
        }
        if(GetParam(0).GetOpcode() == cIf
        || GetParam(0).GetOpcode() == cAbsIf)
        {
            //     if(if(x, a,b), c,d)
            //  -> if(x, if(a, c,d), if(b, c,d))
            // when either a or b is constantly true/false
            CodeTree cond = GetParam(0);
            CodeTree truth_a;
            truth_a.SetOpcode(cond.GetOpcode() == cIf ? cNotNot : cAbsNotNot);
            truth_a.AddParam(cond.GetParam(1));
            truth_a.ConstantFolding();
            CodeTree truth_b;
            truth_b.SetOpcode(cond.GetOpcode() == cIf ? cNotNot : cAbsNotNot);
            truth_b.AddParam(cond.GetParam(2));
            truth_b.ConstantFolding();
            if(truth_a.IsImmed() || truth_b.IsImmed())
            {
                CodeTree then_tree;
                then_tree.SetOpcode(cond.GetOpcode());
                then_tree.AddParam(cond.GetParam(1));
                then_tree.AddParam(GetParam(1));
                then_tree.AddParam(GetParam(2));
                then_tree.Rehash();
                CodeTree else_tree;
                else_tree.SetOpcode(cond.GetOpcode());
                else_tree.AddParam(cond.GetParam(2));
                else_tree.AddParam(GetParam(1));
                else_tree.AddParam(GetParam(2));
                else_tree.Rehash();
                SetOpcode(cond.GetOpcode());
                SetParam(0, cond.GetParam(0));
                SetParamMove(1, then_tree);
                SetParamMove(2, else_tree);
                return true; // rerun cIf optimization
            }
        }
        if(GetParam(1).GetOpcode() == GetParam(2).GetOpcode()
        && (GetParam(1).GetOpcode() == cIf
         || GetParam(1).GetOpcode() == cAbsIf))
        {
            CodeTree& leaf1 = GetParam(1);
            CodeTree& leaf2 = GetParam(2);
            if(leaf1.GetParam(0).IsIdenticalTo(leaf2.GetParam(0))
            && (leaf1.GetParam(1).IsIdenticalTo(leaf2.GetParam(1))
             || leaf1.GetParam(2).IsIdenticalTo(leaf2.GetParam(2))))
            {
            //     if(x, if(y,a,b), if(y,c,d))
            // ->  if(y, if(x,a,c), if(x,b,d))
            // when either a,c are identical or b,d are identical
                CodeTree then_tree;
                then_tree.SetOpcode(GetOpcode());
                then_tree.AddParam(GetParam(0));
                then_tree.AddParam(leaf1.GetParam(1));
                then_tree.AddParam(leaf2.GetParam(1));
                then_tree.Rehash();
                CodeTree else_tree;
                else_tree.SetOpcode(GetOpcode());
                else_tree.AddParam(GetParam(0));
                else_tree.AddParam(leaf1.GetParam(2));
                else_tree.AddParam(leaf2.GetParam(2));
                else_tree.Rehash();
                SetOpcode(leaf1.GetOpcode());
                SetParam(0, leaf1.GetParam(0));
                SetParamMove(1, then_tree);
                SetParamMove(2, else_tree);
                return true; // rerun cIf optimization
            // cIf [x (cIf [y a z]) (cIf [y z b])] : (cXor x y) z (cIf[x a b])
            // ^ if only we had cXor opcode.
            }
            if(leaf1.GetParam(1).IsIdenticalTo(leaf2.GetParam(1))
            && leaf1.GetParam(2).IsIdenticalTo(leaf2.GetParam(2)))
            {
                //    if(x, if(y,a,b), if(z,a,b))
                // -> if( if(x, y,z), a,b)
                CodeTree cond_tree;
                cond_tree.SetOpcode(GetOpcode());
                cond_tree.AddParamMove(GetParam(0));
                cond_tree.AddParam(leaf1.GetParam(0));
                cond_tree.AddParam(leaf2.GetParam(0));
                cond_tree.Rehash();
                SetOpcode(leaf1.GetOpcode());
                SetParamMove(0, cond_tree);
                SetParam(2, leaf1.GetParam(2));
                SetParam(1, leaf1.GetParam(1));
                return true; // rerun cIf optimization
            }
            if(leaf1.GetParam(1).IsIdenticalTo(leaf2.GetParam(2))
            && leaf1.GetParam(2).IsIdenticalTo(leaf2.GetParam(1)))
            {
                //    if(x, if(y,a,b), if(z,b,a))
                // -> if( if(x, y,!z), a,b)
                CodeTree not_tree;
                not_tree.SetOpcode(leaf2.GetOpcode() == cIf ? cNot : cAbsNot);
                not_tree.AddParam(leaf2.GetParam(0));
                not_tree.Rehash();
                CodeTree cond_tree;
                cond_tree.SetOpcode(GetOpcode());
                cond_tree.AddParamMove(GetParam(0));
                cond_tree.AddParam(leaf1.GetParam(0));
                cond_tree.AddParamMove(not_tree);
                cond_tree.Rehash();
                SetOpcode(leaf1.GetOpcode());
                SetParamMove(0, cond_tree);
                SetParam(2, leaf1.GetParam(2));
                SetParam(1, leaf1.GetParam(1));
                return true; // rerun cIf optimization
            }
        }

        // If the sub-expression evaluates to approx. zero, yield param3.
        // If the sub-expression evaluates to approx. nonzero, yield param2.
        MinMaxTree p = GetParam(0).CalculateResultBoundaries();
        switch(GetLogicalValue(p, GetOpcode()==cAbsIf))
        {
            case 1: // true
                Become(GetParam(1));
                return true; // rerun optimization (opcode changed)
            case 0: // false
                Become(GetParam(2));
                return true; // rerun optimization (opcode changed)
            default: ;
        }

        CodeTree& branch1 = GetParam(1);
        CodeTree& branch2 = GetParam(2);

        if(branch1.IsIdenticalTo(branch2))
        {
            // If both branches of an If() are identical, the test becomes unnecessary
            Become(GetParam(1));
            return true; // rerun optimization (opcode changed)
        }

        const OPCODE op1 = branch1.GetOpcode();
        const OPCODE op2 = branch2.GetOpcode();
        if(op1 == op2)
        {
            // If both branches apply the same unary function to different values,
            // extract the function. E.g. if(x,sin(a),sin(b)) -> sin(if(x,a,b))
            if(branch1.GetParamCount() == 1)
            {
                CodeTree changed_if;
                changed_if.SetOpcode(GetOpcode());
                changed_if.AddParamMove(GetParam(0));
                changed_if.AddParam(branch1.GetParam(0));
                changed_if.AddParam(branch2.GetParam(0));
                changed_if.Rehash();
                SetOpcode(op1);
                DelParams();
                AddParamMove(changed_if);
                return true; // rerun optimization (opcode changed)
            }
            if(op1 == cAdd    || op1 == cMul
            || op1 == cAnd    || op1 == cOr
            || op1 == cAbsAnd || op1 == cAbsOr
            || op1 == cMin    || op1 == cMax)
            {
                // If the two groups contain one or more
                // identical values, extract them.
                std::vector<CodeTree> overlap;
                for(size_t a=branch1.GetParamCount(); a-- > 0; )
                {
                    for(size_t b=branch2.GetParamCount(); b-- > 0; )
                    {
                        if(branch1.GetParam(a).IsIdenticalTo(branch2.GetParam(b)))
                        {
                            if(overlap.empty()) { branch1.CopyOnWrite(); branch2.CopyOnWrite(); }
                            overlap.push_back(branch1.GetParam(a));
                            branch2.DelParam(b);
                            branch1.DelParam(a);
                            break;
                        }
                    }
                }
                if(!overlap.empty())
                {
                    branch1.Rehash();
                    branch2.Rehash();
                    CodeTree changed_if;
                    changed_if.SetOpcode(GetOpcode());
                    changed_if.SetParamsMove(GetParams());
                    changed_if.Rehash();
                    SetOpcode(op1);
                    SetParamsMove(overlap);
                    AddParamMove(changed_if);
                    return true; // rerun optimization (opcode changed)
                }
            }
        }
        // if(x, y+z, y) -> if(x, z,0)+y
        if(op1 == cAdd
        || op1 == cMul
        || (op1 == cAnd && branch2.IsLogicalValue())
        || (op1 == cOr  && branch2.IsLogicalValue())
          )
        {
            for(size_t a=branch1.GetParamCount(); a-- > 0; )
                if(branch1.GetParam(a).IsIdenticalTo(branch2))
                {
                    branch1.CopyOnWrite();
                    branch1.DelParam(a);
                    branch1.Rehash();
                    CodeTree branch2_backup = branch2;
                    branch2 = CodeTree( (op1==cAdd||op1==cOr) ? 0.0 : 1.0 );
                    CodeTree changed_if;
                    changed_if.SetOpcode(GetOpcode());
                    changed_if.SetParamsMove(GetParams());
                    changed_if.Rehash();
                    SetOpcode(op1);
                    AddParamMove(branch2_backup);
                    AddParamMove(changed_if);
                    return true; // rerun optimization (opcode changed)
                }
        }
        // if(x, y&z, !!y) -> if(x, z, 1) & y
        if((op1 == cAnd || op1 == cOr) && op2 == cNotNot)
        {
            CodeTree& branch2op = branch2.GetParam(0);
            for(size_t a=branch1.GetParamCount(); a-- > 0; )
                if(branch1.GetParam(a).IsIdenticalTo(branch2op))
                {
                    branch1.CopyOnWrite();
                    branch1.DelParam(a);
                    branch1.Rehash();
                    CodeTree branch2_backup = branch2op;
                    branch2 = CodeTree( (op1==cOr) ? 0.0 : 1.0 );
                    CodeTree changed_if;
                    changed_if.SetOpcode(GetOpcode());
                    changed_if.SetParamsMove(GetParams());
                    changed_if.Rehash();
                    SetOpcode(op1);
                    AddParamMove(branch2_backup);
                    AddParamMove(changed_if);
                    return true; // rerun optimization (opcode changed)
                }
        }
        // if(x, y, y+z) -> if(x, 0,z)+y
        if(op2 == cAdd
        || op2 == cMul
        || (op2 == cAnd && branch1.IsLogicalValue())
        || (op2 == cOr  && branch1.IsLogicalValue())
          )
        {
            for(size_t a=branch2.GetParamCount(); a-- > 0; )
                if(branch2.GetParam(a).IsIdenticalTo(branch1))
                {
                    branch2.CopyOnWrite();
                    branch2.DelParam(a);
                    branch2.Rehash();
                    CodeTree branch1_backup = branch1;
                    branch1 = CodeTree( (op2==cAdd||op2==cOr) ? 0.0 : 1.0 );
                    CodeTree changed_if;
                    changed_if.SetOpcode(GetOpcode());
                    changed_if.SetParamsMove(GetParams());
                    changed_if.Rehash();
                    SetOpcode(op2);
                    AddParamMove(branch1_backup);
                    AddParamMove(changed_if);
                    return true; // rerun optimization (opcode changed)
                }
        }
        // if(x, !!y, y&z) -> if(x, 1, z) & y
        if((op2 == cAnd || op2 == cOr) && op1 == cNotNot)
        {
            CodeTree& branch1op = branch1.GetParam(0);
            for(size_t a=branch2.GetParamCount(); a-- > 0; )
                if(branch2.GetParam(a).IsIdenticalTo(branch1op))
                {
                    branch2.CopyOnWrite();
                    branch2.DelParam(a);
                    branch2.Rehash();
                    CodeTree branch1_backup = branch1op;
                    branch1 = CodeTree( (op2==cOr) ? 0.0 : 1.0 );
                    CodeTree changed_if;
                    changed_if.SetOpcode(GetOpcode());
                    changed_if.SetParamsMove(GetParams());
                    changed_if.Rehash();
                    SetOpcode(op2);
                    AddParamMove(branch1_backup);
                    AddParamMove(changed_if);
                    return true; // rerun optimization (opcode changed)
                }
        }
        return false; // No changes
    }

    bool CodeTree::ConstantFolding_PowOperations()
    {
        if(GetParam(0).IsImmed()
        && GetParam(1).IsImmed())
        {
            double const_value = fp_pow(GetParam(0).GetImmed(),
                                        GetParam(1).GetImmed());
            data = new CodeTreeData(const_value);
            return false;
        }
        if(GetParam(1).IsImmed()
        && (float)GetParam(1).GetImmed() == 1.0)
        {
            // Conversion through a float type value gets rid of
            // awkward abs(x)^1 generated from exp(log(x^6)/6),
            // without sacrificing as much precision as FloatEqual() does.
            // x^1 = x
            Become(GetParam(0));
            return true; // rerun optimization (opcode changed)
        }
        if(GetParam(0).IsImmed()
        && (float)GetParam(0).GetImmed() == 1.0)
        {
            // 1^x = 1
            data = new CodeTreeData(1.0);
            return false;
        }

        // 5^(20*x) = (5^20)^x
        if(GetParam(0).IsImmed()
        && GetParam(1).GetOpcode() == cMul)
        {
            bool changes = false;
            double base_immed = GetParam(0).GetImmed();
            CodeTree mulgroup = GetParam(1);
            for(size_t a=mulgroup.GetParamCount(); a-->0; )
                if(mulgroup.GetParam(a).IsImmed())
                {
                    double imm = mulgroup.GetParam(a).GetImmed();
                    //if(imm >= 0.0)
                    {
                        double new_base_immed = fp_pow(base_immed, imm);
                        if(isinf(new_base_immed) || new_base_immed == 0.0)
                        {
                            // It produced an infinity. Do not change.
                            break;
                        }

                        if(!changes)
                        {
                            changes = true;
                            mulgroup.CopyOnWrite();
                        }
                        base_immed = new_base_immed;
                        mulgroup.DelParam(a);
                        break; //
                    }
                }
            if(changes)
            {
                mulgroup.Rehash();
            #ifdef DEBUG_SUBSTITUTIONS
                std::cout << "Before pow-mul change: "; DumpTree(*this);
                std::cout << "\n";
            #endif
                GetParam(0).Become(CodeTree(base_immed));
                GetParam(1).Become(mulgroup);
            #ifdef DEBUG_SUBSTITUTIONS
                std::cout << "After pow-mul change: "; DumpTree(*this);
                std::cout << "\n";
            #endif
            }
        }
        // (x*20)^2 = x^2 * 20^2
        if(GetParam(1).IsImmed()
        && GetParam(0).GetOpcode() == cMul)
        {
            double exponent_immed = GetParam(1).GetImmed();
            double factor_immed   = 1.0;
            bool changes = false;
            CodeTree& mulgroup = GetParam(0);
            for(size_t a=mulgroup.GetParamCount(); a-->0; )
                if(mulgroup.GetParam(a).IsImmed())
                {
                    double imm = mulgroup.GetParam(a).GetImmed();
                    //if(imm >= 0.0)
                    {
                        double new_factor_immed = fp_pow(imm, exponent_immed);
                        if(isinf(new_factor_immed) || new_factor_immed == 0.0)
                        {
                            // It produced an infinity. Do not change.
                            break;
                        }
                        if(!changes)
                        {
                            changes = true;
                            mulgroup.CopyOnWrite();
                        }
                        factor_immed *= new_factor_immed;
                        mulgroup.DelParam(a);
                        break; //
                    }
                }
            if(changes)
            {
                mulgroup.Rehash();
                CodeTree newpow;
                newpow.SetOpcode(cPow);
                newpow.SetParamsMove(GetParams());
                SetOpcode(cMul);
                AddParamMove(newpow);
                AddParam( CodeTree(factor_immed) );
                return true; // rerun optimization (opcode changed)
            }
        }

        // (x^3)^2 = x^6
        // NOTE: If 3 is even and 3*2 is not, x must be changed to abs(x).
        if(GetParam(0).GetOpcode() == cPow
        && GetParam(1).IsImmed()
        && GetParam(0).GetParam(1).IsImmed())
        {
            double a = GetParam(0).GetParam(1).GetImmed();
            double b = GetParam(1).GetImmed();
            double c = a * b; // new exponent
            if(IsEvenIntegerConst(a) // a is an even int?
            && !IsEvenIntegerConst(c)) // c is not?
            {
                CodeTree newbase;
                newbase.SetOpcode(cAbs);
                newbase.AddParam(GetParam(0).GetParam(0));
                newbase.Rehash();
                SetParamMove(0, newbase);
            }
            else
                SetParam(0, GetParam(0).GetParam(0));
            SetParam(1, CodeTree(c));
        }
        return false; // No changes that require a rerun
    }

    void CodeTree::ConstantFolding_ComparisonOperations()
    {
        static const RangeComparisonData Data[6] =
        {
            // cEqual:
            // Case:      p0 == p1  Antonym: p0 != p1
            // Synonym:   p1 == p0  Antonym: p1 != p0
            { RangeComparisonData::MakeTrue,  // If identical: always true
              {RangeComparisonData::MakeFalse,  // If Always p0 < p1: always false
               RangeComparisonData::Unchanged,
               RangeComparisonData::MakeFalse,  // If Always p0 > p1: always false
               RangeComparisonData::Unchanged}
            },
            // cNEqual:
            // Case:      p0 != p1  Antonym: p0 == p1
            // Synonym:   p1 != p0  Antonym: p1 == p0
            { RangeComparisonData::MakeFalse,  // If identical: always false
              {RangeComparisonData::MakeTrue,  // If Always p0 < p1: always true
               RangeComparisonData::Unchanged,
               RangeComparisonData::MakeTrue,  // If Always p0 > p1: always true
               RangeComparisonData::Unchanged}
            },
            // cLess:
            // Case:      p0 < p1   Antonym: p0 >= p1
            // Synonym:   p1 > p0   Antonym: p1 <= p0
            { RangeComparisonData::MakeFalse,  // If identical: always false
              {RangeComparisonData::MakeTrue,  // If Always p0  < p1: always true
               RangeComparisonData::MakeNEqual,
               RangeComparisonData::Unchanged,
               RangeComparisonData::MakeFalse} // If Always p0 >= p1: always false
            },
            // cLessOrEq:
            // Case:      p0 <= p1  Antonym: p0 > p1
            // Synonym:   p1 >= p0  Antonym: p1 < p0
            { RangeComparisonData::MakeTrue,   // If identical: always true
              {RangeComparisonData::Unchanged,
               RangeComparisonData::MakeTrue,  // If Always p0 <= p1: always true
               RangeComparisonData::MakeFalse, // If Always p0  > p1: always false
               RangeComparisonData::MakeEqual} // If Never  p0  < p1:  use cEqual
            },
            // cGreater:
            // Case:      p0 >  p1  Antonym: p0 <= p1
            // Synonym:   p1 <  p0  Antonym: p1 >= p0
            { RangeComparisonData::MakeFalse,  // If identical: always false
              {RangeComparisonData::Unchanged,
               RangeComparisonData::MakeFalse, // If Always p0 <= p1: always false
               RangeComparisonData::MakeTrue,  // If Always p0  > p1: always true
               RangeComparisonData::MakeNEqual}
            },
            // cGreaterOrEq:
            // Case:      p0 >= p1  Antonym: p0 < p1
            // Synonym:   p1 <= p0  Antonym: p1 > p0
            { RangeComparisonData::MakeTrue,   // If identical: always true
              {RangeComparisonData::MakeFalse, // If Always p0  < p1: always false
               RangeComparisonData::MakeEqual, // If Always p0 >= p1: always true
               RangeComparisonData::Unchanged,
               RangeComparisonData::MakeTrue}  // If Never  p0  > p1:  use cEqual
            }
        };
        switch(Data[GetOpcode()-cEqual].Analyze(GetParam(0), GetParam(1)))
        {
            case RangeComparisonData::MakeFalse:
                data = new CodeTreeData(0.0); break;
            case RangeComparisonData::MakeTrue:
                data = new CodeTreeData(1.0); break;
            case RangeComparisonData::MakeEqual:  SetOpcode(cEqual); break;
            case RangeComparisonData::MakeNEqual: SetOpcode(cNEqual); break;
            case RangeComparisonData::Unchanged:;
        }
    }

    bool CodeTree::ConstantFolding_Assimilate()
    {
        /* If the list contains another list of the same kind, assimilate it */
        bool assimilated = false;
        for(size_t a=GetParamCount(); a-- > 0; )
            if(GetParam(a).GetOpcode() == GetOpcode())
            {
              #ifdef DEBUG_SUBSTITUTIONS
                if(!assimilated)
                {
                    std::cout << "Before assimilation: "; DumpTree(*this);
                    std::cout << "\n";
                    assimilated = true;
                }
              #endif
                // Assimilate its children and remove it
                AddParamsMove(GetParam(a).GetUniqueRef().GetParams(), a);
            }
      #ifdef DEBUG_SUBSTITUTIONS
        if(assimilated)
        {
            std::cout << "After assimilation:   "; DumpTree(*this);
            std::cout << "\n";
        }
      #endif
        return assimilated;
    }

    void CodeTree::ConstantFolding()
    {
    #ifdef DEBUG_SUBSTITUTIONS
        std::cout << "Runs ConstantFolding for: "; DumpTree(*this);
        std::cout << "\n";
    #endif
        using namespace std;
    redo:;

        // Insert here any hardcoded constant-folding optimizations
        // that you want to be done whenever a new subtree is generated.
        /* Not recursive. */

        double const_value = 1.0;
        if(GetOpcode() != cImmed)
        {
            MinMaxTree p = CalculateResultBoundaries();
            if(p.has_min && p.has_max && p.min == p.max)
            {
                // Replace us with this immed
                const_value = p.min;
                goto ReplaceTreeWithConstValue;
            }
        }

        if(false)
        {
            ReplaceTreeWithOne:
                const_value = 1.0;
                goto ReplaceTreeWithConstValue;
            ReplaceTreeWithZero:
                const_value = 0.0;
            ReplaceTreeWithConstValue:
              #ifdef DEBUG_SUBSTITUTIONS
                std::cout << "Replacing "; DumpTree(*this);
                if(IsImmed())
                    std::cout << "(" << std::hex
                              << *(const uint_least64_t*)&GetImmed()
                              << std::dec << ")";
                std::cout << " with const value " << const_value;
                std::cout << "(" << std::hex
                          << *(const uint_least64_t*)&const_value
                          << std::dec << ")";
                std::cout << "\n";
              #endif
                data = new CodeTreeData(const_value);
                return;
            ReplaceTreeWithParam0:
              #ifdef DEBUG_SUBSTITUTIONS
                std::cout << "Before replace: "; DumpTree(*this);
                std::cout << "\n";
              #endif
                Become(GetParam(0));
              #ifdef DEBUG_SUBSTITUTIONS
                std::cout << "After replace: "; DumpTree(*this);
                std::cout << "\n";
              #endif
                goto redo;
        }

        /* Constant folding */
        switch(GetOpcode())
        {
            case cImmed:
                break; // nothing to do
            case cVar:
                break; // nothing to do

            case cAnd:
            case cAbsAnd:
            {
                ConstantFolding_Assimilate();
                for(size_t a=GetParamCount(); a-- > 0; )
                    switch(GetLogicalValue(GetParam(a).CalculateResultBoundaries(),
                                           GetOpcode()==cAbsAnd))
                    {
                        case 0: goto ReplaceTreeWithZero;
                        case 1: DelParam(a); break; // x & y & 1 = x & y;  x & 1 = !!x
                        default: ;
                    }
                switch(GetParamCount())
                {
                    case 0: goto ReplaceTreeWithOne;
                    case 1: SetOpcode(GetOpcode()==cAnd ? cNotNot : cAbsNotNot); goto redo; // Replace self with the single operand
                    default: if(GetOpcode()==cAnd) if(ConstantFolding_AndLogic()) goto redo;
                }
                break;
            }
            case cOr:
            case cAbsOr:
            {
                ConstantFolding_Assimilate();
                for(size_t a=GetParamCount(); a-- > 0; )
                    switch(GetLogicalValue(GetParam(a).CalculateResultBoundaries(),
                                           GetOpcode()==cAbsOr))
                    {
                        case 1: goto ReplaceTreeWithOne;
                        case 0: DelParam(a); break;
                        default: ;
                    }
                switch(GetParamCount())
                {
                    case 0: goto ReplaceTreeWithZero;
                    case 1: SetOpcode(GetOpcode()==cOr ? cNotNot : cAbsNotNot); goto redo; // Replace self with the single operand
                    default: if(GetOpcode()==cOr) if(ConstantFolding_OrLogic()) goto redo;
                }
                break;
            }
            case cNot:
            case cAbsNot:
            {
                unsigned opposite = 0;
                switch(GetParam(0).GetOpcode())
                {
                    case cEqual:       opposite = cNEqual; break;
                    case cNEqual:      opposite = cEqual; break;
                    case cLess:        opposite = cGreaterOrEq; break;
                    case cGreater:     opposite = cLessOrEq; break;
                    case cLessOrEq:    opposite = cGreater; break;
                    case cGreaterOrEq: opposite = cLess; break;
                    //cNotNot already handled by grammar: @L cNotNot
                    case cNot:         opposite = cNotNot; break;
                    case cAbsNot:      opposite = cAbsNotNot; break;
                    case cAbsNotNot:   opposite = cAbsNot; break;
                    default: break;
                }
                if(opposite)
                {
                    SetOpcode(OPCODE(opposite));
                    SetParamsMove(GetParam(0).GetUniqueRef().GetParams());
                    goto redo;
                }

                // If the sub-expression evaluates to approx. zero, yield one.
                // If the sub-expression evaluates to approx. nonzero, yield zero.
                switch(GetLogicalValue(GetParam(0).CalculateResultBoundaries(),
                                       GetOpcode()==cAbsNot))
                {
                    case 1: goto ReplaceTreeWithZero;
                    case 0: goto ReplaceTreeWithOne;
                    default: ;
                }
                break;
            }
            case cNotNot:
            case cAbsNotNot:
            {
                // The function of cNotNot is to protect a logical value from
                // changing. If the parameter is already a logical value,
                // then the cNotNot opcode is redundant.
                if(GetParam(0).IsLogicalValue())
                    goto ReplaceTreeWithParam0;

                // If the sub-expression evaluates to approx. zero, yield zero.
                // If the sub-expression evaluates to approx. nonzero, yield one.
                switch(GetLogicalValue(GetParam(0).CalculateResultBoundaries(),
                                       GetOpcode()==cAbsNotNot))
                {
                    case 0: goto ReplaceTreeWithZero;
                    case 1: goto ReplaceTreeWithOne;
                    default: ;
                }
                break;
            }
            case cIf:
            case cAbsIf:
            {
                if(ConstantFolding_IfOperations())
                    goto redo;
                break;
            }
            case cMul:
            {
            NowWeAreMulGroup: ;
                ConstantFolding_Assimilate();
                // If one sub-expression evalutes to exact zero, yield zero.
                double immed_product = 1.0;
                size_t n_immeds = 0; bool needs_resynth=false;
                for(size_t a=0; a<GetParamCount(); ++a)
                {
                    if(!GetParam(a).IsImmed()) continue;
                    // ^ Only check constant values
                    double immed = GetParam(a).GetImmed();
                    if(immed == 0.0) goto ReplaceTreeWithZero;
                    immed_product *= immed; ++n_immeds;
                }
                // Merge immeds.
                if(n_immeds > 1 || (n_immeds == 1 && FloatEqual(immed_product, 1.0)))
                    needs_resynth = true;
                if(needs_resynth)
                {
                    // delete immeds and add new ones
                #ifdef DEBUG_SUBSTITUTIONS
                    std::cout << "cMul: Will add new immed " << immed_product << "\n";
                #endif
                    for(size_t a=GetParamCount(); a-->0; )
                        if(GetParam(a).IsImmed())
                        {
                        #ifdef DEBUG_SUBSTITUTIONS
                            std::cout << " - For that, deleting immed " << GetParam(a).GetImmed();
                            std::cout << "\n";
                        #endif
                            DelParam(a);
                        }
                    if(!FloatEqual(immed_product, 1.0))
                        AddParam( CodeTree(immed_product) );
                }
                switch(GetParamCount())
                {
                    case 0: goto ReplaceTreeWithOne;
                    case 1: goto ReplaceTreeWithParam0; // Replace self with the single operand
                    default:
                        if(ConstantFolding_MulGrouping()) goto redo;
                        if(ConstantFolding_MulLogicItems()) goto redo;
                }
                break;
            }
            case cAdd:
            {
                ConstantFolding_Assimilate();
                double immed_sum = 0.0;
                size_t n_immeds = 0; bool needs_resynth=false;
                for(size_t a=0; a<GetParamCount(); ++a)
                {
                    if(!GetParam(a).IsImmed()) continue;
                    // ^ Only check constant values
                    double immed = GetParam(a).GetImmed();
                    immed_sum += immed; ++n_immeds;
                }
                // Merge immeds.
                if(n_immeds > 1 || (n_immeds == 1 && immed_sum == 0.0))
                    needs_resynth = true;
                if(needs_resynth)
                {
                    // delete immeds and add new ones
                #ifdef DEBUG_SUBSTITUTIONS
                    std::cout << "cAdd: Will add new immed " << immed_sum << "\n";
                    std::cout << "In: "; DumpTree(*this);
                    std::cout << "\n";
                #endif
                    for(size_t a=GetParamCount(); a-->0; )
                        if(GetParam(a).IsImmed())
                        {
                        #ifdef DEBUG_SUBSTITUTIONS
                            std::cout << " - For that, deleting immed " << GetParam(a).GetImmed();
                            std::cout << "\n";
                        #endif
                            DelParam(a);
                        }
                    if(!(immed_sum == 0.0))
                        AddParam( CodeTree(immed_sum) );
                }
                switch(GetParamCount())
                {
                    case 0: goto ReplaceTreeWithZero;
                    case 1: goto ReplaceTreeWithParam0; // Replace self with the single operand
                    default:
                        if(ConstantFolding_AddGrouping()) goto redo;
                        if(ConstantFolding_AddLogicItems()) goto redo;
                }
                break;
            }
            case cMin:
            {
                ConstantFolding_Assimilate();
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
                size_t preserve=0;
                MinMaxTree smallest_maximum;
                for(size_t a=0; a<GetParamCount(); ++a)
                {
                    MinMaxTree p = GetParam(a).CalculateResultBoundaries();
                    if(p.has_max && (!smallest_maximum.has_max || p.max < smallest_maximum.max))
                    {
                        smallest_maximum.max = p.max;
                        smallest_maximum.has_max = true;
                        preserve=a;
                }   }
                if(smallest_maximum.has_max)
                    for(size_t a=GetParamCount(); a-- > 0; )
                    {
                        MinMaxTree p = GetParam(a).CalculateResultBoundaries();
                        if(p.has_min && a != preserve && p.min >= smallest_maximum.max)
                            DelParam(a);
                    }
                //fprintf(stderr, "Remains: %u\n", (unsigned)GetParamCount());
                if(GetParamCount() == 1)
                {
                    // Replace self with the single operand
                    goto ReplaceTreeWithParam0;
                }
                break;
            }
            case cMax:
            {
                ConstantFolding_Assimilate();
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
                size_t preserve=0;
                MinMaxTree biggest_minimum;
                for(size_t a=0; a<GetParamCount(); ++a)
                {
                    MinMaxTree p = GetParam(a).CalculateResultBoundaries();
                    if(p.has_min && (!biggest_minimum.has_min || p.min > biggest_minimum.min))
                    {
                        biggest_minimum.min = p.min;
                        biggest_minimum.has_min = true;
                        preserve=a;
                }   }
                if(biggest_minimum.has_min)
                {
                    //fprintf(stderr, "Removing all where max < %g\n", biggest_minimum.min);
                    for(size_t a=GetParamCount(); a-- > 0; )
                    {
                        MinMaxTree p = GetParam(a).CalculateResultBoundaries();
                        if(p.has_max && a != preserve && p.max < biggest_minimum.min)
                        {
                            //fprintf(stderr, "Removing %g\n", p.max);
                            DelParam(a);
                        }
                    }
                }
                //fprintf(stderr, "Remains: %u\n", (unsigned)GetParamCount());
                if(GetParamCount() == 1)
                {
                    // Replace self with the single operand
                    goto ReplaceTreeWithParam0;
                }
                break;
            }

            case cEqual:
            case cNEqual:
            case cLess:
            case cGreater:
            case cLessOrEq:
            case cGreaterOrEq:
                ConstantFolding_ComparisonOperations();
                break;

            case cAbs:
            {
                /* If we know the operand is always positive, cAbs is redundant.
                 * If we know the operand is always negative, use actual negation.
                 */
                MinMaxTree p0 = GetParam(0).CalculateResultBoundaries();
                if(p0.has_min && p0.min >= 0.0)
                    goto ReplaceTreeWithParam0;
                if(p0.has_max && p0.max <= NEGATIVE_MAXIMUM)
                {
                    /* abs(negative) = negative*-1 */
                    SetOpcode(cMul);
                    AddParam( CodeTree(-1.0) );
                    /* The caller of ConstantFolding() will do Sort() and Rehash() next.
                     * Thus, no need to do it here. */
                    /* We were changed into a cMul group. Do cMul folding. */
                    goto NowWeAreMulGroup;
                }
                /* If the operand is a cMul group, find elements
                 * that are always positive and always negative,
                 * and move them out, e.g. abs(p*n*x*y) = p*(-n)*abs(x*y)
                 */
                if(GetParam(0).GetOpcode() == cMul)
                {
                    const CodeTree& p = GetParam(0);
                    std::vector<CodeTree> pos_set;
                    std::vector<CodeTree> neg_set;
                    for(size_t a=0; a<p.GetParamCount(); ++a)
                    {
                        p0 = p.GetParam(a).CalculateResultBoundaries();
                        if(p0.has_min && p0.min >= 0.0)
                            { pos_set.push_back(p.GetParam(a)); }
                        if(p0.has_max && p0.max <= NEGATIVE_MAXIMUM)
                            { neg_set.push_back(p.GetParam(a)); }
                    }
                #ifdef DEBUG_SUBSTITUTIONS
                    std::cout << "Abs: mul group has " << pos_set.size()
                              << " pos, " << neg_set.size() << "neg\n";
                #endif
                    if(!pos_set.empty() || !neg_set.empty())
                    {
                #ifdef DEBUG_SUBSTITUTIONS
                        std::cout << "AbsReplace-Before: ";
                        DumpTree(*this);
                        std::cout << "\n" << std::flush;
                        DumpHashes(*this, std::cout);
                #endif
                        CodeTree pclone;
                        pclone.SetOpcode(cMul);
                        for(size_t a=0; a<p.GetParamCount(); ++a)
                        {
                            p0 = p.GetParam(a).CalculateResultBoundaries();
                            if((p0.has_min && p0.min >= 0.0)
                            || (p0.has_max && p0.max <= NEGATIVE_MAXIMUM))
                                {/*pclone.DelParam(a);*/}
                            else
                                pclone.AddParam( p.GetParam(a) );
                            /* Here, p*n*x*y -> x*y.
                             * p is saved in pos_set[]
                             * n is saved in neg_set[]
                             */
                        }
                        pclone.Rehash();
                        CodeTree abs_mul;
                        abs_mul.SetOpcode(cAbs);
                        abs_mul.AddParamMove(pclone);
                        abs_mul.Rehash();
                        CodeTree mulgroup;
                        mulgroup.SetOpcode(cMul);
                        mulgroup.AddParamMove(abs_mul); // cAbs[whatever remains in p]
                        mulgroup.AddParamsMove(pos_set);
                        /* Now:
                         * mulgroup  = p * Abs(x*y)
                         */
                        if(!neg_set.empty())
                        {
                            if(neg_set.size() % 2)
                                mulgroup.AddParam( CodeTree(-1.0) );
                            mulgroup.AddParamsMove(neg_set);
                            /* Now:
                             * mulgroup = p * n * -1 * Abs(x*y)
                             */
                        }
                        Become(mulgroup);
                #ifdef DEBUG_SUBSTITUTIONS
                        std::cout << "AbsReplace-After: ";
                        DumpTree(*this, std::cout);
                        std::cout << "\n" << std::flush;
                        DumpHashes(*this, std::cout);
                #endif
                        /* We were changed into a cMul group. Do cMul folding. */
                        goto NowWeAreMulGroup;
                    }
                }
                break;
            }

            #define HANDLE_UNARY_CONST_FUNC(funcname) \
                if(GetParam(0).IsImmed()) \
                    { const_value = funcname(GetParam(0).GetImmed()); \
                      goto ReplaceTreeWithConstValue; }

            case cLog:
                HANDLE_UNARY_CONST_FUNC(log);
                if(GetParam(0).GetOpcode() == cPow)
                {
                    CodeTree pow = GetParam(0);
                    if(pow.GetParam(0).IsAlwaysSigned(true))  // log(posi ^ y) = y*log(posi)
                    {
                        pow.CopyOnWrite();
                        pow.SetOpcode(cLog);
                        SetOpcode(cMul);
                        AddParamMove(pow.GetParam(1));
                        pow.DelParam(1);
                        pow.Rehash();
                        SetParamMove(0, pow);
                        goto NowWeAreMulGroup;
                    }
                    if(pow.GetParam(1).IsAlwaysParity(false)) // log(x ^ even) = even*log(abs(x))
                    {
                        pow.CopyOnWrite();
                        CodeTree abs;
                        abs.SetOpcode(cAbs);
                        abs.AddParamMove(pow.GetParam(0));
                        abs.Rehash();
                        pow.SetOpcode(cLog);
                        SetOpcode(cMul);
                        pow.SetParamMove(0, abs);
                        AddParamMove(pow.GetParam(1));
                        pow.DelParam(1);
                        pow.Rehash();
                        SetParamMove(0, pow);
                        goto NowWeAreMulGroup;
                    }
                }
                else if(GetParam(0).GetOpcode() == cAbs)
                {
                    // log(abs(x^y)) = y*log(abs(x))
                    CodeTree pow = GetParam(0).GetParam(0);
                    if(pow.GetOpcode() == cPow)
                    {
                        pow.CopyOnWrite();
                        CodeTree abs;
                        abs.SetOpcode(cAbs);
                        abs.AddParamMove(pow.GetParam(0));
                        abs.Rehash();
                        pow.SetOpcode(cLog);
                        SetOpcode(cMul);
                        pow.SetParamMove(0, abs);
                        AddParamMove(pow.GetParam(1));
                        pow.DelParam(1);
                        pow.Rehash();
                        SetParamMove(0, pow);
                        goto NowWeAreMulGroup;
                    }
                }
                break;
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
            case cTrunc: HANDLE_UNARY_CONST_FUNC(fp_trunc); break;
            case cFloor: HANDLE_UNARY_CONST_FUNC(floor); break;
            case cSqrt: HANDLE_UNARY_CONST_FUNC(sqrt); break; // converted into cPow x 0.5
            case cExp: HANDLE_UNARY_CONST_FUNC(exp); break; // convered into cPow CONSTANT_E x
            case cInt: HANDLE_UNARY_CONST_FUNC(fp_int); break;
            case cLog2: HANDLE_UNARY_CONST_FUNC(fp_log2); break;
            case cLog10: HANDLE_UNARY_CONST_FUNC(fp_log10); break;

            case cLog2by:
                if(GetParam(0).IsImmed()
                && GetParam(1).IsImmed())
                    { const_value = fp_log2(GetParam(0).GetImmed()) * GetParam(1).GetImmed();
                      goto ReplaceTreeWithConstValue; }
                break;

            case cMod: /* Can more be done than this? */
                if(GetParam(0).IsImmed()
                && GetParam(1).IsImmed())
                    { const_value = fmod(GetParam(0).GetImmed(), GetParam(1).GetImmed());
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
                MinMaxTree p0 = GetParam(0).CalculateResultBoundaries();
                MinMaxTree p1 = GetParam(1).CalculateResultBoundaries();
                if(GetParam(0).IsImmed()
                && FloatEqual(GetParam(0).GetImmed(), 0.0))   // y == 0
                {
                    if(p1.has_max && p1.max < 0)              // y == 0 && x < 0
                        { const_value = CONSTANT_PI; goto ReplaceTreeWithConstValue; }
                    if(p1.has_min && p1.min >= 0.0)           // y == 0 && x >= 0.0
                        { const_value = 0.0; goto ReplaceTreeWithConstValue; }
                }
                if(GetParam(1).IsImmed()
                && FloatEqual(GetParam(1).GetImmed(), 0.0))   // x == 0
                {
                    if(p0.has_max && p0.max < 0)              // y < 0 && x == 0
                        { const_value = -CONSTANT_PIHALF; goto ReplaceTreeWithConstValue; }
                    if(p0.has_min && p0.min > 0)              // y > 0 && x == 0
                        { const_value =  CONSTANT_PIHALF; goto ReplaceTreeWithConstValue; }
                }
                if(GetParam(0).IsImmed()
                && GetParam(1).IsImmed())
                    { const_value = atan2(GetParam(0).GetImmed(),
                                          GetParam(1).GetImmed());
                      goto ReplaceTreeWithConstValue; }
                if((p1.has_min && p1.min > 0.0)               // p1 != 0.0
                || (p1.has_max && p1.max < NEGATIVE_MAXIMUM)) // become atan(p0 / p1)
                {
                    CodeTree pow_tree;
                    pow_tree.SetOpcode(cPow);
                    pow_tree.AddParamMove(GetParam(1));
                    pow_tree.AddParam(CodeTree(-1.0));
                    pow_tree.Rehash();
                    CodeTree div_tree;
                    div_tree.SetOpcode(cMul);
                    div_tree.AddParamMove(GetParam(0));
                    div_tree.AddParamMove(pow_tree);
                    div_tree.Rehash();
                    SetOpcode(cAtan);
                    SetParamMove(0, div_tree);
                    DelParam(1);
                }
                break;
            }

            case cPow:
            {
                if(ConstantFolding_PowOperations()) goto redo;
                break;
            }

            /* The following opcodes are processed by GenerateFrom()
             * within fpoptimizer_bytecode_to_codetree.cc and thus
             * they will never occur in the calling context for the
             * most of the parsing context. They may however occur
             * at the late phase, so we deal with them.
             */
            case cDiv: // converted into cPow y -1
                if(GetParam(0).IsImmed()
                && GetParam(1).IsImmed()
                && GetParam(1).GetImmed() != 0.0)
                    { const_value = GetParam(0).GetImmed() / GetParam(1).GetImmed();
                      goto ReplaceTreeWithConstValue; }
                break;
            case cInv: // converted into cPow y -1
                if(GetParam(0).IsImmed()
                && GetParam(0).GetImmed() != 0.0)
                    { const_value = 1.0 / GetParam(0).GetImmed();
                      goto ReplaceTreeWithConstValue; }
                // Note: Could use (mulgroup)^immed optimization from cPow
                break;
            case cSub: // converted into cMul y -1
                if(GetParam(0).IsImmed()
                && GetParam(1).IsImmed())
                    { const_value = GetParam(0).GetImmed() - GetParam(1).GetImmed();
                      goto ReplaceTreeWithConstValue; }
                break;
            case cNeg: // converted into cMul x -1
                if(GetParam(0).IsImmed())
                    { const_value = -GetParam(0).GetImmed();
                      goto ReplaceTreeWithConstValue; }
                break;
            case cRad: // converted into cMul x CONSTANT_RD
                if(GetParam(0).IsImmed())
                    { const_value = GetParam(0).GetImmed() * CONSTANT_RD;
                      goto ReplaceTreeWithConstValue; }
                break;
            case cDeg: // converted into cMul x CONSTANT_DR
                if(GetParam(0).IsImmed())
                    { const_value = GetParam(0).GetImmed() * CONSTANT_DR;
                      goto ReplaceTreeWithConstValue; }
                break;
            case cSqr: // converted into cMul x x
                if(GetParam(0).IsImmed())
                    { const_value = GetParam(0).GetImmed() * GetParam(0).GetImmed();
                      goto ReplaceTreeWithConstValue; }
                break;
            case cExp2: // converted into cPow 2.0 x
                HANDLE_UNARY_CONST_FUNC(fp_exp2); break;
            case cRSqrt: // converted into cPow x -0.5
                if(GetParam(0).IsImmed())
                    { const_value = 1.0 / sqrt(GetParam(0).GetImmed());
                      goto ReplaceTreeWithConstValue; }
                break;
            case cCot: // converted into cMul (cPow (cTan x) -1)
                if(GetParam(0).IsImmed())
                    { double tmp = tan(GetParam(0).GetImmed());
                      if(tmp != 0.0)
                      { const_value = 1.0 / tmp;
                        goto ReplaceTreeWithConstValue; } }
                break;
            case cSec: // converted into cMul (cPow (cCos x) -1)
                if(GetParam(0).IsImmed())
                    { double tmp = cos(GetParam(0).GetImmed());
                      if(tmp != 0.0)
                      { const_value = 1.0 / tmp;
                        goto ReplaceTreeWithConstValue; } }
                break;
            case cCsc: // converted into cMul (cPow (cSin x) -1)
                if(GetParam(0).IsImmed())
                    { double tmp = sin(GetParam(0).GetImmed());
                      if(tmp != 0.0)
                      { const_value = 1.0 / tmp;
                        goto ReplaceTreeWithConstValue; } }
                break;

            /* Opcodes that do not occur in the tree for other reasons */
            case cRDiv: // version of cDiv
            case cRSub: // version of cSub
            case cRPow: // version of cPow
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
    }
}

#endif
