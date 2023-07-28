//$DEP: func_sin
//$DEP: func_cos

    template<typename Value_t>
    inline void fp_sinCos(Value_t& sinvalue, Value_t& cosvalue,
                          const Value_t& param)
    {
        // Assuming that "cosvalue" and "param" do not
        // overlap, but "sinvalue" and "param" may.
        cosvalue = fp_cos(param);
        sinvalue = fp_sin(param);
    }

#ifdef _GNU_SOURCE
    /* sincos is a GNU extension. Utilize it, if possible.
     * Otherwise, we are at the whim of the compiler recognizing
     * the opportunity, which may or may not happen.
     */
    inline void fp_sinCos(double& sin, double& cos, const double& a)
    {
        sincos(a, &sin, &cos);
    }
    inline void fp_sinCos(float& sin, float& cos, const float& a)
    {
        sincosf(a, &sin, &cos);
    }
    inline void fp_sinCos(long double& sin, long double& cos,
                          const long double& a)
    {
        sincosl(a, &sin, &cos);
    }
#endif

    inline void fp_sinCos(long&, long&, const long&) {}

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline void fp_sinCos(MpfrFloat& sin, MpfrFloat& cos, const MpfrFloat& a)
    {
        MpfrFloat::sincos(a, sin, cos);
    }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline void fp_sinCos(GmpInt&, GmpInt&, const GmpInt&) {}
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    // Forward declaration of fp_sinhCosh() to break dependency loop
    template<typename Value_t>
    inline void fp_sinhCosh(Value_t& sinhvalue, Value_t& coshvalue,
                            const Value_t& param);

    template<typename T>
    inline void fp_sinCos(
        std::complex<T>& sinvalue,
        std::complex<T>& cosvalue,
        const std::complex<T>& x)
    {
        //const std::complex<T> i (T(), T(1)), expix(fp_exp(i*x)), expmix(fp_exp((-i)*x));
        //cosvalue = (expix + expmix) * T(0.5);
        //sinvalue = (expix - expmix) * (i*T(-0.5));
        // The above expands to the following:
        T srx, crx; fp_sinCos(srx, crx, x.real());
        T six, cix; fp_sinhCosh(six, cix, x.imag());
        sinvalue = std::complex<T>(srx*cix,  crx*six);
        cosvalue = std::complex<T>(crx*cix, -srx*six);
    }
#endif
