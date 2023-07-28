    template<typename Value_t>
    inline Value_t fp_const_log10() // CONSTANT_L10, CONSTANT_L10EI
    {
        return Value_t(2.302585092994045684017991454684364207601101488628772976L);
    }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    template<>
    inline MpfrFloat fp_const_log10<MpfrFloat>() { return MpfrFloat::const_log10(); }
#endif
