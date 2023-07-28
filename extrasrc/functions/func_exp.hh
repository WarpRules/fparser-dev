//$DEP: help_polar_scalar
//$DEP: util_fastcomplex

    template<typename Value_t>
    inline Value_t fp_exp(const Value_t& x) { return std::exp(x); }

    inline long fp_exp(const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_exp(const MpfrFloat& x) { return MpfrFloat::exp(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_exp(const GmpInt&) { return 0; }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_exp(const std::complex<T>& x)
    {
        if(FP_ProbablyHasFastLibcComplex<T>::value)
            return std::exp(x);
        return fp_polar_scalar<T>(fp_exp(x.real()), x.imag());
    }
#endif
