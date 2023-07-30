#define FP_DECLTYPES(o) \
    o(float                     ,F,    f, float ,     "float") \
    o(double                    ,D,    d, double,     "double") \
    o(long double               ,LD,   ld,longdouble, "long double") \
    o(long int                  ,LI,   li,longint,    "long int") \
    o(MpfrFloat                 ,MPFR, mf,mpfr,       ("MpfrFloat("+std::to_string(mantissaBits)+")").c_str()) \
    o(GmpInt                    ,GI,   gi,gmpint,     "GmpInt") \
    o(std::complex<float>       ,CF,   cf,     ,      "std::complex<float>") \
    o(std::complex<double>      ,CD,   cd,     ,      "std::complex<double>") \
    o(std::complex<long double> ,CLD,  cld,    ,      "std::complex<long double>")
#ifndef FP_DISABLE_DOUBLE_TYPE
    #define rt_D(ifyes, ifno) ifyes
#else
    #define rt_D(ifyes, ifno) ifno
#endif
#ifdef FP_SUPPORT_FLOAT_TYPE
    #define rt_F(ifyes, ifno) ifyes
#else
    #define rt_F(ifyes, ifno) ifno
#endif
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
    #define rt_LD(ifyes, ifno) ifyes
#else
    #define rt_LD(ifyes, ifno) ifno
#endif
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
    #define rt_LD(ifyes, ifno) ifyes
#else
    #define rt_LD(ifyes, ifno) ifno
#endif
#ifdef FP_SUPPORT_LONG_INT_TYPE
    #define rt_LI(ifyes, ifno) ifyes
#else
    #define rt_LI(ifyes, ifno) ifno
#endif
#ifdef FP_SUPPORT_GMP_INT_TYPE
    #define rt_GI(ifyes, ifno) ifyes
#else
    #define rt_GI(ifyes, ifno) ifno
#endif
#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    #define rt_MPFR(ifyes, ifno) ifyes
#else
    #define rt_MPFR(ifyes, ifno) ifno
#endif
#ifdef FP_SUPPORT_COMPLEX_FLOAT_TYPE
    #define rt_CF(ifyes, ifno) ifyes
#else
    #define rt_CF(ifyes, ifno) ifno
#endif
#ifdef FP_SUPPORT_COMPLEX_DOUBLE_TYPE
    #define rt_CD(ifyes, ifno) ifyes
#else
    #define rt_CD(ifyes, ifno) ifno
#endif
#ifdef FP_SUPPORT_COMPLEX_LONG_DOUBLE_TYPE
    #define rt_CLD(ifyes, ifno) ifyes
#else
    #define rt_CLD(ifyes, ifno) ifno
#endif
