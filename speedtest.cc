//#define TEST_JIT
#define MEASURE_PARSING_SPEED_ONLY

#include "fparser.hh"
#include <ctime>
#include <string>
#include <iostream>
#include <cmath>

#define FUNC0 x*x+y*y
#define FUNC1 ((3*pow(x,4)-7*pow(x,3)+2*x*x-4*x+10)-(4*pow(y,3)+2*y*y-10*y+2))*10
#define FUNC2 ((3*(x+(5*(y+2)-7*x)*3-y)+4*5+3)-7+(8*x+5*y+(7-x))*4)-10*3+4
#define FUNC3 sin(sqrt(10-x*x+y*y))+cos(sqrt(5-x*x-y*y))+sin(x*x+y*y)
#define FUNC4 exp((-x*x-y*y)/100)*sin(sqrt(x*x+y*y))/(10*2)+sin(pow(x,4)-4*pow(x,3)+3*x*x-2*x+2*5-3)-cos(-2*pow(y,4)+5*pow(y,3)-14*x*x+8*x-120/2+4)

#define StringifyHlp(x) #x
#define Stringify(x) StringifyHlp(x)

#define CreateFunction(funcName, funcBody) \
double funcName(const double* vars) \
{ \
    const double x = vars[0], y = vars[1]; \
    return funcBody; \
}

namespace
{
    struct FuncData
    {
        const char* const funcStr;
        const std::string paramStr;
        double (*const function)(const double*);
    };

    CreateFunction(func0, FUNC0)
    CreateFunction(func1, FUNC1)
    CreateFunction(func2, FUNC2)
    CreateFunction(func3, FUNC3)
    CreateFunction(func4, FUNC4)

    const FuncData funcData[] =
    { { Stringify(FUNC0), "x,y", func0 },
      { Stringify(FUNC1), "x,y", func1 },
      { Stringify(FUNC2), "x,y", func2 },
      { Stringify(FUNC3), "x,y", func3 },
      { Stringify(FUNC4), "x,y", func4 }
    };

    const unsigned FunctionsAmount = sizeof(funcData)/sizeof(funcData[0]);
}

void printInfo(const char* title, const char* unit,
               clock_t iclock, unsigned loops)
{
    iclock = std::clock()-iclock;
    const unsigned perSecond =
        unsigned(std::floor(double(loops)*CLOCKS_PER_SEC/iclock+.5));
    std::cout << title << ": "
              << iclock * (1000000.0 / loops) / CLOCKS_PER_SEC << " us. ("
              << perSecond << " " << unit << "/s)" << std::endl;
}

int main()
{
    FunctionParser fp, fp2;
    double values[3] = { 1, 2, 3 };

    for(unsigned i = 0; i < FunctionsAmount; ++i)
    {
        // Parse function
        // --------------
        std::cout << "\n--- Function:\n\"" << funcData[i].funcStr
                  << "\"" << std::endl;

        int res = fp.Parse(funcData[i].funcStr, funcData[i].paramStr);
        if(res >= 0)
        {
            std::cout << "Col " << res << ": " << fp.ErrorMsg() << std::endl;
            return 1;
        }

        /*
        std::cout << "fparser returns " << fp.Eval(values)
                  << ", C++ returns " << funcData[i].function(values)
                  << std::endl;
        */

        const unsigned ParseLoops = 2000000;
        const unsigned EvalLoops = 10000000;
        const unsigned JitLoops = 50000000;
        const unsigned OptimizationLoops = 100000;
        const unsigned FuncLoops = 100000000;


        // Measure parsing speed
        // ---------------------
        clock_t iclock = std::clock();
        for(unsigned counter = 0; counter < ParseLoops; ++counter)
            fp.Parse(funcData[i].funcStr, funcData[i].paramStr);

        printInfo("Parse time", "parses", iclock, ParseLoops);

#ifndef MEASURE_PARSING_SPEED_ONLY
        // Measure evaluation speed
        // ------------------------
        iclock = std::clock();
        for(unsigned counter = 0; counter < EvalLoops; ++counter)
            fp.Eval(values);

        printInfo("Eval time", "evals", iclock, EvalLoops);

        // Measure evaluation speed, optimized
        // -----------------------------------
        fp2 = fp;
        fp2.Optimize();

        iclock = std::clock();
        for(unsigned counter = 0; counter < EvalLoops; ++counter)
            fp2.Eval(values);

        printInfo("Optimized", "evals", iclock, EvalLoops);


#ifdef TEST_JIT
        // Measure evaluation speed, jit-compiled
        // --------------------------------------
        fp2.CreateJIT();

        iclock = std::clock();
        for(unsigned counter = 0; counter < JitLoops; ++counter)
            fp2.Eval(values);

        printInfo("JIT-compiled", "evals", iclock, JitLoops);
#endif


        // Measure optimization speed
        // --------------------------
        iclock = std::clock();
        for(unsigned counter = 0; counter < OptimizationLoops; ++counter)
        {
            fp2 = fp;
            fp2.Optimize();
        }

        printInfo("Optimization time", "optimizes", iclock, OptimizationLoops);


        // Measure C++ function speed
        // --------------------------
        iclock = std::clock();
        for(unsigned counter = 0; counter < FuncLoops; ++counter)
            funcData[i].function(values);

        printInfo("C++ function time", "evals", iclock, FuncLoops);
#endif
    }

    return 0;
}
