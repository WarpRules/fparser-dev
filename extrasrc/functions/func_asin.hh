//$DEP: func_log
//$DEP: func_sqrt

    // atan(x / sqrt(1 - x^2))
    template<typename Value_t>
    inline Value_t fp_asin(const Value_t& x) { return std::asin(x); }

    inline long fp_asin(const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_asin(const MpfrFloat& x) { return MpfrFloat::asin(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_asin(const GmpInt&) { return 0; }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_asin(const std::complex<T>& x)
    {
        // -i * log(i*x + sqrt(1 - x^2))
        const std::complex<T> i       (T{}, T{1});
        const std::complex<T> minus_i (T{}, T{-1});
        return minus_i * fp_log(i*x + fp_sqrt(std::complex<T>{1,0} - x*x));
        // Note: Real version of asin() cannot handle |x| > 1,
        //       because it would cause sqrt(negative value).
    }
#endif
