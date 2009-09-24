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
        : RefCount(0), Opcode(), Params(), Hash(), Depth(1), Parent(), OptimizedUsing(0)
    {
    }

    CodeTree::CodeTree(double i)
        : RefCount(0), Opcode(cImmed), Params(), Hash(), Depth(1), Parent(), OptimizedUsing(0)
    {
        Value = i;
        Recalculate_Hash_NoRecursion();
    }

    CodeTree::~CodeTree()
    {
    }

    void CodeTree::Rehash(
        bool child_triggered)
    {
        /* If we were triggered by a parent, recurse to children */
        if(!child_triggered)
        {
            for(size_t a=0; a<Params.size(); ++a)
                Params[a]->Rehash(false);
        }

        Recalculate_Hash_NoRecursion();

        /* If we were triggered by a child, recurse to the parent */
        if(child_triggered)
            Rehash_Parents();
    }

    void CodeTree::Rehash_Parents()
    {
        if(Parent)
        {
            //assert(Parent->RefCount > 0);
            Parent->Rehash(true);
        }
    }

    struct ParamComparer
    {
        bool operator() (const CodeTreeP& a, const CodeTreeP& b) const
        {
            if(a->Depth != b->Depth)
                return a->Depth > b->Depth;
            return a->Hash < b->Hash;
        }
    };

    void CodeTree::Sort()
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

    void CodeTree::Sort_Recursive()
    {
        Sort();
        for(size_t a=0; a<Params.size(); ++a)
            Params[a]->Sort_Recursive();
        Recalculate_Hash_NoRecursion();
    }

    void CodeTree::Recalculate_Hash_NoRecursion()
    {
        fphash_t NewHash = { Opcode * FPHASH_CONST(0x3A83A83A83A83A0),
                             Opcode * FPHASH_CONST(0x1131462E270012B)};
        Depth = 1;
        switch(Opcode)
        {
            case cImmed:
            {
                if(Value != 0.0)
                {
                    crc32_t crc = crc32::calc( (const unsigned char*) &Value,
                                                sizeof(Value) );
                    NewHash.hash1 ^= crc | (fphash_value_t(crc) << FPHASH_CONST(32));
                    NewHash.hash2 += ((~fphash_value_t(crc)) * 3) ^ 1234567;
                }
                break; // no params
            }
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
                    if(Params[a]->Depth > MaxChildDepth)
                        MaxChildDepth = Params[a]->Depth;

                    NewHash.hash1 += (1)*FPHASH_CONST(0x2492492492492492);
                    NewHash.hash1 *= FPHASH_CONST(1099511628211);
                    //assert(&*Params[a] != this);
                    NewHash.hash1 += Params[a]->Hash.hash1;

                    NewHash.hash2 += (3)*FPHASH_CONST(0x9ABCD801357);
                    NewHash.hash2 *= FPHASH_CONST(0xECADB912345);
                    NewHash.hash2 += (~Params[a]->Hash.hash1) ^ 4567890;
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

    CodeTreeP CodeTree::Clone()
    {
        CodeTreeP result = new CodeTree;
        result->Become(*this, false, true);
        //assert(Parent->RefCount > 0);
        result->Parent = Parent;
        return result;
    }

    void CodeTree::AddParam(const CodeTreeP& param)
    {
        //std::cout << "AddParam called\n";
        param->Parent = this;
        Params.push_back(param);
    }
    void CodeTree::AddParamMove(CodeTreeP& param)
    {
        param->Parent = this;
        Params.push_back(CodeTreeP());
        Params.back().swap(param);
    }

    void CodeTree::SetParams(const std::vector<CodeTreeP>& RefParams, bool do_clone)
    {
        //std::cout << "SetParams called" << (do_clone ? ", clone" : ", no clone") << "\n";
        Params = RefParams;
        /**
        *** Note: The only reason we need to CLONE the children here
        ***       is because they must have the correct Parent field.
        ***       The Parent is required because of backward-recursive
        ***       hash regeneration. Is there any way around this?
        */

        for(size_t a=0; a<Params.size(); ++a)
        {
            if(do_clone) Params[a] = Params[a]->Clone();
            Params[a]->Parent = this;
        }
    }

    void CodeTree::SetParamsMove(std::vector<CodeTreeP>& RefParams)
    {
        Params.clear();
        Params.swap(RefParams);
        for(size_t a=0; a<Params.size(); ++a)
            Params[a]->Parent = this;
    }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
    void CodeTree::SetParams(std::vector<CodeTreeP>&& RefParams)
    {
        //std::cout << "SetParams&& called\n";
        SetParamsMove(RefParams);
    }
#endif

    void CodeTree::DelParam(size_t index)
    {
        //std::cout << "DelParam(" << index << ") called\n";
    #ifdef __GXX_EXPERIMENTAL_CXX0X__
        /* rvalue reference semantics makes this optimal */
        Params.erase( Params.begin() + index );
    #else
        /* This labor evades the need for refcount +1/-1 shuffling */
        Params[index] = 0;
        for(size_t p=index; p+1<Params.size(); ++p)
            Params[p].UnsafeSetP( &*Params[p+1] );
        Params[Params.size()-1].UnsafeSetP( 0 );
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
        switch( (OPCODE) Opcode)
        {
            case cImmed:
                return FloatEqual(Value, 0.0)
                    || FloatEqual(Value, 1.0);
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
                for(size_t a=0; a<Params.size(); ++a)
                    if(!Params[a]->IsLogicalValue())
                        return false;
                return true;
            case cIf:
                return Params[1]->IsLogicalValue()
                    && Params[2]->IsLogicalValue();
            default:
                break;
        }
        return false; // Not a logical value.
    }

    bool CodeTree::IsAlwaysInteger() const
    {
        switch( (OPCODE) Opcode)
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
                return Params[1]->IsAlwaysInteger()
                    && Params[2]->IsAlwaysInteger();
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
        if(Hash   != b.Hash) return false; // a quick catch-all
        if(Opcode != b.Opcode) return false;
        switch(Opcode)
        {
            case cImmed: if(Value != b.Value) return false; return true;
            case cVar:   if(Var   != b.Var)   return false; return true;
            case cFCall:
            case cPCall: if(Funcno != b.Funcno) return false; break;
            default: break;
        }
        if(Params.size() != b.Params.size()) return false;
        for(size_t a=0; a<Params.size(); ++a)
        {
            if(!Params[a]->IsIdenticalTo(*b.Params[a])) return false;
        }
        return true;
    }

    bool    CodeTree::IsImmed() const { return Opcode == cImmed; }
    bool    CodeTree::IsVar()   const { return Opcode == cVar; }

    void CodeTree::Become(CodeTree& b, bool thrash_original, bool do_clone)
    {
        //std::cout << "Become called\n";
        Opcode = b.Opcode;
        switch(Opcode)
        {
            case cVar:   Var   = b.Var; break;
            case cImmed: Value = b.Value; break;
            case cPCall:
            case cFCall: Funcno = b.Funcno; break;
            default: break;
        }
        if(thrash_original)
            SetParamsMove(b.Params);
        else
            SetParams(b.Params, do_clone);
        Hash  = b.Hash;
        Depth = b.Depth;
    }
}

#endif
