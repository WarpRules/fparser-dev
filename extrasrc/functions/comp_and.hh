//$DEP: comp_truth

    template<typename Value_t>
    inline bool fp_and(const Value_t& a, const Value_t& b)
    {
        return fp_truth(a) && fp_truth(b);
    }
