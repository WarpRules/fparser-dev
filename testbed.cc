#include "fpconfig.hh"
#include "fparser.hh"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <sstream>

#define Epsilon (1e-9)

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define CONST 1.5

#define StringifyHlp(x) #x
#define Stringify(x) StringifyHlp(x)

namespace
{
    // Auxiliary functions
    // -------------------
    inline double abs(double d) { return fabs(d); }
    inline double min(double x, double y) { return x<y ? x : y; }
    inline double max(double x, double y) { return x>y ? x : y; }
    inline double r2d(double x) { return x*180.0/M_PI; }
    inline double d2r(double x) { return x*M_PI/180.0; }
    inline double cot(double x) { return 1.0 / std::tan(x); }
    inline double csc(double x) { return 1.0 / std::sin(x); }
    inline double sec(double x) { return 1.0 / std::cos(x); }
    //inline double log10(double x) { return std::log(x) / std::log(10); }

    double Sqr(const double* p)
    {
        return p[0]*p[0];
    }

    double Sub(const double* p)
    {
        return p[0]-p[1];
    }

    double Value(const double*)
    {
        return 10;
    }
}


//============================================================================
// Test function definitions
//============================================================================
struct Test
{
    const char* funcString;
    const char* paramString;
    double (*funcPtr)(double*);
    unsigned paramAmount;
    double paramMin, paramMax, paramStep;
    bool useDegrees;
};

double f1(double* p)
{
#define P1 "1+(2+3) + x*x+x+1+2+3*4+5*6*\n7-8*9", "x", f1, 1, -1000, 1000, .1, false
    double x = p[0];
    return 1+(2+3) + x*x+x+(1.0+2.0+3.0*4.0+5.0*6.0*7.0-8.0*9.0);
/*
    const double x = p[0], y = p[1], z = p[2];
#define P1 "x - (y*(y*(y*-1))*1)", "x,y,z", f1, 3, .1, 4, .1, false
    return x - (y*(y*(y*-1))*1);
*/
}
double f2(double* p)
{
#define P2 " 2 * x+ sin ( x ) / 5 + 2-sin(x)*sin(x)", "x", f2, 1, -1000, 1000, .1, false
    double x = p[0];
    return 2*x+sin(x)/5 + 2-sin(x)*sin(x);
}
double f3(double* p)
{
#define P3 "(x=y & y=x)+  1+2-3.1*4e2/.5 + x*x+y*y+z*z", "x,y,z", f3, 3, -10, 10, .5, false
    double x = p[0], y = p[1], z = p[2];
    return (x==y && y==x)+ 1.0+2.0-3.1*4e2/.5 + x*x+y*y+z*z;
}
double f4(double* p)
{
#define P4 " ( ((( ( x-y) -( ((y) *2) -3)) )* 4))+sin(x)*cos(y)-cos(x)*sin(y) ", "x,y", f4, 2, -100, 100, .5, false
    double x = p[0], y = p[1];
    return ( ((( ( x-y) -( ((y) *2) -3)) )* 4))+sin(x)*cos(y)-cos(x)*sin(y);
}
double f5(double* p)
{
#define P5 "__A5_x08^o__5_0AB_", "__A5_x08,o__5_0AB_", f5, 2, .1, 10, .05, false
    double x = p[0], y = p[1];
    return pow(x,y);
}
double f6(double* p)
{
#define P6 "if(x>0&y>0,x*y+eval(x-1,y-1),0)+1", "x,y", f6, 2, .1, 10, .1, false
    double x = p[0], y = p[1];
    double v[2] = { x-1, y-1 };
    return (x>1e-14 && y>1e-14 ? x*y+f6(v) : 0)+1;
}
double f7(double* p)
{
#define P7 "cos(x)*sin(1-x)*(1-cos(x/2)*sin(x*5))", "x", f7, 1, -10, 10, .001, false
    double x = p[0];
    return cos(x)*sin(1-x)*(1-cos(x/2)*sin(x*5));
}
double f8(double* p)
{
#define P8 "atan2(x,y)+max(x,y)", "x,y", f8, 2, -10, 10,.025, false
    double x = p[0], y = p[1];
    return atan2(x,y) + (x>y ? x : y);
}
double f9(double* p)
{
#define P9 "1.5+x*y-2+4/8+z+z+z+z+x/(y*z)", "x,y,z", f9, 3, 1, 21, .25, false
    double x = p[0], y = p[1], z = p[2];
    return 1.5+x*y-2.0+4.0/8.0+z+z+z+z+x/(y*z);
}
double f10(double* p)
{
#define P10 "1+sin(cos(max(1+2+3+4+5, x+y+z)))+2", "x,y,z", f10, 3, 1, 21, .25, false
    double x = p[0], y = p[1], z = p[2];
    return 1.0+sin(cos(max(1.0+2.0+3.0+4.0+5.0, x+y+z)))+2.0;
}
double f11(double* p)
{
#define P11 "-(-(-(-(-x))-x))+y*1+log(1.1^z)", "x,y,z", f11, 3, 1, 21, .25, false
    double x = p[0], y = p[1], z = p[2];
    return -(-(-(-(-x))-x))+y*1+log(pow(1.1,z));
}
double f12(double* p)
{
#define P12 "1/log(10^((3-2)/log(x)))", "x", f12, 1, 1, 2000, .05, false
    double x = p[0];
    return 1.0/log(pow(10.0, 1.0/log(x)));
}
double f13(double* p)
{
#define P13 "x^3 * x^4 + y^3 * y^5", "x,y", f13, 2, -50, 50, .5, false
    double x = p[0], y = p[1];
    return pow(x,3) * pow(x,4) + pow(y,3) * pow(y,5);
}
double f14(double* p)
{
#define P14 "x*pi + sin(2*pi) + CONST", "x", f14, 1, -50, 50, .01, false
    double x = p[0];
    return x*M_PI + sin(2*M_PI) + CONST;
}
double f15(double* p)
{
#define P15 "x^y/log(y) + log(x)/log(y) + log(x^y)", "x,y", f15, 2, 1.1, 8, .01, false
    double x = p[0], y = p[1];
    return pow(x,y)/log(y) + log(x)/log(y) + log(pow(x,y));
}
double f16(double* p)
{
#define P16 "if(x<0, if(y<0, x+y, x-y), if(y>0, x*y, x+2*y))", "x,y", f16, 2, -20, 20, .1, false
    double x = p[0], y = p[1];
    return x<0 ? (y<0 ? x+y : x-y) : (y>0 ? x*y : x+2*y);
}
double f17(double* p)
{
#define P17 "sqr(x)+sub(x,y)+psqr(y)+psub(y+1,x-2)-1", "x,y", f17, 2, -20, 20, .1, false
    double x = p[0], y = p[1];
    double p2[] = { y+1, x-2 };
    return Sqr(p)+Sub(p)+Sqr(p+1)+Sub(p2)-1;
}
double f18(double* p)
{
#define P18 " - ( - ( - ( - 5 ) ) ) * -x^ -y^-2", "x,y", f18, 2, 1, 20, .1, false
    double x = p[0], y = p[1];
    return - ( - ( - ( - 5 ) ) ) * -pow(x, -pow(y, -2));
}

double f19(double* p)
{
#define P19 "(x<y)+(x>y)+10*((x <= y)+(x>=y))+100*((x=y)+(x!=y))", "x,y", f19, 2, -100, 100, .5, false
    const double x = p[0], y = p[1];
    return (x<y)+(x>y)+10*((x <= y)+(x>=y))+100*((x==y)+(x!=y));
}

double f20(double* p)
{
#define P20 "(!(x != y) & !x) + !(!(!(!y)))", "x,y", f20, 2, -100, 100, 1, false
    const double x = p[0], y = p[1];
    return  (!(x != y) && !x) + !(!(!(!y)));
}

double f21(double* p)
{
#define P21 "sqr(x)+value()-pvalue ( ) ", "x", f21, 1, -10, 10, 1, false
    return Sqr(p)+Value(0)-5;
}

double f22(double* p)
{
#define P22 "3.5doubled + 10*x tripled - sin(y)doubled + 100*(x doubled-y tripled)doubled + 5/2doubled + 1.1^x doubled + 1.1doubled^x doubled", "x,y", f22, 2, -10, 10, .05, false
    double x = p[0], y = p[1];
    return (3.5*2) + 10*(x*3) - (sin(y)*2) + 100*((x*2)-(y*3))*2 + 5.0/(2*2) +
        pow(1.1, x*2) + pow(1.1*2, x*2);
}

double f23(double* p)
{
#define P23 "(x/(2*acos(0)))*180", "x", f23, 1, -1000, 1000, .1, false
    return (p[0]/(2*acos(0)))*180;
}

double f24(double* p)
{
#define P24 "(min(x, min(1,x)) + min(x, 1))/2 + min(x, 1)*3 + max(0, min(-2,0))", "x", f24, 1, -1000, 1000, .1, false
    return (std::min(*p, std::min(1.0, *p)) + std::min(*p, 1.0))/2 +
        std::min(*p, 1.0)*3 + std::max(0.0, std::min(-2.0, 0.0));
}

double f25(double* p)
{
#define P25 "a^b^c + a^-2 * (-b^2) + (-b^-c)", "a,b,c", f25, 3, 1, 3, .1, false
    const double a = p[0], b = p[1], c = p[2];
    return pow(a, pow(b, c)) + pow(a, -2) * (-pow(b, 2)) + (-pow(b, -c));
}

double f26(double* p)
{
#define P26 "sin(x) + cos(x*1.5) + asin(x*2.5) + acos(x*2.7)", "x", f26, 1, -100, 100, .1, true
    const double x = p[0];
    return sin(d2r(x)) + cos(d2r(x*1.5)) + r2d(asin(x*2.5)) + r2d(acos(x*2.7));
}

double f27(double* p)
{
/*
#define P27 "abs(x)+acos(x)+asin(x)+atan(x)+atan2(x,y)+ceil(x)+cos(x)+cosh(x)+cot(x)+csc(x) + pow(x,y)", "x,y", f27, 2, .1, .9, .025, false
    const double x = p[0], y = p[1];
    return fabs(x)+acos(x)+asin(x)+atan(x)+atan2(x,y)+ceil(x)+cos(x)+cosh(x)+
        1.0/tan(x)+1.0/sin(x) + pow(x,y);
*/
#define P27 "cot(x) + csc(y) + x", "x,y", f27, 2, .1, .9, .025, false
    const double x = p[0], y = p[1];
    return 1.0/tan(x) + 1.0/sin(y) + x;
}

double f28(double* p)
{
#define P28 "exp(x)+floor(x)+int(x)+log(x)+log10(x)+max(x,y)+min(x,y)+sec(x)+sin(x)+sinh(x)+sqrt(x)+tan(x)+tanh(x)", "x,y", f28, 2, .1, .9, .025, false
    const double x = p[0], y = p[1];
    return exp(x)+floor(x)+floor(x+.5)+log(x)+log10(x)+std::max(x,y)+
        std::min(x,y)+1.0/cos(x)+sin(x)+sinh(x)+sqrt(x)+tan(x)+tanh(x);
}

double f29(double* p)
{
#define P29 "x-y*1", "x,y", f29, 1, -100, 100, .1, false
    return p[0] - p[1]*1;
}

double f30(double* p)
{
#define P30 "x - y*1 + (x%y) + x / (y^1.1) + 2^3 + 5%3 + x^(y^0) + x^0.5", "x,y", f30, 2, 3, 10, 1, false
    const double x = p[0], y = p[1];
    return x - y*1 + fmod(x,y) + x / pow(y,1.1) + pow(2,3) + fmod(5,3) +
        pow(x,pow(y,0)) + pow(x,0.5);
}

double f31(double* p)
{
    const double x = p[0], y = p[1], z = p[2];
#define P31 "x - (y*(y*(y*-1))*1) + log(x*exp(1)^y) - log(x^y) + exp(1)^log(x+6) + 10^(log(x+6)/log(y+6)*log(z+6)/log(10)) - exp(1)^(log(x+6)*y) - 5^(log(x+7)/log(5)) + (x*z+17)^3 * (x*z+17)^2 / (x*z+17)^4", "x,y,z", f31, 3, .1, 4, .1, false

    return x - (y*(y*(y*-1))*1) + log(x*pow(exp(1),y)) - log(pow(x,y)) +
        pow(exp(1),log(x+6)) + pow(10,log(x+6)/log(y+6)*log(z+6)/log(10)) -
        pow(exp(1), log(x+6)*y) - pow(5,log(x+7)/log(5)) +
        pow(x*z+17,3) * pow(x*z+17,2) / pow(x*z+17,4);
}

double f32(double* p)
{
    const double x = p[0], y = p[1], z = p[2];
#define P32code \
    x\
    +y/y-min(3,4)-x-max(4,3)+max(3,4)-min(4,3)+0+(z*1)\
    +(x-2+2)+(x*0.5*2)+y*0\
    +min(min(min(4.0,x),1.0),min(x,min(min(y,4.0),z)))\
    +max(max(max(4.0,x),1.0),max(x,max(max(y,4.0),z)))\
    +(abs(1)+acos(1)+asin(1)+atan(1)+ceil(1.1)+cos(0)\
     +cosh(0)+floor(1.1)+log(1)+sin(0)+sinh(0)+tan(1)\
     +tanh(1)+atan2(1,1))\
    +(x-(y-z))\
    +(x+y) + (x*y)\
    +max(x,max(x,max(x,max(x,x))))*-1.0\
    +(z-z)\
    +1/sin(x) + 1/cos(y) + 1/tan(z)\
    +log10(cot(z) + csc(y) + sec(x))\
    +log(30+x)*log(40+y)/log(50+z)\
    +sin(x/57.295779513082320877)\
    +asin(x/10)*57.295779513082320877\
    +floor(-x) + 1/ceil(x)\
    +sqrt(5 * 0.2)\
    +(-x+-x+-x+-x+-x+-x)
#define P32 Stringify(P32code), "x,y,z", f32, 3, 1, 2, .1, false

    return P32code;
}

double f33(double* p)
{
#define P33 "sin(sqrt(10-x*x+y*y))+cos(sqrt(5-x*x-y*y))+sin(x*x+y*y)", "x,y", f33, 1, -100, 100, .1, false
    const double x = p[0], y = p[1];
    return sin(sqrt(10-x*x+y*y))+cos(sqrt(5-x*x-y*y))+sin(x*x+y*y);
}

double f34(double* p)
{
#define P34 "\360\220\200\200+\340\240\200*\302\200-t", "\360\220\200\200,\340\240\200,\302\200,t", f34, 4, -5, 5, 1, false
    double x = p[0], y = p[1], z = p[2], t = p[3];
    return x+y*z-t;
}

double f35(double* p)
{
#define P35 "a_very_long_variable_name_1-a_very_long_variable_name_2+Yet_a_third_very_long_variable_name*a_very_long_variable_name_1", "a_very_long_variable_name_1,a_very_long_variable_name_2,Yet_a_third_very_long_variable_name", f35, 3, -10, 10, 1, false
    double x = p[0], y = p[1], z = p[2];
    return x-y+z*x;
}

double f36(double* p)
{
#define P36 "-if(x<0, x, -x) + -if(x<5, 2, 3)", "x", f36, 1, -10, 10, .1, false
//#define P36 "-if(x<0, x, -x)", "x", f36, 1, -10, 10, .1, false
//#define P36 "-if(x<5, 2, 3)", "x", f36, 1, -10, 10, .1, false
    double x = p[0];
    return -(x<0 ? x : -x) + -(x<5 ? 2 : 3);
    //return -(x<0 ? x : -x);
    //return -(x<5 ? 2 : 3);
}

namespace
{
    Test tests[] =
    {
        { P1 }, { P2 }, { P3 }, { P4 }, { P5 }, { P6 }, { P7 }, { P8 }, { P9 },
        { P10 }, { P11 }, { P12 }, { P13 }, { P14 }, { P15 }, { P16 }, { P17 },
        { P18 }, { P19 }, { P20 }, { P21 }, { P22 }, { P23 }, { P24 }, { P25 },
        { P26 }, { P27 }, { P28 }, { P29 }, { P30 }, { P31 }, { P32 }, { P33 },
        { P34 }, { P35 }, { P36 }
    };

    const unsigned testsAmount = sizeof(tests)/sizeof(tests[0]);
}


//=========================================================================
// Copying testing functions
//=========================================================================
bool TestCopyingNoDeepCopy(FunctionParser p)
{
    double vars[2] = { 3, 5 };

    if(std::fabs(p.Eval(vars) - 13) > Epsilon)
    {
        std::cout << "Failed." << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
        p.PrintByteCode(std::cout);
#endif
        return false;
    }
    else
        std::cout << "Ok." << std::endl;
    return true;
}

bool TestCopyingDeepCopy(FunctionParser p)
{
    double vars[2] = { 3, 5 };

    p.Parse("x*y-1", "x,y");

    if(std::fabs(p.Eval(vars) - 14) > Epsilon)
    {
        std::cout << "Failed." << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
        p.PrintByteCode(std::cout);
#endif
        return false;
    }
    else
        std::cout << "Ok." << std::endl;
    return true;
}

bool TestCopying()
{
    std::cout << "*** Testing copy constructor and assignment:" << std::endl;

    bool retval = true;
    double vars[2] = { 2, 5 };

    FunctionParser p1, p3;
    p1.Parse("x*y-2", "x,y");

    std::cout << "- Copy constructor with no deep copy: ";
    FunctionParser p2(p1);
    if(std::fabs(p2.Eval(vars) - 8) > Epsilon)
    {
        std::cout << "Failed." << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
        p2.PrintByteCode(std::cout);
#endif
        retval = false;
    }
    else
        std::cout << "Ok." << std::endl;

    std::cout << "- With deep copy: ";
    p2.Parse("x*y-1", "x,y");
    if(std::fabs(p2.Eval(vars) - 9) > Epsilon)
    {
        retval = false;
        std::cout << "Failed." << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
        p2.PrintByteCode(std::cout);
#endif
    }
    else
        std::cout << "Ok." << std::endl;

    std::cout << "- Assignment with no deep copy: ";
    p3 = p1;
    if(std::fabs(p3.Eval(vars) - 8) > Epsilon)
    {
        retval = false;
        std::cout << "Failed." << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
        p3.PrintByteCode(std::cout);
#endif
    }
    else
        std::cout << "Ok." << std::endl;

    std::cout << "- With deep copy: ";
    p3.Parse("x*y-1", "x,y");
    if(std::fabs(p3.Eval(vars) - 9) > Epsilon)
    {
        retval = false;
        std::cout << "Failed." << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
        p3.PrintByteCode(std::cout);
#endif
    }
    else
        std::cout << "Ok." << std::endl;

    std::cout << "- Giving as function parameter (no deep copy): ";
    if(!TestCopyingNoDeepCopy(p1))
        retval = false;

    // Final test to check that p1 still works:
    if(std::fabs(p1.Eval(vars) - 8) > Epsilon)
    {
        std::cout << "Failed: p1 was corrupted." << std::endl;
        retval = false;
    }

    std::cout << "- Giving as function parameter (deep copy): ";
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
    std::cout << "*** Testing error situations..." << std::endl;

    bool retval = true;
    FunctionParser fp, tmpfp;
    fp.AddUnit("unit", 2);
    tmpfp.Parse("0", "x");

    const char* const invalidFuncs[] =
    { "abc", "x+y", "123b", "++c", "c++", "c+", "-", "sin", "sin()",
      "sin(x", "sin x", "x+", "x x", "sin(y)", "sin(x, 1)", "x, x",
      "x^^2", "x**x", "x+*x", "unit", "unit x", "x*unit", "unit*unit",
      "unit unit", "x(unit)", "x+unit", "x*unit", "()", "", "x()", "x*()",
      "sin(unit)", "sin unit", "1..2", "(", ")", "(x", "x)", ")x(",
      "(((((((x))))))", "(((((((x))))))))", "2x", "(2)x", "(x)2", "2(x)",
      "x(2)", "[x]", "@x", "$x", "{x}", "max(x)", "max(x, 1, 2)", "if(x,2)",
      "if(x, 2, 3, 4)"
    };
    const unsigned amnt = sizeof(invalidFuncs)/sizeof(invalidFuncs[0]);
    for(unsigned i = 0; i < amnt; ++i)
    {
        if(fp.Parse(invalidFuncs[i], "x") < 0)
        {
            std::cout << "Parsing the invalid function \"" << invalidFuncs[i]
                      << "\" didn't fail\n";
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
            fp.PrintByteCode(std::cout);
#endif
            retval = false;
        }
    }

    const char* const invalidNames[] =
    { "s2%", "sin", "(x)", "5x", "2" };
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
        if(abs(fp.Eval(vars) - wsFunc(vars[0])) > Epsilon)
        {
            std::cout << "Failed.\n";
            return false;
        }
    return true;
}

bool WhiteSpaceTest()
{
    std::cout << "*** Testing whitespaces..." << std::endl;

    FunctionParser fp;
    fp.AddConstant("const", 1.5);
    fp.AddUnit("unit", 2.0);
    std::string function(" x + sin ( ( x * - 1.5 ) - .5 unit * ( ( ( - x ) * "
                         "const + ( 2 - ( x ) unit ) unit ) + 3 unit ) + "
                         "( const ) unit ) + cos ( x ) unit ");

    if(!testWsFunc(fp, function)) return false;

    for(unsigned i = 0; i < function.size(); ++i)
    {
        if(function[i] == ' ')
        {
            function.erase(i, 1);
            if(!testWsFunc(fp, function)) return false;
            function.insert(i, "  ");
            if(!testWsFunc(fp, function)) return false;
            function.erase(i, 1);
        }
    }
    return true;
}


//=========================================================================
// Test integer powers
//=========================================================================
bool runIntPowTest(FunctionParser& fp, int exponent, bool isOptimized)
{
    const int absExponent = exponent < 0 ? -exponent : exponent;

    for(int valueOffset = 1; valueOffset <= 5; ++valueOffset)
    {
        const double value = 1.0 + valueOffset/100.0;
        double v1 = exponent == 0 ? 1 : value;
        for(int i = 2; i <= absExponent; ++i)
            v1 *= value;
        if(exponent < 0) v1 = 1.0/v1;

        const double v2 = fp.Eval(&value);

        const double scale = pow(10.0, floor(log10(fabs(v1))));
        const double sv1 = fabs(v1) < Epsilon ? 0 : v1/scale;
        const double sv2 = fabs(v2) < Epsilon ? 0 : v2/scale;
        const double diff = sv2-sv1;
        if(std::fabs(diff) > Epsilon)
        {
            std::cout << "For x^" << exponent << " with x=" << value
                      << " the library (";
            if(!isOptimized) std::cout << "not ";
            std::cout << std::setprecision(18);
            std::cout << "optimized) returned\n" << v2 << " instead of "
                      << v1 << std::endl;
            return false;
        }
    }

    return true;
}

bool TestIntPow()
{
    std::cout << "*** Testing integral powers..." << std::endl;

    FunctionParser fp;

    for(int exponent = -300; exponent <= 300; ++exponent)
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

        if(!runIntPowTest(fp, exponent, false)) return false;
        fp.Optimize();
        if(!runIntPowTest(fp, exponent, true)) return false;
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
        { { 0x30, 0x39 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { { 0x41, 0x5A }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { { 0x5F, 0x5F }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { { 0x61, 0x7A }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { { 0xC2, 0xDF }, { 0x80, 0xBF }, { 0, 0 }, { 0, 0 } },
        { { 0xE0, 0xE0 }, { 0xA0, 0xBF }, { 0x80, 0xBF }, { 0, 0 } },
        { { 0xE1, 0xEC }, { 0x80, 0xBF }, { 0x80, 0xBF }, { 0, 0 } },
        { { 0xEE, 0xEF }, { 0x80, 0xBF }, { 0x80, 0xBF }, { 0, 0 } },
        { { 0xF0, 0xF0 }, { 0x90, 0xBF }, { 0x80, 0xBF }, { 0x80, 0xBF } },
        { { 0xF1, 0xF3 }, { 0x80, 0xBF }, { 0x80, 0xBF }, { 0x80, 0xBF } },
        { { 0xF4, 0xF4 }, { 0x80, 0x8F }, { 0x80, 0xBF }, { 0x80, 0xBF } }
    };
    const unsigned validValueRangesAmount =
        sizeof(validValueRanges)/sizeof(validValueRanges[0]);

    const CharValueRange invalidValueRanges[][4] =
    {
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
    std::cout << "*** Testing UTF8..." << std::flush;

    CharIter iters[4] =
        { CharIter(true, false), CharIter(false, true), CharIter(false, false),
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

    CharIter invalidIters[2] = { CharIter(), CharIter(true, false) };

    for(unsigned length = 1; length <= 2; ++length)
    {
        std::cout << " " << 4+length << std::flush;
        bool cont = true;
        while(cont)
        {
            identifier.clear();
            for(unsigned i = 0; i < length; ++i)
                invalidIters[(length-1)-i].appendChar(identifier);

            if(fp.Parse(identifier, identifier) < 0)
                return printUTF8TestError2(invalidIters, length);

            cont = false;
            for(unsigned i = 0; i < length; ++i)
                if(invalidIters[i].next())
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
// Main test function
//=========================================================================
bool runTest(unsigned testIndex, FunctionParser& fp)
{
    double vars[10];

    for(unsigned i = 0; i < tests[testIndex].paramAmount; ++i)
        vars[i] = tests[testIndex].paramMin;

    while(true)
    {
        unsigned i = 0;
        while(i < tests[testIndex].paramAmount and
              (vars[i] += tests[testIndex].paramStep) >
              tests[testIndex].paramMax)
        {
            vars[i++] = tests[testIndex].paramMin;
        }

        if(i == tests[testIndex].paramAmount) break;

        double v1 = tests[testIndex].funcPtr(vars);
        double v2 = fp.Eval(vars);

        const double scale = pow(10.0, floor(log10(fabs(v1))));
        double sv1 = fabs(v1) < Epsilon ? 0 : v1/scale;
        double sv2 = fabs(v2) < Epsilon ? 0 : v2/scale;
        double diff = sv2-sv1;

        if(fabs(diff) > Epsilon)
        {
            std::cout << std::endl << "Error: For (";
            for(unsigned ind = 0; ind < tests[testIndex].paramAmount; ++ind)
                std::cout << (ind>0 ? ", " : "") << vars[ind];
            std::cout << ")\nthe library returned "
                      << std::setprecision(18) << v2 << " instead of "
                      << std::setprecision(18) << v1 << std::endl
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


//=========================================================================
// Main
//=========================================================================
int main()
{
    std::cout << "Performing tests..." << std::endl;

    FunctionParser fp0;
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

    bool ret = fp.AddConstant("pi", M_PI);
    ret &= fp.AddConstant("CONST", CONST);
    if(!ret)
    {
        std::cout << "Ooops! AddConstant() didn't work" << std::endl;
        return 1;
    }

    ret = fp.AddUnit("doubled", 2);
    ret &= fp.AddUnit("tripled", 3);
    if(!ret)
    {
        std::cout << "Ooops! AddUnit() didn't work" << std::endl;
        return 1;
    }

    ret = fp.AddFunction("sub", Sub, 2);
    ret &= fp.AddFunction("sqr", Sqr, 1);
    ret &= fp.AddFunction("value", Value, 0);
    if(!ret)
    {
        std::cout << "Ooops! AddFunction(ptr) didn't work" << std::endl;
        return 1;
    }

    FunctionParser SqrFun, SubFun, ValueFun;
    std::cout << "Parsing SqrFun... "; SqrFun.Parse("x*x", "x");
    std::cout << std::endl;
    std::cout << "Parsing SubFun... "; SubFun.Parse("x-y", "x,y");
    std::cout << std::endl;
    std::cout << "Parsing ValueFun... "; ValueFun.Parse("5", "");
    std::cout << std::endl;

    ret = fp.AddFunction("psqr", SqrFun);
    ret &= fp.AddFunction("psub", SubFun);
    ret &= fp.AddFunction("pvalue", ValueFun);
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


    // Main testing loop
    // -----------------
    for(unsigned i = 0; i < testsAmount; ++i)
    {
        int retval = fp.Parse(tests[i].funcString, tests[i].paramString,
                              tests[i].useDegrees);
        if(retval >= 0)
        {
            std::cout << "In \"" << tests[i].funcString << "\" (\""
                      << tests[i].paramString << "\"), col " << retval
                      << ":\n" << fp.ErrorMsg() << std::endl;
            return 1;
        }

        //fp.PrintByteCode(std::cout);
        std::cout << /*std::right <<*/ std::setw(2) << i+1 << ": \""
                  << tests[i].funcString << "\" (" <<
            pow((tests[i].paramMax-tests[i].paramMin)/tests[i].paramStep,
                static_cast<double>(tests[i].paramAmount))
                  << " param. combinations): " << std::flush;

        if(!runTest(i, fp)) return 1;
        std::cout << "Ok." << std::endl;

        fp.Optimize();
        //fp.PrintByteCode(std::cout);
        std::cout << "    Optimized: " << std::flush;
        if(!runTest(i, fp)) return 1;

        std::cout << "(Calling Optimize() several times) " << std::flush;
        for(int j = 0; j < 20; ++j)
            fp.Optimize();
        if(!runTest(i, fp)) return 1;

        std::cout << "Ok." << std::endl;
    }


    // Misc. tests
    // -----------
    if(!TestCopying() || !TestErrorSituations() || !WhiteSpaceTest() ||
       !TestIntPow() || !UTF8Test())
        return 1;

    std::cout << "==================================================\n"
              << "================== All tests OK ==================\n"
              << "==================================================\n";

    return 0;
}
