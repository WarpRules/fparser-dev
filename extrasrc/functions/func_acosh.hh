//$DEP: func_log
//$DEP: func_sqrt

/* acosh() is in c++11 for real, but NOT complex */
#ifdef FP_SUPPORT_CPLUSPLUS11_MATH_FUNCS
    template<typename Value_t>
    inline Value_t fp_acosh(const Value_t& x)
    {
        return std::acosh(x);
    }

  #ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_acosh(const std::complex<T>& x)
    {
        return fp_log(x + fp_sqrt(x*x - std::complex<T>(1)));
    }
  #endif
#else
    template<typename Value_t>
    inline Value_t fp_acosh(const Value_t& x)
    {
        return fp_log(x + fp_sqrt(x*x - Value_t(1)));
    }
#endif

    inline long fp_acosh(const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_acosh(const MpfrFloat& x) { return MpfrFloat::acosh(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_acosh(const GmpInt&) { return 0; }
#endif
