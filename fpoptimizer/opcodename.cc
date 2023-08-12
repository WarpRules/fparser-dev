#include <string>
#include <sstream>
#include <assert.h>

#include <iostream>

#include "fpconfig.hh"
#include "extrasrc/fptypes.hh"

#include "grammar.hh"
#include "opcodename.hh"

using namespace FPoptimizer_Grammar;
using namespace FUNCTIONPARSERTYPES;

const std::string FP_GetOpcodeName(FPoptimizer_Grammar::SpecialOpcode opcode, bool pad)
{
#if 1
    /* Symbolic meanings for the opcodes? */
    const char* p = 0;
    switch( opcode )
    {
        case NumConstant:   p = "NumConstant"; break;
        case ParamHolder:   p = "ParamHolder"; break;
        case SubFunction:   p = "SubFunction"; break;
    }
    std::ostringstream tmp;
    //if(!p) std::cerr << "o=" << opcode << "\n";
    assert(p);
    tmp << p;
    if(pad) while(tmp.str().size() < 12) tmp << ' ';
    return tmp.str();
#else
    /* Just numeric meanings */
    std::ostringstream tmp;
    tmp << opcode;
    if(pad) while(tmp.str().size() < 5) tmp << ' ';
    return tmp.str();
#endif
}

const std::string FP_GetOpcodeName(FUNCTIONPARSERTYPES::OPCODE opcode,        bool pad)
{
#if 1
    /* Symbolic meanings for the opcodes? */
    const char* p = 0;
    switch(opcode)
    {
#define o(code, funcname, nparams, options) \
            case FUNCTIONPARSERTYPES::code: p = #code; break;
        FUNCTIONPARSER_LIST_FUNCTION_OPCODES(o)
#undef o
        case cImmed: p = "cImmed"; break;
        case cJump: p = "cJump"; break;
        case cNeg: p = "cNeg"; break;
        case cAdd: p = "cAdd"; break;
        case cSub: p = "cSub"; break;
        case cMul: p = "cMul"; break;
        case cDiv: p = "cDiv"; break;
        case cMod: p = "cMod"; break;
        case cEqual: p = "cEqual"; break;
        case cNEqual: p = "cNEqual"; break;
        case cLess: p = "cLess"; break;
        case cLessOrEq: p = "cLessOrEq"; break;
        case cGreater: p = "cGreater"; break;
        case cGreaterOrEq: p = "cGreaterOrEq"; break;
        case cNot: p = "cNot"; break;
        case cAnd: p = "cAnd"; break;
        case cOr: p = "cOr"; break;
        case cDeg: p = "cDeg"; break;
        case cRad: p = "cRad"; break;
        case cFCall: p = "cFCall"; break;
        case cPCall: p = "cPCall"; break;
#ifdef FP_SUPPORT_OPTIMIZER
        case cFetch: p = "cFetch"; break;
        case cPopNMov: p = "cPopNMov"; break;
        case cLog2by: p = "cLog2by"; break;
        case cNop: p = "cNop"; break;
#endif
        case cSinCos: p = "cSinCos"; break;
        case cSinhCosh: p = "cSinhCosh"; break;
        case cAbsNot: p = "cAbsNot"; break;
        case cAbsNotNot: p = "cAbsNotNot"; break;
        case cAbsAnd: p = "cAbsAnd"; break;
        case cAbsOr: p = "cAbsOr"; break;
        case cAbsIf: p = "cAbsIf"; break;
        case cDup: p = "cDup"; break;
        case cInv: p = "cInv"; break;
        case cSqr: p = "cSqr"; break;
        case cRDiv: p = "cRDiv"; break;
        case cRSub: p = "cRSub"; break;
        case cNotNot: p = "cNotNot"; break;
        case cRSqrt: p = "cRSqrt"; break;
        case cFma: p = "cFma"; break;
        case cFms: p = "cFms"; break;
        case cFmma: p = "cFmma"; break;
        case cFmms: p = "cFmms"; break;
        case VarBegin: p = "VarBegin"; break;
    }
    std::ostringstream tmp;
    //if(!p) std::cerr << "o=" << opcode << "\n";
    assert(p);
    tmp << p;
    if(pad) while(tmp.str().size() < 12) tmp << ' ';
    return tmp.str();
#else
    /* Just numeric meanings */
    std::ostringstream tmp;
    tmp << opcode;
    if(pad) while(tmp.str().size() < 5) tmp << ' ';
    return tmp.str();
#endif
}
