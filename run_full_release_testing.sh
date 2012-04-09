#!/bin/sh

runTest()
{
    echo "Building testbed_release with"
    echo "FP_FEATURE_FLAGS=$1"
    make -s release_clean
    FP_FEATURE_FLAGS="$1" make -s testbed_release
    if [ "$?" -ne "0" ]; then exit 1; fi
    echo "Running: ./testbed_release $2"
    ./testbed_release "$2"
    if [ "$?" -ne "0" ]; then exit 1; fi
}

runTest "-DFP_SUPPORT_LONG_DOUBLE_TYPE -DFP_DISABLE_DOUBLE_TYPE"
runTest "-D_GLIBCXX_DEBUG"
runTest "-D_GLIBCXX_DEBUG -DFP_NO_SUPPORT_OPTIMIZER" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_NO_SUPPORT_OPTIMIZER" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_USE_THREAD_SAFE_EVAL" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_USE_THREAD_SAFE_EVAL_WITH_ALLOCA" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_NO_SUPPORT_OPTIMIZER -DFP_USE_THREAD_SAFE_EVAL_WITH_ALLOCA" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_SUPPORT_FLOAT_TYPE" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_SUPPORT_LONG_DOUBLE_TYPE -DFP_USE_STRTOLD" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_SUPPORT_LONG_INT_TYPE" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_SUPPORT_MPFR_FLOAT_TYPE" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_SUPPORT_GMP_INT_TYPE" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_SUPPORT_COMPLEX_DOUBLE_TYPE" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_SUPPORT_COMPLEX_FLOAT_TYPE" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_SUPPORT_COMPLEX_LONG_DOUBLE_TYPE -DFP_USE_STRTOLD" -skipSlowAlgo
runTest "-D_GLIBCXX_DEBUG -DFP_SUPPORT_FLOAT_TYPE -DFP_SUPPORT_LONG_DOUBLE_TYPE -DFP_SUPPORT_LONG_INT_TYPE -DFP_SUPPORT_MPFR_FLOAT_TYPE -DFP_SUPPORT_GMP_INT_TYPE -DFP_SUPPORT_COMPLEX_DOUBLE_TYPE -DFP_SUPPORT_COMPLEX_FLOAT_TYPE -DFP_SUPPORT_COMPLEX_LONG_DOUBLE_TYPE" -skipSlowAlgo

make -s release_clean
