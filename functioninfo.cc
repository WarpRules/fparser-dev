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

    double Sqr(const double* p)
    {
        return p[0]*p[0];
    }

    double Sub(const double* p)
    {
        return p[0]-p[1];
    }

    double Value(const double* )
    {
        return 10.0;
    }

    class ParserWithConsts: public FunctionParser
    {
     public:
        ParserWithConsts()
        {
            AddConstant("pi", 3.14159265358979323846);
            AddConstant("e", 2.71828182845904523536);
            AddUnit("k", 1000);
            AddUnit("M", 1000000);
            AddUnit("dozen", 12);
            AddUnit("dozens", 12);

            AddFunction("sqr", Sqr, 1);
            AddFunction("sub", Sub, 2);
            AddFunction("value", Value, 0);
            static FunctionParser SqrFun; SqrFun.Parse("x*x", "x");
            static FunctionParser SubFun; SubFun.Parse("x-y", "x,y");
            static FunctionParser ValueFun; ValueFun.Parse("5", "");
            AddFunction("psqr", SqrFun);
            AddFunction("psub", SubFun);
            AddFunction("pvalue", ValueFun);
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
        TimingInfo mDoubleOptimizeTiming;
        TimingInfo mOptimizedEvalTiming;
        TimingInfo mDoubleOptimizedEvalTiming;
    };

    ParserWithConsts gParser, gAuxParser;
    std::string gFunctionString, gVarString;
    std::vector<std::vector<double> > gVarValues;
    bool gUseDegrees = false;
    const double* gEvalParameters = 0;

    inline void doParse() { gParser.Parse(gFunctionString, gVarString, gUseDegrees); }
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
            totalMilliseconds = unsigned(
                (std::clock() - iClock) * 1000 / CLOCKS_PER_SEC );
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

    unsigned gTimingCounter = 0;
    size_t gTimingTotalCount;

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
        info.mOptimizeTiming = // optimizing a non-optimized parsing
            getTimingInfo<doOptimize, kOptimizeLoopsPerUnit>();

        printTimingInfo();
        gParser.Optimize();
        info.mDoubleOptimizeTiming = // optimizing an already-optimized parsing
            getTimingInfo<doOptimize, kOptimizeLoopsPerUnit>();

        printTimingInfo(); // evaluating an optimized parsing
        info.mOptimizedEvalTiming = getTimingInfo<doEval, kEvalLoopsPerUnit>();

        printTimingInfo();
        gParser.Optimize(); // evaluating a twice-optimized parsing
        info.mDoubleOptimizedEvalTiming =
            getTimingInfo<doEval, kEvalLoopsPerUnit>();
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
                          ParserWithConsts& parser1, const char* parser1Type,
                          ParserWithConsts& parser2, const char* parser2Type)
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
                std::cout << "******* For variable values (";
                for(size_t i = 0; i < varsAmount; ++i)
                {
                    if(i > 0) std::cout << ",";
                    std::cout << values[i];
                }
                std::cout << ")\n";
                std::cout << "******* function " << function1Index+1
                          << " (" << parser1Type << ") returned "
                          << std::setprecision(18) << v1 << "\n";
                std::cout << "******* function " << function2Index+1
                          << " (" << parser2Type << ") returned "
                          << std::setprecision(18) << v2
                          << "\n******* (Difference: " << (v2-v1)
                          << ", scaled diff: "
                          << std::setprecision(18) << diff << ")\n"
                          << SEPARATOR << std::endl;
                return false;
            }
        }
        return true;
    }

    bool had_double_optimization_problems = false;

    bool checkEquality(const std::vector<FunctionInfo>& functions)
    {
        static const char not_optimized[] = "not optimized";
        static const char optimized[]     = "optimized";
        static const char optimized2[]    = "double-optimized";
        static const char* const optimize_labels[3] =
            { not_optimized, optimized, optimized2 };

        ParserWithConsts parser1, parser2, parser3;

        bool errors = false;
        for(size_t ind1 = 0; ind1 < functions.size(); ++ind1)
        {
            parser1.Parse(functions[ind1].mFunctionString, gVarString, gUseDegrees);
            parser2.Parse(functions[ind1].mFunctionString, gVarString, gUseDegrees);
            // parser 1 is not optimized
            parser2.Optimize(); // parser 2 is optimized once

            if(!compareFunctions(ind1, ind1, parser1, not_optimized,
                                 parser2, optimized))
                errors = true;

            parser2.Optimize(); // parser 2 is optimized twice
            if(!compareFunctions(ind1, ind1, parser1, not_optimized,
                                 parser2, optimized2))
                errors = had_double_optimization_problems = true;

            parser1.Optimize(); // parser 1 is optimized once
            if(!compareFunctions(ind1, ind1, parser1, optimized,
                                 parser2, optimized2))
                errors = had_double_optimization_problems = true;

            for(size_t ind2 = ind1+1; ind2 < functions.size(); ++ind2)
            {
                parser1.Parse(functions[ind1].mFunctionString, gVarString, gUseDegrees);
                for(int n_optimizes1 = 0; n_optimizes1 <= 2; ++n_optimizes1)
                {
                    if(errors) break;
                    if(n_optimizes1 > 0) parser1.Optimize();

                    parser2.Parse(functions[ind2].mFunctionString, gVarString, gUseDegrees);

                    for(int n_optimizes2 = 0; n_optimizes2 <= 2; ++n_optimizes2)
                    {
                        if(n_optimizes2 > 0) parser2.Optimize();
                        bool ok = compareFunctions(ind1, ind2,
                            parser1, optimize_labels[n_optimizes1],
                            parser2, optimize_labels[n_optimizes2]);
                        if(!ok)
                        {
                            errors = true;
                            if(n_optimizes1 > 1 || n_optimizes2 > 1)
                                had_double_optimization_problems = true;
                            break;
                        }
                    }
                }
            }
        }
        return !errors;
    }

    void wrapLine(std::string& line, size_t cutter, std::string& wrap_buf,
                  bool always_cut = false)
    {
        if(line.size() <= cutter)
            line.resize(cutter, ' ');
        else
        {
            if(!always_cut)
            {
                for(size_t wrap_at = cutter; wrap_at > 0; --wrap_at)
                {
                    char c = line[wrap_at-1];
                    if(c == '*' || c == '+' || c == '/' || c == '('
                    || c == ')' || c == '^' || c == ',' || c == '&'
                    || c == '|' || c == '-')
                    {
                        wrap_buf = std::string(20, ' ');
                        wrap_buf += line.substr(wrap_at);
                        line.erase(line.begin()+wrap_at, line.end());
                        line.resize(cutter, ' ');
                        return;
                    }
                }
            }

            line.resize(cutter, ' ');
            line[cutter-1] = '~';
        }
    }

    enum PrintMode { print_wrap, print_cut, print_no_cut_or_wrap };

    void printByteCodes(const std::vector<FunctionInfo>& functions,
                        PrintMode mode = print_no_cut_or_wrap)
    {
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
        ParserWithConsts parser;
        const char* const wall =
            (mode == print_no_cut_or_wrap)
                ? "\33[0m| "
                : "| ";
        const char* const newline =
            (mode == print_no_cut_or_wrap)
                ? "\33[0m\n"
                : "\n";
        const char* colors[3] = { "\33[37m", "\33[36m", "\33[32m" };
        if(mode != print_no_cut_or_wrap)
            colors[0] = colors[1] = colors[2] = "";

        for(size_t i = 0; i < functions.size(); ++i)
        {
            std::stringstream streams[3];

            parser.Parse(functions[i].mFunctionString, gVarString, gUseDegrees);

            size_t one_column  = 38;
            size_t two_columns = one_column * 2 + 2;

            streams[0] <<
                "Function " << i+1 << " original\n"
                "-------------------\n";
            parser.PrintByteCode(streams[0]);

            streams[1] <<
                "Optimized\n"
                "---------\n";
            parser.Optimize();
            {
                std::ostringstream streams2_bytecodeonly;
                parser.PrintByteCode(streams2_bytecodeonly);
                streams[1] << streams2_bytecodeonly.str();

                parser.Optimize();
                {
                    std::ostringstream streams3_bytecodeonly;
                    parser.PrintByteCode(streams3_bytecodeonly);

                    if(had_double_optimization_problems ||
                       streams2_bytecodeonly.str() !=
                       streams3_bytecodeonly.str())
                    {
                        streams[2] <<
                            "Double-optimized\n"
                            "----------------\n";
                        streams[2] << streams3_bytecodeonly.str();
                        //one_column  = 24;
                        //two_columns = one_column * 2 + 2;
                    }
                }
            }

            std::string streams_wrap_buf[3];
            std::string lines[3];
            while(true)
            {
                bool all_empty = true;
                for(int p=0; p<3; ++p)
                {
                    if(!streams_wrap_buf[p].empty())
                    {
                        lines[p].clear();
                        lines[p].swap( streams_wrap_buf[p] );
                    }
                    else if(streams[p])
                        std::getline(streams[p], lines[p]);
                    else
                        lines[p].clear();
                    if(!lines[p].empty()) all_empty = false;
                }
                if(all_empty) break;

                if(mode != print_no_cut_or_wrap)
                {
                    if(!lines[1].empty())
                        wrapLine(lines[0], one_column, streams_wrap_buf[0],
                                 mode == print_cut);
                    else if(!lines[2].empty())
                        wrapLine(lines[0], two_columns, streams_wrap_buf[0],
                                 mode == print_cut);
                    if(!lines[2].empty() && !lines[1].empty())
                        wrapLine(lines[1], one_column, streams_wrap_buf[1],
                                 mode == print_cut);
                }
                else
                {
                    bool wrap0 = false;
                    if(!lines[1].empty())
                    {
                        if(lines[0].size() >= one_column) wrap0 = true;
                        else lines[0].resize(one_column, ' ');
                    }
                    else if(!lines[2].empty())
                    {
                        if(lines[0].size() >= two_columns) wrap0 = true;
                        else lines[0].resize(two_columns, ' ');
                    }

                    if(wrap0)
                    {
                        lines[1].swap(streams_wrap_buf[1]);
                        if(!lines[2].empty() && lines[0].size() >= two_columns)
                            lines[2].swap(streams_wrap_buf[2]);
                        else
                            lines[0].resize(two_columns, ' ');
                    }

                    bool wrap1 = false;
                    if(!lines[2].empty() && !lines[1].empty())
                    {
                        if(lines[1].size() >= one_column) wrap1 = true;
                        else lines[1].resize(one_column, ' ');
                    }

                    if(wrap1 && !lines[2].empty())
                    {
                        lines[2].swap(streams_wrap_buf[2]);
                    }
                }

                std::cout << colors[0] << lines[0];
                if(!lines[1].empty())
                    std::cout << wall << colors[1] << lines[1];
                if(!lines[2].empty())
                    std::cout << wall << colors[2] << lines[2];
                std::cout << newline;
            }
            std::cout << SEPARATOR << std::endl;
        }
#endif
    }

    void printFunctionTimings(std::vector<FunctionInfo>& functions)
    {
        std::printf
        ("    ,------------------------------------------------------------------------,\n"
         "    |      Parse |      Eval |  Eval (O) | Eval (O2) |  Optimize  | Repeat O.|\n"
         ",---+------------+-----------+-----------+-----------+------------+----------+\n");
        for(size_t i = 0; i < functions.size(); ++i)
        {
            getTimingInfo(functions[i]);
            std::printf("|%2u | %10.3f |%10.3f |%10.3f |%10.3f |%11.1f |%10.1f|\n",
                        unsigned(i+1),
                        functions[i].mParseTiming.mMicroSeconds,
                        functions[i].mEvalTiming.mMicroSeconds,
                        functions[i].mOptimizedEvalTiming.mMicroSeconds,
                        functions[i].mDoubleOptimizedEvalTiming.mMicroSeconds,
                        functions[i].mOptimizeTiming.mMicroSeconds,
                        functions[i].mDoubleOptimizeTiming.mMicroSeconds
                       );
        }
        std::printf
        ("'----------------------------------------------------------------------------'\n");
    }

    bool checkFunctionValidity(FunctionInfo& info)
    {
        int result = info.mParser.Parse(info.mFunctionString, gVarString, gUseDegrees);
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
            int oldIndex = -1;

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

                int index = parser.Parse(funcStr, gVarString, gUseDegrees);
                if(index < 0) break;
                if(index == oldIndex) return;

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
                    return;

                varNames.insert(funcStr.substr(index, index2-index));
                oldIndex = index;
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
        else if(std::strcmp(argv[i], "-deg") == 0)
            gUseDegrees = true;
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
    {
        if(!checkFunctionValidity(functions[i]))
            return 1;
    }

    const bool validVarValuesFound = findValidVarValues(functions);

    std::cout << SEPARATOR << std::endl;
    for(size_t i = 0; i < functions.size(); ++i)
        std::cout << "- Function " << i+1 << ": \""
                  << functions[i].mFunctionString << "\"\n";
    const size_t varsAmount = gVarValues[0].size();
    const size_t varValueSetsAmount = gVarValues.size();
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
