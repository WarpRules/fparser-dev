#include <list>
#include <algorithm>

#include "constantfolding.hh"
#include "codetree.hh"
#include "fptypes.hh"
#include "../lib/crc32.hh"

#ifdef FP_SUPPORT_OPTIMIZER

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
    void CodeTree<Value_t>::Rehash(bool constantfolding)
    {
        if(constantfolding)
        {
            ConstantFolding(*this);
        }
        data->Sort();
        data->Recalculate_Hash_NoRecursion();
    }

    template<typename Value_t>
    void CodeTreeData<Value_t>::Recalculate_Hash_NoRecursion()
    {
        /* Hash structure:
         *     hash1: sorting key (8 bytes, 64 bits)
         *              byte 1: opcode
         *     hash2: unique value
         */
        fphash_t NewHash ( fphash_value_t(Opcode) << 56,
                           Opcode * FPHASH_CONST(0x1131462E270012B) );
        Depth = 1;
        switch(Opcode)
        {
            case cImmed:              // Value
            {
                /* TODO: For non-POD types, convert the value
                 * into a base-62 string (or something) and hash that.
                 */
              #if 0
                long double value = Value;
                fphash_value_t key = crc32::calc((const unsigned char*)&value, sizeof(value));
                key |= (key << 24);
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
                if(fraction < 0) { fraction = -fraction; key = ~key; } else key += 0x10000;
                fraction -= Value_t(0.5);
                key <<= 40;
                key |= fphash_value_t((fraction+fraction) * Value_t(1u<<31)) << 8;
              #endif
                /* Key = 56-bit unsigned integer value
                 *       that is directly proportional
                 *       to the floating point value.
                 */
                NewHash.hash1 |= key;
                //crc32_t crc = crc32::calc((const unsigned char*)&Value, sizeof(Value));
                fphash_value_t crc = (key >> 10) | (key << (64-10));
                NewHash.hash2 += ((~fphash_value_t(crc)) * 3) ^ 1234567;
                break; // no params
            }
            case VarBegin:            // Var_or_Funcno
            {
                NewHash.hash1 |= fphash_value_t(Var_or_Funcno) << 48;
                NewHash.hash2 += ((fphash_value_t(Var_or_Funcno)) * 11)
                                   ^ FPHASH_CONST(0x3A83A83A83A83A0);
                break; // no params
            }
            case cFCall: case cPCall: // Var_or_Funcno
            {
                NewHash.hash1 |= fphash_value_t(Var_or_Funcno) << 48;
                NewHash.hash2 += ((~fphash_value_t(Var_or_Funcno)) * 7) ^ 3456789;
                /* passthru */
            }
            default:
            {
                size_t MaxChildDepth = 0;
                for(size_t a=0; a<Params.size(); ++a)
                {
                    if(Params[a].GetDepth() > MaxChildDepth)
                        MaxChildDepth = Params[a].GetDepth();

                    NewHash.hash1 += ((Params[a].GetHash().hash1*(a+1)) >> 12);
                    NewHash.hash2 += Params[a].GetHash().hash1;
                    NewHash.hash2 += (3)*FPHASH_CONST(0x9ABCD801357);
                    NewHash.hash2 *= FPHASH_CONST(0xECADB912345);
                    NewHash.hash2 += (~Params[a].GetHash().hash2) ^ 4567890;
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
namespace FPoptimizer_CodeTree
{
    template void CodeTree<double>::Rehash(bool);
    template void CodeTree<double>::FixIncompleteHashes();
    template void CodeTreeData<double>::Recalculate_Hash_NoRecursion();
#ifdef FP_SUPPORT_FLOAT_TYPE
    template void CodeTree<float>::Rehash(bool);
    template void CodeTree<float>::FixIncompleteHashes();
    template void CodeTreeData<float>::Recalculate_Hash_NoRecursion();
#endif
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
    template void CodeTree<long double>::Rehash(bool);
    template void CodeTree<long double>::FixIncompleteHashes();
    template void CodeTreeData<long double>::Recalculate_Hash_NoRecursion();
#endif
}
/* END_EXPLICIT_INSTANTATION */

#endif
