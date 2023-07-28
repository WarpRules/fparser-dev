//$DEP: func_exp
    template<typename Value_t>
    inline Value_t fp_cosh(const Value_t& x) { return std::cosh(x); }

    inline long fp_cosh(const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_cosh(const MpfrFloat& x) { return MpfrFloat::cosh(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_cosh(const GmpInt&) { return 0; }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_cosh(const std::complex<T>& x)
    {
        return std::cosh(x);
        // // (exp(x) + exp(-x)) * 0.5
        // // Also: cosh(Xr)*cos(Xi) + i*sinh(Xr)*sin(Xi)
        // return std::complex<T> (
        //     fp_cosh(x.real())*fp_cos(x.imag()),
        //     fp_sinh(x.real())*fp_sin(x.imag()));
    }
#endif
