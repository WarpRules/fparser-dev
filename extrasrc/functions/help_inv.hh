    template<typename Value_t>
    inline Value_t fp_inv(const Value_t& x) { return Value_t(1) / x; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_inv(const MpfrFloat& x) { return MpfrFloat::inv(x); }
#endif

    // long, mpfr and complex use the default implementation.
