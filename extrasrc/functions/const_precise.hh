    /* Use this function instead of Value_t(v),
     * when constructing values where v is a *precise* double
     * (i.e. integer multiplied by a power of two).
     * For integers, Value_t(v) works fine.
     */
    template<typename Value_t>
    inline Value_t fp_const_preciseDouble(double v)
    {
        return Value_t(v);
    }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    template<>
    inline MpfrFloat fp_const_preciseDouble<MpfrFloat>(double v) { return MpfrFloat::makeFromDouble(v); }
#endif
