//$DEP: func_log
//$DEP: func_sqrt

/* asinh() is in c++11 for real, but NOT complex */
#ifdef FP_SUPPORT_CPLUSPLUS11_MATH_FUNCS
    template<typename Value_t>
    inline Value_t fp_asinh(const Value_t& x)
    {
        return std::asinh(x);
    }

  #ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_asinh(const std::complex<T>& x)
    {
        return fp_log(x + fp_sqrt(x*x + std::complex<T>(1)));
    }
  #endif
#else
    template<typename Value_t>
    inline Value_t fp_asinh(const Value_t& x)
    {
        return fp_log(x + fp_sqrt(x*x + Value_t(1)));
    }
#endif

    inline long fp_asinh(const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_asinh(const MpfrFloat& x) { return MpfrFloat::asinh(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_asinh(const GmpInt&) { return 0; }
#endif
