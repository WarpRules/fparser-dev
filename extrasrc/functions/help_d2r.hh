//$DEP: const_d2r

    template<typename Value_t>
    inline Value_t DegreesToRadians(const Value_t& degrees)
    {
        return degrees * fp_const_deg_to_rad<Value_t>();
    }
