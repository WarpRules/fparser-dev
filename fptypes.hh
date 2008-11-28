//================================
// Function parser v3.0.3 by Warp
//================================

// NOTE:
// This file contains only internal types for the function parser library.
// You don't need to include this file in your code. Include "fparser.hh"
// only.

#include "fpconfig.hh"

namespace FUNCTIONPARSERTYPES
{
// The functions must be in alphabetical order:
    enum OPCODE
    {
        cAbs, cAcos,
#ifndef FP_NO_ASINH
        cAcosh,
#endif
        cAsin,
#ifndef FP_NO_ASINH
        cAsinh,
#endif
        cAtan,
        cAtan2,
#ifndef FP_NO_ASINH
        cAtanh,
#endif
        cCeil, cCos, cCosh, cCot, cCsc,
#ifndef FP_DISABLE_EVAL
        cEval,
#endif
        cExp, cFloor, cIf, cInt, cLog, cLog2, cLog10, cMax, cMin,
        cPow, cSec, cSin, cSinh, cSqrt, cTan, cTanh,

// These do not need any ordering:
        cImmed, cJump,
        cNeg, cAdd, cSub, cMul, cDiv, cMod,
        cEqual, cNEqual, cLess, cLessOrEq, cGreater, cGreaterOrEq,
        cNot, cAnd, cOr,

        cDeg, cRad,

        cFCall, cPCall,

#ifdef FP_SUPPORT_OPTIMIZER
        cVar,   /* Denotes a variable in CodeTree (not used by bytecode) */
        cDup,   /* Duplicates the last value in the stack: Pop A, Push A, Push A */
        cInv,   /* Inverts the last value in the stack (x = 1/x) */
        cFetch, /* Same as Dup, except with absolute index (next value is index) */
        cPopNMov,   /* cPopNMov(x,y) moves [y] to [x] and deletes anything above [x] */
        cSqr,   /* squares the last operand in the stack, no push/pop */
        cRDiv,  /* reverse division (not x/y, but y/x) */
        cRSub,  /* reverse subtraction (not x-y, but y-x) */
        cNotNot, /* Protects the double-not sequence from optimizations */
#endif

        cNop,
        VarBegin
    };

#ifdef ONCE_FPARSER_H_
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
#ifndef FP_NO_ASINH
        { "acosh", 5, cAcosh, 1 },
#endif
        { "asin", 4, cAsin, 1 },
#ifndef FP_NO_ASINH
        { "asinh", 5, cAsinh, 1 },
#endif
        { "atan", 4, cAtan, 1 },
        { "atan2", 5, cAtan2, 2 },
#ifndef FP_NO_ASINH
        { "atanh", 5, cAtanh, 1 },
#endif
        { "ceil", 4, cCeil, 1 },
        { "cos", 3, cCos, 1 },
        { "cosh", 4, cCosh, 1 },
        { "cot", 3, cCot, 1 },
        { "csc", 3, cCsc, 1 },
#ifndef FP_DISABLE_EVAL
        { "eval", 4, cEval, 0 },
#endif
        { "exp", 3, cExp, 1 },
        { "floor", 5, cFloor, 1 },
        { "if", 2, cIf, 0 },
        { "int", 3, cInt, 1 },
        { "log", 3, cLog, 1 },
        { "log10", 5, cLog10, 1 },
        { "log2", 4, cLog2, 1 },
        { "max", 3, cMax, 2 },
        { "min", 3, cMin, 2 },
        { "pow", 3, cPow, 2 },
        { "sec", 3, cSec, 1 },
        { "sin", 3, cSin, 1 },
        { "sinh", 4, cSinh, 1 },
        { "sqrt", 4, cSqrt, 1 },
        { "tan", 3, cTan, 1 },
        { "tanh", 4, cTanh, 1 }
    };

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

    // -1 = (lhs < rhs); 0 = (lhs == rhs); 1 = (lhs > rhs)
    inline int compare(const FuncDefinition& lhs, const NamePtr& rhs)
    {
        for(unsigned i = 0; i < lhs.nameLength; ++i)
        {
            if(i == rhs.nameLength) return 1;
            const char c1 = lhs.name[i], c2 = rhs.name[i];
            if(c1 < c2) return -1;
            if(c2 < c1) return 1;
        }
        return lhs.nameLength < rhs.nameLength ? -1 : 0;
    }

    inline const FuncDefinition* findFunction(const NamePtr& functionName)
    {
        const FuncDefinition* first = Functions;
        const FuncDefinition* last = Functions + FUNC_AMOUNT;

        while(first < last)
        {
            const FuncDefinition* middle = first+(last-first)/2;
            const int comp = compare(*middle, functionName);
            if(comp == 0) return middle;
            if(comp < 0) first = middle+1;
            else last = middle;
        }
        return 0;
    }
#endif
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
