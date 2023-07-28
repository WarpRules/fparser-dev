    template<typename Value_t>
    inline const Value_t& fp_min(const Value_t& d1, const Value_t& d2)
    {
        return d1<d2 ? d1 : d2;
    }
