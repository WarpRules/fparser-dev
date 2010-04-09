/*==========================================================================
  functioninfo
  ------------
  Copyright: Juha Nieminen, Joel Yliluoma
  This program (functioninfo) is distributed under the terms of the
  GNU General Public License (GPL) version 3.
  See gpl.txt for the license text.
============================================================================*/

#include "fparser.hh"
#include "fparser_gmpint.hh"
#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <string>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <sstream>
#include <cassert>

#define SEPARATOR \
"----------------------------------------------------------------------------"

namespace
{
    const unsigned kTestTime = 250; // In milliseconds
    const unsigned kParseLoopsPerUnit = 100000;
    const unsigned kEvalLoopsPerUnit = 300000;
    const unsigned kOptimizeLoopsPerUnit = 1000;
    const bool kPrintTimingProgress = false;

    const unsigned kMaxVarValueSetsAmount = 10000;
    const double kVarValuesUpperLimit = 100000.0;
    const double kVarValuesInitialDelta = 0.1;
    const double kVarValuesDeltaFactor1 = 1.25;
    const double kVarValuesDeltaFactor2 = 10.0;
    const double kVarValuesDeltaFactor2Threshold = 10.0;

    template<typename Value_t> Value_t epsilon() { return Value_t(1e-9); }
    template<> inline float epsilon<float>() { return 1e-5F; }
    template<> inline long epsilon<long>() { return 0; }

#ifdef FP_SUPPORT_GMP_INT_TYPE
    template<> inline GmpInt epsilon<GmpInt>() { return 0; }
#endif

    template<typename Value_t>
    Value_t Sqr(const Value_t* p)
    {
        return p[0]*p[0];
    }

    template<typename Value_t>
    Value_t Sub(const Value_t* p)
    {
        return p[0]-p[1];
    }

    template<typename Value_t>
    Value_t Value(const Value_t*)
    {
        return Value_t(10);
    }

    template<typename Value_t>
    class InitializableParser: public FunctionParserBase<Value_t>
    {
     public:
        InitializableParser(const char* function, const char* vars)
        {
            assert(FunctionParserBase<Value_t>::Parse(function, vars) < 0);
        }
    };


    template<typename Value_t>
    class ParserWithConsts: public FunctionParserBase<Value_t>
    {
     public:
        ParserWithConsts()
        {
            AddConstant("pi", Value_t(3.14159265358979323846));
            AddConstant("e", Value_t(2.71828182845904523536));
            AddUnit("k", Value_t(1000));
            AddUnit("M", Value_t(1000000));
            AddUnit("dozen", Value_t(12));
            AddUnit("dozens", Value_t(12));

            AddFunction("sqr", Sqr<Value_t>, 1);
            AddFunction("sub", Sub<Value_t>, 2);
            AddFunction("value", Value<Value_t>, 0);

            static InitializableParser<Value_t> SqrFun("x*x", "x");
            static InitializableParser<Value_t> SubFun("x-y", "x,y");
            static InitializableParser<Value_t> ValueFun("5", "");

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

    template<typename Value_t>
    struct FunctionInfo
    {
        std::string mFunctionString;
        ParserWithConsts<Value_t> mParser;
        std::vector<Value_t> mValidVarValues;

        TimingInfo mParseTiming;
        TimingInfo mEvalTiming;
        TimingInfo mOptimizeTiming;
        TimingInfo mDoubleOptimizeTiming;
        TimingInfo mOptimizedEvalTiming;
        TimingInfo mDoubleOptimizedEvalTiming;
    };


    template<typename Value_t>
    struct ParserData
    {
        static ParserWithConsts<Value_t> gParser, gAuxParser;
        static std::vector<std::vector<Value_t> > gVarValues;
        static const Value_t* gEvalParameters;
    };

    template<typename Value_t>
    ParserWithConsts<Value_t> ParserData<Value_t>::gParser;

    template<typename Value_t>
    ParserWithConsts<Value_t> ParserData<Value_t>::gAuxParser;

    template<typename Value_t>
    std::vector<std::vector<Value_t> > ParserData<Value_t>::gVarValues;

    template<typename Value_t>
    const Value_t* ParserData<Value_t>::gEvalParameters = 0;


    std::string gFunctionString, gVarString;
    bool gUseDegrees = false;

    template<typename Value_t>
    inline void doParse()
    {
        ParserData<Value_t>::gParser.Parse
            (gFunctionString, gVarString, gUseDegrees);
    }

    template<typename Value_t>
    inline void doEval()
    {
        ParserData<Value_t>::gParser.Eval
            (ParserData<Value_t>::gEvalParameters);
    }

    template<typename Value_t>
    inline void doOptimize()
    {
        ParserData<Value_t>::gAuxParser = ParserData<Value_t>::gParser;
        ParserData<Value_t>::gAuxParser.Optimize();
    }

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

    template<typename Value_t>
    void getTimingInfo(FunctionInfo<Value_t>& info)
    {
        gFunctionString = info.mFunctionString;
        ParserData<Value_t>::gEvalParameters = &info.mValidVarValues[0];

        printTimingInfo();
        info.mParseTiming =
            getTimingInfo<doParse<Value_t>, kParseLoopsPerUnit>();

        printTimingInfo();
        info.mEvalTiming =
            getTimingInfo<doEval<Value_t>, kEvalLoopsPerUnit>();

        printTimingInfo();
        info.mOptimizeTiming = // optimizing a non-optimized func
            getTimingInfo<doOptimize<Value_t>, kOptimizeLoopsPerUnit>();

        printTimingInfo();
        ParserData<Value_t>::gParser.Optimize();
        info.mDoubleOptimizeTiming = // optimizing an already-optimized func
            getTimingInfo<doOptimize<Value_t>, kOptimizeLoopsPerUnit>();

        printTimingInfo();
        info.mOptimizedEvalTiming = // evaluating an optimized func
            getTimingInfo<doEval<Value_t>, kEvalLoopsPerUnit>();

        printTimingInfo();
        ParserData<Value_t>::gParser.Optimize();
        info.mDoubleOptimizedEvalTiming = // evaluating a twice-optimized func
            getTimingInfo<doEval<Value_t>, kEvalLoopsPerUnit>();
    }

    template<typename Value_t>
    inline bool valueIsOk(Value_t value)
    {
        return !(value < -1e14 || value > 1e14);
    }

    template<>
    inline bool valueIsOk<long>(long) { return true; }

#ifdef FP_SUPPORT_GMP_INT_TYPE
    template<>
    inline bool valueIsOk<GmpInt>(GmpInt) { return true; }
#endif

    template<typename Value_t>
    bool findValidVarValues(std::vector<FunctionInfo<Value_t> >& functions)
    {
        unsigned varsAmount = 1;
        for(size_t i = 0; i < gVarString.length(); ++i)
            if(gVarString[i] == ',')
                ++varsAmount;

        std::vector<Value_t> varValues(varsAmount, 0);
        std::vector<double> doubleValues(varsAmount, 0);
        std::vector<double> deltas(varsAmount, kVarValuesInitialDelta);

        for(size_t i = 0; i < functions.size(); ++i)
            functions[i].mValidVarValues = varValues;

        while(true)
        {
            for(unsigned i = 0; i < varsAmount; ++i)
                varValues[i] = Value_t(doubleValues[i]);

            bool wasOk = false;
            for(size_t i = 0; i < functions.size(); ++i)
            {
                Value_t value = functions[i].mParser.Eval(&varValues[0]);
                if(functions[i].mParser.EvalError() == 0 && valueIsOk(value))
                {
                    functions[i].mValidVarValues = varValues;
                    wasOk = true;
                }
            }

            if(wasOk)
            {
                ParserData<Value_t>::gVarValues.push_back(varValues);
                if(ParserData<Value_t>::gVarValues.size() >=
                   kMaxVarValueSetsAmount)
                    return true;
            }

            size_t varIndex = 0;
            while(true)
            {
                doubleValues[varIndex] = -doubleValues[varIndex];
                if(doubleValues[varIndex] < 0.0)
                    break;

                doubleValues[varIndex] += deltas[varIndex];
                if(deltas[varIndex] < kVarValuesDeltaFactor2Threshold)
                    deltas[varIndex] *= kVarValuesDeltaFactor1;
                else
                    deltas[varIndex] *= kVarValuesDeltaFactor2;

                if(doubleValues[varIndex] <= kVarValuesUpperLimit)
                    break;
                else
                {
                    doubleValues[varIndex] = 0.0;
                    deltas[varIndex] = kVarValuesInitialDelta;
                    if(++varIndex == doubleValues.size())
                    {
                        if(ParserData<Value_t>::gVarValues.empty())
                        {
                            ParserData<Value_t>::gVarValues.push_back
                                (std::vector<Value_t>(varsAmount, 0));
                            return false;
                        }
                        return true;
                    }
                }
            }
        }
    }

    template<typename Value_t>
    inline Value_t fpAbs(Value_t value)
    {
        return value < Value_t(0) ? -value : value;
    }

    template<typename Value_t>
    inline Value_t scaledDiff(Value_t v1, Value_t v2)
    {
        const double scale = pow(10.0, floor(log10(double(fpAbs(v1)))));
        const Value_t sv1 =
            fpAbs(v1) < epsilon<Value_t>() ? 0 : Value_t(v1/scale);
        const Value_t sv2 =
            fpAbs(v2) < epsilon<Value_t>() ? 0 : Value_t(v2/scale);
        return sv2 - sv1;
    }

    template<>
    inline long scaledDiff<long>(long v1, long v2)
    {
        return v2 - v1;
    }

#ifdef FP_SUPPORT_GMP_INT_TYPE
    template<>
    inline GmpInt scaledDiff<GmpInt>(GmpInt v1, GmpInt v2)
    {
        return v2 - v1;
    }
#endif

    template<typename Value_t>
    inline bool notEqual(Value_t v1, Value_t v2)
    {
        return fpAbs(scaledDiff(v1, v2)) > epsilon<Value_t>();
    }

    template<>
    inline bool notEqual<long>(long v1, long v2)
    {
        return v1 != v2;
    }

#ifdef FP_SUPPORT_GMP_INT_TYPE
    template<>
    inline bool notEqual<GmpInt>(GmpInt v1, GmpInt v2)
    {
        return v1 != v2;
    }
#endif

    template<typename Value_t>
    bool compareFunctions(size_t function1Index, size_t function2Index,
                          ParserWithConsts<Value_t>& parser1,
                          const char* parser1Type,
                          ParserWithConsts<Value_t>& parser2,
                          const char* parser2Type)
    {
        const size_t varsAmount = ParserData<Value_t>::gVarValues[0].size();
        for(size_t varSetInd = 0;
            varSetInd < ParserData<Value_t>::gVarValues.size();
            ++varSetInd)
        {
            const Value_t* values =
                &ParserData<Value_t>::gVarValues[varSetInd][0];
            const Value_t v1 = parser1.Eval(values);
            const Value_t v2 = parser2.Eval(values);

            if(notEqual(v1, v2))
            {
                if(parser1.EvalError() && parser1Type[0] == 'n')
                {
                    // If the source expression returns an error,
                    // ignore this "failure"
                    continue;
                }

                std::cout << SEPARATOR << "\n******* For variable values (";
                for(size_t i = 0; i < varsAmount; ++i)
                {
                    if(i > 0) std::cout << ",";
                    std::cout << values[i];
                }
                std::cout << ")\n";
                std::cout << "******* function " << function1Index+1
                          << " (" << parser1Type << ") returned ";
                if(parser1.EvalError())
                    std::cout << "error " << parser1.EvalError();
                else
                    std::cout << std::setprecision(18) << v1;
                std::cout << "\n";
                std::cout << "******* function " << function2Index+1
                          << " (" << parser2Type << ") returned ";
                if(parser2.EvalError())
                    std::cout << "error " << parser2.EvalError();
                else
                    std::cout << std::setprecision(18) << v2;
                std::cout << "\n******* (Difference: " << (v2-v1)
                          << ", scaled diff: "
                          << std::setprecision(18) << scaledDiff(v1, v2)
                          << ")" << std::endl;
                return false;
            }
        }
        return true;
    }

    bool had_double_optimization_problems = false;

    template<typename Value_t>
    bool checkEquality(const std::vector<FunctionInfo<Value_t> >& functions)
    {
        static const char not_optimized[] = "not optimized";
        static const char optimized[]     = "optimized";
        static const char optimized2[]    = "double-optimized";
        static const char* const optimize_labels[3] =
            { not_optimized, optimized, optimized2 };

        ParserWithConsts<Value_t> parser1, parser2, parser3;

        bool errors = false;
        for(size_t ind1 = 0; ind1 < functions.size(); ++ind1)
        {
            parser1.Parse
                (functions[ind1].mFunctionString, gVarString, gUseDegrees);
            parser2.Parse
                (functions[ind1].mFunctionString, gVarString, gUseDegrees);
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
                parser1.Parse(functions[ind1].mFunctionString, gVarString,
                              gUseDegrees);
                for(int n_optimizes1 = 0; n_optimizes1 <= 2; ++n_optimizes1)
                {
                    if(errors) break;
                    if(n_optimizes1 > 0) parser1.Optimize();

                    parser2.Parse(functions[ind2].mFunctionString, gVarString,
                                  gUseDegrees);

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

    template<typename Value_t>
    void printByteCodes(const std::vector<FunctionInfo<Value_t> >& functions,
                        PrintMode mode = print_no_cut_or_wrap)
    {
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
        ParserWithConsts<Value_t> parser;
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
            std::cout << SEPARATOR << std::endl;

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
                        else if(lines[0].size() < two_columns)
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
        }
#endif
    }

    template<typename Value_t>
    void printFunctionTimings(std::vector<FunctionInfo<Value_t> >& functions)
    {
        std::printf
        ("    ,------------------------------------------------------------------------,\n"
         "    |      Parse |      Eval |  Eval (O) | Eval (O2) |  Optimize |  Repeat O.|\n"
         ",---+------------+-----------+-----------+-----------+-----------+-----------+\n");
        for(size_t i = 0; i < functions.size(); ++i)
        {
            getTimingInfo(functions[i]);
            std::printf
                ("|%2u | %10.3f |%10.3f |%10.3f |%10.3f |%10.1f |%10.1f |\n",
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

    template<typename Value_t>
    bool checkFunctionValidity(FunctionInfo<Value_t>& info)
    {
        int result = info.mParser.Parse(info.mFunctionString, gVarString,
                                        gUseDegrees);
        if(result >= 0)
        {
            std::cerr << "\"" << info.mFunctionString << "\"\n"
                      << std::string(result+1, ' ')
                      << "^ " << info.mParser.ErrorMsg() << std::endl;
            if(info.mParser.GetParseErrorType() ==
               FunctionParserBase<Value_t>::INVALID_VARS)
                std::cerr << "Vars: \"" << gVarString << "\"" << std::endl;
            return false;
        }
        return true;
    }

    template<typename Value_t>
    void deduceVariables(const std::vector<FunctionInfo<Value_t> >& functions)
    {
        typedef std::set<std::string> StrSet;
        StrSet varNames;
        ParserWithConsts<Value_t> parser;

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
            "  -f                 : Use FunctionParser_f.\n"
            "  -ld                : Use FunctionParser_ld.\n"
            "  -li                : Use FunctionParser_li.\n"
            "  -gi                : Use FunctionParser_gmpint.\n"
            "  -vars <var string> : Specify a var string.\n"
            "  -nt                : No timing measurements.\n"
            "  -ntd               : No timing if functions differ.\n"
            "  -deg               : Use degrees for trigonometry.\n";
        return 1;
    }
}

template<typename Value_t>
int functionInfo(const char* const parserTypeString,
                 const std::vector<std::string>& functionStrings,
                 bool measureTimings, bool noTimingIfEqualityErrors)
{
    std::vector<FunctionInfo<Value_t> > functions(functionStrings.size());
    for(size_t i = 0; i < functions.size(); ++i)
        functions[i].mFunctionString = functionStrings[i];

    if(gVarString.empty())
        deduceVariables(functions);

    for(size_t i = 0; i < functions.size(); ++i)
    {
        if(!checkFunctionValidity(functions[i]))
            return 1;
    }

    const bool validVarValuesFound = findValidVarValues(functions);

    std::cout << SEPARATOR << std::endl
              << "Parser type: " << parserTypeString << std::endl;
    for(size_t i = 0; i < functions.size(); ++i)
        std::cout << "- Function " << i+1 << ": \""
                  << functions[i].mFunctionString << "\"\n";
    const size_t varsAmount = ParserData<Value_t>::gVarValues[0].size();
    const size_t varValueSetsAmount = ParserData<Value_t>::gVarValues.size();
    std::cout << "- Var string: \"" << gVarString << "\" ("
              << ParserData<Value_t>::gVarValues[0].size()
              << (varsAmount == 1 ? " var" : " vars")
              << ") (using " << varValueSetsAmount << " set"
              << (varValueSetsAmount == 1 ? ")\n" : "s)\n");

#if 0
    std::cout << SEPARATOR << "\nTesting with variable values:\n";
    for(size_t i = 0; i < ParserData<Value_t>::gVarValues.size(); ++i)
    {
        if(i > 0) std::cout << (i%5==0 ? "\n" : " ");
        std::cout << "(";
        for(size_t j = 0; j < ParserData<Value_t>::gVarValues[i].size(); ++j)
        {
            if(j > 0) std::cout << ",";
            std::cout << ParserData<Value_t>::gVarValues[i][j];
        }
        std::cout << ")";
    }
    if(!validVarValuesFound)
        std::cout << " [no valid variable values were found...]";
    std::cout << "\n" << SEPARATOR << std::endl;
#else
    if(!validVarValuesFound)
        std::cout << SEPARATOR
                  << "\nWarning: No valid variable values were found."
                  << " Using (0,0)." << std::endl;
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

int main(int argc, char* argv[])
{
    if(argc < 2) return printHelp(argv[0]);

    enum ParserType { FP_D, FP_F, FP_LD, FP_LI, FP_GI };

    std::vector<std::string> functionStrings;
    bool measureTimings = true, noTimingIfEqualityErrors = false;
    ParserType parserType = FP_D;

    for(int i = 1; i < argc; ++i)
    {
        if(std::strcmp(argv[i], "-f") == 0) parserType = FP_F;
        else if(std::strcmp(argv[i], "-ld") == 0) parserType = FP_LD;
        else if(std::strcmp(argv[i], "-li") == 0) parserType = FP_LI;
        else if(std::strcmp(argv[i], "-gi") == 0) parserType = FP_GI;
        else if(std::strcmp(argv[i], "-vars") == 0)
        {
            if(++i == argc) return printHelp(argv[0]);
            gVarString = argv[i];
        }
        else if(std::strcmp(argv[i], "-nt") == 0)
            measureTimings = false;
        else if(std::strcmp(argv[i], "-ntd") == 0)
            noTimingIfEqualityErrors = true;
        else if(std::strcmp(argv[i], "-deg") == 0)
            gUseDegrees = true;
        else if(std::strcmp(argv[i], "--help") == 0
             || std::strcmp(argv[i], "-help") == 0
             || std::strcmp(argv[i], "-h") == 0
             || std::strcmp(argv[i], "/?") == 0)
            printHelp(argv[0]);
        else
            functionStrings.push_back(argv[i]);
    }

    if(functionStrings.empty()) return printHelp(argv[0]);

    const char* notCompiledParserType = 0;

    switch(parserType)
    {
      case FP_D:
#ifndef FP_DISABLE_DOUBLE_TYPE
          return functionInfo<double>
              ("double", functionStrings,
               measureTimings, noTimingIfEqualityErrors);
#else
          notCompiledParserType = "double";
          break;
#endif

      case FP_F:
#ifdef FP_SUPPORT_FLOAT_TYPE
          return functionInfo<float>
              ("float", functionStrings,
               measureTimings, noTimingIfEqualityErrors);
#else
          notCompiledParserType = "float";
          break;
#endif

      case FP_LD:
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
          return functionInfo<long double>
              ("long double", functionStrings,
               measureTimings, noTimingIfEqualityErrors);
#else
          notCompiledParserType = "long double";
          break;
#endif

      case FP_LI:
#ifdef FP_SUPPORT_LONG_INT_TYPE
          return functionInfo<long int>
              ("long int", functionStrings,
               measureTimings, noTimingIfEqualityErrors);
#else
          notCompiledParserType = "long int";
          break;
#endif

      case FP_GI:
#ifdef FP_SUPPORT_GMP_INT_TYPE
          return functionInfo<GmpInt>
              ("GmpInt", functionStrings,
               measureTimings, noTimingIfEqualityErrors);
#else
          notCompiledParserType = "GmpInt";
          break;
#endif
    }

    if(notCompiledParserType)
    {
        std::cout << "Error: Support for type " << notCompiledParserType
                  << " was not compiled in." << std::endl;
        return 1;
    }
    return 0;
}
