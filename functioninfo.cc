#include "fparser.hh"
#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <string>
#include <cstring>
#include <ctime>
#include <cmath>
#include <sstream>

#define SEPARATOR \
"----------------------------------------------------------------------------"

namespace
{
    const unsigned kTestTime = 250; // In milliseconds
    const unsigned kParseLoopsPerUnit = 100000;
    const unsigned kEvalLoopsPerUnit = 300000;
    const unsigned kOptimizeLoopsPerUnit = 1000;
    const double kEpsilon = 1e-9;
    const bool kPrintTimingProgress = false;

    const unsigned kMaxVarValueSetsAmount = 10000;
    const double kVarValuesUpperLimit = 100000.0;
    const double kVarValuesInitialDelta = 0.1;
    const double kVarValuesDeltaFactor1 = 1.25;
    const double kVarValuesDeltaFactor2 = 10.0;
    const double kVarValuesDeltaFactor2Threshold = 10.0;


    class ParserWithConsts: public FunctionParser
    {
     public:
        ParserWithConsts()
        {
            AddConstant("pi", 3.14159265358979323846);
            AddConstant("e", 2.71828182845904523536);
        }
    };

    struct TimingInfo
    {
        double mMicroSeconds;
        unsigned mLoopsPerSecond;
    };

    struct FunctionInfo
    {
        std::string mFunctionString;
        ParserWithConsts mParser;
        TimingInfo mParseTiming;
        TimingInfo mEvalTiming;
        TimingInfo mOptimizeTiming;
        TimingInfo mOptimizedEvalTiming;
    };

    ParserWithConsts gParser, gAuxParser;
    std::string gFunctionString, gVarString;
    std::vector<std::vector<double> > gVarValues;
    const double* gEvalParameters = 0;

    inline void doParse() { gParser.Parse(gFunctionString, gVarString); }
    inline void doEval() { gParser.Eval(gEvalParameters); }
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
        //std::cout << loopUnitsPerformed << "\n";

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
        if(!kPrintTimingProgress) return;
        std::cout << "Timing " << gTimingCounter * 100 / gTimingTotalCount
                  << "%\r" << std::flush;
        ++gTimingCounter;
    }

    void getTimingInfo(FunctionInfo& info)
    {
        gFunctionString = info.mFunctionString;
        gEvalParameters = &gVarValues.back()[0];

        printTimingInfo();
        info.mParseTiming = getTimingInfo<doParse, kParseLoopsPerUnit>();
        printTimingInfo();
        info.mEvalTiming = getTimingInfo<doEval, kEvalLoopsPerUnit>();
        printTimingInfo();
        info.mOptimizeTiming =
            getTimingInfo<doOptimize, kOptimizeLoopsPerUnit>();
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
        std::vector<double> deltas(varsAmount, kVarValuesInitialDelta);

        while(true)
        {
            bool wasOk = false;
            for(size_t i = 0; i < functions.size(); ++i)
            {
                double value = functions[i].mParser.Eval(&varValues[0]);
                if(!(value < -1e14 || value > 1e14))
                {
                    wasOk = true;
                    break;
                }
            }
            if(wasOk)
            {
                gVarValues.push_back(varValues);
                if(gVarValues.size() >= kMaxVarValueSetsAmount)
                    return true;
            }

            size_t varIndex = 0;
            while(true)
            {
                varValues[varIndex] = -varValues[varIndex];
                if(varValues[varIndex] < 0.0)
                    break;

                varValues[varIndex] += deltas[varIndex];
                if(deltas[varIndex] < kVarValuesDeltaFactor2Threshold)
                    deltas[varIndex] *= kVarValuesDeltaFactor1;
                else
                    deltas[varIndex] *= kVarValuesDeltaFactor2;

                if(varValues[varIndex] <= kVarValuesUpperLimit)
                    break;
                else
                {
                    varValues[varIndex] = 0.0;
                    deltas[varIndex] = kVarValuesInitialDelta;
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
                          ParserWithConsts& parser1, bool parser1Optimized,
                          ParserWithConsts& parser2, bool parser2Optimized)
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
                          << SEPARATOR << std::endl;
                return false;
            }
        }
        return true;
    }

    bool checkEquality(const std::vector<FunctionInfo>& functions)
    {
        ParserWithConsts parser1, parser2;

        bool errors = false;
        for(size_t ind1 = 0; ind1 < functions.size(); ++ind1)
        {
            parser1.Parse(functions[ind1].mFunctionString, gVarString);
            parser2.Parse(functions[ind1].mFunctionString, gVarString);
            parser2.Optimize();
            errors = errors || !compareFunctions(ind1, ind1, parser1, false, parser2, true);

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

                if(!ok) errors = true;
            }
        }
        return !errors;
    }

    void printByteCodes(const std::vector<FunctionInfo>& functions)
    {
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
        ParserWithConsts parser;
        for(size_t i = 0; i < functions.size(); ++i)
        {
            std::cout
                << "Function " << i+1
                << " original                   | Optimized\n"
                << "-------------------                   | ---------"
                << std::endl;

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

                if(line1.length() > 38)
                {
                    line1.resize(38, ' ');
                    line1[37] = '~';
                }
                else line1.resize(38, ' ');
                if(line2.length() > 38)
                {
                    line2.resize(38, ' ');
                    line2[37] = '~';
                }
                std::cout << line1 << "| " << line2 << "\n";
            }
            std::cout << SEPARATOR << std::endl;
        }
#endif
    }

    void printFunctionTimings(std::vector<FunctionInfo>& functions)
    {
        std::printf
        ("     ,--------------------------------------------------------,\n"
         "     |        Parse |        Eval |    Eval (O) |    Optimize |\n"
         ",----+--------------+-------------+-------------+-------------+\n");
        for(size_t i = 0; i < functions.size(); ++i)
        {
            getTimingInfo(functions[i]);
            std::printf("| %2u | %12.3f |%12.3f |%12.3f |%12.1f |\n", i+1,
                        functions[i].mParseTiming.mMicroSeconds,
                        functions[i].mEvalTiming.mMicroSeconds,
                        functions[i].mOptimizedEvalTiming.mMicroSeconds,
                        functions[i].mOptimizeTiming.mMicroSeconds);
        }
        std::printf
        ("'-------------------------------------------------------------'\n");
    }

    bool checkFunctionValidity(FunctionInfo& info)
    {
        int result = info.mParser.Parse(info.mFunctionString, gVarString);
        if(result >= 0)
        {
            std::cerr << "\"" << info.mFunctionString << "\"\n"
                      << std::string(result+1, ' ')
                      << "^ " << info.mParser.ErrorMsg() << std::endl;
            return false;
        }
        return true;
    }

    void deduceVariables(const std::vector<FunctionInfo>& functions)
    {
        typedef std::set<std::string> StrSet;
        StrSet varNames;
        ParserWithConsts parser;

        for(size_t funcInd = 0; funcInd < functions.size(); ++funcInd)
        {
            const std::string funcStr = functions[funcInd].mFunctionString;

            while(true)
            {
                gVarString.clear();
                for(StrSet::iterator iter = varNames.begin();
                    iter != varNames.end();
                    ++iter)
                {
                    if(iter != varNames.begin()) gVarString += ",";
                    gVarString += *iter;
                }

                int index = parser.Parse(funcStr, gVarString);
                if(index < 0) break;

                int index2 = index;
                if(index2 < int(funcStr.length()) &&
                   (std::isalpha(funcStr[index2]) || funcStr[index2] == '_'))
                {
                    while(index2 < int(funcStr.length()) &&
                          (std::isalnum(funcStr[index2]) ||
                           funcStr[index2] == '_'))
                        ++index2;
                }

                if(index2 == index)
                {
                    gVarString.clear();
                    return;
                }
                varNames.insert(funcStr.substr(index, index2-index));
            }
        }
    }

    int printHelp(const char* programName)
    {
        std::cerr <<
            "Usage: " << programName <<
            " [<options] <function1> [<function2> ...]\n\n"
            "Options:\n"
            "  -vars <var string> : Specify a var string.\n"
            "  -nt                : No timing measurements.\n"
            "  -ntd               : No timing if functions differ.\n";
        return 1;
    }
}

int main(int argc, char* argv[])
{
    if(argc < 2) return printHelp(argv[0]);

    std::vector<FunctionInfo> functions;
    bool measureTimings = true, noTimingIfEqualityErrors = false;

    for(int i = 1; i < argc; ++i)
    {
        if(std::strcmp(argv[i], "-vars") == 0)
        {
            if(++i == argc) return printHelp(argv[0]);
            gVarString = argv[i];
        }
        else if(std::strcmp(argv[i], "-nt") == 0)
            measureTimings = false;
        else if(std::strcmp(argv[i], "-ntd") == 0)
            noTimingIfEqualityErrors = true;
        else
        {
            functions.push_back(FunctionInfo());
            functions.back().mFunctionString = argv[i];
        }
    }

    if(functions.empty()) return printHelp(argv[0]);

    if(gVarString.empty())
        deduceVariables(functions);

    for(size_t i = 0; i < functions.size(); ++i)
        if(!checkFunctionValidity(functions[i]))
            return 1;

    const bool validVarValuesFound = findValidVarValues(functions);

    std::cout << SEPARATOR << std::endl;
    for(size_t i = 0; i < functions.size(); ++i)
        std::cout << "- Function " << i+1 << ": \""
                  << functions[i].mFunctionString << "\"\n";
    const unsigned varsAmount = gVarValues[0].size();
    const unsigned varValueSetsAmount = gVarValues.size();
    std::cout << "- Var string: \"" << gVarString << "\" ("
              << gVarValues[0].size()
              << (varsAmount == 1 ? " var" : " vars")
              << ") (using " << varValueSetsAmount << " set"
              << (varValueSetsAmount == 1 ? ")\n" : "s)\n");
    std::cout << SEPARATOR << std::endl;

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
    std::cout << "\n" << SEPARATOR << std::endl;
#else
    if(!validVarValuesFound)
        std::cout << "Warning: No valid variable values were found."
                  << " Using (0,0).\n"
                  << SEPARATOR << std::endl;
#endif

    const bool equalityErrors = checkEquality(functions) == false;

    printByteCodes(functions);

    if(noTimingIfEqualityErrors && equalityErrors)
        measureTimings = false;

    if(measureTimings)
    {
        gTimingTotalCount = functions.size() * 4;
        printFunctionTimings(functions);
    }

    return 0;
}
