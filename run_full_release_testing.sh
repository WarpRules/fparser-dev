#!/bin/sh

runTest()
{
    make release_clean
    FP_FEATURE_FLAGS="$1" make testbed_release
    if [ "$?" -ne "0" ]; then exit 1; fi
    echo "Running: ./testbed_release $2"
    ./testbed_release "$2"
    if [ "$?" -ne "0" ]; then exit 1; fi
}

runTest "-D_GLIBCXX_DEBUG"
runTest "-D_GLIBCXX_DEBUG -DFP_ENABLE_EVAL" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_ENABLE_EVAL -DFP_SUPPORT_TR1_MATH_FUNCS" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_ENABLE_EVAL -DFP_SUPPORT_TR1_MATH_FUNCS -DFP_NO_EVALUATION_CHECKS" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_NO_SUPPORT_OPTIMIZER" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_ENABLE_EVAL -DFP_SUPPORT_TR1_MATH_FUNCS -DFP_NO_SUPPORT_OPTIMIZER" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_USE_THREAD_SAFE_EVAL" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_USE_THREAD_SAFE_EVAL_WITH_ALLOCA" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_ENABLE_EVAL -DFP_SUPPORT_TR1_MATH_FUNCS -DFP_NO_SUPPORT_OPTIMIZER -DFP_USE_THREAD_SAFE_EVAL_WITH_ALLOCA -DFP_NO_EVALUATION_CHECKS" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_SUPPORT_FLOAT_TYPE" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_SUPPORT_LONG_DOUBLE_TYPE" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_SUPPORT_LONG_INT_TYPE" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_SUPPORT_MPFR_FLOAT_TYPE" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_SUPPORT_GMP_INT_TYPE" -skipSlowAlgo

make release_clean
