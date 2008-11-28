#include "fpoptimizer_codetree.hh"
#include "fpoptimizer_grammar.hh"

#include "fparser.hh"
#include "fptypes.hh"


using namespace FUNCTIONPARSERTYPES;

namespace FPoptimizer_CodeTree
{
    bool    CodeTree::IsImmed() const { return Opcode == cImmed; }
    bool    CodeTree::IsVar()   const { return Opcode == cVar; }

    void CodeTree::ConstantFolding()
    {
        // Insert here any hardcoded constant-folding optimizations
        // that you want to be done at bytecode->codetree conversion time.
    }
}

using namespace FPoptimizer_CodeTree;

void FunctionParser::Optimize()
{
    if(isOptimized) return;
    CopyOnWrite();

    CodeTree* tree = CodeTree::GenerateFrom(data->ByteCode, data->Immed, *data);
    std::vector<unsigned> byteCode;
    std::vector<double> immed;
    
    size_t stacktop_cur = 0;
    size_t stacktop_max = 0;
    tree->SynthesizeByteCode(byteCode, immed, stacktop_cur, stacktop_max);

    if(data->StackSize < stacktop_max)
    {
        data->StackSize = stacktop_max;
        data->Stack.resize(stacktop_max);
    }

    data->ByteCode.swap(byteCode);
    data->Immed.swap(immed);

    isOptimized = true;
}
