/***************************************************************************\
|* Function Parser for C++ v3.3.2                                          *|
|*-------------------------------------------------------------------------*|
|* Copyright: Juha Nieminen                                                *|
\***************************************************************************/

// NOTE:
// This file contains only internal types for the function parser library.
// You don't need to include this file in your code. Include "fparser.hh"
// only.

#ifndef ONCE_FPARSER_TYPES_H_
#define ONCE_FPARSER_TYPES_H_

#include "fpconfig.hh"
#include <cmath>

namespace FUNCTIONPARSERTYPES
{
    enum OPCODE
    {
// The order of opcodes in the function list must
// match that which is in the Functions[] array.
        cAbs,
        cAcos, cAcosh,
        cAsin, cAsinh,
        cAtan, cAtan2, cAtanh,
        cCeil,
        cCos, cCosh, cCot, cCsc,
        cEval,
        cExp, cExp2, cFloor, cIf, cInt, cLog, cLog10, cLog2, cMax, cMin,
        cPow, cSec, cSin, cSinh, cSqrt, cTan, cTanh,
        cTrunc,

// These do not need any ordering:
        cImmed, cJump,
        cNeg, cAdd, cSub, cMul, cDiv, cMod,
        cEqual, cNEqual, cLess, cLessOrEq, cGreater, cGreaterOrEq,
        cNot, cAnd, cOr,
        cNotNot, /* Protects the double-not sequence from optimizations */

        cDeg, cRad,

        cFCall, cPCall,
        cRPow,

#ifdef FP_SUPPORT_OPTIMIZER
        cVar,   /* Denotes a variable in CodeTree (not used by bytecode) */
        cFetch, /* Same as Dup, except with absolute index
                   (next value is index) */
        cPopNMov, /* cPopNMov(x,y) moves [y] to [x] and deletes anything
                     above [x] */
        cLog2by, /* log2by(x,y) = log2(x) * y */
#endif
        cAbsAnd,    /* As cAnd,       but assume both operands are absolute values */
        cAbsOr,     /* As cOr,        but assume both operands are absolute values */
        cAbsNot,    /* As cAbsNot,    but assume the operand is an absolute value */
        cAbsNotNot, /* As cAbsNotNot, but assume the operand is an absolute value */
        cAbsIf,     /* As cAbsIf,     but assume the 1st operand is an absolute value */

        cDup,   /* Duplicates the last value in the stack:
                   Pop A, Push A, Push A */
        cInv,   /* Inverts the last value in the stack (x = 1/x) */
        cSqr,   /* squares the last operand in the stack, no push/pop */
        cRDiv,  /* reverse division (not x/y, but y/x) */
        cRSub,  /* reverse subtraction (not x-y, but y-x) */
        cRSqrt, /* inverse square-root) */

        cNop,
        VarBegin
    };

#ifdef ONCE_FPARSER_H_
    struct FuncDefinition
    {
        enum FunctionFlags
        {
            Enabled  = 0x01,
            AngleIn  = 0x02,
            AngleOut = 0x04
        };

#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
        const char name[8];
#else
        struct name { } name;
#endif
        unsigned params : 8;
        unsigned flags  : 8;

        inline bool enabled() const { return flags != 0; }
    };

#ifndef FP_DISABLE_EVAL
# define FP_EVAL_FUNCTION_ENABLED FuncDefinition::Enabled
#else
# define FP_EVAL_FUNCTION_ENABLED 0
#endif
#ifdef FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
# define FP_FNAME(n) n
#else
# define FP_FNAME(n) {}
#endif
// This list must be in the same order as that in OPCODE enum,
// because the opcode value is used to index this array, and
// the pointer to array element is used for generating the opcode.
    const FuncDefinition Functions[]=
    {
        /*cAbs  */ { FP_FNAME("abs"),   1, FuncDefinition::Enabled },
        /*cAcos */ { FP_FNAME("acos"),  1, FuncDefinition::Enabled | FuncDefinition::AngleOut },
        /*cAcosh*/ { FP_FNAME("acosh"), 1, FuncDefinition::Enabled | FuncDefinition::AngleOut },
        /*cAsin */ { FP_FNAME("asin"),  1, FuncDefinition::Enabled | FuncDefinition::AngleOut },
        /*cAsinh*/ { FP_FNAME("asinh"), 1, FuncDefinition::Enabled | FuncDefinition::AngleOut },
        /*cAtan */ { FP_FNAME("atan"),  1, FuncDefinition::Enabled | FuncDefinition::AngleOut },
        /*cAtan2*/ { FP_FNAME("atan2"), 2, FuncDefinition::Enabled | FuncDefinition::AngleOut },
        /*cAtanh*/ { FP_FNAME("atanh"), 1, FuncDefinition::Enabled },
        /*cCeil */ { FP_FNAME("ceil"),  1, FuncDefinition::Enabled },
        /*cCos  */ { FP_FNAME("cos"),   1, FuncDefinition::Enabled | FuncDefinition::AngleIn },
        /*cCosh */ { FP_FNAME("cosh"),  1, FuncDefinition::Enabled | FuncDefinition::AngleIn },
        /*cCot  */ { FP_FNAME("cot"),   1, FuncDefinition::Enabled | FuncDefinition::AngleIn },
        /*cCsc  */ { FP_FNAME("csc"),   1, FuncDefinition::Enabled | FuncDefinition::AngleIn },
        /*cEval */ { FP_FNAME("eval"),  0, FP_EVAL_FUNCTION_ENABLED },
        /*cExp  */ { FP_FNAME("exp"),   1, FuncDefinition::Enabled },
        /*cExp2 */ { FP_FNAME("exp2"),  1, FuncDefinition::Enabled },
        /*cFloor*/ { FP_FNAME("floor"), 1, FuncDefinition::Enabled },
        /*cIf   */ { FP_FNAME("if"),    0, FuncDefinition::Enabled },
        /*cInt  */ { FP_FNAME("int"),   1, FuncDefinition::Enabled },
        /*cLog  */ { FP_FNAME("log"),   1, FuncDefinition::Enabled },
        /*cLog10*/ { FP_FNAME("log10"), 1, FuncDefinition::Enabled },
        /*cLog2 */ { FP_FNAME("log2"),  1, FuncDefinition::Enabled },
        /*cMax  */ { FP_FNAME("max"),   2, FuncDefinition::Enabled },
        /*cMin  */ { FP_FNAME("min"),   2, FuncDefinition::Enabled },
        /*cPow  */ { FP_FNAME("pow"),   2, FuncDefinition::Enabled },
        /*cSec  */ { FP_FNAME("sec"),   1, FuncDefinition::Enabled | FuncDefinition::AngleIn },
        /*cSin  */ { FP_FNAME("sin"),   1, FuncDefinition::Enabled | FuncDefinition::AngleIn },
        /*cSinh */ { FP_FNAME("sinh"),  1, FuncDefinition::Enabled | FuncDefinition::AngleIn },
        /*cSqrt */ { FP_FNAME("sqrt"),  1, FuncDefinition::Enabled },
        /*cTan  */ { FP_FNAME("tan"),   1, FuncDefinition::Enabled | FuncDefinition::AngleIn },
        /*cTanh */ { FP_FNAME("tanh"),  1, FuncDefinition::Enabled | FuncDefinition::AngleIn },
        /*cTrunc*/ { FP_FNAME("trunc"), 1, FuncDefinition::Enabled }
    };
#undef FP_FNAME

    struct NamePtr
    {
        const char* name;
        unsigned nameLength;

        NamePtr(const char* n, unsigned l): name(n), nameLength(l) {}

        inline bool operator<(const NamePtr& rhs) const
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

    struct NameData
    {
        enum DataType { CONSTANT, UNIT, FUNC_PTR, PARSER_PTR };

        DataType type;
        std::string name;

        union
        {
            unsigned index;
            double value;
        };

        NameData(DataType t, const std::string& n): type(t), name(n) {}

        inline bool operator<(const NameData& rhs) const
        {
            return name < rhs.name;
        }
    };

    const unsigned FUNC_AMOUNT = sizeof(Functions)/sizeof(Functions[0]);

    /* This function generated with make_function_name_parser.cc */
    inline const FuncDefinition* findFunction(const NamePtr& functionName)
    {
    /* prefix  */if(functionName.nameLength == 0) return 0;
    if(functionName.nameLength > 5) return 0;
    switch(functionName.name[0]) {
    case 'a':
    /* prefix a */if(functionName.nameLength == 1) return 0;
    switch(functionName.name[1]) {
    case 'b':
    /* prefix ab */if(functionName.nameLength == 3
    && 's' == functionName.name[2]
    ) return Functions+cAbs;/*abs*/
    else return 0;
    case 'c':
    /* prefix ac */if(functionName.nameLength == 2) return 0;
    if('o' == functionName.name[2]) {
    /* prefix aco */if(functionName.nameLength == 3) return 0;
    if('s' == functionName.name[3]) {
    /* prefix acos */if(functionName.nameLength == 4) return Functions+cAcos;/*acos*/
    if('h' == functionName.name[4]) {
    /* prefix acosh */return Functions+cAcosh;/*acosh*/
    }else return 0;}else return 0;}else return 0;case 's':
    /* prefix as */if(functionName.nameLength == 2) return 0;
    if('i' == functionName.name[2]) {
    /* prefix asi */if(functionName.nameLength == 3) return 0;
    if('n' == functionName.name[3]) {
    /* prefix asin */if(functionName.nameLength == 4) return Functions+cAsin;/*asin*/
    if('h' == functionName.name[4]) {
    /* prefix asinh */return Functions+cAsinh;/*asinh*/
    }else return 0;}else return 0;}else return 0;case 't':
    /* prefix at */if(functionName.nameLength == 2) return 0;
    if('a' == functionName.name[2]) {
    /* prefix ata */if(functionName.nameLength == 3) return 0;
    if('n' == functionName.name[3]) {
    /* prefix atan */if(functionName.nameLength == 4) return Functions+cAtan;/*atan*/
    switch(functionName.name[4]) {
    case '2':
    /* prefix atan2 */return Functions+cAtan2;/*atan2*/
    case 'h':
    /* prefix atanh */return Functions+cAtanh;/*atanh*/
    default: return 0; }}else return 0;}else return 0;default: return 0; }case 'c':
    /* prefix c */if(functionName.nameLength == 1) return 0;
    if(functionName.nameLength > 4) return 0;
    switch(functionName.name[1]) {
    case 'e':
    /* prefix ce */if(functionName.nameLength == 4
    && 'i' == functionName.name[2]
    && 'l' == functionName.name[3]
    ) return Functions+cCeil;/*ceil*/
    else return 0;
    case 'o':
    /* prefix co */if(functionName.nameLength == 2) return 0;
    switch(functionName.name[2]) {
    case 's':
    /* prefix cos */if(functionName.nameLength == 3) return Functions+cCos;/*cos*/
    if('h' == functionName.name[3]) {
    /* prefix cosh */return Functions+cCosh;/*cosh*/
    }else return 0;case 't':
    /* prefix cot */if(functionName.nameLength == 3
    ) return Functions+cCot;/*cot*/
    else return 0;
    default: return 0; }case 's':
    /* prefix cs */if(functionName.nameLength == 3
    && 'c' == functionName.name[2]
    ) return Functions+cCsc;/*csc*/
    else return 0;
    default: return 0; }case 'e':
    /* prefix e */if(functionName.nameLength == 1) return 0;
    if(functionName.nameLength > 4) return 0;
    switch(functionName.name[1]) {
    case 'v':
    /* prefix ev */if(functionName.nameLength == 4
    && 'a' == functionName.name[2]
    && 'l' == functionName.name[3]
    ) return Functions+cEval;/*eval*/
    else return 0;
    case 'x':
    /* prefix ex */if(functionName.nameLength == 2) return 0;
    if('p' == functionName.name[2]) {
    /* prefix exp */if(functionName.nameLength == 3) return Functions+cExp;/*exp*/
    if('2' == functionName.name[3]) {
    /* prefix exp2 */return Functions+cExp2;/*exp2*/
    }else return 0;}else return 0;default: return 0; }case 'f':
    /* prefix f */if(functionName.nameLength == 5
    && 'l' == functionName.name[1]
    && 'o' == functionName.name[2]
    && 'o' == functionName.name[3]
    && 'r' == functionName.name[4]
    ) return Functions+cFloor;/*floor*/
    else return 0;
    case 'i':
    /* prefix i */if(functionName.nameLength == 1) return 0;
    if(functionName.nameLength > 3) return 0;
    switch(functionName.name[1]) {
    case 'f':
    /* prefix if */if(functionName.nameLength == 2
    ) return Functions+cIf;/*if*/
    else return 0;
    case 'n':
    /* prefix in */if(functionName.nameLength == 3
    && 't' == functionName.name[2]
    ) return Functions+cInt;/*int*/
    else return 0;
    default: return 0; }case 'l':
    /* prefix l */if(functionName.nameLength == 1) return 0;
    if('o' == functionName.name[1]) {
    /* prefix lo */if(functionName.nameLength == 2) return 0;
    if('g' == functionName.name[2]) {
    /* prefix log */if(functionName.nameLength == 3) return Functions+cLog;/*log*/
    switch(functionName.name[3]) {
    case '1':
    /* prefix log1 */if(functionName.nameLength == 5
    && '0' == functionName.name[4]
    ) return Functions+cLog10;/*log10*/
    else return 0;
    case '2':
    /* prefix log2 */if(functionName.nameLength == 4
    ) return Functions+cLog2;/*log2*/
    else return 0;
    default: return 0; }}else return 0;}else return 0;case 'm':
    /* prefix m */if(functionName.nameLength == 1) return 0;
    if(functionName.nameLength > 3) return 0;
    switch(functionName.name[1]) {
    case 'a':
    /* prefix ma */if(functionName.nameLength == 3
    && 'x' == functionName.name[2]
    ) return Functions+cMax;/*max*/
    else return 0;
    case 'i':
    /* prefix mi */if(functionName.nameLength == 3
    && 'n' == functionName.name[2]
    ) return Functions+cMin;/*min*/
    else return 0;
    default: return 0; }case 'p':
    /* prefix p */if(functionName.nameLength == 3
    && 'o' == functionName.name[1]
    && 'w' == functionName.name[2]
    ) return Functions+cPow;/*pow*/
    else return 0;
    case 's':
    /* prefix s */if(functionName.nameLength == 1) return 0;
    if(functionName.nameLength > 4) return 0;
    switch(functionName.name[1]) {
    case 'e':
    /* prefix se */if(functionName.nameLength == 3
    && 'c' == functionName.name[2]
    ) return Functions+cSec;/*sec*/
    else return 0;
    case 'i':
    /* prefix si */if(functionName.nameLength == 2) return 0;
    if('n' == functionName.name[2]) {
    /* prefix sin */if(functionName.nameLength == 3) return Functions+cSin;/*sin*/
    if('h' == functionName.name[3]) {
    /* prefix sinh */return Functions+cSinh;/*sinh*/
    }else return 0;}else return 0;case 'q':
    /* prefix sq */if(functionName.nameLength == 4
    && 'r' == functionName.name[2]
    && 't' == functionName.name[3]
    ) return Functions+cSqrt;/*sqrt*/
    else return 0;
    default: return 0; }case 't':
    /* prefix t */if(functionName.nameLength == 1) return 0;
    switch(functionName.name[1]) {
    case 'a':
    /* prefix ta */if(functionName.nameLength == 2) return 0;
    if(functionName.nameLength > 4) return 0;
    if('n' == functionName.name[2]) {
    /* prefix tan */if(functionName.nameLength == 3) return Functions+cTan;/*tan*/
    if('h' == functionName.name[3]) {
    /* prefix tanh */return Functions+cTanh;/*tanh*/
    }else return 0;}else return 0;case 'r':
    /* prefix tr */if(functionName.nameLength == 5
    && 'u' == functionName.name[2]
    && 'n' == functionName.name[3]
    && 'c' == functionName.name[4]
    ) return Functions+cTrunc;/*trunc*/
    else return 0;
    default: return 0; }default: return 0; }
    }

#ifndef FP_SUPPORT_ASINH
    inline double fp_asinh(double x) { return log(x + sqrt(x*x + 1)); }
    inline double fp_acosh(double x) { return log(x + sqrt(x*x - 1)); }
    inline double fp_atanh(double x) { return log( (1+x) / (1-x) ) * 0.5; }
#else
    inline double fp_asinh(double x) { return asinh(x); }
    inline double fp_acosh(double x) { return acosh(x); }
    inline double fp_atanh(double x) { return atanh(x); }
#endif // FP_SUPPORT_ASINH
    inline double fp_trunc(double x) { return x<0.0 ? ceil(x) : floor(x); }

    /* fp_pow() is a wrapper for std::pow()
     * that produces an identical value for
     * exp(1) ^ 2.0  (0x4000000000000000)
     * as exp(2.0)   (0x4000000000000000)
     * - std::pow() on x86_64
     * produces 2.0  (0x3FFFFFFFFFFFFFFF) instead!
     */
    inline double fp_pow(double x,double y)
    {
        //if(x == 1.0) return 1.0;
        if(x > 0) return std::exp(std::log(x) * y);
        if(y == 0.0) return 1.0;
        if(y < 0) return 1.0 / fp_pow(x, -y);
        return std::pow(x, y);
    }

#ifdef FP_EPSILON
    inline bool FloatEqual(double a, double b)
    { return fabs(a - b) <= FP_EPSILON; }
#else
    inline bool FloatEqual(double a, double b)
    { return a == b; }
#endif // FP_EPSILON

    inline bool IsIntegerConst(double a)
    { return FloatEqual(a, (double)(long)a); }

#endif // ONCE_FPARSER_H_
}

#ifdef ONCE_FPARSER_H_
#include <map>
#include <set>
#include <vector>

struct FunctionParser::Data
{
    unsigned referenceCounter;

    std::string variablesString;
    std::map<FUNCTIONPARSERTYPES::NamePtr, unsigned> variableRefs;

    std::set<FUNCTIONPARSERTYPES::NameData> nameData;
    std::map<FUNCTIONPARSERTYPES::NamePtr,
             const FUNCTIONPARSERTYPES::NameData*> namePtrs;

    struct FuncPtrData
    {
        union { FunctionPtr funcPtr; FunctionParser* parserPtr; };
        unsigned params;
    };

    std::vector<FuncPtrData> FuncPtrs;
    std::vector<FuncPtrData> FuncParsers;

    std::vector<unsigned> ByteCode;
    std::vector<double> Immed;
    std::vector<double> Stack;
    unsigned StackSize;

    Data(): referenceCounter(1),
            variablesString(),
            variableRefs(),
            nameData(),
            namePtrs(),
            FuncPtrs(),
            FuncParsers(),
            ByteCode(),
            Immed(), Stack(), StackSize(0) {}
    Data(const Data&);
    Data& operator=(const Data&); // not implemented on purpose
};
#endif

#endif
