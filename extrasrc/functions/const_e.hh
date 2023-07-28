    template<typename Value_t>
    inline Value_t fp_const_e() // CONSTANT_E
    {
        return Value_t(2.7182818284590452353602874713526624977572L);
    }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    template<>
    inline MpfrFloat fp_const_e<MpfrFloat>() { return MpfrFloat::const_e(); }
#endif
