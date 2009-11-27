#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#include "fpoptimizer_codetree.hh"
#include "fpoptimizer_optimize.hh"

using namespace FUNCTIONPARSERTYPES;

#ifdef FP_SUPPORT_OPTIMIZER
using namespace FPoptimizer_CodeTree;

template<>
void FunctionParserBase<double>::Optimize()
{
    CopyOnWrite();

    //PrintByteCode(std::cout);

    CodeTree tree;
    tree.GenerateFrom(data->ByteCode, data->Immed, *data);

    FPoptimizer_Optimize::ApplyGrammars(tree);

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

template<>
void FunctionParserBase<float>::Optimize()
{}

template<>
void FunctionParserBase<long double>::Optimize()
{}

template<>
void FunctionParserBase<long>::Optimize()
{}

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
template<>
void FunctionParserBase<MpfrFloat>::Optimize()
{}
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
template<>
void FunctionParserBase<GmpInt>::Optimize()
{}
#endif

FUNCTIONPARSER_INSTANTIATE_TYPES

#endif
