#include "codetree.hh"
#include "optimize.hh"
#include "consts.hh"

#include <algorithm>

#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#include "rangeestimation.hh"
#include "constantfolding.hh"

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
    /***************************/
    /* LOGIC (AND, OR, NOT)    */
    /***************************/

    struct ComparisonSetBase
    {
        enum { Lt_Mask = 0x1,   // 1=less
               Eq_Mask = 0x2,   // 2=equal
               Le_Mask = 0x3,   // 1+2 = Less or Equal
               Gt_Mask = 0x4,   // 4=greater
               Ne_Mask = 0x5,   // 4+1 = Greater or Less, i.e. Not equal
               Ge_Mask = 0x6 }; // 4+2 = Greater or Equal
        static int Swap_Mask(int m) { return (m&Eq_Mask)
                                  | ((m&Lt_Mask) ? Gt_Mask : 0)
                                  | ((m&Gt_Mask) ? Lt_Mask : 0); }
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
    };

    template<typename Value_t>
    struct ComparisonSet: public ComparisonSetBase /* For optimizing And, Or */
    {
        struct Comparison
        {
            CodeTree<Value_t> a;
            CodeTree<Value_t> b;
            int relationship;

            Comparison() : a(),b(), relationship() {}
        };
        std::vector<Comparison> relationships;
        struct Item
        {
            CodeTree<Value_t> value;
            bool negated;

            Item() : value(), negated(false) {}
        };
        std::vector<Item> plain_set;
        int const_offset;

        ComparisonSet():
            relationships(),
            plain_set(),
            const_offset(0)
        {
        }

        RelationshipResult AddItem(const CodeTree<Value_t>& a, bool negated, ConditionType type)
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

        RelationshipResult AddRelationship(CodeTree<Value_t> a, CodeTree<Value_t> b, int reltype, ConditionType type)
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

    template<typename Value_t, typename CondType> /* ComparisonSet::ConditionType */
    bool ConstantFolding_LogicCommon(
        CodeTree<Value_t>& tree, CondType cond_type, bool is_logical)
    {
        bool should_regenerate = false;
        ComparisonSet<Value_t> comp;
        for(size_t a=0; a<tree.GetParamCount(); ++a)
        {
            typename ComparisonSetBase::RelationshipResult
                change = ComparisonSetBase::Ok;
            const CodeTree<Value_t>& atree = tree.GetParam(a);
            switch(atree.GetOpcode())
            {
                case cEqual:
                    change = comp.AddRelationship(atree.GetParam(0), atree.GetParam(1), ComparisonSetBase::Eq_Mask, cond_type);
                    break;
                case cNEqual:
                    change = comp.AddRelationship(atree.GetParam(0), atree.GetParam(1), ComparisonSetBase::Ne_Mask, cond_type);
                    break;
                case cLess:
                    change = comp.AddRelationship(atree.GetParam(0), atree.GetParam(1), ComparisonSetBase::Lt_Mask, cond_type);
                    break;
                case cLessOrEq:
                    change = comp.AddRelationship(atree.GetParam(0), atree.GetParam(1), ComparisonSetBase::Le_Mask, cond_type);
                    break;
                case cGreater:
                    change = comp.AddRelationship(atree.GetParam(0), atree.GetParam(1), ComparisonSetBase::Gt_Mask, cond_type);
                    break;
                case cGreaterOrEq:
                    change = comp.AddRelationship(atree.GetParam(0), atree.GetParam(1), ComparisonSetBase::Ge_Mask, cond_type);
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
                    tree.ReplaceWithImmed(0);
                    return true;
            ReplaceTreeWithOne:
                    tree.ReplaceWithImmed(1);
                    return true;
                case ComparisonSetBase::Ok: // ok
                    break;
                case ComparisonSetBase::BecomeZero: // whole set was invalidated
                    goto ReplaceTreeWithZero;
                case ComparisonSetBase::BecomeOne: // whole set was validated
                    goto ReplaceTreeWithOne;
                case ComparisonSetBase::Suboptimal: // something was changed
                    should_regenerate = true;
                    break;
            }
        }
        if(should_regenerate)
        {
          #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "Before ConstantFolding_LogicCommon: "; DumpTree(tree);
            std::cout << "\n";
          #endif

            if(is_logical)
            {
                tree.DelParams(); // delete all params
            }
            else
            {
                // Delete only logical params
                for(size_t a=tree.GetParamCount(); a-- > 0; )
                {
                    const CodeTree<Value_t>& atree = tree.GetParam(a);
                    if(atree.IsLogicalValue())
                        tree.DelParam(a);
                }
            }

            for(size_t a=0; a<comp.plain_set.size(); ++a)
            {
                if(comp.plain_set[a].negated)
                {
                    CodeTree<Value_t> r;
                    r.SetOpcode(cNot);
                    r.AddParamMove(comp.plain_set[a].value);
                    r.Rehash();
                    tree.AddParamMove(r);
                }
                else if(!is_logical)
                {
                    CodeTree<Value_t> r;
                    r.SetOpcode(cNotNot);
                    r.AddParamMove(comp.plain_set[a].value);
                    r.Rehash();
                    tree.AddParamMove(r);
                }
                else
                    tree.AddParamMove(comp.plain_set[a].value);
            }
            for(size_t a=0; a<comp.relationships.size(); ++a)
            {
                CodeTree<Value_t> r;
                r.SetOpcode(cNop); // dummy
                switch(comp.relationships[a].relationship)
                {
                    case ComparisonSetBase::Lt_Mask: r.SetOpcode( cLess ); break;
                    case ComparisonSetBase::Eq_Mask: r.SetOpcode( cEqual ); break;
                    case ComparisonSetBase::Gt_Mask: r.SetOpcode( cGreater ); break;
                    case ComparisonSetBase::Le_Mask: r.SetOpcode( cLessOrEq ); break;
                    case ComparisonSetBase::Ne_Mask: r.SetOpcode( cNEqual ); break;
                    case ComparisonSetBase::Ge_Mask: r.SetOpcode( cGreaterOrEq ); break;
                }
                r.AddParamMove(comp.relationships[a].a);
                r.AddParamMove(comp.relationships[a].b);
                r.Rehash();
                tree.AddParamMove(r);
            }
            if(comp.const_offset != 0)
                tree.AddParam( CodeTreeImmed( Value_t(comp.const_offset) ) );
          #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "After ConstantFolding_LogicCommon: "; DumpTree(tree);
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

    template<typename Value_t>
    bool ConstantFolding_AndLogic(CodeTree<Value_t>& tree)
    {
        return ConstantFolding_LogicCommon(tree, ComparisonSetBase::cond_and, true );
    }
    template<typename Value_t>
    bool ConstantFolding_OrLogic(CodeTree<Value_t>& tree)
    {
        return ConstantFolding_LogicCommon(tree, ComparisonSetBase::cond_or, true );
    }
    template<typename Value_t>
    bool ConstantFolding_AddLogicItems(CodeTree<Value_t>& tree)
    {
        return ConstantFolding_LogicCommon(tree, ComparisonSetBase::cond_add, false );
    }
    template<typename Value_t>
    bool ConstantFolding_MulLogicItems(CodeTree<Value_t>& tree)
    {
        return ConstantFolding_LogicCommon(tree, ComparisonSetBase::cond_mul, false );
    }


    /**************************************/
    /* GROUPING OF COMMON FACTORS / TERMS */
    /**************************************/

    struct CollectionSetBase
    {
        enum CollectionResult
        {
            Ok,
            Suboptimal
        };
    };

    template<typename Value_t>
    struct CollectionSet: public CollectionSetBase /* For optimizing Add,  Mul */
    {
        struct Collection
        {
            CodeTree<Value_t> value;
            CodeTree<Value_t> factor;
            bool factor_needs_rehashing;

            Collection() : value(),factor(), factor_needs_rehashing(false) { }
            Collection(const CodeTree<Value_t>& v, const CodeTree<Value_t>& f)
                : value(v), factor(f), factor_needs_rehashing(false) { }
        };
        std::multimap<fphash_t, Collection> collections;

        typedef typename std::multimap<fphash_t, Collection>::iterator
            PositionType;

        CollectionSet() : collections() {}

        PositionType FindIdenticalValueTo(const CodeTree<Value_t>& value)
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

        CollectionResult AddCollectionTo(const CodeTree<Value_t>& factor,
                                         const PositionType& into_which)
        {
            Collection& c = into_which->second;
            if(c.factor_needs_rehashing)
                c.factor.AddParam(factor);
            else
            {
                CodeTree<Value_t> add;
                add.SetOpcode(cAdd);
                add.AddParamMove(c.factor);
                add.AddParam(factor);
                c.factor.swap(add);
                c.factor_needs_rehashing = true;
            }
            return Suboptimal;
        }

        CollectionResult AddCollection(const CodeTree<Value_t>& value, const CodeTree<Value_t>& factor)
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

        CollectionResult AddCollection(const CodeTree<Value_t>& a)
        {
            return AddCollection(a, CodeTreeImmed(Value_t(1)) );
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

    template<typename Value_t>
    bool IsEvenIntegerConst(const Value_t& v)
    {
        return IsIntegerConst(v) && ((long)v % 2) == 0;
    }

    template<typename Value_t>
    struct ConstantExponentCollection
    {
        typedef std::pair<Value_t, std::vector<CodeTree<Value_t> > > ExponentInfo;
        std::vector<ExponentInfo> data;

        ConstantExponentCollection(): data(){}

        void MoveToSet_Unique(Value_t exponent, std::vector<CodeTree<Value_t> >& source_set)
        {
            data.push_back( std::pair<Value_t, std::vector<CodeTree<Value_t> > >
                            (exponent, std::vector<CodeTree<Value_t> >() ) );
            data.back().second.swap(source_set);
        }
        void MoveToSet_NonUnique(Value_t exponent, std::vector<CodeTree<Value_t> >& source_set)
        {
            typename std::vector<ExponentInfo>::iterator i
                = std::lower_bound(data.begin(), data.end(), exponent, Select1st());
            if(i != data.end() && i->first == exponent)
            {
                i->second.insert(i->second.end(), source_set.begin(), source_set.end());
            }
            else
            {
                //MoveToSet_Unique(exponent, source_set);
                data.insert(i,  std::pair<Value_t, std::vector<CodeTree<Value_t> > >
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
                Value_t exp_a = data[a].first;
                if(fp_equal(exp_a, Value_t(1))) continue;
                for(size_t b=a+1; b<data.size(); ++b)
                {
                    Value_t exp_b = data[b].first;
                    Value_t exp_diff = exp_b - exp_a;
                    if(exp_diff >= fp_abs(exp_a)) break;
                    Value_t exp_diff_still_probable_integer = exp_diff * Value_t(16);
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
                        std::vector<CodeTree<Value_t> >& a_set = data[a].second;
                        std::vector<CodeTree<Value_t> >& b_set = data[b].second;
          #ifdef DEBUG_SUBSTITUTIONS
                        std::cout << "Before ConstantExponentCollection iteration:\n";
                        Dump(std::cout);
          #endif
                        if(IsIntegerConst(exp_b)
                        && IsEvenIntegerConst(exp_b)
                        //&& !IsEvenIntegerConst(exp_diff)
                        && !IsEvenIntegerConst(exp_diff+exp_a))
                        {
                            CodeTree<Value_t> tmp2;
                            tmp2.SetOpcode(cMul);
                            tmp2.SetParamsMove(b_set);
                            tmp2.Rehash();
                            CodeTree<Value_t> tmp;
                            tmp.SetOpcode(cAbs);
                            tmp.AddParamMove(tmp2);
                            tmp.Rehash();
                            b_set.resize(1);
                            b_set[0].swap(tmp);
                        }

                        a_set.insert(a_set.end(), b_set.begin(), b_set.end());

                        std::vector<CodeTree<Value_t> > b_copy = b_set;
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

    template<typename Value_t>
    static CodeTree<Value_t> CollectMulGroup_Item(
        CodeTree<Value_t>& value,
        bool& has_highlevel_opcodes)
    {
        switch(value.GetOpcode())
        {
            case cPow:
            {
                CodeTree<Value_t> exponent = value.GetParam(1);
                value.Become( value.GetParam(0) );
                return exponent;
            }
            /* - disabled to avoid clashes with powi
            case cCbrt:
                value.Become( value.GetParam(0) );
                has_highlevel_opcodes = true;
                return CodeTreeImmed( Value_t(1) / Value_t(3) );
            case cSqrt:
                value.Become( value.GetParam(0) );
                has_highlevel_opcodes = true;
                return CodeTreeImmed( Value_t(0.5) );
            */
            case cRSqrt:
                value.Become( value.GetParam(0) );
                has_highlevel_opcodes = true;
                return CodeTreeImmed( Value_t(-0.5) );
            case cInv:
                value.Become( value.GetParam(0) );
                has_highlevel_opcodes = true;
                return CodeTreeImmed( Value_t(-1) );
            default: break;
        }
        return CodeTreeImmed( Value_t(1) );
    }

    template<typename Value_t>
    static void CollectMulGroup(
        CollectionSet<Value_t>& mul,
        const CodeTree<Value_t>& tree,
        const CodeTree<Value_t>& factor,
        bool& should_regenerate,
        bool& has_highlevel_opcodes
    )
    {
        for(size_t a=0; a<tree.GetParamCount(); ++a)
        {
            CodeTree<Value_t> value(tree.GetParam(a));

            CodeTree<Value_t> exponent ( CollectMulGroup_Item(value, has_highlevel_opcodes) );

            if(!factor.IsImmed() || factor.GetImmed() != 1.0)
            {
                CodeTree<Value_t> new_exp;
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
                        CodeTree<Value_t> val(value.GetParam(b));
                        CodeTree<Value_t> exp(CollectMulGroup_Item(val, tmp));
                        if(exponent_is_even
                        || (exp.IsImmed() && IsEvenIntegerConst(exp.GetImmed())))
                        {
                            CodeTree<Value_t> new_exp;
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
                if(mul.AddCollection(value, exponent) == CollectionSetBase::Suboptimal)
                    should_regenerate = true;
            }
        }
    }

    template<typename Value_t>
    bool ConstantFolding_MulGrouping(CodeTree<Value_t>& tree)
    {
        bool has_highlevel_opcodes = false;
        bool should_regenerate = false;
        CollectionSet<Value_t> mul;

        CollectMulGroup(mul, tree, CodeTreeImmed(Value_t(1)),
                        should_regenerate,
                        has_highlevel_opcodes);

        typedef std::pair<CodeTree<Value_t>/*exponent*/,
                          std::vector<CodeTree<Value_t> >/*base value (mul group)*/
                         > exponent_list;
        typedef std::multimap<fphash_t,/*exponent hash*/
                              exponent_list> exponent_map;
        exponent_map by_exponent;

        for(typename CollectionSet<Value_t>::PositionType
            j = mul.collections.begin();
            j != mul.collections.end();
            ++j)
        {
            CodeTree<Value_t>& value = j->second.value;
            CodeTree<Value_t>& exponent = j->second.factor;
            if(j->second.factor_needs_rehashing) exponent.Rehash();
            const fphash_t exponent_hash = exponent.GetHash();

            typename exponent_map::iterator i = by_exponent.lower_bound(exponent_hash);
            for(; i != by_exponent.end() && i->first == exponent_hash; ++i)
                if(i->second.first.IsIdenticalTo(exponent))
                {
                    if(!exponent.IsImmed() || !fp_equal(exponent.GetImmed(), Value_t(1)))
                        should_regenerate = true;
                    i->second.second.push_back(value);
                    goto skip_b;
                }
            by_exponent.insert(i, std::make_pair(exponent_hash,
                std::make_pair(exponent,
                               std::vector<CodeTree<Value_t> > (size_t(1), value)
                              )));
        skip_b:;
        }

    #ifdef FP_MUL_COMBINE_EXPONENTS
        ConstantExponentCollection<Value_t> by_float_exponent;
        for(typename exponent_map::iterator
            j,i = by_exponent.begin();
            i != by_exponent.end();
            i=j)
        {
            j=i; ++j;
            exponent_list& list = i->second;
            if(list.first.IsImmed())
            {
                Value_t exponent = list.first.GetImmed();
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
            CodeTree<Value_t> before = tree;
            before.CopyOnWrite();

          #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "Before ConstantFolding_MulGrouping: "; DumpTree(before);
            std::cout << "\n";
          #endif
            tree.DelParams();

            /* Group by exponents */
            /* First handle non-constant exponents */
            for(typename exponent_map::iterator
                i = by_exponent.begin();
                i != by_exponent.end();
                ++i)
            {
                exponent_list& list = i->second;
        #ifndef FP_MUL_COMBINE_EXPONENTS
                if(list.first.IsImmed())
                {
                    Value_t exponent = list.first.GetImmed();
                    if(exponent == 0.0) continue;
                    if(FloatEqual(exponent, 1.0))
                    {
                        tree.AddParamsMove(list.second);
                        continue;
                    }
                }
        #endif
                CodeTree<Value_t> mul;
                mul.SetOpcode(cMul);
                mul.SetParamsMove( list.second);
                mul.Rehash();

                if(has_highlevel_opcodes && list.first.IsImmed())
                {
                    if(list.first.GetImmed() == 1.0 / 3.0)
                    {
                        CodeTree<Value_t> cbrt;
                        cbrt.SetOpcode(cCbrt);
                        cbrt.AddParamMove(mul);
                        cbrt.Rehash();
                        tree.AddParamMove(cbrt);
                        continue;
                    }
                    if(list.first.GetImmed() == 0.5)
                    {
                        CodeTree<Value_t> sqrt;
                        sqrt.SetOpcode(cSqrt);
                        sqrt.AddParamMove(mul);
                        sqrt.Rehash();
                        tree.AddParamMove(sqrt);
                        continue;
                    }
                    if(list.first.GetImmed() == -0.5)
                    {
                        CodeTree<Value_t> rsqrt;
                        rsqrt.SetOpcode(cRSqrt);
                        rsqrt.AddParamMove(mul);
                        rsqrt.Rehash();
                        tree.AddParamMove(rsqrt);
                        continue;
                    }
                    if(list.first.GetImmed() == -1.0)
                    {
                        CodeTree<Value_t> inv;
                        inv.SetOpcode(cInv);
                        inv.AddParamMove(mul);
                        inv.Rehash();
                        tree.AddParamMove(inv);
                        continue;
                    }
                }
                CodeTree<Value_t> pow;
                pow.SetOpcode(cPow);
                pow.AddParamMove(mul);
                pow.AddParamMove( list.first );
                pow.Rehash();
                tree.AddParamMove(pow);
            }
        #ifdef FP_MUL_COMBINE_EXPONENTS
            by_exponent.clear();
            /* Then handle constant exponents */
            for(size_t a=0; a<by_float_exponent.data.size(); ++a)
            {
                Value_t exponent = by_float_exponent.data[a].first;
                if(fp_equal(exponent, Value_t(1)))
                {
                    tree.AddParamsMove(by_float_exponent.data[a].second);
                    continue;
                }
                CodeTree<Value_t> mul;
                mul.SetOpcode(cMul);
                mul.SetParamsMove( by_float_exponent.data[a].second );
                mul.Rehash();
                CodeTree<Value_t> pow;
                pow.SetOpcode(cPow);
                pow.AddParamMove(mul);
                pow.AddParam( CodeTreeImmed( exponent ) );
                pow.Rehash();
                tree.AddParamMove(pow);
            }
        #endif
          #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "After ConstantFolding_MulGrouping: "; DumpTree(tree);
            std::cout << "\n";
          #endif
            // return true;
            return !tree.IsIdenticalTo(before); // avoids infinite looping
        }
        return false;
    }

    template<typename Value_t>
    bool ConstantFolding_AddGrouping(CodeTree<Value_t>& tree)
    {
        bool should_regenerate = false;
        CollectionSet<Value_t> add;
        for(size_t a=0; a<tree.GetParamCount(); ++a)
        {
            if(tree.GetParam(a).GetOpcode() == cMul) continue;
            if(add.AddCollection(tree.GetParam(a)) == CollectionSetBase::Suboptimal)
                should_regenerate = true;
            // This catches x + x and x - x
        }
        std::vector<bool> remaining ( tree.GetParamCount() );
        size_t has_mulgroups_remaining = 0;
        for(size_t a=0; a<tree.GetParamCount(); ++a)
        {
            const CodeTree<Value_t>& mulgroup = tree.GetParam(a);
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
                    typename CollectionSet<Value_t>::PositionType c
                        = add.FindIdenticalValueTo(mulgroup.GetParam(b));
                    if(add.Found(c))
                    {
                        CodeTree<Value_t> tmp(mulgroup, typename CodeTree<Value_t>::CloneTag());
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
                std::vector< std::pair<CodeTree<Value_t>, size_t> > occurance_counts;
                std::multimap<fphash_t, size_t> occurance_pos;
                bool found_dup = false;
                for(size_t a=0; a<tree.GetParamCount(); ++a)
                    if(remaining[a])
                    {
                        // This catches x*a + x*b, producing x*(a+b)
                        for(size_t b=0; b<tree.GetParam(a).GetParamCount(); ++b)
                        {
                            const CodeTree<Value_t>& p = tree.GetParam(a).GetParam(b);
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
                    CodeTree<Value_t> group_by; { size_t max = 0;
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
                    CodeTree<Value_t> group_add;
                    group_add.SetOpcode(cAdd);

        #ifdef DEBUG_SUBSTITUTIONS
                    std::cout << "Duplicate across some trees: ";
                    DumpTree(group_by);
                    std::cout << " in ";
                    DumpTree(tree);
                    std::cout << "\n";
        #endif
                    for(size_t a=0; a<tree.GetParamCount(); ++a)
                        if(remaining[a])
                            for(size_t b=0; b<tree.GetParam(a).GetParamCount(); ++b)
                                if(group_by.IsIdenticalTo(tree.GetParam(a).GetParam(b)))
                                {
                                    CodeTree<Value_t> tmp(tree.GetParam(a), typename CodeTree<Value_t>::CloneTag());
                                    tmp.DelParam(b);
                                    tmp.Rehash();
                                    group_add.AddParamMove(tmp);
                                    remaining[a] = false;
                                    break;
                                }
                    group_add.Rehash();
                    CodeTree<Value_t> group;
                    group.SetOpcode(cMul);
                    group.AddParamMove(group_by);
                    group.AddParamMove(group_add);
                    group.Rehash();
                    add.AddCollection(group);
                    should_regenerate = true;
                }
            }

            // all remaining mul-groups.
            for(size_t a=0; a<tree.GetParamCount(); ++a)
                if(remaining[a])
                {
                    if(add.AddCollection(tree.GetParam(a)) == CollectionSetBase::Suboptimal)
                        should_regenerate = true;
                }
        }

        if(should_regenerate)
        {
          #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "Before ConstantFolding_AddGrouping: "; DumpTree(tree);
            std::cout << "\n";
          #endif
            tree.DelParams();

            for(typename CollectionSet<Value_t>::PositionType
                j = add.collections.begin();
                j != add.collections.end();
                ++j)
            {
                CodeTree<Value_t>& value = j->second.value;
                CodeTree<Value_t>& coeff = j->second.factor;
                if(j->second.factor_needs_rehashing) coeff.Rehash();

                if(coeff.IsImmed())
                {
                    if(fp_equal(coeff.GetImmed(), Value_t(0)))
                        continue;
                    if(fp_equal(coeff.GetImmed(), Value_t(1)))
                    {
                        tree.AddParamMove(value);
                        continue;
                    }
                }
                CodeTree<Value_t> mul;
                mul.SetOpcode(cMul);
                mul.AddParamMove(value);
                mul.AddParamMove(coeff);
                mul.Rehash();
                tree.AddParamMove(mul);
            }
          #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "After ConstantFolding_AddGrouping: "; DumpTree(tree);
            std::cout << "\n";
          #endif
            return true;
        }
        return false;
    }

    /**************************************/
    /* IF OPERATIONS                      */
    /**************************************/

    template<typename Value_t>
    bool ConstantFolding_IfOperations(CodeTree<Value_t>& tree)
    {
        // If the If() condition begins with a cNot,
        // remove the cNot and swap the branches.
        for(;;)
        {
            if(tree.GetParam(0).GetOpcode() == cNot)
            {
                tree.SetOpcode(cIf);
                tree.GetParam(0).Become( tree.GetParam(0).GetParam(0) );
                tree.GetParam(1).swap(tree.GetParam(2));
            }
            else if(tree.GetParam(0).GetOpcode() == cAbsNot)
            {
                tree.SetOpcode(cAbsIf);
                tree.GetParam(0).Become( tree.GetParam(0).GetParam(0) );
                tree.GetParam(1).swap(tree.GetParam(2));
            }
            else break;
        }
        if(tree.GetParam(0).GetOpcode() == cIf
        || tree.GetParam(0).GetOpcode() == cAbsIf)
        {
            //     if(if(x, a,b), c,d)
            //  -> if(x, if(a, c,d), if(b, c,d))
            // when either a or b is constantly true/false
            CodeTree<Value_t> cond = tree.GetParam(0);
            CodeTree<Value_t> truth_a;
            truth_a.SetOpcode(cond.GetOpcode() == cIf ? cNotNot : cAbsNotNot);
            truth_a.AddParam(cond.GetParam(1));
            ConstantFolding(truth_a);
            CodeTree<Value_t> truth_b;
            truth_b.SetOpcode(cond.GetOpcode() == cIf ? cNotNot : cAbsNotNot);
            truth_b.AddParam(cond.GetParam(2));
            ConstantFolding(truth_b);
            if(truth_a.IsImmed() || truth_b.IsImmed())
            {
                CodeTree<Value_t> then_tree;
                then_tree.SetOpcode(cond.GetOpcode());
                then_tree.AddParam(cond.GetParam(1));
                then_tree.AddParam(tree.GetParam(1));
                then_tree.AddParam(tree.GetParam(2));
                then_tree.Rehash();
                CodeTree<Value_t> else_tree;
                else_tree.SetOpcode(cond.GetOpcode());
                else_tree.AddParam(cond.GetParam(2));
                else_tree.AddParam(tree.GetParam(1));
                else_tree.AddParam(tree.GetParam(2));
                else_tree.Rehash();
                tree.SetOpcode(cond.GetOpcode());
                tree.SetParam(0, cond.GetParam(0));
                tree.SetParamMove(1, then_tree);
                tree.SetParamMove(2, else_tree);
                return true; // rerun cIf optimization
            }
        }
        if(tree.GetParam(1).GetOpcode() == tree.GetParam(2).GetOpcode()
        && (tree.GetParam(1).GetOpcode() == cIf
         || tree.GetParam(1).GetOpcode() == cAbsIf))
        {
            CodeTree<Value_t>& leaf1 = tree.GetParam(1);
            CodeTree<Value_t>& leaf2 = tree.GetParam(2);
            if(leaf1.GetParam(0).IsIdenticalTo(leaf2.GetParam(0))
            && (leaf1.GetParam(1).IsIdenticalTo(leaf2.GetParam(1))
             || leaf1.GetParam(2).IsIdenticalTo(leaf2.GetParam(2))))
            {
            //     if(x, if(y,a,b), if(y,c,d))
            // ->  if(y, if(x,a,c), if(x,b,d))
            // when either a,c are identical or b,d are identical
                CodeTree<Value_t> then_tree;
                then_tree.SetOpcode(tree.GetOpcode());
                then_tree.AddParam(tree.GetParam(0));
                then_tree.AddParam(leaf1.GetParam(1));
                then_tree.AddParam(leaf2.GetParam(1));
                then_tree.Rehash();
                CodeTree<Value_t> else_tree;
                else_tree.SetOpcode(tree.GetOpcode());
                else_tree.AddParam(tree.GetParam(0));
                else_tree.AddParam(leaf1.GetParam(2));
                else_tree.AddParam(leaf2.GetParam(2));
                else_tree.Rehash();
                tree.SetOpcode(leaf1.GetOpcode());
                tree.SetParam(0, leaf1.GetParam(0));
                tree.SetParamMove(1, then_tree);
                tree.SetParamMove(2, else_tree);
                return true; // rerun cIf optimization
            // cIf [x (cIf [y a z]) (cIf [y z b])] : (cXor x y) z (cIf[x a b])
            // ^ if only we had cXor opcode.
            }
            if(leaf1.GetParam(1).IsIdenticalTo(leaf2.GetParam(1))
            && leaf1.GetParam(2).IsIdenticalTo(leaf2.GetParam(2)))
            {
                //    if(x, if(y,a,b), if(z,a,b))
                // -> if( if(x, y,z), a,b)
                CodeTree<Value_t> cond_tree;
                cond_tree.SetOpcode(tree.GetOpcode());
                cond_tree.AddParamMove(tree.GetParam(0));
                cond_tree.AddParam(leaf1.GetParam(0));
                cond_tree.AddParam(leaf2.GetParam(0));
                cond_tree.Rehash();
                tree.SetOpcode(leaf1.GetOpcode());
                tree.SetParamMove(0, cond_tree);
                tree.SetParam(2, leaf1.GetParam(2));
                tree.SetParam(1, leaf1.GetParam(1));
                return true; // rerun cIf optimization
            }
            if(leaf1.GetParam(1).IsIdenticalTo(leaf2.GetParam(2))
            && leaf1.GetParam(2).IsIdenticalTo(leaf2.GetParam(1)))
            {
                //    if(x, if(y,a,b), if(z,b,a))
                // -> if( if(x, y,!z), a,b)
                CodeTree<Value_t> not_tree;
                not_tree.SetOpcode(leaf2.GetOpcode() == cIf ? cNot : cAbsNot);
                not_tree.AddParam(leaf2.GetParam(0));
                not_tree.Rehash();
                CodeTree<Value_t> cond_tree;
                cond_tree.SetOpcode(tree.GetOpcode());
                cond_tree.AddParamMove(tree.GetParam(0));
                cond_tree.AddParam(leaf1.GetParam(0));
                cond_tree.AddParamMove(not_tree);
                cond_tree.Rehash();
                tree.SetOpcode(leaf1.GetOpcode());
                tree.SetParamMove(0, cond_tree);
                tree.SetParam(2, leaf1.GetParam(2));
                tree.SetParam(1, leaf1.GetParam(1));
                return true; // rerun cIf optimization
            }
        }

        // If the sub-expression evaluates to approx. zero, yield param3.
        // If the sub-expression evaluates to approx. nonzero, yield param2.
        MinMaxTree<Value_t> p = CalculateResultBoundaries( tree.GetParam(0) );
        switch(GetLogicalValue(p, tree.GetOpcode()==cAbsIf))
        {
            case 1: // true
                tree.Become(tree.GetParam(1));
                return true; // rerun optimization (opcode changed)
            case 0: // false
                tree.Become(tree.GetParam(2));
                return true; // rerun optimization (opcode changed)
            default: ;
        }

        CodeTree<Value_t>& branch1 = tree.GetParam(1);
        CodeTree<Value_t>& branch2 = tree.GetParam(2);

        if(branch1.IsIdenticalTo(branch2))
        {
            // If both branches of an If() are identical, the test becomes unnecessary
            tree.Become(tree.GetParam(1));
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
                CodeTree<Value_t> changed_if;
                changed_if.SetOpcode(tree.GetOpcode());
                changed_if.AddParamMove(tree.GetParam(0));
                changed_if.AddParam(branch1.GetParam(0));
                changed_if.AddParam(branch2.GetParam(0));
                changed_if.Rehash();
                tree.SetOpcode(op1);
                tree.DelParams();
                tree.AddParamMove(changed_if);
                return true; // rerun optimization (opcode changed)
            }
            if(op1 == cAdd    || op1 == cMul
            || op1 == cAnd    || op1 == cOr
            || op1 == cAbsAnd || op1 == cAbsOr
            || op1 == cMin    || op1 == cMax)
            {
                // If the two groups contain one or more
                // identical values, extract them.
                std::vector<CodeTree<Value_t> > overlap;
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
                    CodeTree<Value_t> changed_if;
                    changed_if.SetOpcode(tree.GetOpcode());
                    changed_if.SetParamsMove(tree.GetParams());
                    changed_if.Rehash();
                    tree.SetOpcode(op1);
                    tree.SetParamsMove(overlap);
                    tree.AddParamMove(changed_if);
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
                    CodeTree<Value_t> branch2_backup = branch2;
                    branch2 = CodeTreeImmed( Value_t( (op1==cAdd||op1==cOr) ? 0 : 1 ) );
                    CodeTree<Value_t> changed_if;
                    changed_if.SetOpcode(tree.GetOpcode());
                    changed_if.SetParamsMove(tree.GetParams());
                    changed_if.Rehash();
                    tree.SetOpcode(op1);
                    tree.AddParamMove(branch2_backup);
                    tree.AddParamMove(changed_if);
                    return true; // rerun optimization (opcode changed)
                }
        }
        // if(x, y&z, !!y) -> if(x, z, 1) & y
        if((op1 == cAnd || op1 == cOr) && op2 == cNotNot)
        {
            CodeTree<Value_t>& branch2op = branch2.GetParam(0);
            for(size_t a=branch1.GetParamCount(); a-- > 0; )
                if(branch1.GetParam(a).IsIdenticalTo(branch2op))
                {
                    branch1.CopyOnWrite();
                    branch1.DelParam(a);
                    branch1.Rehash();
                    CodeTree<Value_t> branch2_backup = branch2op;
                    branch2 = CodeTreeImmed( Value_t( (op1==cOr) ? 0 : 1 ) );
                    CodeTree<Value_t> changed_if;
                    changed_if.SetOpcode(tree.GetOpcode());
                    changed_if.SetParamsMove(tree.GetParams());
                    changed_if.Rehash();
                    tree.SetOpcode(op1);
                    tree.AddParamMove(branch2_backup);
                    tree.AddParamMove(changed_if);
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
                    CodeTree<Value_t> branch1_backup = branch1;
                    branch1 = CodeTreeImmed( Value_t( (op2==cAdd||op2==cOr) ? 0 : 1 ) );
                    CodeTree<Value_t> changed_if;
                    changed_if.SetOpcode(tree.GetOpcode());
                    changed_if.SetParamsMove(tree.GetParams());
                    changed_if.Rehash();
                    tree.SetOpcode(op2);
                    tree.AddParamMove(branch1_backup);
                    tree.AddParamMove(changed_if);
                    return true; // rerun optimization (opcode changed)
                }
        }
        // if(x, !!y, y&z) -> if(x, 1, z) & y
        if((op2 == cAnd || op2 == cOr) && op1 == cNotNot)
        {
            CodeTree<Value_t>& branch1op = branch1.GetParam(0);
            for(size_t a=branch2.GetParamCount(); a-- > 0; )
                if(branch2.GetParam(a).IsIdenticalTo(branch1op))
                {
                    branch2.CopyOnWrite();
                    branch2.DelParam(a);
                    branch2.Rehash();
                    CodeTree<Value_t> branch1_backup = branch1op;
                    branch1 = CodeTreeImmed( Value_t( (op2==cOr) ? 0 : 1 ) );
                    CodeTree<Value_t> changed_if;
                    changed_if.SetOpcode(tree.GetOpcode());
                    changed_if.SetParamsMove(tree.GetParams());
                    changed_if.Rehash();
                    tree.SetOpcode(op2);
                    tree.AddParamMove(branch1_backup);
                    tree.AddParamMove(changed_if);
                    return true; // rerun optimization (opcode changed)
                }
        }
        return false; // No changes
    }

    /**************************************/
    /* OPERATIONS DONE TO POW()           */
    /**************************************/

    template<typename Value_t>
    bool ConstantFolding_PowOperations(CodeTree<Value_t>& tree)
    {
        if(tree.GetParam(0).IsImmed()
        && tree.GetParam(1).IsImmed())
        {
            Value_t const_value = fp_pow(tree.GetParam(0).GetImmed(),
                                        tree.GetParam(1).GetImmed());
            tree.ReplaceWithImmed(const_value);
            return false;
        }
        if(tree.GetParam(1).IsImmed()
        && (float)tree.GetParam(1).GetImmed() == 1.0)
        {
            // Conversion through a float type value gets rid of
            // awkward abs(x)^1 generated from exp(log(x^6)/6),
            // without sacrificing as much precision as FloatEqual() does.
            // x^1 = x
            tree.Become(tree.GetParam(0));
            return true; // rerun optimization (opcode changed)
        }
        if(tree.GetParam(0).IsImmed()
        && (float)tree.GetParam(0).GetImmed() == 1.0)
        {
            // 1^x = 1
            tree.ReplaceWithImmed(1);
            return false;
        }

        // 5^(20*x) = (5^20)^x
        if(tree.GetParam(0).IsImmed()
        && tree.GetParam(1).GetOpcode() == cMul)
        {
            bool changes = false;
            Value_t base_immed = tree.GetParam(0).GetImmed();
            CodeTree<Value_t> mulgroup = tree.GetParam(1);
            for(size_t a=mulgroup.GetParamCount(); a-->0; )
                if(mulgroup.GetParam(a).IsImmed())
                {
                    Value_t imm = mulgroup.GetParam(a).GetImmed();
                    //if(imm >= 0.0)
                    {
                        Value_t new_base_immed = fp_pow(base_immed, imm);
                        if(isinf(new_base_immed)
                        || fp_equal(new_base_immed, Value_t(0)))
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
                std::cout << "Before pow-mul change: "; DumpTree(tree);
                std::cout << "\n";
            #endif
                tree.GetParam(0).Become(CodeTreeImmed(base_immed));
                tree.GetParam(1).Become(mulgroup);
            #ifdef DEBUG_SUBSTITUTIONS
                std::cout << "After pow-mul change: "; DumpTree(tree);
                std::cout << "\n";
            #endif
            }
        }
        // (x*20)^2 = x^2 * 20^2
        if(tree.GetParam(1).IsImmed()
        && tree.GetParam(0).GetOpcode() == cMul)
        {
            Value_t exponent_immed = tree.GetParam(1).GetImmed();
            Value_t factor_immed   = 1.0;
            bool changes = false;
            CodeTree<Value_t>& mulgroup = tree.GetParam(0);
            for(size_t a=mulgroup.GetParamCount(); a-->0; )
                if(mulgroup.GetParam(a).IsImmed())
                {
                    Value_t imm = mulgroup.GetParam(a).GetImmed();
                    //if(imm >= 0.0)
                    {
                        Value_t new_factor_immed = fp_pow(imm, exponent_immed);
                        if(isinf(new_factor_immed)
                        || fp_equal(new_factor_immed, Value_t(0)))
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
                CodeTree<Value_t> newpow;
                newpow.SetOpcode(cPow);
                newpow.SetParamsMove(tree.GetParams());
                tree.SetOpcode(cMul);
                tree.AddParamMove(newpow);
                tree.AddParam( CodeTreeImmed(factor_immed) );
                return true; // rerun optimization (opcode changed)
            }
        }

        // (x^3)^2 = x^6
        // NOTE: If 3 is even and 3*2 is not, x must be changed to abs(x).
        if(tree.GetParam(0).GetOpcode() == cPow
        && tree.GetParam(1).IsImmed()
        && tree.GetParam(0).GetParam(1).IsImmed())
        {
            Value_t a = tree.GetParam(0).GetParam(1).GetImmed();
            Value_t b = tree.GetParam(1).GetImmed();
            Value_t c = a * b; // new exponent
            if(IsEvenIntegerConst(a) // a is an even int?
            && !IsEvenIntegerConst(c)) // c is not?
            {
                CodeTree<Value_t> newbase;
                newbase.SetOpcode(cAbs);
                newbase.AddParam(tree.GetParam(0).GetParam(0));
                newbase.Rehash();
                tree.SetParamMove(0, newbase);
            }
            else
                tree.SetParam(0, tree.GetParam(0).GetParam(0));
            tree.SetParam(1, CodeTreeImmed(c));
        }
        return false; // No changes that require a rerun
    }

    /*****************************************/
    /* RANGE-BASED TREATMENTS TO COMPARISONS */
    /*****************************************/

    struct RangeComparisonData
    {
        enum Decision
        {
            MakeFalse=0,
            MakeTrue=1,
            MakeNEqual=2,
            MakeEqual=3,
            MakeNotNotP0=4,
            MakeNotNotP1=5,
            MakeNotP0=6,
            MakeNotP1=7,
            Unchanged=8
        };
        enum WhatDoWhenCase
        {
            Never =0,
            Eq0   =1, // val==0
            Eq1   =2, // val==1
            Gt0Le1=3, // val>0 && val<=1
            Ge0Lt1=4  // val>=0 && val<1
        };

        Decision if_identical; // What to do when operands are identical
        Decision if_always[4]; // What to do if Always <, <=, >, >=
        struct { Decision what : 4; WhatDoWhenCase when : 4; }
            p0_logical_a, p1_logical_a,
            p0_logical_b, p1_logical_b;

        template<typename Value_t>
        Decision Analyze(const CodeTree<Value_t>& a, const CodeTree<Value_t>& b) const
        {
            if(a.IsIdenticalTo(b))
                return if_identical;

            MinMaxTree<Value_t> p0 = CalculateResultBoundaries(a);
            MinMaxTree<Value_t> p1 = CalculateResultBoundaries(b);
            if(p0.has_max && p1.has_min)
            {
                if(p0.max <  p1.min && if_always[0] != Unchanged)
                    return if_always[0]; // p0 < p1
                if(p0.max <= p1.min && if_always[1] != Unchanged)
                    return if_always[1]; // p0 <= p1
            }
            if(p0.has_min && p1.has_max)
            {
                if(p0.min >  p1.max && if_always[2] != Unchanged)
                    return if_always[2]; // p0 > p1
                if(p0.min >= p1.max && if_always[3] != Unchanged)
                    return if_always[3]; // p0 >= p1
            }

            if(a.IsLogicalValue())
            {
                if(p0_logical_a.what != Unchanged)
                    if(TestCase(p0_logical_a.when, p1)) return p0_logical_a.what;
                if(p0_logical_b.what != Unchanged)
                    if(TestCase(p0_logical_b.when, p1)) return p0_logical_b.what;
            }
            if(b.IsLogicalValue())
            {
                if(p1_logical_a.what != Unchanged)
                    if(TestCase(p1_logical_a.when, p0)) return p1_logical_a.what;
                if(p1_logical_b.what != Unchanged)
                    if(TestCase(p1_logical_b.when, p0)) return p1_logical_b.what;
            }
            return Unchanged;
        }

        template<typename Value_t>
        static bool TestCase(WhatDoWhenCase when, const MinMaxTree<Value_t>& p)
        {
            if(!p.has_min || !p.has_max) return false;
            switch(when)
            {
                case Eq0: return p.min==0.0 && p.max==p.min;
                case Eq1: return p.min==1.0 && p.max==p.max;
                case Gt0Le1: return p.min>0 && p.max<=1;
                case Ge0Lt1: return p.min>=0 && p.max<1;
                default:;
            }
            return false;
        }
    };

    namespace RangeComparisonsData
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
               RangeComparisonData::Unchanged},
             // NotNot(p0) if p1==1    NotNot(p1) if p0==1
             //    Not(p0) if p1==0       Not(p1) if p0==0
              {RangeComparisonData::MakeNotNotP0, RangeComparisonData::Eq1},
              {RangeComparisonData::MakeNotNotP1, RangeComparisonData::Eq1},
              {RangeComparisonData::MakeNotP0, RangeComparisonData::Eq0},
              {RangeComparisonData::MakeNotP1, RangeComparisonData::Eq0}
            },
            // cNEqual:
            // Case:      p0 != p1  Antonym: p0 == p1
            // Synonym:   p1 != p0  Antonym: p1 == p0
            { RangeComparisonData::MakeFalse,  // If identical: always false
              {RangeComparisonData::MakeTrue,  // If Always p0 < p1: always true
               RangeComparisonData::Unchanged,
               RangeComparisonData::MakeTrue,  // If Always p0 > p1: always true
               RangeComparisonData::Unchanged},
             // NotNot(p0) if p1==0    NotNot(p1) if p0==0
             //    Not(p0) if p1==1       Not(p1) if p0==1
              {RangeComparisonData::MakeNotNotP0, RangeComparisonData::Eq0},
              {RangeComparisonData::MakeNotNotP1, RangeComparisonData::Eq0},
              {RangeComparisonData::MakeNotP0, RangeComparisonData::Eq1},
              {RangeComparisonData::MakeNotP1, RangeComparisonData::Eq1}
            },
            // cLess:
            // Case:      p0 < p1   Antonym: p0 >= p1
            // Synonym:   p1 > p0   Antonym: p1 <= p0
            { RangeComparisonData::MakeFalse,  // If identical: always false
              {RangeComparisonData::MakeTrue,  // If Always p0  < p1: always true
               RangeComparisonData::MakeNEqual,
               RangeComparisonData::MakeFalse, // If Always p0 > p1: always false
               RangeComparisonData::MakeFalse},// If Always p0 >= p1: always false
             // Not(p0)   if p1>0 & p1<=1    --   NotNot(p1) if p0>=0 & p0<1
              {RangeComparisonData::MakeNotP0,    RangeComparisonData::Gt0Le1},
              {RangeComparisonData::MakeNotNotP1, RangeComparisonData::Ge0Lt1},
              {RangeComparisonData::Unchanged, RangeComparisonData::Never},
              {RangeComparisonData::Unchanged, RangeComparisonData::Never}
            },
            // cLessOrEq:
            // Case:      p0 <= p1  Antonym: p0 > p1
            // Synonym:   p1 >= p0  Antonym: p1 < p0
            { RangeComparisonData::MakeTrue,   // If identical: always true
              {RangeComparisonData::Unchanged, // If Always p0  < p1: ?
               RangeComparisonData::MakeTrue,  // If Always p0 <= p1: always true
               RangeComparisonData::MakeFalse, // If Always p0  > p1: always false
               RangeComparisonData::MakeEqual},// If Never  p0  < p1:  use cEqual
             // Not(p0)    if p1>=0 & p1<1   --   NotNot(p1) if p0>0 & p0<=1
              {RangeComparisonData::MakeNotP0,    RangeComparisonData::Ge0Lt1},
              {RangeComparisonData::MakeNotNotP1, RangeComparisonData::Gt0Le1},
              {RangeComparisonData::Unchanged, RangeComparisonData::Never},
              {RangeComparisonData::Unchanged, RangeComparisonData::Never}
            },
            // cGreater:
            // Case:      p0 >  p1  Antonym: p0 <= p1
            // Synonym:   p1 <  p0  Antonym: p1 >= p0
            { RangeComparisonData::MakeFalse,  // If identical: always false
              {RangeComparisonData::MakeFalse, // If Always p0  < p1: always false
               RangeComparisonData::MakeFalse, // If Always p0 <= p1: always false
               RangeComparisonData::MakeTrue,  // If Always p0  > p1: always true
               RangeComparisonData::MakeNEqual},
             // NotNot(p0) if p1>=0 & p1<1   --   Not(p1)   if p0>0 & p0<=1
              {RangeComparisonData::MakeNotNotP0, RangeComparisonData::Ge0Lt1},
              {RangeComparisonData::MakeNotP1,    RangeComparisonData::Gt0Le1},
              {RangeComparisonData::Unchanged, RangeComparisonData::Never},
              {RangeComparisonData::Unchanged, RangeComparisonData::Never}
            },
            // cGreaterOrEq:
            // Case:      p0 >= p1  Antonym: p0 < p1
            // Synonym:   p1 <= p0  Antonym: p1 > p0
            { RangeComparisonData::MakeTrue,   // If identical: always true
              {RangeComparisonData::MakeFalse, // If Always p0  < p1: always false
               RangeComparisonData::MakeEqual, // If Always p0 >= p1: always true
               RangeComparisonData::Unchanged, // If always p0  > p1: ?
               RangeComparisonData::MakeTrue}, // If Never  p0  > p1:  use cEqual
             // NotNot(p0) if p1>0 & p1<=1   --   Not(p1)    if p0>=0 & p0<1
              {RangeComparisonData::MakeNotNotP0, RangeComparisonData::Gt0Le1},
              {RangeComparisonData::MakeNotP1,    RangeComparisonData::Ge0Lt1},
              {RangeComparisonData::Unchanged, RangeComparisonData::Never},
              {RangeComparisonData::Unchanged, RangeComparisonData::Never}
            }
        };
    }

    template<typename Value_t>
    bool ConstantFolding_ComparisonOperations(CodeTree<Value_t>& tree)
    {
        using namespace RangeComparisonsData;

        switch(Data[tree.GetOpcode()-cEqual].
            Analyze(tree.GetParam(0), tree.GetParam(1)))
        {
            case RangeComparisonData::MakeFalse:
                tree.ReplaceWithImmed(0); return true;
            case RangeComparisonData::MakeTrue:
                tree.ReplaceWithImmed(1); return true;
            case RangeComparisonData::MakeEqual:  tree.SetOpcode(cEqual); return true;
            case RangeComparisonData::MakeNEqual: tree.SetOpcode(cNEqual); return true;
            case RangeComparisonData::MakeNotNotP0: tree.SetOpcode(cNotNot); tree.DelParam(1); return true;
            case RangeComparisonData::MakeNotNotP1: tree.SetOpcode(cNotNot); tree.DelParam(0); return true;
            case RangeComparisonData::MakeNotP0: tree.SetOpcode(cNot); tree.DelParam(1); return true;
            case RangeComparisonData::MakeNotP1: tree.SetOpcode(cNot); tree.DelParam(0); return true;
            case RangeComparisonData::Unchanged:;
        }
        return false;
    }

    /*****************************************/
    /* ASSIMILATING SAME-TYPE CHILDREN       */
    /*****************************************/

    template<typename Value_t>
    bool ConstantFolding_Assimilate(CodeTree<Value_t>& tree)
    {
        /* If the list contains another list of the same kind, assimilate it */
        bool assimilated = false;
        for(size_t a=tree.GetParamCount(); a-- > 0; )
            if(tree.GetParam(a).GetOpcode() == tree.GetOpcode())
            {
              #ifdef DEBUG_SUBSTITUTIONS
                if(!assimilated)
                {
                    std::cout << "Before assimilation: "; DumpTree(tree);
                    std::cout << "\n";
                    assimilated = true;
                }
              #endif
                // Assimilate its children and remove it
                tree.AddParamsMove(tree.GetParam(a).GetUniqueRef().GetParams(), a);
            }
      #ifdef DEBUG_SUBSTITUTIONS
        if(assimilated)
        {
            std::cout << "After assimilation:   "; DumpTree(tree);
            std::cout << "\n";
        }
      #endif
        return assimilated;
    }
#ifdef DEBUG_SUBSTITUTIONS
    void OutFloatHex(std::ostream& o, double d)
    {
        union { double d; uint_least64_t h; } data;
        data.d = d;
        o << "(" << std::hex << data.h << std::dec << ")";
    }
    void OutFloatHex(std::ostream& o, float f)
    {
        union { float f; uint_least32_t h; } data;
        data.f = f;
        o << "(" << std::hex << data.h << std::dec << ")";
    }
    void OutFloatHex(std::ostream& o, long double ld)
    {
        union { long double ld;
                struct { uint_least64_t a; unsigned short b; } s; } data;
        data.ld = ld;
        o << "(" << std::hex << data.s.b << data.s.a << std::dec << ")";
    }
#endif
}

namespace FPoptimizer_CodeTree
{
    template<typename Value_t>
    void ConstantFolding(CodeTree<Value_t>& tree)
    {
    #ifdef DEBUG_SUBSTITUTIONS
        std::cout << "Runs ConstantFolding for: ";
        DumpTree(tree);
        std::cout << "\n";
        DumpHashes(tree);
    #endif
        using namespace std;
    redo:;

        // Insert here any hardcoded constant-folding optimizations
        // that you want to be done whenever a new subtree is generated.
        /* Not recursive. */

        Value_t const_value = 1.0;
        if(tree.GetOpcode() != cImmed)
        {
            MinMaxTree<Value_t> p = CalculateResultBoundaries(tree);
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
                std::cout << "Replacing "; DumpTree(tree);
                if(tree.IsImmed())
                    OutFloatHex(std::cout, tree.GetImmed());
                std::cout << " with const value " << const_value;
                OutFloatHex(std::cout, const_value);
                std::cout << "\n";
              #endif
                tree.ReplaceWithImmed(const_value);
                return;
            ReplaceTreeWithParam0:
              #ifdef DEBUG_SUBSTITUTIONS
                std::cout << "Before replace: ";
                std::cout << std::hex
                          << '[' << tree.GetHash().hash1
                          << ',' << tree.GetHash().hash2
                          << ']' << std::dec;
                DumpTree(tree);
                std::cout << "\n";
              #endif
                tree.Become(tree.GetParam(0));
              #ifdef DEBUG_SUBSTITUTIONS
                std::cout << "After replace: ";
                std::cout << std::hex
                          << '[' << tree.GetHash().hash1
                          << ',' << tree.GetHash().hash2
                          << ']' << std::dec;
                DumpTree(tree);
                std::cout << "\n";
              #endif
                goto redo;
        }

        /* Constant folding */
        switch(tree.GetOpcode())
        {
            case cImmed:
                break; // nothing to do
            case VarBegin:
                break; // nothing to do

            case cAnd:
            case cAbsAnd:
            {
                ConstantFolding_Assimilate(tree);
                for(size_t a=tree.GetParamCount(); a-- > 0; )
                    switch(GetLogicalValue(
                        CalculateResultBoundaries( tree.GetParam(a) ),
                        tree.GetOpcode()==cAbsAnd))
                    {
                        case 0: goto ReplaceTreeWithZero;
                        case 1: tree.DelParam(a); break; // x & y & 1 = x & y;  x & 1 = !!x
                        default: ;
                    }
                switch(tree.GetParamCount())
                {
                    case 0: goto ReplaceTreeWithOne;
                    case 1: tree.SetOpcode(tree.GetOpcode()==cAnd ? cNotNot : cAbsNotNot); goto redo; // Replace self with the single operand
                    default: if(tree.GetOpcode()==cAnd) if(ConstantFolding_AndLogic(tree)) goto redo;
                }
                break;
            }
            case cOr:
            case cAbsOr:
            {
                ConstantFolding_Assimilate(tree);
                for(size_t a=tree.GetParamCount(); a-- > 0; )
                    switch(GetLogicalValue(
                        CalculateResultBoundaries( tree.GetParam(a) ),
                        tree.GetOpcode()==cAbsOr))
                    {
                        case 1: goto ReplaceTreeWithOne;
                        case 0: tree.DelParam(a); break;
                        default: ;
                    }
                switch(tree.GetParamCount())
                {
                    case 0: goto ReplaceTreeWithZero;
                    case 1: tree.SetOpcode(tree.GetOpcode()==cOr ? cNotNot : cAbsNotNot); goto redo; // Replace self with the single operand
                    default: if(tree.GetOpcode()==cOr) if(ConstantFolding_OrLogic(tree)) goto redo;
                }
                break;
            }
            case cNot:
            case cAbsNot:
            {
                unsigned opposite = 0;
                switch(tree.GetParam(0).GetOpcode())
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
                    tree.SetOpcode(OPCODE(opposite));
                    tree.SetParamsMove(tree.GetParam(0).GetUniqueRef().GetParams());
                    goto redo;
                }

                // If the sub-expression evaluates to approx. zero, yield one.
                // If the sub-expression evaluates to approx. nonzero, yield zero.
                switch(GetLogicalValue(CalculateResultBoundaries( tree.GetParam(0) ),
                                       tree.GetOpcode()==cAbsNot))
                {
                    case 1: goto ReplaceTreeWithZero;
                    case 0: goto ReplaceTreeWithOne;
                    default: ;
                }
                if(tree.GetOpcode() == cNot && tree.GetParam(0).IsAlwaysSigned(true))
                    tree.SetOpcode(cAbsNot);

                if(tree.GetParam(0).GetOpcode() == cIf
                || tree.GetParam(0).GetOpcode() == cAbsIf)
                {
                    CodeTree<Value_t> iftree = tree.GetParam(0);
                    const CodeTree<Value_t>& ifp1 = iftree.GetParam(1);
                    const CodeTree<Value_t>& ifp2 = iftree.GetParam(2);
                    if(ifp1.GetOpcode() == cNot
                    || ifp1.GetOpcode() == cAbsNot)
                    {
                        // cNot [(cIf [x (cNot[y]) z])] -> cIf [x (cNotNot[y]) (cNot[z])]
                        tree.SetParam(0, iftree.GetParam(0)); // condition
                        CodeTree<Value_t> p1;
                        p1.SetOpcode(ifp1.GetOpcode()==cNot ? cNotNot : cAbsNotNot);
                        p1.AddParam(ifp1.GetParam(0));
                        p1.Rehash();
                        tree.AddParamMove(p1);
                        CodeTree<Value_t> p2;
                        p2.SetOpcode(tree.GetOpcode());
                        p2.AddParam(ifp2);
                        p2.Rehash();
                        tree.AddParamMove(p2);
                        tree.SetOpcode(iftree.GetOpcode());
                        goto redo;
                    }
                    if(ifp2.GetOpcode() == cNot
                    || ifp2.GetOpcode() == cAbsNot)
                    {
                        // cNot [(cIf [x y (cNot[z])])] -> cIf [x (cNot[y]) (cNotNot[z])]
                        tree.SetParam(0, iftree.GetParam(0)); // condition
                        CodeTree<Value_t> p1;
                        p1.SetOpcode(tree.GetOpcode());
                        p1.AddParam(ifp1);
                        p1.Rehash();
                        tree.AddParamMove(p1);
                        CodeTree<Value_t> p2;
                        p2.SetOpcode(ifp2.GetOpcode()==cNot ? cNotNot : cAbsNotNot);
                        p2.AddParam(ifp2.GetParam(0));
                        p2.Rehash();
                        tree.AddParamMove(p2);
                        tree.SetOpcode(iftree.GetOpcode());
                        goto redo;
                    }
                }
                break;
            }
            case cNotNot:
            case cAbsNotNot:
            {
                // The function of cNotNot is to protect a logical value from
                // changing. If the parameter is already a logical value,
                // then the cNotNot opcode is redundant.
                if(tree.GetParam(0).IsLogicalValue())
                    goto ReplaceTreeWithParam0;

                // If the sub-expression evaluates to approx. zero, yield zero.
                // If the sub-expression evaluates to approx. nonzero, yield one.
                switch(GetLogicalValue(CalculateResultBoundaries( tree.GetParam(0) ),
                                       tree.GetOpcode()==cAbsNotNot))
                {
                    case 0: goto ReplaceTreeWithZero;
                    case 1: goto ReplaceTreeWithOne;
                    default: ;
                }
                if(tree.GetOpcode() == cNotNot && tree.GetParam(0).IsAlwaysSigned(true))
                    tree.SetOpcode(cAbsNotNot);

                if(tree.GetParam(0).GetOpcode() == cIf
                || tree.GetParam(0).GetOpcode() == cAbsIf)
                {
                    CodeTree<Value_t> iftree = tree.GetParam(0);
                    const CodeTree<Value_t>& ifp1 = iftree.GetParam(1);
                    const CodeTree<Value_t>& ifp2 = iftree.GetParam(2);
                    if(ifp1.GetOpcode() == cNot
                    || ifp1.GetOpcode() == cAbsNot)
                    {
                        // cNotNot [(cIf [x (cNot[y]) z])] -> cIf [x (cNot[y]) (cNotNot[z])]
                        tree.SetParam(0, iftree.GetParam(0)); // condition
                        tree.AddParam(ifp1);
                        CodeTree<Value_t> p2;
                        p2.SetOpcode(tree.GetOpcode());
                        p2.AddParam(ifp2);
                        p2.Rehash();
                        tree.AddParamMove(p2);
                        tree.SetOpcode(iftree.GetOpcode());
                        goto redo;
                    }
                    if(ifp2.GetOpcode() == cNot
                    || ifp2.GetOpcode() == cAbsNot)
                    {
                        // cNotNot [(cIf [x y (cNot[z])])] -> cIf [x (cNotNot[y]) (cNot[z])]
                        tree.SetParam(0, iftree.GetParam(0)); // condition
                        CodeTree<Value_t> p1;
                        p1.SetOpcode(tree.GetOpcode());
                        p1.AddParam(ifp1);
                        p1.Rehash();
                        tree.AddParamMove(p1);
                        tree.AddParam(ifp2);
                        tree.SetOpcode(iftree.GetOpcode());
                        goto redo;
                    }
                }
                break;
            }
            case cIf:
            case cAbsIf:
            {
                if(ConstantFolding_IfOperations(tree))
                    goto redo;
                break;
            }
            case cMul:
            {
            NowWeAreMulGroup: ;
                ConstantFolding_Assimilate(tree);
                // If one sub-expression evalutes to exact zero, yield zero.
                Value_t immed_product = Value_t(1);
                size_t n_immeds = 0; bool needs_resynth=false;
                for(size_t a=0; a<tree.GetParamCount(); ++a)
                {
                    if(!tree.GetParam(a).IsImmed()) continue;
                    // ^ Only check constant values
                    Value_t immed = tree.GetParam(a).GetImmed();
                    if(immed == Value_t(0) ) goto ReplaceTreeWithZero;
                    immed_product *= immed; ++n_immeds;
                }
                // Merge immeds.
                if(n_immeds > 1 || (n_immeds == 1 && FloatEqual(immed_product, Value_t(1))))
                    needs_resynth = true;
                if(needs_resynth)
                {
                    // delete immeds and add new ones
                #ifdef DEBUG_SUBSTITUTIONS
                    std::cout << "cMul: Will add new immed " << immed_product << "\n";
                #endif
                    for(size_t a=tree.GetParamCount(); a-->0; )
                        if(tree.GetParam(a).IsImmed())
                        {
                        #ifdef DEBUG_SUBSTITUTIONS
                            std::cout << " - For that, deleting immed " << tree.GetParam(a).GetImmed();
                            std::cout << "\n";
                        #endif
                            tree.DelParam(a);
                        }
                    if(!FloatEqual(immed_product, Value_t(1)))
                        tree.AddParam( CodeTreeImmed(immed_product) );
                }
                switch(tree.GetParamCount())
                {
                    case 0: goto ReplaceTreeWithOne;
                    case 1: goto ReplaceTreeWithParam0; // Replace self with the single operand
                    default:
                        if(ConstantFolding_MulGrouping(tree)) goto redo;
                        if(ConstantFolding_MulLogicItems(tree)) goto redo;
                }
                break;
            }
            case cAdd:
            {
                ConstantFolding_Assimilate(tree);
                Value_t immed_sum = 0.0;
                size_t n_immeds = 0; bool needs_resynth=false;
                for(size_t a=0; a<tree.GetParamCount(); ++a)
                {
                    if(!tree.GetParam(a).IsImmed()) continue;
                    // ^ Only check constant values
                    Value_t immed = tree.GetParam(a).GetImmed();
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
                    std::cout << "In: "; DumpTree(tree);
                    std::cout << "\n";
                #endif
                    for(size_t a=tree.GetParamCount(); a-->0; )
                        if(tree.GetParam(a).IsImmed())
                        {
                        #ifdef DEBUG_SUBSTITUTIONS
                            std::cout << " - For that, deleting immed " << tree.GetParam(a).GetImmed();
                            std::cout << "\n";
                        #endif
                            tree.DelParam(a);
                        }
                    if(!(immed_sum == 0.0))
                        tree.AddParam( CodeTreeImmed(immed_sum) );
                }
                switch(tree.GetParamCount())
                {
                    case 0: goto ReplaceTreeWithZero;
                    case 1: goto ReplaceTreeWithParam0; // Replace self with the single operand
                    default:
                        if(ConstantFolding_AddGrouping(tree)) goto redo;
                        if(ConstantFolding_AddLogicItems(tree)) goto redo;
                }
                break;
            }
            case cMin:
            {
                ConstantFolding_Assimilate(tree);
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
                MinMaxTree<Value_t> smallest_maximum;
                for(size_t a=0; a<tree.GetParamCount(); ++a)
                {
                    while(a+1 < tree.GetParamCount() && tree.GetParam(a).IsIdenticalTo(tree.GetParam(a+1)))
                        tree.DelParam(a+1);
                    MinMaxTree<Value_t> p = CalculateResultBoundaries( tree.GetParam(a) );
                    if(p.has_max && (!smallest_maximum.has_max || (p.max) < smallest_maximum.max))
                    {
                        smallest_maximum.max = p.max;
                        smallest_maximum.has_max = true;
                        preserve=a;
                }   }
                if(smallest_maximum.has_max)
                    for(size_t a=tree.GetParamCount(); a-- > 0; )
                    {
                        MinMaxTree<Value_t> p = CalculateResultBoundaries( tree.GetParam(a) );
                        if(p.has_min && a != preserve && p.min >= smallest_maximum.max)
                            tree.DelParam(a);
                    }
                //fprintf(stderr, "Remains: %u\n", (unsigned)tree.GetParamCount());
                if(tree.GetParamCount() == 1)
                {
                    // Replace self with the single operand
                    goto ReplaceTreeWithParam0;
                }
                break;
            }
            case cMax:
            {
                ConstantFolding_Assimilate(tree);
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
                MinMaxTree<Value_t> biggest_minimum;
                for(size_t a=0; a<tree.GetParamCount(); ++a)
                {
                    while(a+1 < tree.GetParamCount() && tree.GetParam(a).IsIdenticalTo(tree.GetParam(a+1)))
                        tree.DelParam(a+1);
                    MinMaxTree<Value_t> p = CalculateResultBoundaries( tree.GetParam(a) );
                    if(p.has_min && (!biggest_minimum.has_min || p.min > biggest_minimum.min))
                    {
                        biggest_minimum.min = p.min;
                        biggest_minimum.has_min = true;
                        preserve=a;
                }   }
                if(biggest_minimum.has_min)
                {
                    //fprintf(stderr, "Removing all where max < %g\n", biggest_minimum.min);
                    for(size_t a=tree.GetParamCount(); a-- > 0; )
                    {
                        MinMaxTree<Value_t> p = CalculateResultBoundaries( tree.GetParam(a) );
                        if(p.has_max && a != preserve && (p.max) < biggest_minimum.min)
                        {
                            //fprintf(stderr, "Removing %g\n", p.max);
                            tree.DelParam(a);
                        }
                    }
                }
                //fprintf(stderr, "Remains: %u\n", (unsigned)tree.GetParamCount());
                if(tree.GetParamCount() == 1)
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
                if(ConstantFolding_ComparisonOperations(tree)) goto redo;
                // Any reversible functions:
                //   sin(x)  -> ASIN: Not doable, x can be cyclic
                //   asin(x) -> SIN: doable.
                //                   Invalid combinations are caught by
                //                   range-estimation. Threshold is at |pi/2|.
                //   acos(x) -> COS: doable.
                //                   Invalid combinations are caught by
                //                   range-estimation. Note that though
                //                   the range is contiguous, it is direction-flipped.
                //    log(x) -> EXP: no problem
                //   exp2, exp10: Converted to cPow, done by grammar.
                //   atan(x) -> TAN: doable.
                //                   Invalid combinations are caught by
                //                   range-estimation. Threshold is at |pi/2|.
                //   sinh(x) -> ASINH: no problem
                //   tanh(x) -> ATANH: no problem, but atanh is limited to -1..1
                //                     Invalid combinations are caught by
                //                     range-estimation, but the exact value
                //                     of 1.0 still needs checking, because
                //                     it involves infinity.
                if(tree.GetParam(1).IsImmed())
                    switch(tree.GetParam(0).GetOpcode())
                    {
                        case cAsin:
                            tree.SetParam(0, tree.GetParam(0).GetParam(0));
                            tree.SetParam(1, CodeTreeImmed(fp_sin(tree.GetParam(1).GetImmed())));
                            goto redo;
                        case cAcos:
                            // -1..+1 --> pi..0 (polarity-flipping)
                            tree.SetParam(0, tree.GetParam(0).GetParam(0));
                            tree.SetParam(1, CodeTreeImmed(fp_cos(tree.GetParam(1).GetImmed())));
                            tree.SetOpcode( tree.GetOpcode()==cLess ? cGreater
                                     : tree.GetOpcode()==cLessOrEq ? cGreaterOrEq
                                     : tree.GetOpcode()==cGreater ? cLess
                                     : tree.GetOpcode()==cGreaterOrEq ? cLessOrEq
                                     : tree.GetOpcode() );
                            goto redo;
                        case cAtan:
                            tree.SetParam(0, tree.GetParam(0).GetParam(0));
                            tree.SetParam(1, CodeTreeImmed(fp_tan(tree.GetParam(1).GetImmed())));
                            goto redo;
                        case cLog:
                            // Different logarithms have a constant-multiplication,
                            // which is no problem.
                            tree.SetParam(0, tree.GetParam(0).GetParam(0));
                            tree.SetParam(1, CodeTreeImmed(fp_exp(tree.GetParam(1).GetImmed())));
                            goto redo;
                        case cSinh:
                            tree.SetParam(0, tree.GetParam(0).GetParam(0));
                            tree.SetParam(1, CodeTreeImmed(fp_asinh(tree.GetParam(1).GetImmed())));
                            goto redo;
                        case cTanh:
                            if(fp_less(fp_abs(tree.GetParam(1).GetImmed()), Value_t(1)))
                            {
                                tree.SetParam(0, tree.GetParam(0).GetParam(0));
                                tree.SetParam(1, CodeTreeImmed(fp_atanh(tree.GetParam(1).GetImmed())));
                                goto redo;
                            }
                            break;
                        default: break;
                    }
                break;

            case cAbs:
            {
                /* If we know the operand is always positive, cAbs is redundant.
                 * If we know the operand is always negative, use actual negation.
                 */
                MinMaxTree<Value_t> p0 = CalculateResultBoundaries( tree.GetParam(0) );
                if(p0.has_min && p0.min >= 0.0)
                    goto ReplaceTreeWithParam0;
                if(p0.has_max && p0.max <= fp_const_negativezero<Value_t>())
                {
                    /* abs(negative) = negative*-1 */
                    tree.SetOpcode(cMul);
                    tree.AddParam( CodeTreeImmed(Value_t(1)) );
                    /* The caller of ConstantFolding() will do Sort() and Rehash() next.
                     * Thus, no need to do it here. */
                    /* We were changed into a cMul group. Do cMul folding. */
                    goto NowWeAreMulGroup;
                }
                /* If the operand is a cMul group, find elements
                 * that are always positive and always negative,
                 * and move them out, e.g. abs(p*n*x*y) = p*(-n)*abs(x*y)
                 */
                if(tree.GetParam(0).GetOpcode() == cMul)
                {
                    const CodeTree<Value_t>& p = tree.GetParam(0);
                    std::vector<CodeTree<Value_t> > pos_set;
                    std::vector<CodeTree<Value_t> > neg_set;
                    for(size_t a=0; a<p.GetParamCount(); ++a)
                    {
                        p0 = CalculateResultBoundaries( p.GetParam(a) );
                        if(p0.has_min && p0.min >= 0.0)
                            { pos_set.push_back(p.GetParam(a)); }
                        if(p0.has_max && p0.max <= fp_const_negativezero<Value_t>())
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
                        DumpTree(tree);
                        std::cout << "\n" << std::flush;
                        DumpHashes(tree, std::cout);
                #endif
                        CodeTree<Value_t> pclone;
                        pclone.SetOpcode(cMul);
                        for(size_t a=0; a<p.GetParamCount(); ++a)
                        {
                            p0 = CalculateResultBoundaries( p.GetParam(a) );
                            if((p0.has_min && p0.min >= 0.0)
                            || (p0.has_max && p0.max <= fp_const_negativezero<Value_t>()))
                                {/*pclone.DelParam(a);*/}
                            else
                                pclone.AddParam( p.GetParam(a) );
                            /* Here, p*n*x*y -> x*y.
                             * p is saved in pos_set[]
                             * n is saved in neg_set[]
                             */
                        }
                        pclone.Rehash();
                        CodeTree<Value_t> abs_mul;
                        abs_mul.SetOpcode(cAbs);
                        abs_mul.AddParamMove(pclone);
                        abs_mul.Rehash();
                        CodeTree<Value_t> mulgroup;
                        mulgroup.SetOpcode(cMul);
                        mulgroup.AddParamMove(abs_mul); // cAbs[whatever remains in p]
                        mulgroup.AddParamsMove(pos_set);
                        /* Now:
                         * mulgroup  = p * Abs(x*y)
                         */
                        if(!neg_set.empty())
                        {
                            if(neg_set.size() % 2)
                                mulgroup.AddParam( CodeTreeImmed(Value_t(-1)) );
                            mulgroup.AddParamsMove(neg_set);
                            /* Now:
                             * mulgroup = p * n * -1 * Abs(x*y)
                             */
                        }
                        tree.Become(mulgroup);
                #ifdef DEBUG_SUBSTITUTIONS
                        std::cout << "AbsReplace-After: ";
                        DumpTree(tree, std::cout);
                        std::cout << "\n" << std::flush;
                        DumpHashes(tree, std::cout);
                #endif
                        /* We were changed into a cMul group. Do cMul folding. */
                        goto NowWeAreMulGroup;
                    }
                }
                break;
            }

            #define HANDLE_UNARY_CONST_FUNC(funcname) \
                if(tree.GetParam(0).IsImmed()) \
                    { const_value = funcname(tree.GetParam(0).GetImmed()); \
                      goto ReplaceTreeWithConstValue; }

            case cLog:
                HANDLE_UNARY_CONST_FUNC(fp_log);
                if(tree.GetParam(0).GetOpcode() == cPow)
                {
                    CodeTree<Value_t> pow = tree.GetParam(0);
                    if(pow.GetParam(0).IsAlwaysSigned(true))  // log(posi ^ y) = y*log(posi)
                    {
                        pow.CopyOnWrite();
                        pow.SetOpcode(cLog);
                        tree.SetOpcode(cMul);
                        tree.AddParamMove(pow.GetParam(1));
                        pow.DelParam(1);
                        pow.Rehash();
                        tree.SetParamMove(0, pow);
                        goto NowWeAreMulGroup;
                    }
                    if(pow.GetParam(1).IsAlwaysParity(false)) // log(x ^ even) = even*log(abs(x))
                    {
                        pow.CopyOnWrite();
                        CodeTree<Value_t> abs;
                        abs.SetOpcode(cAbs);
                        abs.AddParamMove(pow.GetParam(0));
                        abs.Rehash();
                        pow.SetOpcode(cLog);
                        tree.SetOpcode(cMul);
                        pow.SetParamMove(0, abs);
                        tree.AddParamMove(pow.GetParam(1));
                        pow.DelParam(1);
                        pow.Rehash();
                        tree.SetParamMove(0, pow);
                        goto NowWeAreMulGroup;
                    }
                }
                else if(tree.GetParam(0).GetOpcode() == cAbs)
                {
                    // log(abs(x^y)) = y*log(abs(x))
                    CodeTree<Value_t> pow = tree.GetParam(0).GetParam(0);
                    if(pow.GetOpcode() == cPow)
                    {
                        pow.CopyOnWrite();
                        CodeTree<Value_t> abs;
                        abs.SetOpcode(cAbs);
                        abs.AddParamMove(pow.GetParam(0));
                        abs.Rehash();
                        pow.SetOpcode(cLog);
                        tree.SetOpcode(cMul);
                        pow.SetParamMove(0, abs);
                        tree.AddParamMove(pow.GetParam(1));
                        pow.DelParam(1);
                        pow.Rehash();
                        tree.SetParamMove(0, pow);
                        goto NowWeAreMulGroup;
                    }
                }
                break;
            case cAcosh: HANDLE_UNARY_CONST_FUNC(fp_acosh); break;
            case cAsinh: HANDLE_UNARY_CONST_FUNC(fp_asinh); break;
            case cAtanh: HANDLE_UNARY_CONST_FUNC(fp_atanh); break;
            case cAcos: HANDLE_UNARY_CONST_FUNC(fp_acos); break;
            case cAsin: HANDLE_UNARY_CONST_FUNC(fp_asin); break;
            case cAtan: HANDLE_UNARY_CONST_FUNC(fp_atan); break;
            case cCosh: HANDLE_UNARY_CONST_FUNC(fp_cosh); break;
            case cSinh: HANDLE_UNARY_CONST_FUNC(fp_sinh); break;
            case cTanh: HANDLE_UNARY_CONST_FUNC(fp_tanh); break;
            case cSin: HANDLE_UNARY_CONST_FUNC(fp_sin); break;
            case cCos: HANDLE_UNARY_CONST_FUNC(fp_cos); break;
            case cTan: HANDLE_UNARY_CONST_FUNC(fp_tan); break;
            case cCeil:
                if(tree.GetParam(0).IsAlwaysInteger(true)) goto ReplaceTreeWithParam0;
                HANDLE_UNARY_CONST_FUNC(fp_ceil); break;
            case cTrunc:
                if(tree.GetParam(0).IsAlwaysInteger(true)) goto ReplaceTreeWithParam0;
                HANDLE_UNARY_CONST_FUNC(fp_trunc); break;
            case cFloor:
                if(tree.GetParam(0).IsAlwaysInteger(true)) goto ReplaceTreeWithParam0;
                HANDLE_UNARY_CONST_FUNC(fp_floor); break;
            case cInt:
                if(tree.GetParam(0).IsAlwaysInteger(true)) goto ReplaceTreeWithParam0;
                HANDLE_UNARY_CONST_FUNC(fp_int); break;
            case cCbrt: HANDLE_UNARY_CONST_FUNC(fp_cbrt); break; // converted into cPow x 0.33333
            case cSqrt: HANDLE_UNARY_CONST_FUNC(fp_sqrt); break; // converted into cPow x 0.5
            case cExp: HANDLE_UNARY_CONST_FUNC(fp_exp); break; // convered into cPow CONSTANT_E x
            case cLog2: HANDLE_UNARY_CONST_FUNC(fp_log2); break;
            case cLog10: HANDLE_UNARY_CONST_FUNC(fp_log10); break;

            case cLog2by:
                if(tree.GetParam(0).IsImmed()
                && tree.GetParam(1).IsImmed())
                    { const_value = fp_log2(tree.GetParam(0).GetImmed()) * tree.GetParam(1).GetImmed();
                      goto ReplaceTreeWithConstValue; }
                break;

            case cMod: /* Can more be done than this? */
                if(tree.GetParam(0).IsImmed()
                && tree.GetParam(1).IsImmed())
                    { const_value = fp_mod(tree.GetParam(0).GetImmed(), tree.GetParam(1).GetImmed());
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
                MinMaxTree<Value_t> p0 = CalculateResultBoundaries( tree.GetParam(0) );
                MinMaxTree<Value_t> p1 = CalculateResultBoundaries( tree.GetParam(1) );
                if(tree.GetParam(0).IsImmed()
                && fp_equal(tree.GetParam(0).GetImmed(), Value_t(0)))   // y == 0
                {
                    if(p1.has_max && (p1.max) < 0)          // y == 0 && x < 0
                        { const_value = fp_const_pi<Value_t>(); goto ReplaceTreeWithConstValue; }
                    if(p1.has_min && p1.min >= 0.0)           // y == 0 && x >= 0.0
                        { const_value = Value_t(0); goto ReplaceTreeWithConstValue; }
                }
                if(tree.GetParam(1).IsImmed()
                && fp_equal(tree.GetParam(1).GetImmed(), Value_t(0)))   // x == 0
                {
                    if(p0.has_max && (p0.max) < 0)          // y < 0 && x == 0
                        { const_value = -fp_const_pihalf<Value_t>(); goto ReplaceTreeWithConstValue; }
                    if(p0.has_min && p0.min > 0)              // y > 0 && x == 0
                        { const_value =  fp_const_pihalf<Value_t>(); goto ReplaceTreeWithConstValue; }
                }
                if(tree.GetParam(0).IsImmed()
                && tree.GetParam(1).IsImmed())
                    { const_value = fp_atan2(tree.GetParam(0).GetImmed(),
                                             tree.GetParam(1).GetImmed());
                      goto ReplaceTreeWithConstValue; }
                if((p1.has_min && p1.min > 0.0)                   // p1 != 0.0
                || (p1.has_max && (p1.max) < fp_const_negativezero<Value_t>())) // become atan(p0 / p1)
                {
                    CodeTree<Value_t> pow_tree;
                    pow_tree.SetOpcode(cPow);
                    pow_tree.AddParamMove(tree.GetParam(1));
                    pow_tree.AddParam(CodeTreeImmed(Value_t(-1)));
                    pow_tree.Rehash();
                    CodeTree<Value_t> div_tree;
                    div_tree.SetOpcode(cMul);
                    div_tree.AddParamMove(tree.GetParam(0));
                    div_tree.AddParamMove(pow_tree);
                    div_tree.Rehash();
                    tree.SetOpcode(cAtan);
                    tree.SetParamMove(0, div_tree);
                    tree.DelParam(1);
                }
                break;
            }

            case cPow:
            {
                if(ConstantFolding_PowOperations(tree)) goto redo;
                break;
            }

            /* The following opcodes are processed by GenerateFrom()
             * within fpoptimizer_bytecode_to_codetree.cc and thus
             * they will never occur in the calling context for the
             * most of the parsing context. They may however occur
             * at the late phase, so we deal with them.
             */
            case cDiv: // converted into cPow y -1
                if(tree.GetParam(0).IsImmed()
                && tree.GetParam(1).IsImmed()
                && tree.GetParam(1).GetImmed() != 0.0)
                    { const_value = tree.GetParam(0).GetImmed() / tree.GetParam(1).GetImmed();
                      goto ReplaceTreeWithConstValue; }
                break;
            case cInv: // converted into cPow y -1
                if(tree.GetParam(0).IsImmed()
                && tree.GetParam(0).GetImmed() != 0.0)
                    { const_value = Value_t(1) / tree.GetParam(0).GetImmed();
                      goto ReplaceTreeWithConstValue; }
                // Note: Could use (mulgroup)^immed optimization from cPow
                break;
            case cSub: // converted into cMul y -1
                if(tree.GetParam(0).IsImmed()
                && tree.GetParam(1).IsImmed())
                    { const_value = tree.GetParam(0).GetImmed() - tree.GetParam(1).GetImmed();
                      goto ReplaceTreeWithConstValue; }
                break;
            case cNeg: // converted into cMul x -1
                if(tree.GetParam(0).IsImmed())
                    { const_value = -tree.GetParam(0).GetImmed();
                      goto ReplaceTreeWithConstValue; }
                break;
            case cRad: // converted into cMul x CONSTANT_RD
                if(tree.GetParam(0).IsImmed())
                    { const_value = tree.GetParam(0).GetImmed() * fp_const_rad_to_deg<Value_t>();
                      goto ReplaceTreeWithConstValue; }
                break;
            case cDeg: // converted into cMul x CONSTANT_DR
                if(tree.GetParam(0).IsImmed())
                    { const_value = tree.GetParam(0).GetImmed() * fp_const_deg_to_rad<Value_t>();
                      goto ReplaceTreeWithConstValue; }
                break;
            case cSqr: // converted into cMul x x
                if(tree.GetParam(0).IsImmed())
                    { const_value = tree.GetParam(0).GetImmed() * tree.GetParam(0).GetImmed();
                      goto ReplaceTreeWithConstValue; }
                break;
            case cExp2: // converted into cPow 2.0 x
                HANDLE_UNARY_CONST_FUNC(fp_exp2); break;
            case cRSqrt: // converted into cPow x -0.5
                if(tree.GetParam(0).IsImmed())
                    { const_value = Value_t(1) / fp_sqrt(tree.GetParam(0).GetImmed());
                      goto ReplaceTreeWithConstValue; }
                break;
            case cCot: // converted into cMul (cPow (cTan x) -1)
                if(tree.GetParam(0).IsImmed())
                    { Value_t tmp = fp_tan(tree.GetParam(0).GetImmed());
                      if(fp_nequal(tmp, Value_t(0)))
                      { const_value = Value_t(1) / tmp;
                        goto ReplaceTreeWithConstValue; } }
                break;
            case cSec: // converted into cMul (cPow (cCos x) -1)
                if(tree.GetParam(0).IsImmed())
                    { Value_t tmp = fp_cos(tree.GetParam(0).GetImmed());
                      if(fp_nequal(tmp, Value_t(0)))
                      { const_value = Value_t(1) / tmp;
                        goto ReplaceTreeWithConstValue; } }
                break;
            case cCsc: // converted into cMul (cPow (cSin x) -1)
                if(tree.GetParam(0).IsImmed())
                    { Value_t tmp = fp_sin(tree.GetParam(0).GetImmed());
                      if(fp_nequal(tmp, Value_t(0)))
                      { const_value = Value_t(1) / tmp;
                        goto ReplaceTreeWithConstValue; } }
                break;
            case cHypot: // converted into cSqrt(cAdd(cMul(x x), cMul(y y)))
                if(tree.GetParam(0).IsImmed() && tree.GetParam(1).IsImmed())
                {
                    const_value = fp_hypot(tree.GetParam(0).GetImmed(),
                                           tree.GetParam(1).GetImmed());
                    goto ReplaceTreeWithConstValue;
                }
                break;

            /* Opcodes that do not occur in the tree for other reasons */
            case cRDiv: // version of cDiv
            case cRSub: // version of cSub
            case cDup:
            case cFetch:
            case cPopNMov:
            case cSinCos:
            case cNop:
            case cJump:
                break; /* Should never occur */

            /* Opcodes that we can't do anything about */
            case cPCall:
            case cFCall:
            case cEval:
                break;
        }
    }
}

/* BEGIN_EXPLICIT_INSTANTATION */
namespace FPoptimizer_CodeTree
{
    template void ConstantFolding(CodeTree<double>& );
#ifdef FP_SUPPORT_FLOAT_TYPE
    template void ConstantFolding(CodeTree<float>& );
#endif
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
    template void ConstantFolding(CodeTree<long double>& );
#endif
}
/* END_EXPLICIT_INSTANTATION */

#endif
