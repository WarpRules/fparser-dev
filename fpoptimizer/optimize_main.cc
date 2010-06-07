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
    tree.GenerateFrom(mData->mByteCode, mData->mImmed, *mData);

    FPoptimizer_Optimize::ApplyGrammars(tree);

    std::vector<unsigned> byteCode;
    std::vector<Value_t> immed;
    size_t stacktop_max = 0;
    tree.SynthesizeByteCode(byteCode, immed, stacktop_max);

    /*std::cout << std::flush;
    std::cerr << std::flush;
    fprintf(stderr, "Estimated stacktop %u\n", (unsigned)stacktop_max);
    fflush(stderr);*/

    if(mData->mStackSize != stacktop_max)
    {
        mData->mStackSize = unsigned(stacktop_max); // Note: Ignoring GCC warning here.
#if !defined(FP_USE_THREAD_SAFE_EVAL) && \
    !defined(FP_USE_THREAD_SAFE_EVAL_WITH_ALLOCA)
        mData->mStack.resize(stacktop_max);
#endif
    }

    mData->mByteCode.swap(byteCode);
    mData->mImmed.swap(immed);

    //PrintByteCode(std::cout);
}

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
