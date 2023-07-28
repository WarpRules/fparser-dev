//$DEP: func_log
//$DEP: const_log10inv

/* log10() is in c++11 for BOTH real and complex */
#ifdef FP_SUPPORT_CPLUSPLUS11_MATH_FUNCS
    template<typename Value_t>
    inline Value_t fp_log10(const Value_t& x)
    {
        return std::log10(x);
    }
#else
    template<typename Value_t>
    inline Value_t fp_log10(const Value_t& x)
    {
        return fp_log(x) * fp_const_log10inv<Value_t>();
    }
#endif

    inline long fp_log10(const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_log10(const MpfrFloat& x) { return MpfrFloat::log10(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_log10(const GmpInt&) { return 0; }
#endif
