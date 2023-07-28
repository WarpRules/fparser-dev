//$DEP: const_log2
//$DEP: func_pow
//$DEP: help_polar_scalar
    template<typename Value_t>
    inline Value_t fp_exp2(const Value_t& x)
    {
        return fp_pow(Value_t(2), x);
    }

    inline long fp_exp2(const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_exp2(const MpfrFloat& x) { return MpfrFloat::exp2(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_exp2(const GmpInt&) { return 0; }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_exp2(const std::complex<T>& x)
    {
        // pow(2, x)
        // polar(2^Xr, Xi*log(2))
        return fp_polar_scalar<T> (fp_exp2(x.real()), x.imag()*fp_const_log2<T>());
    }
#endif
