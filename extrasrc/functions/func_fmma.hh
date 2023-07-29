    template<typename Value_t>
    inline Value_t fp_fmma(const Value_t& x, const Value_t& y,
                           const Value_t& a, const Value_t& b)
    {
        return x*y+a*b;
    }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_fmma(const MpfrFloat& x, const MpfrFloat& y,
                             const MpfrFloat& a, const MpfrFloat& b)
    {
        return MpfrFloat::fmma(x,y, a,b);
    }
#endif

    // long, mpfr and complex use the default implementation.
