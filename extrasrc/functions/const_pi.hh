    template<typename Value_t>
    inline Value_t fp_const_pi() // CONSTANT_PI
    {
        return Value_t(3.1415926535897932384626433832795028841971693993751L);
    }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    template<>
    inline MpfrFloat fp_const_pi<MpfrFloat>() { return MpfrFloat::const_pi(); }
#endif
