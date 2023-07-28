//$DEP: func_log

/* atanh() is in c++11 for real, but NOT complex */
#ifdef FP_SUPPORT_CPLUSPLUS11_MATH_FUNCS
    template<typename Value_t>
    inline Value_t fp_atanh(const Value_t& x)
    {
        return std::atanh(x);
    }

  #ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_atanh(const std::complex<T>& x)
    {
        return fp_log( (std::complex<T>(1)+x) / (std::complex<T>(1)-x))
           * std::complex<T>(0.5);
    }
  #endif
#else
    template<typename Value_t>
    inline Value_t fp_atanh(const Value_t& x)
    {
        return fp_log( (Value_t(1)+x) / (Value_t(1)-x)) * fp_const_preciseDouble<Value_t>(0.5);
        // Note: x = +1 causes division by zero
        //       x = -1 causes log(0)
        // Thus, x must not be +-1
    }
#endif

    inline long fp_atanh(const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_atanh(const MpfrFloat& x) { return MpfrFloat::atanh(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_atanh(const GmpInt&) { return 0; }
#endif
