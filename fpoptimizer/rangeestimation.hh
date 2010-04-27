#ifndef FPOptimizer_RangeEstimationHH
#define FPOptimizer_RangeEstimationHH

#include "codetree.hh"

namespace FPoptimizer_CodeTree
{
    template<typename Value_t>
    struct MinMaxTree
    {
        Value_t min,max;
        bool has_min, has_max;
        MinMaxTree() : min(),max(),has_min(false),has_max(false) { }
        MinMaxTree(Value_t mi,Value_t ma): min(mi),max(ma),has_min(true),has_max(true) { }
        MinMaxTree(bool,Value_t ma): min(),max(ma),has_min(false),has_max(true) { }
        MinMaxTree(Value_t mi,bool): min(mi),max(),has_min(true),has_max(false) { }
    };

    template<typename Value_t>
    MinMaxTree<Value_t> CalculateResultBoundaries(const CodeTree<Value_t>& tree);

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

    template<typename Value_t>
    inline int GetLogicalValue(const MinMaxTree<Value_t>& p, bool abs)
    {
        if(IsLogicalTrueValue(p, abs)) return 1;
        if(IsLogicalFalseValue(p, abs)) return 0;
        return -1;
    }
}

#endif
