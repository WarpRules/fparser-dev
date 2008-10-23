//===============================
// Function parser v2.4 by Warp
//===============================

// Comment out the following line if your compiler supports the (non-standard)
// asinh, acosh and atanh functions and you want them to be supported. If
// you are not sure, just leave it (those function will then not be supported).
#define NO_ASINH


// Uncomment the following line to disable the eval() function if it could
// be too dangerous in the target application:
//#define DISABLE_EVAL


// Comment this line out if you are not going to use the optimizer and want
// a slightly smaller library. The Optimize() method can still be called,
// but it will not do anything.
// If you are unsure, just leave it. It won't slow down the other parts of
// the library.
#define SUPPORT_OPTIMIZER


//============================================================================

#include "fparser.hh"

#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cmath>
#include <new>
#include <algorithm>

using namespace std;

namespace
{
// The functions must be in alphabetical order:
    enum OPCODE
    {
        cAbs, cAcos,
#ifndef NO_ASINH
        cAcosh,
#endif
        cAsin,
#ifndef NO_ASINH
        cAsinh,
#endif
        cAtan,
        cAtan2,
#ifndef NO_ASINH
        cAtanh,
#endif
        cCeil, cCos, cCosh,
#ifndef DISABLE_EVAL
        cEval,
#endif
        cExp, cFloor, cIf, cInt, cLog, cLog10, cMax, cMin,
        cSin, cSinh, cSqrt, cTan, cTanh,

// These do not need any ordering:
        cImmed, cJump,
        cNeg, cAdd, cSub, cMul, cDiv, cMod, cPow,
        cEqual, cLess, cGreater, cAnd, cOr,

#ifdef SUPPORT_OPTIMIZER
        cNop, cVar,
        cDup, cInv,
#endif

        VarBegin
    };

    struct FuncDefinition
    {
        const char* name;
        unsigned nameLength;
        unsigned opcode;
        unsigned params;

        // This is basically strcmp(), but taking 'nameLength' as string
        // length (not ending '\0'):
        bool operator<(const FuncDefinition& rhs) const
        {
            for(unsigned i = 0; i < nameLength; ++i)
            {
                if(i == rhs.nameLength) return false;
                const char c1 = name[i], c2 = rhs.name[i];
                if(c1 < c2) return true;
                if(c2 < c1) return false;
            }
            return nameLength < rhs.nameLength;
        }
    };


// This list must be in alphabetical order:
    const FuncDefinition Functions[]=
    {
        { "abs", 3, cAbs, 1 },
        { "acos", 4, cAcos, 1 },
#ifndef NO_ASINH
        { "acosh", 5, cAcosh, 1 },
#endif
        { "asin", 4, cAsin, 1 },
#ifndef NO_ASINH
        { "asinh", 5, cAsinh, 1 },
#endif
        { "atan", 4, cAtan, 1 },
        { "atan2", 5, cAtan2, 2 },
#ifndef NO_ASINH
        { "atanh", 5, cAtanh, 1 },
#endif
        { "ceil", 4, cCeil, 1 },
        { "cos", 3, cCos, 1 },
        { "cosh", 4, cCosh, 1 },
#ifndef DISABLE_EVAL
        { "eval", 4, cEval, 0 },
#endif
        { "exp", 3, cExp, 1 },
        { "floor", 5, cFloor, 1 },
        { "if", 2, cIf, 0 },
        { "int", 3, cInt, 1 },
        { "log", 3, cLog, 1 },
        { "log10", 5, cLog10, 1 },
        { "max", 3, cMax, 2 },
        { "min", 3, cMin, 2 },
        { "sin", 3, cSin, 1 },
        { "sinh", 4, cSinh, 1 },
        { "sqrt", 4, cSqrt, 1 },
        { "tan", 3, cTan, 1 },
        { "tanh", 4, cTanh, 1 }
    };

    const unsigned FUNC_AMOUNT = sizeof(Functions)/sizeof(Functions[0]);


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
                lower_bound(Functions, Functions+FUNC_AMOUNT, func);
            if(found == Functions+FUNC_AMOUNT || func < *found)
                return 0;
            return found;
        }
        return 0;
    }
};

//---------------------------------------------------------------------------
// Constructors and destructors
//---------------------------------------------------------------------------
//===========================================================================
FunctionParser::FunctionParser():
    ParseErrorType(-1), EvalErrorType(0)
{}

FunctionParser::~FunctionParser()
{}

FunctionParser::CompiledCode::CompiledCode():
    ByteCode(0), ByteCodeSize(0),
    Immed(0), ImmedSize(0),
    Stack(0), StackSize(0)
{}

FunctionParser::CompiledCode::~CompiledCode()
{
    if(ByteCode) { delete[] ByteCode; ByteCode=0; }
    if(Immed) { delete[] Immed; Immed=0; }
    if(Stack) { delete[] Stack; Stack=0; }
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
        "An unexpected error ocurred. Please make a full bug report "
        "to warp@iki.fi",                           // 6
        "Syntax error in parameter 'Vars' given to "
        "FunctionParser::Parse()",                  // 7
        "Illegal number of parameters to function", // 8
        "Syntax error: Premature end of string",    // 9
        "Syntax error: Expecting ( after function", // 10
        ""
    };


    // Parse variables
    bool ParseVars(const string& Vars, map<string, unsigned>& dest)
    {
        unsigned varNumber = VarBegin;
        unsigned ind1 = 0, ind2;

        while(ind1 < Vars.size())
        {
            if(!isalpha(Vars[ind1]) && Vars[ind1]!='_') return false;
            for(ind2=ind1+1; ind2<Vars.size() && Vars[ind2]!=','; ++ind2)
                if(!isalnum(Vars[ind2]) && Vars[ind2]!='_') return false;
            const string varName = Vars.substr(ind1, ind2-ind1);

            if(dest.insert(make_pair(varName, varNumber++)).second == false)
                return false;

            ind1 = ind2+1;
        }
        return true;
    }
};

// Main parsing function
// ---------------------
int FunctionParser::Parse(const std::string& Function,
                          const std::string& Vars)
{
    Variables.clear();

    if(!ParseVars(Vars, Variables))
    {
        ParseErrorType = 7;
        return Function.size();
    }
    varAmount = Variables.size(); // this is for Eval()

    const char* Func = Function.c_str();

    ParseErrorType = -1;

    int Result = CheckSyntax(Func);
    if(Result>=0) return Result;

    if(!Compile(Func)) return Function.size();

    Variables.clear();

    ParseErrorType = -1;
    return -1;
}

namespace
{
    // Is given char an operator?
    inline bool IsOperator(int c)
    {
        return strchr("+-*/%^=<>&|,",c)!=NULL;
    }

    // skip whitespace
    inline void sws(const char* F, int& Ind)
    {
        while(F[Ind] && F[Ind] == ' ') ++Ind;
    }
};

// Returns an iterator to the variable with the same name as 'F', or to
// Variables.end() if no such variable exists:
inline FunctionParser::VarMap_t::const_iterator
FunctionParser::FindVariable(const char* F)
{
    unsigned ind = 0;
    while(isalnum(F[ind]) || F[ind] == '_') ++ind;
    if(ind)
    {
        string name(F, ind);
        return Variables.find(name);
    }
    return Variables.end();
}

//---------------------------------------------------------------------------
// Check function string syntax
// ----------------------------
int FunctionParser::CheckSyntax(const char* Function)
{
    int Ind=0, ParenthCnt=0, c;
    char* Ptr;

    while(true)
    {
        sws(Function, Ind);
        c=Function[Ind];

// Check for valid operand (must appear)

        // Check for leading -
        if(c=='-') { sws(Function, ++Ind); c=Function[Ind]; }
        if(c==0) { ParseErrorType=9; return Ind; }

        // Check for math function
        const FuncDefinition* fptr = FindFunction(&Function[Ind]);
        if(fptr)
        {
            Ind += fptr->nameLength;
            sws(Function, Ind);
            c = Function[Ind];
            if(c!='(') { ParseErrorType=10; return Ind; }
        }

        // Check for opening parenthesis
        if(c=='(')
        {
            ++ParenthCnt;
            sws(Function, ++Ind);
            if(Function[Ind]==')') { ParseErrorType=3; return Ind; }
            continue;
        }

        // Check for number
        if(isdigit(c) || (c=='.' && isdigit(Function[Ind+1])))
        {
            strtod(&Function[Ind], &Ptr);
            Ind += int(Ptr-&Function[Ind]);
            sws(Function, Ind);
            c = Function[Ind];
        }
        else
        { // Check for variable
            VarMap_t::const_iterator vIter = FindVariable(&Function[Ind]);
            if(vIter == Variables.end()) { ParseErrorType=0; return Ind; }
            Ind += vIter->first.size();
            sws(Function, Ind);
            c = Function[Ind];
        }

        // Check for closing parenthesis
        while(c==')')
        {
            if((--ParenthCnt)<0) { ParseErrorType=1; return Ind; }
            sws(Function, ++Ind);
            c=Function[Ind];
        }

// If we get here, we have a legal operand and now a legal operator or
// end of string must follow

        // Check for EOS
        if(c==0) break; // The only way to end the checking loop without error
        // Check for operator
        if(!IsOperator(c)) { ParseErrorType=4; return Ind; }

// If we get here, we have an operand and an operator; the next loop will
// check for another operand (must appear)
        ++Ind;
    } // while

    // Check that all opened parentheses are also closed
    if(ParenthCnt>0) { ParseErrorType=2; return Ind; }

// The string is ok
    ParseErrorType=-1;
    return -1;
}


// Compile function string to bytecode
// -----------------------------------
bool FunctionParser::Compile(const char* Function)
{
    if(Comp.ByteCode) { delete[] Comp.ByteCode; Comp.ByteCode=0; }
    if(Comp.Immed) { delete[] Comp.Immed; Comp.Immed=0; }
    if(Comp.Stack) { delete[] Comp.Stack; Comp.Stack=0; }

    vector<unsigned> byteCode; byteCode.reserve(1024);
    tempByteCode = &byteCode;

    vector<double> immed; immed.reserve(1024);
    tempImmed = &immed;

    Comp.StackSize = Comp.StackPtr = 0;

    CompileExpression(Function, 0);
    if(ParseErrorType >= 0) return false;

    Comp.ByteCodeSize = byteCode.size();
    Comp.ImmedSize = immed.size();

    if(Comp.ByteCodeSize)
    {
        Comp.ByteCode = new unsigned[Comp.ByteCodeSize];
        memcpy(Comp.ByteCode, &byteCode[0],
               sizeof(unsigned)*Comp.ByteCodeSize);
    }
    if(Comp.ImmedSize)
    {
        Comp.Immed = new double[Comp.ImmedSize];
        memcpy(Comp.Immed, &immed[0],
               sizeof(double)*Comp.ImmedSize);
    }
    if(Comp.StackSize)
        Comp.Stack = new double[Comp.StackSize];

    return true;
}


inline void FunctionParser::AddCompiledByte(unsigned c)
{
    tempByteCode->push_back(c);
}

inline void FunctionParser::AddImmediate(double i)
{
    tempImmed->push_back(i);
}

// Compile if()
int FunctionParser::CompileIf(const char* F, int ind)
{
    int ind2 = CompileExpression(F, ind, true); // condition
    sws(F, ind2);
    if(F[ind2] != ',') { ParseErrorType=8; return ind2; }
    AddCompiledByte(cIf);
    unsigned curByteCodeSize = tempByteCode->size();
    AddCompiledByte(0); // Jump index; to be set later
    AddCompiledByte(0); // Immed jump index; to be set later

    --Comp.StackPtr;

    ind2 = CompileExpression(F, ind2+1, true); // then
    sws(F, ind2);
    if(F[ind2] != ',') { ParseErrorType=8; return ind2; }
    AddCompiledByte(cJump);
    unsigned curByteCodeSize2 = tempByteCode->size();
    unsigned curImmedSize2 = tempImmed->size();
    AddCompiledByte(0); // Jump index; to be set later
    AddCompiledByte(0); // Immed jump index; to be set later

    --Comp.StackPtr;

    ind2 = CompileExpression(F, ind2+1, true); // else
    sws(F, ind2);
    if(F[ind2] != ')') { ParseErrorType=8; return ind2; }

    // Set jump indices
    (*tempByteCode)[curByteCodeSize] = curByteCodeSize2+1;
    (*tempByteCode)[curByteCodeSize+1] = curImmedSize2;
    (*tempByteCode)[curByteCodeSize2] = tempByteCode->size()-1;
    (*tempByteCode)[curByteCodeSize2+1] = tempImmed->size();

    return ind2+1;
}

// Compiles element
int FunctionParser::CompileElement(const char* F, int ind)
{
    sws(F, ind);
    char c = F[ind];

    if(c == '(')
    {
        ind = CompileExpression(F, ind+1);
        sws(F, ind);
        return ind+1; // F[ind] is ')'
    }
    else if(c == '-')
    {
        char c2 = F[ind+1];
        if(!isdigit(c2) && c2!='.')
        {
            int ind2 = CompileElement(F, ind+1);
            AddCompiledByte(cNeg);
            return ind2;
        }
    }

    if(isdigit(c) || c=='.' || c=='-') // Number
    {
        const char* startPtr = &F[ind];
        char* endPtr;
        double val = strtod(startPtr, &endPtr);
        AddImmediate(val);
        AddCompiledByte(cImmed);
        ++Comp.StackPtr; if(Comp.StackPtr>Comp.StackSize) Comp.StackSize++;
        return ind+(endPtr-startPtr);
    }

    if(isalpha(c) || c == '_') // Function or variable
    {
        const FuncDefinition* func = FindFunction(F+ind);
        if(func) // is function
        {
            int ind2 = ind + func->nameLength;
            sws(F, ind2); // F[ind2] is '('
            if(strcmp(func->name, "if") == 0) // "if" is a special case
            {
                return CompileIf(F, ind2+1);
            }

            unsigned curStackPtr = Comp.StackPtr;
            ind2 = CompileExpression(F, ind2+1);

#ifndef DISABLE_EVAL
            unsigned requiredParams =
                strcmp(func->name, "eval") == 0 ?
                Variables.size() : func->params;
#else
            unsigned requiredParams = func->params;
#endif
            if(Comp.StackPtr != curStackPtr+requiredParams)
            { ParseErrorType=8; return ind; }

            AddCompiledByte(func->opcode);
            Comp.StackPtr -= func->params - 1;
            sws(F, ind2);
            return ind2+1; // F[ind2] is ')'
        }

        VarMap_t::const_iterator vIter = FindVariable(F+ind);
        if(vIter != Variables.end()) // is variable
        {
            AddCompiledByte(vIter->second);
            ++Comp.StackPtr; if(Comp.StackPtr>Comp.StackSize) Comp.StackSize++;
            return ind + vIter->first.size();
        }
    }

    ParseErrorType = 6;
    return ind;
}

// Compiles '^'
int FunctionParser::CompilePow(const char* F, int ind)
{
    int ind2 = CompileElement(F, ind);
    sws(F, ind2);

    while(F[ind2] == '^')
    {
        ind2 = CompileElement(F, ind2+1);
        sws(F, ind2);
        AddCompiledByte(cPow);
        --Comp.StackPtr;
    }

    return ind2;
}

// Compiles '*', '/' and '%'
int FunctionParser::CompileMult(const char* F, int ind)
{
    int ind2 = CompilePow(F, ind);
    sws(F, ind2);
    char op;

    while((op = F[ind2]) == '*' || op == '/' || op == '%')
    {
        ind2 = CompilePow(F, ind2+1);
        sws(F, ind2);
        switch(op)
        {
          case '*': AddCompiledByte(cMul); break;
          case '/': AddCompiledByte(cDiv); break;
          case '%': AddCompiledByte(cMod); break;
        }
        --Comp.StackPtr;
    }

    return ind2;
}

// Compiles '+' and '-'
int FunctionParser::CompileAddition(const char* F, int ind)
{
    int ind2 = CompileMult(F, ind);
    sws(F, ind2);
    char op;

    while((op = F[ind2]) == '+' || op == '-')
    {
        ind2 = CompileMult(F, ind2+1);
        sws(F, ind2);
        AddCompiledByte(op=='+' ? cAdd : cSub);
        --Comp.StackPtr;
    }

    return ind2;
}

// Compiles '=', '<' and '>'
int FunctionParser::CompileComparison(const char* F, int ind)
{
    int ind2 = CompileAddition(F, ind);
    sws(F, ind2);
    char op;

    while((op = F[ind2]) == '=' || op == '<' || op == '>')
    {
        ind2 = CompileAddition(F, ind2+1);
        sws(F, ind2);
        switch(op)
        {
          case '=': AddCompiledByte(cEqual); break;
          case '<': AddCompiledByte(cLess); break;
          case '>': AddCompiledByte(cGreater); break;
        }
        --Comp.StackPtr;
    }

    return ind2;
}

// Compiles '&'
int FunctionParser::CompileAnd(const char* F, int ind)
{
    int ind2 = CompileComparison(F, ind);
    sws(F, ind2);

    while(F[ind2] == '&')
    {
        ind2 = CompileComparison(F, ind2+1);
        sws(F, ind2);
        AddCompiledByte(cAnd);
        --Comp.StackPtr;
    }

    return ind2;
}

// Compiles '|'
int FunctionParser::CompileOr(const char* F, int ind)
{
    int ind2 = CompileAnd(F, ind);
    sws(F, ind2);

    while(F[ind2] == '|')
    {
        ind2 = CompileAnd(F, ind2+1);
        sws(F, ind2);
        AddCompiledByte(cOr);
        --Comp.StackPtr;
    }

    return ind2;
}

// Compiles ','
int FunctionParser::CompileExpression(const char* F, int ind, bool stopAtComma)
{
    int ind2 = CompileOr(F, ind);
    sws(F, ind2);

    if(stopAtComma) return ind2;

    while(F[ind2] == ',')
    {
        ind2 = CompileOr(F, ind2+1);
        sws(F, ind2);
    }

    return ind2;
}


// Return parse error message
// --------------------------
const char* FunctionParser::ErrorMsg(void) const
{
    if(ParseErrorType>=0) return ParseErrorMessage[ParseErrorType];
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
}

double FunctionParser::Eval(const double* Vars)
{
    unsigned IP, DP=0;
    int SP=-1;

    for(IP=0; IP<Comp.ByteCodeSize; IP++)
    {
        switch(Comp.ByteCode[IP])
        {
// Functions:
          case   cAbs: Comp.Stack[SP]=fabs(Comp.Stack[SP]); break;
          case  cAcos: if(Comp.Stack[SP]<-1 || Comp.Stack[SP]>1)
                       { EvalErrorType=4; return 0; }
                       Comp.Stack[SP]=acos(Comp.Stack[SP]); break;
#ifndef NO_ASINH
          case cAcosh: Comp.Stack[SP]=acosh(Comp.Stack[SP]); break;
#endif
          case  cAsin: if(Comp.Stack[SP]<-1 || Comp.Stack[SP]>1)
                       { EvalErrorType=4; return 0; }
                       Comp.Stack[SP]=asin(Comp.Stack[SP]); break;
#ifndef NO_ASINH
          case cAsinh: Comp.Stack[SP]=asinh(Comp.Stack[SP]); break;
#endif
          case  cAtan: Comp.Stack[SP]=atan(Comp.Stack[SP]); break;
          case cAtan2: Comp.Stack[SP-1]=atan2(Comp.Stack[SP-1],Comp.Stack[SP]);
                       SP--; break;
#ifndef NO_ASINH
          case cAtanh: Comp.Stack[SP]=atanh(Comp.Stack[SP]); break;
#endif
          case  cCeil: Comp.Stack[SP]=ceil(Comp.Stack[SP]); break;
          case   cCos: Comp.Stack[SP]=cos(Comp.Stack[SP]); break;
          case  cCosh: Comp.Stack[SP]=cosh(Comp.Stack[SP]); break;

#ifndef DISABLE_EVAL
          case  cEval:
              {
                  double* tmpStack = Comp.Stack;
                  Comp.Stack = new double[Comp.StackSize];
                  double retVal = Eval(&tmpStack[SP-varAmount+1]);
                  delete[] Comp.Stack;
                  Comp.Stack = tmpStack;
                  SP -= varAmount-1;
                  Comp.Stack[SP] = retVal;
                  break;
              }
#endif

          case   cExp: Comp.Stack[SP]=exp(Comp.Stack[SP]); break;
          case cFloor: Comp.Stack[SP]=floor(Comp.Stack[SP]); break;

          case    cIf:
              {
                  unsigned jumpAddr = Comp.ByteCode[++IP];
                  unsigned immedAddr = Comp.ByteCode[++IP];
                  if(doubleToInt(Comp.Stack[SP]) == 0)
                  {
                      IP = jumpAddr;
                      DP = immedAddr;
                  }
                  SP--; break;
              }

          case   cInt: Comp.Stack[SP]=floor(Comp.Stack[SP]+.5); break;
          case   cLog: if(Comp.Stack[SP]<=0) { EvalErrorType=3; return 0; }
                       Comp.Stack[SP]=log(Comp.Stack[SP]); break;
          case cLog10: if(Comp.Stack[SP]<=0) { EvalErrorType=3; return 0; }
                       Comp.Stack[SP]=log10(Comp.Stack[SP]); break;
          case   cMax: Comp.Stack[SP-1]=Max(Comp.Stack[SP-1],Comp.Stack[SP]);
                       SP--; break;
          case   cMin: Comp.Stack[SP-1]=Min(Comp.Stack[SP-1],Comp.Stack[SP]);
                       SP--; break;
          case   cSin: Comp.Stack[SP]=sin(Comp.Stack[SP]); break;
          case  cSinh: Comp.Stack[SP]=sinh(Comp.Stack[SP]); break;
          case  cSqrt: if(Comp.Stack[SP]<0) { EvalErrorType=2; return 0; }
                       Comp.Stack[SP]=sqrt(Comp.Stack[SP]); break;
          case  cTanh: Comp.Stack[SP]=tanh(Comp.Stack[SP]); break;
          case   cTan: Comp.Stack[SP]=tan(Comp.Stack[SP]); break;


// Misc:
          case cImmed: Comp.Stack[++SP]=Comp.Immed[DP++]; break;
          case  cJump: DP = Comp.ByteCode[IP+2];
                       IP = Comp.ByteCode[IP+1];
                       break;

// Operators:
          case   cNeg: Comp.Stack[SP]=-Comp.Stack[SP]; break;
          case   cAdd: Comp.Stack[SP-1]+=Comp.Stack[SP]; SP--; break;
          case   cSub: Comp.Stack[SP-1]-=Comp.Stack[SP]; SP--; break;
          case   cMul: Comp.Stack[SP-1]*=Comp.Stack[SP]; SP--; break;
          case   cDiv: if(Comp.Stack[SP]==0) { EvalErrorType=1; return 0; }
                       Comp.Stack[SP-1]/=Comp.Stack[SP]; SP--; break;
          case   cMod: if(Comp.Stack[SP]==0) { EvalErrorType=1; return 0; }
                       Comp.Stack[SP-1]=fmod(Comp.Stack[SP-1],Comp.Stack[SP]);
                       SP--; break;
          case   cPow: Comp.Stack[SP-1]=pow(Comp.Stack[SP-1],Comp.Stack[SP]);
                       SP--; break;

          case cEqual: Comp.Stack[SP-1] = (Comp.Stack[SP-1]==Comp.Stack[SP]);
                       SP--; break;
          case  cLess: Comp.Stack[SP-1] = (Comp.Stack[SP-1]<Comp.Stack[SP]);
                       SP--; break;
          case cGreater: Comp.Stack[SP-1] = (Comp.Stack[SP-1]>Comp.Stack[SP]);
                         SP--; break;
          case   cAnd: Comp.Stack[SP-1] =
                           (doubleToInt(Comp.Stack[SP-1]) &&
                            doubleToInt(Comp.Stack[SP]));
                       SP--; break;
          case    cOr: Comp.Stack[SP-1] =
                           (doubleToInt(Comp.Stack[SP-1]) ||
                            doubleToInt(Comp.Stack[SP]));
                       SP--; break;

#ifdef SUPPORT_OPTIMIZER
          case   cVar:
          case   cNop: break; // Paranoia. These should never exist
          case   cDup: Comp.Stack[SP+1]=Comp.Stack[SP]; ++SP; break;
          case   cInv:
              if(Comp.Stack[SP]==0.0) { EvalErrorType=1; return 0; }
              Comp.Stack[SP]=1.0/Comp.Stack[SP];
              break;
#endif

// Variables:
          default:
              Comp.Stack[++SP]=Vars[Comp.ByteCode[IP]-VarBegin];
        }
    }

    EvalErrorType=0;
    return Comp.Stack[SP];
}


void FunctionParser::PrintByteCode(std::ostream& dest) const
{
    for(unsigned IP=0, DP=0; IP<Comp.ByteCodeSize; IP++)
    {
        dest.width(8); dest.fill('0'); hex(dest); uppercase(dest);
        dest << IP << ": ";

        unsigned opcode = Comp.ByteCode[IP];

        if(opcode == cIf)
        {
            dest << "jz\t";
            dest.width(8); dest.fill('0'); hex(dest); uppercase(dest);
            dest << Comp.ByteCode[IP+1]+1 << endl;
            IP += 2;
        }
        else if(opcode == cJump)
        {
            dest << "jump\t";
            dest.width(8); dest.fill('0'); hex(dest); uppercase(dest);
            dest << Comp.ByteCode[IP+1]+1 << endl;
            IP += 2;
        }
        else if(opcode == cImmed)
        {
            dest.precision(10);
            dest << "push\t" << Comp.Immed[DP++] << endl;
        }
        else if(opcode < VarBegin)
        {
            string n;
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
              case cLess: n = "lt"; break;
              case cGreater: n = "gt"; break;
              case cAnd: n = "and"; break;
              case cOr: n = "or"; break;

#ifndef DISABLE_EVAL
              case cEval: n = "call\t0"; break;
#endif

#ifdef SUPPORT_OPTIMIZER
              case cNop: n = "ret"; break;
              case cVar: n = "(var)"; break;
              case cDup: n = "dup"; break;
              case cInv: n = "inv"; break;
#endif

              default: n = Functions[opcode-cAbs].name;
            }
            dest << n << endl;
        }
        else
        {
            dest << "push\tVar" << opcode-VarBegin << endl;
        }
    }
}



//========================================================================
// Optimization code was contributed by Bisqwit (http://iki.fi/bisqwit/)
//========================================================================
#ifdef SUPPORT_OPTIMIZER

#include <set>
#include <deque>
#include <utility>

class SubTree
{
    struct CodeTree *tree;
    bool sign;  // Only possible when parent is cAdd or cMul

    void flipsign() { sign = !sign; }
public:
    SubTree();
    SubTree(const SubTree &b);
    SubTree(const struct CodeTree &b);
    SubTree(double imm);

    ~SubTree();

    const SubTree &operator= (const SubTree &b);
    bool getsign() const { return sign; }

    const struct CodeTree* operator-> () const { return tree; }
    const struct CodeTree& operator* () const { return *tree; }
    struct CodeTree* operator-> () { return tree; }
    struct CodeTree& operator* () { return *tree; }

    bool operator< (const SubTree& b) const;
    bool operator== (const SubTree& b) const;
    void Negate(); // Note: Parent must be cAdd
    void Invert(); // Note: Parent must be cMul
};

namespace
{
    bool IsNegate(const SubTree &p1, const SubTree &p2);
    bool IsInverse(const SubTree &p1, const SubTree &p2);
}


struct CodeTree
{
    // Vector doesn't work here.
    std::deque<SubTree> params;

    unsigned op;  // Operation
    double immed; // In case of cImmed
    unsigned var; // In case of cVar

    CodeTree(): op(cNop), immed(0), var(0) {}
    CodeTree(double imm): op(cImmed), immed(imm), var(0) {}

    bool operator== (const CodeTree& b) const;
    bool operator< (const CodeTree& b) const;


private:
    bool IsSortable() const
    {
        return
          (op == cAdd || op == cMul
        || op == cEqual
        || op == cAnd || op == cOr
        || op == cMax || op == cMin);
    }
    void SortIfPossible()
    {
        if(IsSortable())
        {
            sort(params.begin(), params.end());
        }
    }

    void ReplaceWithConstant(double value)
    {
        params.clear();
        op    = cImmed;
        immed = value;
    }

    void OptimizeConflict()
    {
        // This optimization does this: x-x = 0, x/x = 1, a+b-a = b.

        if(op == cAdd || op == cMul)
        {
        Redo:
            for(unsigned a=0; a<params.size(); ++a)
            {
                for(unsigned b=a+1; b<params.size(); ++b)
                {
                    const SubTree &p1 = params[a];
                    const SubTree &p2 = params[b];

                    if(op==cMul ? IsInverse(p1,p2)
                                : IsNegate(p1,p2))
                    {
                        // These parameters complement each others out
                        params.erase(params.begin()+b);
                        params.erase(params.begin()+a);
                        goto Redo;
                    }
                }
            }
        }
        OptimizeRedundant();
    }

    void OptimizeRedundant()
    {
        // This optimization does this: min()=0, max()=0, add()=0, mul()=1

        if(!params.size())
        {
            if(op == cAdd || op == cMin || op == cMax)
                ReplaceWithConstant(0);
            else if(op == cMul)
                ReplaceWithConstant(1);
            return;
        }

        // And this: mul(x) = x, min(x) = x, max(x) = x, add(x) = x

        if(params.size() == 1)
        {
            if(op == cMul || op == cAdd || op == cMin || op == cMax)
                if(!params[0].getsign())
                {
                    CodeTree tmp = *params[0];
                    *this = tmp;
                }
        }

        OptimizeDoubleNegations();
    }

    void OptimizeDoubleNegations()
    {
        if(op == cAdd)
        {
        	// Eschew double negations

        	// If any of the elements is cMul
        	// and has a numeric constant, negate
        	// the constant and negate sign.
        	for(unsigned a=0; a<params.size(); ++a)
        		if(params[a].getsign()
        		&& params[a]->op == cMul)
        		{
        			CodeTree &p = *params[a];
        			for(unsigned b=0; b<p.params.size(); ++b)
        				if(p.params[b]->op == cImmed)
        				{
        					p.params[b].Negate();
        					params[a].Negate();
        					break;
        				}
        		}
        }

        if(op == cMul)
        {
        	// If any of the elements is cPow
        	// and has a numeric exponent, negate
        	// the exponent and negate sign.
        	for(unsigned a=0; a<params.size(); ++a)
        		if(params[a].getsign()
        		&& params[a]->op == cPow)
        		{
        			CodeTree &p = *params[a];
        			if(p.params[1]->op == cImmed)
        			{
        				p.params[1].Negate();
       					params[a].Negate();
       				}
        		}
        }
    }

    void OptimizeConstantMath1()
    {
        // This optimization does three things:
        //      - For adding groups:
        //          Constants are added together.
        //      - For multiplying groups:
        //          Constants are multiplied together.
        //      - For function calls:
        //          If all parameters are constants,
        //          the call is replaced with constant value.

        // First, do this:
        OptimizeAddMulFlat();

        switch(op)
        {
            case cAdd:
            {
                double constant = 0.0;
                for(unsigned a=0; a<params.size(); )
                {
                    if(params[a]->op != cImmed) { ++a; continue; }
                       if(params[a].getsign())
                           constant -= params[a]->immed;
                       else
                           constant += params[a]->immed;
                    params.erase(params.begin()+a);
                }
                if(constant != 0.0) params.push_back(constant);
                //if(!params.size()) ReplaceWithConstant(constant); -- done later
                break;
            }
            case cMul:
            {
                double constant = 1.0;
                for(unsigned a=0; a<params.size(); )
                {
                    if(params[a]->op != cImmed) { ++a; continue; }
                       if(params[a].getsign())
                           constant /= params[a]->immed; //FIXME: Chance for divide by zero
                       else
                           constant *= params[a]->immed;
                    params.erase(params.begin()+a);
                }
                if(constant != 1.0) params.push_back(constant);
                //if(!params.size()) ReplaceWithConstant(constant); -- done later
                break;
            }
            #define p0 params[0]
            #define p1 params[1]
            #define ConstantUnaryFun(token, fun) \
                case token: \
                    if(p0->op == cImmed) ReplaceWithConstant(fun(p0->immed)); \
                    break;
            #define ConstantBinaryFun(token, fun) \
                case token: \
                    if(p0->op == cImmed && \
                       p1->op == cImmed) ReplaceWithConstant(fun(p0->immed, p1->immed)); \
                    break;

            // FIXME: potential invalid parameters for functions
            //        can cause exceptions here

            ConstantUnaryFun(cAbs,   fabs);
            ConstantUnaryFun(cAcos,  acos);
            ConstantUnaryFun(cAsin,  asin);
            ConstantUnaryFun(cAtan,  atan);
            ConstantUnaryFun(cCeil,  ceil);
            ConstantUnaryFun(cCos,   cos);
            ConstantUnaryFun(cCosh,  cosh);
            ConstantUnaryFun(cExp,   exp);
            ConstantUnaryFun(cFloor, floor);
            ConstantUnaryFun(cLog,   log);
            ConstantUnaryFun(cLog10, log10);
            ConstantUnaryFun(cSin,   sin);
            ConstantUnaryFun(cSinh,  sinh);
            ConstantUnaryFun(cSqrt,  sqrt);
            ConstantUnaryFun(cTan,   tan);
            ConstantUnaryFun(cTanh,  tanh);
            ConstantBinaryFun(cAtan2, atan2);
            ConstantBinaryFun(cMax,   Max);
            ConstantBinaryFun(cMin,   Min);
            ConstantBinaryFun(cPow,   pow);

            #undef p0
            #undef p1
        }
    }

    void OptimizeAddMulFlat()
    {
        // This optimization flattens the topography of the tree.
        //   Examples:
        //       x + (y+z) = x+y+z
        //       x * (y/z) = x*y/z
        //       x / (y/z) = x/y*z

        if(op == cAdd || op == cMul)
        {
            // If children are same type as parent add them here
            for(unsigned a=0; a<params.size(); )
            {
                const SubTree &pa = params[a];
                if(pa->op != op) { ++a; continue; }

                // Child is same type
                for(unsigned b=0; b<pa->params.size(); ++b)
                {
                    const SubTree &pb = pa->params[b];
                    if(pa.getsign())
                    {
                        // +a -(+b +c)
                        // means b and c will be negated

                        SubTree tmp = pb;
                        if(op == cMul)
                            tmp.Invert();
                        else
                            tmp.Negate();
                           params.push_back(tmp);
                    }
                    else
                        params.push_back(pb);
                }
                params.erase(params.begin() + a);

                // Note: OptimizeConstantMath1() would be a good thing to call next.
            }
        }
    }

    void OptimizeLinearCombine()
    {
        // This optimization does the following:
        //
        //   x*x*x*x -> x^4
        //   x+x+x+x -> x*4
        //   x*x     -> x^2
        //   x/z/z   ->
        //

        // Remove conflicts first, so we don't have to worry about signs.
        OptimizeConflict();

        bool didchanges = false;
        if(op == cAdd || op == cMul)
        {
        Redo:
            for(unsigned a=0; a<params.size(); ++a)
            {
                const SubTree &pa = params[a];

                std::set<unsigned> poslist;
                for(unsigned b=a+1; b<params.size(); ++b)
                {
                    const SubTree &pb = params[b];

                    if(*pa == *pb)
                        poslist.insert(b);
                }

                unsigned min = 2;//op==cAdd ? 2 : 1;
                if(poslist.size() >= min)
                {
                    bool negate = pa.getsign();

                    int factor = poslist.size() + 1;

                    if(negate) factor = -factor;

                    SubTree tmp;
                    tmp->op = op==cAdd ? cMul : cPow;
                    tmp->params.push_back(*pa);
                    tmp->params.push_back(CodeTree(factor));

                    std::set<unsigned>::reverse_iterator i;
                    for(i=poslist.rbegin(); i!=poslist.rend(); ++i)
                        params.erase(params.begin() + *i);

                    /*
                    if(negate)
                    {
                        // negative
                        tmp.Negate();
                    }
                    */

                    params[a] = tmp;
                    didchanges = true;
                    goto Redo;
                }
            }
        }
        if(didchanges)
        {
            // As a result, there might be need for this:
            OptimizeAddMulFlat();
            // And this:
            OptimizeRedundant();
        }
    }

    void OptimizeLogarithm()
    {
        // This optimization should do the following:
        //
        //   log(x^z)        -> z * log(x)
        //
        // Also it should do this:
        //
        //   log(x) + log(y) -> log(x * y)

        // First move to exponential form.
        OptimizeLinearCombine();

        if(op == cLog || op == cLog10)
        {
            // We should have one parameter for log() function.
            // If we don't, we're screwed.

            const SubTree &p =* params[0];

            if(p->op == cPow)
            {
                // Found log(x^y)
                const SubTree &p0 = *p->params[0];
                const SubTree &p1 = *p->params[1];
                // We will become a multiplication.
                // Add the exponent to the list now.
                params.push_back(p1);
                // Build the new logarithm.
                SubTree tmp;
                tmp->op = op;
                tmp->params.push_back(p0);
                params.push_back(tmp);

                params.erase(params.begin(), params.end()-2);
                op = cMul;
                // Finished.
            }
        }
        if(op == cAdd)
        {
            // Check which ones are logs.

            std::set<unsigned>::reverse_iterator i;

            for(unsigned phase=1; phase<=2; ++phase)
            {
                unsigned op_to_find = phase==1 ? cLog : cLog10;

                std::set<unsigned> poslist;
                for(unsigned a=0; a<params.size(); ++a)
                {
                    const SubTree &pa = params[a];

                    if(pa->op == op_to_find)
                        poslist.insert(a);
                }
                if(poslist.size() >= 2)
                {
                    SubTree tmp;
                    tmp->op = cMul;
                    for(i=poslist.rbegin(); i!=poslist.rend(); ++i)
                    {
                        const SubTree &pb = params[*i];
                        // Take all of its children
                        for(unsigned b=0; b<pb->params.size(); ++b)
                        {
                            SubTree tmp2 = pb->params[b];
                            if(pb.getsign()) tmp2.Negate();
                            tmp->params.push_back(tmp2);
                        }
                        params.erase(params.begin() + *i);
                    }
                    SubTree tmp2;
                    tmp2->op = op_to_find;
                    tmp2->params.push_back(tmp);
                    params.push_back(tmp2);
                }
            }
            // Done, hopefully
        }
    }

    void OptimizePowMulAdd()
    {
    	// BROKEN, DON'T USE (x / (y*z) -> x*y*z)
    }

    void OptimizeExponents()
    {
        // x^3 * x^2 -> x^6

        // First move to exponential form.
        OptimizeLinearCombine();

        bool didchanges = false;

        if(op == cMul)
        {
        Redo:
            for(unsigned a=0; a<params.size(); ++a)
            {
                const SubTree &pa = params[a];

                if(pa->op != cPow) continue;

                std::set<unsigned>::reverse_iterator i;
                std::set<unsigned> poslist;

                for(unsigned b=a+1; b<params.size(); ++b)
                {
                    const SubTree &pb = params[b];
                    if(pb->op == cPow
                    && *pa->params[0] == *pb->params[0])
                    {
                        poslist.insert(b);
                    }
                }

                if(poslist.size() >= 1)
                {
                    poslist.insert(a);

                    CodeTree base = *pa->params[0];

                    SubTree exponent;
                    exponent->op = cMul;

                    // Collect all exponents to cMul
                    for(i=poslist.rbegin(); i!=poslist.rend(); ++i)
                    {
                        const SubTree &pb = params[*i];

                        SubTree tmp2 = pb->params[1];
                        if(pb.getsign()) tmp2.Invert();
                        exponent->params.push_back(tmp2);
                    }

                    exponent->Optimize();

                    SubTree result = base;

                    result->op = cPow;
                    result->params.push_back(base);
                    result->params.push_back(exponent);

                    for(i=poslist.rbegin(); i!=poslist.rend(); ++i)
                    {
                        params.erase(params.begin() + *i);
                    }

                    params.push_back(result);
                    didchanges = true;
                    goto Redo;
                }
            }
        }

        OptimizePowMulAdd();

        if(didchanges)
        {
            // As a result, there might be need for this:
            OptimizeConflict();
        }
    }

    void OptimizeLinearExplode()
    {
        // x^2 -> x*x
        // But only if x is just a simple thing

        // Won't work on anything else.
        if(op != cPow) return;

        // TODO TODO TODO
    }

    void OptimizePascal()
    {
    }

public:
    void Optimize();

    void Compile(vector<unsigned> &byteCode,
                 vector<double>   &immed,
                 CodeTree &stacktop) const;
};

void CodeTree::Optimize()
{
    // Phase:
    //   Phase 0: Do local optimizations.
    //   Phase 1: Optimize each.
    //   Phase 2: Do local optimizations again.
    //   Phase 3: Sort the parameters.

    for(unsigned phase=0; phase<=3; ++phase)
    {
        if(phase == 3)
        {
            /* If the parameter order may be changed,
             * sort them.
             * NOTE: Remove cAnd and cOr here if fast boolean is in use
             */

            SortIfPossible();
        }
        if(phase == 1)
        {
            // Optimize each parameter.
            for(unsigned a=0; a<params.size(); ++a)
            {
                params[a]->Optimize();
            }
            continue;
        }
        if(phase == 0 || phase == 2)
        {
            // Do local optimizations.

            OptimizeAddMulFlat();
            OptimizeConstantMath1();
            OptimizeConflict();
            OptimizeLogarithm();
            OptimizeExponents();
            OptimizeLinearExplode();
        }
    }
}

void CodeTree::Compile
   (vector<unsigned> &byteCode,
    vector<double>   &immed,
    CodeTree &stacktop) const
{
    #define AddConst(v) do { \
        byteCode.push_back(cImmed); \
        immed.push_back((v)); \
    } while(0)
    #define AddCmd(op) byteCode.push_back((op))

    switch(op)
    {
        case cAdd:
        case cMul:
        {
            CodeTree left = stacktop;
            unsigned stacksize = 0;
            for(unsigned a=0; a<params.size(); ++a)
            {
                const CodeTree &pa = *params[a];

                if(stacksize == 2)stacksize = 1; // We already cut it

                ++stacksize;

                bool pnega = false;

                if(pa == stacktop)
                    AddCmd(cDup);
                else
                {
                    if(params[a].getsign()) pnega = true;
                    pa.Compile(byteCode, immed, stacktop);
                }

                stacktop = pa;

                if(stacksize == 2)
                {
                    stacktop = CodeTree(); //FIXME

                    if(pnega)
                    {
                        if(op == cAdd)
                        {
                            AddCmd(cSub);
                        }
                        else
                        {
                            AddCmd(cDiv);
                        }
                    }
                    else
                    {
                        AddCmd(op);
                    }
                    // leave stacksize=2 so the next step won't choke
                }
                else if(pnega)
                {
                    if(op == cMul) AddCmd(cInv);
                    else AddCmd(cNeg);
                }
            }
            break;
        }
        case cImmed:
        {
            AddConst(this->immed);
            stacktop = *this;
            break;
        }
        case cVar:
        {
            AddCmd(var);
            stacktop = *this;
            break;
        }
        case cIf:
        {
            // If the parameter amount is != 3, we're screwed.
            params[0]->Compile(byteCode, immed, stacktop);

            unsigned ofs = byteCode.size();
            AddCmd(cIf);
            AddCmd(0); // code index
            AddCmd(0); // immed index

            params[1]->Compile(byteCode, immed, stacktop);

            byteCode[ofs+1] = byteCode.size()-1;
            byteCode[ofs+2] = immed.size();

            ofs = byteCode.size();
            AddCmd(cJump);
            AddCmd(0); // code index
            AddCmd(0); // immed index

            params[2]->Compile(byteCode, immed, stacktop);

            byteCode[ofs+1] = byteCode.size()-1;
            byteCode[ofs+2] = immed.size();

            stacktop = *this;

            break;
        }
        default:
        {
            // If the parameter count is invalid, we're screwed.
            for(unsigned a=0; a<params.size(); ++a)
            {
                const CodeTree &pa = *params[a];
                pa.Compile(byteCode, immed, stacktop);
            }
            AddCmd(op);

            stacktop = *this;
        }
    }
}

bool CodeTree::operator== (const CodeTree& b) const
{
    if(op != b.op) return false;
    if(op == cImmed) if(immed != b.immed) return false;
    if(op == cVar)   if(var != b.var)     return false;
    return params == b.params;
}

bool CodeTree::operator< (const CodeTree& b) const
{
    if(params.size() != b.params.size())
        return params.size() > b.params.size();

    if(op != b.op) return op < b.op;

    if(op == cImmed && immed != b.immed) return immed < b.immed;
    if(op == cVar   && var   != b.var)   return var < b.var;
    for(unsigned a=0; a<params.size(); ++a)
        if(!(params[a] == b.params[a]))
            return params[a] < b.params[a];
    return false;
}

namespace
{
    bool IsNegate(const SubTree &p1, const SubTree &p2)
    {
        if(p1->op == cImmed && p2->op == cImmed)
        {
            return p1->immed == -p2->immed;
        }
        if(p1.getsign() == p2.getsign()) return false;
        return *p1 == *p2;
    }
    bool IsInverse(const SubTree &p1, const SubTree &p2)
    {
        if(p1->op == cImmed && p2->op == cImmed)
        {
            // FIXME: potential divide by zero.
            return p1->immed == 1.0 / p2->immed;
        }
        if(p1.getsign() == p2.getsign()) return false;
        return *p1 == *p2;
    }
}

SubTree::SubTree() : tree(new CodeTree), sign(false)
{
}

SubTree::SubTree(const SubTree &b) : tree(new CodeTree(*b.tree)), sign(b.sign)
{
}

SubTree::SubTree(const CodeTree &b) : tree(new CodeTree(b)), sign(false)
{
}

SubTree::SubTree(double imm) : tree(new CodeTree), sign(false)
{
    tree->op    = cImmed;
    tree->immed = imm;
    tree->var   = 0;
}

SubTree::~SubTree()
{
    delete tree;
}

const SubTree &SubTree::operator= (const SubTree &b)
{
    sign = b.sign;
    CodeTree *oldtree = tree;
    tree = new CodeTree(*b.tree);
    delete oldtree;
    return *this;
}

bool SubTree::operator< (const SubTree& b) const
{
    if(getsign() != b.getsign()) return getsign() < b.getsign();
    return *tree < *b.tree;
}
bool SubTree::operator== (const SubTree& b) const
{
    return sign == b.sign && *tree == *b.tree;
}
void SubTree::Negate() // Note: Parent must be cAdd
{
    if(tree->op != cImmed) { flipsign(); return; }
    tree->immed = -tree->immed;
}
void SubTree::Invert() // Note: Parent must be cMul
{
    if(tree->op != cImmed) { flipsign(); return; }
    tree->immed = 1.0 / tree->immed;
    // FIXME: potential divide by zero.
}

void FunctionParser::MakeTree(struct CodeTree *result) const
{
    vector<CodeTree> stack(1);

    #define GROW(n) do { \
        stacktop += n; \
        if(stack.size() <= stacktop) stack.resize(stacktop+1); \
    } while(0)

    #define EAT(n, opcode) do { \
        unsigned newstacktop = stacktop-n; \
        stack[stacktop].op = (opcode); \
        for(unsigned a=0, b=(n); a<b; ++a) \
            stack[stacktop].params.push_back(stack[newstacktop+a]); \
        stack.erase(stack.begin() + newstacktop, \
                    stack.begin() + stacktop); \
        stacktop = newstacktop; GROW(1); \
    } while(0)

    unsigned stacktop=0;

    std::set<unsigned> labels;

    for(unsigned IP=0, DP=0; ; ++IP)
    {
        if(labels.find(IP) != labels.end())
        {
            // The "else" of an "if" ends here
            EAT(3, cIf);
        }

        if(IP >= Comp.ByteCodeSize)
        {
            break;
        }

        unsigned opcode = Comp.ByteCode[IP];

        if(opcode == cIf)
        {
            IP += 2;
        }
        else if(opcode == cJump)
        {
            labels.insert(Comp.ByteCode[IP+1]+1);
            IP += 2;
        }
        else if(opcode == cImmed)
        {
            stack[stacktop].op     = cImmed;
            stack[stacktop].immed  = Comp.Immed[DP++];
            GROW(1);
        }
        else if(opcode < VarBegin)
        {
            switch(opcode)
            {
                // Unary operators
                case cNeg:
                {
                    EAT(1, cAdd); // Unary minus is negative adding.
                    stack[stacktop-1].params[0].Negate();
                    break;
                }
                // Binary operators
                case cSub:
                {
                    EAT(2, cAdd); // Minus is negative adding
                    stack[stacktop-1].params[1].Negate();
                    break;
                }
                case cDiv:
                {
                    EAT(2, cMul); // Divide is inverse multiply
                    stack[stacktop-1].params[1].Invert();
                    break;
                }
                case cAdd: case cMul:
                case cMod: case cPow:
                case cEqual: case cLess: case cGreater:
                case cAnd: case cOr:
                    EAT(2, opcode);
                    break;

                // Functions
                default:
                {
                    const FuncDefinition& func = Functions[opcode-cAbs];

                    unsigned paramcount = func.params;
#ifndef DISABLE_EVAL
                    if(opcode == cEval) paramcount = varAmount;
#endif
                    EAT(paramcount, opcode);
                }
            }
        }
        else
        {
            stack[stacktop].op     = cVar;
            stack[stacktop].var    = opcode;
            GROW(1);
        }
    }

    if(!stacktop)
    {
        // ERROR: Stack does not have any values!
        return;
    }

    --stacktop; // Ignore the last element, it is always cNop.

    if(stacktop > 0)
    {
        // ERROR: Stack has too many values!
        return;
    }

    // Okay, the tree is now stack[0]
    *result = stack[0];
}

void FunctionParser::Optimize()
{
    CodeTree tree;
    MakeTree(&tree);

    tree.Optimize();

    // Now rebuild from the tree.

    delete[] Comp.ByteCode;
    delete[] Comp.Immed;

    vector<unsigned> byteCode;
    vector<double> immed;

    CodeTree tmpstack;
    tree.Compile(byteCode, immed, tmpstack);

    Comp.ByteCodeSize = byteCode.size();
    Comp.ImmedSize = immed.size();

    if(Comp.ByteCodeSize)
    {
        Comp.ByteCode = new unsigned[Comp.ByteCodeSize];
        memcpy(Comp.ByteCode, &byteCode[0],
               sizeof(unsigned)*Comp.ByteCodeSize);
    }
    if(Comp.ImmedSize)
    {
        Comp.Immed = new double[Comp.ImmedSize];
        memcpy(Comp.Immed, &immed[0],
               sizeof(double)*Comp.ImmedSize);
    }
#if 0
    // FIXME: Stack is overwritten here with something evil
    if(Comp.StackSize)
        Comp.Stack = new double[Comp.StackSize];
#endif
}


#else /* !SUPPORT_OPTIMIZER */

/* keep the linker happy */
void FunctionParser::MakeTree(struct CodeTree *) const {}
void FunctionParser::Optimize()
{
    // Do nothing if no optimizations are supported.
}
#endif
