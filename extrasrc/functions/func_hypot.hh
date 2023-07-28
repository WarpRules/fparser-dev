//$DEP: func_sqrt

/* hypot() is in c++11 for real, but NOT complex */
#ifdef FP_SUPPORT_CPLUSPLUS11_MATH_FUNCS
    template<typename Value_t>
    inline Value_t fp_hypot(const Value_t& x, const Value_t& y)
    {
        return std::hypot(x,y);
    }

  #ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_hypot
        (const std::complex<T>& x, const std::complex<T>& y)
    {
        return fp_sqrt(x*x + y*y);
    }
  #endif
#else
    template<typename Value_t>
    inline Value_t fp_hypot(const Value_t& x, const Value_t& y)
    {
        return fp_sqrt(x*x + y*y);
    }
#endif

    inline long fp_hypot(const long&, const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_hypot(const MpfrFloat& x, const MpfrFloat& y)
    {
        return MpfrFloat::hypot(x, y);
    }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_hypot(const GmpInt&, const GmpInt&) { return 0; }
#endif
