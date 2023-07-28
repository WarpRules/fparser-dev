    template<typename Value_t>
    inline Value_t fp_const_log10inv() // CONSTANT_L10I, CONSTANT_L10E
    {
        return Value_t(0.434294481903251827651128918916605082294397L);
    }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    template<>
    inline MpfrFloat fp_const_log10inv<MpfrFloat>() { return MpfrFloat::const_log10inv(); }
#endif
