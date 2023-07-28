//$DEP: help_sinhcosh
    template<typename Value_t>
    inline Value_t fp_tanh(const Value_t& x) { return std::tanh(x); }

    inline long fp_tanh(const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_tanh(const MpfrFloat& x) { return MpfrFloat::tanh(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_tanh(const GmpInt&) { return 0; }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_tanh(const std::complex<T>& x)
    {
        return std::tanh(x);
        //std::complex<T> si, co;
        //fp_sinhCosh(si, co, x);
        //return si/co;
        // // Also: (exp(2*x)-1) / (exp(2*x)+1)
        // // Also: sinh(x)/tanh(x)
        // // Also: 2/(1+exp(-2x))-1
        // const std::complex<T> exp2x=fp_exp(x+x);
        // return (exp2x-T(1)) / (exp2x+T(1));
    }
#endif
