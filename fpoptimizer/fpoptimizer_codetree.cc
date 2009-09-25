#include <list>
#include <algorithm>

#include "fpoptimizer_codetree.hh"
#include "fptypes.hh"
#include "crc32.hh"
#include "fpoptimizer_consts.hh"

#ifdef FP_SUPPORT_OPTIMIZER

using namespace FUNCTIONPARSERTYPES;
//using namespace FPoptimizer_Grammar;

namespace FPoptimizer_CodeTree
{
    CodeTree::CodeTree()
        : data()
    {
    }

    CodeTree::CodeTree(double i)
        : data(new CodeTreeData(i))
    {
        Recalculate_Hash_NoRecursion();
    }

    CodeTree::~CodeTree()
    {
    }

    struct ParamComparer
    {
        bool operator() (const CodeTree& a, const CodeTree& b) const
        {
            if(a.GetDepth() != b.GetDepth())
                return a.GetDepth() > b.GetDepth();
            return a.GetHash() < b.GetHash();
        }
    };

    void CodeTreeData::Sort()
    {
        /* If the tree is commutative, order the parameters
         * in a set order in order to make equality tests
         * efficient in the optimizer
         */
        switch(Opcode)
        {
            case cAdd:
            case cMul:
            case cMin:
            case cMax:
            case cAnd:
            case cOr:
            case cEqual:
            case cNEqual:
                std::sort(Params.begin(), Params.end(), ParamComparer());
                break;
            case cLess:
                if(ParamComparer() (Params[1], Params[0]))
                    { std::swap(Params[0], Params[1]); Opcode = cGreater; }
                break;
            case cLessOrEq:
                if(ParamComparer() (Params[1], Params[0]))
                    { std::swap(Params[0], Params[1]); Opcode = cGreaterOrEq; }
                break;
            case cGreater:
                if(ParamComparer() (Params[1], Params[0]))
                    { std::swap(Params[0], Params[1]); Opcode = cLess; }
                break;
            case cGreaterOrEq:
                if(ParamComparer() (Params[1], Params[0]))
                    { std::swap(Params[0], Params[1]); Opcode = cLessOrEq; }
                break;
            default:
                break;
        }
    }

    void CodeTreeData::Recalculate_Hash_NoRecursion()
    {
        fphash_t NewHash = { Opcode * FPHASH_CONST(0x3A83A83A83A83A0),
                             Opcode * FPHASH_CONST(0x1131462E270012B)};
        Depth = 1;
        switch(Opcode)
        {
            case cImmed:
                if(Value != 0.0)
                {
                    crc32_t crc = crc32::calc( (const unsigned char*) &Value,
                                                sizeof(Value) );
                    NewHash.hash1 ^= crc | (fphash_value_t(crc) << FPHASH_CONST(32));
                    NewHash.hash2 += ((~fphash_value_t(crc)) * 3) ^ 1234567;
                }
                break; // no params
            case cVar:
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

    void CodeTree::AddParam(const CodeTree& param)
    {
        //std::cout << "AddParam called\n";
        data->Params.push_back(param);
    }
    void CodeTree::AddParamMove(CodeTree& param)
    {
        data->Params.push_back(CodeTree());
        data->Params.back().swap(param);
    }
    void CodeTree::SetParam(size_t which, const CodeTree& b)
    {
        data->Params[which] = b;
    }
    void CodeTree::SetParamMove(size_t which, CodeTree& b)
    {
        data->Params[which].swap(b);
    }

    void CodeTree::AddParams(const std::vector<CodeTree>& RefParams)
    {
        data->Params.insert(data->Params.end(), RefParams.begin(), RefParams.end());
    }
    void CodeTree::AddParamsMove(std::vector<CodeTree>& RefParams)
    {
        size_t endpos = data->Params.size(), added = RefParams.size();
        data->Params.resize(endpos + added, CodeTree());
        for(size_t p=0; p<added; ++p)
            data->Params[endpos+p].swap( RefParams[p] );
    }

    void CodeTree::SetParams(const std::vector<CodeTree>& RefParams)
    {
        //std::cout << "SetParams called" << (do_clone ? ", clone" : ", no clone") << "\n";
        data->Params = RefParams;
    }

    void CodeTree::SetParamsMove(std::vector<CodeTree>& RefParams)
    {
        data->Params.clear();
        data->Params.swap(RefParams);
    }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
    void CodeTree::SetParams(std::vector<CodeTree>&& RefParams)
    {
        //std::cout << "SetParams&& called\n";
        SetParamsMove(RefParams);
    }
#endif

    void CodeTree::DelParam(size_t index)
    {
        std::vector<CodeTree>& Params = data->Params;
        //std::cout << "DelParam(" << index << ") called\n";
    #ifdef __GXX_EXPERIMENTAL_CXX0X__
        /* rvalue reference semantics makes this optimal */
        Params.erase( Params.begin() + index );
    #else
        /* This labor evades the need for refcount +1/-1 shuffling */
        Params[index].data = 0;
        for(size_t p=index; p+1<Params.size(); ++p)
            Params[p].data.UnsafeSetP( &*Params[p+1].data );
        Params[Params.size()-1].data.UnsafeSetP( 0 );
        Params.resize(Params.size()-1);
    #endif
    }

    /* Is the value of this tree definitely odd(true) or even(false)? */
    CodeTree::TriTruthValue CodeTree::GetEvennessInfo() const
    {
        if(!IsImmed()) return Unknown;
        if(!IsLongIntegerImmed()) return Unknown;
        return (GetLongIntegerImmed() & 1) ? IsNever : IsAlways;
    }

    bool CodeTree::IsLogicalValue() const
    {
        switch(data->Opcode)
        {
            case cImmed:
                return FloatEqual(data->Value, 0.0)
                    || FloatEqual(data->Value, 1.0);
            case cAnd:
            case cOr:
            case cNot:
            case cNotNot:
            case cEqual:
            case cNEqual:
            case cLess:
            case cLessOrEq:
            case cGreater:
            case cGreaterOrEq:
                /* These operations always produce truth values (0 or 1) */
                return true;
            case cMul:
            {
                std::vector<CodeTree>& Params = data->Params;
                for(size_t a=0; a<Params.size(); ++a)
                    if(!Params[a].IsLogicalValue())
                        return false;
                return true;
            }
            case cIf:
            {
                std::vector<CodeTree>& Params = data->Params;
                return Params[1].IsLogicalValue()
                    && Params[2].IsLogicalValue();
            }
            default:
                break;
        }
        return false; // Not a logical value.
    }

    bool CodeTree::IsAlwaysInteger() const
    {
        switch(data->Opcode)
        {
            case cImmed:
                return IsLongIntegerImmed();
            case cFloor:
            case cInt:
                return true;
            case cAnd:
            case cOr:
            case cNot:
            case cNotNot:
            case cEqual:
            case cNEqual:
            case cLess:
            case cLessOrEq:
            case cGreater:
            case cGreaterOrEq:
                /* These operations always produce truth values (0 or 1) */
                return true; /* 0 and 1 are both integers */
            case cIf:
            {
                std::vector<CodeTree>& Params = data->Params;
                return Params[1].IsAlwaysInteger()
                    && Params[2].IsAlwaysInteger();
            }
            default:
                break;
        }
        return false; /* Don't know whether it's integer. */
    }

    bool CodeTree::IsAlwaysSigned(bool positive) const
    {
        MinMaxTree tmp = CalculateResultBoundaries();

        if(positive)
            return tmp.has_min && tmp.min >= 0.0
              && (!tmp.has_max || tmp.max >= 0.0);
        else
            return tmp.has_max && tmp.max < 0.0
              && (!tmp.has_min || tmp.min < 0.0);
    }

    bool CodeTree::IsIdenticalTo(const CodeTree& b) const
    {
        if((!&*data) != (!&*b.data)) return false;
        if(&*data == &*b.data) return true;
        return data->IsIdenticalTo(*b.data);
    }

    bool CodeTreeData::IsIdenticalTo(const CodeTreeData& b) const
    {
        if(Hash   != b.Hash) return false; // a quick catch-all
        if(Opcode != b.Opcode) return false;
        switch(Opcode)
        {
            case cImmed: return FloatEqual(Value, b.Value);
            case cVar:   return Var == b.Var;
            case cFCall:
            case cPCall: if(Funcno != b.Funcno) return false; break;
            default: break;
        }
        if(Params.size() != b.Params.size()) return false;
        for(size_t a=0; a<Params.size(); ++a)
        {
            if(!Params[a].IsIdenticalTo(b.Params[a])) return false;
        }
        return true;
    }

    void CodeTree::Become(const CodeTree& b)
    {
        if(&b != this && &*data != &*b.data)
        {
            DataP tmp = b.data;
            CopyOnWrite();
            data.swap(tmp);
        }
    }

    void CodeTree::CopyOnWrite()
    {
        if(!&*data)
            data = new CodeTreeData;
        else if(data->RefCount > 1)
            data = new CodeTreeData(*data);
    }

    CodeTreeData::CodeTreeData()
        : RefCount(0),
          Opcode(), Params(), Hash(), Depth(1), OptimizedUsing(0)
    {
    }

    CodeTreeData::CodeTreeData(const CodeTreeData& b)
        : RefCount(0),
          Opcode(b.Opcode),
          Params(b.Params),
          Hash(b.Hash),
          Depth(b.Depth),
          OptimizedUsing(b.OptimizedUsing)
    {
        switch(Opcode)
        {
            case cVar:   Var   = b.Var; break;
            case cImmed: Value = b.Value; break;
            case cPCall:
            case cFCall: Funcno = b.Funcno; break;
            default: break;
        }
    }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
    CodeTreeData::CodeTreeData(CodeTreeData&& b)
        : RefCount(0),
          Opcode(b.Opcode),
          Params(b.Params),
          Hash(b.Hash),
          Depth(b.Depth),
          OptimizedUsing(b.OptimizedUsing)
    {
        switch(Opcode)
        {
            case cVar:   Var   = b.Var; break;
            case cImmed: Value = b.Value; break;
            case cPCall:
            case cFCall: Funcno = b.Funcno; break;
            default: break;
        }
    }
#endif

    CodeTreeData::CodeTreeData(double i)
        : RefCount(0), Opcode(cImmed), Params(), Hash(), Depth(1), OptimizedUsing(0)
    {
        Value = i;
    }
}

#endif
