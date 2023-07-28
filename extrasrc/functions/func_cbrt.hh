//$DEP: help_polar_scalar
//$DEP: func_log
//$DEP: func_exp

/* cbrt() is in c++11 for real, but NOT complex */
#ifdef FP_SUPPORT_CPLUSPLUS11_MATH_FUNCS
    template<typename Value_t>
    inline Value_t fp_cbrt(const Value_t& x)
    {
        return std::cbrt(x);
    }

  #ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_cbrt(const std::complex<T>& x)
    {
        // For real numbers, prefer giving a real solution
        // rather than a complex solution.
        // For example, cbrt(-3) has the following three solutions:
        //  A) 0.7211247966535 + 1.2490247864016i
        //  B) 0.7211247966535 - 1.2490247864016i
        //  C) -1.442249593307
        // exp(log(x)/3) gives A, but we prefer to give C.
        if(x.imag() == T()) return fp_cbrt(x.real());
        const std::complex<T> t(fp_log(x));
        return fp_polar_scalar(fp_exp(t.real() / T(3)), t.imag() / T(3));
    }
  #endif
#else
    template<typename Value_t>
    inline Value_t fp_cbrt(const Value_t& x)
    {
        return (x > Value_t() ?  fp_exp(fp_log( x) / Value_t(3)) :
                x < Value_t() ? -fp_exp(fp_log(-x) / Value_t(3)) :
                Value_t());
    }
#endif

    inline long fp_cbrt(const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_cbrt(const MpfrFloat& x) { return MpfrFloat::cbrt(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_cbrt(const GmpInt&) { return 0; }
#endif
