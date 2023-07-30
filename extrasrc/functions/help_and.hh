//$DEP: comp_truth

    template<typename Value_t>
    inline const Value_t fp_and(const Value_t& a, const Value_t& b)
    {
        return Value_t(fp_truth(a) && fp_truth(b));
    }
