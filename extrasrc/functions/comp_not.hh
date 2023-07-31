//$DEP: comp_truth

    template<typename Value_t>
    inline bool fp_not(const Value_t& b)
    {
        return !fp_truth(b);
    }

    inline bool fp_not(bool b) { return !b; }
