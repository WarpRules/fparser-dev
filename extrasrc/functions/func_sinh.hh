//$DEP: func_exp

    template<typename Value_t>
    inline Value_t fp_sinh(const Value_t& x) { return std::sinh(x); }

    inline long fp_sinh(const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_sinh(const MpfrFloat& x) { return MpfrFloat::sinh(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_sinh(const GmpInt&) { return 0; }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_sinh(const std::complex<T>& x)
    {
        return std::sinh(x);
        // // (exp(x) - exp(-x)) * 0.5
        // // Also: sinh(Xr)*cos(Xi) + i*cosh(Xr)*sin(Xi)
        // return std::complex<T> (
        //     fp_sinh(x.real())*fp_cos(x.imag()),
        //     fp_cosh(x.real())*fp_sin(x.imag()));
    }
#endif
