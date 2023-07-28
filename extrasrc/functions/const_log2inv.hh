    template<typename Value_t>
    inline Value_t fp_const_log2inv() // CONSTANT_L2I, CONSTANT_L2E
    {
        return Value_t(1.442695040888963407359924681001892137426645954L);
    }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    template<>
    inline MpfrFloat fp_const_log2inv<MpfrFloat>() { return MpfrFloat::const_log2inv(); }
#endif
