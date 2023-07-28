//$DEP: const_log2inv
//$DEP: func_log

/* log2() is in c++11 for real, but NOT complex */

#ifdef FP_SUPPORT_CPLUSPLUS11_MATH_FUNCS
    template<typename Value_t>
    inline Value_t fp_log2(const Value_t& x)
    {
        return std::log2(x);
    }

  #ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_log2(const std::complex<T>& x)
    {
        return fp_log(x) * fp_const_log2inv<T>();
    }
  #endif
#else
    template<typename Value_t>
    inline Value_t fp_log2(const Value_t& x)
    {
        return fp_log(x) * fp_const_log2inv<Value_t>();
    }
#endif

    inline long fp_log2(const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_log2(const MpfrFloat& x) { return MpfrFloat::log2(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_log2(const GmpInt&) { return 0; }
#endif
