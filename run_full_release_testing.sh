#!/bin/sh

runTest()
{
    make release_clean
    FP_FEATURE_FLAGS="$1" make testbed_release
    if [ "$?" -ne "0" ]; then exit 1; fi
    ./testbed_release
    if [ "$?" -ne "0" ]; then exit 1; fi
}

runTest " "
runTest "-DFP_ENABLE_EVAL"
runTest "-DFP_ENABLE_EVAL -DFP_SUPPORT_ASINH"
runTest "-DFP_NO_SUPPORT_OPTIMIZER"
runTest "-DFP_ENABLE_EVAL -DFP_SUPPORT_ASINH -DFP_NO_SUPPORT_OPTIMIZER"
runTest "-DFP_USE_THREAD_SAFE_EVAL"
runTest "-DFP_USE_THREAD_SAFE_EVAL_WITH_ALLOCA"
runTest "-DFP_ENABLE_EVAL -DFP_SUPPORT_ASINH -DFP_NO_SUPPORT_OPTIMIZER -DFP_USE_THREAD_SAFE_EVAL_WITH_ALLOCA"
