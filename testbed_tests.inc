template<typename Value_t>
Value_t cpp_01unit_operators_addsub_d(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/addsub_d"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
    const Value_t& z = vars[2];
#line 5 "tests/01unit_operators/addsub_d"
#line 5 "tests/01unit_operators/addsub_d"
    return x+y+x+x-z+x;
}
template<typename Value_t>
Value_t cpp_01unit_operators_addsub_i(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/addsub_i"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
    const Value_t& z = vars[2];
#line 5 "tests/01unit_operators/addsub_i"
#line 5 "tests/01unit_operators/addsub_i"
    return x+y+x+x-z+x;
}
template<typename Value_t>
Value_t cpp_01unit_operators_and_d(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/and_d"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/01unit_operators/and_d"
#line 5 "tests/01unit_operators/and_d"
    return fp_truth(x) && fp_truth(y);
}
template<typename Value_t>
Value_t cpp_01unit_operators_and_i(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/and_i"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/01unit_operators/and_i"
#line 5 "tests/01unit_operators/and_i"
    return x != Value_t(0) ? y != Value_t(0) : Value_t(0);
}
template<typename Value_t>
Value_t cpp_01unit_operators_cmpeq_d(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/cmpeq_d"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/01unit_operators/cmpeq_d"
#line 5 "tests/01unit_operators/cmpeq_d"
    return x==y;
}
template<typename Value_t>
Value_t cpp_01unit_operators_cmpeq_i(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/cmpeq_i"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/01unit_operators/cmpeq_i"
#line 5 "tests/01unit_operators/cmpeq_i"
    return x==y;
}
template<typename Value_t>
Value_t cpp_01unit_operators_cmpge_d(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/cmpge_d"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/01unit_operators/cmpge_d"
#line 5 "tests/01unit_operators/cmpge_d"
    return x>=y;
}
template<typename Value_t>
Value_t cpp_01unit_operators_cmpge_i(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/cmpge_i"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/01unit_operators/cmpge_i"
#line 5 "tests/01unit_operators/cmpge_i"
    return x>=y;
}
template<typename Value_t>
Value_t cpp_01unit_operators_cmpgt_d(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/cmpgt_d"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/01unit_operators/cmpgt_d"
#line 5 "tests/01unit_operators/cmpgt_d"
    return x>y;
}
template<typename Value_t>
Value_t cpp_01unit_operators_cmpgt_i(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/cmpgt_i"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/01unit_operators/cmpgt_i"
#line 5 "tests/01unit_operators/cmpgt_i"
    return x>y;
}
template<typename Value_t>
Value_t cpp_01unit_operators_cmple_d(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/cmple_d"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/01unit_operators/cmple_d"
#line 5 "tests/01unit_operators/cmple_d"
    return x<=y;
}
template<typename Value_t>
Value_t cpp_01unit_operators_cmple_i(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/cmple_i"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/01unit_operators/cmple_i"
#line 5 "tests/01unit_operators/cmple_i"
    return x<=y;
}
template<typename Value_t>
Value_t cpp_01unit_operators_cmplt_d(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/cmplt_d"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/01unit_operators/cmplt_d"
#line 5 "tests/01unit_operators/cmplt_d"
    return x<y;
}
template<typename Value_t>
Value_t cpp_01unit_operators_cmplt_i(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/cmplt_i"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/01unit_operators/cmplt_i"
#line 5 "tests/01unit_operators/cmplt_i"
    return x<y;
}
template<typename Value_t>
Value_t cpp_01unit_operators_cmpne_d(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/cmpne_d"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/01unit_operators/cmpne_d"
#line 5 "tests/01unit_operators/cmpne_d"
    return x!=y;
}
template<typename Value_t>
Value_t cpp_01unit_operators_cmpne_i(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/cmpne_i"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/01unit_operators/cmpne_i"
#line 5 "tests/01unit_operators/cmpne_i"
    return x!=y;
}
template<typename Value_t>
Value_t cpp_01unit_operators_divmul_d(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/divmul_d"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
    const Value_t& z = vars[2];
#line 5 "tests/01unit_operators/divmul_d"
#line 5 "tests/01unit_operators/divmul_d"
    return x*y*x*x/z*(x-Value_t(0.1));
}
template<typename Value_t>
Value_t cpp_01unit_operators_divmul_i(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/divmul_i"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
    const Value_t& z = vars[2];
#line 5 "tests/01unit_operators/divmul_i"
#line 5 "tests/01unit_operators/divmul_i"
    return x*y*x*x/(z+Value_t(3))*(x+Value_t(4));
}
template<typename Value_t>
Value_t cpp_01unit_operators_mod(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/mod"
    const Value_t& x = vars[0];
    const Value_t& z = vars[1];
#line 5 "tests/01unit_operators/mod"
#line 5 "tests/01unit_operators/mod"
    return fmod(x,z);
}
template<typename Value_t>
Value_t cpp_01unit_operators_mod_i(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/mod_i"
    const Value_t& x = vars[0];
    const Value_t& z = vars[1];
#line 5 "tests/01unit_operators/mod_i"
#line 5 "tests/01unit_operators/mod_i"
    return x%(z+Value_t(3));
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_01unit_operators_modf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/01unit_operators/modf"
    const Value_t& x = vars[0];
    const Value_t& z = vars[1];
#line 6 "tests/01unit_operators/modf"
#line 6 "tests/01unit_operators/modf"
    return fmodf(x,z);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_01unit_operators_modl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/01unit_operators/modl"
    const Value_t& x = vars[0];
    const Value_t& z = vars[1];
#line 6 "tests/01unit_operators/modl"
#line 6 "tests/01unit_operators/modl"
    return fmodl(x,z);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_01unit_operators_modm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/01unit_operators/modm"
    const Value_t& x = vars[0];
    const Value_t& z = vars[1];
#line 6 "tests/01unit_operators/modm"
#line 6 "tests/01unit_operators/modm"
    return x%z;
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_01unit_operators_not_d(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/not_d"
    const Value_t& x = vars[0];
#line 5 "tests/01unit_operators/not_d"
#line 5 "tests/01unit_operators/not_d"
    return Value_t( fp_abs(x) < Value_t(0.5) );
}
template<typename Value_t>
Value_t cpp_01unit_operators_not_i(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/not_i"
    const Value_t& x = vars[0];
#line 5 "tests/01unit_operators/not_i"
#line 5 "tests/01unit_operators/not_i"
    return Value_t(x==Value_t(0));
}
template<typename Value_t>
Value_t cpp_01unit_operators_notnot_d(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/notnot_d"
    const Value_t& x = vars[0];
#line 5 "tests/01unit_operators/notnot_d"
#line 5 "tests/01unit_operators/notnot_d"
    return Value_t( fp_abs(x) >= Value_t(0.5) );
}
template<typename Value_t>
Value_t cpp_01unit_operators_notnot_i(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/notnot_i"
    const Value_t& x = vars[0];
#line 5 "tests/01unit_operators/notnot_i"
#line 5 "tests/01unit_operators/notnot_i"
    return Value_t(x!=Value_t(0));
}
template<typename Value_t>
Value_t cpp_01unit_operators_or_d(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/or_d"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/01unit_operators/or_d"
#line 5 "tests/01unit_operators/or_d"
    return fp_truth(x) || fp_truth(y);
}
template<typename Value_t>
Value_t cpp_01unit_operators_or_i(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/01unit_operators/or_i"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/01unit_operators/or_i"
#line 5 "tests/01unit_operators/or_i"
    return x == Value_t(0) ? y != Value_t(0) : Value_t(1);
}
template<typename Value_t>
Value_t cpp_02unit_functions_abs_d(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/abs_d"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/abs_d"
#line 5 "tests/02unit_functions/abs_d"
    return x<Value_t(0) ? -x : x;
}
template<typename Value_t>
Value_t cpp_02unit_functions_abs_i(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/abs_i"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/abs_i"
#line 5 "tests/02unit_functions/abs_i"
    return x<Value_t(0) ? -x : x;
}
template<typename Value_t>
Value_t cpp_02unit_functions_acos(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/acos"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/acos"
#line 5 "tests/02unit_functions/acos"
    return acos(x);
}
template<typename Value_t>
Value_t cpp_02unit_functions_acos_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/acos_deg"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/acos_deg"
#line 6 "tests/02unit_functions/acos_deg"
    return r2d(acos(x));
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_acosf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/acosf"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/acosf"
#line 6 "tests/02unit_functions/acosf"
    return acosf(x);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_acosf_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/acosf_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/acosf_deg"
#line 7 "tests/02unit_functions/acosf_deg"
    return r2d(acosf(x));
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_acosh(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/acosh"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/acosh"
#line 5 "tests/02unit_functions/acosh"
    return log(x + sqrt(x*x - Value_t(1)));
}
template<typename Value_t>
Value_t cpp_02unit_functions_acosh_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/acosh_deg"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/acosh_deg"
#line 6 "tests/02unit_functions/acosh_deg"
    return r2d(acosh(x));
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_acoshf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/acoshf"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/acoshf"
#line 6 "tests/02unit_functions/acoshf"
    return logf(x + sqrtf(x*x - Value_t(1)));
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_acoshf_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/acoshf_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/acoshf_deg"
#line 7 "tests/02unit_functions/acoshf_deg"
    return r2d(acoshf(x));
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_acoshl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/acoshl"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/acoshl"
#line 6 "tests/02unit_functions/acoshl"
    return logl(x + sqrtl(x*x - Value_t(1)));
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_acoshl_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/acoshl_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/acoshl_deg"
#line 7 "tests/02unit_functions/acoshl_deg"
    return r2d(acoshl(x));
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_acoshm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/acoshm"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/acoshm"
#line 6 "tests/02unit_functions/acoshm"
    return MpfrFloat::acosh(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_acoshm_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/acoshm_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/acoshm_deg"
#line 7 "tests/02unit_functions/acoshm_deg"
    return r2d(MpfrFloat::acosh(x));
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_acosl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/acosl"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/acosl"
#line 6 "tests/02unit_functions/acosl"
    return acosl(x);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_acosl_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/acosl_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/acosl_deg"
#line 7 "tests/02unit_functions/acosl_deg"
    return r2d(acosl(x));
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_acosm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/acosm"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/acosm"
#line 6 "tests/02unit_functions/acosm"
    return MpfrFloat::acos(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_acosm_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/acosm_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/acosm_deg"
#line 7 "tests/02unit_functions/acosm_deg"
    return r2d(MpfrFloat::acos(x));
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_asin(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/asin"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/asin"
#line 5 "tests/02unit_functions/asin"
    return asin(x);
}
template<typename Value_t>
Value_t cpp_02unit_functions_asin_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/asin_deg"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/asin_deg"
#line 6 "tests/02unit_functions/asin_deg"
    return r2d(asin(x));
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_asinf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/asinf"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/asinf"
#line 6 "tests/02unit_functions/asinf"
    return asinf(x);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_asinf_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/asinf_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/asinf_deg"
#line 7 "tests/02unit_functions/asinf_deg"
    return r2d(asinf(x));
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_asinh(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/asinh"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/asinh"
#line 5 "tests/02unit_functions/asinh"
    return log(x + sqrt(x*x+Value_t(1)));
}
template<typename Value_t>
Value_t cpp_02unit_functions_asinh_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/asinh_deg"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/asinh_deg"
#line 6 "tests/02unit_functions/asinh_deg"
    return r2d(asinh(x));
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_asinhf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/asinhf"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/asinhf"
#line 6 "tests/02unit_functions/asinhf"
    return logf(x + sqrtf(x*x+Value_t(1)));
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_asinhf_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/asinhf_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/asinhf_deg"
#line 7 "tests/02unit_functions/asinhf_deg"
    return r2d(asinhf(x));
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_asinhl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/asinhl"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/asinhl"
#line 6 "tests/02unit_functions/asinhl"
    return logl(x + sqrtl(x*x+Value_t(1)));
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_asinhl_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/asinhl_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/asinhl_deg"
#line 7 "tests/02unit_functions/asinhl_deg"
    return r2d(asinhl(x));
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_asinhm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/asinhm"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/asinhm"
#line 6 "tests/02unit_functions/asinhm"
    return MpfrFloat::asinh(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_asinhm_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/asinhm_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/asinhm_deg"
#line 7 "tests/02unit_functions/asinhm_deg"
    return r2d(MpfrFloat::asinh(x));
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_asinl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/asinl"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/asinl"
#line 6 "tests/02unit_functions/asinl"
    return asinl(x);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_asinl_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/asinl_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/asinl_deg"
#line 7 "tests/02unit_functions/asinl_deg"
    return r2d(asinl(x));
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_asinm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/asinm"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/asinm"
#line 6 "tests/02unit_functions/asinm"
    return MpfrFloat::asin(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_asinm_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/asinm_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/asinm_deg"
#line 7 "tests/02unit_functions/asinm_deg"
    return r2d(MpfrFloat::asin(x));
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_atan(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/atan"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/atan"
#line 5 "tests/02unit_functions/atan"
    return atan(x);
}
template<typename Value_t>
Value_t cpp_02unit_functions_atan2(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/atan2"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/02unit_functions/atan2"
#line 5 "tests/02unit_functions/atan2"
    return atan2(x,y);
}
template<typename Value_t>
Value_t cpp_02unit_functions_atan2_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/atan2_deg"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 6 "tests/02unit_functions/atan2_deg"
#line 6 "tests/02unit_functions/atan2_deg"
    return r2d(atan2(x,y));
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_atan2f(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/atan2f"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 6 "tests/02unit_functions/atan2f"
#line 6 "tests/02unit_functions/atan2f"
    return atan2f(x,y);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_atan2f_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/atan2f_deg"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 7 "tests/02unit_functions/atan2f_deg"
#line 7 "tests/02unit_functions/atan2f_deg"
    return r2d(atan2f(x,y));
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_atan2l(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/atan2l"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 6 "tests/02unit_functions/atan2l"
#line 6 "tests/02unit_functions/atan2l"
    return atan2l(x,y);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_atan2l_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/atan2l_deg"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 7 "tests/02unit_functions/atan2l_deg"
#line 7 "tests/02unit_functions/atan2l_deg"
    return r2d(atan2l(x,y));
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_atan2m(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/atan2m"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 6 "tests/02unit_functions/atan2m"
#line 6 "tests/02unit_functions/atan2m"
    return MpfrFloat::atan2(x,y);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_atan2m_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/atan2m_deg"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 7 "tests/02unit_functions/atan2m_deg"
#line 7 "tests/02unit_functions/atan2m_deg"
    return r2d(MpfrFloat::atan2(x,y));
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_atan_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/atan_deg"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/atan_deg"
#line 6 "tests/02unit_functions/atan_deg"
    return r2d(atan(x));
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_atanf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/atanf"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/atanf"
#line 6 "tests/02unit_functions/atanf"
    return atanf(x);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_atanf_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/atanf_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/atanf_deg"
#line 7 "tests/02unit_functions/atanf_deg"
    return r2d(atanf(x));
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_atanh(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/atanh"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/atanh"
#line 5 "tests/02unit_functions/atanh"
    return log( (Value_t(1)+x) / (Value_t(1)-x) ) * Value_t(0.5);
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_atanhf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/atanhf"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/atanhf"
#line 6 "tests/02unit_functions/atanhf"
    return logf( (Value_t(1)+x) / (Value_t(1)-x) ) * Value_t(0.5);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_atanhl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/atanhl"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/atanhl"
#line 6 "tests/02unit_functions/atanhl"
    return logl( (Value_t(1)+x) / (Value_t(1)-x) ) * Value_t(0.5);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_atanhm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/atanhm"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/atanhm"
#line 6 "tests/02unit_functions/atanhm"
    return MpfrFloat::atanh(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_atanl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/atanl"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/atanl"
#line 6 "tests/02unit_functions/atanl"
    return atanl(x);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_atanl_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/atanl_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/atanl_deg"
#line 7 "tests/02unit_functions/atanl_deg"
    return r2d(atanl(x));
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_atanm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/atanm"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/atanm"
#line 6 "tests/02unit_functions/atanm"
    return MpfrFloat::atan(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_atanm_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/atanm_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/atanm_deg"
#line 7 "tests/02unit_functions/atanm_deg"
    return r2d(MpfrFloat::atan(x));
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_cbrt(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/cbrt"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/cbrt"
#line 5 "tests/02unit_functions/cbrt"
    return cbrt(x);
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_cbrtf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/cbrtf"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/cbrtf"
#line 6 "tests/02unit_functions/cbrtf"
    return cbrtf(x);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_cbrtl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/cbrtl"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/cbrtl"
#line 6 "tests/02unit_functions/cbrtl"
    return cbrtl(x);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_cbrtm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/cbrtm"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/cbrtm"
#line 6 "tests/02unit_functions/cbrtm"
    return MpfrFloat::cbrt(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_ceil(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/ceil"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/ceil"
#line 5 "tests/02unit_functions/ceil"
    return ceil(x);
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_ceilf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/ceilf"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/ceilf"
#line 6 "tests/02unit_functions/ceilf"
    return ceilf(x);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_ceill(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/ceill"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/ceill"
#line 6 "tests/02unit_functions/ceill"
    return ceill(x);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_ceilm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/ceilm"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/ceilm"
#line 6 "tests/02unit_functions/ceilm"
    return MpfrFloat::ceil(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_cos(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/cos"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/cos"
#line 5 "tests/02unit_functions/cos"
    return cos(x);
}
template<typename Value_t>
Value_t cpp_02unit_functions_cos_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/cos_deg"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/cos_deg"
#line 6 "tests/02unit_functions/cos_deg"
    return cos(d2r(x));
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_cosf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/cosf"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/cosf"
#line 6 "tests/02unit_functions/cosf"
    return cosf(x);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_cosf_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/cosf_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/cosf_deg"
#line 7 "tests/02unit_functions/cosf_deg"
    return cosf(d2r(x));
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_cosh(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/cosh"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/cosh"
#line 5 "tests/02unit_functions/cosh"
    return cosh(x);
}
template<typename Value_t>
Value_t cpp_02unit_functions_cosh_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/cosh_deg"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/cosh_deg"
#line 6 "tests/02unit_functions/cosh_deg"
    return cosh(d2r(x));
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_coshf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/coshf"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/coshf"
#line 6 "tests/02unit_functions/coshf"
    return coshf(x);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_coshf_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/coshf_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/coshf_deg"
#line 7 "tests/02unit_functions/coshf_deg"
    return coshf(d2r(x));
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_coshl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/coshl"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/coshl"
#line 6 "tests/02unit_functions/coshl"
    return coshl(x);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_coshl_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/coshl_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/coshl_deg"
#line 7 "tests/02unit_functions/coshl_deg"
    return coshl(d2r(x));
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_coshm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/coshm"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/coshm"
#line 6 "tests/02unit_functions/coshm"
    return MpfrFloat::cosh(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_coshm_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/coshm_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/coshm_deg"
#line 7 "tests/02unit_functions/coshm_deg"
    return MpfrFloat::cosh(d2r(x));
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_cosl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/cosl"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/cosl"
#line 6 "tests/02unit_functions/cosl"
    return cosl(x);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_cosl_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/cosl_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/cosl_deg"
#line 7 "tests/02unit_functions/cosl_deg"
    return cosl(d2r(x));
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_cosm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/cosm"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/cosm"
#line 6 "tests/02unit_functions/cosm"
    return MpfrFloat::cos(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_cosm_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/cosm_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/cosm_deg"
#line 7 "tests/02unit_functions/cosm_deg"
    return MpfrFloat::cos(d2r(x));
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_exp(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/exp"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/exp"
#line 5 "tests/02unit_functions/exp"
    return exp(x);
}
template<typename Value_t>
Value_t cpp_02unit_functions_exp2(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/exp2"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/exp2"
#line 5 "tests/02unit_functions/exp2"
    return exp(x*Value_t(0.693147180559945309417232121458176568075500134));
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_exp2f(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/exp2f"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/exp2f"
#line 6 "tests/02unit_functions/exp2f"
    return expf(x*Value_t(0.693147180559945309417232121458176568075500134f));
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_exp2l(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/exp2l"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/exp2l"
#line 6 "tests/02unit_functions/exp2l"
    return expl(x*Value_t(0.693147180559945309417232121458176568075500134l));
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_exp2m(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/exp2m"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/exp2m"
#line 6 "tests/02unit_functions/exp2m"
    return MpfrFloat::exp2(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_expf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/expf"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/expf"
#line 6 "tests/02unit_functions/expf"
    return expf(x);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_expl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/expl"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/expl"
#line 6 "tests/02unit_functions/expl"
    return expl(x);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_expm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/expm"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/expm"
#line 6 "tests/02unit_functions/expm"
    return MpfrFloat::exp(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_floor(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/floor"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/floor"
#line 5 "tests/02unit_functions/floor"
    return floor(x);
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_floorf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/floorf"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/floorf"
#line 6 "tests/02unit_functions/floorf"
    return floorf(x);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_floorl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/floorl"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/floorl"
#line 6 "tests/02unit_functions/floorl"
    return floorl(x);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_floorm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/floorm"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/floorm"
#line 6 "tests/02unit_functions/floorm"
    return MpfrFloat::floor(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_hypot(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/hypot"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/02unit_functions/hypot"
#line 5 "tests/02unit_functions/hypot"
    return sqrt(x*x + y*y);
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_hypotf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/hypotf"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 6 "tests/02unit_functions/hypotf"
#line 6 "tests/02unit_functions/hypotf"
    return sqrtf(x*x + y*y);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_hypotl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/hypotl"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 6 "tests/02unit_functions/hypotl"
#line 6 "tests/02unit_functions/hypotl"
    return sqrtl(x*x + y*y);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_hypotm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/hypotm"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 6 "tests/02unit_functions/hypotm"
#line 6 "tests/02unit_functions/hypotm"
    return MpfrFloat::hypot(x,y);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_if_d(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/if_d"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
    const Value_t& z = vars[2];
#line 5 "tests/02unit_functions/if_d"
#line 5 "tests/02unit_functions/if_d"
    return fp_abs(x) >= Value_t(0.5) ? y : z;
}
template<typename Value_t>
Value_t cpp_02unit_functions_if_i(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/if_i"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
    const Value_t& z = vars[2];
#line 5 "tests/02unit_functions/if_i"
#line 5 "tests/02unit_functions/if_i"
    return (x!=Value_t(0)) ? y : z;
}
template<typename Value_t>
Value_t cpp_02unit_functions_log(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/log"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/log"
#line 5 "tests/02unit_functions/log"
    return log(x);
}
template<typename Value_t>
Value_t cpp_02unit_functions_log2(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/log2"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/log2"
#line 5 "tests/02unit_functions/log2"
    return log(x)*Value_t(1.4426950408889634073599246810018921374266);
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_log2f(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/log2f"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/log2f"
#line 6 "tests/02unit_functions/log2f"
    return logf(x)*Value_t(1.4426950408889634073599246810018921374266f);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_log2l(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/log2l"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/log2l"
#line 6 "tests/02unit_functions/log2l"
    return logl(x)*Value_t(1.4426950408889634073599246810018921374266l);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_log2m(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/log2m"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/log2m"
#line 6 "tests/02unit_functions/log2m"
    return MpfrFloat::log2(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_log10(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/log10"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/log10"
#line 5 "tests/02unit_functions/log10"
    return log(x)*Value_t(0.43429448190325182765112891891660508229);
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_log10f(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/log10f"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/log10f"
#line 6 "tests/02unit_functions/log10f"
    return logf(x)*Value_t(0.43429448190325182765112891891660508229f);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_log10l(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/log10l"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/log10l"
#line 6 "tests/02unit_functions/log10l"
    return logl(x)*Value_t(0.43429448190325182765112891891660508229l);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_log10m(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/log10m"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/log10m"
#line 6 "tests/02unit_functions/log10m"
    return MpfrFloat::log10(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_logf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/logf"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/logf"
#line 6 "tests/02unit_functions/logf"
    return logf(x);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_logl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/logl"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/logl"
#line 6 "tests/02unit_functions/logl"
    return logl(x);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_logm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/logm"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/logm"
#line 6 "tests/02unit_functions/logm"
    return MpfrFloat::log(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_max(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/max"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/02unit_functions/max"
#line 5 "tests/02unit_functions/max"
    return x>y ? x : y;
}
template<typename Value_t>
Value_t cpp_02unit_functions_min(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/min"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/02unit_functions/min"
#line 5 "tests/02unit_functions/min"
    return x<y ? x : y;
}
template<typename Value_t>
Value_t cpp_02unit_functions_pow_neg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/pow_neg"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/02unit_functions/pow_neg"
#line 5 "tests/02unit_functions/pow_neg"
    return pow(-x*Value_t(0.25),y);
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_pow_negf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/pow_negf"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 6 "tests/02unit_functions/pow_negf"
#line 6 "tests/02unit_functions/pow_negf"
    return powf(-x*Value_t(0.25),y);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_pow_negl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/pow_negl"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 6 "tests/02unit_functions/pow_negl"
#line 6 "tests/02unit_functions/pow_negl"
    return powl(-x*Value_t(0.25),y);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_pow_negm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/pow_negm"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 6 "tests/02unit_functions/pow_negm"
#line 6 "tests/02unit_functions/pow_negm"
    return MpfrFloat::pow(-x*Value_t(0.25),y);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_pow_pos(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/pow_pos"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/02unit_functions/pow_pos"
#line 5 "tests/02unit_functions/pow_pos"
    return pow(x,y);
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_pow_posf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/pow_posf"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 6 "tests/02unit_functions/pow_posf"
#line 6 "tests/02unit_functions/pow_posf"
    return powf(x,y);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_pow_posl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/pow_posl"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 6 "tests/02unit_functions/pow_posl"
#line 6 "tests/02unit_functions/pow_posl"
    return powl(x,y);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_pow_posm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/pow_posm"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 6 "tests/02unit_functions/pow_posm"
#line 6 "tests/02unit_functions/pow_posm"
    return MpfrFloat::pow(x,y);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_sin(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/sin"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/sin"
#line 5 "tests/02unit_functions/sin"
    return sin(x);
}
template<typename Value_t>
Value_t cpp_02unit_functions_sin_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/sin_deg"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/sin_deg"
#line 6 "tests/02unit_functions/sin_deg"
    return sin(d2r(x));
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_sinf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/sinf"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/sinf"
#line 6 "tests/02unit_functions/sinf"
    return sinf(x);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_sinf_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/sinf_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/sinf_deg"
#line 7 "tests/02unit_functions/sinf_deg"
    return sinf(d2r(x));
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_sinh(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/sinh"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/sinh"
#line 5 "tests/02unit_functions/sinh"
    return sinh(x);
}
template<typename Value_t>
Value_t cpp_02unit_functions_sinh_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/sinh_deg"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/sinh_deg"
#line 6 "tests/02unit_functions/sinh_deg"
    return sinh(d2r(x));
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_sinhf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/sinhf"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/sinhf"
#line 6 "tests/02unit_functions/sinhf"
    return sinhf(x);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_sinhf_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/sinhf_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/sinhf_deg"
#line 7 "tests/02unit_functions/sinhf_deg"
    return sinhf(d2r(x));
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_sinhl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/sinhl"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/sinhl"
#line 6 "tests/02unit_functions/sinhl"
    return sinhl(x);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_sinhl_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/sinhl_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/sinhl_deg"
#line 7 "tests/02unit_functions/sinhl_deg"
    return sinhl(d2r(x));
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_sinhm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/sinhm"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/sinhm"
#line 6 "tests/02unit_functions/sinhm"
    return MpfrFloat::sinh(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_sinhm_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/sinhm_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/sinhm_deg"
#line 7 "tests/02unit_functions/sinhm_deg"
    return MpfrFloat::sinh(d2r(x));
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_sinl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/sinl"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/sinl"
#line 6 "tests/02unit_functions/sinl"
    return sinl(x);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_sinl_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/sinl_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/sinl_deg"
#line 7 "tests/02unit_functions/sinl_deg"
    return sinl(d2r(x));
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_sinm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/sinm"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/sinm"
#line 6 "tests/02unit_functions/sinm"
    return MpfrFloat::sin(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_sinm_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/sinm_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/sinm_deg"
#line 7 "tests/02unit_functions/sinm_deg"
    return MpfrFloat::sin(d2r(x));
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_sqrt(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/sqrt"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/sqrt"
#line 5 "tests/02unit_functions/sqrt"
    return sqrt(x);
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_sqrtf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/sqrtf"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/sqrtf"
#line 6 "tests/02unit_functions/sqrtf"
    return sqrtf(x);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_sqrtl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/sqrtl"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/sqrtl"
#line 6 "tests/02unit_functions/sqrtl"
    return sqrtl(x);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_sqrtm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/sqrtm"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/sqrtm"
#line 6 "tests/02unit_functions/sqrtm"
    return MpfrFloat::sqrt(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_tan(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/tan"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/tan"
#line 5 "tests/02unit_functions/tan"
    return tan(x);
}
template<typename Value_t>
Value_t cpp_02unit_functions_tan_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/tan_deg"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/tan_deg"
#line 6 "tests/02unit_functions/tan_deg"
    return tan(d2r(x));
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_tanf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/tanf"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/tanf"
#line 6 "tests/02unit_functions/tanf"
    return tanf(x);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_tanf_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/tanf_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/tanf_deg"
#line 7 "tests/02unit_functions/tanf_deg"
    return tanf(d2r(x));
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_tanh(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/tanh"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/tanh"
#line 5 "tests/02unit_functions/tanh"
    return tanh(x);
}
template<typename Value_t>
Value_t cpp_02unit_functions_tanh_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/tanh_deg"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/tanh_deg"
#line 6 "tests/02unit_functions/tanh_deg"
    return tanh(d2r(x));
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_tanhf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/tanhf"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/tanhf"
#line 6 "tests/02unit_functions/tanhf"
    return tanhf(x);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_tanhf_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/tanhf_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/tanhf_deg"
#line 7 "tests/02unit_functions/tanhf_deg"
    return tanhf(d2r(x));
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_tanhl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/tanhl"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/tanhl"
#line 6 "tests/02unit_functions/tanhl"
    return tanhl(x);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_tanhl_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/tanhl_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/tanhl_deg"
#line 7 "tests/02unit_functions/tanhl_deg"
    return tanhl(d2r(x));
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_tanhm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/tanhm"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/tanhm"
#line 6 "tests/02unit_functions/tanhm"
    return MpfrFloat::tanh(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_tanhm_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/tanhm_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/tanhm_deg"
#line 7 "tests/02unit_functions/tanhm_deg"
    return MpfrFloat::tanh(d2r(x));
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_tanl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/tanl"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/tanl"
#line 6 "tests/02unit_functions/tanl"
    return tanl(x);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_tanl_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/tanl_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/tanl_deg"
#line 7 "tests/02unit_functions/tanl_deg"
    return tanl(d2r(x));
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_tanm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/tanm"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/tanm"
#line 6 "tests/02unit_functions/tanm"
    return MpfrFloat::tan(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_tanm_deg(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/02unit_functions/tanm_deg"
    const Value_t& x = vars[0];
#line 7 "tests/02unit_functions/tanm_deg"
#line 7 "tests/02unit_functions/tanm_deg"
    return MpfrFloat::tan(d2r(x));
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_02unit_functions_trunc(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/02unit_functions/trunc"
    const Value_t& x = vars[0];
#line 5 "tests/02unit_functions/trunc"
#line 5 "tests/02unit_functions/trunc"
    return trunc(x);
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_truncf(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/truncf"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/truncf"
#line 6 "tests/02unit_functions/truncf"
    return truncf(x);
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_truncl(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/truncl"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/truncl"
#line 6 "tests/02unit_functions/truncl"
    return truncl(x);
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_02unit_functions_truncm(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/02unit_functions/truncm"
    const Value_t& x = vars[0];
#line 6 "tests/02unit_functions/truncm"
#line 6 "tests/02unit_functions/truncm"
    return MpfrFloat::trunc(x);
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_03unit_constants_e_d(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/03unit_constants/e_d"
    const Value_t& x = vars[0];
#line 5 "tests/03unit_constants/e_d"
#line 5 "tests/03unit_constants/e_d"
    return exp(x*Value_t(0)+Value_t(1));
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_03unit_constants_e_f(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/03unit_constants/e_f"
    const Value_t& x = vars[0];
#line 6 "tests/03unit_constants/e_f"
#line 6 "tests/03unit_constants/e_f"
    return expf(x*Value_t(0)+Value_t(1));
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_03unit_constants_e_ld(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/03unit_constants/e_ld"
    const Value_t& x = vars[0];
#line 6 "tests/03unit_constants/e_ld"
#line 6 "tests/03unit_constants/e_ld"
    return expl(x*Value_t(0)+Value_t(1));
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_03unit_constants_e_mpfr(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/03unit_constants/e_mpfr"
    const Value_t& x = vars[0];
#line 6 "tests/03unit_constants/e_mpfr"
#line 6 "tests/03unit_constants/e_mpfr"
    return MpfrFloat::exp(x*Value_t(0)+Value_t(1));
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_03unit_constants_pi_d(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/03unit_constants/pi_d"
    const Value_t& x = vars[0];
#line 5 "tests/03unit_constants/pi_d"
#line 5 "tests/03unit_constants/pi_d"
    return atan2(x*Value_t(0),-Value_t(1));
}
#if defined(FP_SUPPORT_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_03unit_constants_pi_f(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/03unit_constants/pi_f"
    const Value_t& x = vars[0];
#line 6 "tests/03unit_constants/pi_f"
#line 6 "tests/03unit_constants/pi_f"
    return atan2f(x*Value_t(0),-Value_t(1));
}
#endif /* defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
template<typename Value_t>
Value_t cpp_03unit_constants_pi_ld(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/03unit_constants/pi_ld"
    const Value_t& x = vars[0];
#line 6 "tests/03unit_constants/pi_ld"
#line 6 "tests/03unit_constants/pi_ld"
    return atan2l(x*Value_t(0),-Value_t(1));
}
#endif /* defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
template<typename Value_t>
Value_t cpp_03unit_constants_pi_mpfr(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/03unit_constants/pi_mpfr"
    const Value_t& x = vars[0];
#line 6 "tests/03unit_constants/pi_mpfr"
#line 6 "tests/03unit_constants/pi_mpfr"
    return MpfrFloat::atan2(x*Value_t(0),-Value_t(1));
}
#endif /* defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
template<typename Value_t>
Value_t cpp_50regressions_36(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/50regressions/36"
    const Value_t& x = vars[0];
#line 7 "tests/50regressions/36"
#line 7 "tests/50regressions/36"
    return -(x<Value_t(0) ? x : -x) + -(x<Value_t(5) ? Value_t(2) : Value_t(3));
}
template<typename Value_t>
Value_t cpp_50regressions_42(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/50regressions/42"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 7 "tests/50regressions/42"
#line 7 "tests/50regressions/42"
    return fp_sqrt(x*x) + Value_t(1.5)*(fp_pow(y*y, Value_t(0.25))) + fp_hypot(x,y);
}
template<typename Value_t>
Value_t cpp_50regressions_51(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/50regressions/51"
    const Value_t& x = vars[0];
#line 7 "tests/50regressions/51"
#line 7 "tests/50regressions/51"
    return fp_log(-x);
}
template<typename Value_t>
Value_t cpp_50regressions_57(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/50regressions/57"
    const Value_t& x = vars[0];
#line 7 "tests/50regressions/57"
#line 7 "tests/50regressions/57"
    return fp_cosh(fp_asinh(x));
}
template<typename Value_t>
Value_t cpp_50regressions_59(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/50regressions/59"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 7 "tests/50regressions/59"
#line 7 "tests/50regressions/59"
    return fp_cosh(x*x) + fp_tanh(y*y);
}
template<typename Value_t>
Value_t cpp_99misc_1(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/1"
    const Value_t& x = vars[0];
#line 5 "tests/99misc/1"
#line 5 "tests/99misc/1"
    return x*Value_t(4)/Value_t(2) + (Value_t(1)+(Value_t(2)+Value_t(3))) + x*x+x+(Value_t(1.0)+Value_t(2.0)+Value_t(3.0)*Value_t(4.0)+Value_t(5.0)*Value_t(6.0)*Value_t(7.0)-Value_t(8.0)*Value_t(9.0));
}
template<typename Value_t>
Value_t cpp_99misc_2(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/2"
    const Value_t& x = vars[0];
#line 5 "tests/99misc/2"
#line 5 "tests/99misc/2"
    return Value_t(2) * x + fp_sin(x)/Value_t(.5) + Value_t(2)-fp_sin(x)*fp_sin(x);
}
template<typename Value_t>
Value_t cpp_99misc_3(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/3"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
    const Value_t& z = vars[2];
#line 5 "tests/99misc/3"
#line 5 "tests/99misc/3"
    return fp_and(fp_equal(x,y), fp_equal(y,x))+ Value_t(1.0)+Value_t(2.0)-Value_t(3.1)*Value_t(4e2)/Value_t(.5) + x*x+y*y+z*z;
}
template<typename Value_t>
Value_t cpp_99misc_4(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/4"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/99misc/4"
#line 5 "tests/99misc/4"
    return ( ((( ( x-y) -( ((y) *Value_t(2)) -Value_t(3))) )* Value_t(4)))+fp_sin(x)*fp_cos(y)-fp_cos(x)*fp_sin(y);
}
template<typename Value_t>
Value_t cpp_99misc_5(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/5"
    const Value_t& __A5_x08 = vars[0];
    const Value_t& o__5_0AB_ = vars[1];
#line 5 "tests/99misc/5"
#line 5 "tests/99misc/5"
    return fp_pow(__A5_x08, o__5_0AB_);
}
#if !defined(FP_DISABLE_EVAL)
template<typename Value_t>
Value_t cpp_99misc_6(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/99misc/6"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 6 "tests/99misc/6"
    Value_t b0[2];
#line 6 "tests/99misc/6"
    return (x>Value_t(1e-14) && y>Value_t(1e-14) ? x*y + cpp_99misc_6<Value_t>((b0[0]=(x-Value_t(1)), b0[1]=(y-Value_t(1)), b0)) : Value_t(0))+Value_t(1);
}
#endif /* !defined(FP_DISABLE_EVAL) */
template<typename Value_t>
Value_t cpp_99misc_7(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/7"
    const Value_t& x = vars[0];
#line 5 "tests/99misc/7"
#line 5 "tests/99misc/7"
    return fp_cos(x)*fp_sin(Value_t(1)-x)*(Value_t(1)-fp_cos(x/Value_t(2))*fp_sin(x*Value_t(5)));
}
template<typename Value_t>
Value_t cpp_99misc_8(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/8"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/99misc/8"
#line 5 "tests/99misc/8"
    return fp_atan2(x,y) + (x>y ? x : y);
}
template<typename Value_t>
Value_t cpp_99misc_9(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/9"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
    const Value_t& z = vars[2];
#line 5 "tests/99misc/9"
#line 5 "tests/99misc/9"
    return Value_t(1.5)+x*y-Value_t(2.0)+Value_t(4.0)/Value_t(8.0)+z+z+z+z+x/(y*z);
}
template<typename Value_t>
Value_t cpp_99misc_10(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/10"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
    const Value_t& z = vars[2];
#line 5 "tests/99misc/10"
#line 5 "tests/99misc/10"
    return Value_t(1)+fp_sin(fp_cos( fp_max(Value_t(1)+Value_t(2)+Value_t(3)+Value_t(4)+Value_t(5), x+y+z)))+Value_t(2);
}
template<typename Value_t>
Value_t cpp_99misc_11(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/11"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
    const Value_t& z = vars[2];
#line 5 "tests/99misc/11"
#line 5 "tests/99misc/11"
    return -(-(-(-(-x))-x))+y*Value_t(1)+fp_log(fp_pow(Value_t(1.1), z));
}
template<typename Value_t>
Value_t cpp_99misc_12(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/12"
    const Value_t& x = vars[0];
#line 5 "tests/99misc/12"
#line 5 "tests/99misc/12"
    return Value_t(1.0)/fp_log(fp_pow(Value_t(10.0), Value_t(1.0)/fp_log(x)));
}
template<typename Value_t>
Value_t cpp_99misc_13(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/13"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/99misc/13"
#line 5 "tests/99misc/13"
    return fp_pow(x,Value_t(3)) * fp_pow(x,Value_t(4)) + fp_pow(y,Value_t(3)) * fp_pow(y,Value_t(5));
}
template<typename Value_t>
Value_t cpp_99misc_14(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/14"
    const Value_t& x = vars[0];
#line 5 "tests/99misc/14"
#line 5 "tests/99misc/14"
    return x*Value_t(M_PI) + fp_sin(Value_t(2)*Value_t(M_PI)) + Value_t(CONST);
}
template<typename Value_t>
Value_t cpp_99misc_15(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/15"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/99misc/15"
#line 5 "tests/99misc/15"
    return fp_pow(x,y)/fp_log(y) + fp_log(x)/fp_log(y) + fp_log(fp_pow(x,y));
}
template<typename Value_t>
Value_t cpp_99misc_16(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/16"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/99misc/16"
#line 5 "tests/99misc/16"
    return x<Value_t(0) ? (y<Value_t(0) ? x+y : x-y) : (y>Value_t(0) ? x*y : x+Value_t(2)*y);
}
template<typename Value_t>
Value_t cpp_99misc_17(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/17"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/99misc/17"
    Value_t b1[1];
    Value_t b2[2];
    Value_t b3[1];
    Value_t b4[2];
#line 5 "tests/99misc/17"
    return Sqr<Value_t>((b1[0]=(x), b1))+Sub<Value_t>((b2[0]=(x), b2[1]=(y), b2))+Sqr<Value_t>((b3[0]=(y), b3))+Sub<Value_t>((b4[0]=(y+Value_t(1)), b4[1]=(x-Value_t(2)), b4))-Value_t(1);
}
template<typename Value_t>
Value_t cpp_99misc_18(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/18"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/99misc/18"
#line 5 "tests/99misc/18"
    return - ( - ( - ( - Value_t(5) ) ) ) * -fp_pow(x, -fp_pow(y, -Value_t(2)));
}
template<typename Value_t>
Value_t cpp_99misc_19(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/19"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 17 "tests/99misc/19"
#line 17 "tests/99misc/19"
    return fp_less(x,y)+   Value_t(10)*fp_lessOrEq(x,y)+   Value_t(100)*fp_greater(x,y)+   Value_t(1000)*fp_greaterOrEq(x,y)+   Value_t(10000)*fp_equal(x,y)+   Value_t(100000)*fp_nequal(x,y)   +fp_and(x,y)*Value_t(2)   +fp_or(x,y)*Value_t(20)   +fp_not(x)*Value_t(200)   +fp_truth(x)*Value_t(2000)   +Value_t(4)*fp_not(fp_and(fp_less(x,y), fp_less(x,Value_t(3))))  +Value_t(40)*fp_or(fp_not(fp_greater(x,y)), fp_greater(x,Value_t(3)));
}
template<typename Value_t>
Value_t cpp_99misc_20(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/20"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/99misc/20"
#line 5 "tests/99misc/20"
    return fp_and(fp_not<Value_t>(x != y), fp_not(x)) + fp_truth(y);
}
template<typename Value_t>
Value_t cpp_99misc_21(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/21"
    const Value_t& x = vars[0];
#line 5 "tests/99misc/21"
    Value_t b5[1];
#line 5 "tests/99misc/21"
    return Sqr<Value_t>((b5[0]=(x), b5))+Value<Value_t>((0))-Value_t(5);
}
template<typename Value_t>
Value_t cpp_99misc_22(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/22"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 11 "tests/99misc/22"
#line 11 "tests/99misc/22"
    return (Value_t(3.5)*Value_t(2)) + Value_t(10)*(x*Value_t(3)) - (fp_sin(y)*Value_t(2)) + Value_t(100)*((x*Value_t(2))-(y*Value_t(3)))*Value_t(2) + Value_t(5.0)/(Value_t(2)*Value_t(2)) +  fp_pow(Value_t(1.1), x*Value_t(2)) + fp_pow(Value_t(1.1)*Value_t(2), x*Value_t(2));
}
template<typename Value_t>
Value_t cpp_99misc_23(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/23"
    const Value_t& x = vars[0];
#line 5 "tests/99misc/23"
#line 5 "tests/99misc/23"
    return (x/(Value_t(2)*fp_acos(Value_t(0.0))))*Value_t(180);
}
template<typename Value_t>
Value_t cpp_99misc_24(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/24"
    const Value_t& x = vars[0];
#line 5 "tests/99misc/24"
#line 5 "tests/99misc/24"
    return (fp_min(x, fp_min(Value_t(1),x)) + fp_min(x, Value_t(1)))/Value_t(2) + fp_min(x, Value_t(1))*Value_t(3) + fp_max(Value_t(0), fp_min(-Value_t(2),Value_t(0)));
}
template<typename Value_t>
Value_t cpp_99misc_25(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/25"
    const Value_t& a = vars[0];
    const Value_t& b = vars[1];
    const Value_t& c = vars[2];
#line 5 "tests/99misc/25"
#line 5 "tests/99misc/25"
    return fp_pow(a, fp_pow(b, c)) + fp_pow(a, -Value_t(2)) * (-fp_pow(b, Value_t(2))) + (-fp_pow(b, -c));
}
template<typename Value_t>
Value_t cpp_99misc_26(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 3 "tests/99misc/26"
    const Value_t& x = vars[0];
#line 6 "tests/99misc/26"
#line 6 "tests/99misc/26"
    return fp_sin(d2r(x)) + fp_cos(d2r(x*Value_t(1.5))) + r2d(fp_asin(x/Value_t(110.0))) + r2d(fp_acos(x/Value_t(120.0)));
}
template<typename Value_t>
Value_t cpp_99misc_27(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/27"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/99misc/27"
#line 5 "tests/99misc/27"
    return fp_abs(x)+fp_acos(x)+fp_asin(x)+fp_atan(x)+fp_atan2(x,y)+fp_ceil(x)+fp_cos(x)+fp_cosh(x)+Value_t(1.0)/fp_tan(x)+Value_t(1.0)/fp_sin(x) +fp_pow(x,y);
}
template<typename Value_t>
Value_t cpp_99misc_28(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/28"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 6 "tests/99misc/28"
#line 6 "tests/99misc/28"
    return fp_exp(x)+fp_floor(x)+fp_int(x)+fp_log(x)+fp_log10(x)+fp_max(x,y)+fp_min(x,y)+  Value_t(1)/fp_cos(x)+fp_sin(x)+fp_sinh(x)+fp_sqrt(x)+fp_tan(x)+fp_tanh(x)+fp_ceil(y)+fp_trunc(y);
}
template<typename Value_t>
Value_t cpp_99misc_29(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/29"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/99misc/29"
#line 5 "tests/99misc/29"
    return x-y*Value_t(1);
}
template<typename Value_t>
Value_t cpp_99misc_30(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/30"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 6 "tests/99misc/30"
#line 6 "tests/99misc/30"
    return x - y*Value_t(1) + fp_mod(x,y) + x / fp_pow(y,Value_t(1.1)) + fp_pow(Value_t(2),Value_t(3)) +  fp_mod(Value_t(5),Value_t(3)) + fp_pow(x,fp_pow(y,Value_t(0))) + fp_pow(x,Value_t(0.5));
}
template<typename Value_t>
Value_t cpp_99misc_31(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/31"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
    const Value_t& z = vars[2];
#line 15 "tests/99misc/31"
#line 15 "tests/99misc/31"
    return x - (y*(y*(y*-Value_t(1)))*Value_t(1)) + fp_log(x*fp_pow(fp_exp(Value_t(1.0)),y)) - fp_log(fp_pow(x,y)) +    fp_pow(fp_exp(Value_t(1.0)),fp_log(x+Value_t(6))) +    fp_pow(Value_t(10.0),fp_log(x+Value_t(6))/fp_log(y+Value_t(6))*fp_log(z+Value_t(6))/fp_log(Value_t(10.0))) -    fp_pow(fp_exp(Value_t(1.0)), fp_log(x+Value_t(6))*y) - fp_pow(Value_t(5.0),fp_log(x+Value_t(7))/fp_log(Value_t(5.0))) +  fp_pow(x*z+Value_t(17),Value_t(3)) * fp_pow(x*z+Value_t(17),Value_t(2)) / fp_pow(x*z+Value_t(17),Value_t(4));
}
template<typename Value_t>
Value_t cpp_99misc_32(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/32"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
    const Value_t& z = vars[2];
#line 43 "tests/99misc/32"
#line 43 "tests/99misc/32"
    return x     +y/y-fp_min(Value_t(3),Value_t(4))-x-fp_max(Value_t(4),Value_t(3))+fp_max(Value_t(3),Value_t(4))-fp_min(Value_t(4),Value_t(3))+Value_t(0)+(z*Value_t(1))     +(x-Value_t(2)+Value_t(2))+(x*Value_t(0.5)*Value_t(2))+y*Value_t(0)     +fp_min(fp_min(fp_min(Value_t(4.0),x),Value_t(1.0)),fp_min(x,fp_min(fp_min(y,Value_t(4.0)),z)))     +fp_max(fp_max(fp_max(Value_t(4.0),x),Value_t(1.0)),fp_max(x,fp_max(fp_max(y,Value_t(4.0)),z)))     +(fp_abs(Value_t(1))+fp_acos(Value_t(1.0))+fp_asin(Value_t(1.0))+fp_atan(Value_t(1.0))+fp_ceil(Value_t(1.1))+fp_cos(Value_t(0.0))      +fp_cosh(Value_t(0.0))+fp_floor(Value_t(1.1))+fp_log(Value_t(1.0))+fp_sin(Value_t(0.0))+fp_sinh(Value_t(0.0))+fp_tan(Value_t(1.0))      +fp_tanh(Value_t(1.0))+fp_atan2(Value_t(1.0),Value_t(1.0)))     +(x-(y-z))     +(x+y) + (x*y)     +fp_max(x,fp_max(x,fp_max(x,fp_max(x,x))))*-Value_t(1.0)     +(z-z)     +Value_t(1)/fp_sin(x/Value_t(5)) + Value_t(1)/fp_cos(y/Value_t(5)) + Value_t(1)/fp_tan(z/Value_t(5))     +fp_log10(Value_t(1)/fp_tan(z/Value_t(5)) + Value_t(1)/fp_sin(y/Value_t(5)) + Value_t(1)/fp_cos(x/Value_t(5)))     +fp_log(Value_t(30)+x)*fp_log(Value_t(40)+y)/fp_log(Value_t(50)+z)     +fp_sin(x/Value_t(57.295779513082320877))     +fp_asin(x/Value_t(10))*Value_t(57.295779513082320877)     +fp_floor(-x) + Value_t(1)/fp_ceil(x)     +fp_sqrt(Value_t(5) * Value_t(0.2)) +(-x+-x+-x+-x+-x+-x);
}
template<typename Value_t>
Value_t cpp_99misc_33(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/33"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 8 "tests/99misc/33"
#line 8 "tests/99misc/33"
    return fp_sin(fp_sqrt(Value_t(10)-x*x+y*y))+fp_cos(fp_sqrt(Value_t(15)-x*x-y*y))+fp_sin(x*x+y*y);
}
template<typename Value_t>
Value_t cpp_99misc_34(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/34"
    const Value_t& t = vars[0];
    const Value_t& rvar0 = vars[1];
    const Value_t& rvar1 = vars[2];
    const Value_t& rvar2 = vars[3];
#line 5 "tests/99misc/34"
#line 5 "tests/99misc/34"
    return rvar1+rvar2*rvar0-t;
}
template<typename Value_t>
Value_t cpp_99misc_35(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/35"
    const Value_t& A_very_long_variable_name_1 = vars[0];
    const Value_t& A_very_long_variable_name_2 = vars[1];
    const Value_t& Yet_a_third_very_long_variable_name = vars[2];
#line 7 "tests/99misc/35"
#line 7 "tests/99misc/35"
    return A_very_long_variable_name_1-A_very_long_variable_name_2+  Yet_a_third_very_long_variable_name*A_very_long_variable_name_1;
}
template<typename Value_t>
Value_t cpp_99misc_37(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/37"
    const Value_t& x = vars[0];
#line 5 "tests/99misc/37"
#line 5 "tests/99misc/37"
    return Value_t(5) + Value_t(7.5)*Value_t(8) / Value_t(3) - fp_pow(Value_t(2),Value_t(4))*Value_t(2) + fp_mod(Value_t(7),Value_t(2))+Value_t(4) + x;
}
template<typename Value_t>
Value_t cpp_99misc_38(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/38"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
    const Value_t& z = vars[2];
#line 5 "tests/99misc/38"
#line 5 "tests/99misc/38"
    return fp_asinh(x) + Value_t(1.5)*fp_acosh(y+Value_t(3)) + Value_t(2.2)*fp_atanh(z);
}
template<typename Value_t>
Value_t cpp_99misc_39(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/39"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
    const Value_t& z = vars[2];
#line 12 "tests/99misc/39"
#line 12 "tests/99misc/39"
    return fp_sin(x+fp_cos(y*Value_t(1.5)))-fp_cos(x+fp_sin(y*Value_t(1.5)))+z*z*z*fp_sin(z*z*z-x*x-y*y)-    fp_cos(y*Value_t(1.5))*fp_sin(x+fp_cos(y*Value_t(1.5)))+x*y*z+x*y*Value_t(2.5)+x*y*z*fp_cos(x)+x*y*fp_cos(x)+x*z*fp_cos(x)+  y*z*Value_t(2.5)+(x*y*z*fp_cos(x)-x*y*z-y*fp_cos(x)-x*z*y+x*y+x*z-fp_cos(x)*x);
}
template<typename Value_t>
Value_t cpp_99misc_40(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/40"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
    const Value_t& z = vars[2];
#line 16 "tests/99misc/40"
#line 16 "tests/99misc/40"
    return (x+x+x+x+x+x+x+x+x+x+x+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z)*    (x+x+x+x+x+x+x+x+x+x+x+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z)+    Value_t(2)*(x+x+x+x+x+x+x+x+x+x+x+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z)-    x*y*(x+x+x+x+x+x+x+x+x+x+x+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z)+  x*(x+x+x+x+x+x+x+x+x+x+x+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z);
}
template<typename Value_t>
Value_t cpp_99misc_41(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/41"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
    const Value_t& z = vars[2];
#line 16 "tests/99misc/41"
#line 16 "tests/99misc/41"
    return x*Value_t(3)+x*y+x*z+x*fp_sin(y*z) -    (fp_sin(x)+fp_cos(y))*Value_t(4) +    (fp_sin(x)+fp_cos(y))*x +    (fp_sin(x)+fp_cos(y))*y +  (fp_sin(x)+fp_cos(y))*z;
}
template<typename Value_t>
Value_t cpp_99misc_43(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/43"
    const Value_t& x = vars[0];
#line 8 "tests/99misc/43"
#line 8 "tests/99misc/43"
    return fp_log(x*x)+fp_abs(fp_exp(fp_abs(x)+Value_t(1)));
}
template<typename Value_t>
Value_t cpp_99misc_44(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/44"
    const Value_t& x = vars[0];
#line 21 "tests/99misc/44"
#line 21 "tests/99misc/44"
    return fp_pow(x*x, Value_t(1)/Value_t(8)) +    Value_t(1.1)*fp_pow(x*x*x, Value_t(1)/Value_t(7)) +    Value_t(1.2)*fp_pow(x*x*x*x, Value_t(1)/Value_t(6)) +    Value_t(1.3)*fp_pow(x*x*x*x*x, Value_t(1)/Value_t(5)) +    Value_t(1.4)*fp_pow(x*x*x*x*x*x, Value_t(1)/Value_t(6)) +    Value_t(1.5)*fp_pow(x*x*x*x*x*x*x, Value_t(1)/Value_t(4)) +    Value_t(1.6)*fp_pow(x*x*x*x*x*x*x*x, Value_t(1)/Value_t(3)) +    Value_t(1.7)*fp_pow(x*x*x*x*x*x*x*x*x, Value_t(1)/Value_t(2)) +  Value_t(1.8)*(fp_sqrt(fp_pow(fp_abs(-fp_sqrt(x)), Value_t(3))));
}
template<typename Value_t>
Value_t cpp_99misc_45(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/45"
    const Value_t& x = vars[0];
#line 5 "tests/99misc/45"
#line 5 "tests/99misc/45"
    return fp_pow(x*x, Value_t(1)/Value_t(7)) + Value_t(1.1)*fp_pow(x*x*x*x, Value_t(1)/Value_t(5)) + Value_t(1.2)*fp_pow(x*x*x*x*x*x, Value_t(1)/Value_t(3));
}
template<typename Value_t>
Value_t cpp_99misc_46(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/46"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 10 "tests/99misc/46"
#line 10 "tests/99misc/46"
    return fp_abs(fp_floor(fp_acos(x)+Value_t(4))) +    Value_t(1.1)*fp_abs(fp_floor(fp_acos(y)+Value_t(1.5))) +    fp_less(fp_acos(x), (fp_acos(y)-Value_t(10))) +    Value_t(1.2)*fp_max(-Value_t(4), fp_acos(x)) +  Value_t(1.3)*fp_min(Value_t(9), fp_acos(x)-Value_t(9));
}
template<typename Value_t>
Value_t cpp_99misc_47(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/47"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 7 "tests/99misc/47"
#line 7 "tests/99misc/47"
    return Value_t(1.1)*(fp_exp(x)+fp_exp(-x)) + Value_t(1.2)*(fp_exp(y)-fp_exp(-y)) +  Value_t(1.3)*((fp_exp(-x)+fp_exp(x))/Value_t(2)) + Value_t(1.4)*((fp_exp(-x)-fp_exp(x))/Value_t(2)) + Value_t(1.5)*(fp_cosh(y)+fp_sinh(y));
}
template<typename Value_t>
Value_t cpp_99misc_48(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/48"
    const Value_t& x = vars[0];
#line 6 "tests/99misc/48"
#line 6 "tests/99misc/48"
    return fp_sinh((fp_log(x)/Value_t(5)+Value_t(1))*Value_t(5)) + Value_t(1.2)*fp_cosh((fp_log(x)/fp_log(Value_t(2))+Value_t(1))*fp_log(Value_t(2))) +  fp_not(fp_or(fp_truth(x), fp_not(x/Value_t(4))));
}
template<typename Value_t>
Value_t cpp_99misc_49(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/49"
    const Value_t& x = vars[0];
#line 5 "tests/99misc/49"
#line 5 "tests/99misc/49"
    return fp_atan2(Value_t(0), x) + fp_pow(-Value_t(4)*(x-Value_t(100)), Value_t(3.3));
}
template<typename Value_t>
Value_t cpp_99misc_50(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/50"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 23 "tests/99misc/50"
#line 23 "tests/99misc/50"
    return fp_or(fp_less(x,y), fp_less(y,x)) +     Value_t(2)*fp_and(fp_less(x,y), fp_less(y,x)) +     Value_t(4)*fp_and(fp_lessOrEq(x,y), fp_lessOrEq(y,x)) +     Value_t(8)*fp_and(fp_less(x,y), fp_nequal(x,y)) +     Value_t(16)*fp_or(fp_less(x,y), fp_nequal(x,y)) +     Value_t(32)*fp_and(fp_lessOrEq(x,y), fp_greaterOrEq(x,y)) +     Value_t(64)*fp_or(fp_lessOrEq(x,y), fp_greaterOrEq(x,y)) +     Value_t(128)*fp_and(fp_nequal(x,y), fp_equal(x,y)) +     Value_t(256)*fp_and(fp_nequal(x,y), fp_nequal(x,y)) +  Value_t(512)*fp_and(fp_lessOrEq(x,y), fp_equal(x,y));
}
template<typename Value_t>
Value_t cpp_99misc_52(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/52"
    const Value_t& x = vars[0];
#line 9 "tests/99misc/52"
#line 9 "tests/99misc/52"
    return x + (Value_t(1.0)+Value_t(2.0)+Value_t(3.0)+Value_t(4.0)-Value_t(5.0)-Value_t(6.0)-Value_t(7.0)-Value_t(8.0))/Value_t(3.0) +    Value_t(4.0)*(Value_t(1.0)+fp_sin(Value_t(2.0))+fp_cos(Value_t(4.0)*Value_t(5.0)+Value_t(6.0))/Value_t(2.0)) + fp_cos(Value_t(0.5))*fp_tan(Value_t(0.6)+Value_t(0.2)) -  Value_t(1.1)/fp_log(Value_t(2.1))*fp_sqrt(Value_t(3.3)) + fp_pow(Value_t(2),Value_t(3));
}
template<typename Value_t>
Value_t cpp_99misc_53(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/53"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 8 "tests/99misc/53"
#line 8 "tests/99misc/53"
    return fp_and(x,y) + Value_t(4)*fp_or(fp_int(x/Value_t(10)), fp_int(y/Value_t(10))) +    Value_t(8)*((-fp_not(-fp_not(-x)))+       (fp_not(-fp_not(-fp_not(y)))))  + Value_t(16)*(-x + fp_not(y));
}
template<typename Value_t>
Value_t cpp_99misc_54(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/54"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 20 "tests/99misc/54"
#line 20 "tests/99misc/54"
    return fp_less(x,y)+   fp_lessOrEq(x,y)+   fp_greater(x,y)+   fp_greaterOrEq(x,y)+    fp_equal(x,y)+   fp_nequal(x,y)+   fp_and(x,y)+   fp_or(x,y)+    fp_not(x)+   fp_truth(x)+    fp_not(fp_and(fp_less(x,y),fp_less(x,Value_t(3))))+  fp_or( fp_not(fp_greater(x,y)), fp_greater(x,Value_t(3)));
}
template<typename Value_t>
Value_t cpp_99misc_55(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/55"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 9 "tests/99misc/55"
#line 9 "tests/99misc/55"
    return fp_less(fp_pow(x,Value_t(1.2)), Value_t(0)) +    fp_less(fp_pow(y,Value_t(2.5)), Value_t(0)) +    Value_t(2)*fp_less(x*x, Value_t(0)) +    Value_t(3)*fp_less(fp_pow(y,Value_t(3)), Value_t(0)) +  Value_t(4)*fp_less(fp_pow(x,Value_t(4)), Value_t(0));
}
template<typename Value_t>
Value_t cpp_99misc_56(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/56"
    const Value_t& x = vars[0];
#line 10 "tests/99misc/56"
#line 10 "tests/99misc/56"
    return fp_mod(Value_t(1.75e21), x);
}
template<typename Value_t>
Value_t cpp_99misc_58(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 4 "tests/99misc/58"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 15 "tests/99misc/58"
#line 15 "tests/99misc/58"
    return fp_less(-x,  Value_t(3)) +    fp_greater(x*-Value_t(1), Value_t(5)) +    fp_less(x*-Value_t(3),  Value_t(10)) +    fp_less(x*-Value_t(3),  y*Value_t(7)) +    fp_less(x*Value_t(4),  y*Value_t(7)) +    fp_less(x*Value_t(6),  y*-Value_t(3)) +    fp_less(-x,  Value_t(11)) +  fp_less(Value_t(5),  -y);
}
template<typename Value_t>
Value_t cpp_99misc_60(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/60"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
#line 5 "tests/99misc/60"
    Value_t b6[1];
    Value_t b7[2];
#line 5 "tests/99misc/60"
    return fp_or(fp_or(Sqr<Value_t>((b6[0]=(x), b6)), Sub<Value_t>((b7[0]=(x), b7[1]=(y), b7))), Value<Value_t>((0)));
}
template<typename Value_t>
Value_t cpp_99misc_i1(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/i1"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
    const Value_t& z = vars[2];
#line 5 "tests/99misc/i1"
#line 5 "tests/99misc/i1"
    return Value_t(1)+Value_t(2)+Value_t(3)-Value_t(4)*Value_t(5)*Value_t(6)/Value_t(3)+Value_t(10)/Value_t(2)-Value_t(9)%Value_t(2) + (x+y - Value_t(11)*x + z/Value_t(10) + x/(z+Value_t(31)));
}
template<typename Value_t>
Value_t cpp_99misc_i2(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/i2"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
    const Value_t& z = vars[2];
#line 7 "tests/99misc/i2"
#line 7 "tests/99misc/i2"
    return ((fp_abs(x*y) < Value_t(20))    || (((x+y) > Value_t(30))  && (z > Value_t(5)))) ? fp_min(x,Value_t(2)*y) : fp_max(y,z*Value_t(2));
}
template<typename Value_t>
Value_t cpp_99misc_i3(const Value_t* vars)
{
    using namespace FUNCTIONPARSERTYPES;
#line 2 "tests/99misc/i3"
    const Value_t& x = vars[0];
    const Value_t& y = vars[1];
    const Value_t& z = vars[2];
#line 9 "tests/99misc/i3"
#line 9 "tests/99misc/i3"
    return (x+y) + Value_t(2)*(x-z) + Value_t(3)*(x*y) + Value_t(4)*(y/z) + Value_t(5)*fp_mod(x,z) +    Value_t(6)*(x<y) + Value_t(7)*(x<=z) + Value_t(8)*(x>Value_t(2)*z) + Value_t(9)*(y>=Value_t(3)*z) + Value_t(10)*(x+y!=z) +  Value_t(11)*(Value_t(100)+x) + Value_t(12)*(Value_t(101)-y) + Value_t(13)*(Value_t(102)*z) + Value_t(14)*(Value_t(103)/x);
}
#line 2953 "testbed_tests.inc"

#ifdef FP_SUPPORT_GMP_INT_TYPE
template<>
struct RegressionTests<GmpInt>
{
    static const TestType<GmpInt> Tests[];
};
const TestType<GmpInt>
    RegressionTests<GmpInt>::Tests[] =
{
    { 3, -40, 40, 7, false, cpp_01unit_operators_addsub_i<GmpInt>,
      "x,y,z", "01unit_operators/addsub_i", "x+y+x+x-z+x" },
    { 2, -2, 2, 1, false, cpp_01unit_operators_and_i<GmpInt>,
      "x,y", "01unit_operators/and_i", "x&y" },
    { 2, -2, 2, 1, false, cpp_01unit_operators_cmpeq_i<GmpInt>,
      "x,y", "01unit_operators/cmpeq_i", "x=y" },
    { 2, -2, 2, 1, false, cpp_01unit_operators_cmpge_i<GmpInt>,
      "x,y", "01unit_operators/cmpge_i", "x>=y" },
    { 2, -2, 2, 1, false, cpp_01unit_operators_cmpgt_i<GmpInt>,
      "x,y", "01unit_operators/cmpgt_i", "x>y" },
    { 2, -2, 2, 1, false, cpp_01unit_operators_cmple_i<GmpInt>,
      "x,y", "01unit_operators/cmple_i", "x<=y" },
    { 2, -2, 2, 1, false, cpp_01unit_operators_cmplt_i<GmpInt>,
      "x,y", "01unit_operators/cmplt_i", "x<y" },
    { 2, -2, 2, 1, false, cpp_01unit_operators_cmpne_i<GmpInt>,
      "x,y", "01unit_operators/cmpne_i", "x!=y" },
    { 3, -460, 100, 8, false, cpp_01unit_operators_divmul_i<GmpInt>,
      "x,y,z", "01unit_operators/divmul_i", "x*y*x*x/(z+3)*(x+4)" },
    { 2, -460, 100, 8, false, cpp_01unit_operators_mod_i<GmpInt>,
      "x,z", "01unit_operators/mod_i", "x%(z+3)" },
    { 1, -2, 2, 1, false, cpp_01unit_operators_not_i<GmpInt>,
      "x", "01unit_operators/not_i", "!x" },
    { 1, -2, 2, 1, false, cpp_01unit_operators_notnot_i<GmpInt>,
      "x", "01unit_operators/notnot_i", "!!x" },
    { 2, -2, 2, 1, false, cpp_01unit_operators_or_i<GmpInt>,
      "x,y", "01unit_operators/or_i", "x|y" },
    { 1, -40000,40000,1, false, cpp_02unit_functions_abs_i<GmpInt>,
      "x", "02unit_functions/abs_i", "abs(x)" },
    { 3, -2,2,1, false, cpp_02unit_functions_if_i<GmpInt>,
      "x,y,z", "02unit_functions/if_i", "if(x,y,z)" },
    { 2, -4, 4, 1, false, cpp_02unit_functions_max<GmpInt>,
      "x,y", "02unit_functions/max", "max(x,y)" },
    { 2, -4, 4, 1, false, cpp_02unit_functions_min<GmpInt>,
      "x,y", "02unit_functions/min", "min(x,y)" },
    { 1, -10,10,1, false, cpp_50regressions_36<GmpInt>,
      "x", "50regressions/36", "-if(x<0, x, -x) + -if(x<5, 2, 3)" },
    { 2, -20, 20, 1, false, cpp_99misc_16<GmpInt>,
      "x,y", "99misc/16", "if(x<0, if(y<0, x+y, x-y), if(y>0, x*y, x+2*y))" },
    { 2, -20, 20, 1, false, cpp_99misc_17<GmpInt>,
      "x,y", "99misc/17", "sqr(x)+sub(x,y)+psqr(y)+psub(y+1,x-2)-1" },
    { 2, -100,100,1, false, cpp_99misc_19<GmpInt>,
      "x,y", "99misc/19", "(x<y)+10*(x<=y)+100*(x>y)+1000*(x>=y)+10000*(x=y)+100000*(x!=y)+  (x&y)*2+(x|y)*20+(!x)*200+(!!x)*2000+4*!((x<y)&(x<3))+40*!!(!(x>y)|(x>3))" },
    { 2, -100,100,1, false, cpp_99misc_20<GmpInt>,
      "x,y", "99misc/20", "(!(x != y) & !x) + !(!(!(!y)))" },
    { 1, -10,10,1, false, cpp_99misc_21<GmpInt>,
      "x", "99misc/21", "sqr(x)+value()-pvalue ( )" },
    { 1, -1000,1000,1, false, cpp_99misc_24<GmpInt>,
      "x", "99misc/24", "(min(x, min(1,x)) + min(x, 1))/2 + min(x, 1)*3 + max(0, min(-2,0))" },
    { 2, -5,5,1, false, cpp_99misc_29<GmpInt>,
      "x,y", "99misc/29", "x-y*1" },
    { 4, -5,5,1, false, cpp_99misc_34<GmpInt>,
      "t,う,お,早", "99misc/34", "お+早*う-t" },
    { 3, -10,10,1, false, cpp_99misc_35<GmpInt>,
      "A_very_long_variable_name_1,A_very_long_variable_name_2,Yet_a_third_very_long_variable_name", "99misc/35", "A_very_long_variable_name_1-A_very_long_variable_name_2+  Yet_a_third_very_long_variable_name*A_very_long_variable_name_1" },
    { 2, -10, 10, 1, false, cpp_99misc_50<GmpInt>,
      "x,y", "99misc/50", "(x<y | y<x) +     2*(x<y & y<x) +     4*(x<=y & y<=x) +     8*(x<y & x!=y) +     16*(x<y | x!=y) +     32*(x<=y & x>=y) +     64*(x<=y | x>=y) +     128*(x!=y & x=y) +     256*(x!=y & x!=y) +  512*(x<=y & x=y)" },
    { 2, -11, 11, 1, false, cpp_99misc_58<GmpInt>,
      "x,y", "99misc/58", "(-x < 3) + (x*-1 > 5) + (x*-3 < 10) + (x*-3 < y*7) +  (x*4 < y*7) + (x*6 < y*-3) + (-x < 11) + (5 < -y)" },
    { 2, -2, 2, 1, false, cpp_99misc_60<GmpInt>,
      "x,y", "99misc/60", "sqr(x) | sub(x,y) | value()" },
    { 3, -8,7,1, false, cpp_99misc_i1<GmpInt>,
      "x,y,z", "99misc/i1", "1+2+3-4*5*6/3+10/2-9%2 + (x+y - 11*x + z/10 + x/(z+31))" },
    { 3, -7,7,1, false, cpp_99misc_i2<GmpInt>,
      "x,y,z", "99misc/i2", "if(abs(x*y) < 20 | x+y > 30 & z > 5, min(x,2*y), max(y,z*2))" },
    { 3, 1,7,1, false, cpp_99misc_i3<GmpInt>,
      "x,y,z", "99misc/i3", "(x+y) + 2*(x-z) + 3*(x*y) + 4*(y/z) + 5*(x%z) +    6*(x<y) + 7*(x<=z) + 8*(x>2*z) + 9*(y>=3*z) + 10*(x+y!=z) +  11*(100+x) + 12*(101-y) + 13*(102*z) + 14*(103/x)" },
    TestType<GmpInt>() };
#endif /*FP_SUPPORT_GMP_INT_TYPE */

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
template<>
struct RegressionTests<MpfrFloat>
{
    static const TestType<MpfrFloat> Tests[];
};
const TestType<MpfrFloat>
    RegressionTests<MpfrFloat>::Tests[] =
{
    { 3, -2, 2, 0.1, false, cpp_01unit_operators_addsub_d<MpfrFloat>,
      "x,y,z", "01unit_operators/addsub_d", "x+y+x+x-z+x" },
    { 2, -1.75, 1.75, 0.25, false, cpp_01unit_operators_and_d<MpfrFloat>,
      "x,y", "01unit_operators/and_d", "x&y" },
    { 2, -1.75, 1.75, 0.25, false, cpp_01unit_operators_cmpeq_d<MpfrFloat>,
      "x,y", "01unit_operators/cmpeq_d", "x=y" },
    { 2, -1.75, 1.75, 0.25, false, cpp_01unit_operators_cmpge_d<MpfrFloat>,
      "x,y", "01unit_operators/cmpge_d", "x>=y" },
    { 2, -1.75, 1.75, 0.25, false, cpp_01unit_operators_cmpgt_d<MpfrFloat>,
      "x,y", "01unit_operators/cmpgt_d", "x>y" },
    { 2, -1.75, 1.75, 0.25, false, cpp_01unit_operators_cmple_d<MpfrFloat>,
      "x,y", "01unit_operators/cmple_d", "x<=y" },
    { 2, -1.75, 1.75, 0.25, false, cpp_01unit_operators_cmplt_d<MpfrFloat>,
      "x,y", "01unit_operators/cmplt_d", "x<y" },
    { 2, -1.75, 1.75, 0.25, false, cpp_01unit_operators_cmpne_d<MpfrFloat>,
      "x,y", "01unit_operators/cmpne_d", "x!=y" },
    { 3, -11, 11, 0.3, false, cpp_01unit_operators_divmul_d<MpfrFloat>,
      "x,y,z", "01unit_operators/divmul_d", "x*y*x*x/z*(x-0.1)" },
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 2, -11, 11, 0.3, false, cpp_01unit_operators_modm<MpfrFloat>,
      "x,z", "01unit_operators/modm", "x%z" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
    { 1, -1.25,1.25,0.25, false, cpp_01unit_operators_not_d<MpfrFloat>,
      "x", "01unit_operators/not_d", "!x" },
    { 1, -1.25,1.25,0.25, false, cpp_01unit_operators_notnot_d<MpfrFloat>,
      "x", "01unit_operators/notnot_d", "!!x" },
    { 2, -1.75, 1.75, 0.25, false, cpp_01unit_operators_or_d<MpfrFloat>,
      "x,y", "01unit_operators/or_d", "x|y" },
    { 1, -400, 400, 0.5, false, cpp_02unit_functions_abs_d<MpfrFloat>,
      "x", "02unit_functions/abs_d", "abs(x)" },
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -1, 1, 0.01, false, cpp_02unit_functions_acoshm<MpfrFloat>,
      "x", "02unit_functions/acoshm", "acosh(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -1, 1, 0.01, true, cpp_02unit_functions_acoshm_deg<MpfrFloat>,
      "x", "02unit_functions/acoshm_deg", "acosh(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -1, 1, 0.01, false, cpp_02unit_functions_acosm<MpfrFloat>,
      "x", "02unit_functions/acosm", "acos(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -1, 1, 0.01, true, cpp_02unit_functions_acosm_deg<MpfrFloat>,
      "x", "02unit_functions/acosm_deg", "acos(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -1, 1, 0.01, false, cpp_02unit_functions_asinhm<MpfrFloat>,
      "x", "02unit_functions/asinhm", "asinh(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -1, 1, 0.01, true, cpp_02unit_functions_asinhm_deg<MpfrFloat>,
      "x", "02unit_functions/asinhm_deg", "asinh(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -1, 1, 0.01, false, cpp_02unit_functions_asinm<MpfrFloat>,
      "x", "02unit_functions/asinm", "asin(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -1, 1, 0.01, true, cpp_02unit_functions_asinm_deg<MpfrFloat>,
      "x", "02unit_functions/asinm_deg", "asin(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 2, -4, 4, 0.05, false, cpp_02unit_functions_atan2m<MpfrFloat>,
      "x,y", "02unit_functions/atan2m", "atan2(x,y)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 2, -4, 4, 0.05, true, cpp_02unit_functions_atan2m_deg<MpfrFloat>,
      "x,y", "02unit_functions/atan2m_deg", "atan2(x,y)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -1, 1, 0.01, false, cpp_02unit_functions_atanhm<MpfrFloat>,
      "x", "02unit_functions/atanhm", "atanh(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -400, 400, 0.1, false, cpp_02unit_functions_atanm<MpfrFloat>,
      "x", "02unit_functions/atanm", "atan(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -400, 400, 0.1, true, cpp_02unit_functions_atanm_deg<MpfrFloat>,
      "x", "02unit_functions/atanm_deg", "atan(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -50000, 50000, 1000, false, cpp_02unit_functions_cbrtm<MpfrFloat>,
      "x", "02unit_functions/cbrtm", "cbrt(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -10, 10, 0.25, false, cpp_02unit_functions_ceilm<MpfrFloat>,
      "x", "02unit_functions/ceilm", "ceil(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -400, 400, 0.1, false, cpp_02unit_functions_coshm<MpfrFloat>,
      "x", "02unit_functions/coshm", "cosh(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -400, 400, 0.1, true, cpp_02unit_functions_coshm_deg<MpfrFloat>,
      "x", "02unit_functions/coshm_deg", "cosh(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -40000, 40000, 5, false, cpp_02unit_functions_cosm<MpfrFloat>,
      "x", "02unit_functions/cosm", "cos(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -40000, 40000, 5, true, cpp_02unit_functions_cosm_deg<MpfrFloat>,
      "x", "02unit_functions/cosm_deg", "cos(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -90, 90, 0.01, false, cpp_02unit_functions_exp2m<MpfrFloat>,
      "x", "02unit_functions/exp2m", "exp2(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -90, 90, 0.01, false, cpp_02unit_functions_expm<MpfrFloat>,
      "x", "02unit_functions/expm", "exp(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -10, 10, 0.25, false, cpp_02unit_functions_floorm<MpfrFloat>,
      "x", "02unit_functions/floorm", "floor(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 2, -4, 4, 0.05, false, cpp_02unit_functions_hypotm<MpfrFloat>,
      "x,y", "02unit_functions/hypotm", "hypot(x,y)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
    { 3, -1.25,1.25,0.25, false, cpp_02unit_functions_if_d<MpfrFloat>,
      "x,y,z", "02unit_functions/if_d", "if(x,y,z)" },
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -400, 400, 0.1, false, cpp_02unit_functions_log2m<MpfrFloat>,
      "x", "02unit_functions/log2m", "log2(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -400, 400, 0.1, false, cpp_02unit_functions_log10m<MpfrFloat>,
      "x", "02unit_functions/log10m", "log10(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -400, 400, 0.1, false, cpp_02unit_functions_logm<MpfrFloat>,
      "x", "02unit_functions/logm", "log(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
    { 2, -4, 4, 1, false, cpp_02unit_functions_max<MpfrFloat>,
      "x,y", "02unit_functions/max", "max(x,y)" },
    { 2, -4, 4, 1, false, cpp_02unit_functions_min<MpfrFloat>,
      "x,y", "02unit_functions/min", "min(x,y)" },
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 2, 1,20,1, false, cpp_02unit_functions_pow_negm<MpfrFloat>,
      "x,y", "02unit_functions/pow_negm", "pow(-x*0.25,y)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 2, 0.01, 4, 0.05, false, cpp_02unit_functions_pow_posm<MpfrFloat>,
      "x,y", "02unit_functions/pow_posm", "pow(x,y)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -400, 400, 0.1, false, cpp_02unit_functions_sinhm<MpfrFloat>,
      "x", "02unit_functions/sinhm", "sinh(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -400, 400, 0.1, true, cpp_02unit_functions_sinhm_deg<MpfrFloat>,
      "x", "02unit_functions/sinhm_deg", "sinh(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -40000, 40000, 5, false, cpp_02unit_functions_sinm<MpfrFloat>,
      "x", "02unit_functions/sinm", "sin(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -40000, 40000, 5, true, cpp_02unit_functions_sinm_deg<MpfrFloat>,
      "x", "02unit_functions/sinm_deg", "sin(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, 0, 100000, 1000, false, cpp_02unit_functions_sqrtm<MpfrFloat>,
      "x", "02unit_functions/sqrtm", "sqrt(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -40000, 40000, 5, false, cpp_02unit_functions_tanhm<MpfrFloat>,
      "x", "02unit_functions/tanhm", "tanh(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -40000, 40000, 5, true, cpp_02unit_functions_tanhm_deg<MpfrFloat>,
      "x", "02unit_functions/tanhm_deg", "tanh(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -1.3, 1.3, 0.05, false, cpp_02unit_functions_tanm<MpfrFloat>,
      "x", "02unit_functions/tanm", "tan(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -89, 89, 0.25, true, cpp_02unit_functions_tanm_deg<MpfrFloat>,
      "x", "02unit_functions/tanm_deg", "tan(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, -400, 400, 0.1, false, cpp_02unit_functions_truncm<MpfrFloat>,
      "x", "02unit_functions/truncm", "trunc(x)" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, 0,1,1, false, cpp_03unit_constants_e_mpfr<MpfrFloat>,
      "x", "03unit_constants/e_mpfr", "x*0+ naturalnumber" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
#if defined(FP_SUPPORT_MPFR_FLOAT_TYPE)
    { 1, 0,1,1, false, cpp_03unit_constants_pi_mpfr<MpfrFloat>,
      "x", "03unit_constants/pi_mpfr", "x*0+ pi" },
#endif /*defined(FP_SUPPORT_MPFR_FLOAT_TYPE) */
    { 1, -10,10,1, false, cpp_50regressions_36<MpfrFloat>,
      "x", "50regressions/36", "-if(x<0, x, -x) + -if(x<5, 2, 3)" },
    { 2, -3,3,.025, false, cpp_50regressions_42<MpfrFloat>,
      "x,y", "50regressions/42", "sqrt(x*x) + 1.5*((y*y)^.25) + hypot(x,y)" },
    { 1, -100, -1, .5, false, cpp_50regressions_51<MpfrFloat>,
      "x", "50regressions/51", "log(-x)" },
    { 1, .05, 1.0, .01, false, cpp_50regressions_57<MpfrFloat>,
      "x", "50regressions/57", "cosh(asinh(x))" },
    { 2, -2, 2, 1.2, false, cpp_50regressions_59<MpfrFloat>,
      "x,y", "50regressions/59", "cosh(x^2) + tanh(y^2)" },
    { 1, -1000,1000,.1, false, cpp_99misc_1<MpfrFloat>,
      "x", "99misc/1", "x*4/2 + (1+(2+3)) + x*x+x+1+2+3*4+5*6*\n7-8*9" },
    { 1, -1000,1000,.1, false, cpp_99misc_2<MpfrFloat>,
      "x", "99misc/2", "2 * x+ sin ( x ) / .5 + 2-sin(x)*sin(x)" },
    { 3, -10,10,.5, false, cpp_99misc_3<MpfrFloat>,
      "x,y,z", "99misc/3", "(x=y & y=x)+  1+2-3.1*4e2/.5 + x*x+y*y+z*z" },
    { 2, -10,10,.5, false, cpp_99misc_4<MpfrFloat>,
      "x,y", "99misc/4", "( ((( ( x-y) -( ((y) *2) -3)) )* 4))+sin(x)*cos(y)-cos(x)*sin(y)" },
    { 2, .1,10,.05, false, cpp_99misc_5<MpfrFloat>,
      "__A5_x08,o__5_0AB_", "99misc/5", "__A5_x08^o__5_0AB_" },
#if !defined(FP_DISABLE_EVAL)
    { 2, .1,10,2, false, cpp_99misc_6<MpfrFloat>,
      "x,y", "99misc/6", "if(x>0&y>0,x*y+eval(x-1,y-1),0)+1" },
#endif /*!defined(FP_DISABLE_EVAL) */
    { 1, -10,10,.001, false, cpp_99misc_7<MpfrFloat>,
      "x", "99misc/7", "cos(x)*sin(1-x)*(1-cos(x/2)*sin(x*5))" },
    { 2, -10,10,.05, false, cpp_99misc_8<MpfrFloat>,
      "x,y", "99misc/8", "atan2(x,y)+max(x,y)" },
    { 3, 1,21,.3, false, cpp_99misc_9<MpfrFloat>,
      "x,y,z", "99misc/9", "1.5+x*y-2+4/8+z+z+z+z+x/(y*z)" },
    { 3, 1,4,.3, false, cpp_99misc_10<MpfrFloat>,
      "x,y,z", "99misc/10", "1+sin(cos(max(1+2+3+4+5, x+y+z)))+2" },
    { 3, 1,21,.8, false, cpp_99misc_11<MpfrFloat>,
      "x,y,z", "99misc/11", "-(-(-(-(-x))-x))+y*1+log(1.1^z)" },
    { 1, 1,2000,.05, false, cpp_99misc_12<MpfrFloat>,
      "x", "99misc/12", "1/log(10^((3-2)/log(x)))" },
    { 2, -50,50,.5, false, cpp_99misc_13<MpfrFloat>,
      "x,y", "99misc/13", "x^3 * x^4 + y^3 * y^5" },
    { 1, -50,50,.01, false, cpp_99misc_14<MpfrFloat>,
      "x", "99misc/14", "x*pi + sin(2*pi) + CONST" },
    { 2, 1.1, 6, .052, false, cpp_99misc_15<MpfrFloat>,
      "x,y", "99misc/15", "x^y/log(y) + log(x)/log(y) + log(x^y)" },
    { 2, -20, 20, 1, false, cpp_99misc_16<MpfrFloat>,
      "x,y", "99misc/16", "if(x<0, if(y<0, x+y, x-y), if(y>0, x*y, x+2*y))" },
    { 2, -20, 20, 1, false, cpp_99misc_17<MpfrFloat>,
      "x,y", "99misc/17", "sqr(x)+sub(x,y)+psqr(y)+psub(y+1,x-2)-1" },
    { 2, 1,20,.1, false, cpp_99misc_18<MpfrFloat>,
      "x,y", "99misc/18", "- ( - ( - ( - 5 ) ) ) * -x^ -y^-2" },
    { 2, -100,100,1, false, cpp_99misc_19<MpfrFloat>,
      "x,y", "99misc/19", "(x<y)+10*(x<=y)+100*(x>y)+1000*(x>=y)+10000*(x=y)+100000*(x!=y)+  (x&y)*2+(x|y)*20+(!x)*200+(!!x)*2000+4*!((x<y)&(x<3))+40*!!(!(x>y)|(x>3))" },
    { 2, -100,100,1, false, cpp_99misc_20<MpfrFloat>,
      "x,y", "99misc/20", "(!(x != y) & !x) + !(!(!(!y)))" },
    { 1, -10,10,1, false, cpp_99misc_21<MpfrFloat>,
      "x", "99misc/21", "sqr(x)+value()-pvalue ( )" },
    { 2, -4,4,.1, false, cpp_99misc_22<MpfrFloat>,
      "x,y", "99misc/22", "3.5doubled + 10*x tripled - sin(y)doubled +    100*(x doubled-y tripled)doubled + 5/2doubled + 1.1^x doubled +  1.1doubled^x doubled" },
    { 1, -1000,1000,.1, false, cpp_99misc_23<MpfrFloat>,
      "x", "99misc/23", "(x/(2*acos(0)))*180" },
    { 1, -1000,1000,1, false, cpp_99misc_24<MpfrFloat>,
      "x", "99misc/24", "(min(x, min(1,x)) + min(x, 1))/2 + min(x, 1)*3 + max(0, min(-2,0))" },
    { 3, 1,3,.1, false, cpp_99misc_25<MpfrFloat>,
      "a,b,c", "99misc/25", "a^b^c + a^-2 * (-b^2) + (-b^-c)" },
    { 1, -100,100,.1, true, cpp_99misc_26<MpfrFloat>,
      "x", "99misc/26", "sin(x) + cos(x*1.5) + asin(x/110) + acos(x/120)" },
    { 2, .1, .9, .025, false, cpp_99misc_27<MpfrFloat>,
      "x,y", "99misc/27", "abs(x)+acos(x)+asin(x)+atan(x)+atan2(x,y)+ceil(x)+cos(x)+cosh(x)+cot(x)+csc(x) + pow(x,y)" },
    { 2, .1, .9, .025, false, cpp_99misc_28<MpfrFloat>,
      "x,y", "99misc/28", "exp(x)+floor(x)+int(x)+log(x)+log10(x)+max(x,y)+min(x,y)+sec(x)+sin(x)+sinh(x)+sqrt(x)+tan(x)+tanh(x)+ceil(y)+trunc(y)" },
    { 2, -5,5,1, false, cpp_99misc_29<MpfrFloat>,
      "x,y", "99misc/29", "x-y*1" },
    { 2, 3,10,1, false, cpp_99misc_30<MpfrFloat>,
      "x,y", "99misc/30", "x - y*1 + (x%y) + x / (y^1.1) + 2^3 + 5%3 + x^(y^0) + x^0.5" },
    { 3, .1,4,.3, false, cpp_99misc_31<MpfrFloat>,
      "x,y,z", "99misc/31", "x - (y*(y*(y*-1))*1) + log(x*exp(1.0)^y) - log(x^y) +    exp(1.0)^log(x+6) + 10^(log(x+6)/log(y+6)*log(z+6)/log(10)) -    exp(1.0)^(log(x+6)*y) - 5^(log(x+7)/log(5)) + (x*z+17)^3 * (x*z+17)^2 /  (x*z+17)^4" },
    { 3, 1,2,.05, false, cpp_99misc_32<MpfrFloat>,
      "x,y,z", "99misc/32", "x     +y/y-min(3,4)-x-max(4,3)+max(3,4)-min(4,3)+0+(z*1)     +(x-2+2)+(x*0.5*2)+y*0     +min(min(min(4.0,x),1.0),min(x,min(min(y,4.0),z)))     +max(max(max(4.0,x),1.0),max(x,max(max(y,4.0),z)))     +(abs(1)+acos(1.0)+asin(1.0)+atan(1.0)+ceil(1.1)+cos(0.0)      +cosh(0.0)+floor(1.1)+log(1.0)+sin(0.0)+sinh(0.0)+tan(1.0)      +tanh(1.0)+atan2(1.0,1.0))     +(x-(y-z))     +(x+y) + (x*y)     +max(x,max(x,max(x,max(x,x))))*-1.0     +(z-z)     +1/sin(x/5) + 1/cos(y/5) + 1/tan(z/5)     +log10(cot(z/5) + csc(y/5) + sec(x/5))     +log(30+x)*log(40+y)/log(50+z)     +sin(x/57.295779513082320877)     +asin(x/10)*57.295779513082320877     +floor(-x) + 1/ceil(x)     +sqrt(5 * 0.2) +(-x+-x+-x+-x+-x+-x)" },
    { 2, -2,2,.1, false, cpp_99misc_33<MpfrFloat>,
      "x,y", "99misc/33", "sin(sqrt(10-x*x+y*y))+cos(sqrt(15-x*x-y*y))+sin(x*x+y*y)" },
    { 4, -5,5,1, false, cpp_99misc_34<MpfrFloat>,
      "t,う,お,早", "99misc/34", "お+早*う-t" },
    { 3, -10,10,1, false, cpp_99misc_35<MpfrFloat>,
      "A_very_long_variable_name_1,A_very_long_variable_name_2,Yet_a_third_very_long_variable_name", "99misc/35", "A_very_long_variable_name_1-A_very_long_variable_name_2+  Yet_a_third_very_long_variable_name*A_very_long_variable_name_1" },
    { 1, -10,10,.1, false, cpp_99misc_37<MpfrFloat>,
      "x", "99misc/37", "5 + 7.5*8 / 3 - 2^4*2 + 7%2+4 + x" },
    { 3, -.9, .9, .05, false, cpp_99misc_38<MpfrFloat>,
      "x,y,z", "99misc/38", "asinh(x) + 1.5*acosh(y+3) + 2.2*atanh(z)" },
    { 3, -1.3, 1.3, .15, false, cpp_99misc_39<MpfrFloat>,
      "x,y,z", "99misc/39", "sin(x+cos(y*1.5))-cos(x+sin(y*1.5))+z*z*z*sin(z*z*z-x*x-y*y)-    cos(y*1.5)*sin(x+cos(y*1.5))+x*y*z+x*y*2.5+x*y*z*cos(x)+x*y*cos(x)+x*z*cos(x)+  y*z*2.5+(x*y*z*cos(x)-x*y*z-y*cos(x)-x*z*y+x*y+x*z-cos(x)*x)" },
    { 3, -1.3,1.3,.075, false, cpp_99misc_40<MpfrFloat>,
      "x,y,z", "99misc/40", "(x+x+x+x+x+x+x+x+x+x+x+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z)*    (x+x+x+x+x+x+x+x+x+x+x+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z)+    2*(x+x+x+x+x+x+x+x+x+x+x+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z)-    x*y*(x+x+x+x+x+x+x+x+x+x+x+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z)+  x*(x+x+x+x+x+x+x+x+x+x+x+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z)" },
    { 3, -2, 2, .15, false, cpp_99misc_41<MpfrFloat>,
      "x,y,z", "99misc/41", "x*3+x*y+x*z+x*sin(y*z) -    (sin(x)+cos(y))*4 +    (sin(x)+cos(y))*x +    (sin(x)+cos(y))*y +  (sin(x)+cos(y))*z" },
    { 1, -100,100,.03, false, cpp_99misc_43<MpfrFloat>,
      "x", "99misc/43", "log(x*x)+abs(exp(abs(x)+1))" },
    { 1, 0,100,.025, false, cpp_99misc_44<MpfrFloat>,
      "x", "99misc/44", "(x^2)^(1/8) +    1.1*(x^3)^(1/7) +    1.2*(x^4)^(1/6) +    1.3*(x^5)^(1/5) +    1.4*(x^6)^(1/6) +    1.5*(x^7)^(1/4) +    1.6*(x^8)^(1/3) +    1.7*(x^9)^(1/2) +  1.8*(sqrt(abs(-sqrt(x))^3))" },
    { 1, -10,10,.025, false, cpp_99misc_45<MpfrFloat>,
      "x", "99misc/45", "(x^2)^(1/7) + 1.1*(x^4)^(1/5) + 1.2*(x^6)^(1/3)" },
    { 2, -.9, .9, .015, false, cpp_99misc_46<MpfrFloat>,
      "x,y", "99misc/46", "abs(floor(acos(x)+4)) + 1.1*abs(floor(acos(y)+1.5)) +  (acos(x) < (acos(y)-10)) + 1.2*max(-4, acos(x)) + 1.3*min(9, acos(x)-9)" },
    { 2, -3, 3, .1, false, cpp_99misc_47<MpfrFloat>,
      "x,y", "99misc/47", "1.1*(exp(x)+exp(-x)) + 1.2*(exp(y)-exp(-y)) +  1.3*((exp(-x)+exp(x))/2) + 1.4*((exp(-x)-exp(x))/2) + 1.5*(cosh(y)+sinh(y))" },
    { 1, 2, 1e9, 1.2e7, false, cpp_99misc_48<MpfrFloat>,
      "x", "99misc/48", "sinh((log(x)/5+1)*5) + 1.2*cosh((log(x)/log(2)+1)*log(2)) + !(x | !(x/4))" },
    { 1, -100, 100, .03, false, cpp_99misc_49<MpfrFloat>,
      "x", "99misc/49", "atan2(0, x) + (-4*(x-100))^3.3" },
    { 2, -10, 10, 1, false, cpp_99misc_50<MpfrFloat>,
      "x,y", "99misc/50", "(x<y | y<x) +     2*(x<y & y<x) +     4*(x<=y & y<=x) +     8*(x<y & x!=y) +     16*(x<y | x!=y) +     32*(x<=y & x>=y) +     64*(x<=y | x>=y) +     128*(x!=y & x=y) +     256*(x!=y & x!=y) +  512*(x<=y & x=y)" },
    { 1, -10, 10, .5, false, cpp_99misc_52<MpfrFloat>,
      "x", "99misc/52", "x + (1.0+2.0+3.0+4.0-5.0-6.0-7.0-8.0)/3.0 +    4.0*(1.0+sin(2.0)+cos(4.0*5.0+6.0)/2.0) + cos(0.5)*tan(0.6+0.2) -  1.1/log(2.1)*sqrt(3.3) + 2^3" },
    { 2, 0, 10, 0.5, false, cpp_99misc_53<MpfrFloat>,
      "x,y", "99misc/53", "(x&y) + 4*(int(x/10)|int(y/10)) + 8*((-!-!-x)+(!-!-!y)) + 16*(-------x + !!!!!!!y)" },
    { 2, -10, 100, .5, false, cpp_99misc_54<MpfrFloat>,
      "x,y", "99misc/54", "(x<y)+(x<=y)+(x>y)+(x>=y)+    (x=y)+(x!=y)+(x&y)+(x|y)+    (!x)+(!!x)+    !((x<y)&(x<3))+  !!(!(x>y)|(x>3))" },
    { 2, 1,100, .5, false, cpp_99misc_55<MpfrFloat>,
      "x,y", "99misc/55", "(x^1.2 < 0) + (y^2.5 < 0) + 2*(x*x<0) + 3*(y^3<0) + 4*(x^4<0)" },
    { 1, .25, 100, .25, false, cpp_99misc_56<MpfrFloat>,
      "x", "99misc/56", "1.75e21%x" },
    { 2, -11, 11, 1, false, cpp_99misc_58<MpfrFloat>,
      "x,y", "99misc/58", "(-x < 3) + (x*-1 > 5) + (x*-3 < 10) + (x*-3 < y*7) +  (x*4 < y*7) + (x*6 < y*-3) + (-x < 11) + (5 < -y)" },
    { 2, -2, 2, 1, false, cpp_99misc_60<MpfrFloat>,
      "x,y", "99misc/60", "sqr(x) | sub(x,y) | value()" },
    TestType<MpfrFloat>() };
#endif /*FP_SUPPORT_MPFR_FLOAT_TYPE */

template<>
struct RegressionTests<double>
{
    static const TestType<double> Tests[];
};
const TestType<double>
    RegressionTests<double>::Tests[] =
{
    { 3, -2, 2, 0.1, false, cpp_01unit_operators_addsub_d<double>,
      "x,y,z", "01unit_operators/addsub_d", "x+y+x+x-z+x" },
    { 2, -1.75, 1.75, 0.25, false, cpp_01unit_operators_and_d<double>,
      "x,y", "01unit_operators/and_d", "x&y" },
    { 2, -1.75, 1.75, 0.25, false, cpp_01unit_operators_cmpeq_d<double>,
      "x,y", "01unit_operators/cmpeq_d", "x=y" },
    { 2, -1.75, 1.75, 0.25, false, cpp_01unit_operators_cmpge_d<double>,
      "x,y", "01unit_operators/cmpge_d", "x>=y" },
    { 2, -1.75, 1.75, 0.25, false, cpp_01unit_operators_cmpgt_d<double>,
      "x,y", "01unit_operators/cmpgt_d", "x>y" },
    { 2, -1.75, 1.75, 0.25, false, cpp_01unit_operators_cmple_d<double>,
      "x,y", "01unit_operators/cmple_d", "x<=y" },
    { 2, -1.75, 1.75, 0.25, false, cpp_01unit_operators_cmplt_d<double>,
      "x,y", "01unit_operators/cmplt_d", "x<y" },
    { 2, -1.75, 1.75, 0.25, false, cpp_01unit_operators_cmpne_d<double>,
      "x,y", "01unit_operators/cmpne_d", "x!=y" },
    { 3, -11, 11, 0.3, false, cpp_01unit_operators_divmul_d<double>,
      "x,y,z", "01unit_operators/divmul_d", "x*y*x*x/z*(x-0.1)" },
    { 2, -11, 11, 0.3, false, cpp_01unit_operators_mod<double>,
      "x,z", "01unit_operators/mod", "x%z" },
    { 1, -1.25,1.25,0.25, false, cpp_01unit_operators_not_d<double>,
      "x", "01unit_operators/not_d", "!x" },
    { 1, -1.25,1.25,0.25, false, cpp_01unit_operators_notnot_d<double>,
      "x", "01unit_operators/notnot_d", "!!x" },
    { 2, -1.75, 1.75, 0.25, false, cpp_01unit_operators_or_d<double>,
      "x,y", "01unit_operators/or_d", "x|y" },
    { 1, -400, 400, 0.5, false, cpp_02unit_functions_abs_d<double>,
      "x", "02unit_functions/abs_d", "abs(x)" },
    { 1, -1, 1, 0.01, false, cpp_02unit_functions_acos<double>,
      "x", "02unit_functions/acos", "acos(x)" },
    { 1, -1, 1, 0.01, true, cpp_02unit_functions_acos_deg<double>,
      "x", "02unit_functions/acos_deg", "acos(x)" },
    { 1, -1, 1, 0.01, false, cpp_02unit_functions_acosh<double>,
      "x", "02unit_functions/acosh", "acosh(x)" },
    { 1, -1, 1, 0.01, true, cpp_02unit_functions_acosh_deg<double>,
      "x", "02unit_functions/acosh_deg", "acosh(x)" },
    { 1, -1, 1, 0.01, false, cpp_02unit_functions_asin<double>,
      "x", "02unit_functions/asin", "asin(x)" },
    { 1, -1, 1, 0.01, true, cpp_02unit_functions_asin_deg<double>,
      "x", "02unit_functions/asin_deg", "asin(x)" },
    { 1, -1, 1, 0.01, false, cpp_02unit_functions_asinh<double>,
      "x", "02unit_functions/asinh", "asinh(x)" },
    { 1, -1, 1, 0.01, true, cpp_02unit_functions_asinh_deg<double>,
      "x", "02unit_functions/asinh_deg", "asinh(x)" },
    { 1, -400, 400, 0.1, false, cpp_02unit_functions_atan<double>,
      "x", "02unit_functions/atan", "atan(x)" },
    { 2, -4, 4, 0.05, false, cpp_02unit_functions_atan2<double>,
      "x,y", "02unit_functions/atan2", "atan2(x,y)" },
    { 2, -4, 4, 0.05, true, cpp_02unit_functions_atan2_deg<double>,
      "x,y", "02unit_functions/atan2_deg", "atan2(x,y)" },
    { 1, -400, 400, 0.1, true, cpp_02unit_functions_atan_deg<double>,
      "x", "02unit_functions/atan_deg", "atan(x)" },
    { 1, -1, 1, 0.01, false, cpp_02unit_functions_atanh<double>,
      "x", "02unit_functions/atanh", "atanh(x)" },
    { 1, -50000, 50000, 1000, false, cpp_02unit_functions_cbrt<double>,
      "x", "02unit_functions/cbrt", "cbrt(x)" },
    { 1, -10, 10, 0.25, false, cpp_02unit_functions_ceil<double>,
      "x", "02unit_functions/ceil", "ceil(x)" },
    { 1, -40000, 40000, 5, false, cpp_02unit_functions_cos<double>,
      "x", "02unit_functions/cos", "cos(x)" },
    { 1, -40000, 40000, 5, true, cpp_02unit_functions_cos_deg<double>,
      "x", "02unit_functions/cos_deg", "cos(x)" },
    { 1, -4000, 4000, 0.1, false, cpp_02unit_functions_cosh<double>,
      "x", "02unit_functions/cosh", "cosh(x)" },
    { 1, -4000, 4000, 0.1, true, cpp_02unit_functions_cosh_deg<double>,
      "x", "02unit_functions/cosh_deg", "cosh(x)" },
    { 1, -90, 90, 0.01, false, cpp_02unit_functions_exp<double>,
      "x", "02unit_functions/exp", "exp(x)" },
    { 1, -90, 90, 0.01, false, cpp_02unit_functions_exp2<double>,
      "x", "02unit_functions/exp2", "exp2(x)" },
    { 1, -10, 10, 0.25, false, cpp_02unit_functions_floor<double>,
      "x", "02unit_functions/floor", "floor(x)" },
    { 2, -4, 4, 0.05, false, cpp_02unit_functions_hypot<double>,
      "x,y", "02unit_functions/hypot", "hypot(x,y)" },
    { 3, -1.25,1.25,0.25, false, cpp_02unit_functions_if_d<double>,
      "x,y,z", "02unit_functions/if_d", "if(x,y,z)" },
    { 1, -4000, 4000, 0.1, false, cpp_02unit_functions_log<double>,
      "x", "02unit_functions/log", "log(x)" },
    { 1, -4000, 4000, 0.1, false, cpp_02unit_functions_log2<double>,
      "x", "02unit_functions/log2", "log2(x)" },
    { 1, -4000, 4000, 0.1, false, cpp_02unit_functions_log10<double>,
      "x", "02unit_functions/log10", "log10(x)" },
    { 2, -4, 4, 1, false, cpp_02unit_functions_max<double>,
      "x,y", "02unit_functions/max", "max(x,y)" },
    { 2, -4, 4, 1, false, cpp_02unit_functions_min<double>,
      "x,y", "02unit_functions/min", "min(x,y)" },
    { 2, 1,20,1, false, cpp_02unit_functions_pow_neg<double>,
      "x,y", "02unit_functions/pow_neg", "pow(-x*0.25,y)" },
    { 2, 0.01, 4, 0.05, false, cpp_02unit_functions_pow_pos<double>,
      "x,y", "02unit_functions/pow_pos", "pow(x,y)" },
    { 1, -40000, 40000, 5, false, cpp_02unit_functions_sin<double>,
      "x", "02unit_functions/sin", "sin(x)" },
    { 1, -40000, 40000, 5, true, cpp_02unit_functions_sin_deg<double>,
      "x", "02unit_functions/sin_deg", "sin(x)" },
    { 1, -400, 400, 0.1, false, cpp_02unit_functions_sinh<double>,
      "x", "02unit_functions/sinh", "sinh(x)" },
    { 1, -400, 400, 0.1, true, cpp_02unit_functions_sinh_deg<double>,
      "x", "02unit_functions/sinh_deg", "sinh(x)" },
    { 1, 0, 100000, 1000, false, cpp_02unit_functions_sqrt<double>,
      "x", "02unit_functions/sqrt", "sqrt(x)" },
    { 1, -1.3, 1.3, 0.05, false, cpp_02unit_functions_tan<double>,
      "x", "02unit_functions/tan", "tan(x)" },
    { 1, -89, 89, 0.25, true, cpp_02unit_functions_tan_deg<double>,
      "x", "02unit_functions/tan_deg", "tan(x)" },
    { 1, -40000, 40000, 5, false, cpp_02unit_functions_tanh<double>,
      "x", "02unit_functions/tanh", "tanh(x)" },
    { 1, -40000, 40000, 5, true, cpp_02unit_functions_tanh_deg<double>,
      "x", "02unit_functions/tanh_deg", "tanh(x)" },
    { 1, -4000, 4000, 0.1, false, cpp_02unit_functions_trunc<double>,
      "x", "02unit_functions/trunc", "trunc(x)" },
    { 1, 0,1,1, false, cpp_03unit_constants_e_d<double>,
      "x", "03unit_constants/e_d", "x*0+ naturalnumber" },
    { 1, 0,1,1, false, cpp_03unit_constants_pi_d<double>,
      "x", "03unit_constants/pi_d", "x*0+ pi" },
    { 1, -10,10,1, false, cpp_50regressions_36<double>,
      "x", "50regressions/36", "-if(x<0, x, -x) + -if(x<5, 2, 3)" },
    { 2, -3,3,.025, false, cpp_50regressions_42<double>,
      "x,y", "50regressions/42", "sqrt(x*x) + 1.5*((y*y)^.25) + hypot(x,y)" },
    { 1, -100, -1, .5, false, cpp_50regressions_51<double>,
      "x", "50regressions/51", "log(-x)" },
    { 1, .05, 1.0, .01, false, cpp_50regressions_57<double>,
      "x", "50regressions/57", "cosh(asinh(x))" },
    { 2, -2, 2, 1.2, false, cpp_50regressions_59<double>,
      "x,y", "50regressions/59", "cosh(x^2) + tanh(y^2)" },
    { 1, -1000,1000,.1, false, cpp_99misc_1<double>,
      "x", "99misc/1", "x*4/2 + (1+(2+3)) + x*x+x+1+2+3*4+5*6*\n7-8*9" },
    { 1, -1000,1000,.1, false, cpp_99misc_2<double>,
      "x", "99misc/2", "2 * x+ sin ( x ) / .5 + 2-sin(x)*sin(x)" },
    { 3, -10,10,.5, false, cpp_99misc_3<double>,
      "x,y,z", "99misc/3", "(x=y & y=x)+  1+2-3.1*4e2/.5 + x*x+y*y+z*z" },
    { 2, -10,10,.5, false, cpp_99misc_4<double>,
      "x,y", "99misc/4", "( ((( ( x-y) -( ((y) *2) -3)) )* 4))+sin(x)*cos(y)-cos(x)*sin(y)" },
    { 2, .1,10,.05, false, cpp_99misc_5<double>,
      "__A5_x08,o__5_0AB_", "99misc/5", "__A5_x08^o__5_0AB_" },
#if !defined(FP_DISABLE_EVAL)
    { 2, .1,10,2, false, cpp_99misc_6<double>,
      "x,y", "99misc/6", "if(x>0&y>0,x*y+eval(x-1,y-1),0)+1" },
#endif /*!defined(FP_DISABLE_EVAL) */
    { 1, -10,10,.001, false, cpp_99misc_7<double>,
      "x", "99misc/7", "cos(x)*sin(1-x)*(1-cos(x/2)*sin(x*5))" },
    { 2, -10,10,.05, false, cpp_99misc_8<double>,
      "x,y", "99misc/8", "atan2(x,y)+max(x,y)" },
    { 3, 1,21,.3, false, cpp_99misc_9<double>,
      "x,y,z", "99misc/9", "1.5+x*y-2+4/8+z+z+z+z+x/(y*z)" },
    { 3, 1,4,.3, false, cpp_99misc_10<double>,
      "x,y,z", "99misc/10", "1+sin(cos(max(1+2+3+4+5, x+y+z)))+2" },
    { 3, 1,21,.8, false, cpp_99misc_11<double>,
      "x,y,z", "99misc/11", "-(-(-(-(-x))-x))+y*1+log(1.1^z)" },
    { 1, 1,2000,.05, false, cpp_99misc_12<double>,
      "x", "99misc/12", "1/log(10^((3-2)/log(x)))" },
    { 2, -50,50,.5, false, cpp_99misc_13<double>,
      "x,y", "99misc/13", "x^3 * x^4 + y^3 * y^5" },
    { 1, -50,50,.01, false, cpp_99misc_14<double>,
      "x", "99misc/14", "x*pi + sin(2*pi) + CONST" },
    { 2, 1.1, 6, .052, false, cpp_99misc_15<double>,
      "x,y", "99misc/15", "x^y/log(y) + log(x)/log(y) + log(x^y)" },
    { 2, -20, 20, 1, false, cpp_99misc_16<double>,
      "x,y", "99misc/16", "if(x<0, if(y<0, x+y, x-y), if(y>0, x*y, x+2*y))" },
    { 2, -20, 20, 1, false, cpp_99misc_17<double>,
      "x,y", "99misc/17", "sqr(x)+sub(x,y)+psqr(y)+psub(y+1,x-2)-1" },
    { 2, 1,20,.1, false, cpp_99misc_18<double>,
      "x,y", "99misc/18", "- ( - ( - ( - 5 ) ) ) * -x^ -y^-2" },
    { 2, -100,100,1, false, cpp_99misc_19<double>,
      "x,y", "99misc/19", "(x<y)+10*(x<=y)+100*(x>y)+1000*(x>=y)+10000*(x=y)+100000*(x!=y)+  (x&y)*2+(x|y)*20+(!x)*200+(!!x)*2000+4*!((x<y)&(x<3))+40*!!(!(x>y)|(x>3))" },
    { 2, -100,100,1, false, cpp_99misc_20<double>,
      "x,y", "99misc/20", "(!(x != y) & !x) + !(!(!(!y)))" },
    { 1, -10,10,1, false, cpp_99misc_21<double>,
      "x", "99misc/21", "sqr(x)+value()-pvalue ( )" },
    { 2, -4,4,.1, false, cpp_99misc_22<double>,
      "x,y", "99misc/22", "3.5doubled + 10*x tripled - sin(y)doubled +    100*(x doubled-y tripled)doubled + 5/2doubled + 1.1^x doubled +  1.1doubled^x doubled" },
    { 1, -1000,1000,.1, false, cpp_99misc_23<double>,
      "x", "99misc/23", "(x/(2*acos(0)))*180" },
    { 1, -1000,1000,1, false, cpp_99misc_24<double>,
      "x", "99misc/24", "(min(x, min(1,x)) + min(x, 1))/2 + min(x, 1)*3 + max(0, min(-2,0))" },
    { 3, 1,3,.1, false, cpp_99misc_25<double>,
      "a,b,c", "99misc/25", "a^b^c + a^-2 * (-b^2) + (-b^-c)" },
    { 1, -100,100,.1, true, cpp_99misc_26<double>,
      "x", "99misc/26", "sin(x) + cos(x*1.5) + asin(x/110) + acos(x/120)" },
    { 2, .1, .9, .025, false, cpp_99misc_27<double>,
      "x,y", "99misc/27", "abs(x)+acos(x)+asin(x)+atan(x)+atan2(x,y)+ceil(x)+cos(x)+cosh(x)+cot(x)+csc(x) + pow(x,y)" },
    { 2, .1, .9, .025, false, cpp_99misc_28<double>,
      "x,y", "99misc/28", "exp(x)+floor(x)+int(x)+log(x)+log10(x)+max(x,y)+min(x,y)+sec(x)+sin(x)+sinh(x)+sqrt(x)+tan(x)+tanh(x)+ceil(y)+trunc(y)" },
    { 2, -5,5,1, false, cpp_99misc_29<double>,
      "x,y", "99misc/29", "x-y*1" },
    { 2, 3,10,1, false, cpp_99misc_30<double>,
      "x,y", "99misc/30", "x - y*1 + (x%y) + x / (y^1.1) + 2^3 + 5%3 + x^(y^0) + x^0.5" },
    { 3, .1,4,.3, false, cpp_99misc_31<double>,
      "x,y,z", "99misc/31", "x - (y*(y*(y*-1))*1) + log(x*exp(1.0)^y) - log(x^y) +    exp(1.0)^log(x+6) + 10^(log(x+6)/log(y+6)*log(z+6)/log(10)) -    exp(1.0)^(log(x+6)*y) - 5^(log(x+7)/log(5)) + (x*z+17)^3 * (x*z+17)^2 /  (x*z+17)^4" },
    { 3, 1,2,.05, false, cpp_99misc_32<double>,
      "x,y,z", "99misc/32", "x     +y/y-min(3,4)-x-max(4,3)+max(3,4)-min(4,3)+0+(z*1)     +(x-2+2)+(x*0.5*2)+y*0     +min(min(min(4.0,x),1.0),min(x,min(min(y,4.0),z)))     +max(max(max(4.0,x),1.0),max(x,max(max(y,4.0),z)))     +(abs(1)+acos(1.0)+asin(1.0)+atan(1.0)+ceil(1.1)+cos(0.0)      +cosh(0.0)+floor(1.1)+log(1.0)+sin(0.0)+sinh(0.0)+tan(1.0)      +tanh(1.0)+atan2(1.0,1.0))     +(x-(y-z))     +(x+y) + (x*y)     +max(x,max(x,max(x,max(x,x))))*-1.0     +(z-z)     +1/sin(x/5) + 1/cos(y/5) + 1/tan(z/5)     +log10(cot(z/5) + csc(y/5) + sec(x/5))     +log(30+x)*log(40+y)/log(50+z)     +sin(x/57.295779513082320877)     +asin(x/10)*57.295779513082320877     +floor(-x) + 1/ceil(x)     +sqrt(5 * 0.2) +(-x+-x+-x+-x+-x+-x)" },
    { 2, -2,2,.1, false, cpp_99misc_33<double>,
      "x,y", "99misc/33", "sin(sqrt(10-x*x+y*y))+cos(sqrt(15-x*x-y*y))+sin(x*x+y*y)" },
    { 4, -5,5,1, false, cpp_99misc_34<double>,
      "t,う,お,早", "99misc/34", "お+早*う-t" },
    { 3, -10,10,1, false, cpp_99misc_35<double>,
      "A_very_long_variable_name_1,A_very_long_variable_name_2,Yet_a_third_very_long_variable_name", "99misc/35", "A_very_long_variable_name_1-A_very_long_variable_name_2+  Yet_a_third_very_long_variable_name*A_very_long_variable_name_1" },
    { 1, -10,10,.1, false, cpp_99misc_37<double>,
      "x", "99misc/37", "5 + 7.5*8 / 3 - 2^4*2 + 7%2+4 + x" },
    { 3, -.9, .9, .05, false, cpp_99misc_38<double>,
      "x,y,z", "99misc/38", "asinh(x) + 1.5*acosh(y+3) + 2.2*atanh(z)" },
    { 3, -1.3, 1.3, .15, false, cpp_99misc_39<double>,
      "x,y,z", "99misc/39", "sin(x+cos(y*1.5))-cos(x+sin(y*1.5))+z*z*z*sin(z*z*z-x*x-y*y)-    cos(y*1.5)*sin(x+cos(y*1.5))+x*y*z+x*y*2.5+x*y*z*cos(x)+x*y*cos(x)+x*z*cos(x)+  y*z*2.5+(x*y*z*cos(x)-x*y*z-y*cos(x)-x*z*y+x*y+x*z-cos(x)*x)" },
    { 3, -1.3,1.3,.075, false, cpp_99misc_40<double>,
      "x,y,z", "99misc/40", "(x+x+x+x+x+x+x+x+x+x+x+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z)*    (x+x+x+x+x+x+x+x+x+x+x+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z)+    2*(x+x+x+x+x+x+x+x+x+x+x+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z)-    x*y*(x+x+x+x+x+x+x+x+x+x+x+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z)+  x*(x+x+x+x+x+x+x+x+x+x+x+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z)" },
    { 3, -2, 2, .15, false, cpp_99misc_41<double>,
      "x,y,z", "99misc/41", "x*3+x*y+x*z+x*sin(y*z) -    (sin(x)+cos(y))*4 +    (sin(x)+cos(y))*x +    (sin(x)+cos(y))*y +  (sin(x)+cos(y))*z" },
    { 1, -100,100,.03, false, cpp_99misc_43<double>,
      "x", "99misc/43", "log(x*x)+abs(exp(abs(x)+1))" },
    { 1, 0,100,.025, false, cpp_99misc_44<double>,
      "x", "99misc/44", "(x^2)^(1/8) +    1.1*(x^3)^(1/7) +    1.2*(x^4)^(1/6) +    1.3*(x^5)^(1/5) +    1.4*(x^6)^(1/6) +    1.5*(x^7)^(1/4) +    1.6*(x^8)^(1/3) +    1.7*(x^9)^(1/2) +  1.8*(sqrt(abs(-sqrt(x))^3))" },
    { 1, -10,10,.025, false, cpp_99misc_45<double>,
      "x", "99misc/45", "(x^2)^(1/7) + 1.1*(x^4)^(1/5) + 1.2*(x^6)^(1/3)" },
    { 2, -.9, .9, .015, false, cpp_99misc_46<double>,
      "x,y", "99misc/46", "abs(floor(acos(x)+4)) + 1.1*abs(floor(acos(y)+1.5)) +  (acos(x) < (acos(y)-10)) + 1.2*max(-4, acos(x)) + 1.3*min(9, acos(x)-9)" },
    { 2, -3, 3, .1, false, cpp_99misc_47<double>,
      "x,y", "99misc/47", "1.1*(exp(x)+exp(-x)) + 1.2*(exp(y)-exp(-y)) +  1.3*((exp(-x)+exp(x))/2) + 1.4*((exp(-x)-exp(x))/2) + 1.5*(cosh(y)+sinh(y))" },
    { 1, 2, 1e9, 1.2e7, false, cpp_99misc_48<double>,
      "x", "99misc/48", "sinh((log(x)/5+1)*5) + 1.2*cosh((log(x)/log(2)+1)*log(2)) + !(x | !(x/4))" },
    { 1, -100, 100, .03, false, cpp_99misc_49<double>,
      "x", "99misc/49", "atan2(0, x) + (-4*(x-100))^3.3" },
    { 2, -10, 10, 1, false, cpp_99misc_50<double>,
      "x,y", "99misc/50", "(x<y | y<x) +     2*(x<y & y<x) +     4*(x<=y & y<=x) +     8*(x<y & x!=y) +     16*(x<y | x!=y) +     32*(x<=y & x>=y) +     64*(x<=y | x>=y) +     128*(x!=y & x=y) +     256*(x!=y & x!=y) +  512*(x<=y & x=y)" },
    { 1, -10, 10, .5, false, cpp_99misc_52<double>,
      "x", "99misc/52", "x + (1.0+2.0+3.0+4.0-5.0-6.0-7.0-8.0)/3.0 +    4.0*(1.0+sin(2.0)+cos(4.0*5.0+6.0)/2.0) + cos(0.5)*tan(0.6+0.2) -  1.1/log(2.1)*sqrt(3.3) + 2^3" },
    { 2, 0, 10, 0.5, false, cpp_99misc_53<double>,
      "x,y", "99misc/53", "(x&y) + 4*(int(x/10)|int(y/10)) + 8*((-!-!-x)+(!-!-!y)) + 16*(-------x + !!!!!!!y)" },
    { 2, -10, 100, .5, false, cpp_99misc_54<double>,
      "x,y", "99misc/54", "(x<y)+(x<=y)+(x>y)+(x>=y)+    (x=y)+(x!=y)+(x&y)+(x|y)+    (!x)+(!!x)+    !((x<y)&(x<3))+  !!(!(x>y)|(x>3))" },
    { 2, 1,100, .5, false, cpp_99misc_55<double>,
      "x,y", "99misc/55", "(x^1.2 < 0) + (y^2.5 < 0) + 2*(x*x<0) + 3*(y^3<0) + 4*(x^4<0)" },
    { 1, .25, 100, .25, false, cpp_99misc_56<double>,
      "x", "99misc/56", "1.75e21%x" },
    { 2, -11, 11, 1, false, cpp_99misc_58<double>,
      "x,y", "99misc/58", "(-x < 3) + (x*-1 > 5) + (x*-3 < 10) + (x*-3 < y*7) +  (x*4 < y*7) + (x*6 < y*-3) + (-x < 11) + (5 < -y)" },
    { 2, -2, 2, 1, false, cpp_99misc_60<double>,
      "x,y", "99misc/60", "sqr(x) | sub(x,y) | value()" },
    TestType<double>() };

#ifdef FP_SUPPORT_FLOAT_TYPE
template<>
struct RegressionTests<float>
{
    static const TestType<float> Tests[];
};
const TestType<float>
    RegressionTests<float>::Tests[] =
{
    { 3, -2.0f, 2.0f, 0.1f, false, cpp_01unit_operators_addsub_d<float>,
      "x,y,z", "01unit_operators/addsub_d", "x+y+x+x-z+x" },
    { 2, -1.75f, 1.75f, 0.25f, false, cpp_01unit_operators_and_d<float>,
      "x,y", "01unit_operators/and_d", "x&y" },
    { 2, -1.75f, 1.75f, 0.25f, false, cpp_01unit_operators_cmpeq_d<float>,
      "x,y", "01unit_operators/cmpeq_d", "x=y" },
    { 2, -1.75f, 1.75f, 0.25f, false, cpp_01unit_operators_cmpge_d<float>,
      "x,y", "01unit_operators/cmpge_d", "x>=y" },
    { 2, -1.75f, 1.75f, 0.25f, false, cpp_01unit_operators_cmpgt_d<float>,
      "x,y", "01unit_operators/cmpgt_d", "x>y" },
    { 2, -1.75f, 1.75f, 0.25f, false, cpp_01unit_operators_cmple_d<float>,
      "x,y", "01unit_operators/cmple_d", "x<=y" },
    { 2, -1.75f, 1.75f, 0.25f, false, cpp_01unit_operators_cmplt_d<float>,
      "x,y", "01unit_operators/cmplt_d", "x<y" },
    { 2, -1.75f, 1.75f, 0.25f, false, cpp_01unit_operators_cmpne_d<float>,
      "x,y", "01unit_operators/cmpne_d", "x!=y" },
    { 3, -11.0f, 11.0f, 0.3f, false, cpp_01unit_operators_divmul_d<float>,
      "x,y,z", "01unit_operators/divmul_d", "x*y*x*x/z*(x-0.1)" },
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 2, -11.0f, 11.0f, 0.3f, false, cpp_01unit_operators_modf<float>,
      "x,z", "01unit_operators/modf", "x%z" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
    { 1, -1.25f,1.25f,0.25f, false, cpp_01unit_operators_not_d<float>,
      "x", "01unit_operators/not_d", "!x" },
    { 1, -1.25f,1.25f,0.25f, false, cpp_01unit_operators_notnot_d<float>,
      "x", "01unit_operators/notnot_d", "!!x" },
    { 2, -1.75f, 1.75f, 0.25f, false, cpp_01unit_operators_or_d<float>,
      "x,y", "01unit_operators/or_d", "x|y" },
    { 1, -400.0f, 400.0f, 0.5f, false, cpp_02unit_functions_abs_d<float>,
      "x", "02unit_functions/abs_d", "abs(x)" },
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -1.0f, 1.0f, 0.01f, false, cpp_02unit_functions_acosf<float>,
      "x", "02unit_functions/acosf", "acos(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -1.0f, 1.0f, 0.01f, true, cpp_02unit_functions_acosf_deg<float>,
      "x", "02unit_functions/acosf_deg", "acos(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -1.0f, 1.0f, 0.01f, false, cpp_02unit_functions_acoshf<float>,
      "x", "02unit_functions/acoshf", "acosh(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -1.0f, 1.0f, 0.01f, true, cpp_02unit_functions_acoshf_deg<float>,
      "x", "02unit_functions/acoshf_deg", "acosh(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -1.0f, 1.0f, 0.01f, false, cpp_02unit_functions_asinf<float>,
      "x", "02unit_functions/asinf", "asin(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -1.0f, 1.0f, 0.01f, true, cpp_02unit_functions_asinf_deg<float>,
      "x", "02unit_functions/asinf_deg", "asin(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -1.0f, 1.0f, 0.01f, false, cpp_02unit_functions_asinhf<float>,
      "x", "02unit_functions/asinhf", "asinh(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -1.0f, 1.0f, 0.01f, true, cpp_02unit_functions_asinhf_deg<float>,
      "x", "02unit_functions/asinhf_deg", "asinh(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 2, -4.0f, 4.0f, 0.05f, false, cpp_02unit_functions_atan2f<float>,
      "x,y", "02unit_functions/atan2f", "atan2(x,y)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 2, -4.0f, 4.0f, 0.05f, true, cpp_02unit_functions_atan2f_deg<float>,
      "x,y", "02unit_functions/atan2f_deg", "atan2(x,y)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -400.0f, 400.0f, 0.1f, false, cpp_02unit_functions_atanf<float>,
      "x", "02unit_functions/atanf", "atan(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -400.0f, 400.0f, 0.1f, true, cpp_02unit_functions_atanf_deg<float>,
      "x", "02unit_functions/atanf_deg", "atan(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -1.0f, 1.0f, 0.01f, false, cpp_02unit_functions_atanhf<float>,
      "x", "02unit_functions/atanhf", "atanh(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -50000.0f, 50000.0f, 1000.0f, false, cpp_02unit_functions_cbrtf<float>,
      "x", "02unit_functions/cbrtf", "cbrt(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -10.0f, 10.0f, 0.25f, false, cpp_02unit_functions_ceilf<float>,
      "x", "02unit_functions/ceilf", "ceil(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -40000.0f, 40000.0f, 5.0f, false, cpp_02unit_functions_cosf<float>,
      "x", "02unit_functions/cosf", "cos(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -400.0f, 400.0f, 5.0f, true, cpp_02unit_functions_cosf_deg<float>,
      "x", "02unit_functions/cosf_deg", "cos(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -400.0f, 400.0f, 0.1f, false, cpp_02unit_functions_coshf<float>,
      "x", "02unit_functions/coshf", "cosh(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -400.0f, 400.0f, 0.1f, true, cpp_02unit_functions_coshf_deg<float>,
      "x", "02unit_functions/coshf_deg", "cosh(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -90.0f, 90.0f, 0.01f, false, cpp_02unit_functions_exp2f<float>,
      "x", "02unit_functions/exp2f", "exp2(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -90.0f, 90.0f, 0.01f, false, cpp_02unit_functions_expf<float>,
      "x", "02unit_functions/expf", "exp(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -10.0f, 10.0f, 0.25f, false, cpp_02unit_functions_floorf<float>,
      "x", "02unit_functions/floorf", "floor(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 2, -4.0f, 4.0f, 0.05f, false, cpp_02unit_functions_hypotf<float>,
      "x,y", "02unit_functions/hypotf", "hypot(x,y)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
    { 3, -1.25f,1.25f,0.25f, false, cpp_02unit_functions_if_d<float>,
      "x,y,z", "02unit_functions/if_d", "if(x,y,z)" },
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -400.0f, 400.0f, 0.1f, false, cpp_02unit_functions_log2f<float>,
      "x", "02unit_functions/log2f", "log2(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -400.0f, 400.0f, 0.1f, false, cpp_02unit_functions_log10f<float>,
      "x", "02unit_functions/log10f", "log10(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -400.0f, 400.0f, 0.1f, false, cpp_02unit_functions_logf<float>,
      "x", "02unit_functions/logf", "log(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
    { 2, -4.0f, 4.0f, 1.0f, false, cpp_02unit_functions_max<float>,
      "x,y", "02unit_functions/max", "max(x,y)" },
    { 2, -4.0f, 4.0f, 1.0f, false, cpp_02unit_functions_min<float>,
      "x,y", "02unit_functions/min", "min(x,y)" },
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 2, 1.0f,20.0f,1.0f, false, cpp_02unit_functions_pow_negf<float>,
      "x,y", "02unit_functions/pow_negf", "pow(-x*0.25,y)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 2, 0.01f, 4.0f, 0.05f, false, cpp_02unit_functions_pow_posf<float>,
      "x,y", "02unit_functions/pow_posf", "pow(x,y)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -40000.0f, 40000.0f, 5.0f, false, cpp_02unit_functions_sinf<float>,
      "x", "02unit_functions/sinf", "sin(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -400.0f, 400.0f, 5.0f, true, cpp_02unit_functions_sinf_deg<float>,
      "x", "02unit_functions/sinf_deg", "sin(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -400.0f, 400.0f, 0.1f, false, cpp_02unit_functions_sinhf<float>,
      "x", "02unit_functions/sinhf", "sinh(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -400.0f, 400.0f, 0.1f, true, cpp_02unit_functions_sinhf_deg<float>,
      "x", "02unit_functions/sinhf_deg", "sinh(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, 0.0f, 100000.0f, 1000.0f, false, cpp_02unit_functions_sqrtf<float>,
      "x", "02unit_functions/sqrtf", "sqrt(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -1.3f, 1.3f, 0.05f, false, cpp_02unit_functions_tanf<float>,
      "x", "02unit_functions/tanf", "tan(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -89.0f, 89.0f, 0.25f, true, cpp_02unit_functions_tanf_deg<float>,
      "x", "02unit_functions/tanf_deg", "tan(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -40000.0f, 40000.0f, 5.0f, false, cpp_02unit_functions_tanhf<float>,
      "x", "02unit_functions/tanhf", "tanh(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -40000.0f, 40000.0f, 5.0f, true, cpp_02unit_functions_tanhf_deg<float>,
      "x", "02unit_functions/tanhf_deg", "tanh(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, -400.0f, 400.0f, 0.1f, false, cpp_02unit_functions_truncf<float>,
      "x", "02unit_functions/truncf", "trunc(x)" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, 0.0f,1.0f,1.0f, false, cpp_03unit_constants_e_f<float>,
      "x", "03unit_constants/e_f", "x*0+ naturalnumber" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
#if defined(FP_SUPPORT_FLOAT_TYPE)
    { 1, 0.0f,1.0f,1.0f, false, cpp_03unit_constants_pi_f<float>,
      "x", "03unit_constants/pi_f", "x*0+ pi" },
#endif /*defined(FP_SUPPORT_FLOAT_TYPE) */
    { 1, -10.0f,10.0f,1.0f, false, cpp_50regressions_36<float>,
      "x", "50regressions/36", "-if(x<0, x, -x) + -if(x<5, 2, 3)" },
    { 2, -3.0f,3.0f,.025f, false, cpp_50regressions_42<float>,
      "x,y", "50regressions/42", "sqrt(x*x) + 1.5*((y*y)^.25) + hypot(x,y)" },
    { 1, -100.0f, -1.0f, .5f, false, cpp_50regressions_51<float>,
      "x", "50regressions/51", "log(-x)" },
    { 1, .05f, 1.0f, .01f, false, cpp_50regressions_57<float>,
      "x", "50regressions/57", "cosh(asinh(x))" },
    { 2, -2.0f, 2.0f, 1.2f, false, cpp_50regressions_59<float>,
      "x,y", "50regressions/59", "cosh(x^2) + tanh(y^2)" },
    { 1, -1000.0f,1000.0f,.1f, false, cpp_99misc_1<float>,
      "x", "99misc/1", "x*4/2 + (1+(2+3)) + x*x+x+1+2+3*4+5*6*\n7-8*9" },
    { 1, -1000.0f,1000.0f,.1f, false, cpp_99misc_2<float>,
      "x", "99misc/2", "2 * x+ sin ( x ) / .5 + 2-sin(x)*sin(x)" },
    { 3, -10.0f,10.0f,.5f, false, cpp_99misc_3<float>,
      "x,y,z", "99misc/3", "(x=y & y=x)+  1+2-3.1*4e2/.5 + x*x+y*y+z*z" },
    { 2, -10.0f,10.0f,.5f, false, cpp_99misc_4<float>,
      "x,y", "99misc/4", "( ((( ( x-y) -( ((y) *2) -3)) )* 4))+sin(x)*cos(y)-cos(x)*sin(y)" },
    { 2, .1f,10.0f,.05f, false, cpp_99misc_5<float>,
      "__A5_x08,o__5_0AB_", "99misc/5", "__A5_x08^o__5_0AB_" },
#if !defined(FP_DISABLE_EVAL)
    { 2, .1f,10.0f,2.0f, false, cpp_99misc_6<float>,
      "x,y", "99misc/6", "if(x>0&y>0,x*y+eval(x-1,y-1),0)+1" },
#endif /*!defined(FP_DISABLE_EVAL) */
    { 1, -10.0f,10.0f,.001f, false, cpp_99misc_7<float>,
      "x", "99misc/7", "cos(x)*sin(1-x)*(1-cos(x/2)*sin(x*5))" },
    { 2, -10.0f,10.0f,.05f, false, cpp_99misc_8<float>,
      "x,y", "99misc/8", "atan2(x,y)+max(x,y)" },
    { 3, 1.0f,21.0f,.3f, false, cpp_99misc_9<float>,
      "x,y,z", "99misc/9", "1.5+x*y-2+4/8+z+z+z+z+x/(y*z)" },
    { 3, 1.0f,4.0f,.3f, false, cpp_99misc_10<float>,
      "x,y,z", "99misc/10", "1+sin(cos(max(1+2+3+4+5, x+y+z)))+2" },
    { 3, 1.0f,21.0f,.8f, false, cpp_99misc_11<float>,
      "x,y,z", "99misc/11", "-(-(-(-(-x))-x))+y*1+log(1.1^z)" },
    { 1, 1.0f,2000.0f,.05f, false, cpp_99misc_12<float>,
      "x", "99misc/12", "1/log(10^((3-2)/log(x)))" },
    { 2, -50.0f,50.0f,.5f, false, cpp_99misc_13<float>,
      "x,y", "99misc/13", "x^3 * x^4 + y^3 * y^5" },
    { 1, -50.0f,50.0f,.01f, false, cpp_99misc_14<float>,
      "x", "99misc/14", "x*pi + sin(2*pi) + CONST" },
    { 2, 1.1f, 6.0f, .052f, false, cpp_99misc_15<float>,
      "x,y", "99misc/15", "x^y/log(y) + log(x)/log(y) + log(x^y)" },
    { 2, -20.0f, 20.0f, 1.0f, false, cpp_99misc_16<float>,
      "x,y", "99misc/16", "if(x<0, if(y<0, x+y, x-y), if(y>0, x*y, x+2*y))" },
    { 2, -20.0f, 20.0f, 1.0f, false, cpp_99misc_17<float>,
      "x,y", "99misc/17", "sqr(x)+sub(x,y)+psqr(y)+psub(y+1,x-2)-1" },
    { 2, 1.0f,20.0f,.1f, false, cpp_99misc_18<float>,
      "x,y", "99misc/18", "- ( - ( - ( - 5 ) ) ) * -x^ -y^-2" },
    { 2, -100.0f,100.0f,1.0f, false, cpp_99misc_19<float>,
      "x,y", "99misc/19", "(x<y)+10*(x<=y)+100*(x>y)+1000*(x>=y)+10000*(x=y)+100000*(x!=y)+  (x&y)*2+(x|y)*20+(!x)*200+(!!x)*2000+4*!((x<y)&(x<3))+40*!!(!(x>y)|(x>3))" },
    { 2, -100.0f,100.0f,1.0f, false, cpp_99misc_20<float>,
      "x,y", "99misc/20", "(!(x != y) & !x) + !(!(!(!y)))" },
    { 1, -10.0f,10.0f,1.0f, false, cpp_99misc_21<float>,
      "x", "99misc/21", "sqr(x)+value()-pvalue ( )" },
    { 1, -1000.0f,1000.0f,.1f, false, cpp_99misc_23<float>,
      "x", "99misc/23", "(x/(2*acos(0)))*180" },
    { 1, -1000.0f,1000.0f,1.0f, false, cpp_99misc_24<float>,
      "x", "99misc/24", "(min(x, min(1,x)) + min(x, 1))/2 + min(x, 1)*3 + max(0, min(-2,0))" },
    { 3, 1.0f,3.0f,.1f, false, cpp_99misc_25<float>,
      "a,b,c", "99misc/25", "a^b^c + a^-2 * (-b^2) + (-b^-c)" },
    { 1, -100.0f,100.0f,.1f, true, cpp_99misc_26<float>,
      "x", "99misc/26", "sin(x) + cos(x*1.5) + asin(x/110) + acos(x/120)" },
    { 2, .1f, .9f, .025f, false, cpp_99misc_27<float>,
      "x,y", "99misc/27", "abs(x)+acos(x)+asin(x)+atan(x)+atan2(x,y)+ceil(x)+cos(x)+cosh(x)+cot(x)+csc(x) + pow(x,y)" },
    { 2, .1f, .9f, .025f, false, cpp_99misc_28<float>,
      "x,y", "99misc/28", "exp(x)+floor(x)+int(x)+log(x)+log10(x)+max(x,y)+min(x,y)+sec(x)+sin(x)+sinh(x)+sqrt(x)+tan(x)+tanh(x)+ceil(y)+trunc(y)" },
    { 2, -5.0f,5.0f,1.0f, false, cpp_99misc_29<float>,
      "x,y", "99misc/29", "x-y*1" },
    { 2, 3.0f,10.0f,1.0f, false, cpp_99misc_30<float>,
      "x,y", "99misc/30", "x - y*1 + (x%y) + x / (y^1.1) + 2^3 + 5%3 + x^(y^0) + x^0.5" },
    { 3, 1.0f,2.0f,.05f, false, cpp_99misc_32<float>,
      "x,y,z", "99misc/32", "x     +y/y-min(3,4)-x-max(4,3)+max(3,4)-min(4,3)+0+(z*1)     +(x-2+2)+(x*0.5*2)+y*0     +min(min(min(4.0,x),1.0),min(x,min(min(y,4.0),z)))     +max(max(max(4.0,x),1.0),max(x,max(max(y,4.0),z)))     +(abs(1)+acos(1.0)+asin(1.0)+atan(1.0)+ceil(1.1)+cos(0.0)      +cosh(0.0)+floor(1.1)+log(1.0)+sin(0.0)+sinh(0.0)+tan(1.0)      +tanh(1.0)+atan2(1.0,1.0))     +(x-(y-z))     +(x+y) + (x*y)     +max(x,max(x,max(x,max(x,x))))*-1.0     +(z-z)     +1/sin(x/5) + 1/cos(y/5) + 1/tan(z/5)     +log10(cot(z/5) + csc(y/5) + sec(x/5))     +log(30+x)*log(40+y)/log(50+z)     +sin(x/57.295779513082320877)     +asin(x/10)*57.295779513082320877     +floor(-x) + 1/ceil(x)     +sqrt(5 * 0.2) +(-x+-x+-x+-x+-x+-x)" },
    { 4, -5.0f,5.0f,1.0f, false, cpp_99misc_34<float>,
      "t,う,お,早", "99misc/34", "お+早*う-t" },
    { 3, -10.0f,10.0f,1.0f, false, cpp_99misc_35<float>,
      "A_very_long_variable_name_1,A_very_long_variable_name_2,Yet_a_third_very_long_variable_name", "99misc/35", "A_very_long_variable_name_1-A_very_long_variable_name_2+  Yet_a_third_very_long_variable_name*A_very_long_variable_name_1" },
    { 1, -10.0f,10.0f,.1f, false, cpp_99misc_37<float>,
      "x", "99misc/37", "5 + 7.5*8 / 3 - 2^4*2 + 7%2+4 + x" },
    { 3, -2.0f, 2.0f, .15f, false, cpp_99misc_41<float>,
      "x,y,z", "99misc/41", "x*3+x*y+x*z+x*sin(y*z) -    (sin(x)+cos(y))*4 +    (sin(x)+cos(y))*x +    (sin(x)+cos(y))*y +  (sin(x)+cos(y))*z" },
    { 1, 0.0f,100.0f,.025f, false, cpp_99misc_44<float>,
      "x", "99misc/44", "(x^2)^(1/8) +    1.1*(x^3)^(1/7) +    1.2*(x^4)^(1/6) +    1.3*(x^5)^(1/5) +    1.4*(x^6)^(1/6) +    1.5*(x^7)^(1/4) +    1.6*(x^8)^(1/3) +    1.7*(x^9)^(1/2) +  1.8*(sqrt(abs(-sqrt(x))^3))" },
    { 1, -10.0f,10.0f,.025f, false, cpp_99misc_45<float>,
      "x", "99misc/45", "(x^2)^(1/7) + 1.1*(x^4)^(1/5) + 1.2*(x^6)^(1/3)" },
    { 2, -.9f, .9f, .015f, false, cpp_99misc_46<float>,
      "x,y", "99misc/46", "abs(floor(acos(x)+4)) + 1.1*abs(floor(acos(y)+1.5)) +  (acos(x) < (acos(y)-10)) + 1.2*max(-4, acos(x)) + 1.3*min(9, acos(x)-9)" },
    { 2, -3.0f, 3.0f, .1f, false, cpp_99misc_47<float>,
      "x,y", "99misc/47", "1.1*(exp(x)+exp(-x)) + 1.2*(exp(y)-exp(-y)) +  1.3*((exp(-x)+exp(x))/2) + 1.4*((exp(-x)-exp(x))/2) + 1.5*(cosh(y)+sinh(y))" },
    { 1, 2.0f, 1e9f, 1.2e7f, false, cpp_99misc_48<float>,
      "x", "99misc/48", "sinh((log(x)/5+1)*5) + 1.2*cosh((log(x)/log(2)+1)*log(2)) + !(x | !(x/4))" },
    { 1, -100.0f, 100.0f, .03f, false, cpp_99misc_49<float>,
      "x", "99misc/49", "atan2(0, x) + (-4*(x-100))^3.3" },
    { 2, -10.0f, 10.0f, 1.0f, false, cpp_99misc_50<float>,
      "x,y", "99misc/50", "(x<y | y<x) +     2*(x<y & y<x) +     4*(x<=y & y<=x) +     8*(x<y & x!=y) +     16*(x<y | x!=y) +     32*(x<=y & x>=y) +     64*(x<=y | x>=y) +     128*(x!=y & x=y) +     256*(x!=y & x!=y) +  512*(x<=y & x=y)" },
    { 1, -10.0f, 10.0f, .5f, false, cpp_99misc_52<float>,
      "x", "99misc/52", "x + (1.0+2.0+3.0+4.0-5.0-6.0-7.0-8.0)/3.0 +    4.0*(1.0+sin(2.0)+cos(4.0*5.0+6.0)/2.0) + cos(0.5)*tan(0.6+0.2) -  1.1/log(2.1)*sqrt(3.3) + 2^3" },
    { 2, 0.0f, 10.0f, 0.5f, false, cpp_99misc_53<float>,
      "x,y", "99misc/53", "(x&y) + 4*(int(x/10)|int(y/10)) + 8*((-!-!-x)+(!-!-!y)) + 16*(-------x + !!!!!!!y)" },
    { 2, -10.0f, 100.0f, .5f, false, cpp_99misc_54<float>,
      "x,y", "99misc/54", "(x<y)+(x<=y)+(x>y)+(x>=y)+    (x=y)+(x!=y)+(x&y)+(x|y)+    (!x)+(!!x)+    !((x<y)&(x<3))+  !!(!(x>y)|(x>3))" },
    { 2, 1.0f,100.0f, .5f, false, cpp_99misc_55<float>,
      "x,y", "99misc/55", "(x^1.2 < 0) + (y^2.5 < 0) + 2*(x*x<0) + 3*(y^3<0) + 4*(x^4<0)" },
    { 1, .25f, 100.0f, .25f, false, cpp_99misc_56<float>,
      "x", "99misc/56", "1.75e21%x" },
    { 2, -11.0f, 11.0f, 1.0f, false, cpp_99misc_58<float>,
      "x,y", "99misc/58", "(-x < 3) + (x*-1 > 5) + (x*-3 < 10) + (x*-3 < y*7) +  (x*4 < y*7) + (x*6 < y*-3) + (-x < 11) + (5 < -y)" },
    { 2, -2.0f, 2.0f, 1.0f, false, cpp_99misc_60<float>,
      "x,y", "99misc/60", "sqr(x) | sub(x,y) | value()" },
    TestType<float>() };
#endif /*FP_SUPPORT_FLOAT_TYPE */

#ifdef FP_SUPPORT_LONG_INT_TYPE
template<>
struct RegressionTests<long>
{
    static const TestType<long> Tests[];
};
const TestType<long>
    RegressionTests<long>::Tests[] =
{
    { 3, -40l, 40l, 7l, false, cpp_01unit_operators_addsub_i<long>,
      "x,y,z", "01unit_operators/addsub_i", "x+y+x+x-z+x" },
    { 2, -2l, 2l, 1l, false, cpp_01unit_operators_and_i<long>,
      "x,y", "01unit_operators/and_i", "x&y" },
    { 2, -2l, 2l, 1l, false, cpp_01unit_operators_cmpeq_i<long>,
      "x,y", "01unit_operators/cmpeq_i", "x=y" },
    { 2, -2l, 2l, 1l, false, cpp_01unit_operators_cmpge_i<long>,
      "x,y", "01unit_operators/cmpge_i", "x>=y" },
    { 2, -2l, 2l, 1l, false, cpp_01unit_operators_cmpgt_i<long>,
      "x,y", "01unit_operators/cmpgt_i", "x>y" },
    { 2, -2l, 2l, 1l, false, cpp_01unit_operators_cmple_i<long>,
      "x,y", "01unit_operators/cmple_i", "x<=y" },
    { 2, -2l, 2l, 1l, false, cpp_01unit_operators_cmplt_i<long>,
      "x,y", "01unit_operators/cmplt_i", "x<y" },
    { 2, -2l, 2l, 1l, false, cpp_01unit_operators_cmpne_i<long>,
      "x,y", "01unit_operators/cmpne_i", "x!=y" },
    { 3, -460l, 100l, 8l, false, cpp_01unit_operators_divmul_i<long>,
      "x,y,z", "01unit_operators/divmul_i", "x*y*x*x/(z+3)*(x+4)" },
    { 2, -460l, 100l, 8l, false, cpp_01unit_operators_mod_i<long>,
      "x,z", "01unit_operators/mod_i", "x%(z+3)" },
    { 1, -2l, 2l, 1l, false, cpp_01unit_operators_not_i<long>,
      "x", "01unit_operators/not_i", "!x" },
    { 1, -2l, 2l, 1l, false, cpp_01unit_operators_notnot_i<long>,
      "x", "01unit_operators/notnot_i", "!!x" },
    { 2, -2l, 2l, 1l, false, cpp_01unit_operators_or_i<long>,
      "x,y", "01unit_operators/or_i", "x|y" },
    { 1, -40000l,40000l,1l, false, cpp_02unit_functions_abs_i<long>,
      "x", "02unit_functions/abs_i", "abs(x)" },
    { 3, -2l,2l,1l, false, cpp_02unit_functions_if_i<long>,
      "x,y,z", "02unit_functions/if_i", "if(x,y,z)" },
    { 2, -4l, 4l, 1l, false, cpp_02unit_functions_max<long>,
      "x,y", "02unit_functions/max", "max(x,y)" },
    { 2, -4l, 4l, 1l, false, cpp_02unit_functions_min<long>,
      "x,y", "02unit_functions/min", "min(x,y)" },
    { 1, -10l,10l,1l, false, cpp_50regressions_36<long>,
      "x", "50regressions/36", "-if(x<0, x, -x) + -if(x<5, 2, 3)" },
    { 2, -20l, 20l, 1l, false, cpp_99misc_16<long>,
      "x,y", "99misc/16", "if(x<0, if(y<0, x+y, x-y), if(y>0, x*y, x+2*y))" },
    { 2, -20l, 20l, 1l, false, cpp_99misc_17<long>,
      "x,y", "99misc/17", "sqr(x)+sub(x,y)+psqr(y)+psub(y+1,x-2)-1" },
    { 2, -100l,100l,1l, false, cpp_99misc_19<long>,
      "x,y", "99misc/19", "(x<y)+10*(x<=y)+100*(x>y)+1000*(x>=y)+10000*(x=y)+100000*(x!=y)+  (x&y)*2+(x|y)*20+(!x)*200+(!!x)*2000+4*!((x<y)&(x<3))+40*!!(!(x>y)|(x>3))" },
    { 2, -100l,100l,1l, false, cpp_99misc_20<long>,
      "x,y", "99misc/20", "(!(x != y) & !x) + !(!(!(!y)))" },
    { 1, -10l,10l,1l, false, cpp_99misc_21<long>,
      "x", "99misc/21", "sqr(x)+value()-pvalue ( )" },
    { 1, -1000l,1000l,1l, false, cpp_99misc_24<long>,
      "x", "99misc/24", "(min(x, min(1,x)) + min(x, 1))/2 + min(x, 1)*3 + max(0, min(-2,0))" },
    { 2, -5l,5l,1l, false, cpp_99misc_29<long>,
      "x,y", "99misc/29", "x-y*1" },
    { 4, -5l,5l,1l, false, cpp_99misc_34<long>,
      "t,う,お,早", "99misc/34", "お+早*う-t" },
    { 3, -10l,10l,1l, false, cpp_99misc_35<long>,
      "A_very_long_variable_name_1,A_very_long_variable_name_2,Yet_a_third_very_long_variable_name", "99misc/35", "A_very_long_variable_name_1-A_very_long_variable_name_2+  Yet_a_third_very_long_variable_name*A_very_long_variable_name_1" },
    { 2, -10l, 10l, 1l, false, cpp_99misc_50<long>,
      "x,y", "99misc/50", "(x<y | y<x) +     2*(x<y & y<x) +     4*(x<=y & y<=x) +     8*(x<y & x!=y) +     16*(x<y | x!=y) +     32*(x<=y & x>=y) +     64*(x<=y | x>=y) +     128*(x!=y & x=y) +     256*(x!=y & x!=y) +  512*(x<=y & x=y)" },
    { 2, -11l, 11l, 1l, false, cpp_99misc_58<long>,
      "x,y", "99misc/58", "(-x < 3) + (x*-1 > 5) + (x*-3 < 10) + (x*-3 < y*7) +  (x*4 < y*7) + (x*6 < y*-3) + (-x < 11) + (5 < -y)" },
    { 2, -2l, 2l, 1l, false, cpp_99misc_60<long>,
      "x,y", "99misc/60", "sqr(x) | sub(x,y) | value()" },
    { 3, -8l,7l,1l, false, cpp_99misc_i1<long>,
      "x,y,z", "99misc/i1", "1+2+3-4*5*6/3+10/2-9%2 + (x+y - 11*x + z/10 + x/(z+31))" },
    { 3, -7l,7l,1l, false, cpp_99misc_i2<long>,
      "x,y,z", "99misc/i2", "if(abs(x*y) < 20 | x+y > 30 & z > 5, min(x,2*y), max(y,z*2))" },
    { 3, 1l,7l,1l, false, cpp_99misc_i3<long>,
      "x,y,z", "99misc/i3", "(x+y) + 2*(x-z) + 3*(x*y) + 4*(y/z) + 5*(x%z) +    6*(x<y) + 7*(x<=z) + 8*(x>2*z) + 9*(y>=3*z) + 10*(x+y!=z) +  11*(100+x) + 12*(101-y) + 13*(102*z) + 14*(103/x)" },
    TestType<long>() };
#endif /*FP_SUPPORT_LONG_INT_TYPE */

#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
template<>
struct RegressionTests<long double>
{
    static const TestType<long double> Tests[];
};
const TestType<long double>
    RegressionTests<long double>::Tests[] =
{
    { 3, -2.0l, 2.0l, 0.1l, false, cpp_01unit_operators_addsub_d<long double>,
      "x,y,z", "01unit_operators/addsub_d", "x+y+x+x-z+x" },
    { 2, -1.75l, 1.75l, 0.25l, false, cpp_01unit_operators_and_d<long double>,
      "x,y", "01unit_operators/and_d", "x&y" },
    { 2, -1.75l, 1.75l, 0.25l, false, cpp_01unit_operators_cmpeq_d<long double>,
      "x,y", "01unit_operators/cmpeq_d", "x=y" },
    { 2, -1.75l, 1.75l, 0.25l, false, cpp_01unit_operators_cmpge_d<long double>,
      "x,y", "01unit_operators/cmpge_d", "x>=y" },
    { 2, -1.75l, 1.75l, 0.25l, false, cpp_01unit_operators_cmpgt_d<long double>,
      "x,y", "01unit_operators/cmpgt_d", "x>y" },
    { 2, -1.75l, 1.75l, 0.25l, false, cpp_01unit_operators_cmple_d<long double>,
      "x,y", "01unit_operators/cmple_d", "x<=y" },
    { 2, -1.75l, 1.75l, 0.25l, false, cpp_01unit_operators_cmplt_d<long double>,
      "x,y", "01unit_operators/cmplt_d", "x<y" },
    { 2, -1.75l, 1.75l, 0.25l, false, cpp_01unit_operators_cmpne_d<long double>,
      "x,y", "01unit_operators/cmpne_d", "x!=y" },
    { 3, -11.0l, 11.0l, 0.3l, false, cpp_01unit_operators_divmul_d<long double>,
      "x,y,z", "01unit_operators/divmul_d", "x*y*x*x/z*(x-0.1)" },
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 2, -11.0l, 11.0l, 0.3l, false, cpp_01unit_operators_modl<long double>,
      "x,z", "01unit_operators/modl", "x%z" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
    { 1, -1.25l,1.25l,0.25l, false, cpp_01unit_operators_not_d<long double>,
      "x", "01unit_operators/not_d", "!x" },
    { 1, -1.25l,1.25l,0.25l, false, cpp_01unit_operators_notnot_d<long double>,
      "x", "01unit_operators/notnot_d", "!!x" },
    { 2, -1.75l, 1.75l, 0.25l, false, cpp_01unit_operators_or_d<long double>,
      "x,y", "01unit_operators/or_d", "x|y" },
    { 1, -400.0l, 400.0l, 0.5l, false, cpp_02unit_functions_abs_d<long double>,
      "x", "02unit_functions/abs_d", "abs(x)" },
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -1.0l, 1.0l, 0.01l, false, cpp_02unit_functions_acoshl<long double>,
      "x", "02unit_functions/acoshl", "acosh(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -1.0l, 1.0l, 0.01l, true, cpp_02unit_functions_acoshl_deg<long double>,
      "x", "02unit_functions/acoshl_deg", "acosh(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -1.0l, 1.0l, 0.01l, false, cpp_02unit_functions_acosl<long double>,
      "x", "02unit_functions/acosl", "acos(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -1.0l, 1.0l, 0.01l, true, cpp_02unit_functions_acosl_deg<long double>,
      "x", "02unit_functions/acosl_deg", "acos(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -1.0l, 1.0l, 0.01l, false, cpp_02unit_functions_asinhl<long double>,
      "x", "02unit_functions/asinhl", "asinh(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -1.0l, 1.0l, 0.01l, true, cpp_02unit_functions_asinhl_deg<long double>,
      "x", "02unit_functions/asinhl_deg", "asinh(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -1.0l, 1.0l, 0.01l, false, cpp_02unit_functions_asinl<long double>,
      "x", "02unit_functions/asinl", "asin(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -1.0l, 1.0l, 0.01l, true, cpp_02unit_functions_asinl_deg<long double>,
      "x", "02unit_functions/asinl_deg", "asin(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 2, -4.0l, 4.0l, 0.05l, false, cpp_02unit_functions_atan2l<long double>,
      "x,y", "02unit_functions/atan2l", "atan2(x,y)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 2, -4.0l, 4.0l, 0.05l, true, cpp_02unit_functions_atan2l_deg<long double>,
      "x,y", "02unit_functions/atan2l_deg", "atan2(x,y)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -1.0l, 1.0l, 0.01l, false, cpp_02unit_functions_atanhl<long double>,
      "x", "02unit_functions/atanhl", "atanh(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -400.0l, 400.0l, 0.1l, false, cpp_02unit_functions_atanl<long double>,
      "x", "02unit_functions/atanl", "atan(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -400.0l, 400.0l, 0.1l, true, cpp_02unit_functions_atanl_deg<long double>,
      "x", "02unit_functions/atanl_deg", "atan(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -50000.0l, 50000.0l, 1000.0l, false, cpp_02unit_functions_cbrtl<long double>,
      "x", "02unit_functions/cbrtl", "cbrt(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -10.0l, 10.0l, 0.25l, false, cpp_02unit_functions_ceill<long double>,
      "x", "02unit_functions/ceill", "ceil(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -400.0l, 400.0l, 0.1l, false, cpp_02unit_functions_coshl<long double>,
      "x", "02unit_functions/coshl", "cosh(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -400.0l, 400.0l, 0.1l, true, cpp_02unit_functions_coshl_deg<long double>,
      "x", "02unit_functions/coshl_deg", "cosh(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -40000.0l, 40000.0l, 5.0l, false, cpp_02unit_functions_cosl<long double>,
      "x", "02unit_functions/cosl", "cos(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -40000.0l, 40000.0l, 5.0l, true, cpp_02unit_functions_cosl_deg<long double>,
      "x", "02unit_functions/cosl_deg", "cos(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -90.0l, 90.0l, 0.01l, false, cpp_02unit_functions_exp2l<long double>,
      "x", "02unit_functions/exp2l", "exp2(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -90.0l, 90.0l, 0.01l, false, cpp_02unit_functions_expl<long double>,
      "x", "02unit_functions/expl", "exp(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -10.0l, 10.0l, 0.25l, false, cpp_02unit_functions_floorl<long double>,
      "x", "02unit_functions/floorl", "floor(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 2, -4.0l, 4.0l, 0.05l, false, cpp_02unit_functions_hypotl<long double>,
      "x,y", "02unit_functions/hypotl", "hypot(x,y)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
    { 3, -1.25l,1.25l,0.25l, false, cpp_02unit_functions_if_d<long double>,
      "x,y,z", "02unit_functions/if_d", "if(x,y,z)" },
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -400.0l, 400.0l, 0.1l, false, cpp_02unit_functions_log2l<long double>,
      "x", "02unit_functions/log2l", "log2(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -400.0l, 400.0l, 0.1l, false, cpp_02unit_functions_log10l<long double>,
      "x", "02unit_functions/log10l", "log10(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -400.0l, 400.0l, 0.1l, false, cpp_02unit_functions_logl<long double>,
      "x", "02unit_functions/logl", "log(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
    { 2, -4.0l, 4.0l, 1.0l, false, cpp_02unit_functions_max<long double>,
      "x,y", "02unit_functions/max", "max(x,y)" },
    { 2, -4.0l, 4.0l, 1.0l, false, cpp_02unit_functions_min<long double>,
      "x,y", "02unit_functions/min", "min(x,y)" },
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 2, 1.0l,20.0l,1.0l, false, cpp_02unit_functions_pow_negl<long double>,
      "x,y", "02unit_functions/pow_negl", "pow(-x*0.25,y)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 2, 0.01l, 4.0l, 0.05l, false, cpp_02unit_functions_pow_posl<long double>,
      "x,y", "02unit_functions/pow_posl", "pow(x,y)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -400.0l, 400.0l, 0.1l, false, cpp_02unit_functions_sinhl<long double>,
      "x", "02unit_functions/sinhl", "sinh(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -400.0l, 400.0l, 0.1l, true, cpp_02unit_functions_sinhl_deg<long double>,
      "x", "02unit_functions/sinhl_deg", "sinh(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -40000.0l, 40000.0l, 5.0l, false, cpp_02unit_functions_sinl<long double>,
      "x", "02unit_functions/sinl", "sin(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -40000.0l, 40000.0l, 5.0l, true, cpp_02unit_functions_sinl_deg<long double>,
      "x", "02unit_functions/sinl_deg", "sin(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, 0.0l, 100000.0l, 1000.0l, false, cpp_02unit_functions_sqrtl<long double>,
      "x", "02unit_functions/sqrtl", "sqrt(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -40000.0l, 40000.0l, 5.0l, false, cpp_02unit_functions_tanhl<long double>,
      "x", "02unit_functions/tanhl", "tanh(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -40000.0l, 40000.0l, 5.0l, true, cpp_02unit_functions_tanhl_deg<long double>,
      "x", "02unit_functions/tanhl_deg", "tanh(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -1.3l, 1.3l, 0.05l, false, cpp_02unit_functions_tanl<long double>,
      "x", "02unit_functions/tanl", "tan(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -89.0l, 89.0l, 0.25l, true, cpp_02unit_functions_tanl_deg<long double>,
      "x", "02unit_functions/tanl_deg", "tan(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, -400.0l, 400.0l, 0.1l, false, cpp_02unit_functions_truncl<long double>,
      "x", "02unit_functions/truncl", "trunc(x)" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, 0.0l,1.0l,1.0l, false, cpp_03unit_constants_e_ld<long double>,
      "x", "03unit_constants/e_ld", "x*0+ naturalnumber" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
#if defined(FP_SUPPORT_LONG_DOUBLE_TYPE)
    { 1, 0.0l,1.0l,1.0l, false, cpp_03unit_constants_pi_ld<long double>,
      "x", "03unit_constants/pi_ld", "x*0+ pi" },
#endif /*defined(FP_SUPPORT_LONG_DOUBLE_TYPE) */
    { 1, -10.0l,10.0l,1.0l, false, cpp_50regressions_36<long double>,
      "x", "50regressions/36", "-if(x<0, x, -x) + -if(x<5, 2, 3)" },
    { 2, -3.0l,3.0l,.025l, false, cpp_50regressions_42<long double>,
      "x,y", "50regressions/42", "sqrt(x*x) + 1.5*((y*y)^.25) + hypot(x,y)" },
    { 1, -100.0l, -1.0l, .5l, false, cpp_50regressions_51<long double>,
      "x", "50regressions/51", "log(-x)" },
    { 1, .05l, 1.0l, .01l, false, cpp_50regressions_57<long double>,
      "x", "50regressions/57", "cosh(asinh(x))" },
    { 2, -2.0l, 2.0l, 1.2l, false, cpp_50regressions_59<long double>,
      "x,y", "50regressions/59", "cosh(x^2) + tanh(y^2)" },
    { 1, -1000.0l,1000.0l,.1l, false, cpp_99misc_1<long double>,
      "x", "99misc/1", "x*4/2 + (1+(2+3)) + x*x+x+1+2+3*4+5*6*\n7-8*9" },
    { 1, -1000.0l,1000.0l,.1l, false, cpp_99misc_2<long double>,
      "x", "99misc/2", "2 * x+ sin ( x ) / .5 + 2-sin(x)*sin(x)" },
    { 3, -10.0l,10.0l,.5l, false, cpp_99misc_3<long double>,
      "x,y,z", "99misc/3", "(x=y & y=x)+  1+2-3.1*4e2/.5 + x*x+y*y+z*z" },
    { 2, -10.0l,10.0l,.5l, false, cpp_99misc_4<long double>,
      "x,y", "99misc/4", "( ((( ( x-y) -( ((y) *2) -3)) )* 4))+sin(x)*cos(y)-cos(x)*sin(y)" },
    { 2, .1l,10.0l,.05l, false, cpp_99misc_5<long double>,
      "__A5_x08,o__5_0AB_", "99misc/5", "__A5_x08^o__5_0AB_" },
#if !defined(FP_DISABLE_EVAL)
    { 2, .1l,10.0l,2.0l, false, cpp_99misc_6<long double>,
      "x,y", "99misc/6", "if(x>0&y>0,x*y+eval(x-1,y-1),0)+1" },
#endif /*!defined(FP_DISABLE_EVAL) */
    { 1, -10.0l,10.0l,.001l, false, cpp_99misc_7<long double>,
      "x", "99misc/7", "cos(x)*sin(1-x)*(1-cos(x/2)*sin(x*5))" },
    { 2, -10.0l,10.0l,.05l, false, cpp_99misc_8<long double>,
      "x,y", "99misc/8", "atan2(x,y)+max(x,y)" },
    { 3, 1.0l,21.0l,.3l, false, cpp_99misc_9<long double>,
      "x,y,z", "99misc/9", "1.5+x*y-2+4/8+z+z+z+z+x/(y*z)" },
    { 3, 1.0l,4.0l,.3l, false, cpp_99misc_10<long double>,
      "x,y,z", "99misc/10", "1+sin(cos(max(1+2+3+4+5, x+y+z)))+2" },
    { 3, 1.0l,21.0l,.8l, false, cpp_99misc_11<long double>,
      "x,y,z", "99misc/11", "-(-(-(-(-x))-x))+y*1+log(1.1^z)" },
    { 1, 1.0l,2000.0l,.05l, false, cpp_99misc_12<long double>,
      "x", "99misc/12", "1/log(10^((3-2)/log(x)))" },
    { 2, -50.0l,50.0l,.5l, false, cpp_99misc_13<long double>,
      "x,y", "99misc/13", "x^3 * x^4 + y^3 * y^5" },
    { 1, -50.0l,50.0l,.01l, false, cpp_99misc_14<long double>,
      "x", "99misc/14", "x*pi + sin(2*pi) + CONST" },
    { 2, 1.1l, 6.0l, .052l, false, cpp_99misc_15<long double>,
      "x,y", "99misc/15", "x^y/log(y) + log(x)/log(y) + log(x^y)" },
    { 2, -20.0l, 20.0l, 1.0l, false, cpp_99misc_16<long double>,
      "x,y", "99misc/16", "if(x<0, if(y<0, x+y, x-y), if(y>0, x*y, x+2*y))" },
    { 2, -20.0l, 20.0l, 1.0l, false, cpp_99misc_17<long double>,
      "x,y", "99misc/17", "sqr(x)+sub(x,y)+psqr(y)+psub(y+1,x-2)-1" },
    { 2, 1.0l,20.0l,.1l, false, cpp_99misc_18<long double>,
      "x,y", "99misc/18", "- ( - ( - ( - 5 ) ) ) * -x^ -y^-2" },
    { 2, -100.0l,100.0l,1.0l, false, cpp_99misc_19<long double>,
      "x,y", "99misc/19", "(x<y)+10*(x<=y)+100*(x>y)+1000*(x>=y)+10000*(x=y)+100000*(x!=y)+  (x&y)*2+(x|y)*20+(!x)*200+(!!x)*2000+4*!((x<y)&(x<3))+40*!!(!(x>y)|(x>3))" },
    { 2, -100.0l,100.0l,1.0l, false, cpp_99misc_20<long double>,
      "x,y", "99misc/20", "(!(x != y) & !x) + !(!(!(!y)))" },
    { 1, -10.0l,10.0l,1.0l, false, cpp_99misc_21<long double>,
      "x", "99misc/21", "sqr(x)+value()-pvalue ( )" },
    { 2, -4.0l,4.0l,.1l, false, cpp_99misc_22<long double>,
      "x,y", "99misc/22", "3.5doubled + 10*x tripled - sin(y)doubled +    100*(x doubled-y tripled)doubled + 5/2doubled + 1.1^x doubled +  1.1doubled^x doubled" },
    { 1, -1000.0l,1000.0l,.1l, false, cpp_99misc_23<long double>,
      "x", "99misc/23", "(x/(2*acos(0)))*180" },
    { 1, -1000.0l,1000.0l,1.0l, false, cpp_99misc_24<long double>,
      "x", "99misc/24", "(min(x, min(1,x)) + min(x, 1))/2 + min(x, 1)*3 + max(0, min(-2,0))" },
    { 3, 1.0l,3.0l,.1l, false, cpp_99misc_25<long double>,
      "a,b,c", "99misc/25", "a^b^c + a^-2 * (-b^2) + (-b^-c)" },
    { 1, -100.0l,100.0l,.1l, true, cpp_99misc_26<long double>,
      "x", "99misc/26", "sin(x) + cos(x*1.5) + asin(x/110) + acos(x/120)" },
    { 2, .1l, .9l, .025l, false, cpp_99misc_27<long double>,
      "x,y", "99misc/27", "abs(x)+acos(x)+asin(x)+atan(x)+atan2(x,y)+ceil(x)+cos(x)+cosh(x)+cot(x)+csc(x) + pow(x,y)" },
    { 2, .1l, .9l, .025l, false, cpp_99misc_28<long double>,
      "x,y", "99misc/28", "exp(x)+floor(x)+int(x)+log(x)+log10(x)+max(x,y)+min(x,y)+sec(x)+sin(x)+sinh(x)+sqrt(x)+tan(x)+tanh(x)+ceil(y)+trunc(y)" },
    { 2, -5.0l,5.0l,1.0l, false, cpp_99misc_29<long double>,
      "x,y", "99misc/29", "x-y*1" },
    { 2, 3.0l,10.0l,1.0l, false, cpp_99misc_30<long double>,
      "x,y", "99misc/30", "x - y*1 + (x%y) + x / (y^1.1) + 2^3 + 5%3 + x^(y^0) + x^0.5" },
    { 3, .1l,4.0l,.3l, false, cpp_99misc_31<long double>,
      "x,y,z", "99misc/31", "x - (y*(y*(y*-1))*1) + log(x*exp(1.0)^y) - log(x^y) +    exp(1.0)^log(x+6) + 10^(log(x+6)/log(y+6)*log(z+6)/log(10)) -    exp(1.0)^(log(x+6)*y) - 5^(log(x+7)/log(5)) + (x*z+17)^3 * (x*z+17)^2 /  (x*z+17)^4" },
    { 3, 1.0l,2.0l,.05l, false, cpp_99misc_32<long double>,
      "x,y,z", "99misc/32", "x     +y/y-min(3,4)-x-max(4,3)+max(3,4)-min(4,3)+0+(z*1)     +(x-2+2)+(x*0.5*2)+y*0     +min(min(min(4.0,x),1.0),min(x,min(min(y,4.0),z)))     +max(max(max(4.0,x),1.0),max(x,max(max(y,4.0),z)))     +(abs(1)+acos(1.0)+asin(1.0)+atan(1.0)+ceil(1.1)+cos(0.0)      +cosh(0.0)+floor(1.1)+log(1.0)+sin(0.0)+sinh(0.0)+tan(1.0)      +tanh(1.0)+atan2(1.0,1.0))     +(x-(y-z))     +(x+y) + (x*y)     +max(x,max(x,max(x,max(x,x))))*-1.0     +(z-z)     +1/sin(x/5) + 1/cos(y/5) + 1/tan(z/5)     +log10(cot(z/5) + csc(y/5) + sec(x/5))     +log(30+x)*log(40+y)/log(50+z)     +sin(x/57.295779513082320877)     +asin(x/10)*57.295779513082320877     +floor(-x) + 1/ceil(x)     +sqrt(5 * 0.2) +(-x+-x+-x+-x+-x+-x)" },
    { 2, -2.0l,2.0l,.1l, false, cpp_99misc_33<long double>,
      "x,y", "99misc/33", "sin(sqrt(10-x*x+y*y))+cos(sqrt(15-x*x-y*y))+sin(x*x+y*y)" },
    { 4, -5.0l,5.0l,1.0l, false, cpp_99misc_34<long double>,
      "t,う,お,早", "99misc/34", "お+早*う-t" },
    { 3, -10.0l,10.0l,1.0l, false, cpp_99misc_35<long double>,
      "A_very_long_variable_name_1,A_very_long_variable_name_2,Yet_a_third_very_long_variable_name", "99misc/35", "A_very_long_variable_name_1-A_very_long_variable_name_2+  Yet_a_third_very_long_variable_name*A_very_long_variable_name_1" },
    { 1, -10.0l,10.0l,.1l, false, cpp_99misc_37<long double>,
      "x", "99misc/37", "5 + 7.5*8 / 3 - 2^4*2 + 7%2+4 + x" },
    { 3, -.9l, .9l, .05l, false, cpp_99misc_38<long double>,
      "x,y,z", "99misc/38", "asinh(x) + 1.5*acosh(y+3) + 2.2*atanh(z)" },
    { 3, -1.3l, 1.3l, .15l, false, cpp_99misc_39<long double>,
      "x,y,z", "99misc/39", "sin(x+cos(y*1.5))-cos(x+sin(y*1.5))+z*z*z*sin(z*z*z-x*x-y*y)-    cos(y*1.5)*sin(x+cos(y*1.5))+x*y*z+x*y*2.5+x*y*z*cos(x)+x*y*cos(x)+x*z*cos(x)+  y*z*2.5+(x*y*z*cos(x)-x*y*z-y*cos(x)-x*z*y+x*y+x*z-cos(x)*x)" },
    { 3, -1.3l,1.3l,.075l, false, cpp_99misc_40<long double>,
      "x,y,z", "99misc/40", "(x+x+x+x+x+x+x+x+x+x+x+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z)*    (x+x+x+x+x+x+x+x+x+x+x+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z)+    2*(x+x+x+x+x+x+x+x+x+x+x+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z)-    x*y*(x+x+x+x+x+x+x+x+x+x+x+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z)+  x*(x+x+x+x+x+x+x+x+x+x+x+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z)" },
    { 3, -2.0l, 2.0l, .15l, false, cpp_99misc_41<long double>,
      "x,y,z", "99misc/41", "x*3+x*y+x*z+x*sin(y*z) -    (sin(x)+cos(y))*4 +    (sin(x)+cos(y))*x +    (sin(x)+cos(y))*y +  (sin(x)+cos(y))*z" },
    { 1, -100.0l,100.0l,.03l, false, cpp_99misc_43<long double>,
      "x", "99misc/43", "log(x*x)+abs(exp(abs(x)+1))" },
    { 1, 0.0l,100.0l,.025l, false, cpp_99misc_44<long double>,
      "x", "99misc/44", "(x^2)^(1/8) +    1.1*(x^3)^(1/7) +    1.2*(x^4)^(1/6) +    1.3*(x^5)^(1/5) +    1.4*(x^6)^(1/6) +    1.5*(x^7)^(1/4) +    1.6*(x^8)^(1/3) +    1.7*(x^9)^(1/2) +  1.8*(sqrt(abs(-sqrt(x))^3))" },
    { 1, -10.0l,10.0l,.025l, false, cpp_99misc_45<long double>,
      "x", "99misc/45", "(x^2)^(1/7) + 1.1*(x^4)^(1/5) + 1.2*(x^6)^(1/3)" },
    { 2, -.9l, .9l, .015l, false, cpp_99misc_46<long double>,
      "x,y", "99misc/46", "abs(floor(acos(x)+4)) + 1.1*abs(floor(acos(y)+1.5)) +  (acos(x) < (acos(y)-10)) + 1.2*max(-4, acos(x)) + 1.3*min(9, acos(x)-9)" },
    { 2, -3.0l, 3.0l, .1l, false, cpp_99misc_47<long double>,
      "x,y", "99misc/47", "1.1*(exp(x)+exp(-x)) + 1.2*(exp(y)-exp(-y)) +  1.3*((exp(-x)+exp(x))/2) + 1.4*((exp(-x)-exp(x))/2) + 1.5*(cosh(y)+sinh(y))" },
    { 1, 2.0l, 1e9l, 1.2e7l, false, cpp_99misc_48<long double>,
      "x", "99misc/48", "sinh((log(x)/5+1)*5) + 1.2*cosh((log(x)/log(2)+1)*log(2)) + !(x | !(x/4))" },
    { 1, -100.0l, 100.0l, .03l, false, cpp_99misc_49<long double>,
      "x", "99misc/49", "atan2(0, x) + (-4*(x-100))^3.3" },
    { 2, -10.0l, 10.0l, 1.0l, false, cpp_99misc_50<long double>,
      "x,y", "99misc/50", "(x<y | y<x) +     2*(x<y & y<x) +     4*(x<=y & y<=x) +     8*(x<y & x!=y) +     16*(x<y | x!=y) +     32*(x<=y & x>=y) +     64*(x<=y | x>=y) +     128*(x!=y & x=y) +     256*(x!=y & x!=y) +  512*(x<=y & x=y)" },
    { 1, -10.0l, 10.0l, .5l, false, cpp_99misc_52<long double>,
      "x", "99misc/52", "x + (1.0+2.0+3.0+4.0-5.0-6.0-7.0-8.0)/3.0 +    4.0*(1.0+sin(2.0)+cos(4.0*5.0+6.0)/2.0) + cos(0.5)*tan(0.6+0.2) -  1.1/log(2.1)*sqrt(3.3) + 2^3" },
    { 2, 0.0l, 10.0l, 0.5l, false, cpp_99misc_53<long double>,
      "x,y", "99misc/53", "(x&y) + 4*(int(x/10)|int(y/10)) + 8*((-!-!-x)+(!-!-!y)) + 16*(-------x + !!!!!!!y)" },
    { 2, -10.0l, 100.0l, .5l, false, cpp_99misc_54<long double>,
      "x,y", "99misc/54", "(x<y)+(x<=y)+(x>y)+(x>=y)+    (x=y)+(x!=y)+(x&y)+(x|y)+    (!x)+(!!x)+    !((x<y)&(x<3))+  !!(!(x>y)|(x>3))" },
    { 2, 1.0l,100.0l, .5l, false, cpp_99misc_55<long double>,
      "x,y", "99misc/55", "(x^1.2 < 0) + (y^2.5 < 0) + 2*(x*x<0) + 3*(y^3<0) + 4*(x^4<0)" },
    { 1, .25l, 100.0l, .25l, false, cpp_99misc_56<long double>,
      "x", "99misc/56", "1.75e21%x" },
    { 2, -11.0l, 11.0l, 1.0l, false, cpp_99misc_58<long double>,
      "x,y", "99misc/58", "(-x < 3) + (x*-1 > 5) + (x*-3 < 10) + (x*-3 < y*7) +  (x*4 < y*7) + (x*6 < y*-3) + (-x < 11) + (5 < -y)" },
    { 2, -2.0l, 2.0l, 1.0l, false, cpp_99misc_60<long double>,
      "x,y", "99misc/60", "sqr(x) | sub(x,y) | value()" },
    TestType<long double>() };
#endif /*FP_SUPPORT_LONG_DOUBLE_TYPE */
