#include "fpoptimizer_codetree.hh"
#include "fpoptimizer_optimize.hh"
#include "fpoptimizer_consts.hh"

#include <cmath> /* for CalculateResultBoundaries() */

#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#ifdef FP_SUPPORT_OPTIMIZER

using namespace FUNCTIONPARSERTYPES;

namespace FPoptimizer_CodeTree
{
    void CodeTree::ConstantFolding_FromLogicalParent()
    {
    redo:;
        switch(GetOpcode())
        {
            case cNotNot:
            //ReplaceTreeWithParam0:
                Become(GetParam(0));
                goto redo;
            case cIf:
                CopyOnWrite();
                while(GetParam(1).GetOpcode() == cNotNot)
                    SetParamMove(1, GetParam(1).GetUniqueRef().GetParam(0));
                GetParam(1).ConstantFolding_FromLogicalParent();

                while(GetParam(2).GetOpcode() == cNotNot)
                    SetParamMove(2, GetParam(2).GetUniqueRef().GetParam(0));
                GetParam(2).ConstantFolding_FromLogicalParent();

                Rehash();
                break;
            default: break;
        }
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
        struct Pole
        {
            CodeTree value;
            bool negated;
        };
        std::vector<Pole> plain_set;

        enum RelationshipResult
        {
            Ok,
            BecomeZero,
            BecomeOne,
            Suboptimal
        };

        RelationshipResult AddAndRelationship(CodeTree a, CodeTree b, int reltype)
        {
            if(reltype == 0) return BecomeZero;

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
                    int newrel = relationships[c].relationship & reltype;
                    if(newrel == 0) return BecomeZero;
                    relationships[c].relationship = newrel;
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
        RelationshipResult AddPole(const CodeTree& a, bool negated, bool is_or)
        {
            for(size_t c=0; c<plain_set.size(); ++c)
                if(plain_set[c].value.IsIdenticalTo(a))
                {
                    if(negated != plain_set[c].negated)
                        return is_or ? BecomeOne : BecomeZero;
                    return Suboptimal;
                }
            Pole pole;
            pole.value   = a;
            pole.negated = negated;
            plain_set.push_back(pole);
            return Ok;
        }

        RelationshipResult AddAndPole(const CodeTree& a, bool negated)
        {
            return AddPole(a, negated, false);
        }
        RelationshipResult AddOrPole(const CodeTree& a, bool negated)
        {
            return AddPole(a, negated, true);
        }

        RelationshipResult AddOrRelationship(CodeTree a, CodeTree b, int reltype)
        {
            if(reltype == 0x7) return BecomeOne;

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
                    int newrel = relationships[c].relationship | reltype;
                    if(newrel == 0x7) return BecomeOne;
                    relationships[c].relationship = newrel;
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

        RelationshipResult AddRelationship(const CodeTree& a, const CodeTree& b, int reltype, bool is_or)
        {
            return is_or ? AddOrRelationship(a, b, reltype)
                         :AddAndRelationship(a, b, reltype);
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

    bool CodeTree::ConstantFolding_LogicCommon(bool is_or)
    {
        bool should_regenerate = false;
        ComparisonSet comp;
        for(size_t a=0; a<GetParamCount(); ++a)
        {
            ComparisonSet::RelationshipResult change = ComparisonSet::Ok;
            switch(GetParam(a).GetOpcode())
            {
                case cEqual:
                    change = comp.AddRelationship(GetParam(a).GetParam(0), GetParam(a).GetParam(1), ComparisonSet::Eq_Mask, is_or);
                    break;
                case cNEqual:
                    change = comp.AddRelationship(GetParam(a).GetParam(0), GetParam(a).GetParam(1), ComparisonSet::Ne_Mask, is_or);
                    break;
                case cLess:
                    change = comp.AddRelationship(GetParam(a).GetParam(0), GetParam(a).GetParam(1), ComparisonSet::Lt_Mask, is_or);
                    break;
                case cLessOrEq:
                    change = comp.AddRelationship(GetParam(a).GetParam(0), GetParam(a).GetParam(1), ComparisonSet::Le_Mask, is_or);
                    break;
                case cGreater:
                    change = comp.AddRelationship(GetParam(a).GetParam(0), GetParam(a).GetParam(1), ComparisonSet::Gt_Mask, is_or);
                    break;
                case cGreaterOrEq:
                    change = comp.AddRelationship(GetParam(a).GetParam(0), GetParam(a).GetParam(1), ComparisonSet::Ge_Mask, is_or);
                    break;
                case cNot:
                    change = comp.AddPole(GetParam(a).GetParam(0), true, is_or);
                    break;
                default:
                    change = comp.AddPole(GetParam(a), false, is_or);
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
            std::cout << "Before ConstantFolding_LogicCommon: "; FPoptimizer_Grammar::DumpTree(*this);
            std::cout << "\n";
          #endif
            DelParams();
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
                else
                    AddParamMove(comp.plain_set[a].value);
            }
            for(size_t a=0; a<comp.relationships.size(); ++a)
            {
                CodeTree r;
                r.SetOpcode(cAtan2);
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
          #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "After ConstantFolding_LogicCommon: "; FPoptimizer_Grammar::DumpTree(*this);
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
        return ConstantFolding_LogicCommon(false);
    }
    bool CodeTree::ConstantFolding_OrLogic()
    {
        return ConstantFolding_LogicCommon(true);
    }

    bool CodeTree::ConstantFolding_MulGrouping()
    {
        bool should_regenerate = false;
        CollectionSet mul;
        for(size_t a=0; a<GetParamCount(); ++a)
        {
            CollectionSet::CollectionResult
                result = (GetParam(a).GetOpcode() == cPow)
                    ? mul.AddCollection(GetParam(a).GetParam(0), GetParam(a).GetParam(1))
                    : mul.AddCollection(GetParam(a));
            if(result == CollectionSet::Suboptimal)
                should_regenerate = true;
        }

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

        if(should_regenerate)
        {
          #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "Before ConstantFolding_MulGrouping: "; FPoptimizer_Grammar::DumpTree(*this);
            std::cout << "\n";
          #endif
            DelParams();

            /* Group by exponents */
        #if 0
            std::map<double, std::vector<CodeTree> > by_float_exponent;
        #endif
            /* First handle non-constant exponents */
            for(exponent_map::iterator
                i = by_exponent.begin();
                i != by_exponent.end();
                ++i)
            {
                exponent_list& list = i->second;
                if(list.first.IsImmed())
                {
                    double exponent = list.first.GetImmed();
                    if(FloatEqual(exponent, 0.0)) continue;
                #if 0
                    by_float_exponent[exponent].swap(list.second);
                    continue;
                #else
                    if(FloatEqual(exponent, 1.0))
                    {
                        AddParamsMove(list.second);
                        continue;
                    }
                #endif
                }
                CodeTree mul;
                mul.SetOpcode(cMul);
                mul.SetParamsMove( list.second);
                mul.Rehash();
                CodeTree pow;
                pow.SetOpcode(cPow);
                pow.AddParamMove(mul);
                pow.AddParamMove( list.first );
                pow.Rehash();
                AddParamMove(pow);
            }
        #if 0
            by_exponent.clear();

            /* Then handle constant exponents */
            /* TODO: Group them such that:
             *
             *      x^3 *         z^2 becomes (x*z)^2 * x
             *      x^3 * y^2.5 * z^2 becomes (x*z*y)^2 * y^0.5 * x
             *                    rather than (x*y*z)^2 * (x*y)^0.5 * x^0.5
             */
            for(std::map<double, std::vector<CodeTree> >::iterator
                i = by_float_exponent.begin();
                i != by_float_exponent.end();
                ++i)
            {
                double exponent = i->first;
                if(FloatEqual(exponent, 1.0))
                {
                    AddParamsMove(i->second);
                    continue;
                }
                CodeTree mul;
                mul.SetOpcode(cMul);
                mul.SetParamsMove( i->second );
                mul.Rehash();
                CodeTree pow;
                pow.SetOpcode(cPow);
                pow.AddParamMove(mul);
                pow.AddParam( CodeTree( i->first ) );
                pow.Rehash();
                AddParamMove(pow);
            }
        #endif
          #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "After ConstantFolding_MulGrouping: "; FPoptimizer_Grammar::DumpTree(*this);
            std::cout << "\n";
          #endif
            return true;
        }
        return false;
    }

    struct Select2stRev
    {
        template<typename T>
        bool operator() (const T& a, const T& b) const
        {
            return a.second > b.second;
        }
    };

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
                    FPoptimizer_Grammar::DumpTree(group_by);
                    std::cout << " in ";
                    FPoptimizer_Grammar::DumpTree(*this);
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
            std::cout << "Before ConstantFolding_AddGrouping: "; FPoptimizer_Grammar::DumpTree(*this);
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
                    if(FloatEqual(coeff.GetImmed(), 0.0))
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
            std::cout << "After ConstantFolding_AddGrouping: "; FPoptimizer_Grammar::DumpTree(*this);
            std::cout << "\n";
          #endif
            return true;
        }
        return false;
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
                    std::cout << "Before assimilation: "; FPoptimizer_Grammar::DumpTree(*this);
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
            std::cout << "After assimilation:   "; FPoptimizer_Grammar::DumpTree(*this);
            std::cout << "\n";
        }
      #endif
        return assimilated;
    }

    void CodeTree::ConstantFolding()
    {
    #ifdef DEBUG_SUBSTITUTIONS
        std::cout << "Runs ConstantFolding for: "; FPoptimizer_Grammar::DumpTree(*this);
        std::cout << "\n";
    #endif
        using namespace std;
    redo:;

        // Insert here any hardcoded constant-folding optimizations
        // that you want to be done whenever a new subtree is generated.
        /* Not recursive. */

        double const_value = 1.0;
        size_t which_param = 0;

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

        /* Constant folding */
        switch(GetOpcode())
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
                data = new CodeTreeData(const_value);
                break;
            ReplaceTreeWithParam0:
                which_param = 0;
            ReplaceTreeWithParam:
              #ifdef DEBUG_SUBSTITUTIONS
                std::cout << "Before replace: "; FPoptimizer_Grammar::DumpTree(*this);
                std::cout << "\n";
              #endif
                Become(GetParam(which_param));
              #ifdef DEBUG_SUBSTITUTIONS
                std::cout << "After replace: "; FPoptimizer_Grammar::DumpTree(*this);
                std::cout << "\n";
              #endif
                goto redo;

            case cAnd:
            {
                for(size_t a=0; a<GetParamCount(); ++a)
                    GetParam(a).ConstantFolding_FromLogicalParent();
                ConstantFolding_Assimilate();
                // If the and-list contains an expression that evaluates to approx. zero,
                // the whole list evaluates to zero.
                // If all expressions within the and-list evaluate to approx. nonzero,
                // the whole list evaluates to one.
                bool all_values_are_nonzero = true;
                for(size_t a=0; a<GetParamCount(); ++a)
                {
                    MinMaxTree p = GetParam(a).CalculateResultBoundaries();
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
                switch(GetParamCount())
                {
                    case 0: goto ReplaceTreeWithZero;
                    case 1: SetOpcode(cNotNot); break; // Replace self with the single operand
                    default: if(ConstantFolding_AndLogic()) goto redo;
                }
                break;
            }
            case cOr:
            {
                for(size_t a=0; a<GetParamCount(); ++a)
                    GetParam(a).ConstantFolding_FromLogicalParent();
                ConstantFolding_Assimilate();
                // If the or-list contains an expression that evaluates to approx. nonzero,
                // the whole list evaluates to one.
                // If all expressions within the and-list evaluate to approx. zero,
                // the whole list evaluates to zero.
                bool all_values_are_zero = true;
                for(size_t a=0; a<GetParamCount(); ++a)
                {
                    MinMaxTree p = GetParam(a).CalculateResultBoundaries();
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
                switch(GetParamCount())
                {
                    case 0: goto ReplaceTreeWithOne;
                    case 1: SetOpcode(cNotNot); break; // Replace self with the single operand
                    default: if(ConstantFolding_OrLogic()) goto redo;
                }
                break;
            }
            case cNot:
            {
                GetParam(0).ConstantFolding_FromLogicalParent();
                switch(GetParam(0).GetOpcode())
                {
                    case cEqual:       SetOpcode(cNEqual); goto cNot_moveparam;
                    case cNEqual:      SetOpcode(cEqual); goto cNot_moveparam;
                    case cLess:        SetOpcode(cGreaterOrEq); goto cNot_moveparam;
                    case cGreater:     SetOpcode(cLessOrEq); goto cNot_moveparam;
                    case cLessOrEq:    SetOpcode(cGreater); goto cNot_moveparam;
                    case cGreaterOrEq: SetOpcode(cLess); goto cNot_moveparam;
                    //cNotNot already handled by ConstantFolding_FromLogicalParent()
                    case cNot:         SetOpcode(cNotNot); goto cNot_moveparam;
                         { cNot_moveparam:;
                               SetParamsMove(GetParam(0).GetUniqueRef().GetParams()); goto redo; }
                    default: break;
                }

                // If the sub-expression evaluates to approx. zero, yield one.
                // If the sub-expression evaluates to approx. nonzero, yield zero.
                MinMaxTree p = GetParam(0).CalculateResultBoundaries();
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
                // The function of cNotNot is to protect a logical value from
                // changing. If the parameter is already a logical value,
                // then the cNotNot opcode is redundant.
                if(GetParam(0).IsLogicalValue())
                    goto ReplaceTreeWithParam0;

                // If the sub-expression evaluates to approx. zero, yield zero.
                // If the sub-expression evaluates to approx. nonzero, yield one.
                MinMaxTree p = GetParam(0).CalculateResultBoundaries();
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
                GetParam(0).ConstantFolding_FromLogicalParent();
                // If the If() condition begins with a cNot,
                // remove the cNot and swap the branches.
                while(GetParam(0).GetOpcode() == cNot)
                {
                    GetParam(0).Become( GetParam(0).GetParam(0) );
                    GetParam(1).swap(GetParam(2));
                }

                // If the sub-expression evaluates to approx. zero, yield param3.
                // If the sub-expression evaluates to approx. nonzero, yield param2.
                MinMaxTree p = GetParam(0).CalculateResultBoundaries();
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
                ConstantFolding_Assimilate();
                // If one sub-expression evalutes to exact zero, yield zero.
                double mul_immed_sum = 1.0;
                size_t n_mul_immeds = 0; bool needs_resynth=false;
                for(size_t a=0; a<GetParamCount(); ++a)
                {
                    if(!GetParam(a).IsImmed()) continue;
                    // ^ Only check constant values
                    double immed = GetParam(a).GetImmed();
                    if(FloatEqual(immed, 0.0)) goto ReplaceTreeWithZero;
                    if(FloatEqual(immed, 1.0)) needs_resynth = true;
                    mul_immed_sum *= immed; ++n_mul_immeds;
                }
                // Merge immeds.
                if(n_mul_immeds > 1) needs_resynth = true;
                if(needs_resynth)
                {
                    // delete immeds and add new ones
                #ifdef DEBUG_SUBSTITUTIONS
                    std::cout << "cMul: Will add new immed " << mul_immed_sum << "\n";
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
                    if(!FloatEqual(mul_immed_sum, 1.0))
                        AddParam( CodeTree(mul_immed_sum) );
                }
                if(GetParamCount() == 1)
                {
                    // Replace self with the single operand
                    goto ReplaceTreeWithParam0;
                }
                if(!GetParamCount()) goto ReplaceTreeWithOne;
                if(ConstantFolding_MulGrouping()) goto redo;
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
                    if(FloatEqual(immed, 0.0)) needs_resynth = true;
                    immed_sum += immed; ++n_immeds;
                }
                // Merge immeds.
                if(n_immeds > 1) needs_resynth = true;
                if(needs_resynth)
                {
                    // delete immeds and add new ones
                #ifdef DEBUG_SUBSTITUTIONS
                    std::cout << "cAdd: Will add new immed " << immed_sum << "\n";
                    std::cout << "In: "; FPoptimizer_Grammar::DumpTree(*this);
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
                    if(!FloatEqual(immed_sum, 0.0))
                        AddParam( CodeTree(immed_sum) );
                }
                if(GetParamCount() == 1)
                {
                    // Replace self with the single operand
                    goto ReplaceTreeWithParam0;
                }
                if(!GetParamCount()) goto ReplaceTreeWithZero;
                if(ConstantFolding_AddGrouping()) goto redo;
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
                MinMaxTree smallest_maximum;
                for(size_t a=0; a<GetParamCount(); ++a)
                {
                    MinMaxTree p = GetParam(a).CalculateResultBoundaries();
                    if(p.has_max && (!smallest_maximum.has_max || p.max < smallest_maximum.max))
                    {
                        smallest_maximum.max = p.max;
                        smallest_maximum.has_max = true;
                }   }
                if(smallest_maximum.has_max)
                    for(size_t a=GetParamCount(); a-- > 0; )
                    {
                        MinMaxTree p = GetParam(a).CalculateResultBoundaries();
                        if(p.has_min && p.min > smallest_maximum.max)
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
                MinMaxTree biggest_minimum;
                for(size_t a=0; a<GetParamCount(); ++a)
                {
                    MinMaxTree p = GetParam(a).CalculateResultBoundaries();
                    if(p.has_min && (!biggest_minimum.has_min || p.min > biggest_minimum.min))
                    {
                        biggest_minimum.min = p.min;
                        biggest_minimum.has_min = true;
                }   }
                if(biggest_minimum.has_min)
                {
                    //fprintf(stderr, "Removing all where max < %g\n", biggest_minimum.min);
                    for(size_t a=GetParamCount(); a-- > 0; )
                    {
                        MinMaxTree p = GetParam(a).CalculateResultBoundaries();
                        if(p.has_max && p.max < biggest_minimum.min)
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
            {
                if(GetParam(0).IsIdenticalTo(GetParam(1))) goto ReplaceTreeWithOne;
                /* If we know the two operands' ranges don't overlap, we get zero.
                 * The opposite is more complex and is done in .dat code.
                 */
                MinMaxTree p0 = GetParam(0).CalculateResultBoundaries();
                MinMaxTree p1 = GetParam(1).CalculateResultBoundaries();
                if((p0.has_max && p1.has_min && p1.min > p0.max)
                || (p1.has_max && p0.has_min && p0.min > p1.max))
                    goto ReplaceTreeWithZero;
                break;
            }

            case cNEqual:
            {
                if(GetParam(0).IsIdenticalTo(GetParam(1))) goto ReplaceTreeWithZero;
                /* If we know the two operands' ranges don't overlap, we get one.
                 * The opposite is more complex and is done in .dat code.
                 */
                MinMaxTree p0 = GetParam(0).CalculateResultBoundaries();
                MinMaxTree p1 = GetParam(1).CalculateResultBoundaries();
                if((p0.has_max && p1.has_min && p1.min > p0.max)
                || (p1.has_max && p0.has_min && p0.min > p1.max))
                    goto ReplaceTreeWithOne;
                break;
            }

            case cLess:
            {
                if(GetParam(0).IsIdenticalTo(GetParam(1))) goto ReplaceTreeWithZero;
                MinMaxTree p0 = GetParam(0).CalculateResultBoundaries();
                MinMaxTree p1 = GetParam(1).CalculateResultBoundaries();
                if(p0.has_max && p1.has_min && p0.max < p1.min)
                    goto ReplaceTreeWithOne; // We know p0 < p1
                if(p1.has_max && p0.has_min && p1.max <= p0.min)
                    goto ReplaceTreeWithZero; // We know p1 >= p0
                break;
            }

            case cLessOrEq:
            {
                if(GetParam(0).IsIdenticalTo(GetParam(1))) goto ReplaceTreeWithOne;
                MinMaxTree p0 = GetParam(0).CalculateResultBoundaries();
                MinMaxTree p1 = GetParam(1).CalculateResultBoundaries();
                if(p0.has_max && p1.has_min && p0.max <= p1.min)
                    goto ReplaceTreeWithOne; // We know p0 <= p1
                if(p1.has_max && p0.has_min && p1.max < p0.min)
                    goto ReplaceTreeWithZero; // We know p1 > p0
                break;
            }

            case cGreater:
            {
                if(GetParam(0).IsIdenticalTo(GetParam(1))) goto ReplaceTreeWithZero;
                // Note: Eq case not handled
                MinMaxTree p0 = GetParam(0).CalculateResultBoundaries();
                MinMaxTree p1 = GetParam(1).CalculateResultBoundaries();
                if(p0.has_max && p1.has_min && p0.max <= p1.min)
                    goto ReplaceTreeWithZero; // We know p0 <= p1
                if(p1.has_max && p0.has_min && p1.max < p0.min)
                    goto ReplaceTreeWithOne; // We know p1 > p0
                break;
            }

            case cGreaterOrEq:
            {
                if(GetParam(0).IsIdenticalTo(GetParam(1))) goto ReplaceTreeWithOne;
                // Note: Eq case not handled
                MinMaxTree p0 = GetParam(0).CalculateResultBoundaries();
                MinMaxTree p1 = GetParam(1).CalculateResultBoundaries();
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
                        FPoptimizer_Grammar::DumpTree(*this);
                        std::cout << "\n" << std::flush;
                        FPoptimizer_Grammar::DumpHashes(*this, std::cout);
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
                        FPoptimizer_Grammar::DumpTree(*this, std::cout);
                        std::cout << "\n" << std::flush;
                        FPoptimizer_Grammar::DumpHashes(*this, std::cout);
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
                if(GetParam(0).IsImmed())
                    { const_value = floor(GetParam(0).GetImmed() + 0.5);
                      goto ReplaceTreeWithConstValue; }
                break;
            case cLog2:
                if(GetParam(0).IsImmed())
                    { const_value = log(GetParam(0).GetImmed()) * CONSTANT_L2I;
                      goto ReplaceTreeWithConstValue; }
                break;
            case cLog10:
                if(GetParam(0).IsImmed())
                    { const_value = log(GetParam(0).GetImmed()) * CONSTANT_L10I;
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
                if(GetParam(0).IsImmed()
                && GetParam(1).IsImmed())
                    { const_value = atan2(GetParam(0).GetImmed(),
                                          GetParam(1).GetImmed());
                      goto ReplaceTreeWithConstValue; }
                if((p1.has_min && p1.min > 0.0)
                || (p1.has_max && p1.max < NEGATIVE_MAXIMUM)) // become atan(p0 / p1)
                {
                    CodeTree pow_tree;
                    pow_tree.SetOpcode(cPow);
                    pow_tree.AddParam(GetParam(1));
                    pow_tree.AddParam(CodeTree(-1.0));
                    pow_tree.Rehash();
                    CodeTree div_tree;
                    div_tree.SetOpcode(cMul);
                    div_tree.AddParam(GetParam(0));
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
                if(GetParam(0).IsImmed()
                && GetParam(1).IsImmed())
                    { const_value = pow(GetParam(0).GetImmed(),
                                        GetParam(1).GetImmed());
                      goto ReplaceTreeWithConstValue; }
                if(GetParam(1).IsImmed()
                && GetParam(1).GetImmed() == 1.0)
                {
                    // x^1 = x
                    goto ReplaceTreeWithParam0;
                }
                if(GetParam(0).IsImmed()
                && GetParam(0).GetImmed() == 1.0)
                {
                    // 1^x = 1
                    goto ReplaceTreeWithOne;
                }

                // 5^(20*x) = (5^20)^x
                if(GetParam(0).IsImmed()
                && GetParam(1).GetOpcode() == cMul)
                {
                    bool changes = false;
                    double base_immed = GetParam(0).GetImmed();
                    CodeTree& mulgroup = GetParam(1);
                    for(size_t a=mulgroup.GetParamCount(); a-->0; )
                        if(mulgroup.GetParam(a).IsImmed())
                        {
                            double imm = mulgroup.GetParam(a).GetImmed();
                            if(imm >= 0.0)
                            {
                                if(!changes)
                                {
                                    changes = true;
                                    mulgroup.CopyOnWrite();
                                }
                                base_immed = std::pow(base_immed, imm);
                                mulgroup.DelParam(a);
                                break; //
                            }
                        }
                    if(changes)
                    {
                        GetParam(0).Become(CodeTree(base_immed));
                        mulgroup.Rehash();
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
                    if(a == (double)(long)a && (long)a % 2 == 0     // a is an even int?
                    && !(c == (double)(long)c && (long)c % 2 == 0)) // c is not?
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
                break;
            }

            case cMod:
            {
                /* Can more be done than this? */
                if(GetParam(0).IsImmed()
                && GetParam(1).IsImmed())
                    { const_value = fmod(GetParam(0).GetImmed(),
                                         GetParam(1).GetImmed());
                      goto ReplaceTreeWithConstValue; }
                break;
            }

            /* The following opcodes are processed by GenerateFrom()
             * within fpoptimizer_bytecode_to_codetree.cc and thus
             * they will never occur in the calling context:
             */
            case cDiv: // converted into cPow y -1
            case cRDiv: // similar to above
            case cSub: // converted into cMul y -1
            case cRSub: // similar to above
            case cRad: // converted into cMul x CONSTANT_RD
            case cDeg: // converted into cMul x CONSTANT_DR
            case cSqr: // converted into cMul x x
            case cExp2: // converted into cPow 2.0 x
            case cSqrt: // converted into cPow x 0.5
            case cRSqrt: // converted into cPow x -0.5
            case cCot: // converted into cMul (cPow (cTan x) -1)
            case cSec: // converted into cMul (cPow (cCos x) -1)
            case cCsc: // converted into cMul (cPow (cSin x) -1)
            case cRPow: // converted into cPow y x
                break; /* Should never occur */

            /* The following opcodes are processed by GenerateFrom(),
             * but they may still be synthesized in the grammar matching
             * process:
             * TODO: Figure out whether we should just convert
             * these particular trees into their atomic counterparts
             */
            case cNeg: // converted into cMul x -1
            {
                if(GetParam(0).IsImmed())
                {
                    const_value = -GetParam(0).GetImmed();
                    goto ReplaceTreeWithConstValue;
                }
                break;
            }
            case cInv: // converted into cPow x -1
            {
                if(GetParam(0).IsImmed())
                {
                    const_value = 1.0 / GetParam(0).GetImmed();
                    goto ReplaceTreeWithConstValue;
                }
                break;
            }
            case cExp: // convered into cPow CONSTANT_E x
            {
                if(GetParam(0).IsImmed())
                {
                    const_value = exp( GetParam(0).GetImmed() );
                    goto ReplaceTreeWithConstValue;
                }
                break;
            }

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
    }

    MinMaxTree CodeTree::CalculateResultBoundaries() const
#ifdef DEBUG_SUBSTITUTIONS_extra_verbose
    {
        MinMaxTree tmp = CalculateResultBoundaries_do();
        std::cout << "Estimated boundaries: ";
        if(tmp.has_min) std::cout << tmp.min; else std::cout << "-inf";
        std::cout << " .. ";
        if(tmp.has_max) std::cout << tmp.max; else std::cout << "+inf";
        std::cout << ": ";
        FPoptimizer_Grammar::DumpTree(*this);
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
                if(m.has_min) { if(m.min < 0.0) m.has_min = false; else m.min = log(m.min)*CONSTANT_L2I; } // No boundaries
                if(m.has_max) { if(m.max < 0.0) m.has_max = false; else m.max = log(m.max)*CONSTANT_L2I; }
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
                if(GetParam(1).IsImmed() && FloatEqual(GetParam(1).GetImmed(), 0.0))
                {
                    // Note: This makes 0^0 evaluate into 1.
                    return MinMaxTree(1.0, 1.0); // x^0 = 1
                }
                if(GetParam(0).IsImmed() && FloatEqual(GetParam(0).GetImmed(), 0.0))
                {
                    // Note: This makes 0^0 evaluate into 0.
                    return MinMaxTree(0.0, 0.0); // 0^x = 0
                }
                if(GetParam(0).IsImmed() && FloatEqual(GetParam(0).GetImmed(), 1.0))
                {
                    return MinMaxTree(1.0, 1.0); // 1^x = 1
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
                                && !GetParam(1).IsAlwaysInteger()
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
            case cRPow: // converted into cPow y x
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
