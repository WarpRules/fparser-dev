#===========================================================================
# This Makefile uses quite heavily GNU Make features, so it's probably
# hopeless to try to use it with other Make programs which do not have the
# same extensions.
#
# Also requires: rm, grep, sed and g++ (regardless of what CXX and LD are)
# The optimizer code generator requires bison++
#===========================================================================

# The FP_FEATURE_FLAGS is set by run_full_release_testing.sh, but can be
# used otherwise as well.
ifeq ($(FP_FEATURE_FLAGS),)
FEATURE_FLAGS =
FEATURE_FLAGS += -DFP_ENABLE_EVAL
FEATURE_FLAGS += -DFP_SUPPORT_ASINH
#FEATURE_FLAGS += -DFP_NO_SUPPORT_OPTIMIZER
#FEATURE_FLAGS += -DFP_USE_THREAD_SAFE_EVAL
#FEATURE_FLAGS += -DFP_USE_THREAD_SAFE_EVAL_WITH_ALLOCA
else
FEATURE_FLAGS = $(FP_FEATURE_FLAGS)
endif

OPTIMIZATION=-O3 -ffast-math -march=native
#OPTIMIZATION=-g
#OPTIMIZATION=-g -pg

CXX=g++
LD=g++

FEATURE_FLAGS += -DFUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT
CPPFLAGS=$(FEATURE_FLAGS)
CXXFLAGS=-Wall -W -Wconversion -pedantic -ansi $(OPTIMIZATION)

# For compilation with ICC:
#OPTIMIZATION=-O3 -xT -inline-level=2 -w1 -openmp -mssse3
#CXX=icc
#LD=icc  -L/opt/intel/Compiler/11.0/074/bin/intel64/lib -lirc -lstdc++ -openmp -lguide -lpthread
#CXXFLAGS=-Wall $(OPTIMIZATION) $(FEATURE_FLAGS)

CPPFLAGS += -I"`pwd`"

all: testbed speedtest example

FP_MODULES = 	fparser.o \
		fpoptimizer/fpoptimizer_main.o \
		fpoptimizer/fpoptimizer_bytecode_to_codetree.o \
		fpoptimizer/fpoptimizer_codetree_to_bytecode.o \
		fpoptimizer/fpoptimizer_codetree.o \
		fpoptimizer/fpoptimizer_grammar.o \
		fpoptimizer/fpoptimizer_optimize.o \
		fpoptimizer/fpoptimizer_opcodename.o

testbed: testbed.o $(FP_MODULES)
	$(LD) -o $@ $^

fpoptimizer.o: fpoptimizer.cc

testbed_release: testbed.o fparser.o fpoptimizer.o
	$(LD) -o $@ $^

speedtest: speedtest.o $(FP_MODULES)
	$(LD) -o $@ $^

speedtest_release: speedtest.o fparser.o fpoptimizer.o
	$(LD) -o $@ $^

example: example.o $(FP_MODULES)
	$(LD) -o $@ $^

ftest: ftest.o $(FP_MODULES)
	$(LD) -o $@ $^

powi_speedtest: powi_speedtest.o $(FP_MODULES)
	$(LD) -o $@ $^

fpoptimizer/fpoptimizer_grammar_gen: \
		fpoptimizer/fpoptimizer_grammar_gen.o \
		fpoptimizer/fpoptimizer_opcodename.o
	$(LD) -o $@ $^

fpoptimizer/fpoptimizer_grammar_gen.cc: \
		fpoptimizer/fpoptimizer_grammar_gen.y
	bison++ --output=$@ $<

fpoptimizer/fpoptimizer_grammar.cc: \
		fpoptimizer/fpoptimizer_grammar_gen \
		fpoptimizer/fpoptimizer_grammar_gen.y \
		fpoptimizer/fpoptimizer.dat
	fpoptimizer/fpoptimizer_grammar_gen < fpoptimizer/fpoptimizer.dat > $@

fpoptimizer.cc: \
		fpoptimizer/fpoptimizer_grammar_gen.y \
		fpoptimizer/fpoptimizer_grammar_gen.cc \
		fpoptimizer/fpoptimizer_codetree.hh \
		fpoptimizer/fpoptimizer_grammar.hh \
		fpoptimizer/fpoptimizer_consts.hh \
		fpoptimizer/fpoptimizer_hash.hh \
		fpoptimizer/fpoptimizer_main.cc \
		fpoptimizer/fpoptimizer_codetree.cc \
		fpoptimizer/fpoptimizer_grammar.cc \
		fpoptimizer/fpoptimizer_optimize.cc \
		fpoptimizer/fpoptimizer_opcodename.cc \
		fpoptimizer/fpoptimizer_opcodename.hh \
		fpoptimizer/fpoptimizer_codetree_to_bytecode.cc \
		fpoptimizer/fpoptimizer_bytecode_to_codetree.cc \
		fpoptimizer/fpoptimizer_header.txt \
		fpoptimizer/fpoptimizer_footer.txt
	rm -f fpoptimizer.cc
	cat fpoptimizer/fpoptimizer_header.txt \
	    fpoptimizer/fpoptimizer_hash.hh \
	    fpoptimizer/fpoptimizer_codetree.hh \
	    fpoptimizer/fpoptimizer_grammar.hh \
	    fpoptimizer/fpoptimizer_consts.hh \
	    fpoptimizer/fpoptimizer_opcodename.hh \
	    fpoptimizer/crc32.hh \
	    fpoptimizer/fpoptimizer_opcodename.cc \
	    fpoptimizer/fpoptimizer_codetree.cc \
	    fpoptimizer/fpoptimizer_grammar.cc \
	    fpoptimizer/fpoptimizer_optimize.cc \
	    fpoptimizer/fpoptimizer_main.cc \
	    fpoptimizer/fpoptimizer_codetree_to_bytecode.cc \
	    fpoptimizer/fpoptimizer_bytecode_to_codetree.cc \
	    fpoptimizer/fpoptimizer_footer.txt \
		| grep -v '#include "fpoptimizer' \
		| grep -v '#include "crc32' \
		> $@

#fpoptimizer.cc: fpoptimizer_3.0.3.cc
#	cp $^ $@

pack: example.cc fparser.cc fparser.hh fpoptimizer.cc fparser.txt \
	fpconfig.hh fptypes.hh fparser.html style.css
	zip -9 fparser3.1.2.zip $^

devel_pack:
	tar -cjvf fparser3.1.2_devel.tar.bz2 \
	Makefile example.cc fparser.cc fparser.hh fparser.txt fpconfig.hh \
	fptypes.hh speedtest.cc testbed.cc fparser.html style.css \
	fpoptimizer/*.hh fpoptimizer/*.cc fpoptimizer/fpoptimizer.dat \
	fpoptimizer/*.txt fpoptimizer/fpoptimizer_grammar_gen.y \
	fpoptimizer_3.0.3.cc run_full_release_testing.sh

clean:
	rm -f	testbed testbed_release speedtest speedtest_release \
		example ftest powi_speedtest \
		fpoptimizer/fpoptimizer_grammar_gen \
		*.o fpoptimizer/*.o .dep \
		fpoptimizer/fpoptimizer_grammar_gen.output

release_clean:
	rm -f testbed_release speedtest_release \
		testbed.o fparser.o fpoptimizer.o

distclean: clean
	rm -f	*~

.dep:
	g++ -MM $(CPPFLAGS) $(wildcard *.cc) > .dep
	g++ -MM $(CPPFLAGS) $(wildcard fpoptimizer/*.cc) | sed 's|^.*.o:|fpoptimizer/&|' >> .dep

-include .dep
