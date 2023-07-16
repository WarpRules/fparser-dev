#ifndef FPoptimizerHashHH
#define FPoptimizerHashHH

#ifdef _MSC_VER

typedef unsigned long long fphash_value_t;
#define FPHASH_CONST(x) x##ULL

#else

#include <stdint.h>
typedef uint_fast64_t fphash_value_t;
#define FPHASH_CONST(x) x##ULL

#endif

#include <utility>

namespace FUNCTIONPARSERTYPES
{
    /* We need a two-item tuple.
     * Using std::pair, because it provides == != < operators.
     */
    using fphash_t = std::pair<fphash_value_t, fphash_value_t>;
}

#endif
