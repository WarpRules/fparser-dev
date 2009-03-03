#include <cmath>
#include <list>
#include <algorithm>

#include "fpoptimizer_codetree.hh"
#include "fptypes.hh"

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
                Params[a].param->Rehash(false);
        }

        Recalculate_Hash_NoRecursion();

        /* If we were triggered by a child, recurse to the parent */
        if(child_triggered && Parent)
        {
            //assert(Parent->RefCount > 0);
            Parent->Rehash(true);
        }
    }

    struct ParamComparer
    {
        bool operator() (const CodeTree::Param& a, const CodeTree::Param& b) const
        {
            if(a.param->Depth != b.param->Depth)
                return a.param->Depth > b.param->Depth;
            if(a.sign != b.sign) return a.sign < b.sign;
            return a.param->Hash < b.param->Hash;
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
        }
    }

    void CodeTree::Sort_Recursive()
    {
        Sort();
        for(size_t a=0; a<Params.size(); ++a)
            Params[a].param->Sort_Recursive();
        Recalculate_Hash_NoRecursion();
    }

    void CodeTree::Recalculate_Hash_NoRecursion()
    {
        fphash_t NewHash = Opcode * FPHASH_CONST(0x3A83A83A83A83A0);
        Depth = 1;
        switch(Opcode)
        {
            case cImmed:
                // FIXME: not portable - we're casting double* into uint_least64_t*
                if(Value != 0.0)
                    NewHash ^= *(fphash_t*)&Value;
                break; // no params
            case cVar:
                NewHash ^= (Var<<24) | (Var>>24);
                break; // no params
            case cFCall: case cPCall:
                NewHash ^= (Funcno<<24) | (Funcno>>24);
                /* passthru */
            default:
            {
                size_t MaxChildDepth = 0;
                for(size_t a=0; a<Params.size(); ++a)
                {
                    if(Params[a].param->Depth > MaxChildDepth)
                        MaxChildDepth = Params[a].param->Depth;

                    NewHash += (1+Params[a].sign)*FPHASH_CONST(0x2492492492492492);
                    NewHash *= FPHASH_CONST(1099511628211);
                    //assert(&*Params[a].param != this);
                    NewHash += Params[a].param->Hash;
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

    CodeTree* CodeTree::Clone()
    {
        CodeTree* result = new CodeTree;
        result->Opcode = Opcode;
        switch(Opcode)
        {
            case cImmed:
                result->Value  = Value;
                break;
            case cVar:
                result->Var = Var;
                break;
            case cFCall: case cPCall:
                result->Funcno = Funcno;
                break;
        }
        result->SetParams(Params);
        result->Hash   = Hash;
        result->Depth  = Depth;
        //assert(Parent->RefCount > 0);
        result->Parent = Parent;
        return result;
    }

    void CodeTree::AddParam(const Param& param)
    {
        Params.push_back(param);
        Params.back().param->Parent = this;
    }

    void CodeTree::SetParams(const std::vector<Param>& RefParams)
    {
        Params = RefParams;
        /**
        *** Note: The only reason we need to CLONE the children here
        ***       is because they must have the correct Parent field.
        ***       The Parent is required because of backward-recursive
        ***       hash regeneration. Is there any way around this?
        */

        for(size_t a=0; a<Params.size(); ++a)
        {
            Params[a].param = Params[a].param->Clone();
            Params[a].param->Parent = this;
        }
    }

    void CodeTree::DelParam(size_t index)
    {
        Params.erase(Params.begin() + index);
    }
}

#endif
