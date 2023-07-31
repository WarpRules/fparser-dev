//$DEP: func_log
//$DEP: func_sqrt

    // pi/2 - asin(x)
    template<typename Value_t>
    inline Value_t fp_acos(const Value_t& x) { return std::acos(x); }

    inline long fp_acos(const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_acos(const MpfrFloat& x) { return MpfrFloat::acos(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_acos(const GmpInt&) { return 0; }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_acos(const std::complex<T>& x)
    {
        // -i * log(x + i * sqrt(1 - x^2))
        const std::complex<T> i       (T{}, T{1});
        const std::complex<T> minus_i (T{}, T{-1});
        return minus_i * fp_log(x + i * fp_sqrt(std::complex<T>{1,0} - x*x));
        // Note: Real version of acos() cannot handle |x| > 1,
        //       because it would cause sqrt(negative value).
    }
#endif
