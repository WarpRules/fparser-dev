//$DEP: comp_abstruth

    template<typename Value_t>
    inline bool fp_absNotNot(const Value_t& b)
    {
        return fp_absTruth(b);
    }
