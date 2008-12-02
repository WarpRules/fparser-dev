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

void FunctionParser::Optimize()
{
    if(isOptimized) return;
    CopyOnWrite();

    /*PrintByteCode(std::cout);*/

    FPoptimizer_CodeTree::CodeTree* tree
        = CodeTree::GenerateFrom(data->ByteCode, data->Immed, *data);

    std::set<uint_fast64_t> optimized_children;
    while(FPoptimizer_Grammar::pack.glist[0].ApplyTo(optimized_children, *tree))
        {}

    optimized_children.clear();
    while(FPoptimizer_Grammar::pack.glist[1].ApplyTo(optimized_children, *tree))
        {}

    optimized_children.clear();
    while(FPoptimizer_Grammar::pack.glist[2].ApplyTo(optimized_children, *tree))
        {}

    tree->Sort_Recursive();

    std::vector<unsigned> byteCode;
    std::vector<double> immed;
    size_t stacktop_max = 0;
    tree->SynthesizeByteCode(byteCode, immed, stacktop_max);

    delete tree;

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

    /*PrintByteCode(std::cout);*/

    isOptimized = true;
}
