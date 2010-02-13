/*==========================================================================
  testbed
  ---------
  Copyright: Juha Nieminen, Joel Yliluoma
  This program (testbed) is distributed under the terms of
  the GNU General Public License (GPL) version 3.
  See gpl.txt for the license text.
============================================================================*/

#include "fpconfig.hh"
#include "fparser.hh"

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
#include "fparser_mpfr.hh"
#endif
#ifdef FP_SUPPORT_GMP_INT_TYPE
#include "fparser_gmpint.hh"
#endif

#include <cmath>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <sstream>
#include <algorithm>
#include <cstring>

#define Epsilon   (1e-9)
#define Epsilon_f (1e-1)

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define CONST 1.5

#define StringifyHlp(x) #x
#define Stringify(x) StringifyHlp(x)

#define FIRST_TEST 0
//#define FIRST_TEST 29

namespace
{
    const bool verbose = false;

    // Auxiliary functions
    // -------------------
#ifndef _MSC_VER /* workaround for compiler bug? MSC 15.0 */
    inline double abs(double d) { return fabs(d); }
#endif
    template<typename T>
    inline T min(T x, T y) { return x<y ? x : y; }
    template<typename T>
    inline T max(T x, T y) { return x>y ? x : y; }
    inline long max(long x, long y) { return x>y ? x : y; }
    inline double r2d(double x) { return x*180.0/M_PI; }
    inline double d2r(double x) { return x*M_PI/180.0; }
#ifndef __ICC /* workaround for compiler bug? (ICC 11.0) */
    inline double cot(double x) { return 1.0 / std::tan(x); }
#endif
    inline double csc(double x) { return 1.0 / std::sin(x); }
    inline double sec(double x) { return 1.0 / std::cos(x); }
    //inline double log10(double x) { return std::log(x) / std::log(10); }

    inline bool fBool(double x) { return fabs(x) >= 0.5; }
    inline double fAnd(double x, double y)
    { return double(fBool(x) && fBool(y)); }
    inline double fOr(double x, double y)
    { return double(fBool(x) || fBool(y)); }
    inline double fNot(double x) { return double(!fBool(x)); }

#ifndef FP_SUPPORT_ASINH
    inline double fp_asinh(double x) { return log(x + sqrt(x*x + 1)); }
    inline double fp_acosh(double x) { return log(x + sqrt(x*x - 1)); }
    inline double fp_atanh(double x) { return log( (1+x) / (1-x) ) * 0.5; }
#else
    inline double fp_asinh(double x) { return asinh(x); }
    inline double fp_acosh(double x) { return acosh(x); }
    inline double fp_atanh(double x) { return atanh(x); }
#endif // FP_SUPPORT_ASINH
    inline double fp_trunc(double x) { return x<0.0 ? ceil(x) : floor(x); }

    inline int doubleToInt(double d)
    {
        return d<0 ? -int((-d)+.5) : int(d+.5);
    }

    double Sqr(const double* p) { return p[0]*p[0]; }
    double Sub(const double* p) { return p[0]-p[1]; }
    double Value(const double*) { return 10; }

#ifdef FP_SUPPORT_FLOAT_TYPE
    float Sqr_f(const float* p) { return p[0]*p[0]; }
    float Sub_f(const float* p) { return p[0]-p[1]; }
    float Value_f(const float*) { return 10; }
#endif

#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
    long double Sqr_ld(const long double* p) { return p[0]*p[0]; }
    long double Sub_ld(const long double* p) { return p[0]-p[1]; }
    long double Value_ld(const long double*) { return 10; }
#endif

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    MpfrFloat Sqr_mpfr(const MpfrFloat* p) { return p[0]*p[0]; }
    MpfrFloat Sub_mpfr(const MpfrFloat* p) { return p[0]-p[1]; }
    MpfrFloat Value_mpfr(const MpfrFloat*) { return 10; }
#endif
}


//============================================================================
// Test function definitions
//============================================================================
struct FloatingPointTest
{
    const char* funcString;
    const char* paramString;
    double (*funcPtr)(const double*);
    unsigned paramAmount;
    double paramMin, paramMax, paramStep;
    bool useDegrees;
    const char* testname;
};

struct IntTest
{
    const char* funcString;
    const char* paramString;
    long (*funcPtr)(const long*);
    unsigned paramAmount;
    long paramMin, paramMax, paramStep;
    bool useDegrees;
    const char* testname;
};

double f1(const double* p)
{
#define P1 "x*4/2 + (1+(2+3)) + x*x+x+1+2+3*4+5*6*\n7-8*9", "x", \
        f1, 1, -1000, 1000, .1, false
    const double x = p[0];
    return x*4/2 + (1+(2+3)) + x*x+x+(1.0+2.0+3.0*4.0+5.0*6.0*7.0-8.0*9.0);
/*
    const double x = p[0], y = p[1], z = p[2];
#define P1 "x - (y*(y*(y*-1))*1)", "x,y,z", f1, 3, .1, 4, .1, false
    return x - (y*(y*(y*-1))*1);
*/
}
double f2(const double* p)
{
#define P2 " 2 * x+ sin ( x ) / .5 + 2-sin(x)*sin(x)", "x", \
        f2, 1, -1000, 1000, .1, false
    const double x = p[0];
    return 2*x+sin(x)/.5 + 2-sin(x)*sin(x);
}
double f3(const double* p)
{
#define P3 "(x=y & y=x)+  1+2-3.1*4e2/.5 + x*x+y*y+z*z", "x,y,z", \
        f3, 3, -10, 10, .5, false
    const double x = p[0], y = p[1], z = p[2];
    return (x==y && y==x)+ 1.0+2.0-3.1*4e2/.5 + x*x+y*y+z*z;
}
double f4(const double* p)
{
#define P4 \
    " ( ((( ( x-y) -( ((y) *2) -3)) )* 4))+sin(x)*cos(y)-cos(x)*sin(y) ", \
        "x,y", f4, 2, -100, 100, .5, false
    const double x = p[0], y = p[1];
    return ( ((( ( x-y) -( ((y) *2) -3)) )* 4))+sin(x)*cos(y)-cos(x)*sin(y);
}
double f5(const double* p)
{
#define P5 "__A5_x08^o__5_0AB_", "__A5_x08,o__5_0AB_", \
        f5, 2, .1, 10, .05, false
    const double x = p[0], y = p[1];
    return pow(x,y);
}
#ifndef FP_DISABLE_EVAL
double f6(const double* p)
{
#define P6 "if(x>0&y>0,x*y+eval(x-1,y-1),0)+1", "x,y", \
        f6, 2, .1, 10, .2, false
    const double x = p[0], y = p[1];
    const double v[2] = { x-1, y-1 };
    return (x>1e-14 && y>1e-14 ? x*y+f6(v) : 0)+1;
}
#endif
double f7(const double* p)
{
#define P7 "cos(x)*sin(1-x)*(1-cos(x/2)*sin(x*5))", "x", \
        f7, 1, -10, 10, .001, false
    const double x = p[0];
    return cos(x)*sin(1-x)*(1-cos(x/2)*sin(x*5));
}
double f8(const double* p)
{
#define P8 "atan2(x,y)+max(x,y)", "x,y", f8, 2, -10, 10,.05, false
    const double x = p[0], y = p[1];
    return atan2(x,y) + (x>y ? x : y);
}
double f9(const double* p)
{
#define P9 "1.5+x*y-2+4/8+z+z+z+z+x/(y*z)", "x,y,z", f9, 3, 1, 21, .3, false
    const double x = p[0], y = p[1], z = p[2];
    return 1.5+x*y-2.0+4.0/8.0+z+z+z+z+x/(y*z);
}
double f10(const double* p)
{
#define P10 "1+sin(cos(max(1+2+3+4+5, x+y+z)))+2", "x,y,z", \
        f10, 3, 1, 21, .3, false
    const double x = p[0], y = p[1], z = p[2];
    return 1.0+sin(cos(max(1.0+2.0+3.0+4.0+5.0, x+y+z)))+2.0;
}
double f11(const double* p)
{
#define P11 "-(-(-(-(-x))-x))+y*1+log(1.1^z)", "x,y,z", \
        f11, 3, 1, 21, .25, false
    const double x = p[0], y = p[1], z = p[2];
    return -(-(-(-(-x))-x))+y*1+log(pow(1.1,z));
}
double f12(const double* p)
{
#define P12 "1/log(10^((3-2)/log(x)))", "x", f12, 1, 1, 2000, .05, false
    const double x = p[0];
    return 1.0/log(pow(10.0, 1.0/log(x)));
}
double f13(const double* p)
{
#define P13 "x^3 * x^4 + y^3 * y^5", "x,y", f13, 2, -50, 50, .5, false
    const double x = p[0], y = p[1];
    return pow(x,3) * pow(x,4) + pow(y,3) * pow(y,5);
}
double f14(const double* p)
{
#define P14 "x*pi + sin(2*pi) + CONST", "x", f14, 1, -50, 50, .01, false
    const double x = p[0];
    return x*M_PI + sin(2*M_PI) + CONST;
}
double f15(const double* p)
{
#define P15 "x^y/log(y) + log(x)/log(y) + log(x^y)", "x,y", \
        f15, 2, 1.1, 8, .02, false
    const double x = p[0], y = p[1];
    return pow(x,y)/log(y) + log(x)/log(y) + log(pow(x,y));
}
double f16(const double* p)
{
#define P16 "if(x<0, if(y<0, x+y, x-y), if(y>0, x*y, x+2*y))", "x,y", \
        f16, 2, -20, 20, .1, false
    const double x = p[0], y = p[1];
    return x<0 ? (y<0 ? x+y : x-y) : (y>0 ? x*y : x+2*y);
}
double f17(const double* p)
{
#define P17 "sqr(x)+sub(x,y)+psqr(y)+psub(y+1,x-2)-1", "x,y", \
        f17, 2, -20, 20, .1, false
    const double x = p[0], y = p[1];
    double p2[] = { y+1, x-2 };
    return Sqr(p)+Sub(p)+Sqr(p+1)+Sub(p2)-1;
}
double f18(const double* p)
{
#define P18 " - ( - ( - ( - 5 ) ) ) * -x^ -y^-2", "x,y", \
        f18, 2, 1, 20, .1, false
    const double x = p[0], y = p[1];
    return - ( - ( - ( - 5 ) ) ) * -pow(x, -pow(y, -2));
}

double f19(const double* p)
{
#define P19 "(x<y)+10*(x<=y)+100*(x>y)+1000*(x>=y)+10000*(x=y)+100000*(x!=y)+\
(x&y)*2+(x|y)*20+(!x)*200+(!!x)*2000+4*!((x<y)&(x<3))+40*!!(!(x>y)|(x>3))", \
        "x,y", f19, 2, -100, 100, .5, false
    const double x = p[0], y = p[1];
    return (x<y)+10*(x<=y)+100*(x>y)+1000*(x>=y)+10000*(x==y)+100000*(x!=y)
        +(x&&y)*2+(x||y)*20+(!x)*200+(!!x)*2000
        +4*!((x<y)&&(x<3))+40*!!(!(x>y)||(x>3));
}

double f20(const double* p)
{
#define P20 "(!(x != y) & !x) + !(!(!(!y)))", "x,y", \
        f20, 2, -100, 100, 1, false
    const double x = p[0], y = p[1];
    return  (!(x != y) && !x) + !(!(!(!y)));
}

double f21(const double* p)
{
#define P21 "sqr(x)+value()-pvalue ( ) ", "x", f21, 1, -10, 10, 1, false
    return Sqr(p)+Value(0)-5;
}

double f22(const double* p)
{
#define P22 "3.5doubled + 10*x tripled - sin(y)doubled + \
100*(x doubled-y tripled)doubled + 5/2doubled + 1.1^x doubled + \
1.1doubled^x doubled", "x,y", \
        f22, 2, -10, 10, .05, false
    const double x = p[0], y = p[1];
    return (3.5*2) + 10*(x*3) - (sin(y)*2) + 100*((x*2)-(y*3))*2 + 5.0/(2*2) +
        pow(1.1, x*2) + pow(1.1*2, x*2);
}

double f23(const double* p)
{
#define P23 "(x/(2*acos(0)))*180", "x", f23, 1, -1000, 1000, .1, false
    return (p[0]/(2*acos(0.0)))*180;
}

double f24(const double* p)
{
#define P24 \
    "(min(x, min(1,x)) + min(x, 1))/2 + min(x, 1)*3 + max(0, min(-2,0))", \
        "x", f24, 1, -1000, 1000, .1, false
    return (std::min(*p, std::min(1.0, *p)) + std::min(*p, 1.0))/2 +
        std::min(*p, 1.0)*3 + std::max(0.0, std::min(-2.0, 0.0));
}

double f25(const double* p)
{
#define P25 "a^b^c + a^-2 * (-b^2) + (-b^-c)", "a,b,c", f25, 3, 1, 3, .1, false
    const double a = p[0], b = p[1], c = p[2];
    return pow(a, pow(b, c)) + pow(a, -2) * (-pow(b, 2)) + (-pow(b, -c));
}

double f26(const double* p)
{
#define P26 "sin(x) + cos(x*1.5) + asin(x/110) + acos(x/120)", "x", \
        f26, 1, -100, 100, .1, true
    const double x = p[0];
    return sin(d2r(x)) + cos(d2r(x*1.5)) +
        r2d(asin(x/110.0)) + r2d(acos(x/120.0));
}

double f27(const double* p)
{
#define P27 "abs(x)+acos(x)+asin(x)+atan(x)+atan2(x,y)+ceil(x)+cos(x)+\
cosh(x)+cot(x)+csc(x) + pow(x,y)", "x,y", \
        f27, 2, .1, .9, .025, false
    const double x = p[0], y = p[1];
    return fabs(x)+acos(x)+asin(x)+atan(x)+atan2(x,y)+ceil(x)+cos(x)+cosh(x)+
        1.0/tan(x)+1.0/sin(x) + pow(x,y);
}

double f28(const double* p)
{
#define P28 "exp(x)+floor(x)+int(x)+log(x)+log10(x)+max(x,y)+min(x,y)+\
sec(x)+sin(x)+sinh(x)+sqrt(x)+tan(x)+tanh(x)+ceil(y)+trunc(y)", "x,y", \
        f28, 2, .1, .9, .025, false
    const double x = p[0], y = p[1];
    return exp(x)+floor(x)+floor(x+.5)+log(x)+log10(x)+std::max(x,y)+
        std::min(x,y)+1.0/cos(x)+sin(x)+sinh(x)+sqrt(x)+tan(x)+tanh(x)+
        ceil(y)+fp_trunc(y);
}

double f29(const double* p)
{
#define P29 "x-y*1", "x,y", f29, 2, -100, 100, .1, false
    return p[0] - p[1]*1;
}

double f30(const double* p)
{
#define P30 "x - y*1 + (x%y) + x / (y^1.1) + 2^3 + 5%3 + x^(y^0) + x^0.5", \
        "x,y", f30, 2, 3, 10, 1, false
    const double x = p[0], y = p[1];
    return x - y*1 + fmod(x,y) + x / pow(y,1.1) + pow(2.0,3) + fmod(5.0,3.0) +
        pow(x,pow(y,0)) + pow(x,0.5);
}

double f31(const double* p)
{
    const double x = p[0], y = p[1], z = p[2];
#define P31 "x - (y*(y*(y*-1))*1) + log(x*exp(1.0)^y) - log(x^y) + \
exp(1.0)^log(x+6) + 10^(log(x+6)/log(y+6)*log(z+6)/log(10)) - \
exp(1.0)^(log(x+6)*y) - 5^(log(x+7)/log(5)) + (x*z+17)^3 * (x*z+17)^2 / \
(x*z+17)^4", "x,y,z", f31, 3, .1, 4, .1, false

    return x - (y*(y*(y*-1))*1) + log(x*pow(exp(1.0),y)) - log(pow(x,y)) +
        pow(exp(1.0),log(x+6)) +
        pow(10.0,log(x+6)/log(y+6)*log(z+6)/log(10.0)) -
        pow(exp(1.0), log(x+6)*y) - pow(5.0,log(x+7)/log(5.0)) +
        pow(x*z+17,3) * pow(x*z+17,2) / pow(x*z+17,4);
}

double f32(const double* p)
{
#define P32code \
    x\
    +y/y-min(3,4)-x-max(4,3)+max(3,4)-min(4,3)+0+(z*1)\
    +(x-2+2)+(x*0.5*2)+y*0\
    +min(min(min(4.0,x),1.0),min(x,min(min(y,4.0),z)))\
    +max(max(max(4.0,x),1.0),max(x,max(max(y,4.0),z)))\
    +(abs(1)+acos(1.0)+asin(1.0)+atan(1.0)+ceil(1.1)+cos(0.0)\
     +cosh(0.0)+floor(1.1)+log(1.0)+sin(0.0)+sinh(0.0)+tan(1.0)\
     +tanh(1.0)+atan2(1.0,1.0))\
    +(x-(y-z))\
    +(x+y) + (x*y)\
    +max(x,max(x,max(x,max(x,x))))*-1.0\
    +(z-z)\
    +1/sin(x/5) + 1/cos(y/5) + 1/tan(z/5)\
    +log10(cot(z/5) + csc(y/5) + sec(x/5))\
    +log(30+x)*log(40+y)/log(50+z)\
    +sin(x/57.295779513082320877)\
    +asin(x/10)*57.295779513082320877\
    +floor(-x) + 1/ceil(x)\
    +sqrt(5 * 0.2)\
    +(-x+-x+-x+-x+-x+-x)
#define P32 Stringify(P32code), "x,y,z", f32, 3, 1, 2, .05, false

    const double x = p[0], y = p[1], z = p[2];
    return P32code;
}

double f33(const double* p)
{
#define P33 "sin(sqrt(10-x*x+y*y))+cos(sqrt(15-x*x-y*y))+sin(x*x+y*y)", \
        "x,y", f33, 2, -2, 2, .1, false
    const double x = p[0], y = p[1];
    return sin(sqrt(10-x*x+y*y))+cos(sqrt(15-x*x-y*y))+sin(x*x+y*y);
}

double f34(const double* p)
{
#define P34 "\343\201\212+\346\227\251*\343\201\206-t", \
        "t,\343\201\206,\343\201\212,\346\227\251", \
        f34, 4, -5, 5, 1, false
    const double t = p[0], z = p[1], x = p[2], y = p[3];
    return x+y*z-t;
}

double f35(const double* p)
{
#define P35 "A_very_long_variable_name_1-A_very_long_variable_name_2+\
Yet_a_third_very_long_variable_name*A_very_long_variable_name_1", \
        "A_very_long_variable_name_1,A_very_long_variable_name_2,\
Yet_a_third_very_long_variable_name", f35, 3, -10, 10, 1, false
    const double x = p[0], y = p[1], z = p[2];
    return x-y+z*x;
}

double f36(const double* p)
{
#define P36 "-if(x<0, x, -x) + -if(x<5, 2, 3)", "x", f36, 1, -10, 10, .1, false
    const double x = p[0];
    return -(x<0 ? x : -x) + -(x<5 ? 2 : 3);
}

double f37(const double* p)
{
#define P37 "5 + 7.5*8 / 3 - 2^4*2 + 7%2+4 + x", "x", \
        f37, 1, -10, 10, .1, false
    const double x = p[0];
    return 5 + 7.5*8 / 3 - pow(2.0,4)*2 + 7%2+4 + x;
}

double f38(const double* p)
{
#define P38 "asinh(x) + 1.5*acosh(y+3) + 2.2*atanh(z)", "x,y,z", \
        f38, 3, -.9, .9, .05, false
    const double x = p[0], y = p[1], z = p[2];
    return fp_asinh(x) + 1.5*fp_acosh(y+3) + 2.2*fp_atanh(z);
}

double f39(const double* p)
{
#define P39Code sin(x+cos(y*1.5))-cos(x+sin(y*1.5))+z*z*z*sin(z*z*z-x*x-y*y)-\
cos(y*1.5)*sin(x+cos(y*1.5))+x*y*z+x*y*2.5+x*y*z*cos(x)+x*y*cos(x)+x*z*cos(x)+\
y*z*2.5+(x*y*z*cos(x)-x*y*z-y*cos(x)-x*z*y+x*y+x*z-cos(x)*x)
#define P39 Stringify(P39Code), "x,y,z", f39, 3, -2, 2, .08, false
    const double x = p[0], y = p[1], z = p[2];
    return P39Code;
}

double f40(const double* p)
{
#define P40CodePart x+x+x+x+x+x+x+x+x+x+x+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z+y+z
#define P40Code (P40CodePart)*(P40CodePart)+2*(P40CodePart)-x*y*(P40CodePart)+\
x*(P40CodePart)
#define P40 Stringify(P40Code), "x,y,z", f40, 3, -2, 2, .075, false
    const double x = p[0], y = p[1], z = p[2];
    return P40Code;
}

double f41(const double* p)
{
#define P41CodePart (sin(x)+cos(y))
#define P41Code x*3+x*y+x*z+x*sin(y*z) - \
P41CodePart*4+P41CodePart*x+P41CodePart*y+P41CodePart*z
#define P41 Stringify(P41Code), "x,y,z", f41, 3, -2, 2, .075, false
    const double x = p[0], y = p[1], z = p[2];
    return P41Code;
}

double f42(const double* p)
{
#define P42 "sqrt(x*x) + 1.5*((y*y)^.25) + hypot(x,y)" , "x,y", f42, 2, -10, 10, .025, false
    const double x = p[0], y = p[1];
    const double xx = x*x, yy = y*y; // to avoid gcc bug with -ffast-math
    return sqrt(xx) + 1.5*(pow(yy, .25)) + sqrt(xx + yy);
}

double f43(const double* p)
{
#define P43 "log(x*x)+abs(exp(abs(x)+1))" , "x", f43, 1, -100, 100, .03, false
    const double x = p[0];
    const double xx = x*x;
    return log(xx)+abs(exp(abs(x)+1));
}

double f44(const double* p)
{
#define P44 "(x^2)^(1/8) + 1.1*(x^3)^(1/7) + 1.2*(x^4)^(1/6) + \
1.3*(x^5)^(1/5) + 1.4*(x^6)^(1/6) + 1.5*(x^7)^(1/4) + 1.6*(x^8)^(1/3) + \
1.7*(x^9)^(1/2) + 1.8*(sqrt(abs(-sqrt(x))^3))" , "x", \
        f44, 1, 0, 100, .025, false
    const double x = p[0];
    const double x2 = x*x, x3 = x*x*x;
    const double x4 = x2*x2, x5 = x3*x2, x6 = x3*x3;
    const double x7 = x6*x, x8 = x6*x2, x9 = x6*x3;
    return pow(x2, 1.0/8.0) +
        1.1 * pow(x3, 1.0/7.0) +
        1.2 * pow(x4, 1.0/6.0) +
        1.3 * pow(x5, 1.0/5.0) +
        1.4 * pow(x6, 1.0/6.0) +
        1.5 * pow(x7, 1.0/4.0) +
        1.6 * pow(x8, 1.0/3.0) +
        1.7 * pow(x9, 1.0/2.0) +
        1.8 * sqrt(pow(abs(-sqrt(x)), 3));
}

double f45(const double* p)
{
#define P45 "(x^2)^(1/7) + 1.1*(x^4)^(1/5) + 1.2*(x^6)^(1/3)" , "x", \
        f45, 1, -10, 10, .025, false
    const double x = p[0];
    const double x2 = x*x;
    const double x4 = x2*x2;
    const double x6 = x4*x2;
    return pow(x2, 1.0/7.0) +
        1.1*pow(x4, 1.0/5.0) +
        1.2*pow(x6, 1.0/3.0);
}

double f46(const double* p)
{
#define P46 "abs(floor(acos(x)+4)) + 1.1*abs(floor(acos(y)+1.5)) + \
(acos(x) < (acos(y)-10)) + 1.2*max(-4, acos(x)) + 1.3*min(9, acos(x)-9)" , \
        "x,y", f46, 2, -.9, .9, .015, false
    const double x = p[0], y = p[1];
    return abs(floor(acos(x)+4)) + 1.1*abs(floor(acos(y)+1.5)) +
        (acos(x) < (acos(y)-10)) + 1.2*max(-4.0, acos(x)) +
        1.3*min(9.0, acos(x)-9);
}

double f47(const double* p)
{
#define P47 "1.1*(exp(x)+exp(-x)) + 1.2*(exp(y)-exp(-y)) + \
1.3*((exp(-x)+exp(x))/2) + 1.4*((exp(-x)-exp(x))/2) + 1.5*(cosh(y)+sinh(y))",\
        "x,y", f47, 2, -10, 10, .1, false
    const double x = p[0], y = p[1];
    return 1.1*(exp(x)+exp(-x)) + 1.2*(exp(y)-exp(-y)) +
        1.3*((exp(-x)+exp(x))/2) + 1.4*((exp(-x)-exp(x))/2) +
        1.5*(cosh(y)+sinh(y));
}

double f48(const double* p)
{
#define P48 "sinh((log(x)/5+1)*5) + 1.2*cosh((log(x)/log(2)+1)*log(2)) + \
!(x | !(x/4))" , "x", f48, 1, 2, 1e9, 1.2e7, false
    const double x = p[0];
    return sinh((log(x)/5+1)*5) + 1.2*cosh((log(x)/log(2.0)+1)*log(2.0)) +
        (!(doubleToInt(x) || !doubleToInt(x/4)));
}

double f49(const double* p)
{
#define P49 "atan2(0, x) + (-4*(x-100))^3.3" , "x", f49, 1, -100, 100, .03, false
    const double x = p[0];
    return atan2(0, x) + pow(-4*(x-100), 3.3);
}

double f50(const double* p)
{
#define P50 "(x<y | y<x) + 2*(x<y & y<x) + 4*(x<=y & y<=x) + \
8*(x<y & x!=y) + 16*(x<y | x!=y) + 32*(x<=y & x>=y) + 64*(x<=y | x>=y) + \
128*(x!=y & x=y) + 256*(x!=y & x!=y) + 512*(x<=y & x=y)" , "x,y", \
        f50, 2, -10, 10, 1, false
    const double x = p[0], y = p[1];
    return
        (x<y || y<x) + 2*(x<y && y<x) + 4*(x<=y && y<=x) + 8*(x<y && x!=y) +
        16*(x<y || x!=y) + 32*(x<=y && x>=y) + 64*(x<=y || x>=y) +
        128*(x!=y && x==y) + 256*(x!=y && x!=y) + 512*(x<=y && x==y);
}

double f51(const double* p)
{
#define P51 "log(-x)" , "x", f51, 1, -100, -1, .5, false
    const double x = p[0];
    return log(-x);
}

double f52(const double* p)
{
#define P52Code x + (1.0+2.0+3.0+4.0-5.0-6.0-7.0-8.0)/3.0 + \
  4.0*(1.0+sin(2.0)+cos(4.0*5.0+6.0)/2.0) + cos(0.5)*tan(0.6+0.2) - \
  1.1/log(2.1)*sqrt(3.3)
#define P52 Stringify(P52Code)" + 2^3" , "x", f52, 1, -10, 10, .5, false
    const double x = p[0];
    return P52Code + pow(2.0, 3.0);
}

double f53(const double* p)
{
#define P53 "(x&y) + 1.1*(int(x/10)|int(y/10)) + 1.2*((-!-!-x)+(!-!-!y)) + \
1.3*(-------x + !!!!!!!y)" , "x,y", f53, 2, 0, 10, 1, false
    const double x = p[0], y = p[1];
    return fAnd(x,y) + 1.1*(fOr(doubleToInt(x/10.0),doubleToInt(y/10.0))) +
        1.2*((-fNot(-fNot(-x))) + fNot(-fNot(-fNot(y)))) +
        1.3*(-x + fNot(y));
}

double f54(const double* p)
{
#define P54 "(x<y)+(x<=y)+(x>y)+(x>=y)+(x=y)+(x!=y)+(x&y)+(x|y)+(!x)+(!!x)+\
!((x<y)&(x<3))+!!(!(x>y)|(x>3))", \
        "x,y", f54, 2, -100, 100, .5, false
    const double x = p[0], y = p[1];
    return (x<y)+(x<=y)+(x>y)+(x>=y)+(x==y)+(x!=y)+(fAnd(x,y))+(fOr(x,y))+
        (fNot(x))+(fNot(fNot(x)))+fNot((fAnd((x<y),(x<3))))+
        !!(fOr(!(x>y),(x>3)));
}

double f55(const double* p)
{
#define P55 "(x^1.2 < 0) + (y^2.5 < 0) + 2*(x*x<0) + 3*(y^3<0) + 4*(x^4<0)", \
        "x,y", f55, 2, 1, 100, .5, false
    const double x = p[0], y = p[1];
    return (pow(x,1.2) < 0) + (pow(y,2.5) < 0) + 2*(x*x<0) + 3*(pow(y,3)<0) +
        4*(pow(x,4)<0);
}

double f56(const double* p)
{
//#define P56 "1.6646342e+21%x", "x", f56, 1, .25, 100, .25, false
#define P56 "1.75e21%x", "x", f56, 1, .25, 100, .25, false
    const double x = p[0];
    // 1.6646342e+21 chosen as such to be larger than 2^64,
    // which is the limit where repeated runs of fprem opcode
    // on 387 is required.
    //return fmod(1.6646342e+21, x);
    return fmod(1.75e21, x);
}

double f57(const double* p)
{
#define P57 "cosh(asinh(x))", "x", f57, 1, .05, 1.0, .01, false
    const double x = p[0];
    return cosh(fp_asinh(x));
}

double f58(const double* p)
{
#define P58Code (-x < 3) + (x*-1 > 5) + (x*-3 < 10) + (x*-3 < y*7) + (x*4 < y*7) + (x*6 < y*-3) + (-x < 11) + (5 < -y)
#define P58 Stringify(P58Code), "x,y", f58, 2, -11, 11, 1, false
    const double x = p[0], y = p[1];
    return P58Code;
}

double f59(const double* p)
{
#define P59 "cosh(x^2) + tanh(y^2)", "x,y", f59, 2, -2.0, 2.0, .12, false
    const double x = p[0], y = p[1];
    return cosh(x*x) + tanh(y*y);
}

double f60(const double* p)
{
#define P60 "sqr(x) | sub(x,y) | value()", "x,y", f60, 2, -2.0, 2.0, 1, false
    return fOr(fOr(Sqr(p), Sub(p)), Value(p));
}

namespace
{
    FloatingPointTest floatingPointTests[] =
    {
        { P1,"1" }, { P2,"2" }, { P3,"3" }, { P4,"4" }, { P5,"5" },
#ifndef FP_DISABLE_EVAL
        { P6,"6" },
#endif
        { P7,  "7" }, { P8,  "8" }, { P9,  "9" }, { P10,"10" }, { P11,"11" },
        { P12,"12" }, { P13,"13" }, { P14,"14" }, { P15,"15" }, { P16,"16" },
        { P17,"17" }, { P18,"18" }, { P19,"19" }, { P20,"20" }, { P21,"21" },
        { P22,"22" }, { P23,"23" }, { P24,"24" }, { P25,"25" }, { P26,"26" },
        { P27,"27" }, { P28,"28" }, { P29,"29" }, { P30,"30" }, { P31,"31" },
        { P32,"32" }, { P33,"33" }, { P34,"34" }, { P35,"35" }, { P36,"36" },
        { P37,"37" }, { P38,"38" }, { P39,"39" }, { P40,"40" }, { P41,"41" },
        { P42,"42" }, { P43,"43" }, { P44,"44" }, { P45,"45" }, { P46,"46" },
        { P47,"47" }, { P48,"48" }, { P49,"49" }, { P50,"50" }, { P51,"51" },
        { P52,"52" }, { P53,"53" }, { P54,"54" }, { P55,"55" }, { P56,"56" },
        { P57,"57" }, { P58,"58" }, { P59,"59" }, { P60,"60" }
    };

    const unsigned floatingPointTestsAmount =
        sizeof(floatingPointTests)/sizeof(floatingPointTests[0]);
}


long fi1(const long* p)
{
#define PI1Code 1+2+3-4*5*6/3+10/2-9%2 + (x+y - 11*x + z/10 + x/(z+31))
#define PI1 Stringify(PI1Code), "x,y,z", fi1, 3, -30, 30, 1, false
    const long x = p[0], y = p[1], z = p[2];
    return PI1Code;
}

long fi2(const long* p)
{
#define PI2 "if(abs(x*y) < 20 | x+y > 30 & z > 5, min(x,2*y), max(y,z*2))", \
        "x,y,z", fi2, 3, -30, 30, 1, false
    const long x = p[0], y = p[1], z = p[2];
    return (std::abs(x*y) < 20 || (x+y > 30 && z > 5)) ?
        min(x, 2*y) : max(y, z*2);
}

long fi3(const long* p)
{
#define PI3Code (x+y) + 2*(x-z) + 3*(x*y) + 4*(y/z) + 5*(x%z) + \
        6*(x<y) + 7*(x<=z) + 8*(x>2*z) + 9*(y>=3*z) + 10*(x+y!=z) + \
        11*(100+x) + 12*(101-y) + 13*(102*z) + 14*(103/x)
#define PI3 Stringify(PI3Code), "x,y,z", fi3, 3, 1, 50, 1, false
    const long x = p[0], y = p[1], z = p[2];
    return PI3Code;
}

long fi4(const long* p)
{
#define PI4Code (-x < 3) + (x*-1 > 5) + (x*-3 < 10) + (x*-3 < y*7) + (x*4 < y*7) + (x*6 < y*-3) + (-x < 11) + (5 < -y)
#define PI4 Stringify(PI4Code), "x,y", fi4, 2, -11, 11, 1, false
    const long x = p[0], y = p[1];
    return PI4Code;
}

namespace
{
    IntTest intTests[] =
    {
        { PI1,"1" }, { PI2,"2" }, { PI3,"3" }, { PI4,"4" }
    };

    const unsigned intTestsAmount = sizeof(intTests)/sizeof(intTests[0]);
}


//=========================================================================
// Copying testing functions
//=========================================================================
bool TestCopyingNoDeepCopy(FunctionParser p)
{
    double vars[2] = { 3, 5 };

    if(std::fabs(p.Eval(vars) - 13) > Epsilon)
    {
        std::cout << "- Giving as function parameter (no deep copy): ";
        std::cout << "Failed." << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
        p.PrintByteCode(std::cout);
#endif
        return false;
    }
    return true;
}

bool TestCopyingDeepCopy(FunctionParser p)
{
    double vars[2] = { 3, 5 };

    p.Parse("x*y-1", "x,y");

    if(std::fabs(p.Eval(vars) - 14) > Epsilon)
    {
        std::cout << "- Giving as function parameter (deep copy): ";
        std::cout << "Failed." << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
        p.PrintByteCode(std::cout);
#endif
        return false;
    }
    return true;
}

bool TestCopying()
{
    std::cout << "- Testing copy constructor and assignment..." << std::endl;

    bool retval = true;
    double vars[2] = { 2, 5 };

    FunctionParser p1, p3;
    p1.Parse("x*y-2", "x,y");

    FunctionParser p2(p1);
    if(std::fabs(p2.Eval(vars) - 8) > Epsilon)
    {
        std::cout << "- Copy constructor with no deep copy: ";
        std::cout << "Failed." << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
        p2.PrintByteCode(std::cout);
#endif
        retval = false;
    }

    p2.Parse("x*y-1", "x,y");
    if(std::fabs(p2.Eval(vars) - 9) > Epsilon)
    {
        retval = false;
        std::cout << "- Copy constructor with deep copy: ";
        std::cout << "Failed." << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
        p2.PrintByteCode(std::cout);
#endif
    }

    p3 = p1;
    if(std::fabs(p3.Eval(vars) - 8) > Epsilon)
    {
        retval = false;
        std::cout << "- Assignment with no deep copy: ";
        std::cout << "Failed." << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
        p3.PrintByteCode(std::cout);
#endif
    }

    p3.Parse("x*y-1", "x,y");
    if(std::fabs(p3.Eval(vars) - 9) > Epsilon)
    {
        retval = false;
        std::cout << "- Assignment with deep copy: ";
        std::cout << "Failed." << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
        p3.PrintByteCode(std::cout);
#endif
    }

    if(!TestCopyingNoDeepCopy(p1))
        retval = false;

    // Final test to check that p1 still works:
    if(std::fabs(p1.Eval(vars) - 8) > Epsilon)
    {
        std::cout << "Failed: p1 was corrupted." << std::endl;
        retval = false;
    }

    if(!TestCopyingDeepCopy(p1))
        retval = false;

    // Final test to check that p1 still works:
    if(std::fabs(p1.Eval(vars) - 8) > Epsilon)
    {
        std::cout << "Failed: p1 was corrupted." << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
        p1.PrintByteCode(std::cout);
#endif
        retval = false;
    }

    return retval;
}


//=========================================================================
// Test error situations
//=========================================================================
bool TestErrorSituations()
{
    std::cout << "- Testing error situations..." << std::endl;

    bool retval = true;
    FunctionParser fp, tmpfp;
    fp.AddUnit("unit", 2);
    fp.AddFunction("Value", Value, 0);
    fp.AddFunction("Sqr", Sqr, 1);
    fp.AddFunction("Sub", Sub, 2);
    tmpfp.Parse("0", "x");

    static const struct
    {
        FunctionParser::ParseErrorType expected_error;
        int                            expected_error_position;
        const char*                    function_string;
    } invalidFuncs[] =
    {
      { FunctionParser::MISSING_PARENTH,     5, "sin(x"},
      { FunctionParser::EXPECT_PARENTH_FUNC, 4, "sin x"},
      { FunctionParser::SYNTAX_ERROR,        2, "x+" },
      { FunctionParser::EXPECT_OPERATOR,     2, "x x"},
      { FunctionParser::UNKNOWN_IDENTIFIER,  4, "sin(y)" },
      { FunctionParser::ILL_PARAMS_AMOUNT,   5, "sin(x, 1)" },
      { FunctionParser::EXPECT_OPERATOR,     1, "x, x"},
      { FunctionParser::SYNTAX_ERROR,        2, "x^^2" },
      { FunctionParser::SYNTAX_ERROR,        2, "x**x" },
      { FunctionParser::SYNTAX_ERROR,        2, "x+*x" },
      { FunctionParser::SYNTAX_ERROR,        0, "unit" },
      { FunctionParser::SYNTAX_ERROR,        0, "unit x" },
      { FunctionParser::SYNTAX_ERROR,        2, "x*unit" },
      { FunctionParser::SYNTAX_ERROR,        0, "unit*unit" },
      { FunctionParser::SYNTAX_ERROR,        0, "unit unit" },
      { FunctionParser::EXPECT_OPERATOR,     1, "x(unit)"},
      { FunctionParser::SYNTAX_ERROR,        2, "x+unit" },
      { FunctionParser::SYNTAX_ERROR,        2, "x*unit" },
      { FunctionParser::EMPTY_PARENTH,       1, "()"},
      { FunctionParser::SYNTAX_ERROR,        0, "" },
      { FunctionParser::EXPECT_OPERATOR,     1, "x()"},
      { FunctionParser::EMPTY_PARENTH,       3, "x*()"},
      { FunctionParser::SYNTAX_ERROR,        4, "sin(unit)" },
      { FunctionParser::EXPECT_PARENTH_FUNC, 4, "sin unit"},
      { FunctionParser::EXPECT_OPERATOR,     2, "1..2"},
      { FunctionParser::SYNTAX_ERROR,        1, "(" },
      { FunctionParser::MISM_PARENTH,        0, ")"},
      { FunctionParser::MISSING_PARENTH,     2, "(x"},
      { FunctionParser::EXPECT_OPERATOR,     1, "x)"},
      { FunctionParser::MISM_PARENTH,        0, ")x("},
      { FunctionParser::MISSING_PARENTH,     14,"(((((((x))))))"},
      { FunctionParser::EXPECT_OPERATOR,     15,"(((((((x))))))))"},
      { FunctionParser::EXPECT_OPERATOR,     1, "2x"},
      { FunctionParser::EXPECT_OPERATOR,     3, "(2)x"},
      { FunctionParser::EXPECT_OPERATOR,     3, "(x)2"},
      { FunctionParser::EXPECT_OPERATOR,     1, "2(x)"},
      { FunctionParser::EXPECT_OPERATOR,     1, "x(2)"},
      { FunctionParser::SYNTAX_ERROR,        0, "[x]" },
      { FunctionParser::SYNTAX_ERROR,        0, "@x" },
      { FunctionParser::SYNTAX_ERROR,        0, "$x" },
      { FunctionParser::SYNTAX_ERROR,        0, "{x}" },
      { FunctionParser::ILL_PARAMS_AMOUNT,   5, "max(x)" },
      { FunctionParser::ILL_PARAMS_AMOUNT,   8, "max(x, 1, 2)" },
      { FunctionParser::ILL_PARAMS_AMOUNT,   6, "if(x,2)" },
      { FunctionParser::ILL_PARAMS_AMOUNT,   10,"if(x, 2, 3, 4)" },
      { FunctionParser::MISSING_PARENTH,     6, "Value(x)"},
      { FunctionParser::MISSING_PARENTH,     6, "Value(1+x)"},
      { FunctionParser::MISSING_PARENTH,     6, "Value(1,x)"},
      // Note: ^should these three not return ILL_PARAMS_AMOUNT instead?
      { FunctionParser::ILL_PARAMS_AMOUNT,   4, "Sqr()"},
      { FunctionParser::ILL_PARAMS_AMOUNT,   5, "Sqr(x,1)" },
      { FunctionParser::ILL_PARAMS_AMOUNT,   5, "Sqr(1,2,x)" },
      { FunctionParser::ILL_PARAMS_AMOUNT,   4, "Sub()" },
      { FunctionParser::ILL_PARAMS_AMOUNT,   5, "Sub(x)" },
      { FunctionParser::ILL_PARAMS_AMOUNT,   7, "Sub(x,1,2)" },
      { FunctionParser::UNKNOWN_IDENTIFIER,  2, "x+Sin(1)" },
      { FunctionParser::UNKNOWN_IDENTIFIER,  0, "sub(1,2)" },
      { FunctionParser::UNKNOWN_IDENTIFIER,  0, "sinx(1)"  },
      { FunctionParser::UNKNOWN_IDENTIFIER,  2, "1+X"      },
#ifdef FP_DISABLE_EVAL
      { FunctionParser::UNKNOWN_IDENTIFIER,  0, "eval(x)" }
#endif
    };
    const unsigned amnt = sizeof(invalidFuncs)/sizeof(invalidFuncs[0]);
    for(unsigned i = 0; i < amnt; ++i)
    {
        int parse_result = fp.Parse(invalidFuncs[i].function_string, "x");
        if(parse_result < 0)
        {
            std::cout << "Parsing the invalid function \""
                      << invalidFuncs[i].function_string
                      << "\" didn't fail\n";
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
            fp.PrintByteCode(std::cout);
#endif
            retval = false;
        }
        else if(fp.GetParseErrorType() != invalidFuncs[i].expected_error
             || parse_result != invalidFuncs[i].expected_error_position)
        {
            std::cout << "Parsing the invalid function \""
                      << invalidFuncs[i].function_string
                      << "\" produced ";
            if(fp.GetParseErrorType() != invalidFuncs[i].expected_error)
                std::cout << "wrong error code (" << fp.ErrorMsg() << ")";
            if(parse_result != invalidFuncs[i].expected_error_position)
                std::cout << "wrong pointer (expected "
                          << invalidFuncs[i].expected_error_position
                          << ", got " << parse_result << ")";
            std::cout << "\n";
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
            fp.PrintByteCode(std::cout);
#endif
            retval = false;
        }
    }

    static const char* const invalidNames[] =
    { "s2%", "sin", "(x)", "5x", "2", "\302\240"/*nbsp*/ };
    const unsigned namesAmnt = sizeof(invalidNames)/sizeof(invalidNames[0]);

    for(unsigned i = 0; i < namesAmnt; ++i)
    {
        const char* const n = invalidNames[i];
        if(fp.AddConstant(n, 1))
        {
            std::cout << "Adding an invalid name (\"" << n
                      << "\") as constant didn't fail" << std::endl;
            retval = false;
        }
        if(fp.AddFunction(n, Sqr, 1))
        {
            std::cout << "Adding an invalid name (\"" << n
                      << "\") as funcptr didn't fail" << std::endl;
            retval = false;
        }
        if(fp.AddFunction(n, tmpfp))
        {
            std::cout << "Adding an invalid name (\"" << n
                      << "\") as funcparser didn't fail" << std::endl;
            retval = false;
        }
        if(fp.Parse("0", n) < 0)
        {
            std::cout << "Using an invalid name (\"" << n
                      << "\") as variable name didn't fail" << std::endl;
            retval = false;
        }
    }

    fp.AddConstant("CONST", 1);
    fp.AddFunction("PTR", Sqr, 1);
    fp.AddFunction("PARSER", tmpfp);

    if(fp.AddConstant("PTR", 1))
    {
        std::cout <<
            "Adding a userdef function (\"PTR\") as constant didn't fail"
                  << std::endl;
        retval = false;
    }
    if(fp.AddFunction("CONST", Sqr, 1))
    {
        std::cout <<
            "Adding a userdef constant (\"CONST\") as funcptr didn't fail"
                  << std::endl;
        retval = false;
    }
    if(fp.AddFunction("CONST", tmpfp))
    {
        std::cout <<
            "Adding a userdef constant (\"CONST\") as funcparser didn't fail"
                  << std::endl;
        retval = false;
    }

    return retval;
}


//=========================================================================
// Thoroughly test whitespaces
//=========================================================================
double wsFunc(double x)
{
    return x + sin((x*-1.5)-(.5*2.0)*(((-x)*1.5+(2-(x)*2.0)*2.0)+(3.0*2.0))+
                   (1.5*2.0))+(cos(x)*2.0);
}

bool testWsFunc(FunctionParser& fp, const std::string& function)
{
    int res = fp.Parse(function, "x");
    if(res > -1)
    {
        std::cout << "Parsing function:\n\"" << function
                  << "\"\nfailed at char " << res
                  << ": " << fp.ErrorMsg() << std::endl;
        return false;
    }

    double vars[1];
    for(vars[0] = -2.0; vars[0] <= 2.0; vars[0] += .1)
        if(fabs(fp.Eval(vars) - wsFunc(vars[0])) > Epsilon)
        {
            std::cout << "Failed.\n";
            return false;
        }
    return true;
}

bool WhiteSpaceTest()
{
    std::cout << "- Testing whitespaces..." << std::endl;

    FunctionParser fp;
    fp.AddConstant("const", 1.5);
    fp.AddUnit("unit", 2.0);
    std::string function(" x + sin ( ( x * - 1.5 ) - .5 unit * ( ( ( - x ) * "
                         "const + ( 2 - ( x ) unit ) unit ) + 3 unit ) + "
                         "( const ) unit ) + cos ( x ) unit ");

    if(!testWsFunc(fp, function)) return false;

    static const unsigned char WhiteSpaceTables[][4] =
    {
        { 1, 0x09, 0,0 }, // tab
        { 1, 0x0A, 0,0 }, // linefeed
        { 1, 0x0B, 0,0 }, // vertical tab
        { 1, 0x0D, 0,0 }, // carriage return
        { 1, 0x20, 0,0 }, // space
        { 2, 0xC2,0xA0, 0 }, // U+00A0 (nbsp)
        { 3, 0xE2,0x80,0x80 }, { 3, 0xE2,0x80,0x81 }, // U+2000 to...
        { 3, 0xE2,0x80,0x82 }, { 3, 0xE2,0x80,0x83 }, { 3, 0xE2,0x80,0x84 },
        { 3, 0xE2,0x80,0x85 }, { 3, 0xE2,0x80,0x86 }, { 3, 0xE2,0x80,0x87 },
        { 3, 0xE2,0x80,0x88 }, { 3, 0xE2,0x80,0x89 },
        { 3, 0xE2,0x80,0x8A }, { 3, 0xE2,0x80,0x8B }, // ... U+200B
        { 3, 0xE2,0x80,0xAF }, { 3, 0xE2,0x81,0x9F }, // U+202F and U+205F
        { 3, 0xE3,0x80,0x80 } // U+3000
    };
    const unsigned n_whitespaces = sizeof(WhiteSpaceTables)/sizeof(*WhiteSpaceTables);

    for(unsigned i = 0; i < function.size(); ++i)
    {
        if(function[i] == ' ')
        {
            function.erase(i, 1);
            for(size_t a = 0; a < n_whitespaces; ++a)
            {
                if(!testWsFunc(fp, function)) return false;
                int length = (int)WhiteSpaceTables[a][0];
                const char* sequence = (const char*)&WhiteSpaceTables[a][1];
                function.insert(i, sequence, length);
                if(!testWsFunc(fp, function)) return false;
                function.erase(i, length);
            }
        }
    }
    return true;
}


//=========================================================================
// Test integer powers
//=========================================================================
bool compareExpValues(double value, const std::string& funcStr,
                      double v1, double v2, bool isOptimized)
{
    const double scale = pow(10.0, floor(log10(fabs(v1))));
    const double sv1 = fabs(v1) < Epsilon ? 0 : v1/scale;
    const double sv2 = fabs(v2) < Epsilon ? 0 : v2/scale;
    const double diff = sv2-sv1;
    if(std::fabs(diff) > Epsilon)
    {
        std::cout << "For \"" << funcStr << "\" with x=" << value
                  << " the library (";
        if(!isOptimized) std::cout << "not ";
        std::cout << "optimized) returned\n"
                  << std::setprecision(18) << v2
                  << " instead of " << v1 << std::endl;
        return false;
    }
    return true;
}

bool runIntPowTest(FunctionParser& fp, const std::string& funcStr,
                   int exponent, bool isOptimized)
{
    const int absExponent = exponent < 0 ? -exponent : exponent;

    for(int valueOffset = 0; valueOffset <= 5; ++valueOffset)
    {
        const double value =
            (exponent >= 0 && valueOffset == 0) ? 0.0 :
            1.0+(valueOffset-1)/100.0;
        double v1 = exponent == 0 ? 1 : value;
        for(int i = 2; i <= absExponent; ++i)
            v1 *= value;
        if(exponent < 0) v1 = 1.0/v1;

        const double v2 = fp.Eval(&value);

        if(!compareExpValues(value, funcStr, v1, v2, isOptimized))
            return false;
    }

    return true;
}

bool runFractionalPowTest(const std::string& funcStr, double exponent)
{
    FunctionParser fp;
    if(fp.Parse(funcStr, "x") != -1)
    {
        std::cout << "Parsing \"" << funcStr <<"\" failed: "
                  << fp.ErrorMsg() << "\n";
        return false;
    }

    for(int i = 0; i < 3; ++i)
    {
        for(int valueOffset = 0; valueOffset <= 10; ++valueOffset)
        {
            const double value =
                (exponent >= 0 && valueOffset == 0) ? 0.0 :
                1.0+(valueOffset-1)/2.0;
            const double v1 = std::pow(value, exponent);
            const double v2 = fp.Eval(&value);

            if(!compareExpValues(value, funcStr, v1, v2, i > 0))
                return false;
        }
        fp.Optimize();
    }

    return true;
}

bool TestIntPow()
{
    std::cout << "- Testing integral powers..." << std::endl;

    FunctionParser fp;

    for(int exponent = -1300; exponent <= 1300; ++exponent)
    {
        std::ostringstream os;
        os << "x^" << exponent;
        const std::string func = os.str();
        if(fp.Parse(func, "x") != -1)
        {
            std::cout << "Parsing \"" << func <<"\" failed: "
                      << fp.ErrorMsg() << "\n";
            return false;
        }

        if(!runIntPowTest(fp, func, exponent, false)) return false;
        fp.Optimize();
        if(!runIntPowTest(fp, func, exponent, true)) return false;
    }

    for(int m = -27; m <= 27; ++m)
    {
        for(int n_sqrt=0; n_sqrt<=4; ++n_sqrt)
        for(int n_cbrt=0; n_cbrt<=4; ++n_cbrt)
        {
            if(n_sqrt+n_cbrt == 0) continue;

            std::ostringstream os;
            os << "x^(" << m << "/(1";
            for(int n=0; n<n_sqrt; ++n) os << "*2";
            for(int n=0; n<n_cbrt; ++n) os << "*3";
            os << "))";
            double exponent = double(m);
            if(n_sqrt > 0) exponent /= std::pow(2.0, n_sqrt);
            if(n_cbrt > 0) exponent /= std::pow(3.0, n_cbrt);
            if(!runFractionalPowTest(os.str(), exponent)) return false;
        }
    }

    return true;
}


//=========================================================================
// Test UTF-8 parsing
//=========================================================================
namespace
{
    typedef unsigned char UChar;
    struct CharValueRange { const UChar first, last; };

    const CharValueRange validValueRanges[][4] =
    {
        { { 0x30, 0x39 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }, // digits
        { { 0x41, 0x5A }, { 0, 0 }, { 0, 0 }, { 0, 0 } }, // uppercase ascii
        { { 0x5F, 0x5F }, { 0, 0 }, { 0, 0 }, { 0, 0 } }, // underscore
        { { 0x61, 0x7A }, { 0, 0 }, { 0, 0 }, { 0, 0 } }, // lowercase ascii
        // U+0080 through U+009F
        { { 0xC2, 0xC2 }, { 0x80, 0x9F }, { 0, 0 }, { 0, 0 } },
        // U+00A1 through U+00BF
        { { 0xC2, 0xC2 }, { 0xA1, 0xBF }, { 0, 0 }, { 0, 0 } },
        // U+00C0 through U+07FF
        { { 0xC3, 0xDF }, { 0x80, 0xBF }, { 0, 0 }, { 0, 0 } },
        // U+0800 through U+1FFF (skip U+2000..U+200bB, which are whitespaces)
        { { 0xE0, 0xE0 }, { 0xA0, 0xBF }, { 0x80, 0xBF }, { 0, 0 } },
        { { 0xE1, 0xE1 }, { 0x80, 0xBF }, { 0x80, 0xBF }, { 0, 0 } },
        // U+200C through U+202E (skip U+202F, which is a whitespace)
        { { 0xE2, 0xE2 }, { 0x80, 0x80 }, { 0x8C, 0xAE }, { 0, 0 } },
        // U+2030 through U+205E (skip U+205F, which is a whitespace)
        { { 0xE2, 0xE2 }, { 0x80, 0x80 }, { 0xB0, 0xBF }, { 0, 0 } },
        { { 0xE2, 0xE2 }, { 0x81, 0x81 }, { 0x80, 0x9E }, { 0, 0 } },
        // U+2060 through U+20FF (skip U+3000, which is a whitespace)
        { { 0xE2, 0xE2 }, { 0x81, 0x81 }, { 0xA0, 0xBF }, { 0, 0 } },
        { { 0xE2, 0xE2 }, { 0x82, 0xBF }, { 0x80, 0xBF }, { 0, 0 } },
        // U+3001 through U+CFFF
        { { 0xE3, 0xE3 }, { 0x80, 0x80 }, { 0x81, 0xBF }, { 0, 0 } },
        { { 0xE3, 0xE3 }, { 0x81, 0xBF }, { 0x80, 0xBF }, { 0, 0 } },
        { { 0xE4, 0xEC }, { 0x80, 0xBF }, { 0x80, 0xBF }, { 0, 0 } },
        // U+E000 through U+FFFF
        { { 0xEE, 0xEF }, { 0x80, 0xBF }, { 0x80, 0xBF }, { 0, 0 } },
        // U+10000 through U+FFFFF
        { { 0xF0, 0xF0 }, { 0x90, 0xBF }, { 0x80, 0xBF }, { 0x80, 0xBF } },
        { { 0xF1, 0xF3 }, { 0x80, 0xBF }, { 0x80, 0xBF }, { 0x80, 0xBF } },
        // U+100000 through U+10FFFF
        { { 0xF4, 0xF4 }, { 0x80, 0x8F }, { 0x80, 0xBF }, { 0x80, 0xBF } }
    };
    const unsigned validValueRangesAmount =
        sizeof(validValueRanges)/sizeof(validValueRanges[0]);

    const CharValueRange invalidValueRanges[][4] =
    {
        // spaces:
        { { 0x09, 0x09 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { { 0x0A, 0x0A }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { { 0x0B, 0x0B }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { { 0x0D, 0x0D }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { { 0x20, 0x20 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { { 0xC2, 0xC2 }, { 0xA0, 0xA0 }, { 0, 0 }, { 0, 0 } },
        { { 0xE2, 0xE2 }, { 0x80, 0x80 }, { 0x80, 0x8B }, { 0, 0 } },
        { { 0xE2, 0xE2 }, { 0x80, 0x80 }, { 0xAF, 0xAF }, { 0, 0 } },
        { { 0xE2, 0xE2 }, { 0x81, 0x81 }, { 0x9F, 0x9F }, { 0, 0 } },
        { { 0xE3, 0xE3 }, { 0x80, 0x80 }, { 0x80, 0x80 }, { 0, 0 } },
        // others:
        { { 0xC0, 0xC1 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { { 0xED, 0xED }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { { 0xF5, 0xFF }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { { 0x21, 0x2F }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { { 0x3A, 0x40 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { { 0x5B, 0x5E }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { { 0x60, 0x60 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { { 0x7B, 0x7F }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { { 0x80, 0xFF }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { { 0xE0, 0xEF }, { 0x80, 0xFF }, { 0, 0 }, { 0, 0 } },
        { { 0xF0, 0xF4 }, { 0x80, 0xFF }, { 0x80, 0xFF }, { 0, 0 } },

        { { 0xC2, 0xDF }, { 0x00, 0x7F }, { 0, 0 }, { 0, 0 } },
        { { 0xC2, 0xDF }, { 0xC0, 0xFF }, { 0, 0 }, { 0, 0 } },

        { { 0xE0, 0xE0 }, { 0x00, 0x9F }, { 0x80, 0xBF }, { 0, 0 } },
        { { 0xE0, 0xE0 }, { 0xA0, 0xBF }, { 0x00, 0x7F }, { 0, 0 } },
        { { 0xE0, 0xE0 }, { 0xA0, 0xBF }, { 0xC0, 0xFF }, { 0, 0 } },

        { { 0xE1, 0xEC }, { 0x00, 0x7F }, { 0x80, 0xBF }, { 0, 0 } },
        { { 0xE1, 0xEC }, { 0xC0, 0xFF }, { 0x80, 0xBF }, { 0, 0 } },
        { { 0xE1, 0xEC }, { 0x80, 0xBF }, { 0x00, 0x7F }, { 0, 0 } },
        { { 0xE1, 0xEC }, { 0x80, 0xBF }, { 0xC0, 0xFF }, { 0, 0 } },

        { { 0xEE, 0xEF }, { 0x00, 0x7F }, { 0x80, 0xBF }, { 0, 0 } },
        { { 0xEE, 0xEF }, { 0xC0, 0xFF }, { 0x80, 0xBF }, { 0, 0 } },
        { { 0xEE, 0xEF }, { 0x80, 0xBF }, { 0x00, 0x7F }, { 0, 0 } },
        { { 0xEE, 0xEF }, { 0x80, 0xBF }, { 0xC0, 0xFF }, { 0, 0 } },

        { { 0xF0, 0xF0 }, { 0x00, 0x8F }, { 0x80, 0xBF }, { 0x80, 0xBF } },
        { { 0xF0, 0xF0 }, { 0xC0, 0xFF }, { 0x80, 0xBF }, { 0x80, 0xBF } },
        { { 0xF0, 0xF0 }, { 0x90, 0xBF }, { 0x00, 0x7F }, { 0x80, 0xBF } },
        { { 0xF0, 0xF0 }, { 0x90, 0xBF }, { 0xC0, 0xFF }, { 0x80, 0xBF } },
        { { 0xF0, 0xF0 }, { 0x90, 0xBF }, { 0x80, 0xBF }, { 0x00, 0x7F } },
        { { 0xF0, 0xF0 }, { 0x90, 0xBF }, { 0x80, 0xBF }, { 0xC0, 0xFF } },

        { { 0xF1, 0xF3 }, { 0x00, 0x7F }, { 0x80, 0xBF }, { 0x80, 0xBF } },
        { { 0xF1, 0xF3 }, { 0xC0, 0xFF }, { 0x80, 0xBF }, { 0x80, 0xBF } },
        { { 0xF1, 0xF3 }, { 0x80, 0xBF }, { 0x00, 0x7F }, { 0x80, 0xBF } },
        { { 0xF1, 0xF3 }, { 0x80, 0xBF }, { 0xC0, 0xFF }, { 0x80, 0xBF } },
        { { 0xF1, 0xF3 }, { 0x80, 0xBF }, { 0x80, 0xBF }, { 0x00, 0x7F } },
        { { 0xF1, 0xF3 }, { 0x80, 0xBF }, { 0x80, 0xBF }, { 0xC0, 0xFF } },

        { { 0xF4, 0xF4 }, { 0x00, 0x7F }, { 0x80, 0xBF }, { 0x80, 0xBF } },
        { { 0xF4, 0xF4 }, { 0x90, 0xFF }, { 0x80, 0xBF }, { 0x80, 0xBF } },
        { { 0xF4, 0xF4 }, { 0x80, 0x8F }, { 0x00, 0x7F }, { 0x80, 0xBF } },
        { { 0xF4, 0xF4 }, { 0x80, 0x8F }, { 0xC0, 0xFF }, { 0x80, 0xBF } },
        { { 0xF4, 0xF4 }, { 0x80, 0x8F }, { 0x80, 0xBF }, { 0x00, 0x7F } },
        { { 0xF4, 0xF4 }, { 0x80, 0x8F }, { 0x80, 0xBF }, { 0xC0, 0xFF } }
    };
    const unsigned invalidValueRangesAmount =
        sizeof(invalidValueRanges)/sizeof(invalidValueRanges[0]);

    class CharIter
    {
        const CharValueRange (*valueRanges)[4];
        const unsigned valueRangesAmount;
        UChar charValues[4];
        unsigned rangeIndex, firstRangeIndex, skipIndex;

        void initCharValues()
        {
            for(unsigned i = 0; i < 4; ++i)
                charValues[i] = valueRanges[rangeIndex][i].first;
        }

     public:
        CharIter(bool skipDigits, bool skipLowerCaseAscii):
            valueRanges(validValueRanges),
            valueRangesAmount(validValueRangesAmount),
            rangeIndex(skipDigits ? 1 : 0),
            firstRangeIndex(skipDigits ? 1 : 0),
            skipIndex(skipLowerCaseAscii ? 3 : ~0U)
        {
            initCharValues();
        }

        CharIter():
            valueRanges(invalidValueRanges),
            valueRangesAmount(invalidValueRangesAmount),
            rangeIndex(0), firstRangeIndex(0), skipIndex(~0U)
        {
            initCharValues();
        }

        void appendChar(std::string& dest) const
        {
            for(unsigned i = 0; i < 4; ++i)
            {
                if(charValues[i] == 0) break;
                dest += char(charValues[i]);
            }
        }

        bool next()
        {
            for(unsigned i = 0; i < 4; ++i)
            {
                if(charValues[i] < valueRanges[rangeIndex][i].last)
                {
                    ++charValues[i];
                    return true;
                }
            }
            if(++rangeIndex == skipIndex) ++rangeIndex;
            if(rangeIndex < valueRangesAmount)
            {
                initCharValues();
                return true;
            }
            rangeIndex = firstRangeIndex;
            initCharValues();
            return false;
        }

        void print() const
        {
            std::printf("{");
            for(unsigned i = 0; i < 4; ++i)
            {
                if(charValues[i] == 0) break;
                if(i > 0) std::printf(",");
                std::printf("%02X", unsigned(charValues[i]));
            }
            std::printf("}");
        }
    };

    bool printUTF8TestError(const char* testType,
                            const CharIter* iters, unsigned length,
                            const std::string& identifier)
    {
        std::printf("\n%s failed with identifier ", testType);
        for(unsigned i = 0; i < length; ++i)
            iters[i].print();
        std::printf(": \"%s\"\n", identifier.c_str());
        return false;
    }

    bool printUTF8TestError2(const CharIter* iters, unsigned length)
    {
        std::printf("\nParsing didn't fail with invalid identifier ");
        for(unsigned i = 0; i < length; ++i)
            iters[(length-1)-i].print();
        std::printf("\n");
        return false;
    }
}

bool UTF8Test()
{
    std::cout << "- Testing UTF8..." << std::flush;

    CharIter iters[4] =
        { CharIter(true, false),
          CharIter(false, true),
          CharIter(false, false),
          CharIter(false, false) };
    std::string identifier;
    FunctionParser fp;
    const double value = 0.0;

    for(unsigned length = 1; length <= 4; ++length)
    {
        std::cout << " " << length << std::flush;
        bool cont = true;
        while(cont)
        {
            identifier.clear();
            for(unsigned i = 0; i < length; ++i)
                iters[i].appendChar(identifier);

            if(fp.Parse(identifier, identifier) >= 0)
                return printUTF8TestError("Parsing", iters, length, identifier);

            if(fp.Eval(&value) != 0.0)
                return printUTF8TestError("Evaluation", iters, length,
                                          identifier);

            cont = false;
            const unsigned step = (length == 1) ? 1 : length-1;
            for(unsigned i = 0; i < length; i += step)
                if(iters[i].next())
                {
                    cont = true;
                    break;
                }
        }
    }

    CharIter invalidIters[3] = { CharIter(), CharIter(true, false), CharIter() };
    // test 5: inv
    // test 6: inv + normal
    // test 7: normal + inv

    for(unsigned length = 1; length <= 3; ++length)
    {
        std::cout << " " << 4+length << std::flush;
        unsigned numchars = length < 3 ? length : 2;
        unsigned firstchar = length < 3 ? 0 : 1;
        bool cont = true;
        while(cont)
        {
            identifier.clear();
            identifier += 'a';
            for(unsigned i = 0; i < numchars; ++i)
                invalidIters[firstchar+i].appendChar(identifier);
            identifier += 'a';

            if(fp.Parse(identifier, identifier) < 0)
                return printUTF8TestError2(invalidIters, length);

            cont = false;
            for(unsigned i = 0; i < numchars; ++i)
                if(invalidIters[firstchar+i].next())
                {
                    cont = true;
                    break;
                }
        }
    }

    std::cout << std::endl;
    return true;
}


//=========================================================================
// Test identifier adding and removal
//=========================================================================
bool AddIdentifier(FunctionParser& fp, const std::string& name, int type)
{
    static FunctionParser anotherParser;
    static bool anotherParserInitialized = false;
    if(!anotherParserInitialized)
    {
        anotherParser.Parse("x", "x");
        anotherParserInitialized = true;
    }

    switch(type)
    {
      case 0: return fp.AddConstant(name, 123);
      case 1: return fp.AddUnit(name, 456);
      case 2: return fp.AddFunction(name, Sqr, 1);
      case 3: return fp.AddFunction(name, anotherParser);
    }
    return false;
}

bool TestIdentifiers()
{
    std::cout << "- Testing identifiers..." << std::endl;

    FunctionParser fParser;
    std::vector<std::string> identifierNames(26*26, std::string("AA"));

    unsigned nameInd = 0;
    for(int i1 = 0; i1 < 26; ++i1)
    {
        for(int i2 = 0; i2 < 26; ++i2)
        {
            identifierNames.at(nameInd)[0] = char('A' + i1);
            identifierNames[nameInd][1] = char('A' + i2);

            if(!AddIdentifier(fParser, identifierNames[nameInd], (i1+26*i2)%3))
            {
                std::cout << "Failed to add identifier '"
                          << identifierNames[nameInd] << "'\n";
                return false;
            }

            ++nameInd;
        }
    }

    std::random_shuffle(identifierNames.begin(), identifierNames.end());

    for(unsigned nameInd = 0; nameInd <= identifierNames.size(); ++nameInd)
    {
        for(unsigned removedInd = 0; removedInd < nameInd; ++removedInd)
        {
            if(!AddIdentifier(fParser, identifierNames[removedInd], 3))
            {
                std::cout << "Failure: Identifier '"
                          << identifierNames[removedInd]
                          << "' was still reserved even after removing it.\n";
                return false;
            }
            if(!fParser.RemoveIdentifier(identifierNames[removedInd]))
            {
                std::cout << "Failure: Removing the identifier '"
                          << identifierNames[removedInd]
                          << "' after adding it again failed.\n";
                return false;
            }
        }

        for(unsigned existingInd = nameInd;
            existingInd < identifierNames.size(); ++existingInd)
        {
            if(AddIdentifier(fParser, identifierNames[existingInd], 3))
            {
                std::cout << "Failure: Trying to add identifier '"
                          << identifierNames[existingInd]
                          << "' for a second time didn't fail.\n";
                return false;
            }
        }

        if(nameInd < identifierNames.size())
        {
            if(!fParser.RemoveIdentifier(identifierNames[nameInd]))
            {
                std::cout << "Failure: Trying to remove identifier '"
                          << identifierNames[nameInd] << "' failed.\n";
                return false;
            }
            if(fParser.RemoveIdentifier(identifierNames[nameInd]))
            {
                std::cout << "Failure: Trying to remove identifier '"
                          << identifierNames[nameInd]
                          << "' for a second time didn't fail.\n";
                return false;
            }
        }
    }

    return true;
}


//=========================================================================
// Test user-defined functions
//=========================================================================
namespace
{
    template<int VarsAmount>
    double userFunction(const double* p)
    {
        double result = 1.0;
        for(int i = 0; i < VarsAmount; ++i)
            result += (VarsAmount+i/10.0) * p[i];
        return result;
    }

    double(*userFunctions[])(const double*) =
    {
        userFunction<0>, userFunction<1>, userFunction<2>, userFunction<3>,
        userFunction<4>, userFunction<5>, userFunction<6>, userFunction<7>,
        userFunction<8>, userFunction<9>, userFunction<10>, userFunction<11>
    };
    const unsigned userFunctionsAmount =
        sizeof(userFunctions) / sizeof(userFunctions[0]);

    double nestedFunc1(const double* p)
    {
        return p[0] + 2.0*p[1] + 3.0*p[2];
    }

    double nestedFunc2(const double* p)
    {
        const double params[3] = { -5.0*p[0], -10.0*p[1], -p[0] };
        return p[0] + 4.0*nestedFunc1(params);
    }

    double nestedFunc3(const double* p)
    {
        const double params1[3] = { 2.5*p[0]+2.0, p[2], p[1]/2.5 };
        const double params2[2] = { p[1] / 1.5 - 1.0, p[0] - 2.5 };
        return nestedFunc1(params1) + nestedFunc2(params2);
    }
}

bool testUserDefinedFunctions()
{
    std::cout << "- Testing user-defined functions..." << std::endl;

    FunctionParser nestedParser1, nestedParser2, nestedParser3;
    nestedParser1.Parse("x + 2.0*y + 3.0*z", "x, y, z");
    nestedParser2.AddFunction("nestedFunc1", nestedParser1);
    nestedParser2.Parse("x + 4.0*nestedFunc1(-5.0*x, -10.0*y, -x)", "x,y");
    nestedParser3.AddFunction("nestedFunc1", nestedParser1);
    nestedParser3.AddFunction("nestedFunc2", nestedParser2);
    nestedParser3.Parse("nestedFunc1(2.5*x+2.0, z, y/2.5) + "
                        "nestedFunc2(y/1.5 - 1.0, x - 2.5)", "x,y,z");

    for(int iteration = 0; iteration < 2; ++iteration)
    {
        double nestedFuncParams[3];
        for(int i = 0; i < 100; ++i)
        {
            nestedFuncParams[0] = -10.0 + 20.0*i/100.0;
            for(int j = 0; j < 100; ++j)
            {
                nestedFuncParams[1] = -10.0 + 20.0*j/100.0;
                for(int k = 0; k < 100; ++k)
                {
                    nestedFuncParams[2] = -10.0 + 20.0*k/100.0;

                    const double v1 = nestedParser3.Eval(nestedFuncParams);
                    const double v2 = nestedFunc3(nestedFuncParams);
                    if(fabs(v1-v2) > 1e-10)
                    {
                        std::cout << "Nested function test failed with "
                                  << "parameter values ("
                                  << nestedFuncParams[0] << ","
                                  << nestedFuncParams[1]
                                  << ").\nThe library "
                                  << (iteration > 0 ? "(optimized) " : "")
                                  << "returned " << v1
                                  << " instead of " << v2 << "." << std::endl;
                        return false;
                    }
                }
            }
        }
        nestedParser3.Optimize();
    }

    std::string funcNames[userFunctionsAmount];
    std::string userFunctionParserFunctions[userFunctionsAmount];
    std::string userFunctionParserParameters[userFunctionsAmount];
    FunctionParser userFunctionParsers[userFunctionsAmount];
    double funcParams[userFunctionsAmount];
    FunctionParser parser1, parser2;

    for(unsigned funcInd = 0; funcInd < userFunctionsAmount; ++funcInd)
    {
        std::ostringstream functionString, paramString;

        functionString << '1';
        for(unsigned paramInd = 0; paramInd < funcInd; ++paramInd)
        {
            functionString << "+" << funcInd+paramInd/10.0
                           << "*p" << paramInd;

            if(paramInd > 0) paramString << ',';
            paramString << "p" << paramInd;
        }

        userFunctionParserFunctions[funcInd] = functionString.str();
        userFunctionParserParameters[funcInd] = paramString.str();

        if(userFunctionParsers[funcInd].Parse
           (userFunctionParserFunctions[funcInd],
            userFunctionParserParameters[funcInd]) >= 0)
        {
            std::cout << "Failed to parse function\n\""
                      << functionString.str() << "\"\nwith parameters: \""
                      << paramString.str() << "\":\n"
                      << userFunctionParsers[funcInd].ErrorMsg() << "\n";
            return false;
        }

        for(unsigned testInd = 0; testInd < 10; ++testInd)
        {
            for(unsigned paramInd = 0; paramInd < testInd; ++paramInd)
                funcParams[paramInd] = testInd+paramInd;
            const double result = userFunctions[funcInd](funcParams);
            const double parserResult =
                userFunctionParsers[funcInd].Eval(funcParams);
            if(fabs(result - parserResult) > 1e-8)
            {
                std::cout << "Function\n\"" << functionString.str()
                          << "\"\nwith parameters (";
                for(unsigned paramInd = 0; paramInd < testInd; ++paramInd)
                {
                    if(paramInd > 0) std::cout << ',';
                    std::cout << funcParams[paramInd];
                }
                std::cout << ")\nreturned " << parserResult
                          << " instead of " << result << "\n";
                return false;
            }
        }
    }

    for(unsigned funcInd = 0; funcInd < userFunctionsAmount; ++funcInd)
    {
        funcNames[funcInd] = "func00";
        funcNames[funcInd][4] = char('0' + funcInd/10);
        funcNames[funcInd][5] = char('0' + funcInd%10);

        if(!parser1.AddFunction(funcNames[funcInd], userFunctions[funcInd],
                                funcInd))
        {
            std::cout << "Failed to add user-defined function \""
                      << funcNames[funcInd] << "\".\n";
            return false;
        }
        if(!parser2.AddFunction(funcNames[funcInd],
                                userFunctionParsers[funcInd]))
        {
            std::cout << "Failed to add user-defined function parser \""
                      << funcNames[funcInd] << "\".\n";
            return false;
        }

        std::ostringstream functionString;
        for(unsigned factorInd = 0; factorInd <= funcInd; ++factorInd)
        {
            if(factorInd > 0) functionString << '+';
            functionString << factorInd+1 << "*"
                           << funcNames[factorInd] << '(';
            for(unsigned paramInd = 0; paramInd < factorInd; ++paramInd)
            {
                if(paramInd > 0) functionString << ',';
                const unsigned value = factorInd*funcInd + paramInd;
                functionString << value << "+x";
            }
            functionString << ')';
        }

        if(parser1.Parse(functionString.str(), "x") >= 0)
        {
            std::cout << "parser1 failed to parse function\n\""
                      << functionString.str() << "\":\n"
                      << parser1.ErrorMsg() << "\n";
            return false;
        }
        if(parser2.Parse(functionString.str(), "x") >= 0)
        {
            std::cout << "parser2 failed to parse function\n\""
                      << functionString.str() << "\":\n"
                      << parser2.ErrorMsg() << "\n";
            return false;
        }

        for(unsigned optimizeInd = 0; optimizeInd < 4; ++optimizeInd)
        {
            for(unsigned testInd = 0; testInd < 100; ++testInd)
            {
                const double x = testInd/10.0;
                double result = 0.0;
                for(unsigned factorInd = 0; factorInd <= funcInd; ++factorInd)
                {
                    for(unsigned paramInd = 0; paramInd < factorInd; ++paramInd)
                    {
                        const unsigned value = factorInd*funcInd + paramInd;
                        funcParams[paramInd] = value+x;
                    }
                    result +=
                        (factorInd+1) * userFunctions[factorInd](funcParams);
                }

                const double parser1Result = parser1.Eval(&x);
                const double parser2Result = parser2.Eval(&x);
                const bool parser1Failed = fabs(result - parser1Result) > 1e-8;
                const bool parser2Failed = fabs(result - parser2Result) > 1e-8;

                if(parser1Failed || parser2Failed)
                {
                    std::cout << "For function:\n\"" << functionString.str()
                              << "\"";
                    if(optimizeInd > 0)
                        std::cout << "\n(Optimized " << optimizeInd
                                  << (optimizeInd > 1 ? " times)" : " time)");
                    std::cout << "\nwith x=" << x
                              << " parser";
                    if(parser1Failed)
                        std::cout << "1 returned " << parser1Result;
                    else
                        std::cout << "2 returned " << parser2Result;
                    std::cout << " instead of " << result << ".\n";

                    if(parser2Failed)
                    {
                        std::cout << "The user-defined functions are:\n";
                        for(unsigned i = 0; i <= funcInd; ++i)
                            std::cout << funcNames[i] << "=\""
                                      << userFunctionParserFunctions[i]
                                      << "\"\n";
                    }

                    return false;
                }
            }

            parser1.Optimize();
        }
    }

    return true;
}


//=========================================================================
// Multithreaded test
//=========================================================================
#if defined(FP_USE_THREAD_SAFE_EVAL) || defined(FP_USE_THREAD_SAFE_EVAL_WITH_ALLOCA)
#include <boost/thread.hpp>

class TestingThread
{
    int mThreadNumber;
    FunctionParser* mFp;
    volatile static bool mOk;

    static double function(const double* vars)
    {
        const double x = vars[0], y = vars[1];
        return sin(sqrt(x*x+y*y)) + 2*cos(2*sqrt(2*x*x+2*y*y));
    }

 public:
    TestingThread(int n, FunctionParser* fp):
        mThreadNumber(n), mFp(fp)
    {}

    static bool ok() { return mOk; }

    void operator()()
    {
        double vars[2];
        for(vars[0] = -10.0; vars[0] <= 10.0; vars[0] += 0.02)
        {
            for(vars[1] = -10.0; vars[1] <= 10.0; vars[1] += 0.02)
            {
                if(!mOk) return;

                const double v1 = function(vars);
                const double v2 = mFp->Eval(vars);
                const double scale = pow(10.0, floor(log10(fabs(v1))));
                const double sv1 = fabs(v1) < Epsilon ? 0 : v1/scale;
                const double sv2 = fabs(v2) < Epsilon ? 0 : v2/scale;
                const double diff = sv2-sv1;

                if(fabs(diff) > 1e-6)
                {
                    mOk = false;
                    std::cout << "\nThread " << mThreadNumber
                              << " failed ([" << vars[0] << "," << vars[1]
                              << "] -> " << v2 << " vs. " << v1 << ")"
                              << std::endl;
                    return;
                }
            }
        }
    }
};

volatile bool TestingThread::mOk = true;

bool testMultithreadedEvaluation()
{
    std::cout << "- Testing multithreaded evaluation... 1" << std::flush;

    FunctionParser fp;
    fp.Parse("sin(sqrt(x*x+y*y)) + 2*cos(2*sqrt(2*x*x+2*y*y))", "x,y");

    boost::thread t1(TestingThread(1, &fp)), t2(TestingThread(2, &fp));
    t1.join();
    t2.join();
    if(!TestingThread::ok()) return false;

    std::cout << " 2" << std::flush;
    boost::thread
        t3(TestingThread(3, &fp)), t4(TestingThread(4, &fp)),
        t5(TestingThread(5, &fp)), t6(TestingThread(6, &fp));
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    if(!TestingThread::ok()) return false;

    std::cout << " 3" << std::flush;
    fp.Optimize();
    boost::thread
        t7(TestingThread(7, &fp)), t8(TestingThread(8, &fp)),
        t9(TestingThread(9, &fp));
    t7.join();
    t8.join();
    t9.join();
    if(!TestingThread::ok()) return false;

    std::cout << std::endl;
    return true;
}

#else

bool testMultithreadedEvaluation() { return true; }

#endif


//=========================================================================
// Main test function
//=========================================================================
template<typename Value_t>
inline double toDouble(const Value_t& value)
{ return double(value); }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
template<>
inline double toDouble<MpfrFloat>(const MpfrFloat& value)
{ return value.toDouble(); }
#endif

template<typename Parser_t>
bool runTest(Parser_t& fp, const FloatingPointTest& testData,
             const char* const parserType,
             const typename Parser_t::value_type Eps)
{
    double vars[10];
    typename Parser_t::value_type fp_vars[10];

    for(unsigned i = 0; i < testData.paramAmount; ++i)
        vars[i] = testData.paramMin;

    while(true)
    {
        unsigned i = 0;
        while(i < testData.paramAmount &&
              (vars[i] += testData.paramStep) > testData.paramMax)
        {
            vars[i++] = testData.paramMin;
        }

        if(i == testData.paramAmount) break;

        for(unsigned i = 0; i < testData.paramAmount; ++i)
            fp_vars[i] = vars[i];

        const typename Parser_t::value_type orig_v1 = testData.funcPtr(vars);
        const double v1 = toDouble(orig_v1);
        const double v2 = toDouble(fp.Eval(fp_vars));

        const double scale = pow(10.0, floor(log10(fabs(v1))));
        const double sv1 = fabs(v1) < Eps ? 0 : v1/scale;
        const double sv2 = fabs(v2) < Eps ? 0 : v2/scale;
        const double diff = sv2-sv1;

        if(fabs(diff) > Eps)
        {
            if(!verbose)
                std::cout << "\nFunction:\n\"" << testData.funcString
                          << "\"\n(" << parserType << ")";

            std::cout << std::endl << "Error: For (";
            for(unsigned ind = 0; ind < testData.paramAmount; ++ind)
                std::cout << (ind>0 ? ", " : "") << vars[ind];
            std::cout << ")\nthe library returned "
                      << std::setprecision(18) << v2 << " instead of "
                      << std::setprecision(18) << orig_v1 << std::endl
                      << "(Difference: "
                      << std::setprecision(18) << v2-v1
                      << "; scaled diff "
                      << diff << ")" << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
            fp.PrintByteCode(std::cout);
#endif
            return false;
        }
    }

    return true;
}

template<typename Parser_t>
bool runTest(Parser_t& fp, const IntTest& testData,
             const char* const parserType)
{
    long vars[10];
    typename Parser_t::value_type fp_vars[10];

    for(unsigned i = 0; i < testData.paramAmount; ++i)
        vars[i] = testData.paramMin;

    while(true)
    {
        unsigned i = 0;
        while(i < testData.paramAmount &&
              (vars[i] += testData.paramStep) > testData.paramMax)
        {
            vars[i++] = testData.paramMin;
        }

        if(i == testData.paramAmount) break;

        for(unsigned i = 0; i < testData.paramAmount; ++i)
            fp_vars[i] = vars[i];

        const long v1 = testData.funcPtr(vars);
        const typename Parser_t::value_type v2 = fp.Eval(fp_vars);

        if(v1 != v2)
        {
            if(!verbose)
                std::cout << "\nFunction:\n\"" << testData.funcString
                          << "\"\n(" << parserType << ")";

            std::cout << std::endl << "Error: For (";
            for(unsigned ind = 0; ind < testData.paramAmount; ++ind)
                std::cout << (ind>0 ? ", " : "") << vars[ind];
            std::cout << ")\nthe library returned "
                      << v2 << " instead of "
                      << v1 << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
            fp.PrintByteCode(std::cout);
#endif
            return false;
        }
    }

    return true;
}

//=========================================================================
// Test variable deduction
//=========================================================================
bool checkVarString(const char* idString,
                    FunctionParser& fp, unsigned funcInd, int errorIndex,
                    int variablesAmount, const std::string& variablesString)
{
    const bool stringsMatch =
        (variablesString == floatingPointTests[funcInd].paramString);
    if(errorIndex >= 0 ||
       variablesAmount != int(floatingPointTests[funcInd].paramAmount) ||
       !stringsMatch)
    {
        std::cout << "\n" << idString
                  << " ParseAndDeduceVariables() failed with function:\n\""
                  << floatingPointTests[funcInd].funcString << "\"\n";
        if(errorIndex >= 0)
            std::cout << "Error index: " << errorIndex
                      << ": " << fp.ErrorMsg() << std::endl;
        else if(!stringsMatch)
            std::cout << "Deduced var string was \"" << variablesString
                      << "\" instead of \""
                      << floatingPointTests[funcInd].paramString
                      << "\"." << std::endl;
        else
            std::cout << "Deduced variables amount was "
                      << variablesAmount << " instead of "
                      << floatingPointTests[funcInd].paramAmount << "."
                      << std::endl;
        return false;
    }
    return true;
}

bool testVariableDeduction(FunctionParser& fp, unsigned funcInd)
{
    static std::string variablesString;
    static std::vector<std::string> variables;

    if(verbose)
        std::cout << "(Variable deduction)" << std::flush;

    int variablesAmount = -1;
    int retval = fp.ParseAndDeduceVariables
        (floatingPointTests[funcInd].funcString,
         &variablesAmount, floatingPointTests[funcInd].useDegrees);
    if(retval >= 0 || variablesAmount !=
       int(floatingPointTests[funcInd].paramAmount))
    {
        std::cout <<
            "\nFirst ParseAndDeduceVariables() failed with function:\n\""
                  << floatingPointTests[funcInd].funcString << "\"\n";
        if(retval >= 0)
            std::cout << "Error index: " << retval
                      << ": " << fp.ErrorMsg() << std::endl;
        else
            std::cout << "Deduced variables amount was "
                      << variablesAmount << " instead of "
                      << floatingPointTests[funcInd].paramAmount << "."
                      << std::endl;
        return false;
    }

    variablesAmount = -1;
    retval = fp.ParseAndDeduceVariables
        (floatingPointTests[funcInd].funcString,
         variablesString,
         &variablesAmount,
         floatingPointTests[funcInd].useDegrees);
    if(!checkVarString("Second", fp, funcInd, retval, variablesAmount,
                       variablesString))
        return false;

    retval = fp.ParseAndDeduceVariables(floatingPointTests[funcInd].funcString,
                                        variables,
                                        floatingPointTests[funcInd].useDegrees);
    variablesAmount = int(variables.size());
    variablesString.clear();
    for(unsigned i = 0; i < variables.size(); ++i)
    {
        if(i > 0) variablesString += ',';
        variablesString += variables[i];
    }
    return checkVarString("Third", fp, funcInd, retval, variablesAmount,
                          variablesString);
}


//=========================================================================
// Main
//=========================================================================
template<typename Parser_t, typename TestData_t>
bool parseRegressionTestFunction(Parser_t& parser, const TestData_t& testData,
                                 const char* parserTypeStr)
{
    const int retval =
        parser.Parse(testData.funcString, testData.paramString,
                     testData.useDegrees);
    if(retval >= 0)
    {
        std::cout << "With FunctionParser" << parserTypeStr
                  << "\nin \"" << testData.funcString
                  << "\" (\"" << testData.paramString
                  << "\"), col " << retval
                  << ":\n" << parser.ErrorMsg() << std::endl;
        return false;
    }
    return true;
}

int main(int argc, char* argv[])
{
#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    MpfrFloat::setDefaultMantissaBits(80);
#endif
#ifdef FP_SUPPORT_GMP_INT_TYPE
    GmpInt::setDefaultNumberOfBits(80);
#endif

    bool runUTF8Test = true;

    for(int i = 1; i < argc; ++i)
    {
        if(std::strcmp(argv[i], "-noUTF8Test") == 0) runUTF8Test = false;
    }

    FunctionParser fp0;

    // Test that the parser doesn't crash if Eval() is called before Parse():
    fp0.Eval(0);

    fp0.setDelimiterChar('}');
    int res = fp0.Parse("x+y } ", "x,y");
    if(fp0.GetParseErrorType() != fp0.FP_NO_ERROR || res != 4)
    {
        std::cout << "Delimiter test failed" << std::endl;
        return 1;
    }
    fp0.Parse("x+}y", "x,y");
    if(fp0.GetParseErrorType() == fp0.FP_NO_ERROR)
    {
        std::cout << "Erroneous function with delimiter didn't fail"
                  << std::endl;
        return 1;
    }

    // Setup the function parser for testing
    // -------------------------------------
    FunctionParser fp;
#ifdef FP_SUPPORT_FLOAT_TYPE
    FunctionParser_f fp_f;
#endif
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
    FunctionParser_ld fp_ld;
#endif
#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    FunctionParser_mpfr fp_mpfr;
#endif

    bool ret = fp.AddConstant("pi", M_PI);
    ret = ret && fp.AddConstant("CONST", CONST);
#ifdef FP_SUPPORT_FLOAT_TYPE
    ret = ret && fp_f.AddConstant("pi", (float)M_PI);
    ret = ret && fp_f.AddConstant("CONST", CONST);
#endif
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
    ret = ret && fp_ld.AddConstant("pi", M_PI);
    ret = ret && fp_ld.AddConstant("CONST", CONST);
#endif
#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    ret = ret && fp_mpfr.AddConstant("pi", M_PI);
    ret = ret && fp_mpfr.AddConstant("CONST", CONST);
#endif
    if(!ret)
    {
        std::cout << "Ooops! AddConstant() didn't work" << std::endl;
        return 1;
    }

    ret = fp.AddUnit("doubled", 2);
    ret = ret && fp.AddUnit("tripled", 3);
#ifdef FP_SUPPORT_FLOAT_TYPE
    ret = ret && fp_f.AddUnit("doubled", 2);
    ret = ret && fp_f.AddUnit("tripled", 3);
#endif
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
    ret = ret && fp_ld.AddUnit("doubled", 2);
    ret = ret && fp_ld.AddUnit("tripled", 3);
#endif
#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    ret = ret && fp_mpfr.AddUnit("doubled", 2);
    ret = ret && fp_mpfr.AddUnit("tripled", 3);
#endif
    if(!ret)
    {
        std::cout << "Ooops! AddUnit() didn't work" << std::endl;
        return 1;
    }

    ret = fp.AddFunction("sub", Sub, 2);
    ret = ret && fp.AddFunction("sqr", Sqr, 1);
    ret = ret && fp.AddFunction("value", Value, 0);
#ifdef FP_SUPPORT_FLOAT_TYPE
    ret = ret && fp_f.AddFunction("sub", Sub_f, 2);
    ret = ret && fp_f.AddFunction("sqr", Sqr_f, 1);
    ret = ret && fp_f.AddFunction("value", Value_f, 0);
#endif
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
    ret = ret && fp_ld.AddFunction("sub", Sub_ld, 2);
    ret = ret && fp_ld.AddFunction("sqr", Sqr_ld, 1);
    ret = ret && fp_ld.AddFunction("value", Value_ld, 0);
#endif
#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    ret = ret && fp_mpfr.AddFunction("sub", Sub_mpfr, 2);
    ret = ret && fp_mpfr.AddFunction("sqr", Sqr_mpfr, 1);
    ret = ret && fp_mpfr.AddFunction("value", Value_mpfr, 0);
#endif
    if(!ret)
    {
        std::cout << "Ooops! AddFunction(ptr) didn't work" << std::endl;
        return 1;
    }

    FunctionParser SqrFun, SubFun, ValueFun;
    if(verbose) std::cout << "Parsing SqrFun... ";
    SqrFun.Parse("x*x", "x");
    if(verbose) std::cout << std::endl;
    if(verbose) std::cout << "Parsing SubFun... ";
    SubFun.Parse("x-y", "x,y");
    if(verbose) std::cout << std::endl;
    if(verbose) std::cout << "Parsing ValueFun... ";
    ValueFun.Parse("5", "");
    if(verbose) std::cout << std::endl;

#ifdef FP_SUPPORT_FLOAT_TYPE
    FunctionParser_f SqrFun_f, SubFun_f, ValueFun_f;
    SqrFun_f.Parse("x*x", "x");
    SubFun_f.Parse("x-y", "x,y");
    ValueFun_f.Parse("5", "");
#endif
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
    FunctionParser_ld SqrFun_ld, SubFun_ld, ValueFun_ld;
    SqrFun_ld.Parse("x*x", "x");
    SubFun_ld.Parse("x-y", "x,y");
    ValueFun_ld.Parse("5", "");
#endif
#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    FunctionParser_mpfr SqrFun_mpfr, SubFun_mpfr, ValueFun_mpfr;
    SqrFun_mpfr.Parse("x*x", "x");
    SubFun_mpfr.Parse("x-y", "x,y");
    ValueFun_mpfr.Parse("5", "");
#endif

    ret = fp.AddFunction("psqr", SqrFun);
    ret = ret && fp.AddFunction("psub", SubFun);
    ret = ret && fp.AddFunction("pvalue", ValueFun);
#ifdef FP_SUPPORT_FLOAT_TYPE
    ret = ret && fp_f.AddFunction("psqr", SqrFun_f);
    ret = ret && fp_f.AddFunction("psub", SubFun_f);
    ret = ret && fp_f.AddFunction("pvalue", ValueFun_f);
#endif
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
    ret = ret && fp_ld.AddFunction("psqr", SqrFun_ld);
    ret = ret && fp_ld.AddFunction("psub", SubFun_ld);
    ret = ret && fp_ld.AddFunction("pvalue", ValueFun_ld);
#endif
#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    ret = ret && fp_mpfr.AddFunction("psqr", SqrFun_mpfr);
    ret = ret && fp_mpfr.AddFunction("psub", SubFun_mpfr);
    ret = ret && fp_mpfr.AddFunction("pvalue", ValueFun_mpfr);
#endif
    if(!ret)
    {
        std::cout << "Ooops! AddFunction(parser) didn't work" << std::endl;
        return 1;
    }

    // Test repeated constant addition
    // -------------------------------
    for(double value = 0; value < 20; value += 1)
    {
        if(!fp.AddConstant("TestConstant", value))
        {
            std::cout << "Ooops2! AddConstant() didn't work" << std::endl;
            return 1;
        }

        fp.Parse("TestConstant", "");
        if(fp.Eval(0) != value)
        {
            if(value == 0) std::cout << "Usage of 'TestConstant' failed\n";
            else std::cout << "Changing the value of 'TestConstant' failed\n";
            return 1;
        }
    }


#if(1)
    // Main testing loop
    // -----------------
    std::cout << "- Performing regression tests..." << std::endl;
    std::cout << "Tested parser types: double";
#ifdef FP_SUPPORT_FLOAT_TYPE
    std::cout << ", float";
#endif
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
    std::cout << ", long double";
#endif
#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    std::cout << ", MpfrFloat";
#endif
#ifdef FP_SUPPORT_LONG_INT_TYPE
    std::cout << ", long";
#endif
#ifdef FP_SUPPORT_GMP_INT_TYPE
    std::cout << ", GmpInt";
#endif
    std::cout << std::endl;

    for(unsigned i = FIRST_TEST; i < floatingPointTestsAmount; ++i)
    {
        if(!parseRegressionTestFunction(fp, floatingPointTests[i], ""))
            return 1;

#ifdef FP_SUPPORT_FLOAT_TYPE
        if(!parseRegressionTestFunction(fp_f, floatingPointTests[i], "_f"))
            return 1;
#endif

#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
        if(!parseRegressionTestFunction(fp_ld, floatingPointTests[i], "_ld"))
            return 1;
#endif

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
        if(!parseRegressionTestFunction
           (fp_mpfr, floatingPointTests[i], "_mpfr"))
            return 1;
#endif

        //fp.PrintByteCode(std::cout);
        if(verbose)
            std::cout << /*std::right <<*/ std::setw(2) << i+1 << ": \""
                      << floatingPointTests[i].funcString << "\" (" <<
                pow((floatingPointTests[i].paramMax -
                     floatingPointTests[i].paramMin) /
                    floatingPointTests[i].paramStep,
                    static_cast<double>(floatingPointTests[i].paramAmount))
                      << " param. combinations): " << std::flush;
        else
            std::cout << floatingPointTests[i].testname << std::flush << " ";

        if(!runTest(fp, floatingPointTests[i], "Not optimized", Epsilon))
            return 1;
#ifdef FP_SUPPORT_FLOAT_TYPE
        //if(!runTest(fp_f, floatingPointTests[i], "float, not optimized", Epsilon_f))
        //    return 1;
#endif
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
        if(!runTest(fp_ld, floatingPointTests[i], "long double, not optimized", Epsilon))
            return 1;
#endif
#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
        if(!runTest(fp_mpfr, floatingPointTests[i], "MpfrFloat, not optimized", Epsilon))
            return 1;
#endif

        if(verbose) std::cout << "Ok." << std::endl;

        fp.Optimize();
        //fp.PrintByteCode(std::cout);

        if(verbose) std::cout << "    Optimized: " << std::flush;
        if(!runTest(fp, floatingPointTests[i], "After optimization", Epsilon))
            return 1;

#ifdef FP_SUPPORT_FLOAT_TYPE
        fp_f.Optimize();
        //if(!runTest(fp_f, floatingPointTests[i], "float, optimized", Epsilon_f))
        //    return 1;
#endif
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
        fp_ld.Optimize();
        //if(!runTest(fp_ld, floatingPointTests[i], "long double, optimized", Epsilon))
        //    return 1;
#endif
#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
        fp_mpfr.Optimize();
        //if(!runTest(fp_mpfr, floatingPointTests[i], "mpfr, optimized", Epsilon))
        //    return 1;
#endif

        if(verbose)
            std::cout << "(Calling Optimize() several times) " << std::flush;

        for(int j = 0; j < 20; ++j)
            fp.Optimize();
        if(!runTest(fp, floatingPointTests[i],
                    "After several optimization runs", Epsilon))
            return 1;

        if(!testVariableDeduction(fp, i)) return 1;

        if(verbose) std::cout << "Ok." << std::endl;
    }

#if defined(FP_SUPPORT_LONG_INT_TYPE) || defined(FP_SUPPORT_GMP_INT_TYPE)
    if(!verbose) std::cout << std::endl;

#ifdef FP_SUPPORT_LONG_INT_TYPE
    FunctionParser_li fp_li;
#endif
#ifdef FP_SUPPORT_GMP_INT_TYPE
    FunctionParser_gmpint fp_gmpint;
#endif

    for(unsigned i = 0; i < intTestsAmount; ++i)
    {
        std::cout << intTests[i].testname << std::flush << " ";

#ifdef FP_SUPPORT_LONG_INT_TYPE
        if(!parseRegressionTestFunction(fp_li, intTests[i], "_li"))
            return 1;
        if(!runTest(fp_li, intTests[i], "long int, not optimized"))
            return 1;
#endif
#ifdef FP_SUPPORT_GMP_INT_TYPE
        if(!parseRegressionTestFunction(fp_gmpint, intTests[i], "_gmpint"))
            return 1;
        if(!runTest(fp_gmpint, intTests[i], "GmpInt, not optimized"))
            return 1;
#endif
    }
#endif

    if(!verbose) std::cout << "Ok." << std::endl;
#endif


    // Misc. tests
    // -----------
    if(!TestCopying() || !TestErrorSituations() || !WhiteSpaceTest() ||
       !TestIntPow() || (runUTF8Test && !UTF8Test()) || !TestIdentifiers() ||
       !testUserDefinedFunctions() || !testMultithreadedEvaluation())
        return 1;

    std::cout << "==================================================\n"
              << "================== All tests OK ==================\n"
              << "==================================================\n";

    return 0;
}
