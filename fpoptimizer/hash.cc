#include <list>
#include <algorithm>

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
            ConstantFolding();
        data->Sort();
        data->Recalculate_Hash_NoRecursion();
    }

    template<typename Value_t>
    void CodeTreeData<Value_t>::Recalculate_Hash_NoRecursion()
    {
        fphash_t NewHash = { Opcode * FPHASH_CONST(0x3A83A83A83A83A0),
                             Opcode * FPHASH_CONST(0x1131462E270012B)};
        Depth = 1;
        switch(Opcode)
        {
            case cImmed:
                /* FIXME: Do for other than double type */
                if(Value != 0.0)
                {
                    crc32_t crc = crc32::calc( (const unsigned char*) &Value,
                                                sizeof(Value) );
                    NewHash.hash1 ^= crc | (fphash_value_t(crc) << FPHASH_CONST(32));
                    NewHash.hash2 += ((~fphash_value_t(crc)) * 3) ^ 1234567;
                }
                break; // no params
            case VarBegin:
                NewHash.hash1 ^= (Var<<24) | (Var>>24);
                NewHash.hash2 += (fphash_value_t(Var)*5) ^ 2345678;
                break; // no params
            case cFCall: case cPCall:
            {
                crc32_t crc = crc32::calc( (const unsigned char*) &Funcno, sizeof(Funcno) );
                NewHash.hash1 ^= (crc<<24) | (crc>>24);
                NewHash.hash2 += ((~fphash_value_t(crc)) * 7) ^ 3456789;
                /* passthru */
            }
            default:
            {
                size_t MaxChildDepth = 0;
                for(size_t a=0; a<Params.size(); ++a)
                {
                    if(Params[a].GetDepth() > MaxChildDepth)
                        MaxChildDepth = Params[a].GetDepth();

                    NewHash.hash1 += (1)*FPHASH_CONST(0x2492492492492492);
                    NewHash.hash1 *= FPHASH_CONST(1099511628211);
                    //assert(&*Params[a] != this);
                    NewHash.hash1 += Params[a].GetHash().hash1;

                    NewHash.hash2 += (3)*FPHASH_CONST(0x9ABCD801357);
                    NewHash.hash2 *= FPHASH_CONST(0xECADB912345);
                    NewHash.hash2 += (~Params[a].GetHash().hash1) ^ 4567890;
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
#ifdef FP_SUPPORT_FLOAT_TYPE
    template void CodeTree<float>::Rehash(bool);
    template void CodeTree<float>::FixIncompleteHashes();
#endif
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
    template void CodeTree<long double>::Rehash(bool);
    template void CodeTree<long double>::FixIncompleteHashes();
#endif
}
/* END_EXPLICIT_INSTANTATION */

#endif
