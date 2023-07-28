    template<typename Value_t>
    inline Value_t fp_cos(const Value_t& x) { return std::cos(x); }

    inline long fp_cos(const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_cos(const MpfrFloat& x) { return MpfrFloat::cos(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_cos(const GmpInt&) { return 0; }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_cos(const std::complex<T>& x)
    {
        return std::cos(x);
        // // (exp(i*x) + exp(-i*x)) / (2)
        // //const std::complex<T> i (T(), T(1));
        // //return (fp_exp(i*x) + fp_exp(-i*x)) * T(0.5);
        // // Also: cos(Xr)*cosh(Xi) - i*sin(Xr)*sinh(Xi)
        // return std::complex<T> (
        //     fp_cos(x.real())*fp_cosh(x.imag()),
        //     -fp_sin(x.real())*fp_sinh(x.imag()));
    }
#endif
