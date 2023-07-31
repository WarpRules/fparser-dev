//$DEP: comp_abstruth

    template<typename Value_t>
    inline bool fp_absOr(const Value_t& a, const Value_t& b)
    {
        return fp_absTruth(a) || fp_absTruth(b);
    }
