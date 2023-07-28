//$DEP: func_arg
//$DEP: util_fastcomplex

    template<typename Value_t>
    inline Value_t fp_log(const Value_t& x) { return std::log(x); }

    inline long fp_log(const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_log(const MpfrFloat& x) { return MpfrFloat::log(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_log(const GmpInt&) { return 0; }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_log(const std::complex<T>& x)
    {
        if(FP_ProbablyHasFastLibcComplex<T>::value)
            return std::log(x);
        // log(abs(x))        + i*arg(x)
        // log(Xr^2+Xi^2)*0.5 + i*arg(x)
        if(x.imag() == T{})
            return std::complex<T>( fp_log(fp_abs(x.real())),
                                    fp_arg(x.real()) ); // Note: Uses real-value fp_arg() here!
        return std::complex<T>(
            fp_log(std::norm(x)) * T(0.5),
            fp_arg(x).real() );
    }
#endif
