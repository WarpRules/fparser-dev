//$DEP: const_r2d

    template<typename Value_t>
    inline Value_t RadiansToDegrees(const Value_t& radians)
    {
        return radians * fp_const_rad_to_deg<Value_t>();
    }
