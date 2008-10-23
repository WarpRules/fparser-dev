/***************************************************************************\
|* Function parser v2.4 by Warp                                            *|
|* ----------------------------                                            *|
|* Parses and evaluates the given function with the given variable values. *|
|*                                                                         *|
\***************************************************************************/

#ifndef ONCE_FPARSER_H_
#define ONCE_FPARSER_H_

#include <string>
#include <map>
#include <vector>
#include <iostream>

class FunctionParser
{
public:
    int Parse(const std::string& Function, const std::string& Vars);
    const char* ErrorMsg(void) const;
    double Eval(const double* Vars);
    inline int EvalError(void) const { return EvalErrorType; }

    void Optimize();


    FunctionParser();
    ~FunctionParser();



    // For debugging purposes only:
    void PrintByteCode(std::ostream& dest) const;

//========================================================================
private:
//========================================================================
    int varAmount, ParseErrorType,EvalErrorType;

    typedef std::map<std::string, unsigned> VarMap_t;
    VarMap_t Variables;

    struct CompiledCode
    {   CompiledCode();
        ~CompiledCode();

        unsigned* ByteCode;
        unsigned ByteCodeSize;
        double* Immed;
        unsigned ImmedSize;
        double* Stack;
        unsigned StackSize, StackPtr;
    } Comp;

    // Temp vectors for bytecode:
    std::vector<unsigned>* tempByteCode;
    std::vector<double>* tempImmed;

    VarMap_t::const_iterator FindVariable(const char*);
    int CheckSyntax(const char*);
    bool Compile(const char*);
    bool IsVariable(int);
    void AddCompiledByte(unsigned);
    void AddImmediate(double);
    int CompileIf(const char*, int);
    int CompileElement(const char*, int);
    int CompilePow(const char*, int);
    int CompileMult(const char*, int);
    int CompileAddition(const char*, int);
    int CompileComparison(const char*, int);
    int CompileAnd(const char*, int);
    int CompileOr(const char*, int);
    int CompileExpression(const char*, int, bool=false);


    void MakeTree(struct CodeTree *result) const;

    FunctionParser(const FunctionParser&);
};

#endif
