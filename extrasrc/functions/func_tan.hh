//$DEP: help_sincos

    // sign(x) * sqrt(1 / cos(x)^2 - 1), or sin(x) / cos(x)
    template<typename Value_t>
    inline Value_t fp_tan(const Value_t& x) { return std::tan(x); }

    inline long fp_tan(const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_tan(const MpfrFloat& x) { return MpfrFloat::tan(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_tan(const GmpInt&) { return 0; }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_tan(const std::complex<T>& x)
    {
        return std::tan(x);
        //std::complex<T> si, co;
        //fp_sinCos(si, co, x);
        //return si/co;
        // // (i-i*exp(2i*x)) / (exp(2i*x)+1)
        // const std::complex<T> i (T(), T(1)), exp2ix=fp_exp((2*i)*x);
        // return (i-i*exp2ix) / (exp2ix+T(1));
        // // Also: sin(x)/cos(y)
        // // return fp_sin(x)/fp_cos(x);
    }
#endif
