//$DEP: comp_truth

    template<typename Value_t>
    inline const Value_t fp_not(const Value_t& b)
    {
        return Value_t(!fp_truth(b));
    }
