//#define TEST_JIT
//#define MEASURE_PARSING_SPEED_ONLY

#include "fparser.hh"
#include <ctime>
#include <string>
#include <iostream>
#include <cmath>
#include <sstream>

#include <sys/time.h>

//#define FUNC0 x+y+(sin(x)*cos(x)*log(x)*(-x-y+log(y)-sin(x)))
#define FUNC0 pow(x,14)+pow(y,8)+pow(x,2)+2*x*y+pow(y,2)
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
    {
        { Stringify(FUNC0), "x,y", func0 },
        { Stringify(FUNC1), "x,y", func1 },
        { Stringify(FUNC2), "x,y", func2 },
        { Stringify(FUNC3), "x,y", func3 },
        { Stringify(FUNC4), "x,y", func4 }
    };

    const unsigned FunctionsAmount = sizeof(funcData)/sizeof(funcData[0]);

    std::string beautify(int value)
    {
        std::ostringstream os;
        os << value;
        std::string result = os.str();
        for(size_t i = result.size(); i > 3;)
            result.insert(i -= 3, 1, ' ');
        return result;
    }
}

class Test
{
public:
    void Start(unsigned nloops)
    {
        this->nloops = nloops;
        this->iter   = 0;
        this->result = 0;
        this->reset_threshold = nloops / 10;
        this->nloops = this->reset_threshold * 10;
        gettimeofday(&this->begin, 0);
    }
    bool Loop()
    {
        if(this->iter >= this->nloops) return false;

        this->iter += 1;
        if(!(this->iter % this->reset_threshold))
        {
            TakeResult();
        }
        return true;
    }
    void Report(const char* title, const char* unit)
    {
        std::cout << title << ": "
                  << (result) << " us. ("
                  << beautify(int(1e6/result)) << " " << unit << "/s)\n";
    }
    void TakeResult()
    {
        struct timeval end;
        gettimeofday(&end, 0);
        double begin_d = begin.tv_sec * 1e6 + begin.tv_usec;
        double   end_d =   end.tv_sec * 1e6 +   end.tv_usec;
        double diff_d = (end_d - begin_d) / this->reset_threshold;
        if(iter == this->reset_threshold
        || diff_d < result)
        {
            result = diff_d;
        }
        begin = end;
    }
private:
    unsigned nloops;
    unsigned iter;
    unsigned reset_threshold;
    struct timeval begin;
    double result;
};

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

        const unsigned ParseLoops = 2000000;
        const unsigned EvalLoops = 20000000;
        const unsigned OptimizationLoops = 20000;
        const unsigned FuncLoops = 100000000;

        Test tester;


        // Measure parsing speed
        // ---------------------
        tester.Start(ParseLoops);
        while(tester.Loop())
            fp.Parse(funcData[i].funcStr, funcData[i].paramStr);
        tester.Report("Parse time", "parses");

#ifndef MEASURE_PARSING_SPEED_ONLY
//        fp.PrintByteCode(std::cout);

        // Measure evaluation speed
        // ------------------------
        tester.Start(EvalLoops);
        while(tester.Loop())
            fp.Eval(values);
        tester.Report("Eval time", "evals");

        // Measure evaluation speed, optimized
        // -----------------------------------
        fp2 = fp;
        fp2.Optimize();

//        fp2.PrintByteCode(std::cout);

        tester.Start(EvalLoops);
        while(tester.Loop())
            fp2.Eval(values);
        tester.Report("Optimized", "evals");


#ifdef TEST_JIT
        // Measure evaluation speed, jit-compiled
        // --------------------------------------
        const unsigned JitLoops = 50000000;
        fp2.CreateJIT();

        tester.Start(JitLoops);
        while(tester.Loop())
            fp2.Eval(values);
        tester.Report("JIT-compiled", "evals");
#endif


        // Measure optimization speed
        // --------------------------
        tester.Start(OptimizationLoops);
        while(tester.Loop())
        {
            fp2 = fp;
            fp2.Optimize();
        }
        tester.Report("Optimization time", "optimizes");


        // Measure C++ function speed
        // --------------------------
        tester.Start(FuncLoops);
        while(tester.Loop())
            funcData[i].function(values);

        tester.Report("C++ function time", "evals");
#endif
    }

    return 0;
}
