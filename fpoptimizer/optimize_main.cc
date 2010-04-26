#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#include "codetree.hh"
#include "optimize.hh"

#ifdef FP_SUPPORT_OPTIMIZER

template<typename Value_t>
void FunctionParserBase<Value_t>::Optimize()
{
    using namespace FPoptimizer_CodeTree;

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

FUNCTIONPARSER_INSTANTIATE_TYPES

#endif
