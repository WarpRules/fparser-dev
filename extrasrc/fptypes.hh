/***************************************************************************\
|* Function Parser for C++ v5.0.0                                          *|
|*-------------------------------------------------------------------------*|
|* Copyright: Juha Nieminen, Joel Yliluoma                                 *|
|*                                                                         *|
|* This library is distributed under the terms of the                      *|
|* GNU Lesser General Public License version 3.                            *|
|* (See lgpl.txt and gpl.txt for the license text.)                        *|
\***************************************************************************/

// NOTE:
// This file contains only internal types for the function parser library.
// You don't need to include this file in your code. Include "fparser.hh"
// only.

#ifndef ONCE_FPARSER_TYPES_H_
#define ONCE_FPARSER_TYPES_H_

#include "../fpconfig.hh"
#include <cstring> /* For memcmp */

#ifdef ONCE_FPARSER_H_
#include <map>
#endif

namespace FUNCTIONPARSERTYPES
{
    namespace FunctionFlag
    {
        static constexpr unsigned IsFunction  = 0x80000000u;
        static constexpr unsigned AngleIn     = 0x40000000u;
        static constexpr unsigned AngleOut    = 0x20000000u;
        static constexpr unsigned OkForInt    = 0x10000000u;
        static constexpr unsigned ComplexOnly = 0x08000000u;
    }
#define FUNCTIONPARSER_LIST_FUNCTION_OPCODES(o) \
        o(cAbs  , abs,   1, FunctionFlag::IsFunction | FunctionFlag::OkForInt ) \
        o(cAcos , acos,  1, FunctionFlag::IsFunction | FunctionFlag::AngleOut ) \
        o(cAcosh, acosh, 1, FunctionFlag::IsFunction | FunctionFlag::AngleOut ) \
        /* get the phase angle of a complex value */ \
        o(cArg  , arg,   1, FunctionFlag::IsFunction | FunctionFlag::AngleOut | FunctionFlag::ComplexOnly ) \
        o(cAsin , asin,  1, FunctionFlag::IsFunction | FunctionFlag::AngleOut ) \
        o(cAsinh, asinh, 1, FunctionFlag::IsFunction | FunctionFlag::AngleOut ) \
        o(cAtan , atan,  1, FunctionFlag::IsFunction | FunctionFlag::AngleOut ) \
        o(cAtan2, atan2, 2, FunctionFlag::IsFunction | FunctionFlag::AngleOut ) \
        o(cAtanh, atanh, 1, FunctionFlag::IsFunction ) \
        o(cCbrt , cbrt,  1, FunctionFlag::IsFunction ) \
        o(cCeil , ceil,  1, FunctionFlag::IsFunction ) \
        /* get the complex conjugate of a complex value */ \
        o(cConj , conj,  1, FunctionFlag::IsFunction | FunctionFlag::ComplexOnly ) \
        o(cCos  , cos,   1, FunctionFlag::IsFunction | FunctionFlag::AngleIn ) \
        o(cCosh , cosh,  1, FunctionFlag::IsFunction | FunctionFlag::AngleIn ) \
        o(cCot  , cot,   1, FunctionFlag::IsFunction | FunctionFlag::AngleIn ) \
        o(cCsc  , csc,   1, FunctionFlag::IsFunction | FunctionFlag::AngleIn ) \
        o(cExp  , exp,   1, FunctionFlag::IsFunction ) \
        o(cExp2 , exp2,  1, FunctionFlag::IsFunction ) \
        o(cFloor, floor, 1, FunctionFlag::IsFunction ) \
        o(cHypot, hypot, 2, FunctionFlag::IsFunction ) \
        o(cIf   , if,    0, FunctionFlag::IsFunction | FunctionFlag::OkForInt ) \
        /* get imaginary part of a complex value */ \
        o(cImag , imag,  1, FunctionFlag::IsFunction | FunctionFlag::ComplexOnly ) \
        o(cInt  , int,   1, FunctionFlag::IsFunction ) \
        o(cLog  , log,   1, FunctionFlag::IsFunction ) \
        o(cLog10, log10, 1, FunctionFlag::IsFunction ) \
        o(cLog2 , log2,  1, FunctionFlag::IsFunction ) \
        o(cMax  , max,   2, FunctionFlag::IsFunction | FunctionFlag::OkForInt ) \
        o(cMin  , min,   2, FunctionFlag::IsFunction | FunctionFlag::OkForInt ) \
        /* create a complex number from polar coordinates */ \
        o(cPolar, polar, 2, FunctionFlag::IsFunction | FunctionFlag::ComplexOnly | FunctionFlag::AngleIn ) \
        o(cPow  , pow,   2, FunctionFlag::IsFunction ) \
        /* get real part of a complex value */ \
        o(cReal , real,  1, FunctionFlag::IsFunction | FunctionFlag::ComplexOnly ) \
        o(cSec  , sec,   1, FunctionFlag::IsFunction | FunctionFlag::AngleIn ) \
        o(cSin  , sin,   1, FunctionFlag::IsFunction | FunctionFlag::AngleIn ) \
        o(cSinh , sinh,  1, FunctionFlag::IsFunction | FunctionFlag::AngleIn ) \
        o(cSqrt , sqrt,  1, FunctionFlag::IsFunction ) \
        o(cTan  , tan,   1, FunctionFlag::IsFunction | FunctionFlag::AngleIn ) \
        o(cTanh , tanh,  1, FunctionFlag::IsFunction | FunctionFlag::AngleIn ) \
        o(cTrunc, trunc, 1, FunctionFlag::IsFunction )

    enum OPCODE
    {
#define o(code, funcname, nparams, options) code,
        FUNCTIONPARSER_LIST_FUNCTION_OPCODES(o)
#undef o
// These do not need any ordering:
// Except that if you change the order of {eq,neq,lt,le,gt,ge}, you
// must also change the order in ConstantFolding_ComparisonOperations().
        cImmed, cJump,
        cNeg, cAdd, cSub, cMul, cDiv, cMod,
        cEqual, cNEqual, cLess, cLessOrEq, cGreater, cGreaterOrEq,
        cNot, cAnd, cOr,
        cNotNot, /* Protects the double-not sequence from optimizations */

        cDeg, cRad, /* Multiplication and division by 180 / pi */

        cFCall, cPCall,

#ifdef FP_SUPPORT_OPTIMIZER
        cPopNMov, /* cPopNMov(x,y) moves [y] to [x] and deletes anything
                   * above [x]. Used for disposing of temporaries.
                   */
        cLog2by, /* log2by(x,y) = log2(x) * y */
        cNop,    /* Used by fpoptimizer internally; should not occur in bytecode */
#endif
        cSinCos,   /* sin(x) followed by cos(x) (two values are pushed to stack) */
        cSinhCosh, /* hyperbolic equivalent of sincos */
        cAbsAnd,    /* As cAnd,       but assume both operands are absolute values */
        cAbsOr,     /* As cOr,        but assume both operands are absolute values */
        cAbsNot,    /* As cAbsNot,    but assume the operand is an absolute value */
        cAbsNotNot, /* As cAbsNotNot, but assume the operand is an absolute value */
        cAbsIf,     /* As cAbsIf,     but assume the 1st operand is an absolute value */

        cDup,   /* Duplicates the last value in the stack: Push [Stacktop] */
        cFetch, /* Same as Dup, except with absolute index
                 * (next value is index) */
        cInv,   /* Inverts the last value in the stack (x = 1/x) */
        cSqr,   /* squares the last operand in the stack, no push/pop */
        cRDiv,  /* reverse division (not x/y, but y/x) */
        cRSub,  /* reverse subtraction (not x-y, but y-x) */
        cRSqrt, /* inverse square-root (1/sqrt(x)) */

        cFma,   /* Fused multiply-and-add: a*b+c */
        cFms,   /* Fused multiply-and-sub: a*b-c */
        cFmma,  /* Fused multiply-and-add: a*b+c*d */
        cFmms,  /* Fused multiply-and-sub: a*b-c*d */

        VarBegin
    };

#ifdef ONCE_FPARSER_H_
    /* NamePtr is essentially the same as std::string_view,
     * but std::string_view requires c++17 while fparser
     * currently only depends on c++11.
     * See fptypes_use_stringview branch.
     */
    struct NamePtr
    {
        const char* name;
        unsigned nameLength;

        NamePtr(const char* n, unsigned l): name(n), nameLength(l) {}

        inline bool operator==(const NamePtr& rhs) const
        {
            return nameLength == rhs.nameLength
                && std::memcmp(name, rhs.name, nameLength) == 0;
        }
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

    template<typename Value_t>
    struct NameData
    {
        enum DataType { CONSTANT, UNIT, FUNC_PTR, PARSER_PTR, VARIABLE };
        DataType type;
        unsigned index; // Used with FUNC_PTR, PARSER_PTR, VARIABLE
        Value_t value;  // Used with CONSTANT, UNIT

        NameData(DataType t, unsigned v)       : type(t), index(v), value() { }
        NameData(DataType t, const Value_t& v) : type(t), index(), value(v) { }
        NameData(DataType t, Value_t&& v)      : type(t), index(), value(std::move(v)) { }
    };

    template<typename Value_t>
    using NamePtrsMap = std::map<FUNCTIONPARSERTYPES::NamePtr,
                                 FUNCTIONPARSERTYPES::NameData<Value_t>>;
#endif // ONCE_FPARSER_H_
}

#ifdef ONCE_FPARSER_H_
#include <vector>

template<typename Value_t>
struct FunctionParserBase<Value_t>::Data
{
    unsigned mReferenceCounter = 1;

    char mDelimiterChar = '\0';
    FunctionParserErrorType mParseErrorType =
        FunctionParserErrorType::no_function_parsed_yet;
    int mEvalErrorType = 0;
    bool mUseDegreeConversion = false;
    bool mHasByteCodeFlags = false;
    const char* mErrorLocation = nullptr;

    unsigned mVariablesAmount = 0;
    std::string mVariablesString {};
    FUNCTIONPARSERTYPES::NamePtrsMap<Value_t> mNamePtrs {};

    struct InlineVariable
    {
        FUNCTIONPARSERTYPES::NamePtr mName;
        unsigned mFetchIndex;
    };

    typedef std::vector<InlineVariable> InlineVarNamesContainer;
    InlineVarNamesContainer mInlineVarNames {};

    struct FuncWrapperPtrData
    {
        /* Only one of the pointers will point to a function, the other
           will be null. (The raw function pointer could be implemented
           as a FunctionWrapper specialization, but it's done like this
           for efficiency.) */
        FunctionPtr      mRawFuncPtr     = nullptr;
        FunctionWrapper* mFuncWrapperPtr = nullptr;
        unsigned mNumParams = 0;

        FuncWrapperPtrData();
        ~FuncWrapperPtrData();
        FuncWrapperPtrData(const FuncWrapperPtrData&);
        FuncWrapperPtrData(FuncWrapperPtrData&&);
        FuncWrapperPtrData& operator=(const FuncWrapperPtrData&);
        FuncWrapperPtrData& operator=(FuncWrapperPtrData&&);
    };

    struct FuncParserPtrData
    {
        FunctionParserBase<Value_t>* mParserPtr = nullptr;
        unsigned mNumParams = 0;
    };

    std::vector<FuncWrapperPtrData> mFuncPtrs {};
    std::vector<FuncParserPtrData> mFuncParsers {};

    std::vector<unsigned> mByteCode {};
    std::vector<Value_t> mImmed {};

#if !defined(FP_USE_THREAD_SAFE_EVAL) && \
    !defined(FP_USE_THREAD_SAFE_EVAL_WITH_ALLOCA)
    std::vector<Value_t> mStack {};
    // Note: When mStack exists,
    //       mStack.size() and mStackSize are mutually redundant.
#endif

    unsigned mStackSize = 0;

    Data();
    Data(const Data&);
    Data(Data&&) = delete;
    Data& operator=(const Data&) = delete; // not implemented on purpose
    Data& operator=(Data&&) = delete;
    ~Data();
};
#endif

//#include "fpaux.hh"

#endif
