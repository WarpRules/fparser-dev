/***************************************************************************\
|* Function Parser for C++ v3.3.2                                          *|
|*-------------------------------------------------------------------------*|
|* Copyright: Juha Nieminen, Joel Yliluoma                                 *|
\***************************************************************************/

// NOTE:
// This file contains only internal types for the function parser library.
// You don't need to include this file in your code. Include "fparser.hh"
// only.

#ifndef ONCE_FPARSER_AUX_H_
#define ONCE_FPARSER_AUX_H_

#include <cmath>
#include <cstring>

#ifdef ONCE_FPARSER_H_
namespace FUNCTIONPARSERTYPES
{
    /* This function generated with make_function_name_parser.cc */
    inline const FuncDefinition* findFunction(const NamePtr& functionName)
    {
        switch(functionName.nameLength)
        {
             case 2:
    /* prefix  */if('i' == functionName.name[0]
    && 'f' == functionName.name[1]) return Functions+cIf;/*if*/
    return 0;
             case 3:
    /* prefix  */switch(functionName.name[0]) {
    case 'a':
    /* prefix a */if('b' == functionName.name[1]
    && 's' == functionName.name[2]) return Functions+cAbs;/*abs*/
    return 0;
    case 'c':
    /* prefix c */switch(functionName.name[1]) {
    case 'o':
    /* prefix co */switch(functionName.name[2]) {
    case 's':
    /* prefix cos */return Functions+cCos;/*cos*/
    case 't':
    /* prefix cot */return Functions+cCot;/*cot*/
    default: return 0; }case 's':
    /* prefix cs */if('c' == functionName.name[2]) return Functions+cCsc;/*csc*/
    return 0;
    default: return 0; }case 'e':
    /* prefix e */if('x' == functionName.name[1]
    && 'p' == functionName.name[2]) return Functions+cExp;/*exp*/
    return 0;
    case 'i':
    /* prefix i */if('n' == functionName.name[1]
    && 't' == functionName.name[2]) return Functions+cInt;/*int*/
    return 0;
    case 'l':
    /* prefix l */if('o' == functionName.name[1]
    && 'g' == functionName.name[2]) return Functions+cLog;/*log*/
    return 0;
    case 'm':
    /* prefix m */switch(functionName.name[1]) {
    case 'a':
    /* prefix ma */if('x' == functionName.name[2]) return Functions+cMax;/*max*/
    return 0;
    case 'i':
    /* prefix mi */if('n' == functionName.name[2]) return Functions+cMin;/*min*/
    return 0;
    default: return 0; }case 'p':
    /* prefix p */if('o' == functionName.name[1]
    && 'w' == functionName.name[2]) return Functions+cPow;/*pow*/
    return 0;
    case 's':
    /* prefix s */switch(functionName.name[1]) {
    case 'e':
    /* prefix se */if('c' == functionName.name[2]) return Functions+cSec;/*sec*/
    return 0;
    case 'i':
    /* prefix si */if('n' == functionName.name[2]) return Functions+cSin;/*sin*/
    return 0;
    default: return 0; }case 't':
    /* prefix t */if('a' == functionName.name[1]
    && 'n' == functionName.name[2]) return Functions+cTan;/*tan*/
    return 0;
    default: return 0; }
             case 4:
    /* prefix  */switch(functionName.name[0]) {
    case 'a':
    /* prefix a */switch(functionName.name[1]) {
    case 'c':
    /* prefix ac */if('o' == functionName.name[2]
    && 's' == functionName.name[3]) return Functions+cAcos;/*acos*/
    return 0;
    case 's':
    /* prefix as */if('i' == functionName.name[2]
    && 'n' == functionName.name[3]) return Functions+cAsin;/*asin*/
    return 0;
    case 't':
    /* prefix at */if('a' == functionName.name[2]
    && 'n' == functionName.name[3]) return Functions+cAtan;/*atan*/
    return 0;
    default: return 0; }case 'c':
    /* prefix c */switch(functionName.name[1]) {
    case 'b':
    /* prefix cb */if('r' == functionName.name[2]
    && 't' == functionName.name[3]) return Functions+cCbrt;/*cbrt*/
    return 0;
    case 'e':
    /* prefix ce */if('i' == functionName.name[2]
    && 'l' == functionName.name[3]) return Functions+cCeil;/*ceil*/
    return 0;
    case 'o':
    /* prefix co */if('s' == functionName.name[2]
    && 'h' == functionName.name[3]) return Functions+cCosh;/*cosh*/
    return 0;
    default: return 0; }case 'e':
    /* prefix e */switch(functionName.name[1]) {
    case 'v':
    /* prefix ev */if('a' == functionName.name[2]
    && 'l' == functionName.name[3]) return Functions+cEval;/*eval*/
    return 0;
    case 'x':
    /* prefix ex */if('p' == functionName.name[2]
    && '2' == functionName.name[3]) return Functions+cExp2;/*exp2*/
    return 0;
    default: return 0; }case 'l':
    /* prefix l */{static const char tmp[3] = {'o','g','2'};
    if(std::memcmp(functionName.name+1, tmp, 3) == 0) return Functions+cLog2;/*log2*/
    return 0; }
    case 's':
    /* prefix s */switch(functionName.name[1]) {
    case 'i':
    /* prefix si */if('n' == functionName.name[2]
    && 'h' == functionName.name[3]) return Functions+cSinh;/*sinh*/
    return 0;
    case 'q':
    /* prefix sq */if('r' == functionName.name[2]
    && 't' == functionName.name[3]) return Functions+cSqrt;/*sqrt*/
    return 0;
    default: return 0; }case 't':
    /* prefix t */{static const char tmp[3] = {'a','n','h'};
    if(std::memcmp(functionName.name+1, tmp, 3) == 0) return Functions+cTanh;/*tanh*/
    return 0; }
    default: return 0; }
             case 5:
    /* prefix  */switch(functionName.name[0]) {
    case 'a':
    /* prefix a */switch(functionName.name[1]) {
    case 'c':
    /* prefix ac */{static const char tmp[3] = {'o','s','h'};
    if(std::memcmp(functionName.name+2, tmp, 3) == 0) return Functions+cAcosh;/*acosh*/
    return 0; }
    case 's':
    /* prefix as */{static const char tmp[3] = {'i','n','h'};
    if(std::memcmp(functionName.name+2, tmp, 3) == 0) return Functions+cAsinh;/*asinh*/
    return 0; }
    case 't':
    /* prefix at */if('a' == functionName.name[2]) {
    /* prefix ata */if('n' == functionName.name[3]) {
    /* prefix atan */switch(functionName.name[4]) {
    case '2':
    /* prefix atan2 */return Functions+cAtan2;/*atan2*/
    case 'h':
    /* prefix atanh */return Functions+cAtanh;/*atanh*/
    default: return 0; }}return 0;}return 0;default: return 0; }case 'f':
    /* prefix f */{static const char tmp[4] = {'l','o','o','r'};
    if(std::memcmp(functionName.name+1, tmp, 4) == 0) return Functions+cFloor;/*floor*/
    return 0; }
    case 'l':
    /* prefix l */{static const char tmp[4] = {'o','g','1','0'};
    if(std::memcmp(functionName.name+1, tmp, 4) == 0) return Functions+cLog10;/*log10*/
    return 0; }
    case 't':
    /* prefix t */{static const char tmp[4] = {'r','u','n','c'};
    if(std::memcmp(functionName.name+1, tmp, 4) == 0) return Functions+cTrunc;/*trunc*/
    return 0; }
    default: return 0; }
            default: break;
        }
        return 0;
    }

//==========================================================================
// Math funcs
//==========================================================================
// -------------------------------------------------------------------------
// double
// -------------------------------------------------------------------------
    inline double fp_abs(double x) { return fabs(x); }
    inline double fp_acos(double x) { return acos(x); }
    inline double fp_asin(double x) { return asin(x); }
    inline double fp_atan(double x) { return atan(x); }
    inline double fp_atan2(double x, double y) { return atan2(x, y); }
#ifdef FP_SUPPORT_CBRT
    inline double fp_cbrt(double x) { return cbrt(x); }
#else
    inline double fp_cbrt(double x) { return x<0 ? -exp(log(-x)/3.0)
                                                 :  exp(log( x)/3.0); }
#endif
    inline double fp_ceil(double x) { return ceil(x); }
    inline double fp_cos(double x) { return cos(x); }
    inline double fp_cosh(double x) { return cosh(x); }
    inline double fp_exp(double x) { return exp(x); }
    inline double fp_floor(double x) { return floor(x); }
    inline double fp_int(double x) { return floor(x + .5); }
    inline double fp_log(double x) { return log(x); }
    inline double fp_log10(double x) { return log(x) * 0.43429448190325176116; }
    inline double fp_mod(double x, double y) { return fmod(x, y); }
    inline double fp_sin(double x) { return sin(x); }
    inline double fp_sinh(double x) { return sinh(x); }
    inline double fp_sqrt(double x) { return sqrt(x); }
    inline double fp_tan(double x) { return tan(x); }
    inline double fp_tanh(double x) { return tanh(x); }

#ifndef FP_SUPPORT_ASINH
    inline double fp_asinh(double x) { return log(x + sqrt(x*x + 1.0)); }
    inline double fp_acosh(double x) { return log(x + sqrt(x*x - 1.0)); }
    inline double fp_atanh(double x) { return log((1.0+x) / (1.0-x)) * 0.5; }
#else
    inline double fp_asinh(double x) { return asinh(x); }
    inline double fp_acosh(double x) { return acosh(x); }
    inline double fp_atanh(double x) { return atanh(x); }
#endif // FP_SUPPORT_ASINH

    inline double fp_trunc(double x) { return x<0.0 ? ceil(x) : floor(x); }

    /* fp_pow() is a wrapper for std::pow()
     * that produces an identical value for
     * exp(1) ^ 2.0  (0x4000000000000000)
     * as exp(2.0)   (0x4000000000000000)
     * - std::pow() on x86_64
     * produces 2.0  (0x3FFFFFFFFFFFFFFF) instead!
     */
    inline double fp_pow(double x,double y)
    {
        //if(x == 1.0) return 1.0;
        if(x > 0.0) return exp(log(x) * y);
        if(y == 0.0) return 1.0;
        if(y < 0.0) return 1.0 / fp_pow(x, -y);
        return pow(x, y);
    }

#ifndef FP_SUPPORT_LOG2
    inline double fp_log2(double x) { return log(x) * 1.4426950408889634074; }
#else
    inline double fp_log2(double x) { return log2(x); }
#endif // FP_SUPPORT_LOG2

    inline double fp_exp2(double x) { return fp_pow(2.0, x); }

#ifdef FP_EPSILON
    template<typename Value_t>
    inline Value_t fp_epsilon() { return FP_EPSILON; }
#else
    template<typename Value_t>
    inline Value_t fp_epsilon() { return 0.0; }
#endif

#ifdef FP_EPSILON
    inline bool FloatEqual(double a, double b)
    { return fabs(a - b) <= fp_epsilon<double>(); }
#else
    inline bool FloatEqual(double a, double b)
    { return a == b; }
#endif // FP_EPSILON

    inline bool IsIntegerConst(double a)
    { return FloatEqual(a, (double)(long)a); }



// -------------------------------------------------------------------------
// float
// -------------------------------------------------------------------------
#ifdef FP_SUPPORT_FLOAT_TYPE
    inline float fp_abs(float x) { return fabsf(x); }
    inline float fp_acos(float x) { return acosf(x); }
    inline float fp_asin(float x) { return asinf(x); }
    inline float fp_atan(float x) { return atanf(x); }
    inline float fp_atan2(float x, float y) { return atan2f(x, y); }
#ifdef FP_SUPPORT_CBRT
    inline float fp_cbrt(float x) { return cbrtf(x); }
#else
    inline float fp_cbrt(float x) { return x<0 ? -expf(logf(-x)/3.0F)
                                               :  expf(logf( x)/3.0F); }
#endif
    inline float fp_ceil(float x) { return ceilf(x); }
    inline float fp_cos(float x) { return cosf(x); }
    inline float fp_cosh(float x) { return coshf(x); }
    inline float fp_exp(float x) { return expf(x); }
    inline float fp_floor(float x) { return floorf(x); }
    inline float fp_int(float x) { return floorf(x + .5F); }
    inline float fp_log(float x) { return logf(x); }
    inline float fp_log10(float x) { return logf(x) * 0.43429448190325176116F; }
    inline float fp_mod(float x, float y) { return fmodf(x, y); }
    inline float fp_sin(float x) { return sinf(x); }
    inline float fp_sinh(float x) { return sinhf(x); }
    inline float fp_sqrt(float x) { return sqrtf(x); }
    inline float fp_tan(float x) { return tanf(x); }
    inline float fp_tanh(float x) { return tanhf(x); }

#ifndef FP_SUPPORT_ASINH
    inline float fp_asinh(float x) { return logf(x + sqrt(x*x + 1.0F)); }
    inline float fp_acosh(float x) { return logf(x + sqrt(x*x - 1.0F)); }
    inline float fp_atanh(float x) { return logf((1.0F+x) / (1.0F-x)) * 0.5F; }
#else
    inline float fp_asinh(float x) { return asinhf(x); }
    inline float fp_acosh(float x) { return acoshf(x); }
    inline float fp_atanh(float x) { return atanhf(x); }
#endif // FP_SUPPORT_ASINH

    inline float fp_trunc(float x) { return x<0.0F ? ceilf(x) : floorf(x); }

    inline float fp_pow(float x,float y)
    {
        //if(x == 1.0) return 1.0;
        if(x > 0.0F) return expf(logf(x) * y);
        if(y == 0.0F) return 1.0F;
        if(y < 0.0F) return 1.0F / fp_pow(x, -y);
        return powf(x, y);
    }

#ifndef FP_SUPPORT_LOG2
    inline float fp_log2(float x) { return logf(x) * 1.4426950408889634074F; }
#else
    inline float fp_log2(float x) { return log2f(x); }
#endif // FP_SUPPORT_LOG2

    inline float fp_exp2(float x) { return fp_pow(2.0F, x); }

#ifdef FP_EPSILON
    template<>
    inline float fp_epsilon<float>() { return 1e-8F; }
#else
    template<>
    inline float fp_epsilon<float>() { return 0.0F; }
#endif

#ifdef FP_EPSILON
    inline bool FloatEqual(float a, float b)
    { return fabsf(a - b) <= fp_epsilon<float>(); }
#else
    inline bool FloatEqual(float a, float b)
    { return a == b; }
#endif // FP_EPSILON

    inline bool IsIntegerConst(float a)
    { return FloatEqual(a, (float)(long)a); }
#endif // FP_SUPPORT_FLOAT_TYPE



// -------------------------------------------------------------------------
// long double
// -------------------------------------------------------------------------
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
    inline long double fp_abs(long double x) { return fabsl(x); }
    inline long double fp_acos(long double x) { return acosl(x); }
    inline long double fp_asin(long double x) { return asinl(x); }
    inline long double fp_atan(long double x) { return atanl(x); }
    inline long double fp_atan2(long double x, long double y) { return atan2l(x, y); }
#ifdef FP_SUPPORT_CBRT
    inline long double fp_cbrt(long double x) { return cbrtl(x); }
#else
    inline long double fp_cbrt(long double x) { return x<0 ? -expl(logl(-x)/3.0L)
                                                           :  expl(logl( x)/3.0L); }
#endif
    inline long double fp_ceil(long double x) { return ceill(x); }
    inline long double fp_cos(long double x) { return cosl(x); }
    inline long double fp_cosh(long double x) { return coshl(x); }
    inline long double fp_exp(long double x) { return expl(x); }
    inline long double fp_floor(long double x) { return floorl(x); }
    inline long double fp_int(long double x) { return floorl(x + .5L); }
    inline long double fp_log(long double x) { return logl(x); }
    inline long double fp_log10(long double x) { return logl(x) * 0.43429448190325176116L; }
    inline long double fp_mod(long double x, long double y) { return fmodl(x, y); }
    inline long double fp_sin(long double x) { return sinl(x); }
    inline long double fp_sinh(long double x) { return sinhl(x); }
    inline long double fp_sqrt(long double x) { return sqrtl(x); }
    inline long double fp_tan(long double x) { return tanl(x); }
    inline long double fp_tanh(long double x) { return tanhl(x); }

#ifndef FP_SUPPORT_ASINH
    inline long double fp_asinh(long double x) { return logl(x + sqrt(x*x + 1.0L)); }
    inline long double fp_acosh(long double x) { return logl(x + sqrt(x*x - 1.0L)); }
    inline long double fp_atanh(long double x) { return logl((1.0L+x) / (1.0L-x)) * 0.5L; }
#else
    inline long double fp_asinh(long double x) { return asinhl(x); }
    inline long double fp_acosh(long double x) { return acoshl(x); }
    inline long double fp_atanh(long double x) { return atanhl(x); }
#endif // FP_SUPPORT_ASINH

    inline long double fp_trunc(long double x) { return x<0.0L ? ceill(x) : floorl(x); }

    inline long double fp_pow(long double x,long double y)
    {
        //if(x == 1.0) return 1.0;
        if(x > 0.0L) return expl(logl(x) * y);
        if(y == 0.0L) return 1.0L;
        if(y < 0.0L) return 1.0L / fp_pow(x, -y);
        return powl(x, y);
    }

#ifndef FP_SUPPORT_LOG2
    inline long double fp_log2(long double x) { return log(x) * 1.4426950408889634074L; }
#else
    inline long double fp_log2(long double x) { return log2l(x); }
#endif // FP_SUPPORT_LOG2

    inline long double fp_exp2(long double x) { return fp_pow(2.0L, x); }

#ifdef FP_EPSILON
    inline bool FloatEqual(long double a, long double b)
    { return fabsl(a - b) <= fp_epsilon<double>(); }
#else
    inline bool FloatEqual(long double a, long double b)
    { return a == b; }
#endif // FP_EPSILON

    inline bool IsIntegerConst(long double a)
    { return FloatEqual(a, (long double)(long)a); }
#endif // FP_SUPPORT_LONG_DOUBLE_TYPE
}

#endif // ONCE_FPARSER_H_

#ifdef FP_SUPPORT_FLOAT_TYPE
#define FUNCTIONPARSER_INSTANTIATE_FLOAT \
    template class FunctionParserBase<float>;
#else
#define FUNCTIONPARSER_INSTANTIATE_FLOAT
#endif

#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
#define FUNCTIONPARSER_INSTANTIATE_LONG_DOUBLE \
    template class FunctionParserBase<long double>;
#else
#define FUNCTIONPARSER_INSTANTIATE_LONG_DOUBLE
#endif

/* Add 'FUNCTIONPARSER_INSTANTIATE_TYPES' at the end of all .cc files
   containing FunctionParserBase implementations.
 */
#define FUNCTIONPARSER_INSTANTIATE_TYPES \
    template class FunctionParserBase<double>; \
    FUNCTIONPARSER_INSTANTIATE_FLOAT \
    FUNCTIONPARSER_INSTANTIATE_LONG_DOUBLE

#endif
