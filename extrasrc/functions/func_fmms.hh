    template<typename Value_t>
    inline Value_t fp_fmms(const Value_t& x, const Value_t& y,
                           const Value_t& a, const Value_t& b)
    {
        return x*y-a*b;
    }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_fmms(const MpfrFloat& x, const MpfrFloat& y,
                             const MpfrFloat& a, const MpfrFloat& b)
    {
        return MpfrFloat::fmms(x,y, a,b);
    }
#endif

    // long, mpfr and complex use the default implementation.
