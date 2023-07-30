//$DEP: comp_truth

    template<typename Value_t>
    inline const Value_t fp_notNot(const Value_t& b)
    {
        return Value_t(fp_truth(b));
    }
