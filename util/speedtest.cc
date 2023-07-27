/*==========================================================================
  speedtest
  ---------
  Copyright: Juha Nieminen, Joel Yliluoma
  This program (speedtest) is distributed under the terms of
  the GNU General Public License (GPL) version 3.
  See gpl.txt for the license text.
============================================================================*/

static const char* const kVersionNumber = "1.1.0";

//#define TEST_JIT
//#define MEASURE_PARSING_SPEED_ONLY

#include "fparser.hh"
#include "fparser_mpfr.hh"
#include "extrasrc/fpaux.hh"

#include <ctime>
#include <string>
#include <iostream>
#include <cmath>
#include <sstream>
#include <cstring>

#include <sys/time.h>

#define N(v) static_cast<Value_t>(v)
#define P(v) FUNCTIONPARSERTYPES::fp_const_preciseDouble<Value_t>(v)

#define FUNC0 sin(x)*sin(x) + cos(x)*cos(x) + tan(y)*tan(y)
#define FUNC0P FUNC0

#define FUNC1 ((N(3)*pow(x,N(4))-N(7)*pow(x,N(3))+N(2)*x*x-N(4)*x+N(10)) - (N(4)*pow(y,N(3))+N(2)*y*y-N(10)*y+N(2)))*N(10)
#define FUNC1P ((3*x^4-7*x^3+2*x^2-4*x+10) - (4*y^3+2*y^2-10*y+2))*10

#define FUNC2 ((N(3)*(x+(N(5)*(y+N(2))-N(7)*x)*N(3)-y)+N(4)*N(5)+N(3))-N(7)+(N(8)*x+N(5)*y+(N(7)-x))*N(4))-N(10)*N(3)+N(4)
#define FUNC2P ((3*(x+(5*(y+2)-7*x)*3-y)+4*5+3)-7+(8*x+5*y+(7-x))*4)-10*3+4

#define FUNC3 pow((tan(x)*cos(x)), N(2)) - P(1.2)*log(atan2(sqrt((-pow(y,N(2)))+N(1)), y) * pow(P(4.91), y)) + pow(cos(-x), N(2))
#define FUNC3P (tan(x)*cos(x))^2 - 1.2*log(atan2(sqrt((-y^2)+1), y) * 4.91^y) + cos(-x)^2

#define FUNC4 exp((-x*x-y*y)/N(100))*sin(sqrt(x*x+y*y))/(N(10)*N(2)) + sin(pow(x,N(4))-N(4)*pow(x,N(3))+N(3)*x*x-N(2)*x+N(2)*N(5)-N(3)) - cos(-N(2)*pow(y,N(4))+N(5)*pow(y,N(3))-N(14)*x*x+N(8)*x-N(120)/N(2)+N(4))
#define FUNC4P exp((-x*x-y*y)/100)*sin(sqrt(x*x+y*y))/(10*2) + sin(pow(x,4)-4*pow(x,3)+3*x*x-2*x+2*5-3) - cos(-2*pow(y,4)+5*pow(y,3)-14*x*x+8*x-120/2+4)

#define StringifyHlp(x) #x
#define Stringify(x) StringifyHlp(x)

#define CreateFunction(funcName, Value_t, funcBody) \
Value_t funcName(const Value_t* vars) \
{ \
    const Value_t x = vars[0], y = vars[1]; \
    return funcBody; \
}

namespace
{
    bool gPrintHTML = false;

    struct FuncData
    {
        const char* const funcStr;
        const std::string paramStr;
        double (*const function_d)(const double*);
        float (*const function_f)(const float*);
        long double (*const function_ld)(const long double*);
        MpfrFloat (*const function_mpfr)(const MpfrFloat*);
    };

#define CreateTestFunctions(suffix, type) \
    CreateFunction(func0_##suffix, type, FUNC0) \
    CreateFunction(func1_##suffix, type, FUNC1) \
    CreateFunction(func2_##suffix, type, FUNC2) \
    CreateFunction(func3_##suffix, type, FUNC3) \
    CreateFunction(func4_##suffix, type, FUNC4)

#define Value_t double
    CreateTestFunctions(d, Value_t)
#undef Value_t

#define exp expf
#define pow powf
#define log logf
#define sin sinf
#define cos cosf
#define tan tanf
#define atan2 atan2f
#define sqrt sqrtf

#define Value_t float
    CreateTestFunctions(f, Value_t)
#undef Value_t

#undef exp
#undef pow
#undef log
#undef sin
#undef cos
#undef tan
#undef atan2
#undef sqrt

#define exp expl
#define pow powl
#define log logl
#define sin sinl
#define cos cosl
#define tan tanl
#define atan2 atan2l
#define sqrt sqrtl

#define Value_t long double
    CreateTestFunctions(ld, Value_t)
#undef Value_t

#undef exp
#undef pow
#undef log
#undef sin
#undef cos
#undef tan
#undef atan2
#undef sqrt

#define exp(x) MpfrFloat::exp(x)
#define pow(x, y) MpfrFloat::pow(x, y)
#define log(x) MpfrFloat::log(x)
#define sin(x) MpfrFloat::sin(x)
#define cos(x) MpfrFloat::cos(x)
#define tan(x) MpfrFloat::tan(x)
#define atan2(x, y) MpfrFloat::atan2(x, y)
#define sqrt(x) MpfrFloat::sqrt(x)

#define Value_t MpfrFloat
    CreateTestFunctions(mpfr, Value_t)
#undef Value_t

#undef exp
#undef pow
#undef log
#undef sin
#undef cos
#undef tan
#undef atan2
#undef sqrt

#undef N
#undef P

    const FuncData funcData[] =
    {
        { Stringify(FUNC0P), "x,y", func0_d, func0_f, func0_ld, func0_mpfr },
        { Stringify(FUNC1P), "x,y", func1_d, func1_f, func1_ld, func1_mpfr },
        { Stringify(FUNC2P), "x,y", func2_d, func2_f, func2_ld, func2_mpfr },
        { Stringify(FUNC3P), "x,y", func3_d, func3_f, func3_ld, func3_mpfr },
        { Stringify(FUNC4P), "x,y", func4_d, func4_f, func4_ld, func4_mpfr }
    };

    const unsigned FunctionsAmount = sizeof(funcData)/sizeof(funcData[0]);

    struct BenchmarkLoopAmounts
    {
        const unsigned ParseLoops;
        const unsigned EvalLoops;
        const unsigned OptimizationLoops;
        const unsigned FuncLoops;
    };

    inline double callFunc(const FuncData& data, const double* values)
    {
        return data.function_d(values);
    }

    inline float callFunc(const FuncData& data, const float* values)
    {
        return data.function_f(values);
    }

    inline long double callFunc(const FuncData& data, const long double* values)
    {
        return data.function_ld(values);
    }

    inline MpfrFloat callFunc(const FuncData& data, const MpfrFloat* values)
    {
        return data.function_mpfr(values);
    }

    std::string beautify(int value)
    {
        std::ostringstream os;
        os << value;
        std::string result = os.str();
        for(std::size_t i = result.size(); i > 3;)
            result.insert(i -= 3, gPrintHTML ? "&nbsp;" : " ");
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
    void Report(const char* title, const char* unit,
                bool printTimeAsInt = false)
    {
        if(gPrintHTML)
        {
            std::cout.precision(2);
            std::cout << " <li>" << std::fixed;
        }
        std::cout << title << ": ";
        if(printTimeAsInt) std::cout << int(result);
        else std::cout << result;
        std::cout << (gPrintHTML ? " &micro;s. (" : " us. (")
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

template<typename Parser_t>
int run(const unsigned ParseLoops,
        const unsigned EvalLoops,
        const unsigned OptimizationLoops,
        const unsigned FuncLoops)
{
    Parser_t fp, fp2;
    using Value_t = typename Parser_t::value_type;
    Value_t values[3] =
        { FUNCTIONPARSERTYPES::fp_const_preciseDouble<Value_t>(.25),
          FUNCTIONPARSERTYPES::fp_const_preciseDouble<Value_t>(.5),
          FUNCTIONPARSERTYPES::fp_const_preciseDouble<Value_t>(.75) };

    for(unsigned i = 0; i < FunctionsAmount; ++i)
    {
        // Parse function
        // --------------
        if(gPrintHTML)
            std::cout << "\n<hr>\n<p>Function:\n<code>\""
                      << funcData[i].funcStr << "\"</code>" << std::endl;
        else
            std::cout << "\n--- Function:\n\"" << funcData[i].funcStr
                      << "\"" << std::endl;

        int res = fp.Parse(funcData[i].funcStr, funcData[i].paramStr);
        if(res >= 0)
        {
            std::cout << "Col " << res << ": " << fp.ErrorMsg() << std::endl;
            return 1;
        }

        Test tester;

        if(gPrintHTML) std::cout << "<ul>\n";

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
        tester.Report("Optimization time", "optimizes", gPrintHTML);


        // Measure C++ function speed
        // --------------------------
        if(!gPrintHTML)
        {
            tester.Start(FuncLoops);
            while(tester.Loop())
                callFunc(funcData[i], values);

            tester.Report("C++ function time", "evals");
        }
#endif

        if(gPrintHTML) std::cout << "</ul>\n";
    }

    return 0;
}

int main(int argc, char* argv[])
{
    enum ParserType { FP_D, FP_F, FP_LD, FP_MPFR };
    ParserType parserType = FP_D;

    for(int i = 1; i < argc; ++i)
    {
        if(std::strcmp(argv[i], "-html") == 0) gPrintHTML = true;
        else if(std::strcmp(argv[i], "-f") == 0) parserType = FP_F;
        else if(std::strcmp(argv[i], "-ld") == 0) parserType = FP_LD;
        else if(std::strcmp(argv[i], "-mpfr") == 0) parserType = FP_MPFR;
        else if(std::strcmp(argv[i], "--help") == 0
             || std::strcmp(argv[i], "-help") == 0
             || std::strcmp(argv[i], "-h") == 0
             || std::strcmp(argv[i], "/?") == 0)
        {
            std::cout <<
                "FunctionParser speedtest " << kVersionNumber <<
                "\n\nUsage: " << argv[0] << " [<option> ...]\n"
                "\n"
                "    -f                Test float datatype\n"
                "    -ld               Test long double datatype\n"
                "    -mpfr             Test MPFR datatype\n"
                "    -html             Print output in html format\n"
                "    -h, --help        This help\n"
                "\n";
            return 0;
        }
    }

    switch(parserType)
    {
      case FP_D:
#ifndef FP_DISABLE_DOUBLE_TYPE
          if(!gPrintHTML)
              std::cout << "*** Running tests for FunctionParser ***\n";
          return run<FunctionParser>(1000000, 10000000, 5000, 20000000);
#else
          break;
#endif
      case FP_F:
#ifdef FP_SUPPORT_FLOAT_TYPE
          if(!gPrintHTML)
              std::cout << "*** Running tests for FunctionParser_f ***\n";
          return run<FunctionParser_f>(1000000, 10000000, 5000, 20000000);
#else
          break;
#endif
      case FP_LD:
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
          if(!gPrintHTML)
              std::cout << "*** Running tests for FunctionParser_ld ***\n";
          return run<FunctionParser_ld>(1000000, 10000000, 5000, 20000000);
#else
          break;
#endif
      case FP_MPFR:
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
          if(!gPrintHTML)
              std::cout << "*** Running tests for FunctionParser_mpfr ***\n";
          return run<FunctionParser_mpfr>(50000, 100000, 500, 100000);
#else
          break;
#endif
    }
    std::cerr << "Unsupported datatype (disabled in fpconfig.hh)\n";

    return 0;
}
