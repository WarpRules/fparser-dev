//$DEP: const_pi
//$DEP: const_precise
//$DEP: func_arg
//$DEP: func_atan
//$DEP: func_hypot

    template<typename Value_t>
    inline Value_t fp_atan2(const Value_t& x, const Value_t& y)
    {
        return std::atan2(x, y);
    }

    inline long fp_atan2(const long&, const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_atan2(const MpfrFloat& x, const MpfrFloat& y)
    {
        return MpfrFloat::atan2(x, y);
    }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_atan2(const GmpInt&, const GmpInt&) { return 0; }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_atan2(const std::complex<T>& y,
                                          const std::complex<T>& x)
    {
        if(y == T{}) return fp_arg(x);
        if(x == T{}) return fp_const_pi<T>() * fp_const_preciseDouble<T>(-0.5);
        // 2*atan(y / (sqrt(x^2+y^2) + x)    )
        // 2*atan(    (sqrt(x^2+y^2) - x) / y)
        std::complex<T> res( fp_atan(y / (fp_hypot(x,y) + x)) );
        return res+res;
    }
#endif
