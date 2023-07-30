//$DEP: func_ceil
//$DEP: func_floor
//$DEP: const_precise

    template<typename Value_t>
    inline Value_t fp_int(const Value_t& x)
    {
#ifdef FP_SUPPORT_CPLUSPLUS11_MATH_FUNCS
        return std::round(x);
#else
        return x < Value_t()
        ? fp_ceil(x - fp_const_preciseDouble<Value_t>(0.5))
        : fp_floor(x + fp_const_preciseDouble<Value_t>(0.5));
#endif
    }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_int(const MpfrFloat& x) { return MpfrFloat::round(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_int(const GmpInt& x) { return x; }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_int(const std::complex<T>& x)
    {
        return std::complex<T> (fp_int(x.real()), fp_int(x.imag()));
    }
#endif
