//===============================
// Function parser v2.84 by Warp
//===============================

#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"
using namespace FUNCTIONPARSERTYPES;

#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cmath>

using namespace std;

#ifdef FP_USE_THREAD_SAFE_EVAL_WITH_ALLOCA
#ifndef FP_USE_THREAD_SAFE_EVAL
#define FP_USE_THREAD_SAFE_EVAL
#endif
#endif

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

/* Require parentheses in function calls? (If not, "sin x" works.) */
#define REQUIRE_FCALL_PARENS

/*!re2c
re2c:yyfill:enable = 0;
digit = [0-9];
digitseq = digit+;
integer   = digitseq;
pminteger = [-+]?digitseq;
utf8four  = [\360-\367][\220-\277][\200-\277][\200-\277];
utf8three = [\340-\357][\240-\277][\200-\277];
utf8two   = [\302-\337][\200-\277];
asciichar = [A-Za-z_];
alpha = asciichar|utf8two|utf8three|utf8four;
zzalpha = [a-zA-Z];
space = [ \b\t\v\f\r\n];
anychar = [\000-\377];
exp = [eE]pminteger;
decimpart = [.]digitseq;
numconst = integer? decimpart exp?
         | integer exp?;
identifier = alpha(alpha|digit)*;
sws = space+;
andop = [&][&]?;
orop = [|][|]?;
*/

namespace
{
    const unsigned FUNC_AMOUNT = sizeof(Functions)/sizeof(Functions[0]);


    // BCB4 does not implement the standard lower_bound function.
    // This is used instead:
    const FuncDefinition* fp_lower_bound(const FuncDefinition* first,
                                         const FuncDefinition* last,
                                         const FuncDefinition& value)
    {
        while(first < last)
        {
            const FuncDefinition* middle = first+(last-first)/2;
            if(*middle < value) first = middle+1;
            else last = middle;
        }
        return last;
    }


    // Returns a pointer to the FuncDefinition instance which 'name' is
    // the same as the one given by 'F'. If no such function name exists,
    // returns 0.
    inline const FuncDefinition* FindFunction(const char* F)
    {
        FuncDefinition func = { F, 0, 0, 0 };
        while(isalnum(F[func.nameLength])) ++func.nameLength;
        if(func.nameLength)
        {
            const FuncDefinition* found =
                fp_lower_bound(Functions, Functions+FUNC_AMOUNT, func);
            if(found == Functions+FUNC_AMOUNT || func < *found)
                return 0;
            return found;
        }
        return 0;
    }
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


//---------------------------------------------------------------------------
// Constructors and destructors
//---------------------------------------------------------------------------
//===========================================================================
FunctionParser::FunctionParser():
    parseErrorType(FP_NO_ERROR), evalErrorType(0),
    data(new Data),
    evalRecursionLevel(0)
{
    data->referenceCounter = 1;
}

FunctionParser::~FunctionParser()
{
    if(--(data->referenceCounter) == 0)
    {
        delete data;
    }
}

FunctionParser::FunctionParser(const FunctionParser& cpy):
    parseErrorType(cpy.parseErrorType),
    evalErrorType(cpy.evalErrorType),
    data(cpy.data),
    evalRecursionLevel(0)
{
    ++(data->referenceCounter);
}

FunctionParser& FunctionParser::operator=(const FunctionParser& cpy)
{
    if(data != cpy.data)
    {
        if(--(data->referenceCounter) == 0) delete data;

        parseErrorType = cpy.parseErrorType;
        evalErrorType = cpy.evalErrorType;
        data = cpy.data;
        evalRecursionLevel = cpy.evalRecursionLevel;

        ++(data->referenceCounter);
    }

    return *this;
}

void FunctionParser::ForceDeepCopy()
{
    CopyOnWrite();
}

FunctionParser::Data::Data():
    useDegreeConversion(false),
    isOptimized(false),
    ByteCode(0), ByteCodeSize(0),
    Immed(0), ImmedSize(0),
    Stack(0), StackSize(0)
{}

FunctionParser::Data::~Data()
{
    if(ByteCode) { delete[] ByteCode; ByteCode=0; }
    if(Immed) { delete[] Immed; Immed=0; }
    if(Stack) { delete[] Stack; Stack=0; }
}

// Makes a deep-copy of Data:
FunctionParser::Data::Data(const Data& cpy):
    varAmount(cpy.varAmount), useDegreeConversion(cpy.useDegreeConversion),
    Variables(cpy.Variables), Constants(cpy.Constants), Units(cpy.Units),
    FuncPtrNames(cpy.FuncPtrNames), FuncPtrs(cpy.FuncPtrs),
    FuncParserNames(cpy.FuncParserNames), FuncParsers(cpy.FuncParsers),
    ByteCode(0), ByteCodeSize(cpy.ByteCodeSize),
    Immed(0), ImmedSize(cpy.ImmedSize),
    Stack(0), StackSize(cpy.StackSize)
{
    if(ByteCodeSize) ByteCode = new unsigned[ByteCodeSize];
    if(ImmedSize) Immed = new double[ImmedSize];
    if(StackSize) Stack = new double[StackSize];

    std::copy(cpy.ByteCode, cpy.ByteCode+ByteCodeSize, ByteCode);
    std::copy(cpy.Immed, cpy.Immed+ImmedSize, Immed);

    // No need to copy the stack contents because it's obsolete outside Eval()
}


//---------------------------------------------------------------------------
// Function parsing
//---------------------------------------------------------------------------
//===========================================================================
namespace
{
    // Error messages returned by ErrorMsg():
    const char* ParseErrorMessage[]=
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
        ""
    };


    enum VarNameType
    { NOT_VALID_NAME, VALID_NEW_NAME, RESERVED_FUNCTION,
      USER_DEF_CONST, USER_DEF_UNIT,
      USER_DEF_FUNC_PTR, USER_DEF_FUNC_PARSER
    };

    // Parse variables
    bool ParseVars(const string& Vars, map<string, unsigned>& dest)
    {
        #define YYFILL(n) { }
        #define YYDEBUG(n,c) { printf("state %d, char '%c'\n", n,c); }
        typedef unsigned char YYCTYPE;
        const YYCTYPE* YYCURSOR = (const YYCTYPE*) Vars.c_str();
        const YYCTYPE* YYLIMIT  = YYCURSOR + Vars.size();
        const YYCTYPE* YYMARKER = 0;

        size_t varNumber = VarBegin;
        int CommaState = 0; // 0 = don't expect, 1 = expect, -1 = got

ContParse:
        const YYCTYPE* anchor = YYCURSOR;
        if(anchor >= YYLIMIT) goto DoneParse;
/*!re2c
sws { goto ContParse; }
identifier {
    if(CommaState == 1) return false; CommaState = 1;
    const std::string varName(anchor, YYCURSOR);
    if(dest.insert(make_pair(varName, varNumber++)).second == false)
        { return false; }
    goto ContParse;
}
"," { if(CommaState != 1) return false; CommaState = -1; goto ContParse; }
anychar { return false; }
*/
        #undef YYFILL
        #undef YYDEBUG
DoneParse:
        if(CommaState == -1) return false; // spurious trailing comma
	return true;
    }
}

namespace
{
    bool varNameHasOnlyValidChars(const std::string& name)
    {
        #define YYFILL(n) { }
        #define YYDEBUG(n,c) { printf("state %d, char '%c'\n", n,c); }
        typedef unsigned char YYCTYPE;
        const YYCTYPE* YYCURSOR = (const YYCTYPE*) name.c_str();
        //const YYCTYPE* YYLIMIT  = YYCURSOR + name.size();
        const YYCTYPE* YYMARKER = 0;
/*!re2c
identifier {
        if(*YYCURSOR) return false;
        if(FindFunction(name.c_str())) return false;
        return true;
}
anychar { return false; }
*/
        return false;

        #undef YYFILL
        #undef YYDEBUG
    }
}

int FunctionParser::VarNameType(const std::string& name) const
{
    if(!varNameHasOnlyValidChars(name)) return NOT_VALID_NAME;

    if(FindFunction(name.c_str())) return RESERVED_FUNCTION;
    if(data->Constants.find(name) != data->Constants.end())
        return USER_DEF_CONST;

    // Units are independent from the rest and thus the following check
    // is actually not needed:
    //if(data->Units.find(name) != data->Units.end())
    //    return USER_DEF_UNIT;

    if(data->FuncParserNames.find(name) != data->FuncParserNames.end())
        return USER_DEF_FUNC_PARSER;
    if(data->FuncPtrNames.find(name) != data->FuncPtrNames.end())
        return USER_DEF_FUNC_PTR;

    return VALID_NEW_NAME;
}


// Constants:
bool FunctionParser::AddConstant(const string& name, double value)
{
    int nameType = VarNameType(name);
    if(nameType == VALID_NEW_NAME || nameType == USER_DEF_CONST)
    {
        CopyOnWrite();
        data->Constants[name] = value;
        return true;
    }
    return false;
}

// Units:
bool FunctionParser::AddUnit(const std::string& name, double value)
{
    if(!varNameHasOnlyValidChars(name)) return false;
    CopyOnWrite();
    data->Units[name] = value;
    return true;
}

// Function pointers
bool FunctionParser::AddFunction(const std::string& name,
                                 FunctionPtr func, unsigned paramsAmount)
{
    int nameType = VarNameType(name);
    if(nameType == VALID_NEW_NAME)
    {
        CopyOnWrite();
        data->FuncPtrNames[name] = data->FuncPtrs.size();
        data->FuncPtrs.push_back(Data::FuncPtrData(func, paramsAmount));
        return true;
    }
    return false;
}

bool FunctionParser::CheckRecursiveLinking(const FunctionParser* fp) const
{
    if(fp == this) return true;
    for(unsigned i=0; i<fp->data->FuncParsers.size(); ++i)
        if(CheckRecursiveLinking(fp->data->FuncParsers[i])) return true;
    return false;
}

bool FunctionParser::AddFunction(const std::string& name,
                                 FunctionParser& parser)
{
    if(varNameHasOnlyValidChars(name))
    {
        if(data->FuncPtrNames.find(name) != data->FuncPtrNames.end()
        || data->Constants.find(name) != data->Constants.end())
            return false;

        if(CheckRecursiveLinking(&parser)) return false;

        CopyOnWrite();

        data->FuncParserNames[name] = data->FuncParsers.size();
        data->FuncParsers.push_back(&parser);
        return true;
    }
    return false;
}

struct PrioItem
{
    int prio;
    int opcode;
    int flags;
    int nparams;

    /* For cIf */
    unsigned ByteCodeSize1, ByteCodeSize2, ImmedSize2;

    PrioItem(int p,int o,int f,int n) : prio(p),opcode(o),flags(f),nparams(n) { }
};


// Main parsing function
// ---------------------
int FunctionParser::Parse(const std::string& Function,
                          const std::string& Vars,
                          bool useDegrees)
{
    CopyOnWrite();

    data->isOptimized = false;
    data->Variables.clear();

    if(!ParseVars(Vars, data->Variables))
    {
        parseErrorType = INVALID_VARS;
        return Function.size();
    }
    data->varAmount = data->Variables.size(); // this is for Eval()

    parseErrorType = FP_NO_ERROR;

    data->useDegreeConversion = useDegrees;

    if(data->ByteCode) { delete[] data->ByteCode; data->ByteCode=0; }
    if(data->Immed) { delete[] data->Immed; data->Immed=0; }
    if(data->Stack) { delete[] data->Stack; data->Stack=0; }

    std::vector<unsigned> byteCode; byteCode.reserve(1024);
    tempByteCode = &byteCode;

    std::vector<double> immed; immed.reserve(1024);
    tempImmed = &immed;

    data->StackSize = StackPtr = 0;

    #define YYFILL(n) { }
    #define YYDEBUG(n,c) { printf("state %d, char '%c'\n", n,c); }
    typedef unsigned char YYCTYPE;
    const YYCTYPE* YYCURSOR = (const YYCTYPE*) Function.c_str();
    const YYCTYPE* YYLIMIT  = YYCURSOR + Function.size();
    const YYCTYPE* YYMARKER = 0;

    std::vector<PrioItem> PrioStack;
    PrioStack.reserve(Function.size()/3); /* heuristic guess */

/*

Priority levels:

10 func
9  units
8  ^        (note: right-associative)
7  ! -    (UNARY)
6  * / %
5  + -    (BINARY)
4  = < > !=
3  &
2  |
1  ,

*/

    int CurPrio = 0;
    bool had_op = false;
ContParse:
    const YYCTYPE* anchor = YYCURSOR;
    if(anchor >= YYLIMIT) goto DoneParse;
ContParseWithSameAnchor:
    /*printf("ContParse: anchor=%s\n", anchor);*/

#define PRIO_ENTER_GEN(newprio, op, flags, nparams) \
    do { \
        /*printf("PrioEnter from %d to %d, op=%d(%s), flags=%u\n", \
            CurPrio, newprio,op,#op,flags);*/ \
        PrioStack.push_back(PrioItem(CurPrio,op,flags,nparams)); \
        CurPrio=(newprio); \
        had_op = false; \
    } while(0)

#define PRIO_ENTER_NOP(n,np)            PRIO_ENTER_GEN(n,-1,0,np)
#define PRIO_ENTER_OP(n,op,np)          PRIO_ENTER_GEN(n,op,0,np)
#define PRIO_ENTER_FLAGS(n,op,flags,np) PRIO_ENTER_GEN(n,op,flags,np)

#define PRIO_RETURN() \
    do { \
        if(PrioStack.empty()) \
        { \
            parseErrorType = UNEXPECTED_ERROR; \
            return (anchor - (const YYCTYPE*) Function.c_str()); \
        } \
        const PrioItem& item = PrioStack.back(); \
        /*printf("PrioReturn from %d to %d, op=%d, flags=%u\n", \
            CurPrio, item.prio,item.opcode,item.flags);*/ \
        CurPrio = item.prio; \
        switch(item.opcode) \
        { \
            case cIf: \
                (*tempByteCode)[item.ByteCodeSize1]   = item.ByteCodeSize2+1; \
                (*tempByteCode)[item.ByteCodeSize1+1] = item.ImmedSize2; \
                (*tempByteCode)[item.ByteCodeSize2]   = tempByteCode->size()-1; \
                (*tempByteCode)[item.ByteCodeSize2+1] = tempImmed->size(); \
                break; \
            case cFCall: case cPCall: \
                AddCompiledByte(item.opcode); \
                AddCompiledByte(item.flags); \
                break; \
            default: \
                if(item.opcode >= 0) \
                { \
                    AddFunctionOpcode(item.opcode); \
                } \
        } \
        if(item.opcode >= 0) \
        { \
            StackPtr -= item.nparams; \
            incStackPtr(); \
        } \
        had_op = true; \
        PrioStack.pop_back(); \
    } while(0)

/* left-associative x-nary op (nparams=x) */
#define NARY_OP_LEFT(prio, op, fl, nparams) \
    do { while(CurPrio >= prio) PRIO_RETURN(); \
         PRIO_ENTER_FLAGS(prio, op, fl, nparams); } while(0)

/* right-associative x-nary op (nparams=x) */
#define NARY_OP_RIGHT(prio, op, fl, nparams) \
    do { while(CurPrio > prio) PRIO_RETURN(); \
         PRIO_ENTER_FLAGS(prio, op, fl, nparams); } while(0)

#define BINARY_OP_LEFT(prio,op,fl) NARY_OP_LEFT(prio,op,fl,2)
#define BINARY_OP_RIGHT(prio,op,fl) NARY_OP_RIGHT(prio,op,fl,2)
#define UNARY_OP_LEFT(prio,op,fl) NARY_OP_LEFT(prio,op,fl,1)

#ifdef FP_NO_ASINH
 #define ASINH_ENABLE(x) goto GotIdentifier
#else
 #define ASINH_ENABLE(x) x
#endif
#ifdef FP_DISABLE_EVAL
 #define EVAL_ENABLE(x) goto GotIdentifier
#else
 #define EVAL_ENABLE(x) x
#endif


/*!re2c
sws      { goto ContParse; }
"("      {   if(had_op)
             {
                  parseErrorType = SYNTAX_ERROR;
                  //printf("ERROR: SYNTAX ERROR: %s\n", anchor);
                  return (anchor - (const YYCTYPE*) Function.c_str());
             }
             PRIO_ENTER_NOP(0, 0);
             goto ContParse;
         }
")"      {
             while(CurPrio > 0) PRIO_RETURN();
             PRIO_RETURN();
             had_op = true;
             goto ContParse;
         }

","      {
             while(CurPrio >= 1) PRIO_RETURN();
             if(PrioStack.size() >= 2)
             {
                 /* Get information about the caller */
                 PrioItem& caller = PrioStack[PrioStack.size()-2];
                 if(caller.opcode == cIf)
                 {
                     switch(caller.flags)
                     {
                         case 0: /* The first param of the "if", "condition" */
                         {
                             AddCompiledByte(cIf);
                             caller.ByteCodeSize1 = tempByteCode->size();
                             AddCompiledByte(0); // Jump index; to be set later
                             AddCompiledByte(0); // Immed jump index; to be set later
                             caller.flags = 1;
                             break;
                         }
                         case 1: /* The second param of the "if", "positive branch" */
                         {
                             AddCompiledByte(cJump);
                             caller.ByteCodeSize2 = tempByteCode->size();
                             caller.ImmedSize2    = tempImmed->size();
                             AddCompiledByte(0); // Jump index; to be set later
                             AddCompiledByte(0); // Immed jump index; to be set later
                             caller.flags = 2;
                             break;
                         }
                     }
                 }
             }
             NARY_OP_LEFT(1, -1, 0, 0); goto ContParse;
         }

orop     { BINARY_OP_LEFT(2, cOr, 0); goto ContParse; }

andop    { BINARY_OP_LEFT(3, cAnd, 0); goto ContParse; }

"+"      { if(had_op) /* binary? */
             { BINARY_OP_LEFT(4, cAdd, 0); goto ContParse; }
           else /* unary */
             { goto ContParse; }
         }
"-"      { if(had_op) /* binary? */
             { BINARY_OP_LEFT(4, cSub, 0); goto ContParse; }
           else /* unary */
             { /*while(*YYCURSOR == ' ' || *YYCURSOR == '\t'
                  || *YYCURSOR == '\r' || *YYCURSOR == '\n') ++YYCURSOR;
               */if(*YYCURSOR == '.' || (*YYCURSOR >= '0' && *YYCURSOR <= '9'))
                  goto ContParseWithSameAnchor; // negative number
               PRIO_ENTER_OP(7, cNeg, 1);  goto ContParse;
             }
         }
"!"      { /* unary */
           PRIO_ENTER_OP(7, cNot, 1); goto ContParse; }

"="      { BINARY_OP_LEFT(5, cEqual, 0); goto ContParse; }
"!="     { BINARY_OP_LEFT(5, cNEqual, 0); goto ContParse; }
"<"      { BINARY_OP_LEFT(5, cLess, 0); goto ContParse; }
">"      { BINARY_OP_LEFT(5, cGreater, 0); goto ContParse; }
"<="     { BINARY_OP_LEFT(5, cLessOrEq, 0); goto ContParse; }
">="     { BINARY_OP_LEFT(5, cGreaterOrEq, 0); goto ContParse; }

"*"      { BINARY_OP_LEFT(6, cMul, 0); goto ContParse; }
"/"      { BINARY_OP_LEFT(6, cDiv, 0); goto ContParse; }
"%"      { BINARY_OP_LEFT(6, cMod, 0); goto ContParse; }

"^"      { BINARY_OP_RIGHT(8, cPow, 0); goto ContParse; }

"abs"     { UNARY_OP_LEFT(10, cAbs, 0); goto ExpectOpenParensParser; }
"acos"    { UNARY_OP_LEFT(10, cAcos, 0); goto ExpectOpenParensParser; }
"acosh"   { ASINH_ENABLE(
            UNARY_OP_LEFT(10, cAcosh, 0); goto ExpectOpenParensParser); }
"asin"    { UNARY_OP_LEFT(10, cAsin, 0); goto ExpectOpenParensParser; }
"asinh"   { ASINH_ENABLE(
            UNARY_OP_LEFT(10, cAsinh, 0); goto ExpectOpenParensParser); }
"atan"    { UNARY_OP_LEFT(10, cAtan, 0); goto ExpectOpenParensParser; }
"atan2"   { BINARY_OP_LEFT(10, cAtan2, 0); goto ExpectOpenParensParser; }
"atanh"   { ASINH_ENABLE(
            UNARY_OP_LEFT(10, cAtanh, 0); goto ExpectOpenParensParser); }
"ceil"    { UNARY_OP_LEFT(10, cCeil, 0); goto ExpectOpenParensParser; }
"cos"     { UNARY_OP_LEFT(10, cCos, 0); goto ExpectOpenParensParser; }
"cosh"    { UNARY_OP_LEFT(10, cCosh, 0); goto ExpectOpenParensParser; }
"cot"     { UNARY_OP_LEFT(10, cCot, 0); goto ExpectOpenParensParser; }
"csc"     { UNARY_OP_LEFT(10, cCsc, 0); goto ExpectOpenParensParser; }
"eval"    { EVAL_ENABLE(
            NARY_OP_LEFT(10, cEval, 0, data->varAmount); goto ExpectOpenParensParser); }
"exp"     { UNARY_OP_LEFT(10, cExp, 0); goto ExpectOpenParensParser; }
"floor"   { UNARY_OP_LEFT(10, cFloor, 0); goto ExpectOpenParensParser; }
"if"      { NARY_OP_LEFT(10, cIf, 0, 3); goto ExpectOpenParensParser; }
"int"     { UNARY_OP_LEFT(10, cInt, 0); goto ExpectOpenParensParser; }
"log"     { UNARY_OP_LEFT(10, cLog, 0); goto ExpectOpenParensParser; }
"log10"   { UNARY_OP_LEFT(10, cLog10, 0); goto ExpectOpenParensParser; }
"max"     { BINARY_OP_LEFT(10, cMax, 0); goto ExpectOpenParensParser; }
"min"     { BINARY_OP_LEFT(10, cMin, 0); goto ExpectOpenParensParser; }
"sec"     { UNARY_OP_LEFT(10, cSec, 0); goto ExpectOpenParensParser; }
"sin"     { UNARY_OP_LEFT(10, cSin, 0); goto ExpectOpenParensParser; }
"sinh"    { UNARY_OP_LEFT(10, cSinh, 0); goto ExpectOpenParensParser; }
"sqrt"    { UNARY_OP_LEFT(10, cSqrt, 0); goto ExpectOpenParensParser; }
"tan"     { UNARY_OP_LEFT(10, cTan, 0); goto ExpectOpenParensParser; }
"tanh"    { UNARY_OP_LEFT(10, cTanh, 0); goto ExpectOpenParensParser; }

numconst { char* endptr = (char*) YYCURSOR;
           double val = strtod((const char*)anchor, &endptr);
           YYCURSOR = (YYCTYPE*) endptr;
           /*printf("Got imm %g\n", val);*/
           AddImmediate(val);
           AddCompiledByte(cImmed);
           incStackPtr();
           had_op = true;
           goto ContParse; }

identifier {
GotIdentifier:
    const std::string identifier(anchor, YYCURSOR);

    if(had_op)
    {
        Data::ConstMap_t::const_iterator uIter = data->Units.find(identifier);
        if(uIter != data->Units.end()) /* Is a unit */
        {
            BINARY_OP_LEFT(9, cMul, 0);
            AddImmediate(uIter->second);
            AddCompiledByte(cImmed);
            incStackPtr();
            had_op = true;
            goto ContParse;
        }
        parseErrorType = EXPECT_OPERATOR;
        //printf("ERROR: SYNTAX ERROR: %s\n", anchor);
        return (anchor - (const YYCTYPE*) Function.c_str());
    }

    Data::VarMap_t::const_iterator vIter = data->Variables.find(identifier);
    if(vIter != data->Variables.end()) /* Is a variable */
    {
        AddCompiledByte(vIter->second);
        had_op = true;
        incStackPtr();
        goto ContParse;
    }
    Data::ConstMap_t::const_iterator cIter = data->Constants.find(identifier);
    if(cIter != data->Constants.end()) /* Is a constant */
    {
        AddImmediate(cIter->second);
        AddCompiledByte(cImmed);
        incStackPtr();
        had_op = true;
        goto ContParse;
    }
    Data::VarMap_t::const_iterator fIter = data->FuncPtrNames.find(identifier);
    if(fIter != data->FuncPtrNames.end()) /* Is a FCall pointer */
    {
        NARY_OP_LEFT(10, cFCall, fIter->second, data->FuncPtrs[fIter->second].params);
        goto ExpectOpenParensParser;
    }
    Data::VarMap_t::const_iterator pIter = data->FuncParserNames.find(identifier);
    if(pIter != data->FuncParserNames.end()) /* Is a PCall pointer */
    {
        NARY_OP_LEFT(10, cPCall, pIter->second, data->FuncParsers[pIter->second]->data->varAmount);
        goto ExpectOpenParensParser;
    }
    //printf("ERROR: UNKNOWN IDENTIFIER: %s\n", identifier.c_str());
    parseErrorType = INVALID_VARS;
    return (anchor - (const YYCTYPE*) Function.c_str());
}

anychar {
    parseErrorType = SYNTAX_ERROR;
    return (anchor - (const YYCTYPE*) Function.c_str());
}
*/


ExpectOpenParensParser:
#ifndef REQUIRE_FCALL_PARENS
    goto ContParse;
#else
/*!re2c
sws { goto ExpectOpenParensParser; }
"(" { PRIO_ENTER_NOP(0,0); goto ContParse; }
anychar {
    parseErrorType = MISSING_PARENTH;
    return (YYCURSOR - (const YYCTYPE*) Function.c_str());
}
*/
#endif

    #undef YYFILL
    #undef YYDEBUG

DoneParse:
    if(!had_op)
    {
        parseErrorType = PREMATURE_EOS;
        //printf("ERROR: SYNTAX ERROR: %s\n", anchor);
        return (anchor - (const YYCTYPE*) Function.c_str());
    }
    while(!PrioStack.empty())
    {
    	if(CurPrio == 0)
    	{
    	    parseErrorType = MISSING_PARENTH;
    	    return (anchor - (const YYCTYPE*) Function.c_str());
    	}
    	PRIO_RETURN();
    }
    //std::cout << "Stack ptr: " << StackPtr << std::endl;
    if(StackPtr != 1)
    {
        parseErrorType = StackPtr > 1 ? PREMATURE_EOS : ILL_PARAMS_AMOUNT;
        //printf("ERROR: SYNTAX ERROR: %s\n", anchor);
        return (anchor - (const YYCTYPE*) Function.c_str());
    }

    data->Variables.clear();

    data->ByteCodeSize = byteCode.size();
    data->ImmedSize = immed.size();

    if(data->ByteCodeSize)
    {
        data->ByteCode = new unsigned[data->ByteCodeSize];
        memcpy(data->ByteCode, &byteCode[0],
               sizeof(unsigned)*data->ByteCodeSize);
    }
    if(data->ImmedSize)
    {
        data->Immed = new double[data->ImmedSize];
        memcpy(data->Immed, &immed[0],
               sizeof(double)*data->ImmedSize);
    }
#ifndef FP_USE_THREAD_SAFE_EVAL
    if(data->StackSize)
        data->Stack = new double[data->StackSize];
#endif

    parseErrorType = FP_NO_ERROR;
    /*PrintByteCode(std::cout);*/
    return -1;
}


//---------------------------------------------------------------------------
inline void FunctionParser::AddCompiledByte(unsigned c)
{
    tempByteCode->push_back(c);
}

inline void FunctionParser::AddImmediate(double i)
{
    tempImmed->push_back(i);
}

inline void FunctionParser::AddFunctionOpcode(unsigned opcode)
{
    switch(opcode)
    {
        case cCos:
        case cCosh:
        case cCot:
        case cCsc:
        case cSec:
        case cSin:
        case cSinh:
        case cTan:
        case cTanh:
            if(data->useDegreeConversion)
                AddCompiledByte(cRad);
            AddCompiledByte(opcode);
            return;

        case cAcos:
#ifndef FP_NO_ASINH
        case cAcosh:
        case cAsinh:
        case cAtanh:
#endif
        case cAsin:
        case cAtan:
        case cAtan2:
            AddCompiledByte(opcode);
            if(data->useDegreeConversion)
                AddCompiledByte(cDeg);
            return;

        case cNeg:
            /* Shouldn't these go to the optimizer? -Bisqwit */
            if(tempByteCode->back() == cImmed)
            {
            	tempImmed->back() = -tempImmed->back();
            	return;
            }
            if(tempByteCode->back() == cNeg)
            {
            	tempByteCode->pop_back();
            	return;
            }
            AddCompiledByte(opcode);
            return;

        default:
            AddCompiledByte(opcode);
    }
}

inline void FunctionParser::incStackPtr()
{
    if(++StackPtr > data->StackSize) ++(data->StackSize);
}


// Return parse error message
// --------------------------
const char* FunctionParser::ErrorMsg() const
{
    if(parseErrorType != FP_NO_ERROR) return ParseErrorMessage[parseErrorType];
    return 0;
}

//---------------------------------------------------------------------------
// Function evaluation
//---------------------------------------------------------------------------
//===========================================================================
namespace
{
    inline int doubleToInt(double d)
    {
        return d<0 ? -int((-d)+.5) : int(d+.5);
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
}

double FunctionParser::Eval(const double* Vars)
{
    const unsigned* const ByteCode = data->ByteCode;
    const double* const Immed = data->Immed;
    const unsigned ByteCodeSize = data->ByteCodeSize;
    unsigned IP, DP=0;
    int SP=-1;

#ifdef FP_USE_THREAD_SAFE_EVAL
#ifdef FP_USE_THREAD_SAFE_EVAL_WITH_ALLOCA
    double* const Stack = (double*)alloca(data->StackSize*sizeof(double));
#else
    std::vector<double> Stack(data->StackSize);
#endif
#else
    double* const Stack = data->Stack;
#endif

    for(IP=0; IP<ByteCodeSize; ++IP)
    {
        switch(ByteCode[IP])
        {
// Functions:
          case   cAbs: Stack[SP] = fabs(Stack[SP]); break;
          case  cAcos: if(Stack[SP] < -1 || Stack[SP] > 1)
                       { evalErrorType=4; return 0; }
                       Stack[SP] = acos(Stack[SP]); break;
#ifndef FP_NO_ASINH
          case cAcosh: Stack[SP] = acosh(Stack[SP]); break;
#endif
          case  cAsin: if(Stack[SP] < -1 || Stack[SP] > 1)
                       { evalErrorType=4; return 0; }
                       Stack[SP] = asin(Stack[SP]); break;
#ifndef FP_NO_ASINH
          case cAsinh: Stack[SP] = asinh(Stack[SP]); break;
#endif
          case  cAtan: Stack[SP] = atan(Stack[SP]); break;
          case cAtan2: Stack[SP-1] = atan2(Stack[SP-1], Stack[SP]);
                       --SP; break;
#ifndef FP_NO_ASINH
          case cAtanh: Stack[SP] = atanh(Stack[SP]); break;
#endif
          case  cCeil: Stack[SP] = ceil(Stack[SP]); break;
          case   cCos: Stack[SP] = cos(Stack[SP]); break;
          case  cCosh: Stack[SP] = cosh(Stack[SP]); break;

          case   cCot:
              {
                  double t = tan(Stack[SP]);
                  if(t == 0) { evalErrorType=1; return 0; }
                  Stack[SP] = 1/t; break;
              }
          case   cCsc:
              {
                  double s = sin(Stack[SP]);
                  if(s == 0) { evalErrorType=1; return 0; }
                  Stack[SP] = 1/s; break;
              }


#ifndef FP_DISABLE_EVAL
          case  cEval:
              {
                  double retVal = 0;
                  if(evalRecursionLevel == FP_EVAL_MAX_REC_LEVEL)
                  {
                      evalErrorType = 5;
                  }
                  else
                  {
#ifndef FP_USE_THREAD_SAFE_EVAL
                      data->Stack = new double[data->StackSize];
#endif
                      ++evalRecursionLevel;
                      retVal = Eval(&Stack[SP-data->varAmount+1]);
                      --evalRecursionLevel;
#ifndef FP_USE_THREAD_SAFE_EVAL
                      delete[] data->Stack;
                      data->Stack = Stack;
#endif
                  }
                  SP -= data->varAmount-1;
                  Stack[SP] = retVal;
                  break;
              }
#endif

          case   cExp: Stack[SP] = exp(Stack[SP]); break;
          case cFloor: Stack[SP] = floor(Stack[SP]); break;

          case    cIf:
              {
                  unsigned jumpAddr = ByteCode[++IP];
                  unsigned immedAddr = ByteCode[++IP];
                  if(doubleToInt(Stack[SP]) == 0)
                  {
                      IP = jumpAddr;
                      DP = immedAddr;
                  }
                  --SP; break;
              }

          case   cInt: Stack[SP] = floor(Stack[SP]+.5); break;
          case   cLog: if(Stack[SP] <= 0) { evalErrorType=3; return 0; }
                       Stack[SP] = log(Stack[SP]); break;
          case cLog10: if(Stack[SP] <= 0) { evalErrorType=3; return 0; }
                       Stack[SP] = log10(Stack[SP]); break;
          case   cMax: Stack[SP-1] = Max(Stack[SP-1], Stack[SP]);
                       --SP; break;
          case   cMin: Stack[SP-1] = Min(Stack[SP-1], Stack[SP]);
                       --SP; break;
          case   cSec:
              {
                  double c = cos(Stack[SP]);
                  if(c == 0) { evalErrorType=1; return 0; }
                  Stack[SP] = 1/c; break;
              }
          case   cSin: Stack[SP] = sin(Stack[SP]); break;
          case  cSinh: Stack[SP] = sinh(Stack[SP]); break;
          case  cSqrt: if(Stack[SP] < 0) { evalErrorType=2; return 0; }
                       Stack[SP] = sqrt(Stack[SP]); break;
          case   cTan: Stack[SP] = tan(Stack[SP]); break;
          case  cTanh: Stack[SP] = tanh(Stack[SP]); break;


// Misc:
          case cImmed: Stack[++SP] = Immed[DP++]; break;
          case  cJump: DP = ByteCode[IP+2];
                       IP = ByteCode[IP+1];
                       break;

// Operators:
          case   cNeg: Stack[SP] = -Stack[SP]; break;
          case   cAdd: Stack[SP-1] += Stack[SP]; --SP; break;
          case   cSub: Stack[SP-1] -= Stack[SP]; --SP; break;
          case   cMul: Stack[SP-1] *= Stack[SP]; --SP; break;
          case   cDiv: if(Stack[SP] == 0) { evalErrorType=1; return 0; }
                       Stack[SP-1] /= Stack[SP]; --SP; break;
          case   cMod: if(Stack[SP] == 0) { evalErrorType=1; return 0; }
                       Stack[SP-1] = fmod(Stack[SP-1], Stack[SP]);
                       --SP; break;
          case   cPow: Stack[SP-1] = pow(Stack[SP-1], Stack[SP]);
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

          case   cAnd: Stack[SP-1] =
                           (doubleToInt(Stack[SP-1]) &&
                            doubleToInt(Stack[SP]));
                       --SP; break;
          case    cOr: Stack[SP-1] =
                           (doubleToInt(Stack[SP-1]) ||
                            doubleToInt(Stack[SP]));
                       --SP; break;
          case   cNot: Stack[SP] = !doubleToInt(Stack[SP]); break;

// Degrees-radians conversion:
          case   cDeg: Stack[SP] = RadiansToDegrees(Stack[SP]); break;
          case   cRad: Stack[SP] = DegreesToRadians(Stack[SP]); break;

// User-defined function calls:
          case cFCall:
              {
                  unsigned index = ByteCode[++IP];
                  unsigned params = data->FuncPtrs[index].params;
                  double retVal =
                      data->FuncPtrs[index].ptr(&Stack[SP-params+1]);
                  SP -= int(params)-1;
                  Stack[SP] = retVal;
                  break;
              }

          case cPCall:
              {
                  unsigned index = ByteCode[++IP];
                  unsigned params = data->FuncParsers[index]->data->varAmount;
                  double retVal =
                      data->FuncParsers[index]->Eval(&Stack[SP-params+1]);
                  SP -= int(params)-1;
                  Stack[SP] = retVal;
                  const int error = data->FuncParsers[index]->EvalError();
                  if(error)
                  {
                      evalErrorType = error;
                      return 0;
                  }
                  break;
              }


#ifdef FP_SUPPORT_OPTIMIZER
          case   cVar: break; // Paranoia. This should never exist
          case   cDup: Stack[SP+1] = Stack[SP]; ++SP; break;
          case   cInv:
              if(Stack[SP] == 0.0) { evalErrorType=1; return 0; }
              Stack[SP] = 1.0/Stack[SP];
              break;
#endif

// Variables:
          default:
              Stack[++SP] = Vars[ByteCode[IP]-VarBegin];
        }
    }

    evalErrorType=0;
    return Stack[SP];
}


#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT

#if 0
#include <list>

void FunctionParser::PrintByteCode(std::ostream& dest) const
{
    std::list<unsigned> labels;

    for(unsigned IP=0, DP=0; ; ++IP)
    {
        dest << ' ';
        //dest << "[IP=" << IP << "]";

        while(labels.size() > 0
        && *labels.begin() == IP)
        {
            // end branch started with {
            dest << "} ";
            labels.erase(labels.begin());
        }

        if(IP == data->ByteCodeSize) break;

        unsigned opcode = data->ByteCode[IP];

        if(opcode == cIf)
        {
            dest << '{'; // start branch, end with }
            IP += 2;
        }
        else if(opcode == cImmed)
        {
            dest.precision(20);
            dest << data->Immed[DP];
            ++DP;
        }
        else if(opcode == cJump)
        {
            labels.push_front(data->ByteCode[IP+1]+1);
            dest << "} {";
            IP += 2;
        }
        else
        {
            /* Returns the textual description of the specified opcode.
             * Note: opcode should never be cImmed, cJump or cVar.
             */
            if(opcode >= VarBegin)
            {
                unsigned ind = opcode;
        #ifdef REMEMBER_VARNAMES
                Data::VarMap_t::const_iterator i;
                for(i=data->Variables.begin(); i!=data->Variables.end(); ++i)
                    if(i->second == ind)
                        return i->first;
        #endif
                dest << "Var" << (ind-VarBegin);
            }
            else
            switch( (OPCODE) opcode)
            {
            #define c(o,v) case o: dest << v; break;
            #define t(n)   c(n, #n); break;
                t(cImmed) t(cJump) t(cNeg)
                c(cAdd,"+") c(cSub,"-") c(cMul,"*") c(cDiv,"/")
                c(cMod,"%") c(cPow,"^") c(cEqual,"=") c(cLess,"<")
                c(cGreater,">") c(cNEqual,"!=") c(cLessOrEq,"<=")
                c(cGreaterOrEq,">=")
                t(cAnd) t(cOr) t(cDeg) t(cRad) t(cNot)
              #ifdef FP_SUPPORT_OPTIMIZER
                t(cVar) t(cDup) t(cInv)
              #endif
                t(cFCall) t(cPCall)

            default:
            	dest << Functions[opcode-cAbs].name << " ("
            	     << Functions[opcode-cAbs].params << ")"; break;
            }
        }
    }
    dest << "  (length: " << data->ByteCodeSize << ")";
    dest << "\n";
}

#else

#include <iomanip>
namespace
{
    inline void printHex(std::ostream& dest, unsigned n)
    {
        dest.width(8); dest.fill('0'); std::hex(dest); //uppercase(dest);
        dest << n;
    }
}

void FunctionParser::PrintByteCode(std::ostream& dest) const
{
    const unsigned* const ByteCode = data->ByteCode;
    const double* const Immed = data->Immed;

    dest << "(Stack size: " << data->StackSize << ")\n";

    for(unsigned IP=0, DP=0; IP<data->ByteCodeSize; ++IP)
    {
        printHex(dest, IP);
        dest << ": ";

        unsigned opcode = ByteCode[IP];

        switch(opcode)
        {
          case cIf:
              dest << "jz\t";
              printHex(dest, ByteCode[IP+1]+1);
              dest << endl;
              IP += 2;
              break;

          case cJump:
              dest << "jump\t";
              printHex(dest, ByteCode[IP+1]+1);
              dest << endl;
              IP += 2;
              break;
          case cImmed:
              dest.precision(10);
              dest << "push\t" << Immed[DP++] << endl;
              break;
          case cFCall:
              {
                  unsigned index = ByteCode[++IP];
                  Data::VarMap_t::const_iterator iter =
                      data->FuncPtrNames.begin();
                  while(iter->second != index) ++iter;
                  dest << "fcall\t" << iter->first
                       << " (" << data->FuncPtrs[index].params << ")" << endl;
                  break;
              }

          case cPCall:
              {
                  unsigned index = ByteCode[++IP];
                  Data::VarMap_t::const_iterator iter =
                      data->FuncParserNames.begin();
                  while(iter->second != index) ++iter;
                  dest << "pcall\t" << iter->first
                       << " (" << data->FuncParsers[index]->data->varAmount
                       << ")" << endl;
                  break;
              }

          default:
              if(opcode < VarBegin)
              {
                  string n;
                  unsigned params = 1;
                  switch(opcode)
                  {
                    case cNeg: n = "neg"; break;
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
                    case cNot: n = "not"; break;
                    case cDeg: n = "deg"; break;
                    case cRad: n = "rad"; break;

#ifndef FP_DISABLE_EVAL
                    case cEval: n = "call\t0"; break;
#endif

#ifdef FP_SUPPORT_OPTIMIZER
                    case cVar: n = "(var)"; break;
                    case cDup: n = "dup"; break;
                    case cInv: n = "inv"; break;
#endif

                    default:
                        n = Functions[opcode-cAbs].name;
                        params = Functions[opcode-cAbs].params;
                  }
                  dest << n;
                  if(params != 1) dest << " (" << params << ")";
                  dest << endl;
              }
              else
              {
                  dest << "push\tVar" << opcode-VarBegin << endl;
              }
        }
    }
}
#endif
#endif


#ifndef FP_SUPPORT_OPTIMIZER
void FunctionParser::MakeTree(void *) const {}
void FunctionParser::Optimize()
{
    // Do nothing if no optimizations are supported.
}
#endif
