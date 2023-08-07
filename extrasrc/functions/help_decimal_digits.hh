template<typename> int fp_value_precision_decimal_digits();

#ifndef FP_DISABLE_DOUBLE_TYPE
template<> inline int fp_value_precision_decimal_digits<double>() { return 15; }
#endif
#ifdef FP_SUPPORT_FLOAT_TYPE
template<> inline int fp_value_precision_decimal_digits<float>() { return 6; }
#endif
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
template<> inline int fp_value_precision_decimal_digits<long double>() { return 18; }
#endif
#ifdef FP_SUPPORT_LONG_INT_TYPE
template<> inline int fp_value_precision_decimal_digits<long>() { return 0; }
#endif
#ifdef FP_SUPPORT_GMP_INT_TYPE
template<> inline int fp_value_precision_decimal_digits<GmpInt>() { return 0; }
#endif
#ifdef FP_SUPPORT_COMPLEX_FLOAT_TYPE
template<> inline int fp_value_precision_decimal_digits<std::complex<float>>() { return 6; }
#endif
#ifdef FP_SUPPORT_COMPLEX_DOUBLE_TYPE
template<> inline int fp_value_precision_decimal_digits<std::complex<double>>() { return 15; }
#endif
#ifdef FP_SUPPORT_COMPLEX_LONG_DOUBLE_TYPE
template<> inline int fp_value_precision_decimal_digits<std::complex<long double>>() { return 18; }
#endif

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
template<> inline int fp_value_precision_decimal_digits<MpfrFloat>()
{
    return static_cast<int>(MpfrFloat::getCurrentDefaultMantissaBits() * 0.30103); // * log(2)/log(10)
}
#endif
