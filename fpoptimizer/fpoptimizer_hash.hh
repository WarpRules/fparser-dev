#ifdef _MSC_VER

typedef unsigned long long fphash_t;
#define FPHASH_CONST(x) x##ULL

#else

#include <stdint.h>
typedef uint_fast64_t fphash_t;
#define FPHASH_CONST(x) x##ULL

#endif
