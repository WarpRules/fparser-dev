#include <cmath>
#include <list>
#include <algorithm>

#include "fpoptimizer_codetree.hh"
#include "fptypes.hh"

#include "fpoptimizer_consts.hh"


using namespace FUNCTIONPARSERTYPES;
//using namespace FPoptimizer_Grammar;


namespace FPoptimizer_CodeTree
{
    CodeTree::CodeTree() : Opcode(), Params(), Hash(), Depth(1), Parent()
    {
    }

    CodeTree::~CodeTree()
    {
        for(size_t a=0; a<Params.size(); ++a)
            delete Params[a].param;
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
        Hash = Opcode * 0x3A83A83A83A83A0ULL;
        Depth = 1;
        switch(Opcode)
        {
            case cImmed:
                // FIXME: not portable - we're casting double* into uint_least64_t*
                if(Value != 0.0)
                    Hash ^= *(uint_least64_t*)&Value;
                return; // no params
            case cVar:
                Hash ^= (Var<<24) | (Var>>24);
                return; // no params
            case cFCall: case cPCall:
                Hash ^= (Funcno<<24) | (Funcno>>24);
                break;
        }
        size_t MaxChildDepth = 0;
        for(size_t a=0; a<Params.size(); ++a)
        {
            if(Params[a].param->Depth > MaxChildDepth)
                MaxChildDepth = Params[a].param->Depth;

            Hash += (1+Params[a].sign)*0x2492492492492492ULL;
            Hash *= 1099511628211ULL;
            Hash += Params[a].param->Hash;
        }
        Depth += MaxChildDepth;
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
        result->Params = Params;
        for(size_t a=0; a<Params.size(); ++a)
        {
            result->Params[a].param = Params[a].param->Clone();
            result->Params[a].param->Parent = result;
        }
        result->Hash   = Hash;
        result->Depth  = Depth;
        result->Parent = Parent;
        return result;
    }
}
