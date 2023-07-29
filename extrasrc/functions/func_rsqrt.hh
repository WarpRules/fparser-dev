//$DEP: func_sqrt

    template<typename Value_t>
    inline Value_t fp_rsqrt(const Value_t& x) { return Value_t(1) / fp_sqrt(x); }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_rsqrt(const MpfrFloat& x) { return MpfrFloat::rsqrt(x); }
#endif

    // long, mpfr and complex use the default implementation.
