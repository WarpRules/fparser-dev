    template<typename Value_t>
    inline Value_t fp_sin(const Value_t& x) { return std::sin(x); }

    inline long fp_sin(const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_sin(const MpfrFloat& x) { return MpfrFloat::sin(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_sin(const GmpInt&) { return 0; }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_sin(const std::complex<T>& x)
    {
        return std::sin(x);
        // // (exp(i*x) - exp(-i*x)) / (2i)
        // //const std::complex<T> i (T(), T(1));
        // //return (fp_exp(i*x) - fp_exp(-i*x)) * (T(-0.5)*i);
        // // Also: sin(Xr)*cosh(Xi) + cos(Xr)*sinh(Xi)
        // return std::complex<T> (
        //     fp_sin(x.real())*fp_cosh(x.imag()),
        //     fp_cos(x.real())*fp_sinh(x.imag()));
    }
#endif
