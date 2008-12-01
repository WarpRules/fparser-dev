#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#include "fpoptimizer_codetree.hh"

using namespace FUNCTIONPARSERTYPES;

namespace FPoptimizer_CodeTree
{
    bool    CodeTree::IsImmed() const { return Opcode == cImmed; }
    bool    CodeTree::IsVar()   const { return Opcode == cVar; }
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
