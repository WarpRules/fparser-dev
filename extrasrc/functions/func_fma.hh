    template<typename Value_t>
    inline Value_t fp_fma(const Value_t& x, const Value_t& y,
                          const Value_t& a)
    {
        return x*y+a;
    }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_fma(const MpfrFloat& x, const MpfrFloat& y,
                            const MpfrFloat& a)
    {
        return MpfrFloat::fma(x,y, a);
    }
#endif

    // long, mpfr and complex use the default implementation.
