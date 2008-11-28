#include <cmath>
#include <list>

#include "fpoptimizer_codetree.hh"
#include "fptypes.hh"

#include "fpoptimizer_consts.hh"


using namespace FUNCTIONPARSERTYPES;
//using namespace FPoptimizer_Grammar;


namespace FPoptimizer_CodeTree
{
    CodeTree::CodeTree() : Opcode(), Params(), Hash(), Parent()
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


    void CodeTree::Recalculate_Hash_NoRecursion()
    {
        Hash = Opcode * 0x3A83A83A83A83A0ULL;
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
        for(size_t a=0; a<Params.size(); ++a)
        {
            Hash += (1+Params[a].sign)*0x2492492492492492ULL;
            Hash *= 1099511628211ULL;
            Hash += Params[a].param->Hash;
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
        result->Params = Params;
        for(size_t a=0; a<Params.size(); ++a)
            result->Params[a].param = Params[a].param->Clone();
        result->Hash   = Hash;
        result->Parent = Parent;
        return result;
    }
}
