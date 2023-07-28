//$DEP: const_pi
    template<typename Value_t>
    const Value_t& fp_const_rad_to_deg() // CONSTANT_RD
    {
        static const Value_t factor = Value_t(180) / fp_const_pi<Value_t>(); // to deg from rad
        return factor;
    }
