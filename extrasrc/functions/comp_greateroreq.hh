//$DEP: comp_lessoreq
    template<typename Value_t>
    inline bool fp_greaterOrEq(const Value_t& x, const Value_t& y)
    {
        return fp_lessOrEq(y, x);
    }
