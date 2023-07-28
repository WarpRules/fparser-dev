//$DEP: func_log

    template<typename Value_t>
    inline Value_t fp_atan(const Value_t& x) { return std::atan(x); }

    inline long fp_atan(const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_atan(const MpfrFloat& x) { return MpfrFloat::atan(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_atan(const GmpInt&) { return 0; }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_atan(const std::complex<T>& x)
    {
        // 0.5i * (log(1-i*x) - log(1+i*x))
        // -0.5i * log( (1+i*x) / (1-i*x) )
        const std::complex<T> i            (T{}, T{1});
        const std::complex<T> minus_half_i (T{}, T{-0.5});
        const std::complex<T> one          (T{1}, T{0});
        return minus_half_i * fp_log( (one + i*x) / (one - i*x) );
        // Note: x = -1i causes division by zero
        //       x = +1i causes log(0)
        // Thus, x must not be +-1i
    }
#endif
