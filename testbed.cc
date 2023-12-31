/*==========================================================================
  testbed
  ---------
  Copyright: Juha Nieminen, Joel Yliluoma
  This program (testbed) is distributed under the terms of
  the GNU General Public License (GPL) version 3.
  See gpl.txt for the license text.
============================================================================*/

#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

static const char* const kVersionNumber = "2.3.0.12";

#include "fpconfig.hh"
#include "fparser.hh"
#include "extrasrc/fpaux.hh"
#include "tests/stringutil.hh"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <cassert>
#include <future>
#include <atomic>
#include <mutex>
#include <random>
#include "extrasrc/testbed_types.hh"

/* Verbosity level:
   0 = No progress output. Error reporting as in verbosity level 1.
   1 = Very brief progress and error output.
   2 = More verbose progress output, full error reporting.
   3 = Very verbose progress output, full error reporting.
*/
static int gVerbosityLevel = 1;

static const char* getEvalErrorName(int errorCode)
{
    static const char* const evalErrorNames[6] =
    {
        "no error", "division by zero", "sqrt error", "log error",
        "trigonometric error", "max eval recursion level reached"
    };
    if(errorCode >= 0 && errorCode < 6)
        return evalErrorNames[errorCode];
    return "unknown";
}

static std::vector<const char*> gSelectedRegressionTests;


namespace
{
    template<typename Value_t>
    class UserDefFuncWrapper:
        public FunctionParserBase<Value_t>::FunctionWrapper
    {
        Value_t (*mFuncPtr)(const Value_t*);
        unsigned mCounter;

     public:
        UserDefFuncWrapper(Value_t (*funcPtr)(const Value_t*)) :
            mFuncPtr(funcPtr), mCounter(0)
        {}

        virtual Value_t callFunction(const Value_t* values)
        {
            ++mCounter;
            return mFuncPtr(values);
        }

        unsigned counter() const { return mCounter; }
    };
}

#ifndef _MSC_VER
/*
static void setAnsiColor(unsigned color)
{
    static int bold = 0;
    std::cout << "\33[";
    if(color > 7)
    {
        if(!bold) { std::cout << "1;"; bold=1; }
        color -= 7;
    }
    else if(bold) { std::cout << "0;"; bold=0; }
    std::cout << 30+color << "m";
}
*/

static void setAnsiBold() { std::cout << "\33[1m"; }
static void resetAnsiColor() { std::cout << "\33[0m"; }
#else
/*static void setAnsiColor(unsigned) {}*/
static void setAnsiBold() {}
static void resetAnsiColor() {}
#endif


//=========================================================================
// Definition of tests
//=========================================================================
#include "tests/testbed_autogen.hh"

using DefaultParser = FunctionParserBase<DefaultValue_t>;

static TestType customtest{}; // For runCurrentTrigCombinationTest
inline const TestType& getTest(unsigned index)
{
    if(index != customtest_index) [[likely]]
        return AllTests[index];
    else [[unlikely]]
        return customtest;
}


//=========================================================================
// Utility functions for testing
//=========================================================================
template<typename Value_t>
bool compareValuesWithEpsilon(const Value_t& value1, const Value_t& value2)
{
    using namespace FUNCTIONPARSERTYPES;
    if(IsIntType<Value_t>::value)
        return value1 == value2;
    else
        return fp_abs(value1 - value2) <= testbedEpsilon<Value_t>();
}

//=========================================================================
// Test copying
//=========================================================================
template<typename Value_t>
static bool testCopyingNoDeepCopy(FunctionParserBase<Value_t> parser)
{
    const Value_t vars[2] = { static_cast<Value_t>(3), static_cast<Value_t>(5) };

    if(!compareValuesWithEpsilon(parser.Eval(vars), static_cast<Value_t>(13)))
    {
        if(gVerbosityLevel >= 2)
        {
            std::cout << "\n - Giving as function parameter (no deep copy) failed."
                      << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUGGING
            parser.PrintByteCode(std::cout);
#endif
        }
        return false;
    }
    return true;
}

template<typename Value_t>
static bool testCopyingDeepCopy(FunctionParserBase<Value_t> parser)
{
    const Value_t vars[2] = { static_cast<Value_t>(3), static_cast<Value_t>(5) };

    parser.Parse("x*y-1", "x,y");

    if(!compareValuesWithEpsilon(parser.Eval(vars), static_cast<Value_t>(14)))
    {
        if(gVerbosityLevel >= 2)
        {
            std::cout << "\n - Giving as function parameter (deep copy) failed."
                      << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUGGING
            parser.PrintByteCode(std::cout);
#endif
        }
        return false;
    }
    return true;
}

template<typename Value_t>
static int testCopyingWithValueType()
{
    using Parser_t = FunctionParserBase<Value_t>;

    bool retval = true;
    const Value_t vars[2] = { static_cast<Value_t>(2), static_cast<Value_t>(5) };

    Parser_t parser1, parser3;
    parser1.Parse("x*y-2", "x,y");

    // Test shallow copying
    Parser_t parser2(parser1);
    if(!compareValuesWithEpsilon(parser2.Eval(vars), static_cast<Value_t>(8)))
    {
        retval = false;
        if(gVerbosityLevel >= 2)
        {
            std::cout << "\n - Copy constructor with no deep copy failed."
                      << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUGGING
            parser2.PrintByteCode(std::cout);
#endif
        }
    }

    // Test changing a shallow-copied parser
    parser2.Parse("x*y-1", "x,y");
    if(!compareValuesWithEpsilon(parser2.Eval(vars), static_cast<Value_t>(9)))
    {
        retval = false;
        if(gVerbosityLevel >= 2)
        {
            std::cout << "\n - Copy constructor with deep copy failed."
                      << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUGGING
            parser2.PrintByteCode(std::cout);
#endif
        }
    }

    // Test shallow assignment
    parser3 = parser1;
    if(!compareValuesWithEpsilon(parser3.Eval(vars), static_cast<Value_t>(8)))
    {
        retval = false;
        if(gVerbosityLevel >= 2)
        {
            std::cout << "\n - Assignment with no deep copy failed."
                      << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUGGING
            parser3.PrintByteCode(std::cout);
#endif
        }
    }

    // Test changing a shallow-assigned parser
    parser3.Parse("x*y-1", "x,y");
    if(!compareValuesWithEpsilon(parser3.Eval(vars), static_cast<Value_t>(9)))
    {
        retval = false;
        if(gVerbosityLevel >= 2)
        {
            std::cout << "\n - Assignment with deep copy failed."
                      << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUGGING
            parser3.PrintByteCode(std::cout);
#endif
        }
    }

    // Test passing a parser by value, with the function using shallow copying
    if(!testCopyingNoDeepCopy(parser1))
        retval = false;

    // Final test to check that p1 still works:
    if(!compareValuesWithEpsilon(parser1.Eval(vars), static_cast<Value_t>(8)))
    {
        retval = false;
        if(gVerbosityLevel >= 2)
            std::cout << "\n - Failed: parser1 was corrupted." << std::endl;
    }

    // Test passing a parser by value, with the function using deep copying
    if(!testCopyingDeepCopy(parser1))
        retval = false;

    // Final test to check that p1 still works:
    if(!compareValuesWithEpsilon(parser1.Eval(vars), static_cast<Value_t>(8)))
    {
        retval = false;
        if(gVerbosityLevel >= 2)
        {
            std::cout << "\n - Failed: parser1 was corrupted." << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUGGING
            parser1.PrintByteCode(std::cout);
#endif
        }
    }

    return retval;
}

static int testCopying()
{
    #define o(type, enumcode, opt1,opt2, verbosetype) \
        rt_##enumcode( \
            if(gVerbosityLevel >= 3) \
                std::cout << "\n - Testing copying with FunctionParserBase<" # type ">"; \
            if(!testCopyingWithValueType<type>()) return false; , \
        )
    FP_DECLTYPES(o)
    #undef o
    return true;
}


//=========================================================================
// Test error situations
//=========================================================================
struct ErrorSituationsTestData
{
    FunctionParserErrorType expected_error;
    int expected_error_position;
    const char* function_string;
};

template<typename Value_t, unsigned testDataAmount, unsigned invalidNamesAmount>
static int testErrorSituationsWithValueType(const ErrorSituationsTestData(&testData)[testDataAmount],
                                            const char* const (&invalidNames)[invalidNamesAmount])
{
    using Parser_t = FunctionParserBase<Value_t>;

    bool retval = true;
    Parser_t parser, tmpParser;
    parser.AddUnit("unit", static_cast<Value_t>(2));
    parser.AddFunction("Value", userDefFuncValue<Value_t>, 0);
    parser.AddFunction("Sqr", userDefFuncSqr<Value_t>, 1);
    parser.AddFunctionWrapper("Sub", UserDefFuncWrapper<Value_t> (userDefFuncSub<Value_t>), 2);
    tmpParser.Parse("0", "x");

    for(unsigned data_index = 0; data_index < testDataAmount; ++data_index)
    {
        int parse_result = parser.Parse(testData[data_index].function_string, "x");
        if(parse_result < 0)
        {
            retval = false;
            if(gVerbosityLevel >= 2)
            {
                std::cout << "\n - Parsing the invalid function \""
                          << testData[data_index].function_string
                          << "\" didn't fail\n";
#ifdef FUNCTIONPARSER_SUPPORT_DEBUGGING
                parser.PrintByteCode(std::cout);
#endif
            }
        }
        else if(parser.ParseError() != testData[data_index].expected_error
             || parse_result != testData[data_index].expected_error_position)
        {
            retval = false;
            if(gVerbosityLevel >= 2)
            {
                std::cout << "\n - Parsing the invalid function \""
                          << testData[data_index].function_string
                          << "\" produced ";
                if(parser.ParseError() != testData[data_index].expected_error)
                {
                    std::cout << "wrong error code (" << parser.ErrorMsg() << ")";
                }
                if(parse_result != testData[data_index].expected_error_position)
                {
                    std::cout << "wrong pointer (expected "
                              << testData[data_index].expected_error_position
                              << ", got " << parse_result << ")";
                }
                std::cout << "\n";
#ifdef FUNCTIONPARSER_SUPPORT_DEBUGGING
                parser.PrintByteCode(std::cout);
#endif
            }
        }
    }

    for(unsigned nameIndex = 0; nameIndex < invalidNamesAmount; ++nameIndex)
    {
        const char* const name = invalidNames[nameIndex];
        if(parser.AddConstant(name, static_cast<Value_t>(1)))
        {
            retval = false;
            if(gVerbosityLevel >= 2)
                std::cout << "\n - Adding an invalid name (\"" << name
                          << "\") as constant didn't fail" << std::endl;
        }
        if(parser.AddFunction(name, userDefFuncSqr<Value_t>, 1))
        {
            retval = false;
            if(gVerbosityLevel >= 2)
                std::cout << "\n - Adding an invalid name (\"" << name
                          << "\") as funcptr didn't fail" << std::endl;
        }
        if(parser.AddFunction(name, tmpParser))
        {
            retval = false;
            if(gVerbosityLevel >= 2)
                std::cout << "\n - Adding an invalid name (\"" << name
                          << "\") as funcparser didn't fail" << std::endl;
        }
        if(parser.Parse("0", name) < 0)
        {
            retval = false;
            if(gVerbosityLevel >= 2)
                std::cout << "\n - Using an invalid name (\"" << name
                          << "\") as variable name didn't fail" << std::endl;
        }
    }

    parser.AddConstant("CONST", FUNCTIONPARSERTYPES::fp_const_preciseDouble<Value_t>(CONST));
    parser.AddFunction("PTR", userDefFuncSqr<Value_t>, 1);
    parser.AddFunction("PARSER", tmpParser);

    if(parser.AddConstant("PTR", static_cast<Value_t>(1)))
    {
        retval = false;
        if(gVerbosityLevel >= 2)
            std::cout << "\n - Adding a userdef function (\"PTR\") as "
                      << "constant didn't fail" << std::endl;
    }
    if(parser.AddFunction("CONST", userDefFuncSqr<Value_t>, 1))
    {
        retval = false;
        if(gVerbosityLevel >= 2)
            std::cout << "\n - Adding a userdef constant (\"CONST\") as "
                      << "funcptr didn't fail" << std::endl;
    }
    if(parser.AddFunction("CONST", tmpParser))
    {
        retval = false;
        if(gVerbosityLevel >= 2)
            std::cout << "\n - Adding a userdef constant (\"CONST\") as "
                      << "funcparser didn't fail" << std::endl;
    }

    return retval;
}

static int testErrorSituations()
{
    static const ErrorSituationsTestData test_data_common[] =
    {
        { FunctionParserErrorType::syntax_error,                       2, "x+" },
        { FunctionParserErrorType::expect_operator,                    2, "x x"},
        { FunctionParserErrorType::unknown_identifier,                 0, "y" },
        { FunctionParserErrorType::expect_operator,                    1, "x, x"},
        { FunctionParserErrorType::syntax_error,                       2, "x**x" },
        { FunctionParserErrorType::syntax_error,                       2, "x+*x" },
        { FunctionParserErrorType::syntax_error,                       0, "unit" },
        { FunctionParserErrorType::syntax_error,                       0, "unit x" },
        { FunctionParserErrorType::syntax_error,                       2, "x*unit" },
        { FunctionParserErrorType::syntax_error,                       0, "unit*unit" },
        { FunctionParserErrorType::syntax_error,                       0, "unit unit" },
        { FunctionParserErrorType::expect_operator,                    1, "x(unit)"},
        { FunctionParserErrorType::syntax_error,                       2, "x+unit" },
        { FunctionParserErrorType::syntax_error,                       2, "x*unit" },
        { FunctionParserErrorType::empty_parentheses,                  1, "()"},
        { FunctionParserErrorType::syntax_error,                       0, "" },
        { FunctionParserErrorType::expect_operator,                    1, "x()"},
        { FunctionParserErrorType::empty_parentheses,                  3, "x*()"},
        { FunctionParserErrorType::syntax_error,                       1, "(" },
        { FunctionParserErrorType::mismatched_parenthesis,             0, ")"},
        { FunctionParserErrorType::missing_parenthesis,                2, "(x"},
        { FunctionParserErrorType::expect_operator,                    1, "x)"},
        { FunctionParserErrorType::mismatched_parenthesis,             0, ")x("},
        { FunctionParserErrorType::missing_parenthesis,                14, "(((((((x))))))"},
        { FunctionParserErrorType::expect_operator,                    15, "(((((((x))))))))"},
        { FunctionParserErrorType::expect_operator,                    1, "2x"},
        { FunctionParserErrorType::expect_operator,                    3, "(2)x"},
        { FunctionParserErrorType::expect_operator,                    3, "(x)2"},
        { FunctionParserErrorType::expect_operator,                    1, "2(x)"},
        { FunctionParserErrorType::expect_operator,                    1, "x(2)"},
        { FunctionParserErrorType::syntax_error,                       0, "[x]" },
        { FunctionParserErrorType::syntax_error,                       0, "@x" },
        { FunctionParserErrorType::syntax_error,                       0, "$x" },
        { FunctionParserErrorType::syntax_error,                       0, "{x}" },
        { FunctionParserErrorType::illegal_parameters_amount,          6, "if(x,2)" },
        { FunctionParserErrorType::illegal_parameters_amount,          10, "if(x, 2, 3, 4)" },
        { FunctionParserErrorType::missing_parenthesis,                6, "Value(x)"},
        { FunctionParserErrorType::missing_parenthesis,                6, "Value(1+x)"},
        { FunctionParserErrorType::missing_parenthesis,                6, "Value(1,x)"},
        // Note: ^should these three not return ILL_PARAMS_AMOUNT instead?
        { FunctionParserErrorType::illegal_parameters_amount,          4, "Sqr()"},
        { FunctionParserErrorType::illegal_parameters_amount,          5, "Sqr(x,1)" },
        { FunctionParserErrorType::illegal_parameters_amount,          5, "Sqr(1,2,x)" },
        { FunctionParserErrorType::illegal_parameters_amount,          4, "Sub()" },
        { FunctionParserErrorType::illegal_parameters_amount,          5, "Sub(x)" },
        { FunctionParserErrorType::illegal_parameters_amount,          7, "Sub(x,1,2)" },
        { FunctionParserErrorType::unknown_identifier,                 2, "x+Sin(1)" },
        { FunctionParserErrorType::unknown_identifier,                 0, "sub(1,2)" },
        { FunctionParserErrorType::unknown_identifier,                 0, "sinx(1)"  },
        { FunctionParserErrorType::unknown_identifier,                 2, "1+X"      },
        { FunctionParserErrorType::unknown_identifier,                 0, "eval(x)" },
        { FunctionParserErrorType::illegal_parameters_amount,          5, "max(x)" },
        { FunctionParserErrorType::illegal_parameters_amount,          8, "max(x, 1, 2)" },
    };

    static const ErrorSituationsTestData test_data_fp_parsers[] =
    {
        { FunctionParserErrorType::syntax_error,                       2, "x^^2" },
        { FunctionParserErrorType::expect_operator,                    2, "1..2"},
        { FunctionParserErrorType::missing_parenthesis,                5, "sin(x" },
        { FunctionParserErrorType::missing_parenthesis_after_function, 4, "sin x" },
        { FunctionParserErrorType::unknown_identifier,                 4, "sin(y)" },
        { FunctionParserErrorType::illegal_parameters_amount,          5, "sin(x, 1)" },
        { FunctionParserErrorType::syntax_error,                       4, "sin(unit)" },
        { FunctionParserErrorType::missing_parenthesis_after_function, 4, "sin unit"},
    };

    static const ErrorSituationsTestData test_data_int_parsers[] =
    {
        { FunctionParserErrorType::expect_operator,                    1, "x^^2" },
        { FunctionParserErrorType::expect_operator,                    1, "1..2"},
        { FunctionParserErrorType::unknown_identifier,                 0, "sin(x)" },
        { FunctionParserErrorType::unknown_identifier,                 0, "cos(x)" },
        { FunctionParserErrorType::unknown_identifier,                 8, "1 + 2 * sin(x)" },
    };

    static const char* const invalid_names_common[] =
        { "s2%", "(x)", "5x", "2", "\302\240"/*nbsp*/ };

    static const char* const invalid_names_fp_parsers[] =
        { "sin", "cos", "tan", "sqrt", "sinh", "cosh", "tanh", "hypot" };

    int retval = 1;

#define o(type, enumcode, opt1,opt2, verbosetype) \
    rt_##enumcode( \
        if(gVerbosityLevel >= 3) \
            std::cout << "\n - Testing error handing with FunctionParserBase<" # type ">"; \
        retval &= testErrorSituationsWithValueType<type>(test_data_common, invalid_names_common); \
        if(FUNCTIONPARSERTYPES::IsIntType<type>::value) \
            retval &= testErrorSituationsWithValueType<type>(test_data_int_parsers, invalid_names_common); \
        else \
            retval &= testErrorSituationsWithValueType<type>(test_data_fp_parsers, invalid_names_fp_parsers); \
        , )
    FP_DECLTYPES(o)
#undef o

    return retval;
}


//=========================================================================
// Thoroughly test whitespaces
//=========================================================================
template<typename Value_t>
static bool testWhitespaceFunction(FunctionParserBase<Value_t>& parser, const std::string& function,
                                   Value_t(*cppFunction)(const Value_t&))
{
    using namespace FUNCTIONPARSERTYPES;

    int res = parser.Parse(function, "x");
    if(res > -1)
    {
        if(gVerbosityLevel >= 2)
            std::cout << "\n - Parsing function:\n\"" << function
                      << "\"\nfailed at char " << res
                      << ": " << parser.ErrorMsg() << std::endl;
        return false;
    }

    static const Value_t
        v_1_0 = FUNCTIONPARSERTYPES::fp_const_preciseDouble<Value_t>(1.0),
        v_5_0 = FUNCTIONPARSERTYPES::fp_const_preciseDouble<Value_t>(5.0);

    Value_t vars[1] = { -v_5_0 };
    for(; vars[0] <= v_5_0; vars[0] += v_1_0)
    {
        const Value_t value = parser.Eval(vars);
        const Value_t expected = cppFunction(vars[0]);
        if(FUNCTIONPARSERTYPES::fp_abs(value - expected) > testbedEpsilon<Value_t>())
        {
            if(gVerbosityLevel >= 2)
            {
                std::cout << "\n - For function:\n\"" << function
                          << "\"\nparser returned value " << value << " instead of " << expected << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUGGING
                parser.PrintByteCode(std::cout);
#endif
            }
            return false;
        }
    }
    return true;
}

template<typename Value_t>
static int testWhitespacesWithType(const char *testFunctionString, Value_t(*cppFunction)(const Value_t&))
{
    using namespace FUNCTIONPARSERTYPES;

    FunctionParserBase<Value_t> parser;
    parser.AddConstant("const", fp_const_preciseDouble<Value_t>(5.2));
    parser.AddUnit("unit", fp_const_preciseDouble<Value_t>(2));
    std::string function = testFunctionString;

    if(!testWhitespaceFunction(parser, function, cppFunction)) return false;

    static const unsigned char whiteSpaceTables[][4] =
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
    const unsigned n_whitespaces =
        sizeof(whiteSpaceTables)/sizeof(*whiteSpaceTables);

    for(unsigned i = 0; i < function.size(); ++i)
    {
        if(function[i] == ' ')
        {
            function.erase(i, 1);
            for(std::size_t a = 0; a < n_whitespaces; ++a)
            {
                if(!testWhitespaceFunction(parser, function, cppFunction)) return false;
                int length = static_cast<int>(whiteSpaceTables[a][0]);
                const char* sequence = (const char*)&whiteSpaceTables[a][1];
                function.insert(i, sequence, length);
                if(!testWhitespaceFunction(parser, function, cppFunction)) return false;
                function.erase(i, length);
            }
        }
    }
    return true;
}

template<typename Value_t>
static Value_t whitespaceFunction_floating_point(const Value_t& x)
{
    using namespace FUNCTIONPARSERTYPES;

    static const Value_t
        v_1_0 = fp_const_preciseDouble<Value_t>(1.0),
        v_1_5 = fp_const_preciseDouble<Value_t>(1.5),
        v_2_0 = fp_const_preciseDouble<Value_t>(2.0),
        v_3_0 = fp_const_preciseDouble<Value_t>(3.0),
        v_5_2 = fp_const_preciseDouble<Value_t>(5.2);
    return
        x + fp_sin((x*-v_1_5) - v_1_0 * (((-x)*v_5_2 + (v_2_0 - x*v_2_0)*v_2_0)+(v_3_0*v_2_0))+ (v_5_2*v_2_0)) + fp_cos(x)*v_2_0;
}

template<typename Value_t>
static Value_t whitespaceFunction_int(const Value_t& x)
{
    using namespace FUNCTIONPARSERTYPES;

    static const Value_t
        v_1 = fp_const_preciseDouble<Value_t>(1),
        v_2 = fp_const_preciseDouble<Value_t>(2),
        v_5 = fp_const_preciseDouble<Value_t>(5),
        v_6 = fp_const_preciseDouble<Value_t>(6);
    return
        x + fp_abs((x*-v_1) - v_6 * (((-x)*v_5 + (v_2 - x*v_2)*v_2) + v_6)+ (v_5*v_2)) + fp_abs(x)*v_2;
}

static int whiteSpaceTest()
{
    const char *const function_string_floating_point =
        " x + sin ( ( x * - 1.5 ) - .5 unit * ( ( ( - x ) * const + ( 2 - ( x ) unit ) unit ) + 3 unit ) + ( const ) unit ) + cos ( x ) unit ";
    const char *const function_string_int =
        " x + abs ( ( x * - 1 ) - 3 unit * ( ( ( - x ) * const + ( 2 - ( x ) unit ) unit ) + 3 unit ) + ( const ) unit ) + abs ( x ) unit ";

    int retval = 1;

#define o(type, enumcode, opt1,opt2, verbosetype) \
    rt_##enumcode( \
        if(gVerbosityLevel >= 3) \
            std::cout << "\n - Testing whitespace with FunctionParserBase<" # type ">"; \
        if(FUNCTIONPARSERTYPES::IsIntType<type>::value) \
            retval &= testWhitespacesWithType<type>(function_string_int, whitespaceFunction_int); \
        else \
            retval &= testWhitespacesWithType<type>(function_string_floating_point, whitespaceFunction_floating_point); \
        , )
    FP_DECLTYPES(o)
#undef o

    return retval;
}

//=========================================================================
// Test integer powers
//=========================================================================
template<typename Value_t>
static bool compareExpValues(const Value_t& value,
                             const std::string& funcStr,
                             const Value_t& v1,
                             const Value_t& v2,
                             bool isOptimized)
{
    using namespace FUNCTIONPARSERTYPES;

    const Value_t epsilon = testbedEpsilon<Value_t>();
    const Value_t abs_diff_v1_v2 = fp_abs(v1 - v2);
    const Value_t diff =
        fp_abs(v1) < epsilon ?
        (fp_abs(v2) < epsilon ? abs_diff_v1_v2 : abs_diff_v1_v2 / v2) :
         abs_diff_v1_v2 / v1;

    if(diff > epsilon)
    {
        if(gVerbosityLevel >= 2)
        {
            std::cout << "\n - For \"" << funcStr << "\" with x=" << value << " the library (";
            if(!isOptimized) std::cout << "not ";
            std::cout << "optimized) returned\n"
                      << std::setprecision(18) << v2
                      << " instead of " << v1
                      << "\n(Epsilon = " << epsilon << ")" << std::endl;
        }
        return false;
    }
    return true;
}

template<typename Value_t>
static bool runIntPowTest(FunctionParserBase<Value_t>& parser, const std::string& funcStr,
                          int exponent, bool isOptimized)
{
    using namespace FUNCTIONPARSERTYPES;

    const int absExponent = exponent < 0 ? -exponent : exponent;
    const Value_t value_0 = fp_const_preciseDouble<Value_t>(0);
    const Value_t value_1 = fp_const_preciseDouble<Value_t>(1);
    const Value_t value_100 = fp_const_preciseDouble<Value_t>(100);

    for(int valueOffset = 0; valueOffset <= 5; ++valueOffset)
    {
        const Value_t valueOffset_minus_1 = fp_const_preciseDouble<Value_t>(valueOffset - 1);
        const Value_t value = (exponent >= 0 && valueOffset == 0) ? value_0 : value_1 + valueOffset_minus_1 / value_100;
        Value_t v1 = exponent == 0 ? value_1 : value;
        for(int i = 2; i <= absExponent; ++i)
            v1 *= value;
        if(exponent < 0) v1 = value_1 / v1;

        const Value_t v2 = parser.Eval(&value);

        if(!compareExpValues(value, funcStr, v1, v2, isOptimized))
            return false;
    }

    return true;
}

template<typename Value_t>
static bool runFractionalPowTest(const std::string& funcStr, const Value_t& exponent)
{
    using namespace FUNCTIONPARSERTYPES;

    FunctionParserBase<Value_t> parser;
    if(parser.Parse(funcStr, "x") != -1)
    {
        if(gVerbosityLevel >= 2)
            std::cout << "\n - Parsing \"" << funcStr <<"\" failed: " << parser.ErrorMsg() << "\n";
        return false;
    }

    const Value_t value_0 = fp_const_preciseDouble<Value_t>(0);
    const Value_t value_1 = fp_const_preciseDouble<Value_t>(1);
    const Value_t value_2 = fp_const_preciseDouble<Value_t>(2);

    for(int i = 0; i < 3; ++i)
    {
        for(int valueOffset = 0; valueOffset <= 10; ++valueOffset)
        {
            const Value_t valueOffset_minus_1 = fp_const_preciseDouble<Value_t>(valueOffset - 1);
            const Value_t value = (exponent >= value_0 && valueOffset == 0) ? value_0 : value_1 + valueOffset_minus_1 / value_2;
            const Value_t v1 = fp_pow(value, exponent);
            const Value_t v2 = parser.Eval(&value);

            if(!compareExpValues(value, funcStr, v1, v2, i > 0))
                return false;
        }
        parser.Optimize();
    }

    return true;
}

template<typename Value_t>
static int testIntPowWithType()
{
    using namespace FUNCTIONPARSERTYPES;

    FunctionParserBase<Value_t> parser;

    for(int exponent = -1300; exponent <= 1300; ++exponent)
    {
        std::ostringstream os;
        os << "x^" << exponent;
        const std::string func = os.str();
        if(parser.Parse(func, "x") != -1)
        {
            if(gVerbosityLevel >= 2)
                std::cout << "\n - Parsing \"" << func <<"\" failed: " << parser.ErrorMsg() << "\n";
            return false;
        }

        if(!runIntPowTest(parser, func, exponent, false)) return false;
        parser.Optimize();
        if(!runIntPowTest(parser, func, exponent, true)) return false;
    }

    const Value_t value_2 = fp_const_preciseDouble<Value_t>(2);
    const Value_t value_3 = fp_const_preciseDouble<Value_t>(3);

    for(int m = -27; m <= 27; ++m)
    {
        for(int n_sqrt = 0; n_sqrt <= 4; ++n_sqrt)
        {
            const Value_t pow_2_n_sqrt = fp_pow(value_2, fp_const_preciseDouble<Value_t>(n_sqrt));
            for(int n_cbrt = 0; n_cbrt <= 4; ++n_cbrt)
            {
                if(n_sqrt + n_cbrt == 0) continue;

                const Value_t pow_3_n_cbrt = fp_pow(value_3, fp_const_preciseDouble<Value_t>(n_cbrt));

                std::ostringstream os;
                os << "x^(" << m << "/(1";
                for(int n=0; n < n_sqrt; ++n) os << "*2";
                for(int n=0; n < n_cbrt; ++n) os << "*3";
                os << "))";
                Value_t exponent = fp_const_preciseDouble<Value_t>(m);
                if(n_sqrt > 0) exponent /= pow_2_n_sqrt;
                if(n_cbrt > 0) exponent /= pow_3_n_cbrt;
                if(!runFractionalPowTest<Value_t>(os.str(), exponent)) return false;
            }
        }
    }

    return true;
}

static int testIntPow()
{
    int retval = 1;

#define o(type, enumcode, opt1,opt2, verbosetype) \
    rt_##enumcode( \
        if(FUNCTIONPARSERTYPES::IsIntType<type>::value) {} \
        else { \
            if(gVerbosityLevel >= 3) \
                std::cout << "\n - Testing integer and fractional powers with FunctionParserBase<" # type ">"; \
            retval &= testIntPowWithType<type>(); \
        } , )
    FP_DECLTYPES(o)
#undef o

    return retval;
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
        if(gVerbosityLevel >= 2)
        {
            std::printf("\n - %s failed with identifier ", testType);
            for(unsigned i = 0; i < length; ++i)
                iters[i].print();
            std::printf(": \"%s\"\n", identifier.c_str());
        }
        return false;
    }

    bool printUTF8TestError2(const CharIter* iters, unsigned length)
    {
        if(gVerbosityLevel >= 2)
        {
            std::printf("\n - Parsing didn't fail with invalid identifier ");
            for(unsigned i = 0; i < length; ++i)
                iters[(length-1)-i].print();
            std::printf("\n");
        }
        return false;
    }
}

template<typename Value_t>
static int UTF8TestWithType()
{
    using namespace FUNCTIONPARSERTYPES;

    CharIter iters[4] =
        { CharIter(true, false),
          CharIter(false, true),
          CharIter(false, false),
          CharIter(false, false) };
    std::string identifier;
    FunctionParserBase<Value_t> parser;
    const Value_t value = fp_const_preciseDouble<Value_t>(0);

    for(unsigned length = 1; length <= 4; ++length)
    {
        if(gVerbosityLevel >= 1)
            std::cout << "." << std::flush;
        bool cont = true;
        while(cont)
        {
            identifier.clear();
            for(unsigned i = 0; i < length; ++i)
                iters[i].appendChar(identifier);

            if(parser.Parse(identifier, identifier) >= 0)
                return printUTF8TestError("Parsing", iters, length, identifier);

            if(parser.Eval(&value) != value)
                return printUTF8TestError("Evaluation", iters, length, identifier);

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

    CharIter invalidIters[3] =
        { CharIter(), CharIter(true, false), CharIter() };
    // test 5: inv
    // test 6: inv + normal
    // test 7: normal + inv

    for(unsigned length = 1; length <= 3; ++length)
    {
        if(gVerbosityLevel >= 1)
            std::cout << "." << std::flush;
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

            if(parser.Parse(identifier, identifier) < 0)
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

    return true;
}

static int UTF8Test()
{
    int retval = 1;

#define o(type, enumcode, opt1,opt2, verbosetype) \
    rt_##enumcode( \
        if(gVerbosityLevel >= 3) \
            std::cout << "\n - Testing UTF8 support with FunctionParserBase<" # type "> "; \
        else if(gVerbosityLevel >= 1) std::cout << " " # enumcode; \
        retval &= UTF8TestWithType<type>(); \
        , )
    FP_DECLTYPES(o)
#undef o

    return retval;
}


//=========================================================================
// Test identifier adding and removal
//=========================================================================
template<typename Value_t>
static bool addIdentifier(FunctionParserBase<Value_t>& parser, const std::string& name, int type)
{
    using namespace FUNCTIONPARSERTYPES;

    static FunctionParserBase<Value_t> anotherParser;
    static bool anotherParserInitialized = false;
    if(!anotherParserInitialized)
    {
        anotherParser.Parse("x", "x");
        anotherParserInitialized = true;
    }

    switch(type)
    {
      case 0: return parser.AddConstant(name, fp_const_preciseDouble<Value_t>(123));
      case 1: return parser.AddUnit(name, fp_const_preciseDouble<Value_t>(456));
      case 2: return parser.AddFunction(name, userDefFuncSqr<Value_t>, 1);
      case 3: return parser.AddFunction(name, anotherParser);
    }
    return false;
}

template<typename Value_t>
static int testIdentifiersWithType()
{
    FunctionParserBase<Value_t> fParser;
    std::vector<std::string> identifierNames(26*26, std::string("AA"));

    unsigned nameInd = 0;
    for(int i1 = 0; i1 < 26; ++i1)
    {
        for(int i2 = 0; i2 < 26; ++i2)
        {
            identifierNames.at(nameInd)[0] = char('A' + i1);
            identifierNames[nameInd][1] = char('A' + i2);

            if(!addIdentifier(fParser, identifierNames[nameInd], (i1+26*i2)%3))
            {
                if(gVerbosityLevel >= 2)
                    std::cout << "\n - Failed to add identifier '" << identifierNames[nameInd] << "'\n";
                return false;
            }

            ++nameInd;
        }
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(identifierNames.begin(), identifierNames.end(), g);

    for(unsigned nameInd = 0; nameInd <= identifierNames.size(); ++nameInd)
    {
        for(unsigned removedInd = 0; removedInd < nameInd; ++removedInd)
        {
            if(!addIdentifier(fParser, identifierNames[removedInd], 3))
            {
                if(gVerbosityLevel >= 2)
                    std::cout << "\n - Failure: Identifier '" << identifierNames[removedInd]
                              << "' was still reserved even after removing it.\n";
                return false;
            }
            if(!fParser.RemoveIdentifier(identifierNames[removedInd]))
            {
                if(gVerbosityLevel >= 2)
                    std::cout << "\n - Failure: Removing the identifier '" << identifierNames[removedInd]
                              << "' after adding it again failed.\n";
                return false;
            }
        }

        for(unsigned existingInd = nameInd; existingInd < identifierNames.size(); ++existingInd)
        {
            if(addIdentifier(fParser, identifierNames[existingInd], 3))
            {
                if(gVerbosityLevel >= 2)
                    std::cout << "\n - Failure: Trying to add identifier '" << identifierNames[existingInd]
                              << "' for a second time didn't fail.\n";
                return false;
            }
        }

        if(nameInd < identifierNames.size())
        {
            if(!fParser.RemoveIdentifier(identifierNames[nameInd]))
            {
                if(gVerbosityLevel >= 2)
                    std::cout << "\n - Failure: Trying to remove identifier '"
                              << identifierNames[nameInd] << "' failed.\n";
                return false;
            }
            if(fParser.RemoveIdentifier(identifierNames[nameInd]))
            {
                if(gVerbosityLevel >= 2)
                    std::cout << "\n - Failure: Trying to remove identifier '" << identifierNames[nameInd]
                              << "' for a second time didn't fail.\n";
                return false;
            }
        }
    }

    return true;
}

static int testIdentifiers()
{
    int retval = 1;

#define o(type, enumcode, opt1,opt2, verbosetype) \
    rt_##enumcode( \
        if(gVerbosityLevel >= 3) \
            std::cout << "\n - Testing adding/removing identifiers with FunctionParserBase<" # type "> "; \
        retval &= testIdentifiersWithType<type>(); \
        , )
    FP_DECLTYPES(o)
#undef o

    return retval;
}


//=========================================================================
// Test user-defined functions
//=========================================================================
namespace
{
    template<int VarsAmount>
    DefaultValue_t userFunction(const DefaultValue_t* p)
    {
        DefaultValue_t result = 1.0;
        for(int i = 0; i < VarsAmount; ++i)
            result += (VarsAmount+i/10.0) * p[i];
        return result;
    }

    DefaultValue_t(*userFunctions[])(const DefaultValue_t*) =
    {
        userFunction<0>, userFunction<1>, userFunction<2>, userFunction<3>,
        userFunction<4>, userFunction<5>, userFunction<6>, userFunction<7>,
        userFunction<8>, userFunction<9>, userFunction<10>, userFunction<11>
    };
    const unsigned userFunctionsAmount =
        sizeof(userFunctions) / sizeof(userFunctions[0]);

    DefaultValue_t nestedFunc1(const DefaultValue_t* p)
    {
        return p[0] + 2.0*p[1] + 3.0*p[2];
    }

    DefaultValue_t nestedFunc2(const DefaultValue_t* p)
    {
        const DefaultValue_t params[3] = { -5.0*p[0], -10.0*p[1], -p[0] };
        return p[0] + 4.0*nestedFunc1(params);
    }

    DefaultValue_t nestedFunc3(const DefaultValue_t* p)
    {
        const DefaultValue_t params1[3] = { 2.5*p[0]+2.0, p[2], p[1]/2.5 };
        const DefaultValue_t params2[2] = { p[1] / 1.5 - 1.0, p[0] - 2.5 };
        return nestedFunc1(params1) + nestedFunc2(params2);
    }
}

int testUserDefinedFunctions()
{
    const DefaultValue_t epsilon = testbedEpsilon<DefaultValue_t>();

    DefaultParser nestedParser1, nestedParser2, nestedParser3;
    nestedParser1.Parse("x + 2.0*y + 3.0*z", "x, y, z");
    nestedParser2.AddFunction("nestedFunc1", nestedParser1);
    nestedParser2.Parse("x + 4.0*nestedFunc1(-5.0*x, -10.0*y, -x)", "x,y");
    nestedParser3.AddFunction("nestedFunc1", nestedParser1);
    nestedParser3.AddFunction("nestedFunc2", nestedParser2);
    nestedParser3.Parse("nestedFunc1(2.5*x+2.0, z, y/2.5) + "
                        "nestedFunc2(y/1.5 - 1.0, x - 2.5)", "x,y,z");

    for(int iteration = 0; iteration < 2; ++iteration)
    {
        DefaultValue_t nestedFuncParams[3];
        for(int i = 0; i < 100; ++i)
        {
            nestedFuncParams[0] = -10.0 + 20.0*i/100.0;
            for(int j = 0; j < 100; ++j)
            {
                nestedFuncParams[1] = -10.0 + 20.0*j/100.0;
                for(int k = 0; k < 100; ++k)
                {
                    nestedFuncParams[2] = -10.0 + 20.0*k/100.0;

                    const DefaultValue_t v1 =
                        nestedParser3.Eval(nestedFuncParams);
                    const DefaultValue_t v2 =
                        nestedFunc3(nestedFuncParams);
                    if(std::fabs(v1-v2) > epsilon)
                    {
                        if(gVerbosityLevel >= 2)
                            std::cout
                                << "\n - Nested function test failed with "
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
    DefaultParser userFunctionParsers[userFunctionsAmount];
    DefaultValue_t funcParams[userFunctionsAmount];
    DefaultParser parser1, parser2;

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
            if(gVerbosityLevel >= 2)
                std::cout << "\n - Failed to parse function\n\""
                          << functionString.str() << "\"\nwith parameters: \""
                          << paramString.str() << "\":\n"
                          << userFunctionParsers[funcInd].ErrorMsg() << "\n";
            return false;
        }

        for(unsigned testInd = 0; testInd < 10; ++testInd)
        {
            for(unsigned paramInd = 0; paramInd < testInd; ++paramInd)
                funcParams[paramInd] = testInd+paramInd;
            const DefaultValue_t result = userFunctions[funcInd](funcParams);
            const DefaultValue_t parserResult =
                userFunctionParsers[funcInd].Eval(funcParams);
            if(std::fabs(result - parserResult) > epsilon)
            {
                if(gVerbosityLevel >= 2)
                {
                    std::cout << "\n - Function\n\"" << functionString.str()
                              << "\"\nwith parameters (";
                    for(unsigned paramInd = 0; paramInd < testInd; ++paramInd)
                    {
                        if(paramInd > 0) std::cout << ',';
                        std::cout << funcParams[paramInd];
                    }
                    std::cout << ")\nreturned " << parserResult
                              << " instead of " << result << "\n";
                }
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
            if(gVerbosityLevel >= 2)
                std::cout << "\n - Failed to add user-defined function \""
                          << funcNames[funcInd] << "\".\n";
            return false;
        }
        if(!parser2.AddFunction(funcNames[funcInd],
                                userFunctionParsers[funcInd]))
        {
            if(gVerbosityLevel >= 2)
                std::cout
                    << "\n - Failed to add user-defined function parser \""
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
            if(gVerbosityLevel >= 2)
                std::cout << "\n - parser1 failed to parse function\n\""
                          << functionString.str() << "\":\n"
                          << parser1.ErrorMsg() << "\n";
            return false;
        }
        if(parser2.Parse(functionString.str(), "x") >= 0)
        {
            if(gVerbosityLevel >= 2)
                std::cout << "\n - parser2 failed to parse function\n\""
                          << functionString.str() << "\":\n"
                          << parser2.ErrorMsg() << "\n";
            return false;
        }

        for(unsigned optimizeInd = 0; optimizeInd < 4; ++optimizeInd)
        {
            for(unsigned testInd = 0; testInd < 100; ++testInd)
            {
                const DefaultValue_t x = testInd/10.0;
                DefaultValue_t result = 0.0;
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

                const DefaultValue_t parser1Result = parser1.Eval(&x);
                const DefaultValue_t parser2Result = parser2.Eval(&x);
                const bool parser1Failed =
                    std::fabs(result - parser1Result) > epsilon;
                const bool parser2Failed =
                    std::fabs(result - parser2Result) > epsilon;

                if(parser1Failed || parser2Failed)
                {
                    if(gVerbosityLevel >= 2)
                    {
                        std::cout << "\n - For function:\n\""
                                  << functionString.str() << "\"";
                        if(optimizeInd > 0)
                            std::cout << "\n(Optimized " << optimizeInd
                                      << (optimizeInd > 1 ?
                                          " times)" : " time)");
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
#if defined(FP_USE_THREAD_SAFE_EVAL) || \
    defined(FP_USE_THREAD_SAFE_EVAL_WITH_ALLOCA)
#include <thread>
#include <atomic>

class TestingThread
{
    int mThreadNumber;
    DefaultParser* mFp;
    static std::atomic<bool> mOk;

    static DefaultValue_t function(const DefaultValue_t* vars)
    {
        const DefaultValue_t x = vars[0], y = vars[1];
        return sin(sqrt(x*x+y*y)) + 2*cos(2*sqrt(2*x*x+2*y*y));
    }

 public:
    TestingThread(int n, DefaultParser* fp):
        mThreadNumber(n), mFp(fp)
    {}

    static bool ok()     { return mOk; }
    static void set_ok() { mOk = true; }

    void operator()()
    {
        const DefaultValue_t epsilon = testbedEpsilon<DefaultValue_t>() * 2;
        DefaultValue_t vars[2];
        for(vars[0] = -10.0; vars[0] <= 10.0; vars[0] += 0.02)
        {
            for(vars[1] = -10.0; vars[1] <= 10.0; vars[1] += 0.02)
            {
                if(!mOk) return;

                const DefaultValue_t v1 = function(vars);
                const DefaultValue_t v2 = mFp->Eval(vars);
                /*
                const double scale = pow(10.0, floor(log10(fabs(v1))));
                const double sv1 = fabs(v1) < testbedEpsilon<double>() ? 0 : v1/scale;
                const double sv2 = fabs(v2) < testbedEpsilon<double>() ? 0 : v2/scale;
                const double diff = fabs(sv2-sv1);
                */
                const DefaultValue_t diff =
                    std::fabs(v1) < epsilon ?
                    (std::fabs(v2) < epsilon ?
                     std::fabs(v1 - v2) :
                     std::fabs((v1 - v2) / v2)) :
                    std::fabs((v1 - v2) / v1);

                if(std::fabs(diff) > epsilon)
                {
                    mOk = false;
                    if(gVerbosityLevel >= 2)
                        std::cout << "\n - Thread " << mThreadNumber
                                  << " failed ([" << vars[0] << "," << vars[1]
                                  << "] -> " << v2 << " vs. " << v1 << ")"
                                  << std::endl;
                    return;
                }
            }
        }
    }
};

std::atomic<bool> TestingThread::mOk;

int testMultithreadedEvaluation()
{
    DefaultParser fp;
    fp.Parse("sin(sqrt(x*x+y*y)) + 2*cos(2*sqrt(2*x*x+2*y*y))", "x,y");

    TestingThread::set_ok();

    if(gVerbosityLevel >= 1)
        std::cout << " 1" << std::flush;
    std::thread t1(TestingThread(1, &fp)), t2(TestingThread(2, &fp));
    t1.join();
    t2.join();
    if(!TestingThread::ok()) return false;

    if(gVerbosityLevel >= 1)
        std::cout << " 2" << std::flush;
    std::thread
        t3(TestingThread(3, &fp)), t4(TestingThread(4, &fp)),
        t5(TestingThread(5, &fp)), t6(TestingThread(6, &fp));
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    if(!TestingThread::ok()) return false;

    if(gVerbosityLevel >= 1)
        std::cout << " 3" << std::flush;
    fp.Optimize();
    std::thread
        t7(TestingThread(7, &fp)), t8(TestingThread(8, &fp)),
        t9(TestingThread(9, &fp));
    t7.join();
    t8.join();
    t9.join();
    if(!TestingThread::ok()) return false;

    return true;
}

#else

int testMultithreadedEvaluation()
{
    return -1;
}

#endif

//=========================================================================
// Test variable deduction
//=========================================================================
template<typename OutStream, typename Value_t>
bool checkVarString(const char* idString,
                    FunctionParserBase<Value_t> & fp,
                    unsigned testIndex,
                    int errorIndex,
                    int variablesAmount, const std::string& variablesString,
                    OutStream&    out,
                    std::ostream& briefErrorMessages)
{
    const TestType &testData = getTest(testIndex);

    const bool stringsMatch =
        (variablesString == testData.paramString);
    if(errorIndex >= 0 ||
       variablesAmount != int(testData.paramAmount) ||
       !stringsMatch)
    {
        if(gVerbosityLevel >= 2)
        {
            out << "\n" << idString
                << " ParseAndDeduceVariables() failed with function:\n\""
                << testData.funcString << "\"\n";
            if(errorIndex >= 0)
                out << "Error index: " << errorIndex
                    << ": " << fp.ErrorMsg() << std::endl;
            else if(!stringsMatch)
                out << "Deduced var string was \"" << variablesString
                    << "\" instead of \""
                    << testData.paramString
                    << "\"." << std::endl;
            else
                out << "Deduced variables amount was "
                    << variablesAmount << " instead of "
                    << testData.paramAmount << "."
                    << std::endl;
        }
        else
        {
            briefErrorMessages << "- " << testData.testName
                               << ": Failed ParseAndDeduceVariables().\n";
        }
        return false;
    }
    return true;
}

template<typename OutStream, typename Value_t>
bool testVariableDeduction(FunctionParserBase<Value_t>& fp,
                           unsigned testIndex,
                           OutStream&    out,
                           std::ostream& briefErrorMessages)
{
    static thread_local std::string variablesString;
    static thread_local std::vector<std::string> variables;
    const TestType &testData = getTest(testIndex);

    if(gVerbosityLevel >= 3)
        out << "(Variable deduction)" << std::flush;

    int variablesAmount = -1;
    int retval = fp.ParseAndDeduceVariables
        (testData.funcString, &variablesAmount, testData.useDegrees);
    if(retval >= 0 || variablesAmount != int(testData.paramAmount))
    {
        if(gVerbosityLevel >= 2)
        {
            out <<"\nFirst ParseAndDeduceVariables() failed with function:\n\""
                << testData.funcString << "\"\n";
            if(retval >= 0)
                out << "Error index: " << retval
                          << ": " << fp.ErrorMsg() << std::endl;
            else
                out << "Deduced variables amount was "
                          << variablesAmount << " instead of "
                          << testData.paramAmount << "."
                          << std::endl;
        }
        else
        {
            briefErrorMessages << "- " << testData.testName
                               << ": Failed ParseAndDeduceVariables().\n";
        }
        return false;
    }

    variablesAmount = -1;
    retval = fp.ParseAndDeduceVariables
        (testData.funcString,
         variablesString,
         &variablesAmount,
         testData.useDegrees);
    if(!checkVarString("Second", fp, testIndex, retval, variablesAmount,
                       variablesString, out, briefErrorMessages))
        return false;

    retval = fp.ParseAndDeduceVariables(testData.funcString,
                                        variables,
                                        testData.useDegrees);
    variablesAmount = int(variables.size());
    variablesString.clear();
    for(unsigned i = 0; i < variables.size(); ++i)
    {
        if(i > 0) variablesString += ',';
        variablesString += variables[i];
    }
    return checkVarString("Third", fp, testIndex, retval, variablesAmount,
                          variablesString, out, briefErrorMessages);
}


//=========================================================================
// Main test function
//=========================================================================
namespace
{
    template<typename Value_t>
    void testAgainstDouble(Value_t*, Value_t, unsigned, std::ostream&) {}

#if defined(FP_TEST_WANT_MPFR_FLOAT_TYPE) && defined(FP_TEST_WANT_DOUBLE_TYPE)
    void testAgainstDouble(MpfrFloat* vars, MpfrFloat parserValue,
                           unsigned testIndex,
                           std::ostream& error)
    {
        const TestType &testData = getTest(testIndex);

        if(!testData.hasDouble) return;

        double doubleVars[10];
        for(unsigned i = 0; i < 10; ++i) doubleVars[i] = vars[i].toDouble();

        const double Eps = testbedEpsilon<double>();

        const double v1 = evaluate_test<double>(testIndex, doubleVars);
        const double v2 = parserValue.toDouble();

        /*
        using namespace FUNCTIONPARSERTYPES;
        const double scale = fp_pow(10.0, fp_floor(fp_log10(fp_abs(v1))));
        const double sv1 = fp_abs(v1) < Eps ? 0 : v1/scale;
        const double sv2 = fp_abs(v2) < Eps ? 0 : v2/scale;
        const double diff = fp_abs(sv2-sv1);
        */
        const double diff =
            std::fabs(v1) < Eps ?
            (std::fabs(v2) < Eps ? std::fabs(v1 - v2) :
             std::fabs((v1 - v2) / v2)) :
            std::fabs((v1 - v2) / v1);

        if(diff > Eps)
        {
            using namespace FUNCTIONPARSERTYPES;
            if(gVerbosityLevel >= 2)
                error << std::setprecision(16) << v2 << " instead of "
                      << std::setprecision(16) << v1
                      << "\n(Difference: "
                      << std::setprecision(16) << v2-v1
                      << ", epsilon: "
                      << std::setprecision(16) << Eps
                      << "; scaled diff "
                      << std::setprecision(16) << diff
                      << ")\nwhen tested against the double function.";
            else
                error << std::setprecision(16) << v2 << " vs "
                      << std::setprecision(16) << v1
                      << " (diff: "
                      << std::setprecision(16) << v2-v1
                      << ", sdiff "
                      << std::setprecision(16) << diff
                      << ") against double.";
        }
    }
#endif

    template<typename Value_t>
    void testAgainstLongInt(Value_t*, Value_t, unsigned, std::ostream&) {}

#if defined(FP_TEST_WANT_GMP_INT_TYPE) && defined(FP_TEST_WANT_LONG_INT_TYPE)
    void testAgainstLongInt(GmpInt* vars, GmpInt parserValue, unsigned testIndex,
                            std::ostream& error)
    {
        const TestType &testData = getTest(testIndex);

        if(!testData.hasLong) return;

        long longVars[10];
        for(unsigned i = 0; i < 10; ++i) longVars[i] = vars[i].toInt();

        const long longValue = evaluate_test<long>(testIndex, longVars);
        if(longValue != parserValue)
        {
            if(gVerbosityLevel >= 2)
                error << parserValue << " instead of " << longValue
                      << "\nwhen tested against the long int function.";
            else
                error << parserValue << " vs " << longValue
                      << " against long.";
        }
    }
#endif
}

template<typename Value_t>
const char* getNameForValue_t()
{
    #define o(type, enumcode, opt1,opt2, verbosetype) \
        rt_##enumcode( \
            if(std::is_same<Value_t, type>::value) return #type; \
        , );
    FP_DECLTYPES(o)
    #undef o
    return "unknown value type";
}

template<typename OutStream, typename Value_t>
bool runRegressionTest(FunctionParserBase<Value_t>& fp,
                       unsigned testIndex,
                       const Value_t& paramMin, const Value_t& paramMax, const Value_t& paramStep,
                       const std::string& testType,
                       const Value_t Eps,
                       OutStream&    out,
                       std::ostream& briefErrorMessages)
{
    using namespace FUNCTIONPARSERTYPES;
    const TestType &testData = getTest(testIndex);

    Value_t vars[10];
    Value_t fp_vars[10];

    bool is_complex = IsComplexType<Value_t>::value;
    Value_t complex_1_0 = Value_t(1);
    Value_t complex_0_1 = is_complex ? fp_sqrt((Value_t)(-1)) : 0;
    // ^ This expression constructs 1i in a way that compiles fine in real & integer modes
    Value_t complex_1_1 = complex_0_1 + complex_1_0;
    bool use_complex_stepping = is_complex && fp_imag(paramStep) == Value_t();

    for(unsigned i = 0; i < testData.paramAmount; ++i)
    {
        vars[i] = paramMin;
        if(use_complex_stepping)
        {
            // Instead of e.g. -3, construct the min as -3 + -3i
            vars[i] *= complex_1_1;
        }
    }

    while(true)
    {
        for(unsigned i = 0; i < testData.paramAmount; ++i)
            fp_vars[i] = vars[i];

        if(gVerbosityLevel >= 4)
        {
            out << "Trying (";
            for(unsigned ind = 0; ind < testData.paramAmount; ++ind)
                out << (ind>0 ? ", " : "") << vars[ind];
            out << ")\n" << std::flush;
        }
        const Value_t v1 = evaluate_test<Value_t>(testIndex, vars);
        if(true) /*test Eval() */
        {
            Value_t v2 = fp.Eval(fp_vars);

            std::ostringstream error;

            if(fp.EvalError() > 0)
            {
                error << "EvalError " << fp.EvalError() << " ("
                      << getEvalErrorName(fp.EvalError()) << ")";
            }
            else if(IsIntType<Value_t>::value)
            {
                if(v1 != v2)
                {
                    using namespace FUNCTIONPARSERTYPES;
                    if(gVerbosityLevel >= 2)
                        error << v2 << " instead of " << v1;
                    else
                        error << v2 << " vs " << v1;
                }
                else
                    testAgainstLongInt(vars, v2, testIndex, error);
            }
            else
            {
            #ifdef FP_SUPPORT_COMPLEX_NUMBERS
                if(IsComplexType<Value_t>::value && testData.ignoreImagSign)
                {
                    // Ignore differences in the sign of the imaginary component
                    if(fp_less(fp_imag(v1), fp_real(Value_t{}))
                    != fp_less(fp_imag(v2), fp_real(Value_t{})))
                    {
                        v2 = fp_conj(v2);
                    }
                }
            #endif
                using namespace FUNCTIONPARSERTYPES;
                /*
                const Value_t scale =
                    fp_pow(Value_t(10.0), fp_floor(fp_log10(fp_abs(v1))));
                const Value_t sv1 = fp_abs(v1) < Eps ? 0 : v1/scale;
                const Value_t sv2 = fp_abs(v2) < Eps ? 0 : v2/scale;
                const Value_t diff = fp_abs(sv2-sv1);
                */
                const Value_t diff =
                    fp_abs(v1) < Eps ?
                    (fp_abs(v2) < Eps ? fp_abs(v1 - v2) :
                     fp_abs((v1 - v2) / v2)) :
                    fp_abs((v1 - v2) / v1);
                /*
                const Value_t diff =
                    v1 == Value_t(0) ?
                    (v2 == Value_t(0) ? Value_t(0) :
                     fp_abs((v1 - v2) / v2)) :
                    fp_abs((v1 - v2) / v1);
                */

                if(diff > Eps)
                {
                    using namespace FUNCTIONPARSERTYPES;
                    if(gVerbosityLevel >= 2)
                        error << std::setprecision(28) << v2
                              //   "the library returned "
                              << "\n          instead of "
                              << std::setprecision(28) << v1
                              << "\n(Difference: "
                              << std::setprecision(28) << v2-v1
                              << ", epsilon: "
                              << std::setprecision(28) << Eps
                              << "; scaled diff "
                              << std::setprecision(28) << diff
                              << ")";
                    else
                        error << std::setprecision(16) << v2 << " vs "
                              << std::setprecision(16) << v1
                              << " (diff: "
                              << std::setprecision(16) << v2-v1
                              << ", sdiff "
                              << std::setprecision(16) << diff
                              << ")";
                }
                else
                    testAgainstDouble(vars, v2, testIndex, error);
            }

            if(!error.str().empty())
            {
                if(gVerbosityLevel == 2)
                    out << "\n****************************\nTest "
                        << testData.testName
                        << ", function:\n\"" << testData.funcString
                        << "\"\n(" << getNameForValue_t<Value_t>() << testType << ")";

                if(gVerbosityLevel >= 2)
                {
                    using namespace FUNCTIONPARSERTYPES;
                    // ^ For output of complex numbers according to fparser practice

                    out << "\nError: For (" << std::setprecision(20);
                    for(unsigned ind = 0; ind < testData.paramAmount; ++ind)
                        out << (ind>0 ? ", " : "") << vars[ind];
                    out << ")\nthe library returned " << error.str()
                        << '\n';
#ifdef FUNCTIONPARSER_SUPPORT_DEBUGGING
                    fp.PrintByteCode(std::cout);
#endif
                    out << std::flush;
                }
                else
                {
                    using namespace FUNCTIONPARSERTYPES;

                    out << "";         // lock
                    briefErrorMessages << "- " << testData.testName << " (";
                    for(unsigned ind = 0; ind < testData.paramAmount; ++ind)
                        briefErrorMessages << (ind>0 ? "," : "") << vars[ind];
                    briefErrorMessages << "): " << error.str() << "\n";
                    out << std::flush; // unlock
                }
                return false;
            }
        } /* test Eval() */

        unsigned paramInd = 0;
        while(paramInd < testData.paramAmount)
        {
            using namespace FUNCTIONPARSERTYPES;
            /* ^ Import a possible <= operator from that
             *   namespace for this particular comparison only */
            vars[paramInd] += paramStep;
            if(use_complex_stepping)
            {
                if(fp_real(vars[paramInd]) <= fp_real(paramMax))
                    break;
                // Reset the real-axis component to minimum
                // and step the imag-axis
                auto imagOnly = (vars[paramInd] - fp_conj(vars[paramInd]))
                                / Value_t(2);
                vars[paramInd] = fp_real(paramMin) + imagOnly + complex_0_1 * paramStep;
                if(fp_imag(vars[paramInd]) <= fp_real(paramMax))
                {
                    break;
                }
            }
            else
            {
                if(vars[paramInd] <= paramMax) break;
                vars[paramInd] = paramMin;
            }
            ++paramInd;
        }
        if(paramInd == testData.paramAmount) break;
    }
    return true;
}

bool IsSelectedTest(const char* testName)
{
    for(std::size_t a=0; a<gSelectedRegressionTests.size(); ++a)
        if(WildMatch_Dirmask(gSelectedRegressionTests[a], testName))
            return true;
    return false;
}

struct locked_cout
{
    locked_cout(std::mutex& lock) : lk(lock)
    {
        lk.unlock();
    }

    template<typename t> // Using lowercase t because testbed_tests.inc defines T identifier.
    locked_cout& operator<< (t&& value)
    {
        if(!lk.owns_lock())
        {
            lk.lock();
        }
        std::cout << value;
        return *this;
    }

    std::unique_lock<std::mutex> lk;
};

locked_cout& operator<< (locked_cout& out, std::ostream& (*pf)(std::ostream&))
{
    std::cout << pf;
    if(out.lk.owns_lock())
    {
        out.lk.unlock();
    }
    return out;
}

template<typename Value_t>
bool runRegressionTests(unsigned n_threads,
                        unsigned thread_index,
                        std::mutex& print_lock,
                        std::ostream& briefErrorMessages,
                        std::string& prev_test_prefix,
                        std::atomic<unsigned>& test_counter)
{
    using namespace FUNCTIONPARSERTYPES;
    // Setup the function parser for testing
    // -------------------------------------
    FunctionParserBase<Value_t> fp;
    locked_cout out(print_lock);

    bool ret = fp.AddConstant("pi", fp_const_pi<Value_t>());
    ret = ret && fp.AddConstant("naturalnumber", fp_const_e<Value_t>());
    ret = ret && fp.AddConstant("logtwo", fp_const_log2<Value_t>());
    ret = ret && fp.AddConstant("logten", fp_const_log10<Value_t>());
    ret = ret && fp.AddConstant("CONST", fp_const_preciseDouble<Value_t>(CONST));
    if(!ret)
    {
        if(thread_index == 0)
            out << "Ooops! AddConstant() didn't work" << std::endl;
        return false;
    }

    ret = fp.AddUnit("doubled", 2);
    ret = ret && fp.AddUnit("tripled", 3);
    if(!ret)
    {
        if(thread_index == 0)
            out << "Ooops! AddUnit() didn't work" << std::endl;
        return false;
    }

    ret = fp.AddFunctionWrapper
        ("sub", UserDefFuncWrapper<Value_t>(userDefFuncSub<Value_t>), 2);
    ret = ret && fp.AddFunction("sqr", userDefFuncSqr<Value_t>, 1);
    ret = ret && fp.AddFunction("value", userDefFuncValue<Value_t>, 0);
    if(!ret)
    {
        if(thread_index == 0)
            out << "Ooops! AddFunction(ptr) didn't work" << std::endl;
        return false;
    }

    UserDefFuncWrapper<Value_t>* wrapper =
        dynamic_cast<UserDefFuncWrapper<Value_t>*>
        (fp.GetFunctionWrapper("sub"));
    if(!wrapper || wrapper->counter() != 0)
    {
        if(thread_index == 0)
            out << "Ooops! AddFunctionWrapper() didn't work" << std::endl;
        return false;
    }

    FunctionParserBase<Value_t> SqrFun, SubFun, ValueFun;
    if(gVerbosityLevel >= 3 && thread_index == 0) out << "Parsing SqrFun... ";
    SqrFun.Parse("x*x", "x");
    if(gVerbosityLevel >= 3 && thread_index == 0) out << "\nParsing SubFun... ";
    SubFun.Parse("x-y", "x,y");
    if(gVerbosityLevel >= 3 && thread_index == 0) out << "\nParsing ValueFun... ";
    ValueFun.Parse("5", "");
    if(gVerbosityLevel >= 3 && thread_index == 0) out << std::endl;

    ret = fp.AddFunction("psqr", SqrFun);
    ret = ret && fp.AddFunction("psub", SubFun);
    ret = ret && fp.AddFunction("pvalue", ValueFun);
    if(!ret)
    {
        if(thread_index == 0)
            out << "Ooops! AddFunction(parser) didn't work" << std::endl;
        return false;
    }

    // Test repeated constant addition
    // -------------------------------
   {using namespace FUNCTIONPARSERTYPES; // For a possible custom < operator
    for(Value_t value = 1; value <= Value_t(20); value += 1l)
    {
        Value_t value2 = value;
        if(value != value2)
        {
            out << "Ooops! Value_t does not retain value when copied!" << std::endl;
        }
        if(!fp.AddConstant("TestConstant", value))
        {
            if(thread_index == 0)
                out << "Ooops2! AddConstant() didn't work" << std::endl;
            return false;
        }

        fp.Parse("TestConstant", "");
        Value_t result = fp.Eval(nullptr);
        if(result != value)
        {
            if(thread_index == 0)
            {
                if(value == Value_t(0))
                    out << "Usage of 'TestConstant' failed: got " << result << ", wanted " << value << std::endl;
                else
                    out << "Changing the value of 'TestConstant' failed: got " << result << ", wanted " << value << std::endl;
            }
            return false;
        }
    }}

    bool allRegressionTestsOk = true;

    constexpr unsigned maxtests = sizeof(RegressionTests<Value_t>::Tests)
                                / sizeof(*RegressionTests<Value_t>::Tests);
    for(;;) // unsigned i = 0; i < maxtests; ++i)
    {
        unsigned i = test_counter++;
        if(i >= maxtests) break;

        unsigned testIndex = RegressionTests<Value_t>::Tests[i];
        const TestType& testData = getTest(testIndex);
        if(!IsSelectedTest(testData.testName)) continue;

        const Value_t paramMin = (fp.Parse(testData.paramMin, ""),fp.Eval(nullptr));
        const Value_t paramMax = (fp.Parse(testData.paramMax, ""),fp.Eval(nullptr));
        const Value_t paramStep = (fp.Parse(testData.paramStep, ""),fp.Eval(nullptr));

        const int retval =
            fp.Parse(testData.funcString, testData.paramString,
                     testData.useDegrees);
        if(retval >= 0)
        {
            out <<
                "With FunctionParserBase<" << getNameForValue_t<Value_t>() << ">"
                "\nin \"" << testData.funcString <<
                "\" (\"" << testData.paramString <<
                "\"), col " << retval <<
                ":\n" << fp.ErrorMsg() << std::endl;
            return false;
        }

        //fp.PrintByteCode(std::cout);
        if(gVerbosityLevel >= 3)
        {
            Value_t n_testvalues = (paramMax - paramMin) / paramStep;
            bool use_complex_stepping = IsComplexType<Value_t>::value
                                     && fp_imag(paramStep) == Value_t();
            if(use_complex_stepping)
            {
                // Parameter space is squared
                n_testvalues *= n_testvalues;
            }
            out << /*std::right <<*/ std::setw(2)
                << testData.testName << ": \""
                << testData.funcString << "\" ("
                << std::pow(makeLongInteger(n_testvalues),
                            (long)testData.paramAmount)
                << " param. combinations): " << std::flush;
        }
        else if(gVerbosityLevel == 2)
        {
            out << "";
            const char* tn = testData.testName;
            const char* p = std::strrchr(tn, '/');
            if(!p)
                { prev_test_prefix = ""; out << tn; }
            else
            {
                std::string path_prefix(tn, p-tn);
                if(path_prefix == prev_test_prefix)
                    out << (p+1);
                else
                    { if(!prev_test_prefix.empty()) out << std::endl;
                      out << tn;
                      prev_test_prefix = path_prefix; }
            }
            out << ' ' << std::flush;
        }

        bool thisTestOk =
            runRegressionTest(fp, testIndex,
                              paramMin, paramMax, paramStep,
                              ", not optimized",
                              testbedEpsilon<Value_t>(),
                              out, briefErrorMessages);

        if(thisTestOk)
        {
            if(gVerbosityLevel >= 3) out << "Ok." << std::endl;

            fp.Optimize();
            //fp.PrintByteCode(out);

            if(gVerbosityLevel >= 3)
                out << "    Optimized: " << std::flush;

            thisTestOk =
                runRegressionTest(fp, testIndex,
                                  paramMin, paramMax, paramStep,
                                  ", after optimization",
                                  testbedEpsilon<Value_t>(), out, briefErrorMessages);
            if(thisTestOk)
            {
                if(gVerbosityLevel >= 3)
                    out << "(Calling Optimize() several times) "
                              << std::flush;

                for(int j = 0; j < 20; ++j)
                    fp.Optimize();

                /* Sometimes literals drift when the optimizer is run many
                   times, which can become significant with floats. The only
                   purpose to test running the optimizer several times is just
                   to see that it doesn't break. It's not intended to be called
                   several times normally. Hence just skip testing with floats,
                   because the drift just causes differences larger than
                   epsilon...
                */
                if(!std::is_same<Value_t, float>::value)
                {
                    thisTestOk =
                        runRegressionTest
                        (fp, testIndex,
                         paramMin, paramMax, paramStep,
                         ", after several optimization runs",
                         testbedEpsilon<Value_t>(), out, briefErrorMessages);
                }

                if(thisTestOk)
                {
                    thisTestOk =
                        testVariableDeduction(fp, testIndex, out, briefErrorMessages);

                    if(thisTestOk && gVerbosityLevel >= 3)
                        out << "Ok." << std::endl;
                }
            }
        } // if(thisTestOk)

        if(!thisTestOk) allRegressionTestsOk = false;

        if(gVerbosityLevel == 1)
            out << (thisTestOk ? "." : "!") << std::flush;
    } // for(unsigned i = 0; i < maxtests; ++i)

    if(gVerbosityLevel >= 2 && n_threads == 1)
        out << "User-defined function \"sub\" was called "
            << (dynamic_cast<UserDefFuncWrapper<Value_t>*>
                (fp.GetFunctionWrapper("sub"))->counter())
            << " times." << std::endl;

    return allRegressionTestsOk;
}

template<typename Value_t>
bool runRegressionTests(unsigned n_threads = std::thread::hardware_concurrency())
{
#if !(defined(FP_USE_THREAD_SAFE_EVAL) || \
      defined(FP_USE_THREAD_SAFE_EVAL_WITH_ALLOCA))
    n_threads = 1;
#endif

    if(gVerbosityLevel >= 1)
    {
        setAnsiBold();
        std::cout << "==================== Parser type \"" << getNameForValue_t<Value_t>()
                  << "\" ====================" << std::endl;
        resetAnsiColor();
    }
    if(gVerbosityLevel >= 3)
    {
        n_threads = 1;
    }

    std::vector<std::future<bool>> test_runners;
    std::ostringstream briefErrorMessages;
    std::string prev_test_prefix;
    std::mutex print_lock;
    std::atomic<unsigned> test_counter{};

    test_runners.reserve(n_threads);
    for(unsigned n=0; n<n_threads; ++n)
        test_runners.emplace_back(std::async(std::launch::async,
            [=,&print_lock,&briefErrorMessages,&prev_test_prefix,&test_counter]{
            return runRegressionTests<Value_t>(n_threads, n, print_lock, briefErrorMessages, prev_test_prefix, test_counter);
        }));

    bool allRegressionTestsOk = true;
    for(auto& v: test_runners)
        if(!v.get())
            allRegressionTestsOk = false;

    if(allRegressionTestsOk)
    {
        if(gVerbosityLevel == 1 || gVerbosityLevel == 2)
            std::cout << std::endl;
    }
    else if(gVerbosityLevel <= 1)
    {
        if(gVerbosityLevel == 1) std::cout << std::endl;
        std::cout << briefErrorMessages.str() << std::flush;
    }
    return allRegressionTestsOk;
}


//=========================================================================
// Optimizer tests
//=========================================================================
namespace OptimizerTests
{
    // --------------------------------------------------------------------
    // Optimizer test 1
    // --------------------------------------------------------------------
    /* Tests functions of the form "A(x^B)^C op D(x^E)^F", where:
       - A,D = {sin,cos,tan,sinh,cosh,tanh,exp}
       - B,E = {1,2}
       - C,F = {-2,-1,0,1,2}
       - op = +, *
    */
    struct MathFuncData
    {
        DefaultValue_t (*mathFunc)(DefaultValue_t d);
        const char* funcName;
    };

    const MathFuncData mathFuncs[] =
    {
        { &std::sin, "sin" }, { &std::cos, "cos" }, { &std::tan, "tan" },
        { &std::sinh, "sinh" }, { &std::cosh, "cosh" }, { &std::tanh, "tanh" },
        { &std::exp, "exp" }
    };
    const unsigned mathFuncsAmount = sizeof(mathFuncs) / sizeof(mathFuncs[0]);

    unsigned mathFuncIndexA, mathFuncIndexD;
    int exponent_B, exponent_E;
    int exponent_C, exponent_F;
    unsigned operatorIndex;

    DefaultValue_t evaluateFunction(const DefaultValue_t* params)
    {
        const DefaultValue_t x = params[0];
        const MathFuncData& data1 = mathFuncs[mathFuncIndexA];
        const MathFuncData& data2 = mathFuncs[mathFuncIndexD];

        const DefaultValue_t angle1 =
            (exponent_B == 1 ? x : std::pow(x, exponent_B));
        const DefaultValue_t angle2 =
            (exponent_E == 1 ? x : std::pow(x, exponent_E));
        const DefaultValue_t part1 =
            std::pow(data1.mathFunc(angle1), exponent_C);
        const DefaultValue_t part2 =
            std::pow(data2.mathFunc(angle2), exponent_F);

        if(operatorIndex == 0) return part1 + part2;
        return part1 * part2;
    }

    bool runCurrentTrigCombinationTest()
    {
        const MathFuncData& data1 = mathFuncs[mathFuncIndexA];
        const MathFuncData& data2 = mathFuncs[mathFuncIndexD];

        std::ostringstream os;
        os << data1.funcName << "(x^" << exponent_B << ")^" << exponent_C;
        if(operatorIndex == 0) os << "+";
        else os << "*";
        os << data2.funcName << "(x^" << exponent_E << ")^" << exponent_F;
        const std::string funcString = os.str();

        customtest = TestType
        {
            "'trig. combo optimizer test'",
            funcString.c_str(),
            "x",
            "-4.0","4.0","0.49",
            1,
            false,/*degrees*/
            true, /*double*/
            false,/*long*/
            false /*ignore imaginary sign*/
        };
        const auto& testData = customtest;
        unsigned testIndex = customtest_index;

        DefaultParser parser;

        const DefaultValue_t paramMin = (parser.Parse(testData.paramMin, ""),parser.Eval(nullptr));
        const DefaultValue_t paramMax = (parser.Parse(testData.paramMax, ""),parser.Eval(nullptr));
        const DefaultValue_t paramStep = (parser.Parse(testData.paramStep, ""),parser.Eval(nullptr));

        if(parser.Parse(funcString, "x") >= 0)
        {
            std::cout << "Oops: Function \"" << funcString
                      << "\" was malformed." << std::endl;
            return false;
        }

        std::ostringstream briefErrorMessages;

        if(!runRegressionTest(parser, testIndex,
                              paramMin, paramMax, paramStep,
                              ", default type",
                              testbedEpsilon<DefaultValue_t>(), std::cout, briefErrorMessages))
        {
            if(gVerbosityLevel == 1)
                std::cout << "\n - " << briefErrorMessages.str() << std::flush;
            return false;
        }
        return true;
    }

    bool runTrigCombinationTests()
    {
        unsigned testCounter = 0;

        for(mathFuncIndexA = 0;
            mathFuncIndexA < mathFuncsAmount;
            ++mathFuncIndexA)
        {
            for(mathFuncIndexD = 0;
                mathFuncIndexD < mathFuncsAmount;
                ++mathFuncIndexD)
            {
                for(exponent_B = 1; exponent_B <= 2; ++exponent_B)
                {
                    for(exponent_E = 1; exponent_E <= 2; ++exponent_E)
                    {
                        for(exponent_C = -2; exponent_C <= 2; ++exponent_C)
                        {
                            for(exponent_F = -2; exponent_F <= 2; ++exponent_F)
                            {
                                for(operatorIndex = 0;
                                    operatorIndex < 2;
                                    ++operatorIndex)
                                {
                                    ++testCounter;
                                    if(!runCurrentTrigCombinationTest())
                                        return false;
                                }
                            }
                        }
                    }
                }
            }
        }

        if(gVerbosityLevel >= 1)
            std::cout << " (" << testCounter << ")" << std::flush;
        return true;
    }


    // --------------------------------------------------------------------
    // Optimizer test 2
    // --------------------------------------------------------------------
    /* Tests functions of the form "A op B [op C]", where
       A, B, C = { var, !var, !!var, var comp value }
       var = A -> x, B -> y, C -> z
       comp = { <, <=, =, !=, >, >= }
       value = { -1, -.5, 0, .5, 1 }
       op = { and, or, not and, not or }
    */
    // opIndex = 0-32 for doubles, 0-20 for ints
    const char* getOperandString(char expression[9], char varName, unsigned opIndex)
    {
        if(opIndex <= 2)
        {
            expression[0] = '!';
            expression[1] = '!';
            expression[2] = varName;
            expression[3] = '\0';
            return expression + (2 - opIndex);
        }

        opIndex -= 3;
        const unsigned compIndex = opIndex % 6, valueIndex = opIndex / 6;
        assert(valueIndex <= 4);

        static const char* const comp[] =
            { "< ", "<=", "= ", "!=", "> ", ">=" };
        static const char* const value[] =
            { "-1 ", "0  ", "1  ", ".5 ", "-.5" };

        expression[0] = '(';
        expression[1] = varName;
        expression[2] = comp[compIndex][0];
        expression[3] = comp[compIndex][1];
        expression[4] = value[valueIndex][0];
        expression[5] = value[valueIndex][1];
        expression[6] = value[valueIndex][2];
        expression[7] = ')';
        expression[8] = '\0';
        return expression;
    }

    template<typename Value_t>
    Value_t getOperandValue(Value_t varValue, unsigned opIndex)
    {
        using namespace FUNCTIONPARSERTYPES;

        switch(opIndex)
        {
          case 0: return varValue;
          case 1: return fp_not(varValue);
          case 2: return fp_truth(varValue);
        }

        opIndex -= 3;
        const unsigned compIndex = opIndex % 6, valueIndex = opIndex / 6;

        static const Value_t value[] =
            { -1, 0, 1, Value_t(.5), Value_t(-.5) };

        switch(compIndex)
        {
          case 0: return fp_less(varValue, value[valueIndex]);
          case 1: return fp_lessOrEq(varValue, value[valueIndex]);
          case 2: return fp_equal(varValue, value[valueIndex]);
          case 3: return fp_nequal(varValue, value[valueIndex]);
          case 4: return fp_greater(varValue, value[valueIndex]);
          case 5: return fp_greaterOrEq(varValue, value[valueIndex]);
        }
        assert(false);
        return 0;
    }

    // exprIndex = 0-3
    std::string getBooleanExpression(const std::string& operand1,
                                     const std::string& operand2,
                                     unsigned exprIndex)
    {
        switch(exprIndex)
        {
          case 0: return operand1 + "&" + operand2;
          case 1: return operand1 + "|" + operand2;
          case 2: return "!(" + operand1 + "&" + operand2 + ")";
          case 3: return "!(" + operand1 + "|" + operand2 + ")";
        }
        assert(false);
        return "";
    }

    template<typename Value_t>
    Value_t getBooleanValue(Value_t operand1Value, Value_t operand2Value,
                            unsigned exprIndex)
    {
        using namespace FUNCTIONPARSERTYPES;
        switch(exprIndex)
        {
          case 0: return fp_and(operand1Value, operand2Value);
          case 1: return fp_or(operand1Value, operand2Value);
          case 2: return fp_not(fp_and(operand1Value, operand2Value));
          case 3: return fp_not(fp_or(operand1Value, operand2Value));
        }
        assert(false);
        return 0;
    }

    bool updateIndices(unsigned* operandIndices, unsigned* exprIndices,
                       unsigned operands, const unsigned maxOperandIndex)
    {
        for(unsigned oi = 0; oi < operands; ++oi)
        {
            if(++operandIndices[oi] <= maxOperandIndex)
                return true;
            operandIndices[oi] = 0;
        }
        for(unsigned ei = 0; ei < operands-1; ++ei)
        {
            if(++exprIndices[ei] <= 3) return true;
            exprIndices[ei] = 0;
        }
        return false;
    }

    template<typename Value_t, unsigned varsAmount>
    bool runBooleanComparisonEvaluation(const unsigned* operandIndices,
                                        const unsigned* exprIndices,
                                        const unsigned operands,
                                        FunctionParserBase<Value_t>& fparser,
                                        const std::string& functionString,
                                        bool optimized)
    {
        const bool isIntegral = FUNCTIONPARSERTYPES::IsIntType<Value_t>::value;
        const unsigned varValuesToTest = isIntegral ? 3 : 5;

        static const Value_t values[] =
            { -1, 0, 1, Value_t(0.5), Value_t(-0.5) };

        unsigned valueIndices[varsAmount] = {};
        Value_t variableValues[varsAmount] = {};
        for(unsigned i = 0; i < operands; ++i) valueIndices[i] = 0;

        bool stop = false;
        while(!stop)
        {
            for(unsigned i = 0; i < operands; ++i)
                variableValues[i] = values[valueIndices[i]];

            const Value_t parserValue = fparser.Eval(variableValues);

            Value_t correctValue = getOperandValue(variableValues[0],
                                                   operandIndices[0]);

            for(unsigned i = 1; i < operands; ++i)
                correctValue =
                    getBooleanValue(correctValue,
                                    getOperandValue(variableValues[i],
                                                    operandIndices[i]),
                                    exprIndices[i-1]);

            if(FUNCTIONPARSERTYPES::fp_nequal(parserValue, correctValue))
            {
                const bool isIntegral =
                    FUNCTIONPARSERTYPES::IsIntType<Value_t>::value;
                if(gVerbosityLevel >= 2)
                {
                    using namespace FUNCTIONPARSERTYPES;
                    std::cout
                        << "\nFor function \"" << functionString
                        << "\" (";
                    for(unsigned i = 0; i < operands; ++i)
                        std::cout << (i>0 ? "," : "")
                                  << variableValues[i];
                    std::cout
                        << "): Parser<"
                        << (isIntegral ? "long" : "double")
                        << ">"
                        << (optimized ? " (optimized)" : "")
                        << "\nreturned " << parserValue
                        << " instead of " << correctValue
                        << std::endl;
#ifdef FUNCTIONPARSER_SUPPORT_DEBUGGING
                    fparser.PrintByteCode(std::cout);
#endif
                }
                else if(gVerbosityLevel >= 1)
                {
                    using namespace FUNCTIONPARSERTYPES;
                    std::cout << "<" << (isIntegral ? "long" : "double");
                    std::cout << (optimized ? ",optimized" : "");
                    std::cout << ">\"" << functionString
                              << "\"(";
                    for(unsigned i = 0; i < operands; ++i)
                        std::cout << (i>0 ? "," : "")
                                  << variableValues[i];
                    std::cout << "): ";
                    std::cout << parserValue << " vs " << correctValue;
                    std::cout << "\n";
                }
                return false;
            }

            stop = true;
            for(unsigned i = 0; i < operands; ++i)
            {
                if(++valueIndices[i] < varValuesToTest)
                    { stop = false; break; }
                valueIndices[i] = 0;
            }
        }

        return true;
    }

    template<typename Value_t>
    bool runBooleanComparisonTestsForType()
    {
        const bool isIntegral = FUNCTIONPARSERTYPES::IsIntType<Value_t>::value;
        const unsigned maxOperandIndex = isIntegral ? 20 : 32;

        static const char varNames[] = { 'x', 'y', 'z' };
        static const char varString[] = "x,y,z";
        const unsigned varsAmount = sizeof(varNames) / sizeof(varNames[0]);

        unsigned operandIndices[varsAmount];
        unsigned exprIndices[varsAmount - 1];

        unsigned testCounter = 0;
        FunctionParserBase<Value_t> fparser;

        bool errors = false;

        for(unsigned operands = 2; operands <= varsAmount; ++operands)
        {
            for(unsigned i = 0; i < operands; ++i) operandIndices[i] = 0;
            for(unsigned i = 0; i < operands-1; ++i) exprIndices[i] = 0;

            do
            {
                // Generate function string:
                char operandBuffer[9];
                std::string functionString =
                    getOperandString(operandBuffer, varNames[0], operandIndices[0]);

                for(unsigned i = 1; i < operands; ++i)
                {
                    functionString =
                        getBooleanExpression
                            (i == 1 ? functionString : "(" + functionString + ")",
                             getOperandString(operandBuffer, varNames[i], operandIndices[i]),
                             exprIndices[i-1]);
                }

                //std::cout << '"' << functionString << "\"\n";

                // Parse function string:
                int errorIndex = fparser.Parse(functionString, varString);
                if(errorIndex >= 0)
                {
                    std::cout << "\nOops! Function \"" << functionString
                              << "\" was malformed.\n";
                    return false;
                }

                // Evaluate function and test for correctness:
                if(!runBooleanComparisonEvaluation<Value_t, varsAmount>
                   (operandIndices, exprIndices, operands,
                    fparser, functionString, false))
                {
                    if (gVerbosityLevel < 1) return false;
                    errors = true;
                }

                fparser.Optimize();

                if(!runBooleanComparisonEvaluation<Value_t, varsAmount>
                   (operandIndices, exprIndices, operands,
                    fparser, functionString, true))
                {
                    if (gVerbosityLevel < 1) return false;
                    errors = true;
                }

                ++testCounter;
            }
            while(updateIndices(operandIndices, exprIndices,
                                operands, maxOperandIndex));
        }
        if(errors) return false;

        if(gVerbosityLevel >= 1)
            std::cout << " (" << testCounter << ")" << std::flush;

        return true;
    }
}

int testOptimizer1()
{
    return OptimizerTests::runTrigCombinationTests();
}

int testOptimizer2()
{
    return OptimizerTests::runBooleanComparisonTestsForType<DefaultValue_t>();
}

int testOptimizer3()
{
#ifdef FP_SUPPORT_LONG_INT_TYPE
    return OptimizerTests::runBooleanComparisonTestsForType<long>();
#else
    return -1;
#endif
}


//=========================================================================
// Help output
//=========================================================================
void printAvailableTests(std::vector<std::string>& tests)
{
    std::cout << "Available tests:\n";
    std::size_t column=0;
    std::string prev_test_prefix;

    bool counting_tests = false;
    long last_count     = 0, count_length = 0;

    for(std::size_t a=0; a<tests.size(); ++a)
    {
        std::string tn = tests[a];
        std::size_t p = tn.rfind('/');
        if(p == tn.npos)
            prev_test_prefix = "";
        else
        {
            std::string path_prefix(tn, 0, p);
            if(path_prefix != prev_test_prefix)
            {
                if(counting_tests && count_length > 1)
                {
                    std::ostringstream tmp; tmp << "-" << last_count;
                    std::cout << tmp.str(); column += tmp.str().size();
                }
                counting_tests = false;
                if(column) { std::cout << std::endl; column=0; }
                prev_test_prefix = path_prefix;
                std::cout << "    " << path_prefix << "/\n";
            }
            tn.erase(0, p+1);
        }
        if(column+tn.size() >= 76) { column=0; std::cout << "\n"; }
        if(column==0) { std::cout << "        "; column+=8; }
        else { std::cout << " "; column+=1; }

        /* TODO: Rewrite this such that backspaces are not needed,
         *       because they don't work with util-linux's "more"
         */
        char* endptr = 0;
        long val = strtol(tn.c_str(), &endptr, 10);
        if(!*endptr)
        {
            if(!counting_tests)
            {
                counting_tests = true; count_length = 1; last_count = val;
            }
            else if(val == last_count+1)
            {
                ++count_length;
                last_count = val; std::cout << "\b"; --column; continue;
            }
            else if(count_length > 1)
            {
                std::ostringstream tmp; tmp << "\b-" << last_count << " ";
                std::cout << tmp.str(); column += tmp.str().size();
                counting_tests = false;
            }
            else counting_tests = false;
        }
        else if(counting_tests && count_length > 1)
        {
            std::ostringstream tmp; tmp << "\b-" << last_count << " ";
            std::cout << tmp.str(); column += tmp.str().size();
            counting_tests = false;
        }
        else counting_tests = false;

        std::cout << tn;
        column += tn.size();
    }
    if(column) std::cout << std::endl;
}

//=========================================================================
// Main
//=========================================================================
int main(int argc, char* argv[])
{
#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    MpfrFloat::setDefaultMantissaBits(96);
#endif
#ifdef FP_SUPPORT_GMP_INT_TYPE
    GmpInt::setDefaultNumberOfBits(80);
#endif

    bool skipSlowAlgo = false;
    bool runAllTypes = true;
    bool runAlgoTests = true;
    #define o(type, enumcode, opt1,opt2, verbosetype) \
        bool run_##opt1 = false;
    FP_DECLTYPES(o)
    #undef o
    unsigned runAlgoTest = 0;

    for(int i = 1; i < argc; ++i)
    {
        if(std::strcmp(argv[i], "-q") == 0) gVerbosityLevel -= 1; // becomes 0
        else if(std::strcmp(argv[i], "-v") == 0) gVerbosityLevel += 1; // becomes 2
        else if(std::strcmp(argv[i], "-vv") == 0) gVerbosityLevel += 2; // becomes 3
        else if(std::strcmp(argv[i], "-vvv") == 0) gVerbosityLevel += 3; // becomes 4
        else if(std::strcmp(argv[i], "-noalgo") == 0) runAlgoTests = false;
        else if(std::strcmp(argv[i], "-skipSlowAlgo") == 0) skipSlowAlgo = true;
        else if(std::strcmp(argv[i], "-algo") == 0)
        {
            if(i+1 < argc) runAlgoTest = std::atoi(argv[++i]);
            runAlgoTests = true;
        }
        else if(std::strcmp(argv[i], "-tests") == 0)
        {
            runAlgoTests = false;

            std::vector<std::string> tests;
        #define o(type, enumcode, opt1,opt2, verbosetype) \
            rt_##enumcode(\
                if(runAllTypes || run_##opt1) \
                for(unsigned t: RegressionTests<type>::Tests) \
                    tests.push_back(AllTests[t].testName); \
            , )
            FP_DECLTYPES(o)
        #undef o
            std::sort(tests.begin(), tests.end(), natcomp);
            tests.erase(std::unique(tests.begin(), tests.end()), tests.end());

            if(!argv[i+1] || std::strcmp(argv[i+1], "help") == 0)
            {
                printAvailableTests(tests);
                return 0;
            }
            while(i+1 < argc && argv[i+1][0] != '-')
            {
                const char* t = argv[++i];
                if(std::none_of(tests.begin(), tests.end(),
                    [t](const std::string& e) { return WildMatch_Dirmask(t, e.c_str()); }))
                {
                    std::cout << "No such test: " << t
                              << "\n\"testbed -tests help\" to list "
                              << "available tests.\n";
                    return -1;
                }
                gSelectedRegressionTests.push_back(t);
            }
        }
        #define o(type, enumcode, opt1,opt2, verbosetype) \
            else if(        std::strcmp(argv[i], "-" #opt1) == 0 \
            || (#opt2[0] && std::strcmp(argv[i], "-" #opt2) == 0)) \
                runAllTypes = false, run_##opt1 = true;
        FP_DECLTYPES(o)
        #undef o
        else if(std::strcmp(argv[i], "--help") == 0
             || std::strcmp(argv[i], "-help") == 0
             || std::strcmp(argv[i], "-h") == 0
             || std::strcmp(argv[i], "/?") == 0)
        {
            std::cout <<
                "FunctionParser testbed " << kVersionNumber <<
                "\n\nUsage: " << argv[0] << " [<option> ...]\n"
                "\n"
                "    -q                Quiet (no progress, brief error reports)\n"
                "    -v                Verbose (progress, full error reports)\n"
                "    -vv               Very verbose\n"
                "    -tests <tests>    Select tests to perform, wildcards ok (implies -noalgo)\n"
                "                      Example: -tests 'cmp*'\n"
                "    -tests help       List available tests\n";
            #define o(type, enumcode, opt1,opt2, verbosetype) do { \
                std::string optstr = "-" #opt1; \
                if(#opt2[0]) optstr += ", -" # opt2; \
                std::cout << "    " << std::left << std::setw(18) << optstr \
                          << "Test " #type " datatype.\n"; \
            } while(0);
            FP_DECLTYPES(o)
            #undef o
            std::cout <<
                "    -algo <n>         Run only algorithmic test <n>\n"
                "    -noalgo           Skip all algorithmic tests\n"
                "    -skipSlowAlgo     Skip slow algorithmic tests\n"
                "    -h, --help        This help\n";
            return 0;
        }
        else if(std::strlen(argv[i]) > 0)
        {
            std::cout << "Unknown option: '" << argv[i] << "'\n";
            return 1;
        }
    }

    if(gSelectedRegressionTests.empty())
        gSelectedRegressionTests.push_back("*");

    DefaultParser fp0;

    // Test that the parser doesn't crash if Eval() is called before Parse():
    fp0.Eval(nullptr);

    const char* const delimiterTestFunction = "x+y } ";
    fp0.setDelimiterChar('}');
    int res = fp0.Parse(delimiterTestFunction, "x,y");
    if(fp0.ParseError() != FunctionParserErrorType::no_error || res != 4)
    {
        std::cout << "Delimiter test \"" << delimiterTestFunction
                  << "\" failed at " << res << ": " << fp0.ErrorMsg()
                  << std::endl;
        return 1;
    }
    fp0.Parse("x+}y", "x,y");
    if(fp0.ParseError() == FunctionParserErrorType::no_error)
    {
        std::cout << "Erroneous function with delimiter didn't fail"
                  << std::endl;
        return 1;
    }

    bool allTestsOk = true;

    if(!runAllTypes || runAlgoTest == 0)
    {
    #define o(type, enumcode, opt1,opt2, verbosetype) \
        rt_##enumcode(\
            if(runAllTypes || run_##opt1) \
                if(!runRegressionTests<type>()) \
                    allTestsOk = false; \
        , )
        FP_DECLTYPES(o)
    #undef o
    }

////////////////////////////
////////////////////////////
////////////////////////////
////////////////////////////

    // Misc. tests
    // -----------
    const struct
    {
        const char* const testName;
        int(*testFunction)();
    }
    algorithmicTests[] =
    {
        { "Copy constructor and assignment", &testCopying },
        { "Error situations", &testErrorSituations },
        { "Whitespaces", &whiteSpaceTest },
        { "Optimizer test 1 (trig. combinations)", &testOptimizer1 },
        { "Optimizer test 2 (bool combinations, double)",
          (skipSlowAlgo || (!runAllTypes && !run_d)) ? nullptr : &testOptimizer2 },
        { "Optimizer test 3 (bool combinations, long)",
          (!runAllTypes && !run_li) ? nullptr : &testOptimizer3 },
        { "Integral powers",  &testIntPow },
        { "UTF8 test", skipSlowAlgo ? nullptr : &UTF8Test },
        { "Identifier test", &testIdentifiers },
        { "Used-defined functions", &testUserDefinedFunctions },
        { "Multithreading", &testMultithreadedEvaluation }
    };

    const unsigned algorithmicTestsAmount =
        sizeof(algorithmicTests) / sizeof(algorithmicTests[0]);

    if(runAlgoTests)
    {
        for(unsigned i = 0; i < algorithmicTestsAmount; ++i)
        {
            if(runAlgoTest >= 1 && runAlgoTest <= algorithmicTestsAmount &&
               runAlgoTest != i+1)
                continue;

            if(gVerbosityLevel >= 1)
                std::cout << "Algo test " << i+1 << ": "
                          << algorithmicTests[i].testName << std::flush;

            if(!algorithmicTests[i].testFunction)
            {
                if(gVerbosityLevel >= 1)
                    std::cout << ": Skipped." << std::endl;
                continue;
            }

            int result = algorithmicTests[i].testFunction();

            if(result == 0)
            {
                allTestsOk = false;
                if(gVerbosityLevel == 0)
                    std::cout << "Algo test " << i+1 << ": "
                              << algorithmicTests[i].testName;
                if(gVerbosityLevel <= 1)
                    std::cout << ": FAILED." << std::endl;
            }
            else if(gVerbosityLevel >= 1)
            {
                if(result < 0 )
                    std::cout << ": (No support)" << std::endl;
                else
                    std::cout << ": Ok." << std::endl;
            }
        }
    }

    if(!allTestsOk)
    {
        std::cout << "Some tests failed." << std::endl;
        return 1;
    }
    if(gVerbosityLevel == 1)
        std::cout << "================= All tests OK =================\n";
    else if(gVerbosityLevel >= 2)
        std::cout << "================================================\n"
                  << "================= All tests OK =================\n"
                  << "================================================\n";
    return 0;
}
