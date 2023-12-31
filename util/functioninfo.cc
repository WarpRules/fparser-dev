/*==========================================================================
  functioninfo
  ------------
  Copyright: Juha Nieminen, Joel Yliluoma
  This program (functioninfo) is distributed under the terms of the
  GNU General Public License (GPL) version 3.
  See gpl.txt for the license text.
============================================================================*/

static const char* const kVersionNumber = "1.3.0.4";

#include "fparser.hh"
#include "fparser_mpfr.hh"
#include "fparser_gmpint.hh"
#include "extrasrc/fpaux.hh"
#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <functional>
#include <cctype>
#include "extrasrc/testbed_types.hh"

static const char SEPARATOR[] =
"----------------------------------------------------------------------------";

/* Prints a value to the given stream, using the given precision.
   In this case "precision" refers to the amount of most-significant decimal digits.
   For example, printing different values of type double with a precision of 15 (ie.
   15 most-significant digits of the value will be printed), will result in eg:

     1.23456789012345
     12.3456789012345
     12345.6789012345
     12345678901.2345
     -1234567.89012345

   If the integer portion of the value contains more digits than the specified precision,
   then that many most-significant digits will be printed and zeros appended.

     123456789012345000000
*/
template<typename Value_t>
static void printValueWithPrecision(std::ostream& os, const Value_t& value, int precision)
{
    using namespace FUNCTIONPARSERTYPES;
    const int integer_part_digits = 1 +
        static_cast<int>(makeLongInteger(fp_log10(fp_abs(value))));

    if(integer_part_digits <= precision)
        precision -= integer_part_digits;
    else
        precision = 0;

    std::ios_base::fmtflags flags = os.flags();
    os << std::fixed << std::setprecision(precision) << value;
    os.flags(flags);
}

template<typename Value_t>
struct ValueWithPrecisionPrinter
{
    const Value_t& value;
    int precision;
    ValueWithPrecisionPrinter(const Value_t& v, int p): value(v), precision(p) {}
};

template<typename Value_t>
std::ostream& operator<<(std::ostream& os, const ValueWithPrecisionPrinter<Value_t>& printer)
{
    printValueWithPrecision(os, printer.value, printer.precision);
    return os;
}

[[maybe_unused]]
static void testPrintValueWithPrecision()
{
    const struct { double value; const char *expectedOutput; }
    testData[] =
    {
        {     0.001, "0.00100000000000000" },
        {      0.01, "0.0100000000000000" },
        {       0.1, "0.100000000000000" },
        {       1.0, "1.00000000000000" },
        {      10.0, "10.0000000000000" },
        {     100.0, "100.000000000000" },
        {    1000.0, "1000.00000000000" },
        {   10000.0, "10000.0000000000" },
        {  100000.0, "100000.000000000" },
        { 1000000.0, "1000000.00000000" },
        { 0.00123456789012345, "0.00123456789012345" },
        { 0.0123456789012345, "0.0123456789012345" },
        { 0.123456789012345, "0.123456789012345" },
        { 1.23456789012345, "1.23456789012345" },
        { 12.3456789012345, "12.3456789012345" },
        { 123.456789012345, "123.456789012345" },
        { 1234.56789012345, "1234.56789012345" },
        { 12345.6789012345, "12345.6789012345" },
        { 123456.789012345, "123456.789012345" },
    };

    std::ostringstream outputStream;
    for(const auto& testDataValues: testData)
    {
        outputStream.str("");
        printValueWithPrecision(outputStream, testDataValues.value, 15);
        if(outputStream.str() != testDataValues.expectedOutput)
        {
            std::cerr << "INTERNAL ERROR: For value " << testDataValues.value
                      << " printValueWithPrecision() resulted in:\n| " << outputStream.str()
                      << "\ninstead of expected:\n| " << testDataValues.expectedOutput << "\n";
        }
    }
}

namespace
{
    template<typename Value_t>
    struct TimingConst
    {
        static const unsigned kParseLoopsPerUnit = 100000*15;
        static const unsigned kEvalLoopsPerUnit = 300000*15;
        static const unsigned kOptimizeLoopsPerUnit = 1000*15;
    };

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    template<>
    struct TimingConst<MpfrFloat>
    {
        static const unsigned kParseLoopsPerUnit = 100000*15;
        static const unsigned kEvalLoopsPerUnit = 10000*15;
        static const unsigned kOptimizeLoopsPerUnit = 500*15;
    };
#endif

    const unsigned kTestTime = 250; // In milliseconds
    const bool kPrintTimingProgress = false;

    const unsigned kMaxVarValueSetsAmount = 10000;
    const double kVarValuesUpperLimit = 100000.0;
    const double kVarValuesInitialDelta = 0.1;
    const double kVarValuesDeltaFactor1 = 1.25;
    const double kVarValuesDeltaFactor2 = 10.0;
    const double kVarValuesDeltaFactor2Threshold = 10.0;

#ifndef __MINGW32__
    const bool color_support = true;
#else
    const bool color_support = false;
#endif

    bool gIgnoreErrors = false;
    bool gOnlySuppliedValues = false;
    bool gPrintByteCodeExpressions = true;
    int gVerbosityLevel = 1;

    template<typename Value_t> Value_t epsilon() { return Value_t(1e-9); }
    template<> inline float epsilon<float>() { return 1e-5F; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    template<> inline MpfrFloat epsilon<MpfrFloat>()
    { return MpfrFloat::someEpsilon(); }
#endif

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
            typedef FunctionParserBase<Value_t> b;
            b::AddConstant("pi", FUNCTIONPARSERTYPES::fp_const_pi<Value_t>());
            b::AddConstant("e", FUNCTIONPARSERTYPES::fp_const_e<Value_t>());
            b::AddUnit("k", Value_t(1000));
            b::AddUnit("M", Value_t(1000000));
            b::AddUnit("dozen", Value_t(12));
            b::AddUnit("dozens", Value_t(12));

            b::AddFunction("sqr",   [](const Value_t*p){ return p[0]*p[0];   }, 1);
            b::AddFunction("sub",   [](const Value_t*p){ return p[0]-p[1];   }, 2);
            b::AddFunction("value", [](const Value_t* ){ return Value_t{10}; }, 0);

            static InitializableParser<Value_t> SqrFun("x*x", "x");
            static InitializableParser<Value_t> SubFun("x-y", "x,y");
            static InitializableParser<Value_t> ValueFun("5", "");

            b::AddFunction("psqr", SqrFun);
            b::AddFunction("psub", SubFun);
            b::AddFunction("pvalue", ValueFun);
        }

        // Publicize fparser's parsing functions
        using FunctionParserBase<Value_t>::ParseLiteral;
        using FunctionParserBase<Value_t>::ParseIdentifier;
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

    TimingInfo getTimingInfo(unsigned loopsPerUnit, void(*Function)())
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
    std::size_t gTimingTotalCount;

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

        using timings = TimingConst<Value_t>;
        auto doParse = []
        {
            ParserData<Value_t>::gParser.Parse
                (gFunctionString, gVarString, gUseDegrees);
        };
        auto doEval = []
        {
            ParserData<Value_t>::gParser.Eval
                (ParserData<Value_t>::gEvalParameters);
        };
        auto doOptimize = []
        {
            ParserData<Value_t>::gAuxParser = ParserData<Value_t>::gParser;
            ParserData<Value_t>::gAuxParser.Optimize();
        };

        printTimingInfo();
        info.mParseTiming = getTimingInfo(timings::kParseLoopsPerUnit, doParse);

        printTimingInfo();
        info.mEvalTiming = getTimingInfo(timings::kEvalLoopsPerUnit, doEval);

        printTimingInfo();
        info.mOptimizeTiming = // optimizing a non-optimized func
            getTimingInfo(timings::kOptimizeLoopsPerUnit, doOptimize);

        printTimingInfo();
        ParserData<Value_t>::gParser.Optimize();
        info.mDoubleOptimizeTiming = // optimizing an already-optimized func
            getTimingInfo(timings::kOptimizeLoopsPerUnit, doOptimize);

        printTimingInfo();
        info.mOptimizedEvalTiming = // evaluating an optimized func
            getTimingInfo(timings::kEvalLoopsPerUnit, doEval);

        printTimingInfo();
        ParserData<Value_t>::gParser.Optimize();
        info.mDoubleOptimizedEvalTiming = // evaluating a twice-optimized func
            getTimingInfo(timings::kEvalLoopsPerUnit, doEval);
    }

    template<typename Value_t>
    inline bool valueIsOk(const Value_t& value)
    {
        using namespace FUNCTIONPARSERTYPES;
        if(IsIntType<Value_t>::value)
        {
            return true;
        }
        Value_t limit(10000000); limit *= limit; // Makes 1e14
        return fp_less(fp_abs(value), fp_real(limit));
    }

    template<typename Value_t>
    std::vector<Value_t> findImmeds(const std::vector<FunctionInfo<Value_t> >& functions)
    {
        using parser = ParserWithConsts<Value_t>;
        std::vector<Value_t> result;

        for(std::size_t a=0; a<functions.size(); ++a)
        {
            const std::string& functionString = functions[a].mFunctionString;
            const char* function = functionString.c_str();
            std::size_t len = functionString.size();

            for(std::size_t pos=0; pos<len; )
            {
                std::pair<const char*, Value_t>
                    literal = parser::ParseLiteral(function+pos);
                if(literal.first != (function+pos))
                {
                    result.push_back(literal.second);
                    result.push_back(-literal.second);
                    pos = literal.first - function;
                    continue;
                }
                unsigned identifier = parser::ParseIdentifier(function);

                unsigned skip_length = identifier & 0xFFFF;
                if(skip_length == 0) skip_length = 1;
                pos += skip_length;
            }
        }

        std::sort(result.begin(), result.end(),
                  [](const Value_t& a, const Value_t& b)
                      { return FUNCTIONPARSERTYPES::fp_less(a,b); });
        result.erase(std::unique(result.begin(), result.end()), result.end());
        return result;
    }

    template<typename Value_t>
    double makeDoubleFrom(const Value_t& v)
    {
        /* FIXME: Why is this function needed?
         * Why does findValidVarValues() use "double" datatype?
         */
        return double(v);
    }

#ifdef FP_SUPPORT_GMP_INT_TYPE
    template<>
    double makeDoubleFrom(const GmpInt& v)
    {
        return v.toInt();
    }
#endif

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    template<>
    double makeDoubleFrom(const MpfrFloat& v)
    {
        return v.toDouble();
    }
#endif

    template<typename Value_t>
    std::vector<Value_t> parseUserGivenVarValues(const std::string& str)
    {
        using parser = ParserWithConsts<Value_t>;
        std::vector<Value_t> values;

        const char* ptr = str.c_str();
        while(true)
        {
            while(std::isspace(*ptr)) { ++ptr; }
            std::pair<const char*, Value_t> literal = parser::ParseLiteral(ptr);
            auto endptr = literal.first;
            auto& value = literal.second;
            if(FUNCTIONPARSERTYPES::IsComplexType<Value_t>::value
            && (*endptr == '+' || *endptr == '-'))
            {
                ptr = endptr;
                if(*ptr == '+') ++ptr;
                std::pair<const char*, Value_t> literal2 = parser::ParseLiteral(ptr);
                endptr = literal2.first;
                value += literal2.second;
            }
            if(endptr == ptr || !endptr) break;

            values.push_back( std::move(value) );
            ptr = endptr;
        }

        return values;
    }

    template<typename Value_t, bool IsComplexType=FUNCTIONPARSERTYPES::IsComplexType<Value_t>::value>
    struct findValidVarValuesAux
    {
        /* TODO: Add comments to this code explaining what it's doing
         */
        static bool find(std::vector<FunctionInfo<Value_t> >& functions,
                         const std::string& userGivenVarValuesString)
        {
            unsigned varsAmount = 1;
            for(std::size_t i = 0; i < gVarString.length(); ++i)
                if(gVarString[i] == ',')
                    ++varsAmount;

            std::vector<Value_t> userGivenVarValues;
            if(!userGivenVarValuesString.empty())
            {
                userGivenVarValues =
                    parseUserGivenVarValues<Value_t>(userGivenVarValuesString);
                if(userGivenVarValues.size() != varsAmount)
                {
                    std::cout << "Warning: Wrong amount of values specified with "
                        "-varValues. Ignoring." << std::endl;
                    userGivenVarValues.clear();
                }
            }
            if(userGivenVarValues.empty() && gOnlySuppliedValues)
            {
                std::cout << "Warning: -only option used without -varValues. Ignoring -only." << std::endl;
                gOnlySuppliedValues = false;
            }

            std::vector<Value_t> varValues(varsAmount, Value_t());
            std::vector<double> doubleValues(varsAmount, 0);
            std::vector<double> deltas(varsAmount, kVarValuesInitialDelta);

            std::vector<Value_t> immedList = findImmeds(functions);

            if(userGivenVarValues.empty())
            {
                for(std::size_t i = 0; i < functions.size(); ++i)
                    functions[i].mValidVarValues = varValues;
            }
            else
            {
                for(std::size_t i = 0; i < functions.size(); ++i)
                    functions[i].mValidVarValues = userGivenVarValues;
                ParserData<Value_t>::gVarValues.push_back(userGivenVarValues);
            }

            if(gOnlySuppliedValues) return true;

            std::vector<std::size_t> immedCounter(varsAmount, 0);

            while(true)
            {
                for(unsigned i = 0; i < varsAmount; ++i)
                {
                    /* FIXME: These values are not precise */
                    varValues[i] = FUNCTIONPARSERTYPES::fp_const_preciseDouble<Value_t>(doubleValues[i]);
                }

                bool wasOk = false;
                for(std::size_t i = 0; i < functions.size(); ++i)
                {
                    Value_t value = functions[i].mParser.Eval(&varValues[0]);
                    if(functions[i].mParser.EvalError() == 0 && valueIsOk(value))
                    {
                        if(userGivenVarValues.empty())
                            functions[i].mValidVarValues = varValues;
                        wasOk = true;
                    }
                }

                if(wasOk)
                {
                    ParserData<Value_t>::gVarValues.push_back(varValues);
                    if(ParserData<Value_t>::gVarValues.size() >=
                       kMaxVarValueSetsAmount || gOnlySuppliedValues)
                        return true;
                }

                std::size_t varIndex = 0;
                while(true)
                {
                    if(immedCounter[varIndex] == 0)
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
                    }

                    if(immedCounter[varIndex] < immedList.size())
                    {
                        std::size_t& i = immedCounter[varIndex];
                        doubleValues[varIndex] = makeDoubleFrom(immedList[i]);
                        i += 1;
                        break;
                    }

                    immedCounter[varIndex] = 0;
                    doubleValues[varIndex] = 0.0;
                    deltas[varIndex] = kVarValuesInitialDelta;
                    if(++varIndex == doubleValues.size())
                    {
                        if(ParserData<Value_t>::gVarValues.empty())
                        {
                            ParserData<Value_t>::gVarValues.push_back(std::vector<Value_t>(varsAmount, Value_t()));
                            return false;
                        }
                        return true;
                    }
                }
            }
        }
    };

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename Value_t>
    struct findValidVarValuesAux<Value_t, true>
    {
        /* Same as above, but for complex numbers */

        static double makeDouble1From(const Value_t& v)
        {
            return makeDoubleFrom(v.real());
        }
        static double makeDouble2From(const Value_t& v)
        {
            return makeDoubleFrom(v.imag());
        }

        static bool find(std::vector<FunctionInfo<Value_t> >& functions,
                         const std::string& userGivenVarValuesString)
        {
            unsigned varsAmount = 1;
            for(std::size_t i = 0; i < gVarString.length(); ++i)
                if(gVarString[i] == ',')
                    ++varsAmount;

            std::vector<Value_t> userGivenVarValues;
            if(!userGivenVarValuesString.empty())
            {
                userGivenVarValues =
                    parseUserGivenVarValues<Value_t>(userGivenVarValuesString);
                if(userGivenVarValues.size() != varsAmount)
                {
                    std::cout << "Warning: Wrong amount of values specified with "
                        "-varValues. Ignoring." << std::endl;
                    userGivenVarValues.clear();
                }
            }
            if(userGivenVarValues.empty() && gOnlySuppliedValues)
            {
                std::cout << "Warning: -only option used without -varValues. Ignoring -only." << std::endl;
                gOnlySuppliedValues = false;
            }

            const unsigned valuesAmount = varsAmount*2;

            std::vector<Value_t> varValues(varsAmount, 0);
            std::vector<double> doubleValues(valuesAmount, 0);
            std::vector<double> deltas(valuesAmount, kVarValuesInitialDelta);

            std::vector<Value_t> immedList = findImmeds(functions);

            if(userGivenVarValues.empty())
            {
                for(std::size_t i = 0; i < functions.size(); ++i)
                    functions[i].mValidVarValues = varValues;
            }
            else
            {
                for(std::size_t i = 0; i < functions.size(); ++i)
                    functions[i].mValidVarValues = userGivenVarValues;
                ParserData<Value_t>::gVarValues.push_back(userGivenVarValues);
            }
            if(gOnlySuppliedValues) return true;

            std::vector<std::size_t> immedCounter(valuesAmount, 0);

            while(true)
            {
                for(unsigned i = 0; i < varsAmount; ++i)
                    varValues[i] = Value_t(
                        doubleValues[i*2+0],
                        doubleValues[i*2+1]
                    );

                bool wasOk = false;
                for(std::size_t i = 0; i < functions.size(); ++i)
                {
                    Value_t value = functions[i].mParser.Eval(&varValues[0]);
                    if(functions[i].mParser.EvalError() == 0 && valueIsOk(value))
                    {
                        if(userGivenVarValues.empty())
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

                std::size_t valueIndex = 0;
                while(true)
                {
                    if(immedCounter[valueIndex] == 0)
                    {
                        doubleValues[valueIndex] = -doubleValues[valueIndex];
                        if(doubleValues[valueIndex] < 0.0)
                            break;

                        doubleValues[valueIndex] += deltas[valueIndex];
                        if(deltas[valueIndex] < kVarValuesDeltaFactor2Threshold)
                            deltas[valueIndex] *= kVarValuesDeltaFactor1;
                        else
                            deltas[valueIndex] *= kVarValuesDeltaFactor2;

                        if(doubleValues[valueIndex] <= kVarValuesUpperLimit)
                            break;
                    }

                    if(immedCounter[valueIndex] < immedList.size())
                    {
                        std::size_t& i = immedCounter[valueIndex];
                        doubleValues[valueIndex] =
                            (valueIndex & 1)
                                ? makeDouble2From( immedList[i] )
                                : makeDouble1From( immedList[i] );
                        i += 1;
                        break;
                    }

                    immedCounter[valueIndex] = 0;
                    doubleValues[valueIndex] = 0.0;
                    deltas[valueIndex] = kVarValuesInitialDelta;
                    if(++valueIndex == doubleValues.size())
                    {
                        if(ParserData<Value_t>::gVarValues.empty())
                        {
                            ParserData<Value_t>::gVarValues.push_back
                                (std::vector<Value_t>(varsAmount, Value_t()));
                            return false;
                        }
                        return true;
                    }
                }
            }
        }
    };
#endif

    template<typename Value_t>
    bool findValidVarValues(std::vector<FunctionInfo<Value_t> >& functions,
                            const std::string& userGivenVarValuesString)
    {
        return findValidVarValuesAux<Value_t>
            ::find(functions, userGivenVarValuesString);
    }

    template<typename Value_t>
    inline Value_t scaledDiff(const Value_t& v1, const Value_t& v2)
    {
        using namespace FUNCTIONPARSERTYPES;
        if(IsIntType<Value_t>::value)
        {
            return v2 - v1;
        }
        if(IsComplexType<Value_t>::value)
        {
            if(fp_imag(v1) != Value_t() || fp_imag(v2) != Value_t())
            {
                return fp_abs(scaledDiff(fp_real(v1), fp_real(v2)))
                     + fp_abs(scaledDiff(fp_imag(v1), fp_imag(v2)));
            }
        }
        const Value_t scale =
            fp_pow(Value_t(10), Value_t(fp_floor(fp_log10(fp_abs(v1)))));
        const Value_t sv1 =
            fp_abs(v1) < epsilon<Value_t>() ? 0 : v1/scale;
        const Value_t sv2 =
            fp_abs(v2) < epsilon<Value_t>() ? 0 : v2/scale;
        return sv2 - sv1;
    }

    template<typename Value_t>
    inline bool notEqual(const Value_t& v1, const Value_t& v2)
    {
        using namespace FUNCTIONPARSERTYPES;
        if(IsIntType<Value_t>::value)
        {
            return v1 != v2;
        }
        return fp_abs(scaledDiff(v1, v2)) > epsilon<Value_t>();
    }

    template<typename Value_t>
    bool compareFunctions(std::size_t function1Index,
                          std::size_t function2Index,
                          ParserWithConsts<Value_t>& parser1,
                          const char* parser1Type,
                          ParserWithConsts<Value_t>& parser2,
                          const char* parser2Type)
    {
        const std::size_t varsAmount =
            ParserData<Value_t>::gVarValues[0].size();
        for(std::size_t varSetInd = 0;
            varSetInd < ParserData<Value_t>::gVarValues.size();
            ++varSetInd)
        {
            const Value_t* values =
                &ParserData<Value_t>::gVarValues[varSetInd][0];
            const Value_t v1 = parser1.Eval(values);
            const Value_t v2 = parser2.Eval(values);

            if(notEqual(v1, v2) || (parser1.EvalError() && gOnlySuppliedValues))
            {
                if(parser1.EvalError() && parser1Type[0] == 'n')
                {
                    // If the source expression returns an error,
                    // ignore this "failure"
                    // Unless this is an user-supplied expression
                    if(gIgnoreErrors || !gOnlySuppliedValues)
                        continue;
                }
                if(parser1.EvalError() != parser2.EvalError() && gIgnoreErrors)
                {
                    continue;
                }

                using namespace FUNCTIONPARSERTYPES;
                std::cout << SEPARATOR << "\n******* For variable values (";
                for(std::size_t i = 0; i < varsAmount; ++i)
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
        for(std::size_t ind1 = 0; ind1 < functions.size(); ++ind1)
        {
            if(gVerbosityLevel > 1) std::cout << "--func " << ind1 << "-- a\n";
            parser1.Parse
                (functions[ind1].mFunctionString, gVarString, gUseDegrees);
            if(gVerbosityLevel > 1) std::cout << "--func " << ind1 << "-- b\n";
            parser2.Parse
                (functions[ind1].mFunctionString, gVarString, gUseDegrees);
            // parser 1 is not optimized

            // Printing the bytecode right _here_ is useful
            // for debugging situations where fparser crashes
            // before printByteCodes() is reached, such as
            // within Optimize() or Eval().

            ////std::cout << "Not optimized:\n"; parser2.PrintByteCode(std::cout);
            if(gVerbosityLevel > 1) std::cout << "--func " << ind1 << "-- optimize\n";
            parser2.Optimize(); // parser 2 is optimized once

            ////std::cout << "Is optimized:\n"; parser2.PrintByteCode(std::cout);

            if(!compareFunctions(ind1, ind1, parser1, not_optimized,
                                 parser2, optimized))
                errors = true;

            parser2.Optimize(); // parser 2 is optimized twice
            ////std::cout << "Twice optimized:\n"; parser2.PrintByteCode(std::cout);

            if(!compareFunctions(ind1, ind1, parser1, not_optimized,
                                 parser2, optimized2))
                errors = had_double_optimization_problems = true;

            parser1.Optimize(); // parser 1 is optimized once
            if(!compareFunctions(ind1, ind1, parser1, optimized,
                                 parser2, optimized2))
                errors = had_double_optimization_problems = true;

            for(std::size_t ind2 = ind1+1; ind2 < functions.size(); ++ind2)
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

    void wrapLine(std::string& line, std::size_t cutter, std::string& wrap_buf,
                  bool always_cut = false)
    {
        if(line.size() <= cutter)
            line.resize(cutter, ' ');
        else
        {
            if(!always_cut)
            {
                for(std::size_t wrap_at = cutter; wrap_at > 0; --wrap_at)
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
#ifdef FUNCTIONPARSER_SUPPORT_DEBUGGING
        ParserWithConsts<Value_t> parser;
        const char* const wall =
            (mode == print_no_cut_or_wrap && color_support)
                ? "\33[0m| "
                : "| ";
        const char* const newline =
            (mode == print_no_cut_or_wrap && color_support)
                ? "\33[0m\n"
                : "\n";
        const char* colors[3] = { "\33[37m", "\33[36m", "\33[32m" };
        if(mode != print_no_cut_or_wrap || !color_support)
            colors[0] = colors[1] = colors[2] = "";

        for(std::size_t i = 0; i < functions.size(); ++i)
        {
            std::cout << SEPARATOR << std::endl;

            std::stringstream streams[3];

            parser.Parse(functions[i].mFunctionString, gVarString, gUseDegrees);

            std::size_t one_column  = 38;
            std::size_t two_columns = one_column * 2 + 2;

            streams[0] <<
                "Function " << i+1 << " original\n"
                "-------------------\n";
            parser.PrintByteCode(streams[0], gPrintByteCodeExpressions);

            streams[1] <<
                "Optimized\n"
                "---------\n";
            parser.Optimize();
            {
                std::ostringstream streams2_bytecodeonly;
                parser.PrintByteCode(streams2_bytecodeonly,
                                     gPrintByteCodeExpressions);
                streams[1] << streams2_bytecodeonly.str();

                parser.Optimize();
                {
                    std::ostringstream streams3_bytecodeonly;
                    parser.PrintByteCode(streams3_bytecodeonly,
                                         gPrintByteCodeExpressions);

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

            #if 0
            std::cout << "Code 0\n" << streams[0].str() << std::endl;
            std::cout << "Code 1\n" << streams[1].str() << std::endl;
            std::cout << "Code 2\n" << streams[2].str() << std::endl;
            #else
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
            #endif
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
        for(std::size_t i = 0; i < functions.size(); ++i)
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
        int result = info.mParser.Parse(info.mFunctionString, gVarString, gUseDegrees);
        if(result >= 0)
        {
            std::cerr << "\"" << info.mFunctionString << "\"\n"
                      << std::string(result+1, ' ')
                      << "^ " << info.mParser.ErrorMsg() << std::endl;
            if(info.mParser.ParseError() == FunctionParserErrorType::invalid_vars)
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

        for(std::size_t funcInd = 0; funcInd < functions.size(); ++funcInd)
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

    template<typename Value_t>
    void evaluateFunctionsAndPrintResult(std::vector<FunctionInfo<Value_t>>& functions,
                                         const std::string& evalVarValues)
    {
        if(!findValidVarValues(functions, evalVarValues))
        {
            std::cout << "ERROR: Parameter to -eval contained invalid syntax:\n\"" << evalVarValues << "\"\n";
            return;
        }

        functions[0].mParser.Parse(functions[0].mFunctionString, gVarString, gUseDegrees);
        const Value_t result = functions[0].mParser.Eval(&functions[0].mValidVarValues[0]);

        const int precision = FUNCTIONPARSERTYPES::fp_value_precision_decimal_digits<Value_t>();

        std::cout << "Function: \"" << functions[0].mFunctionString
                  << "\"\n- Variables: \"" << gVarString
                  << "\" with values:\n";
        for(const Value_t& value: functions[0].mValidVarValues)
            std::cout << "- " << ValueWithPrecisionPrinter<Value_t>(value, precision) << "\n";

        std::cout << "Result: " << ValueWithPrecisionPrinter<Value_t>(result, precision) << "\n";
    }

    int printHelp(const char* programName)
    {
        std::cerr <<
            "FunctionParser functioninfo utility " << kVersionNumber <<
            "\n\nUsage: " << programName <<
            " [<options] <function1> [<function2> ...]\n\n"
            "Options:\n";
        #define o(type, enumcode, opt1,opt2, verbosetype) do { \
            std::string optstr = "-" #opt1; \
            if(#opt2[0]) optstr += ", -" # opt2; \
            std::cerr << "  " << std::left << std::setw(20) << optstr \
                      << ": Use FunctionParser_" #opt1 ".\n"; \
        } while(0);
        FP_DECLTYPES(o)
        #undef o
        std::cerr <<
            "  -mpfr_bits <bits>   : MpfrFloat mantissa bits (default 80).\n"
            "  -vars <string>      : Specify a var string.\n"
            "  -nt                 : No timing measurements.\n"
            "  -ntd                : No timing if functions differ.\n"
            "  -deg                : Use degrees for trigonometry.\n"
            "  -noexpr             : Don't print byte code expressions.\n"
            "  -noerr              : Ignore differences in whether function produces an error.\n"
            "  -varValues <values> : Space-separated variable values to use.\n"
            "  -only               : Test using only the supplied variable values.\n"
            "  -eval <values>      : Evaluate the function using the specified\n"
            "                        space-separated variable values. Example:\n"
            "                        functioninfo \"x+y\" -eval \"10 25\"\n";
        return 1;
    }
}

template<typename Value_t>
int functionInfo(const char* const parserTypeString,
                 const std::vector<std::string>& functionStrings,
                 bool measureTimings, bool noTimingIfEqualityErrors,
                 const std::string& userGivenVarValues,
                 const std::string& evalVarValues)
{
    std::vector<FunctionInfo<Value_t> > functions(functionStrings.size());
    for(std::size_t i = 0; i < functions.size(); ++i)
        functions[i].mFunctionString = functionStrings[i];

    if(gVarString.empty())
        deduceVariables(functions);

    for(std::size_t i = 0; i < functions.size(); ++i)
    {
        if(!checkFunctionValidity(functions[i]))
            return 1;
    }

    const bool validVarValuesFound =
        findValidVarValues(functions, userGivenVarValues);

    std::cout << SEPARATOR << std::endl
              << "Parser type: " << parserTypeString << std::endl;
    for(std::size_t i = 0; i < functions.size(); ++i)
        std::cout << "- Function " << i+1 << ": \""
                  << functions[i].mFunctionString << "\"\n";
    const std::size_t varsAmount = ParserData<Value_t>::gVarValues[0].size();
    const std::size_t varValueSetsAmount = ParserData<Value_t>::gVarValues.size();
    std::cout << "- Var string: \"" << gVarString << "\" ("
              << ParserData<Value_t>::gVarValues[0].size()
              << (varsAmount == 1 ? " var" : " vars")
              << ") (using " << varValueSetsAmount << " set"
              << (varValueSetsAmount == 1 ? ")\n" : "s)\n");

#if 0
    std::cout << SEPARATOR << "\nTesting with variable values:\n";
    for(std::size_t i = 0; i < ParserData<Value_t>::gVarValues.size(); ++i)
    {
        if(i > 0) std::cout << (i%5==0 ? "\n" : " ");
        std::cout << "(";
        for(std::size_t j = 0; j < ParserData<Value_t>::gVarValues[i].size(); ++j)
        {
            if(j > 0) std::cout << ",";
            using namespace FUNCTIONPARSERTYPES;
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

    if(!evalVarValues.empty())
    {
        evaluateFunctionsAndPrintResult(functions, evalVarValues);
    }

    /* Release all global references to Value_t() _before_
     * the global destructors for GmpInt and MpfrFloat are run
     */
    ParserData<Value_t>::gVarValues.clear();

    return 0;
}

int main(int argc, char* argv[])
{
    testPrintValueWithPrecision();

    if(argc < 2) return printHelp(argv[0]);

    enum ParserType {
        #define o(type, enumcode, opt1,opt2, verbosetype) FP_##enumcode,
        FP_DECLTYPES(o)
        #undef o
    };

    std::vector<std::string> functionStrings;
    bool measureTimings = true, noTimingIfEqualityErrors = false;
    ParserType parserType = FP_D;
    unsigned long mantissaBits = 80;
    std::string userGivenVarValues, evalVarValues;

    for(int i = 1; i < argc; ++i)
    {
        if(std::strcmp(argv[i], "-vars") == 0)
        {
            if(++i == argc) return printHelp(argv[0]);
            gVarString = argv[i];
        }
        #define o(type, enumcode, opt1,opt2, verbosetype) \
            else if(        std::strcmp(argv[i], "-" #opt1) == 0 \
            || (#opt2[0] && std::strcmp(argv[i], "-" #opt2) == 0)) \
                parserType = FP_##enumcode;
        FP_DECLTYPES(o)
        #undef o
        else if(std::strcmp(argv[i], "-nt") == 0)
            measureTimings = false;
        else if(std::strcmp(argv[i], "-v") == 0)
            ++gVerbosityLevel;
        else if(std::strcmp(argv[i], "-q") == 0)
            --gVerbosityLevel;
        else if(std::strcmp(argv[i], "-ntd") == 0)
            noTimingIfEqualityErrors = true;
        else if(std::strcmp(argv[i], "-deg") == 0)
            gUseDegrees = true;
        else if(std::strcmp(argv[i], "-mpfr_bits") == 0)
        {
            if(++i == argc) return printHelp(argv[0]);
            mantissaBits = std::atol(argv[i]);
        }
        else if(std::strcmp(argv[i], "-noexpr") == 0)
            gPrintByteCodeExpressions = false;
        else if(std::strcmp(argv[i], "-noerr") == 0)
            gIgnoreErrors = true;
        else if(std::strcmp(argv[i], "-only") == 0)
            gOnlySuppliedValues = true;
        else if(std::strcmp(argv[i], "-varValues") == 0)
        {
            if(++i == argc) return printHelp(argv[0]);
            userGivenVarValues = argv[i];
        }
        else if(std::strcmp(argv[i], "-eval") == 0)
        {
            if(++i == argc) return printHelp(argv[0]);
            evalVarValues = argv[i];
        }
        else if(std::strcmp(argv[i], "--help") == 0
             || std::strcmp(argv[i], "-help") == 0
             || std::strcmp(argv[i], "-h") == 0
             || std::strcmp(argv[i], "/?") == 0)
            return printHelp(argv[0]);
        else
            functionStrings.push_back(argv[i]);
    }

    if(functionStrings.empty()) return printHelp(argv[0]);

    const char* notCompiledParserType = nullptr;

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    MpfrFloat::setDefaultMantissaBits(mantissaBits);
#endif

    switch(parserType)
    {
    #define o(type, enumcode, opt1,opt2, verbosetype) \
        case FP_##enumcode: \
            rt_##enumcode(\
                return functionInfo<type>(\
                    verbosetype, functionStrings, \
                    measureTimings, noTimingIfEqualityErrors, \
                    userGivenVarValues, evalVarValues) , \
                notCompiledParserType = #type); \
            break;
        FP_DECLTYPES(o)
    #undef o
    }

    if(notCompiledParserType)
    {
        std::cout << "Error: Support for type " << notCompiledParserType
                  << " was not compiled in." << std::endl;
        return 1;
    }
    return 0;
}
