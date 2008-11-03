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

// Main parsing function
// ---------------------
namespace
{
    struct ShiftedState
    {
        std::string ident;
        double num;
        int opcode;
        int state;
    };
}
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

    /* Parser defs */
    /* Note: The specific order of Terminals affects the numbering
     * used in Actions[] in BisonState. So don't change too hastily.
     */
<<PARSING_DEFS_PLACEHOLDER>>

    /* Parser state */
    int CurrentState=0;
    std::vector<ShiftedState> ShiftedStates;

    /* Lexer information */
    double      LastNum=0;
    std::string LastIdentifier;
    int         LastOpcode=0;
    Terminals   LastTerminal;

    fprintf(stderr, "Parsing %s\n", YYCURSOR);

    /* Parser & lexer */
ContParse:
    const YYCTYPE* anchor = YYCURSOR;
    if(anchor >= YYLIMIT) goto DoneParse;
//ContParseWithSameAnchor:
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

#define DO_TERM(t) LastTerminal=t; goto GotTerminal
#define DO_OP_TERM(o, t) LastOpcode=o; DO_TERM(t)

/*!re2c
sws      { goto ContParse; }
"("      {  DO_TERM(T_LParens);  }
")"      {  DO_TERM(T_RParens);  }
","      {  DO_TERM(T_Comma);    }
orop     {  DO_OP_TERM(cOr, T_OrOp);     }
andop    {  DO_OP_TERM(cAnd, T_AndOp);    }
"+"      {  DO_OP_TERM(cAdd, T_Plus);     }
"-"      {  DO_OP_TERM(cSub, T_Minus);    }
"!"      {  DO_OP_TERM(cNot, T_Bang);     }
"="      {  DO_OP_TERM(cEqual, T_CompOp); }
"!="     {  DO_OP_TERM(cNEqual, T_CompOp); }
"<"      {  DO_OP_TERM(cLess, T_CompOp); }
">"      {  DO_OP_TERM(cGreater, T_CompOp); }
"<="     {  DO_OP_TERM(cLessOrEq, T_CompOp); }
">="     {  DO_OP_TERM(cGreaterOrEq, T_CompOp); }

"*"      {  DO_OP_TERM(cMul, T_TimesMulModOp);  }
"/"      {  DO_OP_TERM(cDiv, T_TimesMulModOp);  }
"%"      {  DO_OP_TERM(cMod, T_TimesMulModOp);  }

"^"      {  DO_OP_TERM(cPow, T_Pow);     }

"abs"     { DO_OP_TERM(cAbs, T_PreFunc1);  }
"acos"    { DO_OP_TERM(cAcos, T_PreFunc1);  }
"acosh"   { ASINH_ENABLE( DO_OP_TERM(cAcosh, T_PreFunc1) ); }
"asin"    { DO_OP_TERM(cAsin, T_PreFunc1);  }
"asinh"   { ASINH_ENABLE( DO_OP_TERM(cAsinh, T_PreFunc1) ); }
"atan"    { DO_OP_TERM(cAtan, T_PreFunc1);  }
"atan2"   { DO_OP_TERM(cAtan2, T_PreFunc2);  }
"atanh"   { ASINH_ENABLE( DO_OP_TERM(cAtanh, T_PreFunc1) ); }
"ceil"    { DO_OP_TERM(cCeil, T_PreFunc1); }
"cos"     { DO_OP_TERM(cCos, T_PreFunc1); }
"cosh"    { DO_OP_TERM(cCosh, T_PreFunc1); }
"cot"     { DO_OP_TERM(cCot, T_PreFunc1); }
"csc"     { DO_OP_TERM(cCsc, T_PreFunc1); }
"eval"    { EVAL_ENABLE( DO_TERM(T_Eval) ); }
"exp"     { DO_OP_TERM(cExp, T_PreFunc1); }
"floor"   { DO_OP_TERM(cFloor, T_PreFunc1); }
"if"      { DO_TERM(T_If); }
"int"     { DO_OP_TERM(cInt, T_PreFunc1); }
"log"     { DO_OP_TERM(cLog, T_PreFunc1); }
"log10"   { DO_OP_TERM(cLog10, T_PreFunc1); }
"max"     { DO_OP_TERM(cMax, T_PreFunc2); }
"min"     { DO_OP_TERM(cMin, T_PreFunc2); }
"sec"     { DO_OP_TERM(cSec, T_PreFunc1); }
"sin"     { DO_OP_TERM(cSin, T_PreFunc1); }
"sinh"    { DO_OP_TERM(cSinh, T_PreFunc1); }
"sqrt"    { DO_OP_TERM(cSqrt, T_PreFunc1); }
"tan"     { DO_OP_TERM(cTan, T_PreFunc1); }
"tanh"    { DO_OP_TERM(cTanh, T_PreFunc1); }

numconst {
    char* endptr = (char*) YYCURSOR;
    LastNum = strtod((const char*)anchor, &endptr);
    YYCURSOR = (YYCTYPE*) endptr;
    //printf("Got imm %g\n", val);
    DO_TERM(T_NumConst); }
identifier {
GotIdentifier:
    LastIdentifier.assign(anchor, YYCURSOR);
    DO_TERM(T_Identifier);
}

anychar {
    DO_TERM(T_Garbage);
}
*/

    #undef YYFILL
    #undef YYDEBUG
DoneParse:
    DO_TERM(T_Zend);
    /* Note: There's theoretically a possibility for an
     * infinite loop here:
     *    ContParse->DoneParse->GotTerminal->ContParse
     *
     * However, now really. The only situation where
     * GotTerminal may return to ContParse is if it
     * does a SHIFT(), and there's no grammar rule
     * that causes a SHIF to be done for T_Zend.
     */
    static const bool DoDebug = false;

GotTerminal:
    #define CREATE_SHIFT(t) \
        t.ident  = LastIdentifier; \
        t.num    = LastNum; \
        t.opcode = LastOpcode; \
        t.state  = CurrentState

    #define SHIFT(newstate) do { \
        ShiftedState t; \
        CREATE_SHIFT(t); \
        if(DoDebug)fprintf(stderr, "- Pushing opcode %d num %g id %s\n", \
            t.opcode, t.num, t.ident.c_str()); \
        ShiftedStates.push_back(t); \
        CurrentState = newstate; \
    } while(0)

    #define GET_PARAM(offs) \
        const ShiftedState& param##offs = \
            ShiftedStates[ShiftedStates.size() - n_reduce + offs]

    /* Do the parsing decision */
    const BisonState& State = States[CurrentState];
    int Action = State.Actions[LastTerminal];

    if(DoDebug)fprintf(stderr, "State %d, LastTerminal=%s, Action=%d\n",
        CurrentState, TerminalNames[LastTerminal], Action);

    if(Action == 127)
    {
        goto ReallyDoneParse; // Accept the result
    }
    else if(Action > 0)
    {
        /* Shift */
        SHIFT(Action);
        goto ContParse;
    }
    else if(Action < 0)
    {
        /* Reduce using rule (-Action) */

        ShiftedState SaveTerminalBeforeReduce;
        CREATE_SHIFT(SaveTerminalBeforeReduce);

        LastIdentifier.clear();

        // Reduce using which rule?
        // These rule numbers are directly derived from the
        // fparser-parsingtree.output file, and must be changed
        // any time you add/remove/reorder clauses in the fparser.y file.
        const int n_reduce                      = BisonGrammar[-Action].n_reduce;
        const NonTerminals produced_nonterminal = BisonGrammar[-Action].produced_nonterminal;

        switch(-Action)
        {
            case   1: //exp: NUMCONST
            {
                GET_PARAM(0);
                AddImmediate(param0.num);
                AddCompiledByte(cImmed);
                incStackPtr();
                break;
            }
            case   2: //   | PreFunc1 func_params_list_opt
            case   3: //   | PreFunc2 func_params_list_opt
            case   4: //   | PreFunc3 func_params_list_opt
            {
                GET_PARAM(0); // ident
                GET_PARAM(1); // func_params_list_opt
                int n_params_expected = 0;
                if(Action==-2) n_params_expected=1;
                if(Action==-3) n_params_expected=2;
                if(Action==-4) n_params_expected=3;
                if(DoDebug)fprintf(stderr, "Expects %d params, gets %d\n",
                    n_params_expected, param1.opcode);
                if(param1.opcode != n_params_expected)
                {
                    parseErrorType = ILL_PARAMS_AMOUNT;
                    //printf("ERROR: SYNTAX ERROR: %s\n", anchor);
                    return (anchor - (const YYCTYPE*) Function.c_str());
                }
                AddFunctionOpcode(param0.opcode);
                StackPtr -= param1.opcode; incStackPtr();
                break;
            }
            case   5: //   | EVAL func_params_list_opt
            {
                GET_PARAM(1); // func_params_list_opt
                int n_params_expected = data->varAmount;
                if(param1.opcode != n_params_expected)
                {
                    parseErrorType = ILL_PARAMS_AMOUNT;
                    //printf("ERROR: SYNTAX ERROR: %s\n", anchor);
                    return (anchor - (const YYCTYPE*) Function.c_str());
                }
                AddCompiledByte(cEval);
                StackPtr -= param1.opcode; incStackPtr();
                break;
            }
            case   6: // (after first exp in if)
            {
                AddCompiledByte(cIf);
                size_t ByteCodeSize1 = tempByteCode->size();
                AddCompiledByte(0); // Jump index; to be set later
                AddCompiledByte(0); // Immed jump index; to be set later
                LastOpcode = ByteCodeSize1;
                break;
            }
            case   7: // (after second exp in if)
            {
                AddCompiledByte(cJump);
                size_t ByteCodeSize2 = tempByteCode->size();
                size_t ImmedSize2    = tempImmed->size();
                AddCompiledByte(0); // Jump index; to be set later
                AddCompiledByte(0); // Immed jump index; to be set later
                LastOpcode = ByteCodeSize2;
                LastNum    = ImmedSize2; // FIXME: possibly loses precision
                break;
            }
            case   8: //exp: If LParens exp @1 Comma exp @2 Comma exp RParens
            {
                GET_PARAM(3); // NT_A1
                GET_PARAM(6); // NT_A2
                size_t ByteCodeSize1 = param3.opcode;
                size_t ByteCodeSize2 = param6.opcode;
                size_t ImmedSize2    = param6.num;

                (*tempByteCode)[ByteCodeSize1]   = ByteCodeSize2+1;
                (*tempByteCode)[ByteCodeSize1+1] = ImmedSize2;
                (*tempByteCode)[ByteCodeSize2]   = tempByteCode->size()-1;
                (*tempByteCode)[ByteCodeSize2+1] = tempImmed->size();
                --StackPtr;
                break;
            }
            case   9: //   | Identifier func_params_list_opt
            {
                GET_PARAM(0); // ident
                GET_PARAM(1); // func_params_list_opt
                int n_params_expected = 0, opcode = 0, funcno = 0;

                Data::VarMap_t::const_iterator fIter = data->FuncPtrNames.find(param0.ident);
                if(fIter != data->FuncPtrNames.end()) /* Is a FCall pointer */
                {
                    n_params_expected = data->FuncPtrs[fIter->second].params;
                    opcode            = cFCall;
                    funcno            = fIter->second;
                }
                else
                {
                    Data::VarMap_t::const_iterator pIter = data->FuncParserNames.find(param0.ident);
                    if(pIter != data->FuncParserNames.end()) /* Is a PCall pointer */
                    {
                        opcode        = cPCall;
                        funcno        = pIter->second;
                        n_params_expected = data->FuncParsers[pIter->second]->data->varAmount;
                    }
                    else
                    {
                        parseErrorType = INVALID_VARS;
                        return (anchor - (const YYCTYPE*) Function.c_str());
                    }
                }

                if(n_params_expected != param1.opcode)
                {
                    parseErrorType = ILL_PARAMS_AMOUNT;
                    //printf("ERROR: SYNTAX ERROR: %s\n", anchor);
                    return (anchor - (const YYCTYPE*) Function.c_str());
                }

                AddCompiledByte(opcode);
                AddCompiledByte(funcno);
                StackPtr -= param1.opcode; incStackPtr();
                break;
            }
            case  10: //   | IDENTIFIER
            {
                GET_PARAM(0); //ident
                Data::VarMap_t::const_iterator vIter = data->Variables.find(param0.ident);
                if(vIter != data->Variables.end()) /* Is a variable */
                    AddCompiledByte(vIter->second);
                else
                {
                    Data::ConstMap_t::const_iterator cIter = data->Constants.find(param0.ident);
                    if(cIter != data->Constants.end()) /* Is a constant */
                    {
                        AddImmediate(cIter->second);
                        AddCompiledByte(cImmed);
                    }
                    else
                    {
                        parseErrorType = INVALID_VARS;
                        return (anchor - (const YYCTYPE*) Function.c_str());
                    }
                }
                incStackPtr();
                break;
            }
            case  11: //   | exp '+' exp
            case  12: //   | exp '-' exp
            case  13: //   | exp '&' exp
            case  14: //   | exp '|' exp
            case  15: //   | exp COMP_OP exp
            case  16: //   | exp TIMES_MUL_MOD_OP exp
            case  17: //   | exp '^' exp
            {
                GET_PARAM(1);
                AddCompiledByte( param1.opcode );
                StackPtr -= 2; incStackPtr();
                break;
            }
            case  18: //   | '!' exp
                AddCompiledByte(cNot);
                break;
            case  19: //   | '-' exp
                AddCompiledByte(cNeg);
                /* Note: Double negation / negation of immeds
                 * is dealt with in AddCompiledByte()
                 */
                break;
            case  20: //   | '(' exp ')'
                /* nothing to do */
                break;

            case  21: //exp: exp unit_name
            {
                GET_PARAM(1); // ident

                Data::ConstMap_t::const_iterator uIter = data->Units.find(param1.ident);
                if(uIter == data->Units.end()) // Is not a unit?
                {
                    parseErrorType = EXPECT_OPERATOR;
                    //printf("ERROR: SYNTAX ERROR: %s\n", anchor);
                    return (anchor - (const YYCTYPE*) Function.c_str());
                }
                AddImmediate(uIter->second);
                AddCompiledByte(cImmed);
                incStackPtr();
                AddCompiledByte(cMul);
                --StackPtr;
                break;
            }

            case 22: // func_params_list_opt: LParens func_params_opt RParens
            {
                GET_PARAM(1);// func_params_opt
                LastOpcode = param1.opcode; // denote the number of params
                break;
            }

            case 23: // func_params_opt: func_params
            {
                GET_PARAM(0);// func_params
                LastOpcode = param0.opcode; // denote the number of params
                break;
            }
            case 24: // func_params_opt: <empty>
                LastOpcode = 0; // denote 0 params
                break;

            case 25: // func_params: func_params Comma exp
            {
                GET_PARAM(0);// func_params
                LastOpcode = param0.opcode + 1; // denote 1 param more
                break;
            }
            case 26: // func_params: exp
                LastOpcode = 1; // denote 1 param
                break;
            case 27: // unit_name: Identifier
            {
                GET_PARAM(0);// Identifier
                LastIdentifier = param0.ident;
                break;
            }

            default:
                parseErrorType = UNEXPECTED_ERROR; return 0; // shouldn't happen
        }

        if(DoDebug)fprintf(stderr, "Reduced using rule %d, produced %s - eating %d\n",
            -Action, NonTerminalNames[produced_nonterminal], n_reduce);

        if(n_reduce > 0)
        {
            /* Load up the state where we reduced to */
            CurrentState = ShiftedStates[ShiftedStates.size()-n_reduce].state;
            ShiftedStates.resize(ShiftedStates.size()-n_reduce);
        }

        /* Check out what is the new state to which
         * we should go after performing the reduce
         */
        const BisonState& PoppedState = States[CurrentState];
        int NewState = PoppedState.Goto[produced_nonterminal];
        if(!NewState)
        {
            // No state? Shouldn't happen. This indicates broken bison data.
            fprintf(stderr, "No CurrentState to go to in state %d?\n", NewState);
            parseErrorType = UNEXPECTED_ERROR; return 0;
        }
        SHIFT(NewState);

        /* Restore the terminal that was before reduce */
        LastOpcode     = SaveTerminalBeforeReduce.opcode;
        LastNum        = SaveTerminalBeforeReduce.num;
        LastIdentifier = SaveTerminalBeforeReduce.ident;

        /* And go deal with it in the new state */
        goto GotTerminal;
    }
    else
    {
        /* The parse tree indicates that the given token
         * should not occur here. Generate a parse error.
         */
        /* Decide what kind of parse error to indicate.
         */
        parseErrorType = SYNTAX_ERROR;

        std::string errormessage = "Parse error - expected", delim = " ";
        if(State.Actions[T_Zend] != 0)
            { errormessage += delim + "EOS"; delim = " or "; }
        if(State.Actions[T_OrOp] != 0)
            { errormessage += delim + "operator"; delim = " or ";
              if(LastTerminal == T_RParens
              || LastTerminal == T_LParens) parseErrorType = MISM_PARENTH;
            }
        if(State.Actions[T_RParens] != 0)
            { errormessage += delim + "')'"; delim = " or ";
              parseErrorType = MISSING_PARENTH; }
        if(State.Goto[NT_unit_name] != 0)
            { errormessage += delim + "unit"; delim = " or "; }

        if(State.Goto[NT_exp] != 0)
            { errormessage += delim + "expression"; delim = " or "; }
        else if(State.Actions[T_LParens] != 0)
            { errormessage += delim + "'('"; delim = " or ";
              parseErrorType = EXPECT_PARENTH_FUNC; }
        else if(State.Actions[T_Comma] != 0)
            { errormessage += delim + "','"; delim = " or "; }

        fprintf(stderr, "%s\n> %s\n> %*s^\n",
            errormessage.c_str(),
            Function.c_str(),
            int(anchor - (const YYCTYPE*) Function.c_str()), "");

        return (anchor - (const YYCTYPE*) Function.c_str());
    }
    /* Not reached */
    goto ContParse;


ReallyDoneParse:
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
        {
            /* Shouldn't these go to the optimizer? -Bisqwit */
            unsigned LastOpcode = GetLastCompiledOpcode();
            if(LastOpcode == cImmed)
            {
                // negate the last immed and return
            	tempImmed->back() = -tempImmed->back();
            	return;
            }
            if(LastOpcode == cNeg)
            {
            	tempByteCode->pop_back(); // remove duplicate cNeg
            	// and don't add a new one
            	return;
            }
            AddCompiledByte(opcode);
            return;
        }
        default:
            AddCompiledByte(opcode);
    }
}
unsigned FunctionParser::GetLastCompiledOpcode() const
{
    if(tempByteCode->size() <= 0)
        return 0;
    if(tempByteCode->size() <= 1)
        return tempByteCode->back();

    const unsigned& last1 = tempByteCode->back();
    // Check the preceding opcode to avoid a false positive
    // when a cFCall is followed by a procedure index that
    // happens to be identical with the value of cImmed.
    const unsigned& last2 = (*tempByteCode)[tempByteCode->size()-2];
    if(last2 == cFCall || last2 == cPCall)
    {
        // FIXME: Still has a possibility for false positive,
        // if last_3_ is a cFCall or cPCall...
        return last2;
    }
    return last1;
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
