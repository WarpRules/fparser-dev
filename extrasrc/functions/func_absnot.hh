//$DEP: comp_abstruth

    template<typename Value_t>
    inline const Value_t fp_absNot(const Value_t& b)
    {
        return Value_t(!fp_absTruth(b));
    }
