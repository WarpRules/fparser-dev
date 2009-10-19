#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#include "fpoptimizer_codetree.hh"
#include "fpoptimizer_grammar.hh"

using namespace FUNCTIONPARSERTYPES;

#ifdef FP_SUPPORT_OPTIMIZER
using namespace FPoptimizer_CodeTree;
using namespace FPoptimizer_Grammar;

void FunctionParser::Optimize()
{
    CopyOnWrite();

    //PrintByteCode(std::cout);

    CodeTree tree;
    tree.GenerateFrom(data->ByteCode, data->Immed, *data);

    while(ApplyGrammar(pack.glist[0], tree)) // INTERMEDIATE + BASIC
        { //std::cout << "Rerunning 1\n";
            tree.FixIncompleteHashes();
        }

    while(ApplyGrammar(pack.glist[1], tree)) // FINAL1 + BASIC
        { //std::cout << "Rerunning 2\n";
            tree.FixIncompleteHashes();
        }

    while(ApplyGrammar(pack.glist[2], tree)) // FINAL2
        { //std::cout << "Rerunning 3\n";
            tree.FixIncompleteHashes();
        }

    std::vector<unsigned> byteCode;
    std::vector<double> immed;
    size_t stacktop_max = 0;
    tree.SynthesizeByteCode(byteCode, immed, stacktop_max);

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
