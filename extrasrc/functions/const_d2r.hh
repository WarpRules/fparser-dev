//$DEP: const_pi
    template<typename Value_t>
    const Value_t& fp_const_deg_to_rad() // CONSTANT_DR
    {
        static const Value_t factor = fp_const_pi<Value_t>() / Value_t(180); // to rad from deg
        return factor;
    }
