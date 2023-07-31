//$DEP: comp_abstruth

    template<typename Value_t>
    inline bool fp_absNot(const Value_t& b)
    {
        return !fp_absTruth(b);
    }
