//$DEP: comp_less
    template<typename Value_t>
    inline bool fp_greater(const Value_t& x, const Value_t& y)
    {
        return fp_less(y, x);
    }
