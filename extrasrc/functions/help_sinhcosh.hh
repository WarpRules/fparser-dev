//$DEP: func_exp
//$DEP: const_precise
//$DEP: help_sincos

    template<typename Value_t>
    inline void fp_sinhCosh(Value_t& sinhvalue, Value_t& coshvalue,
                            const Value_t& param)
    {
        const Value_t ex(fp_exp(param)), emx(fp_exp(-param));
        sinhvalue = fp_const_preciseDouble<Value_t>(0.5)*(ex-emx);
        coshvalue = fp_const_preciseDouble<Value_t>(0.5)*(ex+emx);
    }

    inline void fp_sinhCosh(long&, long&, const long&) {}

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    // Mpfr version might be best synthesized through the generic version as well.
    /*
    inline void fp_sinhCosh(MpfrFloat& sinhvalue, MpfrFloat& coshvalue,
                            const MpfrFloat& param)
    {
        const MpfrFloat paramCopy = param;
        sinhvalue = fp_sinh(paramCopy);
        coshvalue = fp_cosh(paramCopy);
    }
    */
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline void fp_sinhCosh(GmpInt&, GmpInt&, const GmpInt&) {}
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline void fp_sinhCosh(
        std::complex<T>& sinhvalue,
        std::complex<T>& coshvalue,
        const std::complex<T>& x)
    {
        T srx, crx; fp_sinhCosh(srx, crx, x.real());
        T six, cix; fp_sinCos(six, cix, x.imag());
        sinhvalue = std::complex<T>(srx*cix, crx*six);
        coshvalue = std::complex<T>(crx*cix, srx*six);
    }
#endif
