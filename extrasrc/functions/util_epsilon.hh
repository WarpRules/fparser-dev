    template<typename Value_t>
    struct Epsilon
    {
        static Value_t value;
        static Value_t defaultValue() { return 0; }
    };

    template<> inline double Epsilon<double>::defaultValue() { return 1E-12; }
    #if defined(FP_SUPPORT_FLOAT_TYPE) || defined(FP_SUPPORT_COMPLEX_FLOAT_TYPE)
    template<> inline float Epsilon<float>::defaultValue() { return 1E-5F; }
    #endif
    #if defined(FP_SUPPORT_LONG_DOUBLE_TYPE) || defined(FP_SUPPORT_COMPLEX_LONG_DOUBLE_TYPE)
    template<> inline long double Epsilon<long double>::defaultValue() { return 1E-14L; }
    #endif

    #ifdef FP_SUPPORT_COMPLEX_DOUBLE_TYPE
    template<> inline std::complex<double>
    Epsilon<std::complex<double> >::defaultValue() { return 1E-12; }
    #endif

    #ifdef FP_SUPPORT_COMPLEX_FLOAT_TYPE
    template<> inline std::complex<float>
    Epsilon<std::complex<float> >::defaultValue() { return 1E-5F; }
    #endif

    #ifdef FP_SUPPORT_COMPLEX_LONG_DOUBLE_TYPE
    template<> inline std::complex<long double>
    Epsilon<std::complex<long double> >::defaultValue() { return 1E-14L; }
    #endif

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    template<> inline MpfrFloat
    Epsilon<MpfrFloat>::defaultValue() { return MpfrFloat::someEpsilon(); }
#endif

    template<typename Value_t> Value_t Epsilon<Value_t>::value =
        Epsilon<Value_t>::defaultValue();

    //template<> inline long fp_epsilon<long>() { return 0; }
