#ifndef FPOptimizer_RangeEstimationHH
#define FPOptimizer_RangeEstimationHH

#include "codetree.hh"
#include "valuerange.hh"

namespace FPoptimizer_CodeTree
{
    enum TriTruthValue { IsAlways, IsNever, Unknown };

    /* Analysis functions for a MinmaxTree */
    template<typename Value_t>
    inline bool IsLogicalTrueValue(const range<Value_t>& p, bool abs)
    {
        if(FUNCTIONPARSERTYPES::IsIntType<Value_t>::result)
        {
            if(p.min.known && p.min.val >= Value_t(1)) return true;
            if(!abs && p.max.known && p.max.val <= Value_t(-1)) return true;
        }
        else
        {
            if(p.min.known && p.min.val >= Value_t(0.5)) return true;
            if(!abs && p.max.known && p.max.val <= Value_t(-0.5)) return true;
        }
        return false;
    }

    template<typename Value_t>
    inline bool IsLogicalFalseValue(const range<Value_t>& p, bool abs)
    {
        if(FUNCTIONPARSERTYPES::IsIntType<Value_t>::result)
        {
            if(abs)
                return p.max.known && p.max.val < Value_t(1);
            else
                return p.min.known && p.max.known
                  && p.min.val > Value_t(-1) && p.max.val < Value_t(1);
        }
        else
        {
            if(abs)
                return p.max.known && p.max.val < Value_t(0.5);
            else
                return p.min.known && p.max.known
                   && p.min.val > Value_t(-0.5) && p.max.val < Value_t(0.5);
        }
    }

    /* This function calculates the minimum and maximum values
     * of the tree's result. If an estimate cannot be made,
     * min.known/max.known are indicated as false.
     */
    template<typename Value_t>
    range<Value_t> CalculateResultBoundaries(const CodeTree<Value_t>& tree);


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
        range<Value_t> p = CalculateResultBoundaries(tree);
        if(p.min.known && p.min.val >= Value_t()) return IsAlways;
        if(p.max.known && p.max.val <  Value_t()) return IsNever;
        return Unknown;
    }

    template<typename Value_t>
    inline TriTruthValue GetLogicalValue(const CodeTree<Value_t>& tree, bool abs)
    {
        range<Value_t> p = CalculateResultBoundaries(tree);
        if(IsLogicalTrueValue(p, abs)) return IsAlways;
        if(IsLogicalFalseValue(p, abs)) return IsNever;
        return Unknown;
    }
}

#endif
