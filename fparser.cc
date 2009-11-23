/***************************************************************************\
|* Function Parser for C++ v3.3.2                                          *|
|*-------------------------------------------------------------------------*|
|* Copyright: Juha Nieminen, Joel Yliluoma                                 *|
\***************************************************************************/

#include "fpconfig.hh"
#include "fparser.hh"

#include <set>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cmath>
#include <cassert>
using namespace std;

#include "fptypes.hh"
using namespace FUNCTIONPARSERTYPES;

#ifdef FP_USE_THREAD_SAFE_EVAL_WITH_ALLOCA
#ifndef FP_USE_THREAD_SAFE_EVAL
#define FP_USE_THREAD_SAFE_EVAL
#endif
#endif

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif


//=========================================================================
// Name handling functions
//=========================================================================
namespace
{
    template<typename Value_t>
    bool addNewNameData(namePtrsType<Value_t>& namePtrs,
                        std::pair<NamePtr, NameData<Value_t> >& newName,
                        bool isVar)
    {
        typename namePtrsType<Value_t>::iterator nameIter =
            namePtrs.lower_bound(newName.first);

        if(nameIter != namePtrs.end() && newName.first == nameIter->first)
        {
            // redefining a var is not allowed.
            if(isVar)
                return false;

            // redefining other tokens is allowed, if the type stays the same.
            if(nameIter->second.type != newName.second.type)
                return false;

            // update the data
            nameIter->second = newName.second;
            return true;
        }

        if(!isVar)
        {
            // Allocate a copy of the name (pointer stored in the map key)
            // However, for VARIABLEs, the pointer points to VariableString,
            // which is managed separately. Thusly, only done when !IsVar.
            char* namebuf = new char[newName.first.nameLength];
            memcpy(namebuf, newName.first.name, newName.first.nameLength);
            newName.first.name = namebuf;
        }

        namePtrs.insert(nameIter, newName);
        return true;
    }

    unsigned readOpcode(const char* input)
    {
    /*
     Return value if built-in function:
              16 lowest bits = function name length
              15 next bits   = function opcode
              1 bit (&0x80000000U) = indicates function
     Return value if not built-in function:
              31 lowest bits = function name length
              other bits zero
    */
#include "fp_identifier_parser.inc"
        return 0;
    }

    bool containsOnlyValidNameChars(const std::string& name)
    {
        if(name.empty()) return false;
        return readOpcode(name.c_str()) == (unsigned) name.size();
    }

    template<typename Value_t>
    inline bool truthValue(Value_t d)
    {
        return fp_abs(d) >= Value_t(0.5);
    }

    template<typename Value_t>
    inline bool truthValue_abs(Value_t abs_d)
    {
        return abs_d >= Value_t(0.5);
    }

    template<typename Value_t>
    inline Value_t Min(Value_t d1, Value_t d2) { return d1<d2 ? d1 : d2; }

    template<typename Value_t>
    inline Value_t Max(Value_t d1, Value_t d2) { return d1>d2 ? d1 : d2; }

    template<typename Value_t>
    inline Value_t DegreesToRadians(Value_t degrees)
    {
        return degrees*(Value_t(M_PI) / Value_t(180.0));
    }

    template<typename Value_t>
    inline Value_t RadiansToDegrees(Value_t radians)
    {
        return radians*(Value_t(180.0) / Value_t(M_PI));
    }

    template<typename Value_t>
    inline bool isEvenInteger(Value_t value)
    {
        long longval = (long)value;
        return FloatEqual(value, Value_t(longval)) &&
            (longval%2) == 0;
    }

    template<typename Value_t>
    inline bool isOddInteger(Value_t value)
    {
        long longval = (long)value;
        return FloatEqual(value, Value_t(longval)) &&
            (longval%2) != 0;
    }

    template<typename Value_t>
    inline Value_t parseLiteral(const char* nptr, char** endptr)
    {
        return strtod(nptr, endptr);
    }

    template<>
    inline float parseLiteral<float>(const char* nptr, char** endptr)
    {
        return strtof(nptr, endptr);
    }

    template<>
    inline long double parseLiteral<long double>(const char* nptr,
                                                 char** endptr)
    {
        return strtold(nptr, endptr);
    }
}


//=========================================================================
// Data struct implementation
//=========================================================================
template<typename Value_t>
FunctionParserBase<Value_t>::Data::Data():
    referenceCounter(1),
    numVariables(0),
    StackSize(0)
{}

template<typename Value_t>
FunctionParserBase<Value_t>::Data::Data(const Data& rhs):
    referenceCounter(0),
    numVariables(rhs.numVariables),
    variablesString(rhs.variablesString),
    namePtrs(),
    FuncPtrs(rhs.FuncPtrs),
    FuncParsers(rhs.FuncParsers),
    ByteCode(rhs.ByteCode),
    Immed(rhs.Immed),
    Stack(),
    StackSize(rhs.StackSize)
{
    Stack.resize(rhs.Stack.size());

    for(typename namePtrsType<Value_t>::const_iterator i =
            rhs.namePtrs.begin();
        i != rhs.namePtrs.end();
        ++i)
    {
        if(i->second.type == NameData<Value_t>::VARIABLE)
        {
            const size_t variableStringOffset =
                i->first.name - rhs.variablesString.c_str();
            std::pair<NamePtr, NameData<Value_t> > tmp
                (NamePtr(&variablesString[variableStringOffset],
                         i->first.nameLength),
                 i->second);
            namePtrs.insert(namePtrs.end(), tmp);
        }
        else
        {
            std::pair<NamePtr, NameData<Value_t> > tmp
                (NamePtr(new char[i->first.nameLength], i->first.nameLength),
                 i->second );
            memcpy(const_cast<char*>(tmp.first.name), i->first.name,
                   tmp.first.nameLength);
            namePtrs.insert(namePtrs.end(), tmp);
        }
    }
}

template<typename Value_t>
FunctionParserBase<Value_t>::Data::~Data()
{
    for(typename namePtrsType<Value_t>::iterator i =
            namePtrs.begin();
        i != namePtrs.end();
        ++i)
    {
        if(i->second.type != NameData<Value_t>::VARIABLE)
            delete[] i->first.name;
    }
}


//=========================================================================
// FunctionParser constructors, destructor and assignment
//=========================================================================
template<typename Value_t>
FunctionParserBase<Value_t>::FunctionParserBase():
    delimiterChar(0),
    parseErrorType(NO_FUNCTION_PARSED_YET), evalErrorType(0),
    data(new Data),
    useDegreeConversion(false),
    evalRecursionLevel(0),
    StackPtr(0), errorLocation(0)
{
}

template<typename Value_t>
FunctionParserBase<Value_t>::~FunctionParserBase()
{
    if(--(data->referenceCounter) == 0)
        delete data;
}

template<typename Value_t>
FunctionParserBase<Value_t>::FunctionParserBase(const FunctionParserBase& cpy):
    delimiterChar(cpy.delimiterChar),
    parseErrorType(cpy.parseErrorType),
    evalErrorType(cpy.evalErrorType),
    data(cpy.data),
    useDegreeConversion(cpy.useDegreeConversion),
    evalRecursionLevel(0),
    StackPtr(0), errorLocation(0)
{
    ++(data->referenceCounter);
}

template<typename Value_t>
FunctionParserBase<Value_t>&
FunctionParserBase<Value_t>::operator=(const FunctionParserBase& cpy)
{
    if(data != cpy.data)
    {
        if(--(data->referenceCounter) == 0) delete data;

        delimiterChar = cpy.delimiterChar;
        parseErrorType = cpy.parseErrorType;
        evalErrorType = cpy.evalErrorType;
        data = cpy.data;
        useDegreeConversion = cpy.useDegreeConversion;
        evalRecursionLevel = cpy.evalRecursionLevel;

        ++(data->referenceCounter);
    }

    return *this;
}


template<typename Value_t>
void FunctionParserBase<Value_t>::setDelimiterChar(char c)
{
    delimiterChar = c;
}


//---------------------------------------------------------------------------
// Copy-on-write method
//---------------------------------------------------------------------------
template<typename Value_t>
void FunctionParserBase<Value_t>::CopyOnWrite()
{
    if(data->referenceCounter > 1)
    {
        Data* oldData = data;
        data = new Data(*oldData);
        --(oldData->referenceCounter);
        data->referenceCounter = 1;
    }
}

template<typename Value_t>
void FunctionParserBase<Value_t>::ForceDeepCopy()
{
    CopyOnWrite();
}


//=========================================================================
// User-defined constant and function addition
//=========================================================================
template<typename Value_t>
bool FunctionParserBase<Value_t>::AddConstant(const std::string& name,
                                              Value_t value)
{
    if(!containsOnlyValidNameChars(name)) return false;

    CopyOnWrite();
    std::pair<NamePtr, NameData<Value_t> > newName
        (NamePtr(name.data(), unsigned(name.size())),
         NameData<Value_t>(NameData<Value_t>::CONSTANT, value));

    return addNewNameData(data->namePtrs, newName, false);
}

template<typename Value_t>
bool FunctionParserBase<Value_t>::AddUnit(const std::string& name,
                                          Value_t value)
{
    if(!containsOnlyValidNameChars(name)) return false;

    CopyOnWrite();
    std::pair<NamePtr, NameData<Value_t> > newName
        (NamePtr(name.data(), unsigned(name.size())),
         NameData<Value_t>(NameData<Value_t>::UNIT, value));
    return addNewNameData(data->namePtrs, newName, false);
}

template<typename Value_t>
bool FunctionParserBase<Value_t>::AddFunction
(const std::string& name, FunctionPtr ptr, unsigned paramsAmount)
{
    if(!containsOnlyValidNameChars(name)) return false;

    CopyOnWrite();
    std::pair<NamePtr, NameData<Value_t> > newName
        (NamePtr(name.data(), unsigned(name.size())),
         NameData<Value_t>(NameData<Value_t>::FUNC_PTR,
                           unsigned(data->FuncPtrs.size())));

    const bool success = addNewNameData(data->namePtrs, newName, false);
    if(success)
    {
        data->FuncPtrs.push_back(typename Data::FuncPtrData());
        data->FuncPtrs.back().funcPtr = ptr;
        data->FuncPtrs.back().params = paramsAmount;
    }
    return success;
}

template<typename Value_t>
bool FunctionParserBase<Value_t>::CheckRecursiveLinking
(const FunctionParserBase* fp) const
{
    if(fp == this) return true;
    for(unsigned i = 0; i < fp->data->FuncParsers.size(); ++i)
        if(CheckRecursiveLinking(fp->data->FuncParsers[i].parserPtr))
            return true;
    return false;
}

template<typename Value_t>
bool FunctionParserBase<Value_t>::AddFunction(const std::string& name,
                                              FunctionParserBase& fp)
{
    if(!containsOnlyValidNameChars(name) || CheckRecursiveLinking(&fp))
        return false;

    CopyOnWrite();
    std::pair<NamePtr, NameData<Value_t> > newName
        (NamePtr(name.data(), unsigned(name.size())),
         NameData<Value_t>(NameData<Value_t>::PARSER_PTR,
                           unsigned(data->FuncParsers.size())));

    const bool success = addNewNameData(data->namePtrs, newName, false);
    if(success)
    {
        data->FuncParsers.push_back(typename Data::FuncPtrData());
        data->FuncParsers.back().parserPtr = &fp;
        data->FuncParsers.back().params = fp.data->numVariables;
    }
    return success;
}

template<typename Value_t>
bool FunctionParserBase<Value_t>::RemoveIdentifier(const std::string& name)
{
    CopyOnWrite();

    NamePtr namePtr(name.data(), unsigned(name.size()));

    typename namePtrsType<Value_t>::iterator
        nameIter = data->namePtrs.find(namePtr);

    if(nameIter != data->namePtrs.end())
    {
        if(nameIter->second.type == NameData<Value_t>::VARIABLE)
        {
            // Illegal attempt to delete variables
            return false;
        }
        delete[] nameIter->first.name;
        data->namePtrs.erase(nameIter);
        return true;
    }
    return false;
}


//=========================================================================
// Function parsing
//=========================================================================
namespace
{
    // Error messages returned by ErrorMsg():
    const char* const ParseErrorMessage[]=
    {
        "Syntax error",                             // 0
        "Mismatched parenthesis",                   // 1
        "Missing ')'",                              // 2
        "Empty parentheses",                        // 3
        "Syntax error: Operator expected",          // 4
        "Not enough memory",                        // 5
        "An unexpected error occurred. Please make a full bug report "
        "to the author",                            // 6
        "Syntax error in parameter 'Vars' given to "
        "FunctionParser::Parse()",                  // 7
        "Illegal number of parameters to function", // 8
        "Syntax error: Premature end of string",    // 9
        "Syntax error: Expecting ( after function", // 10
        "(No function has been parsed yet)",
        ""
    };

    template<typename Value_t>
    inline typename FunctionParserBase<Value_t>::ParseErrorType
    noCommaError(char c)
    {
        return c == ')' ?
            FunctionParserBase<Value_t>::ILL_PARAMS_AMOUNT :
            FunctionParserBase<Value_t>::SYNTAX_ERROR;
    }

    template<typename Value_t>
    inline typename FunctionParserBase<Value_t>::ParseErrorType
    noParenthError(char c)
    {
        return c == ',' ?
            FunctionParserBase<Value_t>::ILL_PARAMS_AMOUNT :
            FunctionParserBase<Value_t>::MISSING_PARENTH;
    }

    template<typename CharPtr>
    inline void SkipSpace(CharPtr& function)
    {
/*
        Space characters in unicode:
U+0020  SPACE                      Depends on font, often adjusted (see below)
U+00A0  NO-BREAK SPACE             As a space, but often not adjusted
U+2000  EN QUAD                    1 en (= 1/2 em)
U+2001  EM QUAD                    1 em (nominally, the height of the font)
U+2002  EN SPACE                   1 en (= 1/2 em)
U+2003  EM SPACE                   1 em
U+2004  THREE-PER-EM SPACE         1/3 em
U+2005  FOUR-PER-EM SPACE          1/4 em
U+2006  SIX-PER-EM SPACE           1/6 em
U+2007  FIGURE SPACE               Tabular width, the width of digits
U+2008  PUNCTUATION SPACE          The width of a period .
U+2009  THIN SPACE                 1/5 em (or sometimes 1/6 em)
U+200A  HAIR SPACE                 Narrower than THIN SPACE
U+200B  ZERO WIDTH SPACE           Nominally no width, but may expand
U+202F  NARROW NO-BREAK SPACE      Narrower than NO-BREAK SPACE (or SPACE)
U+205F  MEDIUM MATHEMATICAL SPACE  4/18 em
U+3000  IDEOGRAPHIC SPACE          The width of ideographic (CJK) characters.
        Also:
U+000A  \n
U+000D  \r
U+0009  \t
U+000B  \v
        As UTF-8 sequences:
            09
            0A
            0B
            0D
            20
            C2 A0
            E2 80 80-8B
            E2 80 AF
            E2 81 9F
            E3 80 80
*/
        while(true)
        {
            const unsigned char byte = (unsigned char)*function;

#if(' ' == 32) /* ASCII */
            if(sizeof(unsigned long) == 8)
            {
                const unsigned n = sizeof(unsigned long)*8-1;
                // ^ avoids compiler warning when not 64-bit
                if(byte <= ' ')
                {
                    unsigned long shifted = 1UL << byte;
                    const unsigned long mask =
                        (1UL << ('\r'&n)) |
                        (1UL << ('\n'&n)) |
                        (1UL << ('\v'&n)) |
                        (1UL << ('\t'&n)) |
                        (1UL << (' ' &n));
                    if(mask & shifted) { ++function; continue; }
                    return;
                }
            }
            else
            {
                unsigned char cbyte = (unsigned char)(0x20 - *function);
                if(cbyte <= 0x17)
                {
                    unsigned shifted = 1U << cbyte;
                    const unsigned mask =
                        (1U << (0x20 - '\r')) |
                        (1U << (0x20 - '\n')) |
                        (1U << (0x20 - '\v')) |
                        (1U << (0x20 - '\t')) |
                        (1U << (0x20 - ' '));
                    if(mask & shifted) { ++function; continue; }
                    return;
                }
            }
#endif // #if(' ' == 32)

#if(' ' == 32)
            if(byte < 0xC2) return;
#endif

            switch(byte)
            {
#if(' ' != 32)
              case ' ':
              case '\r':
              case '\n':
              case '\v':
              case '\t':
                  ++function;
                  break;
#endif
              case 0xC2:
              {
                  unsigned char byte2 = function[1];
                  if(byte2 == 0xA0) { function += 2; continue; }
                  break;
              }
              case 0xE2:
              {
                  unsigned char byte2 = function[1];
                  if(byte2 == 0x81 && (unsigned char)(function[2]) == 0x9F)
                  {
                      function += 3;
                      continue;
                  }
                  if(byte2 == 0x80 &&
                     ((unsigned char)(function[2]) == 0xAF ||
                      ((unsigned char)(function[2]) >= 0x80 &&
                       (unsigned char)(function[2]) <= 0x8B)))
                  {
                      function += 3;
                      continue;
                  }
                  break;
              }
              case 0xE3:
              {
                  unsigned char byte2 = function[1];
                  if(byte2 == 0x80 && (unsigned char)(function[2]) == 0x80)
                  {
                      function += 3;
                      continue;
                  }
                  break;
              }
            }
            break;
        } // while(true)
    } // SkipSpace(CharPtr& function)
}

// Return parse error message
// --------------------------
template<typename Value_t>
const char* FunctionParserBase<Value_t>::ErrorMsg() const
{
    return ParseErrorMessage[parseErrorType];
}


// Parse variables
// ---------------
template<typename Value_t>
bool FunctionParserBase<Value_t>::ParseVariables
(const std::string& inputVarString)
{
    if(data->variablesString == inputVarString) return true;

    /* Delete existing variables from namePtrs */
    for(typename namePtrsType<Value_t>::iterator i =
            data->namePtrs.begin();
        i != data->namePtrs.end(); )
    {
        if(i->second.type == NameData<Value_t>::VARIABLE)
        {
            typename namePtrsType<Value_t>::iterator j (i);
            ++i;
            data->namePtrs.erase(j);
        }
        else ++i;
    }
    data->variablesString = inputVarString;

    const std::string& vars = data->variablesString;
    const unsigned len = unsigned(vars.size());

    unsigned varNumber = VarBegin;

    const char* beginPtr = vars.c_str();
    const char* finalPtr = beginPtr + len;
    while(beginPtr < finalPtr)
    {
        SkipSpace(beginPtr);
        unsigned nameLength = readOpcode(beginPtr);
        if(nameLength == 0 || (nameLength & 0x80000000U)) return false;
        const char* endPtr = beginPtr + nameLength;
        SkipSpace(endPtr);
        if(endPtr != finalPtr && *endPtr != ',') return false;

        std::pair<NamePtr, NameData<Value_t> > newName
            (NamePtr(beginPtr, nameLength),
             NameData<Value_t>(NameData<Value_t>::VARIABLE, varNumber++));

        if(!addNewNameData(data->namePtrs, newName, true))
        {
            return false;
        }

        beginPtr = endPtr + 1;
    }

    data->numVariables = varNumber - VarBegin;
    return true;
}

// Parse interface functions
// -------------------------
template<typename Value_t>
int FunctionParserBase<Value_t>::Parse(const char* Function,
                                       const std::string& Vars,
                                       bool useDegrees)
{
    CopyOnWrite();

    if(!ParseVariables(Vars))
    {
        parseErrorType = INVALID_VARS;
        return int(strlen(Function));
    }

    return ParseFunction(Function, useDegrees);
}

template<typename Value_t>
int FunctionParserBase<Value_t>::Parse(const std::string& Function,
                                       const std::string& Vars,
                                       bool useDegrees)
{
    CopyOnWrite();

    if(!ParseVariables(Vars))
    {
        parseErrorType = INVALID_VARS;
        return int(Function.size());
    }

    return ParseFunction(Function.c_str(), useDegrees);
}


// Main parsing function
// ---------------------
template<typename Value_t>
int FunctionParserBase<Value_t>::ParseFunction(const char* function,
                                               bool useDegrees)
{
    useDegreeConversion = useDegrees;
    parseErrorType = FP_NO_ERROR;

    data->ByteCode.clear(); data->ByteCode.reserve(128);
    data->Immed.clear(); data->Immed.reserve(128);
    data->StackSize = StackPtr = 0;

    const char* ptr = CompileExpression(function);
    if(parseErrorType != FP_NO_ERROR) return int(errorLocation - function);

    assert(ptr); // Should never be null at this point. It's a bug otherwise.
    if(*ptr)
    {
        if(delimiterChar == 0 || *ptr != delimiterChar)
            parseErrorType = EXPECT_OPERATOR;
        return int(ptr - function);
    }

#ifndef FP_USE_THREAD_SAFE_EVAL
    data->Stack.resize(data->StackSize);
#endif

    return -1;
}


//=========================================================================
// Parsing and bytecode compiling functions
//=========================================================================
template<typename Value_t>
inline const char* FunctionParserBase<Value_t>::SetErrorType(ParseErrorType t,
                                                             const char* pos)
{
    parseErrorType = t;
    errorLocation = pos;
    return 0;
}

template<typename Value_t>
inline void FunctionParserBase<Value_t>::incStackPtr()
{
    if(++StackPtr > data->StackSize) ++(data->StackSize);
}

namespace
{
    const unsigned char powi_factor_table[128] =
    {
        0,1,0,0,0,0,0,0, 0, 0,0,0,0,0,0,3,/*   0 -  15 */
        0,0,0,0,0,0,0,0, 0, 5,0,3,0,0,3,0,/*  16 -  31 */
        0,0,0,0,0,0,0,3, 0, 0,0,0,0,5,0,0,/*  32 -  47 */
        0,0,5,3,0,0,3,5, 0, 3,0,0,3,0,0,3,/*  48 -  63 */
        0,0,0,0,0,0,0,0, 0, 0,0,3,0,0,3,0,/*  64 -  79 */
        0,9,0,0,0,5,0,3, 0, 0,5,7,0,0,0,5,/*  80 -  95 */
        0,0,0,3,5,0,3,0, 0, 3,0,0,3,0,5,3,/*  96 - 111 */
        0,0,3,5,0,9,0,7, 3,11,0,3,0,5,3,0,/* 112 - 127 */
    };

    inline int get_powi_factor(int abs_int_exponent)
    {
        if(abs_int_exponent >= int(sizeof(powi_factor_table))) return 0;
        return powi_factor_table[abs_int_exponent];
    }

#if 0
    int EstimatePowiComplexity(int abs_int_exponent)
    {
        int cost = 0;
        while(abs_int_exponent > 1)
        {
            int factor = get_powi_factor(abs_int_exponent);
            if(factor)
            {
                cost += EstimatePowiComplexity(factor);
                abs_int_exponent /= factor;
                continue;
            }
            if(!(abs_int_exponent & 1))
            {
                abs_int_exponent /= 2;
                cost += 3; // sqr
            }
            else
            {
                cost += 4; // dup+mul
                abs_int_exponent -= 1;
            }
        }
        return cost;
    }
#endif

    bool IsEligibleIntPowiExponent(int int_exponent)
    {
        if(int_exponent == 0) return false;
        int abs_int_exponent = int_exponent;
    #if 0
        int cost = 0;

        if(abs_int_exponent < 0)
        {
            cost += 11;
            abs_int_exponent = -abs_int_exponent;
        }

        cost += EstimatePowiComplexity(abs_int_exponent);

        return cost < (10*3 + 4*4);
    #else
        if(abs_int_exponent < 0) abs_int_exponent = -abs_int_exponent;

        return (abs_int_exponent >= 1)
            && (abs_int_exponent <= 46 ||
              (abs_int_exponent <= 1024 &&
              (abs_int_exponent & (abs_int_exponent - 1)) == 0));
    #endif
    }

    bool IsLogicalOpcode(unsigned op)
    {
        switch(op)
        {
          case cAnd: case cAbsAnd:
          case cOr:  case cAbsOr:
          case cNot: case cAbsNot:
          case cNotNot: case cAbsNotNot:
          case cEqual: case cNEqual:
          case cLess: case cLessOrEq:
          case cGreater: case cGreaterOrEq:
              return true;
          default: break;
        }
        return false;
    }

    bool IsNeverNegativeValueOpcode(unsigned op)
    {
        switch(op)
        {
          case cAnd: case cAbsAnd:
          case cOr:  case cAbsOr:
          case cNot: case cAbsNot:
          case cNotNot: case cAbsNotNot:
          case cEqual: case cNEqual:
          case cLess: case cLessOrEq:
          case cGreater: case cGreaterOrEq:
          case cSqrt: case cRSqrt: case cSqr:
          case cAbs:
          case cAcos: case cCosh:
              return true;
          default: break;
        }
        return false;
    }

    bool IsAlwaysIntegerOpcode(unsigned op)
    {
        switch(op)
        {
          case cAnd: case cAbsAnd:
          case cOr:  case cAbsOr:
          case cNot: case cAbsNot:
          case cNotNot: case cAbsNotNot:
          case cEqual: case cNEqual:
          case cLess: case cLessOrEq:
          case cGreater: case cGreaterOrEq:
          case cInt: case cFloor: case cCeil: case cTrunc:
              return true;
          default: break;
        }
        return false;
    }

#ifdef FP_EPSILON
    const double EpsilonOrZero = FP_EPSILON;
#else
    const double EpsilonOrZero = 0.0;
#endif

}

template<typename Value_t>
inline void FunctionParserBase<Value_t>::AddImmedOpcode(Value_t value)
{
    data->Immed.push_back(value);
    data->ByteCode.push_back(cImmed);
}

template<typename Value_t>
inline void FunctionParserBase<Value_t>::CompilePowi(int abs_int_exponent)
{
    int num_muls=0;
    while(abs_int_exponent > 1)
    {
        int factor = get_powi_factor(abs_int_exponent);
        if(factor)
        {
            CompilePowi(factor);
            abs_int_exponent /= factor;
            continue;
        }
        if(!(abs_int_exponent & 1))
        {
            abs_int_exponent /= 2;
            data->ByteCode.push_back(cSqr);
        }
        else
        {
            data->ByteCode.push_back(cDup);
            incStackPtr();
            abs_int_exponent -= 1;
            ++num_muls;
        }
    }
    if(num_muls > 0)
    {
        data->ByteCode.resize(data->ByteCode.size()+num_muls, cMul);
        StackPtr -= num_muls;
    }
}

template<typename Value_t>
inline bool FunctionParserBase<Value_t>::TryCompilePowi(Value_t original_immed)
{
    Value_t changed_immed = original_immed;
    for(int sqrt_count=0; /**/; ++sqrt_count)
    {
        int int_exponent = (int)changed_immed;
        if(changed_immed == Value_t(int_exponent) &&
           IsEligibleIntPowiExponent(int_exponent))
        {
            int abs_int_exponent = int_exponent;
            if(abs_int_exponent < 0)
                abs_int_exponent = -abs_int_exponent;

            data->Immed.pop_back(); data->ByteCode.pop_back();
            while(sqrt_count > 0)
            {
                int opcode = cSqrt;
                if(sqrt_count == 1 && int_exponent < 0)
                {
                    opcode = cRSqrt;
                    int_exponent = -int_exponent;
                }
                data->ByteCode.push_back(opcode);
                --sqrt_count;
            }
            CompilePowi(abs_int_exponent);
            if(int_exponent < 0) data->ByteCode.push_back(cInv);
            return true;
        }
        if(sqrt_count >= 4) break;
        changed_immed += changed_immed;
    }

    // When we don't know whether x >= 0, we still know that
    // x^y can be safely converted into exp(y * log(x))
    // when y is _not_ integer, because we know that x >= 0.
    // Otherwise either expression will give a NaN.
    if(/*!IsIntegerConst(original_immed) ||*/
       IsNeverNegativeValueOpcode(data->ByteCode[data->ByteCode.size()-2]))
    {
        data->Immed.pop_back();
        data->ByteCode.pop_back();
        AddFunctionOpcode(cLog);
        AddImmedOpcode(original_immed);
        AddFunctionOpcode(cMul);
        AddFunctionOpcode(cExp);
        return true;
    }
    return false;
}

#include "fp_opcode_add.inc"

template<typename Value_t>
const char* FunctionParserBase<Value_t>::CompileIf(const char* function)
{
    if(*function != '(') return SetErrorType(EXPECT_PARENTH_FUNC, function);

    function = CompileExpression(function+1);
    if(!function) return 0;
    if(*function != ',')
        return SetErrorType(noCommaError<Value_t>(*function), function);

    OPCODE opcode = cIf;
    if(data->ByteCode.back() == cNotNot) data->ByteCode.pop_back();
    if(IsNeverNegativeValueOpcode(data->ByteCode.back()))
    {
        // If we know that the condition to be tested is always
        // a positive value (such as when produced by "x<y"),
        // we can use the faster opcode to evaluate it.
        // cIf tests whether fabs(cond) >= 0.5,
        // cAbsIf simply tests whether cond >= 0.5.
        opcode = cAbsIf;
    }

    data->ByteCode.push_back(opcode);
    const unsigned curByteCodeSize = unsigned(data->ByteCode.size());
    data->ByteCode.push_back(0); // Jump index; to be set later
    data->ByteCode.push_back(0); // Immed jump index; to be set later

    --StackPtr;

    function = CompileExpression(function + 1);
    if(!function) return 0;
    if(*function != ',')
        return SetErrorType(noCommaError<Value_t>(*function), function);

    data->ByteCode.push_back(cJump);
    const unsigned curByteCodeSize2 = unsigned(data->ByteCode.size());
    const unsigned curImmedSize2 = unsigned(data->Immed.size());
    data->ByteCode.push_back(0); // Jump index; to be set later
    data->ByteCode.push_back(0); // Immed jump index; to be set later

    --StackPtr;

    function = CompileExpression(function + 1);
    if(!function) return 0;
    if(*function != ')')
        return SetErrorType(noParenthError<Value_t>(*function), function);

    /* A cNop is added as an easy fix for the problem which happens if cNeg
       or other similar opcodes optimized by Parse() immediately follow an
       else-branch which could be confused as optimizable with that opcode
       (eg. cImmed). The optimizer removes the cNop safely.
     */
    data->ByteCode.push_back(cNop);

    // Set jump indices
    data->ByteCode[curByteCodeSize] = curByteCodeSize2+1;
    data->ByteCode[curByteCodeSize+1] = curImmedSize2;
    data->ByteCode[curByteCodeSize2] = unsigned(data->ByteCode.size())-1;
    data->ByteCode[curByteCodeSize2+1] = unsigned(data->Immed.size());

    ++function;
    SkipSpace(function);
    return function;
}

template<typename Value_t>
const char* FunctionParserBase<Value_t>::CompileFunctionParams
(const char* function, unsigned requiredParams)
{
    if(*function != '(') return SetErrorType(EXPECT_PARENTH_FUNC, function);

    if(requiredParams > 0)
    {
        function = CompileExpression(function+1);
        if(!function) return 0;

        for(unsigned i = 1; i < requiredParams; ++i)
        {
            if(*function != ',')
                return SetErrorType(noCommaError<Value_t>(*function), function);

            function = CompileExpression(function+1);
            if(!function) return 0;
        }
        // No need for incStackPtr() because each parse parameter calls it
        StackPtr -= requiredParams-1;
    }
    else
    {
        incStackPtr(); // return value of function is pushed onto the stack
        ++function;
        SkipSpace(function);
    }

    if(*function != ')')
        return SetErrorType(noParenthError<Value_t>(*function), function);
    ++function;
    SkipSpace(function);
    return function;
}

template<typename Value_t>
const char* FunctionParserBase<Value_t>::CompileElement(const char* function)
{
    switch( (unsigned char) *function)
    {
      case '(': // Expression in parentheses
      {
          ++function;
          SkipSpace(function);
          if(*function == ')') return SetErrorType(EMPTY_PARENTH, function);

          function = CompileExpression(function);
          if(!function) return 0;

          if(*function != ')') return SetErrorType(MISSING_PARENTH, function);

          ++function;
          SkipSpace(function);
          return function;
      }

      case '.': case '0': case '1': case '2':
      case '3': case '4': case '5': case '6':
      case '7': case '8': case '9': // Number
      {
          char* endPtr;
          const Value_t val = parseLiteral<Value_t>(function, &endPtr);
          if(endPtr == function) return SetErrorType(SYNTAX_ERROR, function);

          AddImmedOpcode(val);
          incStackPtr();

          SkipSpace(endPtr);
          return endPtr;
      }

      case ')': return SetErrorType(MISM_PARENTH, function);

      /* The switch-case here covers the ascii range
       * from 40 to 57 almost completely. A few characters
       * however are missing, but they are not part of
       * valid identifiers. Include them here to reduce
       * the number of jumps in the compiled program.
       */
      case '*': case '+': case ',': case '-': case '/':
          return SetErrorType(SYNTAX_ERROR, function);
    }

    unsigned nameLength = readOpcode(function);

    if(nameLength != 0) // Function, variable or constant
    {
        if(nameLength & 0x80000000U) // Function
        {
            OPCODE func_opcode = OPCODE( (nameLength >> 16) & 0x7FFF );
            const char* endPtr = function + (nameLength & 0xFFFF);
            SkipSpace(endPtr);

            const FuncDefinition& funcDef = Functions[func_opcode];

            if(func_opcode == cIf) // "if" is a special case
                return CompileIf(endPtr);

            unsigned requiredParams = funcDef.params;
    #ifndef FP_DISABLE_EVAL
            if(func_opcode == cEval)
                requiredParams = data->numVariables;
    #endif

            function = CompileFunctionParams(endPtr, requiredParams);
            if(!function) return 0;

            if(useDegreeConversion)
            {
                if(funcDef.flags & FuncDefinition::AngleIn)
                    AddFunctionOpcode(cRad);

                AddFunctionOpcode(func_opcode);

                if(funcDef.flags & FuncDefinition::AngleOut)
                    AddFunctionOpcode(cDeg);
            }
            else
            {
                AddFunctionOpcode(func_opcode);
            }
            return function;
        }

        NamePtr name(function, nameLength);
        const char* endPtr = function + nameLength;
        SkipSpace(endPtr);

        typename namePtrsType<Value_t>::iterator nameIter =
            data->namePtrs.find(name);
        if(nameIter != data->namePtrs.end())
        {
            const NameData<Value_t>* nameData = &nameIter->second;
            switch(nameData->type)
            {
              case NameData<Value_t>::VARIABLE: // is variable
                  data->ByteCode.push_back(nameData->index);
                  incStackPtr();
                  return endPtr;

              case NameData<Value_t>::CONSTANT:
                  AddImmedOpcode(nameData->value);
                  incStackPtr();
                  return endPtr;

              case NameData<Value_t>::UNIT: break;

      /* The reason why a cNop is added after a cFCall and a cPCall opcode is
         that the function index could otherwise be confused with an actual
         opcode (most prominently cImmed), making parse-time optimizations bug
         (eg. if cNeg immediately follows an index value equal to cImmed, in
         which case the parser would "optimize" it to negating the (inexistent)
         literal, causing mayhem). The optimizer gets rid of the cNop safely.
         (Another option would be to add some offset to the function index
         when storing it in the bytecode, and then subtract that offset when
         interpreting the bytecode, but this causes more programming overhead
         than the speed overhead caused by the cNop to be worth the trouble,
         especially since the function call caused by the opcode is quite slow
         anyways.)
       */
              case NameData<Value_t>::FUNC_PTR:
                  function = CompileFunctionParams
                      (endPtr, data->FuncPtrs[nameData->index].params);
                  data->ByteCode.push_back(cFCall);
                  data->ByteCode.push_back(nameData->index);
                  data->ByteCode.push_back(cNop);
                  return function;

              case NameData<Value_t>::PARSER_PTR:
                  function = CompileFunctionParams
                      (endPtr, data->FuncParsers[nameData->index].params);
                  data->ByteCode.push_back(cPCall);
                  data->ByteCode.push_back(nameData->index);
                  data->ByteCode.push_back(cNop);
                  return function;
            }
        }
    }

    return SetErrorType(SYNTAX_ERROR, function);
}

template<typename Value_t>
const char*
FunctionParserBase<Value_t>::CompilePossibleUnit(const char* function)
{
    unsigned nameLength = readOpcode(function);
    if(nameLength & 0x80000000U) return function; // built-in function name
    if(nameLength != 0)
    {
        NamePtr name(function, nameLength);

        typename namePtrsType<Value_t>::iterator nameIter =
            data->namePtrs.find(name);
        if(nameIter != data->namePtrs.end())
        {
            const NameData<Value_t>* nameData = &nameIter->second;
            if(nameData->type == NameData<Value_t>::UNIT)
            {
                AddImmedOpcode(nameData->value);
                incStackPtr();
                AddFunctionOpcode(cMul);
                --StackPtr;

                const char* endPtr = function + nameLength;
                SkipSpace(endPtr);
                return endPtr;
            }
        }
    }

    return function;
}

template<typename Value_t>
inline const char*
FunctionParserBase<Value_t>::CompilePow(const char* function)
{
    function = CompileElement(function);
    if(!function) return 0;
    function = CompilePossibleUnit(function);

    if(*function == '^')
    {
        ++function;
        SkipSpace(function);

        unsigned op = cPow;
        if(data->ByteCode.back() == cImmed)
        {
            if(data->Immed.back() == Value_t(2.7182818284590452353602874713526624977572L))
                { op = cExp;  data->ByteCode.pop_back(); data->Immed.pop_back(); --StackPtr; }
            else if(data->Immed.back() == Value_t(2.0))
                { op = cExp2; data->ByteCode.pop_back(); data->Immed.pop_back(); --StackPtr; }
        }

        function = CompileUnaryMinus(function);
        if(!function) return 0;

        // add opcode
        AddFunctionOpcode(op);

        --StackPtr;
    }
    return function;
}

template<typename Value_t>
inline const char*
FunctionParserBase<Value_t>::CompileUnaryMinus(const char* function)
{
    char op = *function;
    switch(op)
    {
        case '-':
        case '!':
            ++function;
            SkipSpace(function);

            function = CompileUnaryMinus(function);
            if(!function) return 0;

            AddFunctionOpcode(op=='-' ? cNeg : cNot);

            return function;
        default: break;
    }
    return CompilePow(function);
}

template<typename Value_t>
inline const char*
FunctionParserBase<Value_t>::CompileMult(const char* function)
{
    unsigned op = 0;
    while(true)
    {
        function = CompileUnaryMinus(function);
        if(!function) return 0;

        // add opcode
        if(op)
        {
            AddFunctionOpcode(op);
            --StackPtr;
        }
        switch(*function)
        {
            case '*': op = cMul; break;
            case '/': op = cDiv; break;
            case '%': op = cMod; break;
            default: return function;
        }

        ++function;
        SkipSpace(function);

        if(op != cMod &&
           data->ByteCode.back() == cImmed &&
           data->Immed.back() == 1.0)
        {
            op = (op == cDiv ? cInv : 0);
            data->Immed.pop_back();
            data->ByteCode.pop_back();
        }
    }
    return function;
}

template<typename Value_t>
inline const char*
FunctionParserBase<Value_t>::CompileAddition(const char* function)
{
    unsigned op=0;
    while(true)
    {
        function = CompileMult(function);
        if(!function) return 0;

        // add opcode
        if(op)
        {
            AddFunctionOpcode(op);
            --StackPtr;
        }
        switch(*function)
        {
            case '+': op = cAdd; break;
            case '-': op = cSub; break;
            default: return function;
        }

        ++function;
        SkipSpace(function);

        if(data->ByteCode.back() == cImmed &&
           data->Immed.back() == Value_t(0))
        {
            op = (op == cSub ? cNeg : 0);
            data->Immed.pop_back();
            data->ByteCode.pop_back();
        }
    }
    return function;
}

template<typename Value_t>
inline const char*
FunctionParserBase<Value_t>::CompileComparison(const char* function)
{
    unsigned op=0;
    while(true)
    {
        function = CompileAddition(function);
        if(!function) return 0;

        if(op)
        {
            AddFunctionOpcode(op);
            --StackPtr;
        }
        switch(*function)
        {
          case '=':
              ++function; op = cEqual; break;
          case '!':
              if(function[1] == '=') { function += 2; op = cNEqual; break; }
              // If '=' does not follow '!', a syntax error will
              // be generated at the outermost parsing level
              return function;
          case '<':
              if(function[1] == '=') { function += 2; op = cLessOrEq; break; }
              ++function; op = cLess; break;
          case '>':
              if(function[1] == '=') { function += 2; op = cGreaterOrEq; break; }
              ++function; op = cGreater; break;
          default: return function;
        }
        SkipSpace(function);
    }
    return function;
}

template<typename Value_t>
inline const char*
FunctionParserBase<Value_t>::CompileAnd(const char* function)
{
    size_t param0end=0;
    while(true)
    {
        function = CompileComparison(function);
        if(!function) return 0;

        if(param0end)
        {
            if(data->ByteCode.back() == cNotNot) data->ByteCode.pop_back();

            unsigned& param0last = data->ByteCode[param0end-1];
            unsigned& param1last = data->ByteCode.back();
            if(IsNeverNegativeValueOpcode(param1last) &&
               IsNeverNegativeValueOpcode(param0last))
            {
                /* Change !x & !y into !(x | y). Because y might
                 * contain an cIf, we replace the first cNot/cAbsNot
                 * with cNop to avoid jump indices being broken.
                 */
                if((param0last == cNot || param0last == cAbsNot)
                && (param1last == cNot || param1last == cAbsNot))
                {
                    param1last = (param0last==cAbsNot && param1last==cAbsNot)
                                    ? cAbsOr : cOr;
                    param0last = cNop;
                    AddFunctionOpcode(cAbsNot);
                }
                else
                    AddFunctionOpcode(cAbsAnd);
            }
            else
                AddFunctionOpcode(cAnd);
            --StackPtr;
        }
        if(*function != '&') break;
        ++function;
        SkipSpace(function);
        param0end = data->ByteCode.size();
    }
    return function;
}

template<typename Value_t>
const char*
FunctionParserBase<Value_t>::CompileExpression(const char* function)
{
    size_t param0end=0;
    while(true)
    {
        SkipSpace(function);
        function = CompileAnd(function);
        if(!function) return 0;

        if(param0end)
        {
            if(data->ByteCode.back() == cNotNot) data->ByteCode.pop_back();

            unsigned& param0last = data->ByteCode[param0end-1];
            unsigned& param1last = data->ByteCode.back();
            if(IsNeverNegativeValueOpcode(param1last)
            && IsNeverNegativeValueOpcode(param0last))
            {
                /* Change !x | !y into !(x & y). Because y might
                 * contain an cIf, we replace the first cNot/cAbsNot
                 * with cNop to avoid jump indices being broken.
                 */
                if((param0last == cNot || param0last == cAbsNot)
                && (param1last == cNot || param1last == cAbsNot))
                {
                    param1last = (param0last==cAbsNot && param1last==cAbsNot)
                                    ? cAbsAnd : cAnd;
                    param0last = cNop;
                    AddFunctionOpcode(cAbsNot);
                }
                else
                    AddFunctionOpcode(cAbsOr);
            }
            else
                AddFunctionOpcode(cOr);
            --StackPtr;
        }
        if(*function != '|') break;
        ++function;
        param0end = data->ByteCode.size();
    }
    return function;
}

//===========================================================================
// Function evaluation
//===========================================================================
template<typename Value_t>
Value_t FunctionParserBase<Value_t>::Eval(const Value_t* Vars)
{
    if(parseErrorType != FP_NO_ERROR) return Value_t(0);

    const unsigned* const ByteCode = &(data->ByteCode[0]);
    const Value_t* const Immed = data->Immed.empty() ? 0 : &(data->Immed[0]);
    const unsigned ByteCodeSize = unsigned(data->ByteCode.size());
    unsigned IP, DP=0;
    int SP=-1;

#ifdef FP_USE_THREAD_SAFE_EVAL
    /* If Eval() may be called by multiple threads simultaneously,
     * then Eval() must allocate its own stack.
     */
#ifdef FP_USE_THREAD_SAFE_EVAL_WITH_ALLOCA
    /* alloca() allocates room from the hardware stack.
     * It is automatically freed when the function returns.
     */
    Value_t* const Stack = (Value_t*)alloca(data->StackSize*sizeof(Value_t));
#else
    /* Allocate from the heap. Ensure that it is freed
     * automatically no matter which exit path is taken.
     */
    struct AutoDealloc
    {
        Value_t* ptr;
        ~AutoDealloc() { delete[] ptr; }
    } AutoDeallocStack = { new Value_t[data->StackSize] };
    Value_t*& Stack = AutoDeallocStack.ptr;
#endif
#else
    /* No thread safety, so use a global stack. */
    std::vector<Value_t>& Stack = data->Stack;
#endif

    for(IP=0; IP<ByteCodeSize; ++IP)
    {
        switch(ByteCode[IP])
        {
// Functions:
          case   cAbs: Stack[SP] = fp_abs(Stack[SP]); break;

          case  cAcos:
#           ifndef FP_NO_EVALUATION_CHECKS
              if(Stack[SP] < Value_t(-1) || Stack[SP] > Value_t(1))
              { evalErrorType=4; return Value_t(0); }
#           endif
              Stack[SP] = fp_acos(Stack[SP]); break;

          case cAcosh: Stack[SP] = fp_acosh(Stack[SP]); break;

          case  cAsin:
#           ifndef FP_NO_EVALUATION_CHECKS
              if(Stack[SP] < Value_t(-1) || Stack[SP] > Value_t(1))
              { evalErrorType=4; return Value_t(0); }
#           endif
              Stack[SP] = fp_asin(Stack[SP]); break;

          case cAsinh: Stack[SP] = fp_asinh(Stack[SP]); break;

          case  cAtan: Stack[SP] = fp_atan(Stack[SP]); break;

          case cAtan2: Stack[SP-1] = fp_atan2(Stack[SP-1], Stack[SP]);
                       --SP; break;

          case cAtanh: Stack[SP] = fp_atanh(Stack[SP]); break;

          case  cCbrt: Stack[SP] = fp_cbrt(Stack[SP]); break;

          case  cCeil: Stack[SP] = fp_ceil(Stack[SP]); break;

          case   cCos: Stack[SP] = fp_cos(Stack[SP]); break;

          case  cCosh: Stack[SP] = fp_cosh(Stack[SP]); break;

          case   cCot:
              {
                  const Value_t t = fp_tan(Stack[SP]);
#               ifndef FP_NO_EVALUATION_CHECKS
                  if(t == Value_t(0)) { evalErrorType=1; return Value_t(0); }
#               endif
                  Stack[SP] = Value_t(1)/t; break;
              }

          case   cCsc:
              {
                  const Value_t s = fp_sin(Stack[SP]);
#               ifndef FP_NO_EVALUATION_CHECKS
                  if(s == 0) { evalErrorType=1; return Value_t(0); }
#               endif
                  Stack[SP] = Value_t(1)/s; break;
              }


#       ifndef FP_DISABLE_EVAL
          case  cEval:
              {
                  const unsigned varAmount = data->numVariables;
                  Value_t retVal = Value_t(0);
                  if(evalRecursionLevel == FP_EVAL_MAX_REC_LEVEL)
                  {
                      evalErrorType = 5;
                  }
                  else
                  {
                      ++evalRecursionLevel;
#                   ifndef FP_USE_THREAD_SAFE_EVAL
                      /* Eval() will use data->Stack for its storage.
                       * Swap the current stack with an empty one.
                       * This is the not-thread-safe method.
                       */
                      std::vector<Value_t> tmpStack(Stack.size());
                      data->Stack.swap(tmpStack);
                      retVal = Eval(&tmpStack[SP - varAmount + 1]);
                      data->Stack.swap(tmpStack);
#                   else
                      /* Thread safety mode. We don't need to
                       * worry about stack reusing here, because
                       * each instance of Eval() will allocate
                       * their own stack.
                       */
                      retVal = Eval(&Stack[SP - varAmount + 1]);
#                   endif
                      --evalRecursionLevel;
                  }
                  SP -= varAmount-1;
                  Stack[SP] = retVal;
                  break;
              }
#       endif

          case   cExp: Stack[SP] = fp_exp(Stack[SP]); break;

          case   cExp2: Stack[SP] = fp_exp2(Stack[SP]); break;

          case cFloor: Stack[SP] = fp_floor(Stack[SP]); break;

          case    cIf:
                  if(truthValue(Stack[SP--]))
                      IP += 2;
                  else
                  {
                      const unsigned* buf = &ByteCode[IP+1];
                      IP = buf[0];
                      DP = buf[1];
                  }
                  break;

          case   cInt: Stack[SP] = fp_int(Stack[SP]); break;

          case   cLog:
#           ifndef FP_NO_EVALUATION_CHECKS
              if(!(Stack[SP] > Value_t(0)))
              { evalErrorType=3; return Value_t(0); }
#           endif
              Stack[SP] = fp_log(Stack[SP]); break;

          case cLog10:
#           ifndef FP_NO_EVALUATION_CHECKS
              if(!(Stack[SP] > Value_t(0)))
              { evalErrorType=3; return Value_t(0); }
#           endif
              Stack[SP] = fp_log10(Stack[SP]);
              break;

          case  cLog2:
#           ifndef FP_NO_EVALUATION_CHECKS
              if(!(Stack[SP] > Value_t(0)))
              { evalErrorType=3; return Value_t(0); }
#           endif
              Stack[SP] = fp_log2(Stack[SP]);
              break;

          case   cMax: Stack[SP-1] = Max(Stack[SP-1], Stack[SP]);
                       --SP; break;

          case   cMin: Stack[SP-1] = Min(Stack[SP-1], Stack[SP]);
                       --SP; break;

          case   cPow:
#           ifndef FP_NO_EVALUATION_CHECKS
              // x:Negative ^ y:NonInteger is failure,
              // except when the reciprocal of y forms an integer
              /*if(Stack[SP-1] < Value_t(0) &&
                 !IsIntegerConst(Stack[SP]) &&
                 !IsIntegerConst(1.0 / Stack[SP]))
              { evalErrorType=3; return Value_t(0); }*/
              // x:0 ^ y:negative is failure
              if(Stack[SP-1] == Value_t(0) &&
                 Stack[SP] < Value_t(0))
              { evalErrorType=3; return Value_t(0); }
#           endif
              Stack[SP-1] = fp_pow(Stack[SP-1], Stack[SP]);
              --SP; break;

          case  cTrunc: Stack[SP] = fp_trunc(Stack[SP]); break;

          case   cSec:
              {
                  const Value_t c = fp_cos(Stack[SP]);
#               ifndef FP_NO_EVALUATION_CHECKS
                  if(c == Value_t(0)) { evalErrorType=1; return Value_t(0); }
#               endif
                  Stack[SP] = Value_t(1)/c; break;
              }

          case   cSin: Stack[SP] = fp_sin(Stack[SP]); break;

          case  cSinh: Stack[SP] = fp_sinh(Stack[SP]); break;

          case  cSqrt:
#           ifndef FP_NO_EVALUATION_CHECKS
              if(Stack[SP] < Value_t(0)) { evalErrorType=2; return Value_t(0); }
#           endif
              Stack[SP] = fp_sqrt(Stack[SP]); break;

          case   cTan: Stack[SP] = fp_tan(Stack[SP]); break;

          case  cTanh: Stack[SP] = fp_tanh(Stack[SP]); break;


// Misc:
          case cImmed: Stack[++SP] = Immed[DP++]; break;

          case  cJump:
              {
                  const unsigned* buf = &ByteCode[IP+1];
                  IP = buf[0];
                  DP = buf[1];
                  break;
              }

// Operators:
          case   cNeg: Stack[SP] = -Stack[SP]; break;
          case   cAdd: Stack[SP-1] += Stack[SP]; --SP; break;
          case   cSub: Stack[SP-1] -= Stack[SP]; --SP; break;
          case   cMul: Stack[SP-1] *= Stack[SP]; --SP; break;

          case   cDiv:
#           ifndef FP_NO_EVALUATION_CHECKS
              if(Stack[SP] == Value_t(0))
              { evalErrorType=1; return Value_t(0); }
#           endif
              Stack[SP-1] /= Stack[SP]; --SP; break;

          case   cMod:
#           ifndef FP_NO_EVALUATION_CHECKS
              if(Stack[SP] == Value_t(0))
              { evalErrorType=1; return Value_t(0); }
#           endif
              Stack[SP-1] = fp_mod(Stack[SP-1], Stack[SP]);
              --SP; break;

          case cEqual:
              Stack[SP-1] = Value_t
                  (fp_abs(Stack[SP-1] - Stack[SP]) <= fp_epsilon<Value_t>());
              --SP; break;

          case cNEqual:
              Stack[SP-1] = Value_t
                  (fp_abs(Stack[SP-1] - Stack[SP]) >= fp_epsilon<Value_t>());
              --SP; break;

          case  cLess:
              Stack[SP-1] = Value_t
                  (Stack[SP-1] < Stack[SP] - fp_epsilon<Value_t>());
              --SP; break;

          case  cLessOrEq:
              Stack[SP-1] = Value_t
                  (Stack[SP-1] <= Stack[SP] + fp_epsilon<Value_t>());
              --SP; break;

          case cGreater:
              Stack[SP-1] = Value_t
                  (Stack[SP] < Stack[SP-1] - fp_epsilon<Value_t>());
              --SP; break;

          case cGreaterOrEq:
              Stack[SP-1] = Value_t
                  (Stack[SP] <= Stack[SP-1] + fp_epsilon<Value_t>());
              --SP; break;

          case   cNot: Stack[SP] = Value_t(!truthValue(Stack[SP])); break;

          case   cAnd:
              Stack[SP-1] = Value_t
                  (truthValue(Stack[SP-1]) && truthValue(Stack[SP]));
              --SP; break;

          case    cOr:
              Stack[SP-1] = Value_t
                  (truthValue(Stack[SP-1]) || truthValue(Stack[SP]));
              --SP; break;

          case cNotNot: Stack[SP] = Value_t(truthValue(Stack[SP])); break;

// Degrees-radians conversion:
          case   cDeg: Stack[SP] = RadiansToDegrees(Stack[SP]); break;
          case   cRad: Stack[SP] = DegreesToRadians(Stack[SP]); break;

// User-defined function calls:
          case cFCall:
              {
                  unsigned index = ByteCode[++IP];
                  unsigned params = data->FuncPtrs[index].params;
                  Value_t retVal =
                      data->FuncPtrs[index].funcPtr(&Stack[SP-params+1]);
                  SP -= int(params)-1;
                  Stack[SP] = retVal;
                  break;
              }

          case cPCall:
              {
                  unsigned index = ByteCode[++IP];
                  unsigned params = data->FuncParsers[index].params;
                  Value_t retVal =
                      data->FuncParsers[index].parserPtr->Eval
                      (&Stack[SP-params+1]);
                  SP -= int(params)-1;
                  Stack[SP] = retVal;
                  const int error =
                      data->FuncParsers[index].parserPtr->EvalError();
                  if(error)
                  {
                      evalErrorType = error;
                      return 0;
                  }
                  break;
              }


#ifdef FP_SUPPORT_OPTIMIZER
          case   cVar: break;  // Paranoia. These should never exist

          case   cFetch:
              {
                  unsigned stackOffs = ByteCode[++IP];
                  Stack[SP+1] = Stack[stackOffs]; ++SP;
                  break;
              }

          case   cPopNMov:
              {
                  unsigned stackOffs_target = ByteCode[++IP];
                  unsigned stackOffs_source = ByteCode[++IP];
                  Stack[stackOffs_target] = Stack[stackOffs_source];
                  SP = stackOffs_target;
                  break;
              }

          case  cLog2by:
#           ifndef FP_NO_EVALUATION_CHECKS
              if(Stack[SP-1] <= Value_t(0))
              { evalErrorType=3; return Value_t(0); }
#           endif
              Stack[SP-1] = fp_log2(Stack[SP-1]) * Stack[SP];
              --SP;
              break;
#endif // FP_SUPPORT_OPTIMIZER
          case cAbsNot:
              Stack[SP] = Value_t(!truthValue_abs(Stack[SP])); break;
          case cAbsNotNot:
              Stack[SP] = Value_t(truthValue_abs(Stack[SP])); break;
          case cAbsAnd:
              Stack[SP-1] = Value_t(truthValue_abs(Stack[SP-1]) &&
                                    truthValue_abs(Stack[SP]));
              --SP; break;
          case cAbsOr:
              Stack[SP-1] = Value_t(truthValue_abs(Stack[SP-1]) ||
                                    truthValue_abs(Stack[SP]));
              --SP; break;
          case cAbsIf:
              if(truthValue_abs(Stack[SP--]))
                  IP += 2;
              else
              {
                  const unsigned* buf = &ByteCode[IP+1];
                  IP = buf[0];
                  DP = buf[1];
              }
              break;

          case   cDup: Stack[SP+1] = Stack[SP]; ++SP; break;

          case   cInv:
#           ifndef FP_NO_EVALUATION_CHECKS
              if(Stack[SP] == Value_t(0))
              { evalErrorType=1; return Value_t(0); }
#           endif
              Stack[SP] = Value_t(1)/Stack[SP];
              break;

          case   cSqr:
              Stack[SP] = Stack[SP]*Stack[SP];
              break;

          case   cRDiv:
#           ifndef FP_NO_EVALUATION_CHECKS
              if(Stack[SP-1] == Value_t(0))
              { evalErrorType=1; return Value_t(0); }
#           endif
              Stack[SP-1] = Stack[SP] / Stack[SP-1]; --SP; break;

          case   cRSub: Stack[SP-1] = Stack[SP] - Stack[SP-1]; --SP; break;

          case   cRSqrt:
#           ifndef FP_NO_EVALUATION_CHECKS
              if(Stack[SP] == Value_t(0))
              { evalErrorType=1; return Value_t(0); }
#           endif
              Stack[SP] = Value_t(1) / sqrt(Stack[SP]); break;

          case cNop: break;

// Variables:
          default:
              Stack[++SP] = Vars[ByteCode[IP]-VarBegin];
        }
    }

    evalErrorType=0;
    return Stack[SP];
}


//===========================================================================
// Variable deduction
//===========================================================================
namespace
{
    template<typename Value_t>
    int deduceVariables(FunctionParserBase<Value_t>& fParser,
                        const char* funcStr,
                        std::string& destVarString,
                        int* amountOfVariablesFound,
                        std::vector<std::string>* destVarNames,
                        bool useDegrees)
    {
        typedef std::set<std::string> StrSet;
        StrSet varNames;

        int oldIndex = -1;

        while(true)
        {
            destVarString.clear();
            for(StrSet::iterator iter = varNames.begin();
                iter != varNames.end();
                ++iter)
            {
                if(iter != varNames.begin()) destVarString += ",";
                destVarString += *iter;
            }

            const int index =
                fParser.Parse(funcStr, destVarString, useDegrees);
            if(index < 0) break;
            if(index == oldIndex) return index;

            unsigned nameLength = readOpcode(funcStr + index);
            if(nameLength & 0x80000000U) return index;
            if(nameLength == 0) return index;

            varNames.insert(std::string(funcStr + index, nameLength));
            oldIndex = index;
        }

        if(amountOfVariablesFound)
            *amountOfVariablesFound = int(varNames.size());

        if(destVarNames)
            destVarNames->assign(varNames.begin(), varNames.end());

        return -1;
    }
}

template<typename Value_t>
int FunctionParserBase<Value_t>::ParseAndDeduceVariables
(const std::string& function,
 int* amountOfVariablesFound,
 bool useDegrees)
{
    std::string varString;
    return deduceVariables(*this, function.c_str(), varString,
                           amountOfVariablesFound, 0, useDegrees);
}

template<typename Value_t>
int FunctionParserBase<Value_t>::ParseAndDeduceVariables
(const std::string& function,
 std::string& resultVarString,
 int* amountOfVariablesFound,
 bool useDegrees)
{
    std::string varString;
    const int index =
        deduceVariables(*this, function.c_str(), varString,
                        amountOfVariablesFound, 0, useDegrees);
    if(index < 0) resultVarString = varString;
    return index;
}

template<typename Value_t>
int FunctionParserBase<Value_t>::ParseAndDeduceVariables
(const std::string& function,
 std::vector<std::string>& resultVars,
 bool useDegrees)
{
    std::string varString;
    std::vector<std::string> vars;
    const int index =
        deduceVariables(*this, function.c_str(), varString,
                        0, &vars, useDegrees);
    if(index < 0) resultVars.swap(vars);
    return index;
}


//===========================================================================
// Debug output
//===========================================================================
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
#include <iomanip>
#include <sstream>
namespace
{
    inline void printHex(std::ostream& dest, unsigned n)
    {
        std::ios::fmtflags flags = dest.flags();
        dest.width(4); dest.fill('0'); std::hex(dest); //uppercase(dest);
        dest << n;
        dest.flags(flags);
    }

    void padLine(std::ostringstream& dest, unsigned destLength)
    {
        for(size_t currentLength = dest.str().length();
            currentLength < destLength;
            ++currentLength)
        {
            dest << ' ';
        }
    }

    template<typename Value_t>
    std::string findName(const namePtrsType<Value_t>& nameMap,
                         unsigned index,
                         typename NameData<Value_t>::DataType type)
    {
        for(typename namePtrsType<Value_t>::const_iterator
                iter = nameMap.begin();
            iter != nameMap.end();
            ++iter)
        {
            if(iter->second.type != type) continue;
            if(iter->second.index == index)
                return std::string(iter->first.name,
                                   iter->first.name + iter->first.nameLength);
        }
        return "?";
    }

    const struct PowiMuliType
    {
        unsigned opcode_square;
        unsigned opcode_cumulate;
        unsigned opcode_invert;
        unsigned opcode_half;
        unsigned opcode_invhalf;
    } iseq_powi = {cSqr,cMul,cInv,cSqrt,cRSqrt},
      iseq_muli = {~unsigned(0), cAdd,cNeg, ~unsigned(0),~unsigned(0) };

    template<typename Value_t>
    Value_t ParsePowiMuli(
        const PowiMuliType& opcodes,
        const std::vector<unsigned>& ByteCode, unsigned& IP,
        unsigned limit,
        size_t factor_stack_base,
        std::vector<Value_t>& stack,
        bool IgnoreExcess)
    {
        Value_t result = Value_t(1);
        while(IP < limit)
        {
            if(ByteCode[IP] == opcodes.opcode_square)
            {
                if(!IsIntegerConst(result)) break;
                result *= Value_t(2);
                ++IP;
                continue;
            }
            if(ByteCode[IP] == opcodes.opcode_invert)
            {
                if(result < Value_t(0)) break;
                result = -result;
                ++IP;
                continue;
            }
            if(ByteCode[IP] == opcodes.opcode_half)
            {
                if(IsIntegerConst(result) && result > Value_t(0) &&
                   ((long)result) % 2 == 0)
                    break;
                if(IsIntegerConst(result * Value_t(0.5))) break;
                result *= Value_t(0.5);
                ++IP;
                continue;
            }
            if(ByteCode[IP] == opcodes.opcode_invhalf)
            {
                if(IsIntegerConst(result) && result > Value_t(0) &&
                   ((long)result) % 2 == 0)
                    break;
                if(IsIntegerConst(result * Value_t(-0.5))) break;
                result *= Value_t(-0.5);
                ++IP;
                continue;
            }

            unsigned dup_fetch_pos = IP;
            Value_t lhs = Value_t(1);

    #ifdef FP_SUPPORT_OPTIMIZER
            if(ByteCode[IP] == cFetch)
            {
                unsigned index = ByteCode[++IP];
                if(index < factor_stack_base
                || size_t(index-factor_stack_base) >= stack.size())
                {
                    // It wasn't a powi-fetch after all
                    IP = dup_fetch_pos;
                    break;
                }
                lhs = stack[index - factor_stack_base];
                // Note: ^This assumes that cFetch of recentmost
                //        is always converted into cDup.
                goto dup_or_fetch;
            }
    #endif
            if(ByteCode[IP] == cDup)
            {
                lhs = result;
                goto dup_or_fetch;

            dup_or_fetch:
                stack.push_back(result);
                ++IP;
                Value_t subexponent = ParsePowiMuli
                    (opcodes,
                     ByteCode, IP, limit,
                     factor_stack_base, stack,
                     IgnoreExcess);
                if(IP >= limit && IgnoreExcess)
                    return lhs*subexponent;
                if(IP >= limit || ByteCode[IP] != opcodes.opcode_cumulate)
                {
                    // It wasn't a powi-dup after all
                    IP = dup_fetch_pos;
                    break;
                }
                ++IP; // skip opcode_cumulate
                stack.pop_back();
                result += lhs*subexponent;
                continue;
            }
            break;
        }
        return result;
    }

    template<typename Value_t>
    Value_t ParsePowiSequence(const std::vector<unsigned>& ByteCode,
                              unsigned& IP, unsigned limit,
                              size_t factor_stack_base,
                              bool IgnoreExcess = false)
    {
        std::vector<Value_t> stack;
        stack.push_back(Value_t(1));
        return ParsePowiMuli(iseq_powi, ByteCode, IP, limit,
                             factor_stack_base, stack,
                             IgnoreExcess);
    }

    template<typename Value_t>
    Value_t ParseMuliSequence(const std::vector<unsigned>& ByteCode,
                              unsigned& IP, unsigned limit,
                              size_t factor_stack_base,
                              bool IgnoreExcess = false)
    {
        std::vector<Value_t> stack;
        stack.push_back(Value_t(1));
        return ParsePowiMuli(iseq_muli, ByteCode, IP, limit,
                             factor_stack_base, stack,
                             IgnoreExcess);
    }

    struct IfInfo
    {
        std::pair<int,std::string> condition;
        std::pair<int,std::string> thenbranch;
        unsigned endif_location;

        IfInfo() : condition(), thenbranch(), endif_location() { }
    };
}

template<typename Value_t>
void FunctionParserBase<Value_t>::PrintByteCode(std::ostream& dest,
                                                bool showExpression) const
{
    dest << "Size of stack: " << data->StackSize << "\n";

    std::ostringstream outputBuffer;
    std::ostream& output = (showExpression ? outputBuffer : dest);

    const std::vector<unsigned>& ByteCode = data->ByteCode;
    const std::vector<Value_t>& Immed = data->Immed;

    std::vector<std::pair<int,std::string> > stack;
    std::vector<IfInfo> if_stack;

    for(unsigned IP = 0, DP = 0; IP <= ByteCode.size(); ++IP)
    {
    after_powi_or_muli:;
        std::string n;
        bool out_params = false;
        unsigned params = 2, produces = 1, opcode = 0;

        if(showExpression && !if_stack.empty() &&
          (   // Normal If termination rule:
              if_stack.back().endif_location == IP
              // This rule matches when cJumps are threaded:
           || (IP < ByteCode.size() && ByteCode[IP] == cJump
               && !if_stack.back().thenbranch.second.empty())
          ))
        {
            printHex(output, IP);
            if(if_stack.back().endif_location == IP)
                output << ": (phi)";
            else
                output << ": (phi_threaded)";

            stack.resize(stack.size()+2);
            std::swap(stack[stack.size()-3], stack[stack.size()-1]);
            std::swap(if_stack.back().condition,  stack[stack.size()-3]);
            std::swap(if_stack.back().thenbranch, stack[stack.size()-2]);
            opcode = cIf;
            params = 3;
            --IP;
            if_stack.pop_back();
        }
        else
        {
            if(IP >= ByteCode.size()) break;
            opcode = ByteCode[IP];

            if(showExpression && (
                opcode == cSqr || opcode == cDup
             || opcode == cInv
             || opcode == cSqrt || opcode == cRSqrt
    #ifdef FP_SUPPORT_OPTIMIZER
             || opcode == cFetch
    #endif
            ))
            {
                unsigned changed_ip = IP;
                Value_t exponent =
                    ParsePowiSequence<Value_t>
                    (ByteCode, changed_ip,
                     if_stack.empty()
                     ? (unsigned)ByteCode.size()
                     : if_stack.back().endif_location,
                     stack.size()-1);
                std::string        operation_prefix;
                std::ostringstream operation_value;
                int prio = 0;
                if(exponent == 1.0)
                {
                    if(opcode != cDup) goto not_powi_or_muli;
                    Value_t factor =
                        ParseMuliSequence<Value_t>
                        (ByteCode, changed_ip,
                         if_stack.empty()
                         ? (unsigned)ByteCode.size()
                         : if_stack.back().endif_location,
                         stack.size()-1);
                    if(factor == Value_t(1) || factor == Value_t(-1))
                        goto not_powi_or_muli;
                    operation_prefix = "*";
                    operation_value << factor;
                    prio = 3;
                }
                else
                {
                    prio = 2;
                    operation_prefix = "^";
                    operation_value << exponent;
                }

                //unsigned explanation_before = changed_ip-2;
                unsigned explanation_before = changed_ip-1;

                const char* explanation_prefix = "_";
                for(const unsigned first_ip = IP; IP < changed_ip; ++IP)
                {
                    printHex(output, IP);
                    output << ": ";

                    const char* sep = "|";
                    if(first_ip+1 == changed_ip)
                    { sep = "="; explanation_prefix = " "; }
                    else if(IP   == first_ip) sep = "\\";
                    else if(IP+1 == changed_ip) sep = "/";
                    else explanation_prefix = "=";

                    switch(ByteCode[IP])
                    {
                        case cInv: output << "inv"; break;
                        case cNeg: output << "neg"; break;
                        case cDup: output << "dup"; break;
                        case cSqr: output << "sqr"; break;
                        case cMul: output << "mul"; break;
                        case cAdd: output << "add"; break;
                        case cCbrt: output << "cbrt"; break;
                        case cSqrt: output << "sqrt"; break;
                        case cRSqrt: output << "rsqrt"; break;
    #ifdef FP_SUPPORT_OPTIMIZER
                        case cFetch:
                        {
                            unsigned index = ByteCode[++IP];
                            output << "cFetch(" << index << ")";
                            break;
                        }
    #endif
                        default: break;
                    }
                    padLine(outputBuffer, 20);
                    output << sep;
                    if(IP >= explanation_before)
                    {
                        explanation_before = (unsigned)ByteCode.size();
                        output << explanation_prefix
                               << '[' << (stack.size()-1) << ']';
                        std::string last = stack.back().second;
                        if(stack.back().first >= prio)
                            last = "(" + last + ")";
                        output << last;
                        output << operation_prefix;
                        output << operation_value.str();
                    }
                    else
                    {
                        unsigned p = first_ip;
                        Value_t exp = operation_prefix=="^" ?
                            ParsePowiSequence<Value_t>
                            (ByteCode, p, IP+1, stack.size()-1, true) :
                            ParseMuliSequence<Value_t>
                            (ByteCode, p, IP+1, stack.size()-1, true);
                        std::string last = stack.back().second;
                        if(stack.back().first >= prio)
                            last = "(" + last + ")";
                        output << " ..." << last;
                        output << operation_prefix;
                        output << exp;
                    }
                    dest << outputBuffer.str() << std::endl;
                    outputBuffer.str("");
                }

                std::string& last = stack.back().second;
                if(stack.back().first >= prio)
                    last = "(" + last + ")";
                last += operation_prefix;
                last += operation_value.str();
                stack.back().first = prio;

                goto after_powi_or_muli;
            }
        not_powi_or_muli:;
            printHex(output, IP);
            output << ": ";

            switch(opcode)
            {
              case cIf:
              {
                  unsigned label = ByteCode[IP+1]+1;
                  output << "jz ";
                  printHex(output, label);
                  params = 1;
                  produces = 0;
                  IP += 2;

                  if_stack.resize(if_stack.size() + 1);
                  std::swap( if_stack.back().condition, stack.back() );
                  if_stack.back().endif_location = (unsigned) ByteCode.size();
                  stack.pop_back();
                  break;
              }
              case cAbsIf:
              {
                  unsigned dp    = ByteCode[IP+2];
                  unsigned label = ByteCode[IP+1]+1;
                  output << "jz_abs " << dp << ",";
                  printHex(output, label);
                  params = 1;
                  produces = 0;
                  IP += 2;

                  if_stack.resize(if_stack.size() + 1);
                  std::swap( if_stack.back().condition, stack.back() );
                  if_stack.back().endif_location = (unsigned) ByteCode.size();
                  stack.pop_back();
                  break;
              }

              case cJump:
              {
                  unsigned dp    = ByteCode[IP+2];
                  unsigned label = ByteCode[IP+1]+1;

                  if(!if_stack.empty() && !stack.empty())
                  {
                      std::swap(if_stack.back().thenbranch, stack.back());
                      if_stack.back().endif_location = label;
                      stack.pop_back();
                  }

                  output << "jump " << dp << ",";
                  printHex(output, label);
                  params = 0;
                  produces = 0;
                  IP += 2;
                  break;
              }
              case cImmed:
              {
                  if(showExpression)
                  {
                      std::ostringstream buf;
                      buf.precision(8);
                      buf << Immed[DP];
                      stack.push_back( std::make_pair(0, buf.str()) );
                  }
                  output.precision(8);
                  output << "push " << Immed[DP];
                  ++DP;
                  produces = 0;
                  break;
              }

              case cFCall:
                  {
                      const unsigned index = ByteCode[++IP];
                      params = data->FuncPtrs[index].params;
                      static std::string name;
                      name = "f:" + findName(data->namePtrs, index,
                                             NameData<Value_t>::FUNC_PTR);
                      n = name.c_str();
                      out_params = true;
                      break;
                  }

              case cPCall:
                  {
                      const unsigned index = ByteCode[++IP];
                      params = data->FuncParsers[index].params;
                      static std::string name;
                      name = "p:" + findName(data->namePtrs, index,
                                             NameData<Value_t>::PARSER_PTR);
                      n = name.c_str();
                      out_params = true;
                      break;
                  }

              default:
                  if(OPCODE(opcode) < VarBegin)
                  {
                      switch(opcode)
                      {
                        case cNeg: n = "neg"; params = 1; break;
                        case cAdd: n = "add"; break;
                        case cSub: n = "sub"; break;
                        case cMul: n = "mul"; break;
                        case cDiv: n = "div"; break;
                        case cMod: n = "mod"; break;
                        case cPow: n = "pow"; break;
                        case cEqual: n = "eq"; break;
                        case cNEqual: n = "neq"; break;
                        case cLess: n = "lt"; break;
                        case cLessOrEq: n = "le"; break;
                        case cGreater: n = "gt"; break;
                        case cGreaterOrEq: n = "ge"; break;
                        case cAnd: n = "and"; break;
                        case cOr: n = "or"; break;
                        case cNot: n = "not"; params = 1; break;
                        case cNotNot: n = "notnot"; params = 1; break;
                        case cDeg: n = "deg"; params = 1; break;
                        case cRad: n = "rad"; params = 1; break;

    #ifndef FP_DISABLE_EVAL
                        case cEval: n = "eval"; params = data->numVariables;
    #endif

    #ifdef FP_SUPPORT_OPTIMIZER
                        case cVar:    n = "(var)"; break;
                        case cLog2by: n = "log2by"; params = 2; out_params = 1; break;
                        case cFetch:
                        {
                            unsigned index = ByteCode[++IP];
                            if(showExpression && index < stack.size())
                                stack.push_back(stack[index]);
                            output << "cFetch(" << index << ")";
                            produces = 0;
                            break;
                        }
                        case cPopNMov:
                        {
                            size_t a = ByteCode[++IP];
                            size_t b = ByteCode[++IP];
                            if(showExpression && b < stack.size())
                            {
                                std::pair<int, std::string> stacktop(0, "?");
                                if(b < stack.size()) stacktop = stack[b];
                                stack.resize(a);
                                stack.push_back(stacktop);
                            }
                            output << "cPopNMov(" << a << ", " << b << ")";
                            produces = 0;
                            break;
                        }
    #endif
                        case cAbsAnd: n = "abs_and"; break;
                        case cAbsOr:  n = "abs_or"; break;
                        case cAbsNot: n = "abs_not"; params = 1; break;
                        case cAbsNotNot: n = "abs_notnot"; params = 1; break;
                        case cDup:
                        {
                            if(showExpression)
                                stack.push_back(stack.back());
                            output << "dup";
                            produces = 0;
                            break;
                        }
                        case cInv: n = "inv"; params = 1; break;
                        case cSqr: n = "sqr"; params = 1; break;
                        case cRDiv: n = "rdiv"; break;
                        case cRSub: n = "rsub"; break;
                        case cRSqrt: n = "rsqrt"; params = 1; break;

                        case cNop:
                            output << "nop"; params = 0; produces = 0;
                            break;

                        default:
                            n = Functions[opcode-cAbs].name;
                            params = Functions[opcode-cAbs].params;
                            out_params = params != 1;
                      }
                  }
                  else
                  {
                      if(showExpression)
                      {
                          stack.push_back(std::make_pair(0,
                              (findName(data->namePtrs, opcode,
                                        NameData<Value_t>::VARIABLE))));
                      }
                      output << "push Var" << opcode-VarBegin;
                      produces = 0;
                  }
            }
        }
        if(produces) output << n;
        if(out_params) output << " (" << params << ")";
        if(showExpression)
        {
            padLine(outputBuffer, 20);

            if(produces > 0)
            {
                std::ostringstream buf;
                const char *paramsep = ",", *suff = "";
                int prio = 0; bool commutative = false;
                switch(opcode)
                {
                  case cIf: buf << "if("; suff = ")";
                      break;
                  case cAbsIf: buf << "if("; suff = ")";
                      break;
                  case cOr:  prio = 6; paramsep = "|"; commutative = true;
                      break;
                  case cAnd: prio = 5; paramsep = "&"; commutative = true;
                      break;
                  case cAdd: prio = 4; paramsep = "+"; commutative = true;
                      break;
                  case cSub: prio = 4; paramsep = "-";
                      break;
                  case cMul: prio = 3; paramsep = "*"; commutative = true;
                      break;
                  case cDiv: prio = 3; paramsep = "/";
                      break;
                  case cPow: prio = 2; paramsep = "^";
                      break;
                  case cAbsOr:  prio = 6; paramsep = "|"; commutative = true;
                      break;
                  case cAbsAnd: prio = 5; paramsep = "&"; commutative = true;
                      break;
                  case cSqr: prio = 2; suff = "^2";
                      break;
                  case cNeg: buf << "(-("; suff = "))";
                      break;
                  default: buf << n << '('; suff = ")";
                }

                const char* sep = "";
                for(unsigned a=0; a<params; ++a)
                {
                    buf << sep;
                    if(stack.size() + a < params)
                        buf << "?";
                    else
                    {
                        const std::pair<int,std::string>& prev =
                            stack[stack.size() - params + a];
                        if(prio > 0 && (prev.first > prio ||
                                        (prev.first==prio && !commutative)))
                            buf << '(' << prev.second << ')';
                        else
                            buf << prev.second;
                    }
                    sep = paramsep;
                }
                if(stack.size() >= params)
                    stack.resize(stack.size() - params);
                else
                    stack.clear();
                buf << suff;
                stack.push_back(std::make_pair(prio, buf.str()));
                //if(n.size() <= 4 && !out_params) padLine(outputBuffer, 20);
            }
            //padLine(outputBuffer, 20);
            output << "= ";
            if(((opcode == cIf || opcode == cAbsIf) && params != 3)
              || opcode == cJump || opcode == cNop)
                output << "(void)";
            else if(stack.empty())
                output << "[?] ?";
            else
                output << '[' << (stack.size()-1) << ']'
                       << stack.back().second;
        }

        if(showExpression)
        {
            dest << outputBuffer.str() << std::endl;
            outputBuffer.str("");
        }
        else
            output << std::endl;
    }
}
#endif


#ifndef FP_SUPPORT_OPTIMIZER
template<typename Value_t>
void FunctionParserBase<Value_t>::Optimize()
{
    // Do nothing if no optimizations are supported.
}
#endif

template<>
void FunctionParserBase<float>::Optimize()
{}

template<>
void FunctionParserBase<long double>::Optimize()
{}

FUNCTIONPARSER_INSTANTIATE_TYPES
