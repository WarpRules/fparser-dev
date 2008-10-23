//===============================
// Function parser v2.3 by Warp
//===============================

// Comment out the following line if your compiler supports the (non-standard)
// asinh, acosh and atanh functions and you want them to be supported. If
// you are not sure, just leave it (those function will then not be supported).
#define NO_ASINH

// Uncomment the following line to disable the eval() function if it could
// be too dangerous in the target application:
//#define DISABLE_EVAL


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
#ifndef NO_ASINH
        cAtanh,
#endif
        cCeil, cCos, cCosh,
#ifndef DISABLE_EVAL
        cEval,
#endif
        cExp, cFloor, cIf, cInt, cLog, cMax, cMin,
        cSin, cSinh, cSqrt, cTan, cTanh,

        cImmed, cJump,
        cNeg, cAdd, cSub, cMul, cDiv, cMod, cPow,
        cEqual, cLess, cGreater, cAnd, cOr,

        VarBegin
    };

    struct FuncDefinition
    {
        const char* name;
        unsigned nameLength;
        unsigned opcode;
        unsigned params;

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
        { "max", 3, cMax, 2 },
        { "min", 3, cMin, 2 },
        { "sin", 3, cSin, 1 },
        { "sinh", 4, cSinh, 1 },
        { "sqrt", 4, cSqrt, 1 },
        { "tan", 3, cTan, 1 },
        { "tanh", 4, cTanh, 1 }
    };

    const unsigned FUNC_AMOUNT = sizeof(Functions)/sizeof(Functions[0]);


    inline const FuncDefinition* FindFunction(const char* F)
    {
        FuncDefinition func = { F, 0, 0, 0 };
        while(isalpha(F[func.nameLength])) ++func.nameLength;
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

// Copy constructor (only for private use)
FunctionParser::FunctionParser(const FunctionParser& cpy):
    varAmount(cpy.varAmount),
    EvalErrorType(cpy.EvalErrorType),
    Comp(cpy.Comp)
{}

FunctionParser::~FunctionParser()
{}

FunctionParser::CompiledCode::CompiledCode():
    ByteCode(0), ByteCodeSize(0),
    Immed(0), ImmedSize(0),
    Stack(0), StackSize(0),
    thisIsACopy(false)
{}

FunctionParser::CompiledCode::CompiledCode(const CompiledCode& cpy):
    ByteCode(cpy.ByteCode), ByteCodeSize(cpy.ByteCodeSize),
    Immed(cpy.Immed), ImmedSize(cpy.ImmedSize),
    Stack(new double[cpy.StackSize]), StackSize(cpy.StackSize),
    thisIsACopy(true)
{}

FunctionParser::CompiledCode::~CompiledCode()
{
    if(!thisIsACopy && ByteCode) { delete[] ByteCode; ByteCode=0; }
    if(!thisIsACopy && Immed) { delete[] Immed; Immed=0; }
    if(Stack) { delete[] Stack; Stack=0; }
}


//---------------------------------------------------------------------------
// Function parsing
//---------------------------------------------------------------------------
//===========================================================================
namespace
{
    const char* ParseErrorMessage[]=
    {
        "Syntax error",                             // 0
        "Mismatched parenthesis",                   // 1
        "Missing ')'",                              // 2
        "Empty parentheses",                        // 3
        "Syntax error. Operator expected",          // 4
        "Not enough memory",                        // 5
        "An unexpected error ocurred. Please make a full bug report "
        "to warp@iki.fi",                           // 6
        "Syntax error in parameter 'Vars' given to "
        "FunctionParser::Parse()",                  // 7
        "Illegal number of parameters to function"  // 8
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
        if(c==0) { ParseErrorType=0; return Ind; }

        // Check for math function
        const FuncDefinition* fptr = FindFunction(&Function[Ind]);
        if(fptr)
        {
            Ind += fptr->nameLength;
            sws(Function, Ind);
            c = Function[Ind];
            if(c!='(') { ParseErrorType=0; return Ind; }
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
        int ind2 = CompileElement(F, ind+1);
        AddCompiledByte(cNeg);
        return ind2;
    }

    if(isdigit(c) || c=='.') // Number
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
#ifndef NO_ASINH
          case cAtanh: Comp.Stack[SP]=atanh(Comp.Stack[SP]); break;
#endif
          case  cCeil: Comp.Stack[SP]=ceil(Comp.Stack[SP]); break;
          case   cCos: Comp.Stack[SP]=cos(Comp.Stack[SP]); break;
          case  cCosh: Comp.Stack[SP]=cosh(Comp.Stack[SP]); break;

#ifndef DISABLE_EVAL
          case  cEval:
              {
                  FunctionParser fpcopy(*this);
                  double retVal = fpcopy.Eval(&Comp.Stack[SP-varAmount+1]);
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
        else if(opcode == cEval)
        {
            dest << "call\t0" << endl;
        }
        else if(/*opcode >= cAbs &&*/ opcode <= cTanh)
        {
            dest << Functions[opcode-cAbs].name << endl;
        }
        else if(opcode == cImmed)
        {
            dest.precision(10);
            dest << "push\t" << Comp.Immed[DP++] << endl;
        }
        else if(opcode == cJump)
        {
            dest << "jump\t";
            dest.width(8); dest.fill('0'); hex(dest); uppercase(dest);
            dest << Comp.ByteCode[IP+1]+1 << endl;
            IP += 2;
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
            }
            dest << n << endl;
        }
        else
        {
            dest << "push\tVar" << opcode-VarBegin << endl;
        }
    }    
}
