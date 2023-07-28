#include "fparser.hh"
#include "extrasrc/fpaux.hh"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

/*
#define CONSTANT_L10B  0.3010299956639811952137 // log10(2)
#define CONSTANT_L10BI 3.3219280948873623478703 // 1/log10(2)
#define CONSTANT_LB10  CONSTANT_L10BI          // log2(10)
#define CONSTANT_LB10I CONSTANT_L10B           // 1/log2(10)
*/

#define CONSTANT_POS_INF     HUGE_VAL  // positive infinity, from math.h
#define CONSTANT_NEG_INF   (-HUGE_VAL) // negative infinity

namespace FUNCTIONPARSERTYPES
{
    template<typename Value_t>
    inline Value_t fp_const_pihalf() // CONSTANT_PIHALF
    {
        static const Value_t val = fp_const_pi<Value_t>() * fp_const_preciseDouble<Value_t>(0.5);
        return val;
    }
    template<typename Value_t>
    inline Value_t fp_const_twopi() // CONSTANT_TWOPI
    {
        static const Value_t val = fp_const_pi<Value_t>() * Value_t(2);
        return val;
    }
    template<typename Value_t>
    inline Value_t fp_const_negativezero()
    {
        return -Epsilon<Value_t>::value;
    }
}
