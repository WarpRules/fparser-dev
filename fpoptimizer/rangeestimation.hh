#ifndef FPOptimizer_RangeEstimationHH
#define FPOptimizer_RangeEstimationHH

#include "codetree.hh"

namespace FPoptimizer_CodeTree
{
    enum TriTruthValue { IsAlways, IsNever, Unknown };

    /* MinMaxTree expresses the range of values that an expression can take. */
    template<typename Value_t>
    struct MinMaxTree
    {
        Value_t min,max;
        bool has_min, has_max;
        MinMaxTree() : min(),max(),has_min(false),has_max(false) { }
        MinMaxTree(Value_t mi,Value_t ma): min(mi),max(ma),has_min(true),has_max(true) { }
        MinMaxTree(bool,Value_t ma): min(),max(ma),has_min(false),has_max(true) { }
        MinMaxTree(Value_t mi,bool): min(mi),max(),has_min(true),has_max(false) { }

        void set_abs();
        void set_neg();

        template<unsigned Compare>
        void set_min_max_if
            (const Value_t& v,
             Value_t (*const func)(Value_t),
             MinMaxTree<Value_t> model = MinMaxTree<Value_t>());

        template<unsigned Compare>
        void set_min_if
            (const Value_t& v,
             Value_t (*const func)(Value_t),
             MinMaxTree<Value_t> model = MinMaxTree<Value_t>());

        template<unsigned Compare>
        void set_max_if
            (const Value_t& v,
             Value_t (*const func)(Value_t),
             MinMaxTree<Value_t> model = MinMaxTree<Value_t>());

        void set_min
            (Value_t (*const func)(Value_t),
             MinMaxTree<Value_t> model = MinMaxTree<Value_t>());

        void set_max
            (Value_t (*const func)(Value_t),
             MinMaxTree<Value_t> model = MinMaxTree<Value_t>());

        void set_min_max
            (Value_t (*const func)(Value_t),
             MinMaxTree<Value_t> model = MinMaxTree<Value_t>());
    };

    /* Analysis functions for a MinmaxTree */
    template<typename Value_t>
    inline bool IsLogicalTrueValue(const MinMaxTree<Value_t>& p, bool abs)
    {
        if(p.has_min && p.min >= 0.5) return true;
        if(!abs && p.has_max && p.max <= -0.5) return true;
        return false;
    }

    template<typename Value_t>
    inline bool IsLogicalFalseValue(const MinMaxTree<Value_t>& p, bool abs)
    {
        if(abs)
            return p.has_max && p.max < 0.5;
        else
            return p.has_min && p.has_max
               && p.min > -0.5 && p.max < 0.5;
    }

    /* This function calculates the minimum and maximum values
     * of the tree's result. If an estimate cannot be made,
     * has_min/has_max are indicated as false.
     */
    template<typename Value_t>
    MinMaxTree<Value_t> CalculateResultBoundaries(const CodeTree<Value_t>& tree);


    template<typename Value_t>
    bool IsLogicalValue(const CodeTree<Value_t>& tree);

    template<typename Value_t>
    TriTruthValue GetIntegerInfo(const CodeTree<Value_t>& tree);

    template<typename Value_t>
    inline TriTruthValue GetEvennessInfo(const CodeTree<Value_t>& tree)
    {
        if(!tree.IsImmed()) return Unknown;
        const Value_t& value = tree.GetImmed();
        if(isEvenInteger(value)) return IsAlways;
        if(isOddInteger(value)) return IsNever;
        return Unknown;
    }

    template<typename Value_t>
    inline TriTruthValue GetPositivityInfo(const CodeTree<Value_t>& tree)
    {
        MinMaxTree<Value_t> p = CalculateResultBoundaries(tree);
        if(p.has_min && p.min >= Value_t(0)) return IsAlways;
        if(p.has_max && p.max <  Value_t(0)) return IsNever;
        return Unknown;
    }

    template<typename Value_t>
    inline TriTruthValue GetLogicalValue(const CodeTree<Value_t>& tree, bool abs)
    {
        MinMaxTree<Value_t> p = CalculateResultBoundaries(tree);
        if(IsLogicalTrueValue(p, abs)) return IsAlways;
        if(IsLogicalFalseValue(p, abs)) return IsNever;
        return Unknown;
    }
}

#endif
