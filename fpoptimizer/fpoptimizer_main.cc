#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#include "fpoptimizer_codetree.hh"
#include "fpoptimizer_grammar.hh"

using namespace FUNCTIONPARSERTYPES;

namespace FPoptimizer_CodeTree
{
    bool    CodeTree::IsImmed() const { return Opcode == cImmed; }
    bool    CodeTree::IsVar()   const { return Opcode == cVar; }
}

using namespace FPoptimizer_CodeTree;

#ifdef FP_SUPPORT_OPTIMIZER
void FunctionParser::Optimize()
{
    if(isOptimized) return;
    CopyOnWrite();

    //PrintByteCode(std::cout);

    FPoptimizer_CodeTree::CodeTreeP tree
        = CodeTree::GenerateFrom(data->ByteCode, data->Immed, *data);

    while(FPoptimizer_Grammar::pack.glist[0].ApplyTo(*tree))
        {}

    while(FPoptimizer_Grammar::pack.glist[1].ApplyTo(*tree))
        {}

    while(FPoptimizer_Grammar::pack.glist[2].ApplyTo(*tree))
        {}

    tree->Sort_Recursive();

    std::vector<unsigned> byteCode;
    std::vector<double> immed;
    size_t stacktop_max = 0;
    tree->SynthesizeByteCode(byteCode, immed, stacktop_max);

    /*std::cout << std::flush;
    std::cerr << std::flush;
    fprintf(stderr, "Estimated stacktop %u\n", (unsigned)stacktop_max);
    fflush(stderr);*/

    if(data->StackSize != stacktop_max)
    {
        data->StackSize = stacktop_max;
        data->Stack.resize(stacktop_max);
    }

    data->ByteCode.swap(byteCode);
    data->Immed.swap(immed);

    //PrintByteCode(std::cout);

    isOptimized = true;
}
#endif
