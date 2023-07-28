//$DEP: func_ceil
//$DEP: func_floor

    template<typename Value_t>
    inline Value_t fp_trunc(const Value_t& x)
    {
        return x < Value_t() ? fp_ceil(x) : fp_floor(x);
    }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_trunc(const MpfrFloat& x) { return MpfrFloat::trunc(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_trunc(const GmpInt& x) { return x; }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_trunc(const std::complex<T>& x)
    {
        return std::complex<T> (fp_trunc(x.real()), fp_trunc(x.imag()));
    }
#endif
