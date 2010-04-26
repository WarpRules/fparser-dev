#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#include "codetree.hh"
#include "optimize.hh"

using namespace FUNCTIONPARSERTYPES;

#ifdef FP_SUPPORT_OPTIMIZER
using namespace FPoptimizer_CodeTree;

template<typename Value_t>
void FunctionParserBase<Value_t>::Optimize()
{
    CopyOnWrite();

    //PrintByteCode(std::cout);

    CodeTree<Value_t> tree;
    tree.GenerateFrom(data->ByteCode, data->Immed, *data);

    FPoptimizer_Optimize::ApplyGrammars(tree);

    std::vector<unsigned> byteCode;
    std::vector<Value_t> immed;
    size_t stacktop_max = 0;
    tree.SynthesizeByteCode(byteCode, immed, stacktop_max);

    /*std::cout << std::flush;
    std::cerr << std::flush;
    fprintf(stderr, "Estimated stacktop %u\n", (unsigned)stacktop_max);
    fflush(stderr);*/

    if(data->StackSize != stacktop_max)
    {
        data->StackSize = stacktop_max; // note: gcc warning is meaningful
#ifndef FP_USE_THREAD_SAFE_EVAL
        data->Stack.resize(stacktop_max);
#endif
    }

    data->ByteCode.swap(byteCode);
    data->Immed.swap(immed);

    //PrintByteCode(std::cout);
}

template void FunctionParserBase<double>::Optimize();

#ifdef FP_SUPPORT_FLOAT_TYPE
template void FunctionParserBase<float>::Optimize();
#endif

#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
template void FunctionParserBase<long double>::Optimize();
#endif

#ifdef FP_SUPPORT_LONG_INT_TYPE
template<>
void FunctionParserBase<long>::Optimize()
{}
#endif

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

//FUNCTIONPARSER_INSTANTIATE_TYPES

#endif
