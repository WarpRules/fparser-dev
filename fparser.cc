/***************************************************************************\
|* Function Parser for C++ v3.3.2                                          *|
|*-------------------------------------------------------------------------*|
|* Copyright: Juha Nieminen                                                *|
\***************************************************************************/

#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"
using namespace FUNCTIONPARSERTYPES;

#include <set>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cmath>
#include <cassert>
using namespace std;

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
    bool addNewNameData(std::set<NameData>& nameData,
                        std::map<NamePtr, const NameData*>& namePtrs,
                        const NameData& newData)
    {
        const FuncDefinition* funcDef =
            findFunction(NamePtr(&(newData.name[0]),
                                 unsigned(newData.name.size())));
        if(funcDef && funcDef->enabled())
            return false;

        std::set<NameData>::iterator dataIter = nameData.find(newData);

        if(dataIter != nameData.end())
        {
            if(dataIter->type != newData.type) return false;
            namePtrs.erase(NamePtr(&(dataIter->name[0]),
                                   unsigned(dataIter->name.size())));
            nameData.erase(dataIter);
        }

        dataIter = nameData.insert(newData).first;
        namePtrs[NamePtr(&(dataIter->name[0]),
                         unsigned(dataIter->name.size()))] = &(*dataIter);
        return true;
    }

    const char* readIdentifier(const char* ptr)
    {
        static const char A=10, B=11;
        /*  ^ define numeric constants for two-digit numbers
         *    so as not to disturb the layout of this neat table
         */
        static const char tab[0x100] =
        {
            0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, //00-0F
            0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, //10-1F
            0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, //20-2F
            9,9,9,9, 9,9,9,9, 9,9,0,0, 0,0,0,0, //30-3F
            0,2,2,2, 2,2,2,2, 2,2,2,2, 2,2,2,2, //40-4F
            2,2,2,2, 2,2,2,2, 2,2,2,0, 0,0,0,2, //50-5F
            0,2,2,2, 2,2,2,2, 2,2,2,2, 2,2,2,2, //60-6F
            2,2,2,2, 2,2,2,2, 2,2,2,0, 0,0,0,0, //70-7F
            8,8,8,8, 8,8,8,8, 8,8,8,8, 8,8,8,8, //80-8F
            A,A,A,A, A,A,A,A, A,A,A,A, A,A,A,A, //90-9F
            B,B,B,B, B,B,B,B, B,B,B,B, B,B,B,B, //A0-AF
            B,B,B,B, B,B,B,B, B,B,B,B, B,B,B,B, //B0-BF
            0,0,4,4, 4,4,4,4, 4,4,4,4, 4,4,4,4, //C0-CF
            4,4,4,4, 4,4,4,4, 4,4,4,4, 4,4,4,4, //D0-DF
            5,3,3,3, 3,3,3,3, 3,3,3,3, 3,0,3,3, //E0-EC, EE-EF
            6,1,1,1, 7,0,0,0, 0,0,0,0, 0,0,0,0  //F0-FF
        };
        /* Classes:
         *   9 = digits    (30-39)
         *   2 = A-Z_a-z   (41-5A, 5F, 61-7A)
         *   8 = 80-8F
         *   A = 90-9F
         *   B = A0-BF
         *   4 = C2-DF
         *   5 = E0
         *   3 = E1-EC, EE-EF
         *   6 = F0
         *   1 = F1-F3
         *   7 = F4
         *
         * Allowed multibyte utf8 sequences consist of these class options:
         *   [4]             [8AB]
         *   [5]         [B] [8AB]
         *   [3]       [8AB] [8AB]
         *   [6] [AB]  [8AB] [8AB]
         *   [1] [8AB] [8AB] [8AB]
         *   [7] [8]   [8AB] [8AB]
         * In addition, the first characters may be
         *   [2]
         * And the following characters may be
         *   [92]
         * These may never begin the character:
         *   [08AB]
         *
         * The numberings are such chosen to optimize the
         * following switch-statements for code generation.
         */

        const unsigned char* uptr = (const unsigned char*) ptr;
        switch(tab[uptr[0]])
        {
            case 2: goto loop_2; // A-Z_a-z
            case 5: goto loop_5; // E0
            case 3: goto loop_3; // E1-EC, EE-EF
            case 4: goto loop_4; // C2-DF

            case 1: goto loop_1; // F0-F4 XXX F1-F3
            case 6: goto loop_6; //       XXX F0
            case 7: goto loop_7; //       XXX F4
        }
        return (const char*) uptr;

    loop:
        switch(tab[uptr[0]])
        {
            case 9: // 0-9
            case 2: // A-Z_a-z
            loop_2:
                uptr += 1;
                goto loop;
            case 6: // F0:
            loop_6:
                if(uptr[1] < 0x90 || uptr[1] > 0xBF) break;
                goto len4pos2;
            case 1: // F1-F3:
            loop_1:
                if(uptr[1] < 0x80 || uptr[1] > 0xBF) break;
            len4pos2:
                if(uptr[2] < 0x80 || uptr[2] > 0xBF) break;
                if(uptr[3] < 0x80 || uptr[3] > 0xBF) break;
                uptr += 4;
                goto loop;
            case 7: // F4:
            loop_7:
                if(tab[uptr[1]] != 8) break;
                goto len4pos2;
            case 5: // E0
            loop_5:
                if(tab[uptr[1]] != B) break;
                goto len3pos2;
            case 3: // E1-EC, EE-EF
            loop_3:
                if(uptr[1] < 0x80 || uptr[1] > 0xBF) break;
            len3pos2:
                if(uptr[2] < 0x80 || uptr[2] > 0xBF) break;
                uptr += 3;
                goto loop;
            case 4: // C2-DF
            loop_4:
                if(uptr[1] < 0x80 || uptr[1] > 0xBF) break;
                uptr += 2;
                goto loop;
        }
        return (const char*) uptr;
    }

    bool containsOnlyValidNameChars(const std::string& name)
    {
        if(name.empty()) return false;
        const char* endPtr = readIdentifier(name.c_str());
        return *endPtr == '\0';
    }

    inline bool truthValue(double d)
    {
        //return !!(d<0 ? -int((-d)+.5) : int(d+.5));
        return fabs(d) >= 0.5;
    }
    inline bool truthValue_abs(double abs_d)
    {
        return abs_d >= 0.5;
    }

    inline double Min(double d1, double d2)
    {
        return d1<d2 ? d1 : d2;
    }
    inline double Max(double d1, double d2)
    {
        return d1>d2 ? d1 : d2;
    }

    inline double DegreesToRadians(double degrees)
    {
        return degrees*(M_PI/180.0);
    }
    inline double RadiansToDegrees(double radians)
    {
        return radians*(180.0/M_PI);
    }

    inline bool isEvenInteger(double value)
    {
        long longval = (long)value;
        return FloatEqual(value, (double)longval)
            && (longval%2) == 0;
    }
    inline bool isOddInteger(double value)
    {
        long longval = (long)value;
        return FloatEqual(value, (double)longval)
            && (longval%2) != 0;
    }
}


//=========================================================================
// Data struct implementation
//=========================================================================
FunctionParser::Data::Data(const Data& rhs):
    referenceCounter(0),
    variablesString(),
    variableRefs(),
    nameData(rhs.nameData),
    namePtrs(),
    FuncPtrs(rhs.FuncPtrs),
    FuncParsers(rhs.FuncParsers),
    ByteCode(rhs.ByteCode),
    Immed(rhs.Immed),
    Stack(),
    StackSize(rhs.StackSize)
{
    Stack.resize(rhs.Stack.size());

    for(std::set<NameData>::const_iterator iter = nameData.begin();
        iter != nameData.end(); ++iter)
    {
        namePtrs[NamePtr(&(iter->name[0]), unsigned(iter->name.size()))] =
            &(*iter);
    }
}


//=========================================================================
// FunctionParser constructors, destructor and assignment
//=========================================================================
FunctionParser::FunctionParser():
    delimiterChar(0),
    parseErrorType(NO_FUNCTION_PARSED_YET), evalErrorType(0),
    data(new Data),
    useDegreeConversion(false),
    evalRecursionLevel(0),
    StackPtr(0), errorLocation(0)
{
}

FunctionParser::~FunctionParser()
{
    if(--(data->referenceCounter) == 0)
        delete data;
}

FunctionParser::FunctionParser(const FunctionParser& cpy):
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

FunctionParser& FunctionParser::operator=(const FunctionParser& cpy)
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

void FunctionParser::setDelimiterChar(char c)
{
    delimiterChar = c;
}


//---------------------------------------------------------------------------
// Copy-on-write method
//---------------------------------------------------------------------------
void FunctionParser::CopyOnWrite()
{
    if(data->referenceCounter > 1)
    {
        Data* oldData = data;
        data = new Data(*oldData);
        --(oldData->referenceCounter);
        data->referenceCounter = 1;
    }
}

void FunctionParser::ForceDeepCopy()
{
    CopyOnWrite();
}


//=========================================================================
// User-defined constant and function addition
//=========================================================================
bool FunctionParser::AddConstant(const std::string& name, double value)
{
    if(!containsOnlyValidNameChars(name)) return false;

    CopyOnWrite();
    NameData newData(NameData::CONSTANT, name);
    newData.value = value;
    return addNewNameData(data->nameData, data->namePtrs, newData);
}

bool FunctionParser::AddUnit(const std::string& name, double value)
{
    if(!containsOnlyValidNameChars(name)) return false;

    CopyOnWrite();
    NameData newData(NameData::UNIT, name);
    newData.value = value;
    return addNewNameData(data->nameData, data->namePtrs, newData);
}

bool FunctionParser::AddFunction(const std::string& name,
                                 FunctionPtr ptr, unsigned paramsAmount)
{
    if(!containsOnlyValidNameChars(name)) return false;

    CopyOnWrite();
    NameData newData(NameData::FUNC_PTR, name);
    newData.index = unsigned(data->FuncPtrs.size());

    data->FuncPtrs.push_back(Data::FuncPtrData());
    data->FuncPtrs.back().funcPtr = ptr;
    data->FuncPtrs.back().params = paramsAmount;

    const bool retval = addNewNameData(data->nameData, data->namePtrs, newData);
    if(!retval) data->FuncPtrs.pop_back();
    return retval;
}

bool FunctionParser::CheckRecursiveLinking(const FunctionParser* fp) const
{
    if(fp == this) return true;
    for(unsigned i = 0; i < fp->data->FuncParsers.size(); ++i)
        if(CheckRecursiveLinking(fp->data->FuncParsers[i].parserPtr))
            return true;
    return false;
}

bool FunctionParser::AddFunction(const std::string& name, FunctionParser& fp)
{
    if(!containsOnlyValidNameChars(name) || CheckRecursiveLinking(&fp))
        return false;

    CopyOnWrite();
    NameData newData(NameData::PARSER_PTR, name);
    newData.index = unsigned(data->FuncParsers.size());

    data->FuncParsers.push_back(Data::FuncPtrData());
    data->FuncParsers.back().parserPtr = &fp;
    data->FuncParsers.back().params = unsigned(fp.data->variableRefs.size());

    const bool retval = addNewNameData(data->nameData, data->namePtrs, newData);
    if(!retval) data->FuncParsers.pop_back();
    return retval;
}

bool FunctionParser::RemoveIdentifier(const std::string& name)
{
    CopyOnWrite();

    const NameData dataToRemove(NameData::CONSTANT, name);
    std::set<NameData>::iterator dataIter = data->nameData.find(dataToRemove);

    if(dataIter != data->nameData.end())
    {
        data->namePtrs.erase(NamePtr(&(dataIter->name[0]),
                                     unsigned(dataIter->name.size())));
        data->nameData.erase(dataIter);
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
}

// Return parse error message
// --------------------------
const char* FunctionParser::ErrorMsg() const
{
    return ParseErrorMessage[parseErrorType];
}


// Parse variables
// ---------------
bool FunctionParser::ParseVariables(const std::string& inputVarString)
{
    if(data->variablesString == inputVarString) return true;

    data->variableRefs.clear();
    data->variablesString = inputVarString;

    const std::string& vars = data->variablesString;
    const unsigned len = unsigned(vars.size());

    unsigned varNumber = VarBegin;

    const char* beginPtr = vars.c_str();
    const char* finalPtr = beginPtr + len;

    while(beginPtr < finalPtr)
    {
        const char* endPtr = readIdentifier(beginPtr);
        if(endPtr == beginPtr) return false;
        if(endPtr != finalPtr && *endPtr != ',') return false;

        NamePtr namePtr(beginPtr, unsigned(endPtr - beginPtr));

        const FuncDefinition* funcDef = findFunction(namePtr);
        if(funcDef && funcDef->enabled()) return false;

        std::map<NamePtr, const NameData*>::iterator nameIter =
            data->namePtrs.find(namePtr);
        if(nameIter != data->namePtrs.end()) return false;

        if(!(data->variableRefs.insert(make_pair(namePtr, varNumber++)).second))
            return false;

        beginPtr = endPtr + 1;
    }
    return true;
}

// Parse interface functions
// -------------------------
int FunctionParser::Parse(const char* Function, const std::string& Vars,
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

int FunctionParser::Parse(const std::string& Function, const std::string& Vars,
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
int FunctionParser::ParseFunction(const char* function, bool useDegrees)
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
inline const char* FunctionParser::SetErrorType(ParseErrorType t,
                                                const char* pos)
{
    parseErrorType = t;
    errorLocation = pos;
    return 0;
}

inline void FunctionParser::incStackPtr()
{
    if(++StackPtr > data->StackSize) ++(data->StackSize);
}

namespace
{
    bool IsEligibleIntPowiExponent(int int_exponent)
    {
        int abs_int_exponent = int_exponent;
        if(abs_int_exponent < 0) abs_int_exponent = -abs_int_exponent;

        return (abs_int_exponent >= 1)
            && (abs_int_exponent <= 46 ||
              (abs_int_exponent <= 1024 &&
              (abs_int_exponent & (abs_int_exponent - 1)) == 0));
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
    static const double EpsilonOrZero = FP_EPSILON;
#else
    static const double EpsilonOrZero = 0.0;
#endif

}

#ifdef FP_SUPPORT_OPTIMIZER
namespace FPoptimizer_ByteCode
{
    extern signed char powi_table[256];
}
#endif

inline void FunctionParser::AddImmedOpcode(double value)
{
    data->Immed.push_back(value);
    data->ByteCode.push_back(cImmed);
}

inline void FunctionParser::CompilePowi(int int_exponent)
{
    int num_muls=0;
    while(int_exponent > 1)
    {
#ifdef FP_SUPPORT_OPTIMIZER
        if(int_exponent < 256)
        {
            int half = FPoptimizer_ByteCode::powi_table[int_exponent];
            if(half != 1 && !(int_exponent % half))
            {
                CompilePowi(half);
                int_exponent /= half;
                continue;
            }
            else if(half >= 3)
            {
                data->ByteCode.push_back(cDup);
                incStackPtr();
                CompilePowi(half);
                data->ByteCode.push_back(cMul);
                --StackPtr;
                int_exponent -= half+1;
                continue;
            }
        }
#endif
        if(!(int_exponent & 1))
        {
            int_exponent /= 2;
            data->ByteCode.push_back(cSqr);
        }
        else
        {
            data->ByteCode.push_back(cDup);
            incStackPtr();
            int_exponent -= 1;
            ++num_muls;
        }
    }
    if(num_muls > 0)
    {
        data->ByteCode.resize(data->ByteCode.size()+num_muls,
                              cMul);
        StackPtr -= num_muls;
    }
}

inline bool FunctionParser::TryCompilePowi(double original_immed)
{
    double changed_immed = original_immed;
    for(int sqrt_count=0; /**/; ++sqrt_count)
    {
        int int_exponent = (int)changed_immed;
        if(changed_immed == (double)int_exponent
        && IsEligibleIntPowiExponent(int_exponent))
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
    if(!IsIntegerConst(original_immed)
    || IsNeverNegativeValueOpcode(data->ByteCode[data->ByteCode.size()-2])
      )
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

namespace
{
    inline FunctionParser::ParseErrorType noCommaError(char c)
    {
        return c == ')' ?
            FunctionParser::ILL_PARAMS_AMOUNT : FunctionParser::SYNTAX_ERROR;
    }

    inline FunctionParser::ParseErrorType noParenthError(char c)
    {
        return c == ',' ?
            FunctionParser::ILL_PARAMS_AMOUNT : FunctionParser::MISSING_PARENTH;
    }
}

const char* FunctionParser::CompileIf(const char* function)
{
    if(*function != '(') return SetErrorType(EXPECT_PARENTH_FUNC, function);

    function = CompileExpression(function+1);
    if(!function) return 0;
    if(*function != ',') return SetErrorType(noCommaError(*function), function);

    OPCODE opcode = cIf;
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
    if(*function != ',') return SetErrorType(noCommaError(*function), function);

    data->ByteCode.push_back(cJump);
    const unsigned curByteCodeSize2 = unsigned(data->ByteCode.size());
    const unsigned curImmedSize2 = unsigned(data->Immed.size());
    data->ByteCode.push_back(0); // Jump index; to be set later
    data->ByteCode.push_back(0); // Immed jump index; to be set later

    --StackPtr;

    function = CompileExpression(function + 1);
    if(!function) return 0;
    if(*function != ')')
        return SetErrorType(noParenthError(*function), function);

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
    while(isspace(*function)) ++function;
    return function;
}

const char* FunctionParser::CompileFunctionParams(const char* function,
                                                  unsigned requiredParams)
{
    if(*function != '(') return SetErrorType(EXPECT_PARENTH_FUNC, function);

    if(requiredParams > 0)
    {
        function = CompileExpression(function+1);
        if(!function) return 0;

        for(unsigned i = 1; i < requiredParams; ++i)
        {
            if(*function != ',')
                return SetErrorType(noCommaError(*function), function);

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
        while(isspace(*function)) ++function;
    }

    if(*function != ')')
        return SetErrorType(noParenthError(*function), function);
    ++function;
    while(isspace(*function)) ++function;
    return function;
}

const char* FunctionParser::CompileElement(const char* function)
{
    switch( (unsigned char) *function)
    {
      case '(': // Expression in parentheses
      {
          ++function;
          while(isspace(*function)) ++function;
          if(*function == ')') return SetErrorType(EMPTY_PARENTH, function);

          function = CompileExpression(function);
          if(!function) return 0;

          if(*function != ')') return SetErrorType(MISSING_PARENTH, function);

          ++function;
          while(isspace(*function)) ++function;
          return function;
      }

      case '.': case '0': case '1': case '2':
      case '3': case '4': case '5': case '6':
      case '7': case '8': case '9': // Number
      {
          char* endPtr;
          const double val = strtod(function, &endPtr);
          if(endPtr == function) return SetErrorType(SYNTAX_ERROR, function);

          AddImmedOpcode(val);
          incStackPtr();

          while(isspace(*endPtr)) ++endPtr;
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

    const char* endPtr = readIdentifier(function);
    if(endPtr != function) // Function, variable or constant
    {
        NamePtr name(function, unsigned(endPtr - function));
        while(isspace(*endPtr)) ++endPtr;

        const FuncDefinition* funcDef = findFunction(name);
        if(funcDef && funcDef->enabled()) // is function
        {
            if(funcDef->opcode == cIf) // "if" is a special case
                return CompileIf(endPtr);

            unsigned requiredParams = funcDef->params;
#ifndef FP_DISABLE_EVAL
            if(funcDef->opcode == cEval)
                requiredParams = unsigned(data->variableRefs.size();
#endif

            function = CompileFunctionParams(endPtr, requiredParams);
            if(!function) return 0;

            if(useDegreeConversion)
            {
                if(funcDef->flags & FuncDefinition::AngleIn)
                    AddFunctionOpcode(cRad);

                AddFunctionOpcode(funcDef->opcode);

                if(funcDef->flags & FuncDefinition::AngleOut)
                    AddFunctionOpcode(cDeg);
            }
            else
            {
                AddFunctionOpcode(funcDef->opcode);
            }
            return function;
        }

        std::map<NamePtr, unsigned>::iterator varIter =
            data->variableRefs.find(name);
        if(varIter != data->variableRefs.end()) // is variable
        {
            data->ByteCode.push_back(varIter->second);
            incStackPtr();
            return endPtr;
        }

        std::map<NamePtr, const NameData*>::iterator nameIter =
            data->namePtrs.find(name);
        if(nameIter != data->namePtrs.end())
        {
            const NameData* nameData = nameIter->second;
            switch(nameData->type)
            {
              case NameData::CONSTANT:
                  AddImmedOpcode(nameData->value);
                  incStackPtr();
                  return endPtr;

              case NameData::UNIT: break;

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
              case NameData::FUNC_PTR:
                  function = CompileFunctionParams
                      (endPtr, data->FuncPtrs[nameData->index].params);
                  data->ByteCode.push_back(cFCall);
                  data->ByteCode.push_back(nameData->index);
                  data->ByteCode.push_back(cNop);
                  return function;

              case NameData::PARSER_PTR:
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

const char* FunctionParser::CompilePossibleUnit(const char* function)
{
    const char* endPtr = readIdentifier(function);

    if(endPtr != function)
    {
        NamePtr name(function, unsigned(endPtr - function));
        while(isspace(*endPtr)) ++endPtr;

        std::map<NamePtr, const NameData*>::iterator nameIter =
            data->namePtrs.find(name);
        if(nameIter != data->namePtrs.end())
        {
            const NameData* nameData = nameIter->second;
            if(nameData->type == NameData::UNIT)
            {
                AddImmedOpcode(nameData->value);
                incStackPtr();
                AddFunctionOpcode(cMul);
                --StackPtr;
                return endPtr;
            }
        }
    }

    return function;
}

inline const char* FunctionParser::CompilePow(const char* function)
{
    function = CompileElement(function);
    if(!function) return 0;
    function = CompilePossibleUnit(function);

    if(*function == '^')
    {
        ++function;
        while(isspace(*function)) ++function;

        bool base_is_immed = false;
        double base_immed = 0;
        if(data->ByteCode.back() == cImmed)
        {
            base_is_immed = true;
            base_immed = data->Immed.back();
            data->Immed.pop_back();
            data->ByteCode.pop_back();
        }

        function = CompileUnaryMinus(function);
        if(!function) return 0;

        // Check if the exponent is a literal
        if(data->ByteCode.back() == cImmed)
        {
            // If operator is applied to two literals, calculate it now:
            if(base_is_immed)
                data->Immed.back() = fp_pow(base_immed, data->Immed.back());
            else
                AddFunctionOpcode(cPow);
        }
        else if(base_is_immed)
        {
            if(base_immed > 0.0)
            {
                double mulvalue = std::log(base_immed);
                AddImmedOpcode(mulvalue);
                incStackPtr();
                AddFunctionOpcode(cMul);
                --StackPtr;
                AddFunctionOpcode(cExp);
            }
            else /* uh-oh, we've got e.g. (-5)^x, and we already deleted
                    -5 from the stack */
            {
                AddImmedOpcode(base_immed);
                incStackPtr();
                AddFunctionOpcode(cRPow);
            }
        }
        else // add opcode
            AddFunctionOpcode(cPow);

        --StackPtr;
    }
    return function;
}

inline const char* FunctionParser::CompileUnaryMinus(const char* function)
{
    char op = *function;
    switch(op)
    {
        case '-':
        case '!':
            ++function;
            while(isspace(*function)) ++function;

            function = CompileUnaryMinus(function);
            if(!function) return 0;

            AddFunctionOpcode(op=='-' ? cNeg : cNot);

            return function;

        default:
            return CompilePow(function);
    }
}

inline const char* FunctionParser::CompileMult(const char* function)
{
    unsigned op=0;
    for(; ; )
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
        while(isspace(*function)) ++function;

        if(op == cDiv
        && data->ByteCode.back() == cImmed
        && data->Immed.back() == 1.0)
        {
            op = cInv;
            data->Immed.pop_back();
            data->ByteCode.pop_back();
        }
    }
    return function;

}

inline const char* FunctionParser::CompileAddition(const char* function)
{
    unsigned op=0;
    for(; ; )
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
        while(isspace(*function)) ++function;

        if(op == cSub
        && data->ByteCode.back() == cImmed
        && data->Immed.back() == 0.0)
        {
            op = cNeg;
            data->Immed.pop_back();
            data->ByteCode.pop_back();
        }
    }
    return function;
}

inline const char* FunctionParser::CompileComparison(const char* function)
{
    unsigned op=0;
    for(;;)
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
        while(isspace(*function)) ++function;
    }
    return function;
}

inline const char* FunctionParser::CompileAnd(const char* function)
{
    size_t param0end=0;
    for(;;)
    {
        function = CompileComparison(function);
        if(!function) return 0;

        if(param0end)
        {
            if(IsNeverNegativeValueOpcode(data->ByteCode.back())
            && IsNeverNegativeValueOpcode(data->ByteCode[param0end-1]))
                AddFunctionOpcode(cAbsAnd);
            else
                AddFunctionOpcode(cAnd);
            --StackPtr;
        }
        if(*function != '&') break;
        ++function;
        while(isspace(*function)) ++function;
        param0end = data->ByteCode.size();
    }
    return function;
}

const char* FunctionParser::CompileExpression(const char* function)
{
    size_t param0end=0;
    for(;;)
    {
        while(isspace(*function)) ++function;
        function = CompileAnd(function);
        if(!function) return 0;

        if(param0end)
        {
            if(IsNeverNegativeValueOpcode(data->ByteCode.back())
            && IsNeverNegativeValueOpcode(data->ByteCode[param0end-1]))
                AddFunctionOpcode(cAbsOr);
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
double FunctionParser::Eval(const double* Vars)
{
    if(parseErrorType != FP_NO_ERROR) return 0.0;

    const unsigned* const ByteCode = &(data->ByteCode[0]);
    const double* const Immed = data->Immed.empty() ? 0 : &(data->Immed[0]);
    const unsigned ByteCodeSize = unsigned(data->ByteCode.size());
    unsigned IP, DP=0;
    int SP=-1;

#ifdef FP_USE_THREAD_SAFE_EVAL
#ifdef FP_USE_THREAD_SAFE_EVAL_WITH_ALLOCA
    double* const Stack = (double*)alloca(data->StackSize*sizeof(double));
#else
    std::vector<double> Stack(data->StackSize);
#endif
#else
    std::vector<double>& Stack = data->Stack;
#endif

    for(IP=0; IP<ByteCodeSize; ++IP)
    {
        switch(ByteCode[IP])
        {
// Functions:
          case   cAbs: Stack[SP] = fabs(Stack[SP]); break;

          case  cAcos:
#                    ifndef FP_NO_EVALUATION_CHECKS
                       if(Stack[SP] < -1 || Stack[SP] > 1)
                       { evalErrorType=4; return 0; }
#                    endif
                       Stack[SP] = acos(Stack[SP]); break;

          case cAcosh: Stack[SP] = fp_acosh(Stack[SP]); break;

          case  cAsin:
#                    ifndef FP_NO_EVALUATION_CHECKS
                       if(Stack[SP] < -1 || Stack[SP] > 1)
                       { evalErrorType=4; return 0; }
#                    endif
                       Stack[SP] = asin(Stack[SP]); break;

          case cAsinh: Stack[SP] = fp_asinh(Stack[SP]); break;

          case  cAtan: Stack[SP] = atan(Stack[SP]); break;

          case cAtan2: Stack[SP-1] = atan2(Stack[SP-1], Stack[SP]);
                       --SP; break;

          case cAtanh: Stack[SP] = fp_atanh(Stack[SP]); break;

          case  cCeil: Stack[SP] = ceil(Stack[SP]); break;

          case   cCos: Stack[SP] = cos(Stack[SP]); break;

          case  cCosh: Stack[SP] = cosh(Stack[SP]); break;

          case   cCot:
              {
                  const double t = tan(Stack[SP]);
#               ifndef FP_NO_EVALUATION_CHECKS
                  if(t == 0) { evalErrorType=1; return 0; }
#               endif
                  Stack[SP] = 1/t; break;
              }

          case   cCsc:
              {
                  const double s = sin(Stack[SP]);
#               ifndef FP_NO_EVALUATION_CHECKS
                  if(s == 0) { evalErrorType=1; return 0; }
#               endif
                  Stack[SP] = 1/s; break;
              }


#       ifndef FP_DISABLE_EVAL
          case  cEval:
              {
                  const unsigned varAmount =
                      unsigned(data->variableRefs.size());
                  double retVal = 0;
                  if(evalRecursionLevel == FP_EVAL_MAX_REC_LEVEL)
                  {
                      evalErrorType = 5;
                  }
                  else
                  {
                      ++evalRecursionLevel;
#                   ifndef FP_USE_THREAD_SAFE_EVAL
                      std::vector<double> tmpStack(Stack.size());
                      data->Stack.swap(tmpStack);
                      retVal = Eval(&tmpStack[SP - varAmount + 1]);
                      data->Stack.swap(tmpStack);
#                   else
                      retVal = Eval(&Stack[SP - varAmount + 1]);
#                   endif
                      --evalRecursionLevel;
                  }
                  SP -= varAmount-1;
                  Stack[SP] = retVal;
                  break;
              }
#       endif

          case   cExp: Stack[SP] = exp(Stack[SP]); break;

          case   cExp2:
            //#ifdef FP_SUPPORT_EXP2
            //  Stack[SP] = exp2(Stack[SP]);
            //#else
              Stack[SP] = fp_pow(2.0, Stack[SP]);
            //#endif
              break;

          case cFloor: Stack[SP] = floor(Stack[SP]); break;

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

          case   cInt: Stack[SP] = floor(Stack[SP]+.5); break;

          case   cLog:
#                    ifndef FP_NO_EVALUATION_CHECKS
                       if(Stack[SP] <= 0) { evalErrorType=3; return 0; }
#                    endif
                       Stack[SP] = log(Stack[SP]); break;

          case cLog10:
#                    ifndef FP_NO_EVALUATION_CHECKS
                       if(Stack[SP] <= 0) { evalErrorType=3; return 0; }
#                    endif
                       //Stack[SP] = log10(Stack[SP]);
                       Stack[SP] = log(Stack[SP]) * 0.43429448190325176116;
                       break;

          case  cLog2:
#                    ifndef FP_NO_EVALUATION_CHECKS
                       if(Stack[SP] <= 0) { evalErrorType=3; return 0; }
#                    endif
                     #ifdef FP_SUPPORT_LOG2
                       Stack[SP] = log2(Stack[SP]);
                     #else
                       Stack[SP] = log(Stack[SP]) * 1.4426950408889634074;
                     #endif
                       break;

          case   cMax: Stack[SP-1] = Max(Stack[SP-1], Stack[SP]);
                       --SP; break;

          case   cMin: Stack[SP-1] = Min(Stack[SP-1], Stack[SP]);
                       --SP; break;

          case   cPow: Stack[SP-1] = fp_pow(Stack[SP-1], Stack[SP]);
                       --SP; break;
          case   cRPow: Stack[SP-1] = fp_pow(Stack[SP], Stack[SP-1]);
                        --SP; break;

          case  cTrunc: Stack[SP] = fp_trunc(Stack[SP]); break;

          case   cSec:
              {
                  const double c = cos(Stack[SP]);
#               ifndef FP_NO_EVALUATION_CHECKS
                  if(c == 0) { evalErrorType=1; return 0; }
#               endif
                  Stack[SP] = 1/c; break;
              }

          case   cSin: Stack[SP] = sin(Stack[SP]); break;

          case  cSinh: Stack[SP] = sinh(Stack[SP]); break;

          case  cSqrt:
#                    ifndef FP_NO_EVALUATION_CHECKS
                       if(Stack[SP] < 0) { evalErrorType=2; return 0; }
#                    endif
                       Stack[SP] = sqrt(Stack[SP]); break;

          case   cTan: Stack[SP] = tan(Stack[SP]); break;

          case  cTanh: Stack[SP] = tanh(Stack[SP]); break;


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
#                    ifndef FP_NO_EVALUATION_CHECKS
                       if(Stack[SP] == 0) { evalErrorType=1; return 0; }
#                    endif
                       Stack[SP-1] /= Stack[SP]; --SP; break;

          case   cMod:
#                    ifndef FP_NO_EVALUATION_CHECKS
                       if(Stack[SP] == 0) { evalErrorType=1; return 0; }
#                    endif
                       Stack[SP-1] = fmod(Stack[SP-1], Stack[SP]);
                       --SP; break;

#ifdef FP_EPSILON
          case cEqual: Stack[SP-1] =
                           (fabs(Stack[SP-1]-Stack[SP]) <= FP_EPSILON);
                       --SP; break;

          case cNEqual: Stack[SP-1] =
                            (fabs(Stack[SP-1] - Stack[SP]) >= FP_EPSILON);
                       --SP; break;

          case  cLess: Stack[SP-1] = (Stack[SP-1] < Stack[SP]-FP_EPSILON);
                       --SP; break;

          case  cLessOrEq: Stack[SP-1] = (Stack[SP-1] <= Stack[SP]+FP_EPSILON);
                       --SP; break;

          case cGreater: Stack[SP-1] = (Stack[SP-1]-FP_EPSILON > Stack[SP]);
                         --SP; break;

          case cGreaterOrEq: Stack[SP-1] =
                                 (Stack[SP-1]+FP_EPSILON >= Stack[SP]);
                         --SP; break;
#else
          case cEqual: Stack[SP-1] = (Stack[SP-1] == Stack[SP]);
                       --SP; break;

          case cNEqual: Stack[SP-1] = (Stack[SP-1] != Stack[SP]);
                       --SP; break;

          case  cLess: Stack[SP-1] = (Stack[SP-1] < Stack[SP]);
                       --SP; break;

          case  cLessOrEq: Stack[SP-1] = (Stack[SP-1] <= Stack[SP]);
                       --SP; break;

          case cGreater: Stack[SP-1] = (Stack[SP-1] > Stack[SP]);
                         --SP; break;

          case cGreaterOrEq: Stack[SP-1] = (Stack[SP-1] >= Stack[SP]);
                         --SP; break;
#endif

          case   cNot: Stack[SP] = !truthValue(Stack[SP]); break;

          case   cAnd: Stack[SP-1] =
                           (truthValue(Stack[SP-1]) &&
                            truthValue(Stack[SP  ]));
                       --SP; break;

          case    cOr: Stack[SP-1] =
                           (truthValue(Stack[SP-1]) ||
                            truthValue(Stack[SP  ]));
                       --SP; break;

          case cNotNot: Stack[SP] = truthValue(Stack[SP]); break;

// Degrees-radians conversion:
          case   cDeg: Stack[SP] = RadiansToDegrees(Stack[SP]); break;
          case   cRad: Stack[SP] = DegreesToRadians(Stack[SP]); break;

// User-defined function calls:
          case cFCall:
              {
                  unsigned index = ByteCode[++IP];
                  unsigned params = data->FuncPtrs[index].params;
                  double retVal =
                      data->FuncPtrs[index].funcPtr(&Stack[SP-params+1]);
                  SP -= int(params)-1;
                  Stack[SP] = retVal;
                  break;
              }

          case cPCall:
              {
                  unsigned index = ByteCode[++IP];
                  unsigned params = data->FuncParsers[index].params;
                  double retVal =
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
#                    ifndef FP_NO_EVALUATION_CHECKS
                       if(Stack[SP-1] <= 0) { evalErrorType=3; return 0; }
#                    endif
                     #ifdef FP_SUPPORT_LOG2
                       Stack[SP-1] = log2(Stack[SP-1]) * Stack[SP];
                     #else
                       Stack[SP-1] = log(Stack[SP-1]) * Stack[SP] * 1.4426950408889634074;
                     #endif
                       --SP;
                       break;
#endif // FP_SUPPORT_OPTIMIZER
          case   cAbsNot:  Stack[SP] = !truthValue_abs(Stack[SP]); break;
          case cAbsNotNot: Stack[SP] =  truthValue_abs(Stack[SP]); break;
          case cAbsAnd:  Stack[SP-1] = truthValue_abs(Stack[SP-1])
                                    && truthValue_abs(Stack[SP]);
                         --SP; break;
          case cAbsOr:  Stack[SP-1] = truthValue_abs(Stack[SP-1])
                                   || truthValue_abs(Stack[SP]);
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
              if(Stack[SP] == 0.0) { evalErrorType=1; return 0; }
#           endif
              Stack[SP] = 1.0/Stack[SP];
              break;

          case   cSqr:
              Stack[SP] = Stack[SP]*Stack[SP];
              break;

          case   cRDiv:
#                    ifndef FP_NO_EVALUATION_CHECKS
                        if(Stack[SP-1] == 0) { evalErrorType=1; return 0; }
#                    endif
                        Stack[SP-1] = Stack[SP] / Stack[SP-1]; --SP; break;

          case   cRSub: Stack[SP-1] = Stack[SP] - Stack[SP-1]; --SP; break;

          case   cRSqrt:
#                      ifndef FP_NO_EVALUATION_CHECKS
                         if(Stack[SP] == 0) { evalErrorType=1; return 0; }
#                      endif
                         Stack[SP] = 1.0 / sqrt(Stack[SP]); break;

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
    int deduceVariables(FunctionParser& fParser,
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

            const char* endPtr = readIdentifier(funcStr + index);
            if(endPtr == funcStr + index) return index;

            varNames.insert(std::string(funcStr + index, endPtr));
            oldIndex = index;
        }

        if(amountOfVariablesFound)
            *amountOfVariablesFound = int(varNames.size());

        if(destVarNames)
            destVarNames->assign(varNames.begin(), varNames.end());

        return -1;
    }
}

int FunctionParser::ParseAndDeduceVariables
(const std::string& function,
 int* amountOfVariablesFound,
 bool useDegrees)
{
    std::string varString;
    return deduceVariables(*this, function.c_str(), varString,
                           amountOfVariablesFound, 0, useDegrees);
}

int FunctionParser::ParseAndDeduceVariables
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

int FunctionParser::ParseAndDeduceVariables
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

    typedef std::map<FUNCTIONPARSERTYPES::NamePtr, unsigned> VariablesMap;
    std::string findVariableName(const VariablesMap& varMap, unsigned index)
    {
        for(VariablesMap::const_iterator iter = varMap.begin();
            iter != varMap.end();
            ++iter)
        {
            if(iter->second == index)
                return std::string(iter->first.name,
                                   iter->first.name + iter->first.nameLength);
        }
        return "?";
    }

    typedef std::vector<double> FactorStack;

    const struct PowiMuliType
    {
        unsigned opcode_square;
        unsigned opcode_cumulate;
        unsigned opcode_invert;
        unsigned opcode_half;
        unsigned opcode_invhalf;
    } iseq_powi = {cSqr,cMul,cInv,cSqrt,cRSqrt},
      iseq_muli = {~unsigned(0), cAdd,cNeg, ~unsigned(0),~unsigned(0) };

    double ParsePowiMuli(
        const PowiMuliType& opcodes,
        const std::vector<unsigned>& ByteCode, unsigned& IP,
        unsigned limit,
        size_t factor_stack_base,
        FactorStack& stack)
    {
        double result = 1.0;
        while(IP < limit)
        {
            if(ByteCode[IP] == opcodes.opcode_square)
            {
                if(!IsIntegerConst(result)) break;
                result *= 2;
                ++IP;
                continue;
            }
            if(ByteCode[IP] == opcodes.opcode_invert)
            {
                if(result < 0) break;
                result = -result;
                ++IP;
                continue;
            }
            if(ByteCode[IP] == opcodes.opcode_half)
            {
                if(IsIntegerConst(result) && result > 0 &&
                   ((long)result) % 2 == 0)
                    break;
                if(IsIntegerConst(result * 0.5)) break;
                result *= 0.5;
                ++IP;
                continue;
            }
            if(ByteCode[IP] == opcodes.opcode_invhalf)
            {
                if(IsIntegerConst(result) && result > 0 &&
                   ((long)result) % 2 == 0)
                    break;
                if(IsIntegerConst(result * -0.5)) break;
                result *= -0.5;
                ++IP;
                continue;
            }

            unsigned dup_fetch_pos = IP;
            double lhs = 1.0;

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
                double subexponent = ParsePowiMuli
                    (opcodes,
                     ByteCode, IP, limit,
                     factor_stack_base, stack);
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

    double ParsePowiSequence(const std::vector<unsigned>& ByteCode,
                             unsigned& IP, unsigned limit,
                             size_t factor_stack_base)
    {
        FactorStack stack;
        stack.push_back(1.0);
        return ParsePowiMuli(iseq_powi, ByteCode, IP, limit,
                             factor_stack_base, stack);
    }

    double ParseMuliSequence(const std::vector<unsigned>& ByteCode,
                             unsigned& IP, unsigned limit,
                             size_t factor_stack_base)
    {
        FactorStack stack;
        stack.push_back(1.0);
        return ParsePowiMuli(iseq_muli, ByteCode, IP, limit,
                             factor_stack_base, stack);
    }

    struct IfInfo
    {
        std::pair<int,std::string> condition;
        std::pair<int,std::string> thenbranch;
        unsigned endif_location;

        IfInfo() : condition(), thenbranch(), endif_location() { }
    };
}

void FunctionParser::PrintByteCode(std::ostream& dest,
                                   bool showExpression) const
{
    dest << "Size of stack: " << data->StackSize << "\n";

    std::ostringstream outputBuffer;
    std::ostream& output = (showExpression ? outputBuffer : dest);

    const std::vector<unsigned>& ByteCode = data->ByteCode;
    const std::vector<double>& Immed = data->Immed;

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
                double exponent =
                    ParsePowiSequence(ByteCode, changed_ip,
                                      if_stack.empty()
                                      ? (unsigned)ByteCode.size()
                                      : if_stack.back().endif_location,
                                      stack.size()-1);
                std::ostringstream operation;
                int prio = 0;
                if(exponent == 1.0)
                {
                    if(opcode != cDup) goto not_powi_or_muli;
                    double factor =
                        ParseMuliSequence(ByteCode, changed_ip,
                                          if_stack.empty()
                                          ? (unsigned)ByteCode.size()
                                          : if_stack.back().endif_location,
                                          stack.size()-1);
                    if(factor == 1.0 || factor == -1.0) goto not_powi_or_muli;
                    operation << '*' << factor;
                    prio = 3;
                }
                else
                {
                    prio = 2;
                    operation << '^' << exponent;
                }

                unsigned explanation_before = changed_ip-2;
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
                        std::string& last = stack.back().second;
                        if(stack.back().first >= prio)
                            last = "(" + last + ")";
                        last += operation.str();
                        output << last;
                        stack.back().first = prio;
                    }
                    dest << outputBuffer.str() << std::endl;
                    outputBuffer.str("");
                }
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
                      std::set<NameData>::const_iterator iter =
                          data->nameData.begin();
                      while(iter->type != NameData::FUNC_PTR ||
                            iter->index != index)
                          ++iter;
                      output << "fcall " << iter->name;
                      out_params = true;
                      break;
                  }

              case cPCall:
                  {
                      const unsigned index = ByteCode[++IP];
                      params = data->FuncParsers[index].params;
                      std::set<NameData>::const_iterator iter =
                          data->nameData.begin();
                      while(iter->type != NameData::PARSER_PTR ||
                            iter->index != index)
                          ++iter;
                      output << "pcall " << iter->name;
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
                        case cRPow: n = "rpow"; break;
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
                        case cEval: n = "eval"; params = (unsigned)data->variableRefs.size(); break;
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
                              (findVariableName(data->variableRefs, opcode))));
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
void FunctionParser::Optimize()
{
    // Do nothing if no optimizations are supported.
}
#endif
