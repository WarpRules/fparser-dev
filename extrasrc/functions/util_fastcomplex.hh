    template<typename T>
    struct FP_ProbablyHasFastLibcComplex: public std::false_type {};
    /* The generic sqrt() etc. implementations in libstdc++
     * are very plain and non-optimized; however, it contains
     * callbacks to libc complex math functions where possible,
     * and I suspect that those may actually be well optimized.
     * So we use std:: functions when we suspect they may be fast,
     * and otherwise we use our own optimized implementations.
     */
#if defined(__GNUC__) && defined(FP_SUPPORT_COMPLEX_NUMBERS)
    template<> struct FP_ProbablyHasFastLibcComplex<float>: public std::true_type {};
    template<> struct FP_ProbablyHasFastLibcComplex<double>: public std::true_type {};
    template<> struct FP_ProbablyHasFastLibcComplex<long double>: public std::true_type {};
#endif
