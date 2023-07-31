//$DEP: comp_truth

    template<typename Value_t>
    inline bool fp_notNot(const Value_t& b)
    {
        return fp_truth(b);
    }

    inline bool fp_notNot(bool b) { return b; }
