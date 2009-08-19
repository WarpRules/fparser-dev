#include "fparser.hh"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstring>
#include <ctime>
#include <cmath>
#include <sstream>

#define SEPARATOR \
"--------------------------------------------------------------------\n"

namespace
{
    const unsigned kTestTime = 200; // In milliseconds
    const unsigned kParseLoopsPerUnit = 20000;
    const unsigned kEvalLoopsPerUnit = 50000;
    const unsigned kOptimizeLoopsPerUnit = 10000;
    const unsigned kVarValueSetsAmount = 50;
    const double kEpsilon = 1e-9;

    struct TimingInfo
    {
        double mMicroSeconds;
        unsigned mLoopsPerSecond;
    };

    struct FunctionInfo
    {
        std::string mFunctionString;
        FunctionParser mParser;
        TimingInfo mParseTiming;
        TimingInfo mEvalTiming;
        TimingInfo mOptimizeTiming;
        TimingInfo mOptimizedEvalTiming;
    };

    FunctionParser gParser, gAuxParser;
    std::string gFunctionString, gVarString("x");
    std::vector<std::vector<double> > gVarValues;

    inline void doParse() { gParser.Parse(gFunctionString, gVarString); }
    inline void doEval() { gParser.Eval(&gVarValues[0][0]); }
    inline void doOptimize() { gAuxParser = gParser; gAuxParser.Optimize(); }

    template<void(*Function)(), unsigned loopsPerUnit>
    TimingInfo getTimingInfo()
    {
        unsigned loopUnitsPerformed = 0;
        unsigned totalMilliseconds;
        std::clock_t iClock = std::clock();
        do
        {
            for(unsigned i = 0; i < loopsPerUnit; ++i)
                Function();
            ++loopUnitsPerformed;
            totalMilliseconds =
                (std::clock() - iClock) * 1000 / CLOCKS_PER_SEC;
        }
        while(totalMilliseconds < kTestTime);

        const double totalSeconds = totalMilliseconds / 1000.0;
        const double totalLoops =
            double(loopUnitsPerformed) * double(loopsPerUnit);

        TimingInfo info;
        info.mMicroSeconds = totalSeconds * 1e6 / totalLoops;
        info.mLoopsPerSecond = unsigned(totalLoops / totalSeconds + 0.5);

        return info;
    }

    unsigned gTimingCounter = 0, gTimingTotalCount;

    void printTimingInfo()
    {
        std::cout << "\rTiming " << gTimingCounter * 100 / gTimingTotalCount
                  << "%" << std::flush;
        ++gTimingCounter;
    }

    void getTimingInfo(FunctionInfo& info)
    {
        gFunctionString = info.mFunctionString;

        printTimingInfo();
        info.mParseTiming = getTimingInfo<doParse, kParseLoopsPerUnit>();
        printTimingInfo();
        info.mEvalTiming = getTimingInfo<doEval, kEvalLoopsPerUnit>();
        printTimingInfo();
        info.mOptimizeTiming = getTimingInfo<doEval, kOptimizeLoopsPerUnit>();
        gParser.Optimize();
        printTimingInfo();
        info.mOptimizedEvalTiming = getTimingInfo<doEval, kEvalLoopsPerUnit>();
    }

    bool findValidVarValues(std::vector<FunctionInfo>& functions)
    {
        unsigned varsAmount = 1;
        for(size_t i = 0; i < gVarString.length(); ++i)
            if(gVarString[i] == ',')
                ++varsAmount;

        std::vector<double> varValues(varsAmount, 0);
        std::vector<double> deltas(varsAmount, .25);

        while(true)
        {
            bool ok = true;
            for(size_t i = 0; i < functions.size(); ++i)
            {
                double value = functions[i].mParser.Eval(&varValues[0]);
                if(value < -1e12 || value > 1e12)
                {
                    ok = false;
                    break;
                }
            }
            if(ok)
            {
                gVarValues.push_back(varValues);
                if(gVarValues.size() >= kVarValueSetsAmount)
                    return true;
            }

            size_t varIndex = 0;
            while(true)
            {
                varValues[varIndex] = -varValues[varIndex];
                if(varValues[varIndex] < 0.0)
                    break;

                varValues[varIndex] += deltas[varIndex];
                deltas[varIndex] *= 3.0;

                if(varValues[varIndex] <= 1000000.0)
                    break;
                else
                {
                    varValues[varIndex] = 0.0;
                    deltas[varIndex] = 0.25;
                    if(++varIndex == varValues.size())
                    {
                        if(gVarValues.empty())
                        {
                            gVarValues.push_back
                                (std::vector<double>(varsAmount, 0));
                            return false;
                        }
                        return true;
                    }
                }
            }
        }
    }

    bool compareFunctions(size_t function1Index, size_t function2Index,
                          FunctionParser& parser1, bool parser1Optimized,
                          FunctionParser& parser2, bool parser2Optimized)
    {
        const size_t varsAmount = gVarValues[0].size();
        for(size_t varSetInd = 0; varSetInd < gVarValues.size(); ++varSetInd)
        {
            const double* values = &gVarValues[varSetInd][0];
            const double v1 = parser1.Eval(values);
            const double v2 = parser2.Eval(values);

            const double scale = pow(10.0, floor(log10(fabs(v1))));
            const double sv1 = fabs(v1) < kEpsilon ? 0 : v1/scale;
            const double sv2 = fabs(v2) < kEpsilon ? 0 : v2/scale;
            const double diff = sv2-sv1;
            if(std::fabs(diff) > kEpsilon)
            {
                std::cout << "For variable values (";
                for(size_t i = 0; i < varsAmount; ++i)
                {
                    if(i > 0) std::cout << ",";
                    std::cout << values[i];
                }
                std::cout << ")\n";
                std::cout << "function " << function1Index+1 << " (";
                if(!parser1Optimized) std::cout << "not ";
                std::cout << "optimized) returned "
                          << std::setprecision(18) << v1 << "\n";
                std::cout << "function " << function2Index+1 << " (";
                if(!parser2Optimized) std::cout << "not ";
                std::cout << "optimized) returned "
                          << std::setprecision(18) << v2
                          << "\n(Difference: " << (v2-v1)
                          << ", scaled diff: "
                          << std::setprecision(18) << diff << ")\n"
                          << SEPARATOR;
                return false;
            }
        }
        return true;
    }

    void checkEquality(const std::vector<FunctionInfo>& functions)
    {
        FunctionParser parser1, parser2;

        for(size_t ind1 = 0; ind1 < functions.size(); ++ind1)
        {
            parser1.Parse(functions[ind1].mFunctionString, gVarString);
            parser2.Parse(functions[ind1].mFunctionString, gVarString);
            parser2.Optimize();
            compareFunctions(ind1, ind1, parser1, false, parser2, true);

            for(size_t ind2 = ind1+1; ind2 < functions.size(); ++ind2)
            {
                parser1.Parse(functions[ind1].mFunctionString, gVarString);
                parser2.Parse(functions[ind2].mFunctionString, gVarString);
                bool ok = compareFunctions(ind1, ind2,
                                           parser1, false, parser2, false);

                if(ok)
                {
                    parser1.Optimize();
                    ok = compareFunctions(ind1, ind2,
                                          parser1, true, parser2, false);
                }

                if(ok)
                {
                    parser2.Optimize();
                    ok = compareFunctions(ind1, ind2,
                                          parser1, true, parser2, true);
                }

                if(ok)
                {
                    parser1.Parse(functions[ind1].mFunctionString, gVarString);
                    compareFunctions(ind1, ind2,
                                     parser1, false, parser2, true);
                }
            }
        }
    }

    void printByteCodes(const std::vector<FunctionInfo>& functions)
    {
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
        FunctionParser parser;
        for(size_t i = 0; i < functions.size(); ++i)
        {
            std::cout << "Function " << i+1
                      << " original           Optimized\n"
                      << "-------------------           ---------\n";

            std::stringstream stream1, stream2;
            parser.Parse(functions[i].mFunctionString, gVarString);
            parser.PrintByteCode(stream1);
            parser.Optimize();
            parser.PrintByteCode(stream2);

            std::string line1, line2;
            while(true)
            {
                if(stream1) std::getline(stream1, line1);
                else line1.clear();
                if(stream2) std::getline(stream2, line2);
                else line2.clear();
                if(line1.empty() && line2.empty()) break;

                line1.resize(30, ' ');
                std::cout << line1 << line2 << "\n";
            }
            std::cout << SEPARATOR;
        }
#endif
    }

    void printFunctionTimings(const std::vector<FunctionInfo>& functions)
    {
        std::printf
            ("     ,------------------------------------------------,\n"
             "     |      Parse |      Eval |  Eval (O) |  Optimize |\n"
             ",----+------------+-----------+-----------+-----------+\n");
        for(size_t i = 0; i < functions.size(); ++i)
            std::printf("| %2u | %10.3f |%10.3f |%10.3f |%10.3f |\n", i+1,
                        functions[i].mParseTiming.mMicroSeconds,
                        functions[i].mEvalTiming.mMicroSeconds,
                        functions[i].mOptimizedEvalTiming.mMicroSeconds,
                        functions[i].mOptimizeTiming.mMicroSeconds);
        std::printf
            ("'-----------------------------------------------------'\n");
    }

    bool checkFunctionValidity(FunctionInfo& info)
    {
        int result = info.mParser.Parse(info.mFunctionString, gVarString);
        if(result >= 0)
        {
            std::cerr << "\"" << info.mFunctionString << "\"\n"
                      << std::string(result+1, ' ')
                      << "^ " << info.mParser.ErrorMsg() << "\n";
            return false;
        }
        return true;
    }

    int printHelp(const char* programName)
    {
        std::cerr <<
            "Usage: " << programName <<
            " [<options] <function1> [<function2> ...]\n\n"
            "Options:\n"
            "  -v <var string> : Specify a var string. Default is \"x\".\n"
            "  -nt             : No timing measurements.\n";
        return 1;
    }
}

int main(int argc, char* argv[])
{
    if(argc < 2) return printHelp(argv[0]);

    std::vector<FunctionInfo> functions;
    bool measureTimings = true;

    for(int i = 1; i < argc; ++i)
    {
        if(std::strcmp(argv[i], "-v") == 0)
        {
            if(++i == argc) return printHelp(argv[0]);
            gVarString = argv[i];
        }
        else if(std::strcmp(argv[i], "-nt") == 0)
            measureTimings = false;
        else
        {
            functions.push_back(FunctionInfo());
            functions.back().mFunctionString = argv[i];
        }
    }

    if(functions.empty()) return printHelp(argv[0]);

    for(size_t i = 0; i < functions.size(); ++i)
        if(!checkFunctionValidity(functions[i]))
            return 1;

    const bool validVarValuesFound = findValidVarValues(functions);

    if(measureTimings)
    {
        gTimingTotalCount = functions.size() * 4;
        for(size_t i = 0; i < functions.size(); ++i)
            getTimingInfo(functions[i]);
        printTimingInfo();
        std::cout << std::endl;
    }

    std::cout << SEPARATOR;
    for(size_t i = 0; i < functions.size(); ++i)
        std::cout << "- Function " << i+1 << ": \""
                  << functions[i].mFunctionString << "\"\n";
    const unsigned varsAmount = gVarValues[0].size();
    std::cout << "- Var string: \"" << gVarString << "\" ("
              << gVarValues[0].size()
              << (varsAmount == 1 ? " variable)\n" : " variables)\n");
    std::cout << SEPARATOR;

#if(0)
    std::cout << "Testing with variable values:\n";
    for(size_t i = 0; i < gVarValues.size(); ++i)
    {
        if(i > 0) std::cout << (i%5==0 ? "\n" : " ");
        std::cout << "(";
        for(size_t j = 0; j < gVarValues[i].size(); ++j)
        {
            if(j > 0) std::cout << ",";
            std::cout << gVarValues[i][j];
        }
        std::cout << ")";
    }
    if(!validVarValuesFound)
        std::cout << " [no valid variable values were found...]";
    std::cout << "\n" << SEPARATOR;
#else
    if(!validVarValuesFound)
        std::cout << "Warning: No valid variable values were found."
                  << " Using (0,0).\n"
                  << SEPARATOR;
#endif

    checkEquality(functions);
    printByteCodes(functions);

    if(measureTimings)
    {
        printFunctionTimings(functions);
    }

    return 0;
}
