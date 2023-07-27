#define CONST 1.5

#ifndef FP_DISABLE_DOUBLE_TYPE
using DefaultValue_t = double;
#elif defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
using DefaultValue_t = long double;
#elif defined(FP_SUPPORT_FLOAT_TYPE)
using DefaultValue_t = float;
#elif defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
using DefaultValue_t = Mpfrfloat;
#else
#error "FunctionParserBase<double> was disabled and no viable floating point alternative has been defined"
#endif

namespace OptimizerTests
{
    template<typename T>
    inline T evaluateFunction(const T*) { return T{}; }

    DefaultValue_t evaluateFunction(const DefaultValue_t* params);
}

struct TestType
{
    const char* testName;
    const char* funcString;
    const char* paramString;
    const char* paramMin;
    const char* paramMax;
    const char* paramStep;
    unsigned paramAmount;
    bool useDegrees;
    bool hasDouble;      // If there is an equivalent
    //                      test for the "double" datatype?
    bool hasLong;        // If there is an equivalent
    //                      test for the "long" datatype?
    bool ignoreImagSign; // Is this function prone to randomizing
    //                      the imaginary component sign?
};
extern const TestType AllTests[];

template<typename Value_t>
struct RegressionTests
{
    static constexpr const unsigned short Tests[] = { (unsigned short)(~0u) };
};

constexpr unsigned customtest_index = ~0u;


namespace
{

    // Auxiliary functions
    // -------------------
    template<typename Value_t>
    inline Value_t r2d(Value_t x)
    { return x * (Value_t(180) / FUNCTIONPARSERTYPES::fp_const_pi<Value_t>()); }

    template<typename Value_t>
    inline Value_t d2r(Value_t x)
    { return x * (FUNCTIONPARSERTYPES::fp_const_pi<Value_t>() / Value_t(180)); }

    //inline double log10(double x) { return std::log(x) / std::log(10); }

    template<typename Value_t>
    inline Value_t userDefFuncSqr(const Value_t* p) { return p[0]*p[0]; }

    template<typename Value_t>
    inline Value_t userDefFuncSub(const Value_t* p) { return p[0]-p[1]; }

    template<typename Value_t>
    inline Value_t userDefFuncValue(const Value_t*) { return 10; }

    template<typename Value_t>
    inline Value_t testbedEpsilon() { return Value_t(1e-9); }

    template<>
    inline float testbedEpsilon<float>() { return 1e-3f; }

    template<>
    inline long double testbedEpsilon<long double>() { return 1e-10l; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    template<>
    inline MpfrFloat testbedEpsilon<MpfrFloat>()
    {
        static const MpfrFloat eps =
            FUNCTIONPARSERTYPES::fp_const_preciseDouble<MpfrFloat>(4.1e-19);
        return eps;
    }
#endif

#ifdef FP_SUPPORT_COMPLEX_FLOAT_TYPE
    template<>
    inline std::complex<float> testbedEpsilon<std::complex<float> >()
    { return testbedEpsilon<float>(); }
#endif

#ifdef FP_SUPPORT_COMPLEX_LONG_DOUBLE_TYPE
    template<>
    inline std::complex<long double> testbedEpsilon<std::complex<long double> >()
    { return testbedEpsilon<long double>(); }
#endif
}

#undef FP_TEST_WANT_FLOAT_TYPE
#ifdef FP_SUPPORT_FLOAT_TYPE
 #define FP_TEST_WANT_FLOAT_TYPE
#endif
#undef FP_TEST_WANT_DOUBLE_TYPE
#ifndef FP_DISABLE_DOUBLE_TYPE
 #define FP_TEST_WANT_DOUBLE_TYPE
#endif
#undef FP_TEST_WANT_LONG_DOUBLE_TYPE
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
 #define FP_TEST_WANT_LONG_DOUBLE_TYPE
#endif
#undef FP_TEST_WANT_MPFR_FLOAT_TYPE
#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
 #define FP_TEST_WANT_MPFR_FLOAT_TYPE
#endif
#undef FP_TEST_WANT_GMP_INT_TYPE
#ifdef FP_SUPPORT_GMP_INT_TYPE
 #define FP_TEST_WANT_GMP_INT_TYPE
#endif
#undef FP_TEST_WANT_LONG_INT_TYPE
#if defined(FP_SUPPORT_LONG_INT_TYPE) || defined(FP_SUPPORT_GMP_INT_TYPE)
 #define FP_TEST_WANT_LONG_INT_TYPE
#endif
#undef FP_TEST_WANT_COMPLEX_FLOAT_TYPE
#ifdef FP_SUPPORT_COMPLEX_FLOAT_TYPE
 #define FP_TEST_WANT_COMPLEX_FLOAT_TYPE
#endif
#undef FP_TEST_WANT_COMPLEX_DOUBLE_TYPE
#ifdef FP_SUPPORT_COMPLEX_DOUBLE_TYPE
 #define FP_TEST_WANT_COMPLEX_DOUBLE_TYPE
#endif
#undef FP_TEST_WANT_COMPLEX_LONG_DOUBLE_TYPE
#ifdef FP_SUPPORT_COMPLEX_LONG_DOUBLE_TYPE
 #define FP_TEST_WANT_COMPLEX_LONG_DOUBLE_TYPE
#endif

[[noreturn]] inline void unreachable_helper()
{
  #if defined(__cpp_lib_unreachable) && __cpp_lib_unreachable >= 202202L
    std::unreachable();
  #else
   #ifdef __GNUC__ /* GCC, Clang, ICC */
    __builtin_unreachable();
   #elif defined(_MSC_VER) /* MSVC */
    __assume(false);
   #endif
  #endif
}

