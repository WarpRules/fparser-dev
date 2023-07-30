//$DEP: util_fastcomplex
//$DEP: help_polar_scalar

    template<typename Value_t>
    inline Value_t fp_sqrt(const Value_t& x) { return std::sqrt(x); }

    inline long fp_sqrt(const long&) { return 1; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_sqrt(const MpfrFloat& x) { return MpfrFloat::sqrt(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_sqrt(const GmpInt&) { return 0; }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_sqrt(const std::complex<T>& x)
    {
        if(FP_ProbablyHasFastLibcComplex<T>::value)
            return std::sqrt(x);
        return fp_polar_scalar<T> (std::sqrt(std::abs(x)), T(0.5)*std::arg(x));
    }
#endif
