//$DEP: comp_abstruth

    template<typename Value_t>
    inline const Value_t fp_absOr(const Value_t& a, const Value_t& b)
    {
        return Value_t(fp_absTruth(a) || fp_absTruth(b));
    }
