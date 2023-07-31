//$DEP: comp_truth

    template<typename Value_t>
    inline bool fp_and(const Value_t& a, const Value_t& b)
    {
        return fp_truth(a) && fp_truth(b);
    }

    template<typename Value_t>
    inline bool fp_and(const Value_t& a, bool b)
    {
        return fp_truth(a) && b;
    }

    template<typename Value_t>
    inline bool fp_and(bool a, const Value_t& b)
    {
        return a && fp_truth(b);
    }

    inline bool fp_and(bool a, bool b) { return a && b; }
