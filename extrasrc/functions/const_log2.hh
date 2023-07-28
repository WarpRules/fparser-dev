    template<typename Value_t>
    inline Value_t fp_const_log2() // CONSTANT_L2
    {
        return Value_t(0.69314718055994530941723212145817656807550013436025525412L);
    }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    template<>
    inline MpfrFloat fp_const_log2<MpfrFloat>() { return MpfrFloat::const_log2(); }
#endif
