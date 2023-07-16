#include <list>
#include <bitset>
#include <algorithm>

#include "constantfolding.hh"
#include "codetree.hh"
#include "extrasrc/fptypes.hh"


#ifdef FP_SUPPORT_OPTIMIZER


#ifdef FP_SUPPORT_GMP_INT_TYPE
# include <gmp.h>
#endif
#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
# include <mpfr.h>
#endif

using namespace FUNCTIONPARSERTYPES;
//using namespace FPoptimizer_Grammar;

namespace
{
    template<typename Value_t>
    bool MarkIncompletes(FPoptimizer_CodeTree::CodeTree<Value_t>& tree)
    {
        if(tree.Is_Incompletely_Hashed())
            return true;

        bool needs_rehash = false;
        for(size_t a=0; a<tree.GetParamCount(); ++a)
            needs_rehash |= MarkIncompletes(tree.GetParam(a));
        if(needs_rehash)
            tree.Mark_Incompletely_Hashed();
        return needs_rehash;
    }

    template<typename Value_t>
    void FixIncompletes(FPoptimizer_CodeTree::CodeTree<Value_t>& tree)
    {
        if(tree.Is_Incompletely_Hashed())
        {
            for(size_t a=0; a<tree.GetParamCount(); ++a)
                FixIncompletes(tree.GetParam(a));
            tree.Rehash();
        }
    }
}

namespace FPoptimizer_CodeTree
{
    template<typename Value_t>
    void CodeTree<Value_t>::Sort()
    {
        data->Sort();
    }

    template<typename Value_t>
    void CodeTree<Value_t>::Rehash(bool constantfolding)
    {
        if(constantfolding)
            ConstantFolding(*this); // also runs Sort()
        else
            Sort();

        data->Recalculate_Hash_NoRecursion();
    }

    template<typename Value_t>
    struct ImmedHashGenerator
    {
        static void MakeHash(
            FUNCTIONPARSERTYPES::fphash_t& NewHash,
            const Value_t& Value)
        {
            /* TODO: For non-POD types, convert the value
             * into a base-62 string (or something) and hash that.
             */
            NewHash.first = 0; // Try to ensure immeds gets always sorted first
          #if 0
            long double value = Value;
            fphash_value_t key = crc32::calc((const unsigned char*)&value, sizeof(value));
            key ^= (key << 24);
          #elif 0
            union
            {
                struct
                {
                    unsigned char filler1[16];
                    Value_t       v;
                    unsigned char filler2[16];
                } buf2;
                struct
                {
                    unsigned char filler3[sizeof(Value_t)+16-sizeof(fphash_value_t)];
                    fphash_value_t key;
                } buf1;
            } data;
            memset(&data, 0, sizeof(data));
            data.buf2.v = Value;
            fphash_value_t key = data.buf1.key;
          #else
            int exponent;
            Value_t fraction = std::frexp(Value, &exponent);
            fphash_value_t key = (unsigned(exponent+0x8000) & 0xFFFF);
            if(fraction < 0)
                { fraction = -fraction; key = key^0xFFFF; }
            else
                key += 0x10000;
            fraction -= Value_t(0.5);
            key <<= 39; // covers bits 39..55 now
            key |= fphash_value_t((fraction+fraction) * Value_t(1u<<31)) << 8;
            // fraction covers bits 8..39 now
          #endif
            /* Key = 56-bit unsigned integer value
             *       that is directly proportional
             *       to the floating point value.
             */
            NewHash.first |= key;
            //crc32_t crc = crc32::calc((const unsigned char*)&Value, sizeof(Value));
            fphash_value_t crc = (key >> 10) | (key << (64-10));
            NewHash.second += ((~fphash_value_t(crc)) * 3) ^ 1234567;
        }
    };

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    template<>
    struct ImmedHashGenerator<MpfrFloat>
    {
        static void MakeHash(
            FUNCTIONPARSERTYPES::fphash_t& NewHash,
            const MpfrFloat& Value)
        {
            mpfr_t raw;
            (const_cast<MpfrFloat&>(Value)).get_raw_mpfr_data(raw);
            const mp_limb_t* data = raw->_mpfr_d;
            NewHash.first  =  raw->_mpfr_exp;
            NewHash.first  += (long)(raw->_mpfr_sign) << 32;
            NewHash.first  += (long)(raw->_mpfr_prec) << 48;
            NewHash.second  = raw->_mpfr_prec;
            NewHash.second += (long)(raw->_mpfr_exp)  << 24;
            NewHash.second += (long)(raw->_mpfr_sign) << 56;
            int num = raw->_mpfr_prec;
            for(int n=0; n<num; ++n)
            {
                NewHash.first = NewHash.first * 11400714819323198485ul + data[n];
                NewHash.second ^= NewHash.first;
            }
        }
    };
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    struct ImmedHashGenerator< std::complex<T> >
    {
        static void MakeHash(
            FUNCTIONPARSERTYPES::fphash_t& NewHash,
            const std::complex<T>& Value)
        {
            ImmedHashGenerator<T>::MakeHash(NewHash, Value.real());
            FUNCTIONPARSERTYPES::fphash_t temp;
            ImmedHashGenerator<T>::MakeHash(temp, Value.imag());
            NewHash.first ^= temp.second;
            NewHash.second ^= temp.first;
        }
    };
#endif

#ifdef FP_SUPPORT_LONG_INT_TYPE
    template<>
    struct ImmedHashGenerator<long>
    {
        static void MakeHash(
            FUNCTIONPARSERTYPES::fphash_t& NewHash,
            long Value)
        {
            fphash_value_t key = Value;
            /* Key = 56-bit unsigned integer value
             *       that is directly proportional
             *       to the floating point value.
             */
            NewHash.first |= key;
            //crc32_t crc = crc32::calc((const unsigned char*)&Value, sizeof(Value));
            fphash_value_t crc = (key >> 10) | (key << (64-10));
            NewHash.second += ((~fphash_value_t(crc)) * 3) ^ 1234567;
        }
    };
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    template<>
    struct ImmedHashGenerator<GmpInt>
    {
        static void MakeHash(
            FUNCTIONPARSERTYPES::fphash_t& NewHash,
            const GmpInt& Value)
        {
            mpz_t raw;
            (const_cast<GmpInt&>(Value)).get_raw_mpfr_data(raw);
            const mp_limb_t* data = raw->_mp_d;
            const int        num  = raw->_mp_size;
            NewHash.first = 0;
            NewHash.second = num;
            for(int n=0; n<num; ++n)
            {
                NewHash.first = NewHash.first * 11400714819323198485ul + data[n];
                NewHash.second ^= NewHash.first;
            }
        }
    };
#endif

    template<typename Value_t>
    void CodeTreeData<Value_t>::Recalculate_Hash_NoRecursion()
    {
        /* Hash structure:
         *     first: sorting key (8 bytes, 64 bits)
         *              byte 1: opcode
         *     second: unique value
         */
        fphash_t NewHash ( fphash_value_t(Opcode) << 56,
                           Opcode * FPHASH_CONST(0x1131462E270012B) );
        Depth = 1;
        switch(Opcode)
        {
            case cImmed:              // Value
            {
                ImmedHashGenerator<Value_t>::MakeHash(NewHash, Value);
                break; // no params
            }
            case VarBegin:            // Var_or_Funcno
            {
                NewHash.first |= fphash_value_t(Var_or_Funcno) << 48;
                NewHash.second += ((fphash_value_t(Var_or_Funcno)) * 11)
                                   ^ FPHASH_CONST(0x3A83A83A83A83A0);
                break; // no params
            }
            case cFCall: case cPCall: // Var_or_Funcno
            {
                NewHash.first |= fphash_value_t(Var_or_Funcno) << 48;
                NewHash.second += ((~fphash_value_t(Var_or_Funcno)) * 7) ^ 3456789;
                /* passthru */
                goto dfl; // suppress a warning
                /* [[fallthrough]]; */ //c++17 only
            }
            default:
            {
            dfl:;
                size_t MaxChildDepth = 0;
                for(size_t a=0; a<Params.size(); ++a)
                {
                    if(Params[a].GetDepth() > MaxChildDepth)
                        MaxChildDepth = Params[a].GetDepth();

                    NewHash.first += ((Params[a].GetHash().first*(a+1)) >> 12);
                    NewHash.second += Params[a].GetHash().first;
                    NewHash.second += (3)*FPHASH_CONST(0x9ABCD801357);
                    NewHash.second *= FPHASH_CONST(0xECADB912345);
                    NewHash.second += (~Params[a].GetHash().second) ^ 4567890;
                }
                Depth += MaxChildDepth;
            }
        }
        if(Hash != NewHash)
        {
            Hash = NewHash;
            OptimizedUsing = 0;
        }
    }

    template<typename Value_t>
    void CodeTree<Value_t>::FixIncompleteHashes()
    {
        MarkIncompletes(*this);
        FixIncompletes(*this);
    }
}

/* BEGIN_EXPLICIT_INSTANTATION */
#include "instantiate.hh"
namespace FPoptimizer_CodeTree
{
#define FP_INSTANTIATE(type) \
    template void CodeTree<type>::Sort(); \
    template void CodeTree<type>::Rehash(bool); \
    template void CodeTree<type>::FixIncompleteHashes(); \
    template void CodeTreeData<type>::Recalculate_Hash_NoRecursion();
    FPOPTIMIZER_EXPLICITLY_INSTANTIATE(FP_INSTANTIATE)
#undef FP_INSTANTIATE
}
/* END_EXPLICIT_INSTANTATION */

#endif
