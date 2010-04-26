#include <list>
#include <algorithm>

#include "codetree.hh"
#include "fptypes.hh"
#include "consts.hh"

#ifdef FP_SUPPORT_OPTIMIZER

using namespace FUNCTIONPARSERTYPES;
//using namespace FPoptimizer_Grammar;

namespace FPoptimizer_CodeTree
{
    template<typename Value_t>
    CodeTree<Value_t>::CodeTree()
        : data(new CodeTreeData<Value_t> ())
    {
        data->Opcode = cNop;
    }

    template<typename Value_t>
    CodeTree<Value_t>::CodeTree(const Value_t& i)
        : data(new CodeTreeData<Value_t>(i))
    {
        data->Recalculate_Hash_NoRecursion();
    }

    template<typename Value_t>
    CodeTree<Value_t>::CodeTree(unsigned v, CodeTree<Value_t>::VarTag)
        : data(new CodeTreeData<Value_t>)
    {
        data->Opcode = VarBegin;
        data->Var    = v;
        data->Recalculate_Hash_NoRecursion();
    }

    template<typename Value_t>
    CodeTree<Value_t>::CodeTree(const CodeTree<Value_t>& b, CodeTree<Value_t>::CloneTag)
        : data(new CodeTreeData<Value_t>(*b.data))
    {
    }

    template<typename Value_t>
    CodeTree<Value_t>::~CodeTree()
    {
    }

    template<typename Value_t>
    struct ParamComparer
    {
        bool operator() (const CodeTree<Value_t>& a, const CodeTree<Value_t>& b) const
        {
            if(a.GetDepth() != b.GetDepth())
                return a.GetDepth() > b.GetDepth();
            return a.GetHash() < b.GetHash();
        }
    };

    template<typename Value_t>
    void CodeTreeData<Value_t>::Sort()
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
            case cHypot:
            case cEqual:
            case cNEqual:
                std::sort(Params.begin(), Params.end(), ParamComparer<Value_t>());
                break;
            case cLess:
                if(ParamComparer<Value_t>() (Params[1], Params[0]))
                    { std::swap(Params[0], Params[1]); Opcode = cGreater; }
                break;
            case cLessOrEq:
                if(ParamComparer<Value_t>() (Params[1], Params[0]))
                    { std::swap(Params[0], Params[1]); Opcode = cGreaterOrEq; }
                break;
            case cGreater:
                if(ParamComparer<Value_t>() (Params[1], Params[0]))
                    { std::swap(Params[0], Params[1]); Opcode = cLess; }
                break;
            case cGreaterOrEq:
                if(ParamComparer<Value_t>() (Params[1], Params[0]))
                    { std::swap(Params[0], Params[1]); Opcode = cLessOrEq; }
                break;
            default:
                break;
        }
    }

    template<typename Value_t>
    void CodeTree<Value_t>::AddParam(const CodeTree<Value_t>& param)
    {
        //std::cout << "AddParam called\n";
        data->Params.push_back(param);
    }
    template<typename Value_t>
    void CodeTree<Value_t>::AddParamMove(CodeTree<Value_t>& param)
    {
        data->Params.push_back(CodeTree<Value_t>());
        data->Params.back().swap(param);
    }
    template<typename Value_t>
    void CodeTree<Value_t>::SetParam(size_t which, const CodeTree<Value_t>& b)
    {
        DataP slot_holder ( data->Params[which].data );
        data->Params[which] = b;
    }
    template<typename Value_t>
    void CodeTree<Value_t>::SetParamMove(size_t which, CodeTree<Value_t>& b)
    {
        DataP slot_holder ( data->Params[which].data );
        data->Params[which].swap(b);
    }

    template<typename Value_t>
    void CodeTree<Value_t>::AddParams(const std::vector<CodeTree<Value_t> >& RefParams)
    {
        data->Params.insert(data->Params.end(), RefParams.begin(), RefParams.end());
    }
    template<typename Value_t>
    void CodeTree<Value_t>::AddParamsMove(std::vector<CodeTree<Value_t> >& RefParams)
    {
        size_t endpos = data->Params.size(), added = RefParams.size();
        data->Params.resize(endpos + added, CodeTree<Value_t>());
        for(size_t p=0; p<added; ++p)
            data->Params[endpos+p].swap( RefParams[p] );
    }
    template<typename Value_t>
    void CodeTree<Value_t>::AddParamsMove(std::vector<CodeTree<Value_t> >& RefParams, size_t replacing_slot)
    {
        DataP slot_holder ( data->Params[replacing_slot].data );
        DelParam(replacing_slot);
        AddParamsMove(RefParams);
    /*
        const size_t n_added = RefParams.size();
        const size_t oldsize = data->Params.size();
        const size_t newsize = oldsize + n_added - 1;
        if(RefParams.empty())
            DelParam(replacing_slot);
        else
        {
            //    0 1 2 3 4 5 6 7 8 9 10 11
            //    a a a a X b b b b b
            //    a a a a Y Y Y b b b b  b
            //
            //   replacing_slot = 4
            //   n_added = 3
            //   oldsize = 10
            //   newsize = 12
            //   tail_length = 5

            data->Params.resize(newsize);
            data->Params[replacing_slot].data = 0;
            const size_t tail_length = oldsize - replacing_slot -1;
            for(size_t tail=0; tail<tail_length; ++tail)
                data->Params[newsize-1-tail].data.UnsafeSetP(
                &*data->Params[newsize-1-tail-(n_added-1)].data);
            for(size_t head=1; head<n_added; ++head)
                data->Params[replacing_slot+head].data.UnsafeSetP( 0 );
            for(size_t p=0; p<n_added; ++p)
                data->Params[replacing_slot+p].swap( RefParams[p] );
        }
    */
    }

    template<typename Value_t>
    void CodeTree<Value_t>::SetParams(const std::vector<CodeTree<Value_t> >& RefParams)
    {
        //std::cout << "SetParams called" << (do_clone ? ", clone" : ", no clone") << "\n";
        std::vector<CodeTree<Value_t> > tmp(RefParams);
        data->Params.swap(tmp);
    }

    template<typename Value_t>
    void CodeTree<Value_t>::SetParamsMove(std::vector<CodeTree<Value_t> >& RefParams)
    {
        data->Params.swap(RefParams);
        RefParams.clear();
    }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
    template<typename Value_t>
    void CodeTree<Value_t>::SetParams(std::vector<CodeTree<Value_t> >&& RefParams)
    {
        //std::cout << "SetParams&& called\n";
        SetParamsMove(RefParams);
    }
#endif

    template<typename Value_t>
    void CodeTree<Value_t>::DelParam(size_t index)
    {
        std::vector<CodeTree<Value_t> >& Params = data->Params;
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

    template<typename Value_t>
    void CodeTree<Value_t>::DelParams()
    {
        data->Params.clear();
    }

    /* Is the value of this tree definitely odd(true) or even(false)? */
    template<typename Value_t>
    typename CodeTree<Value_t>::TriTruthValue
        CodeTree<Value_t>::GetEvennessInfo() const
    {
        if(!IsImmed()) return Unknown;
        if(!IsLongIntegerImmed()) return Unknown;
        return (GetLongIntegerImmed() & 1) ? IsNever : IsAlways;
    }

    template<typename Value_t>
    bool CodeTree<Value_t>::IsLogicalValue() const
    {
        switch(data->Opcode)
        {
            case cImmed:
                return FloatEqual(data->Value, Value_t(0))
                    || FloatEqual(data->Value, Value_t(1));
            case cAnd:
            case cOr:
            case cNot:
            case cNotNot:
            case cAbsAnd:
            case cAbsOr:
            case cAbsNot:
            case cAbsNotNot:
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
                std::vector<CodeTree<Value_t> >& Params = data->Params;
                for(size_t a=0; a<Params.size(); ++a)
                    if(!Params[a].IsLogicalValue())
                        return false;
                return true;
            }
            case cIf:
            case cAbsIf:
            {
                std::vector<CodeTree<Value_t> >& Params = data->Params;
                return Params[1].IsLogicalValue()
                    && Params[2].IsLogicalValue();
            }
            default:
                break;
        }
        return false; // Not a logical value.
    }

    template<typename Value_t>
    bool CodeTree<Value_t>::IsAlwaysInteger(bool integer) const
    {
        switch(data->Opcode)
        {
            case cImmed:
                return IsLongIntegerImmed() ? integer==true : integer==false;
            case cFloor:
            case cInt:
                return integer==true;
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
                return integer==true; /* 0 and 1 are both integers */
            case cIf:
            {
                std::vector<CodeTree<Value_t> >& Params = data->Params;
                return Params[1].IsAlwaysInteger(integer)
                    && Params[2].IsAlwaysInteger(integer);
                return true; /* 0 and 1 are both integers */
            }
            case cAdd:
            case cMul:
            {
                for(size_t a=GetParamCount(); a-- > 0; )
                    if(!GetParam(a).IsAlwaysInteger(integer))
                        return false;
                return true;
            }
            default:
                break;
        }
        return false; /* Don't know whether it's integer. */
    }

    template<typename Value_t>
    bool CodeTree<Value_t>::IsAlwaysSigned(bool positive) const
    {
        MinMaxTree<Value_t> tmp = CalculateResultBoundaries();

        if(positive)
            return tmp.has_min && tmp.min >= 0.0
              && (!tmp.has_max || tmp.max >= 0.0);
        else
            return tmp.has_max && tmp.max < 0.0
              && (!tmp.has_min || tmp.min < 0.0);
    }

    template<typename Value_t>
    bool CodeTree<Value_t>::IsIdenticalTo(const CodeTree<Value_t>& b) const
    {
        //if((!&*data) != (!&*b.data)) return false;
        if(&*data == &*b.data) return true;
        return data->IsIdenticalTo(*b.data);
    }

    template<typename Value_t>
    bool CodeTreeData<Value_t>::IsIdenticalTo(const CodeTreeData<Value_t>& b) const
    {
        if(Hash   != b.Hash) return false; // a quick catch-all
        if(Opcode != b.Opcode) return false;
        switch(Opcode)
        {
            case cImmed:   return FloatEqual(Value, b.Value);
            case VarBegin: return Var == b.Var;
            case cFCall:
            case cPCall:   if(Funcno != b.Funcno) return false; break;
            default: break;
        }
        if(Params.size() != b.Params.size()) return false;
        for(size_t a=0; a<Params.size(); ++a)
        {
            if(!Params[a].IsIdenticalTo(b.Params[a])) return false;
        }
        return true;
    }

    template<typename Value_t>
    void CodeTree<Value_t>::Become(const CodeTree<Value_t>& b)
    {
        if(&b != this && &*data != &*b.data)
        {
            DataP tmp = b.data;
            CopyOnWrite();
            data.swap(tmp);
        }
    }

    template<typename Value_t>
    void CodeTree<Value_t>::CopyOnWrite()
    {
        if(data->RefCount > 1)
            data = new CodeTreeData<Value_t>(*data);
    }

    template<typename Value_t>
    CodeTree<Value_t> CodeTree<Value_t>::GetUniqueRef()
    {
        if(data->RefCount > 1)
            return CodeTree<Value_t>(*this, CloneTag());
        return *this;
    }

    template<typename Value_t>
    CodeTreeData<Value_t>::CodeTreeData()
        : RefCount(0),
          Opcode(cNop), Params(), Hash(), Depth(1), OptimizedUsing(0)
    {
    }

    template<typename Value_t>
    CodeTreeData<Value_t>::CodeTreeData(const CodeTreeData& b)
        : RefCount(0),
          Opcode(b.Opcode),
          Params(b.Params),
          Hash(b.Hash),
          Depth(b.Depth),
          OptimizedUsing(b.OptimizedUsing)
    {
        switch(Opcode)
        {
            case VarBegin: Var   = b.Var; break;
            case cImmed:   Value = b.Value; break;
            case cPCall:
            case cFCall:   Funcno = b.Funcno; break;
            default: break;
        }
    }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
    template<typename Value_t>
    CodeTreeData<Value_t>::CodeTreeData(CodeTreeData<Value_t>&& b)
        : RefCount(0),
          Opcode(b.Opcode),
          Params(b.Params),
          Hash(b.Hash),
          Depth(b.Depth),
          OptimizedUsing(b.OptimizedUsing)
    {
        switch(Opcode)
        {
            case VarBegin: Var   = b.Var; break;
            case cImmed:   Value = b.Value; break;
            case cPCall:
            case cFCall:   Funcno = b.Funcno; break;
            default: break;
        }
    }
#endif

    template<typename Value_t>
    CodeTreeData<Value_t>::CodeTreeData(const Value_t& i)
        : RefCount(0), Opcode(cImmed), Params(), Hash(), Depth(1), OptimizedUsing(0)
    {
        Value = i;
    }
}

/* BEGIN_EXPLICIT_INSTANTATION */
namespace FPoptimizer_CodeTree
{
    template class CodeTree<double>;
    template class CodeTreeData<double>;
#ifdef FP_SUPPORT_FLOAT_TYPE
    template class CodeTree<float>;
    template class CodeTreeData<float>;
#endif
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
    template class CodeTree<long double>;
    template class CodeTreeData<long double>;
#endif
}
/* END_EXPLICIT_INSTANTATION */

#endif

