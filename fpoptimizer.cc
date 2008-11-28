#include "fpoptimizer.hh"
#include "fptypes.hh"

#include "fpoptimizer_consts.hh"


using namespace FUNCTIONPARSERTYPES;
//using namespace FPoptimizer_Grammar;

namespace FPoptimizer_CodeTree
{
    bool    CodeTree::IsImmed() const { return Opcode == cImmed; }
    bool      CodeTree::IsVar() const { return Opcode == cVar; }
    
    void CodeTree::ConstantFolding()
    {
        // Insert here any hardcoded constant-folding optimizations
        // that you want to be done at bytecode->codetree conversion time.
    }
}
