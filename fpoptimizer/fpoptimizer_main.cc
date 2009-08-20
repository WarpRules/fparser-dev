#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#include "fpoptimizer_codetree.hh"
#include "fpoptimizer_grammar.hh"

using namespace FUNCTIONPARSERTYPES;

#ifdef FP_SUPPORT_OPTIMIZER
namespace FPoptimizer_CodeTree
{
    bool    CodeTree::IsImmed() const { return Opcode == cImmed; }
    bool    CodeTree::IsVar()   const { return Opcode == cVar; }
}

using namespace FPoptimizer_CodeTree;

void FunctionParser::Optimize()
{
    CopyOnWrite();

    //PrintByteCode(std::cout);

    FPoptimizer_CodeTree::CodeTreeP tree
        = CodeTree::GenerateFrom(data->ByteCode, data->Immed, *data);

    while(FPoptimizer_Grammar::pack.glist[0].ApplyTo(*tree)) // entry
        { //std::cout << "Rerunning 0\n";
        }

    while(FPoptimizer_Grammar::pack.glist[1].ApplyTo(*tree)) // intermediate
        { //std::cout << "Rerunning 1\n";
        }

    while(FPoptimizer_Grammar::pack.glist[2].ApplyTo(*tree)) // final1
        { //std::cout << "Rerunning 2\n";
        }

    while(FPoptimizer_Grammar::pack.glist[3].ApplyTo(*tree)) // final2
        { //std::cout << "Rerunning 3\n";
        }

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
        data->StackSize = stacktop_max; // note: gcc warning is meaningful
        data->Stack.resize(stacktop_max);
    }

    data->ByteCode.swap(byteCode);
    data->Immed.swap(immed);

    //PrintByteCode(std::cout);
}

#endif
