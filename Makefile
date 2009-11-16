#===========================================================================
# This Makefile uses quite heavily GNU Make features, so it's probably
# hopeless to try to use it with other Make programs which do not have the
# same extensions.
#
# Also requires: rm, grep, sed and g++ (regardless of what CXX and LD are)
# The optimizer code generator requires bison++
#===========================================================================

RELEASE_VERSION=3.3.2

# The FP_FEATURE_FLAGS is set by run_full_release_testing.sh, but can be
# used otherwise as well.
ifeq ($(FP_FEATURE_FLAGS),)
FEATURE_FLAGS =
FEATURE_FLAGS += -DFP_ENABLE_EVAL
FEATURE_FLAGS += -DFP_SUPPORT_TR1_MATH_FUNCS
#FEATURE_FLAGS += -DFP_NO_SUPPORT_OPTIMIZER
#FEATURE_FLAGS += -DFP_USE_THREAD_SAFE_EVAL
#FEATURE_FLAGS += -DFP_USE_THREAD_SAFE_EVAL_WITH_ALLOCA
#FEATURE_FLAGS += -DFP_NO_EVALUATION_CHECKS
#FEATURE_FLAGS += -D_GLIBCXX_DEBUG
#FEATURE_FLAGS += -DFP_DISABLE_SHORTCUT_LOGICAL_EVALUATION
else
FEATURE_FLAGS = $(FP_FEATURE_FLAGS)
endif

OPTIMIZATION=-O3 -ffast-math -march=native -fexpensive-optimizations
#OPTIMIZATION=-g
#OPTIMIZATION=-g -pg

CXX=g++
LD=g++

FEATURE_FLAGS += -DFUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT

CPPFLAGS=$(FEATURE_FLAGS)
CXXFLAGS=-Wall -W -Wconversion -pedantic -ansi $(OPTIMIZATION)
#CXXFLAGS += -Wunreachable-code

#CXXFLAGS += -std=c++0x

# For compilation with ICC:
#OPTIMIZATION=-O3 -xT -inline-level=2 -w1 -openmp -mssse3
#CXX=icc
#LD=icc  -L/opt/intel/Compiler/11.0/074/bin/intel64/lib -lirc -lstdc++ -openmp -lguide -lpthread
#CXXFLAGS=-Wall $(OPTIMIZATION) $(FEATURE_FLAGS)

CPPFLAGS += -I"`pwd`"

all: testbed speedtest example functioninfo

FP_MODULES = 	fparser.o \
		fpoptimizer/fpoptimizer_grammar_data.o \
		fpoptimizer/fpoptimizer_main.o \
		fpoptimizer/fpoptimizer_readbytecode.o \
		fpoptimizer/fpoptimizer_makebytecode.o \
		fpoptimizer/fpoptimizer_codetree.o \
		fpoptimizer/fpoptimizer_grammar.o \
		fpoptimizer/fpoptimizer_optimize.o \
		fpoptimizer/fpoptimizer_optimize_match.o \
		fpoptimizer/fpoptimizer_optimize_synth.o \
		fpoptimizer/fpoptimizer_optimize_debug.o \
		fpoptimizer/fpoptimizer_constantfolding.o \
		fpoptimizer/fpoptimizer_rangeestimation.o \
		fpoptimizer/fpoptimizer_opcodename.o \
		fpoptimizer/fpoptimizer_bytecodesynth.o \
		fpoptimizer/fpoptimizer_transformations.o \
		fpoptimizer/fpoptimizer_debug.o \
		fpoptimizer/fpoptimizer_hash.o

RELEASE_PACK_FILES = example.cc fparser.cc fparser.hh fpoptimizer.cc \
	fpconfig.hh fptypes.hh fp_opcode_add.inc fparser.html style.css

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

koe: koe.o $(FP_MODULES)
	$(LD) -o $@ $^

functioninfo: functioninfo.o $(FP_MODULES)
	$(LD) -o $@ $^

fpoptimizer/grammar_parser: \
		fpoptimizer/grammar_parser.o \
		fpoptimizer/fpoptimizer_grammar.o \
		fpoptimizer/fpoptimizer_opcodename.o
	$(LD) -o $@ $^

fpoptimizer/grammar_parser.cc: \
		fpoptimizer/grammar_parser.y
	bison++ --output=$@ $<

fpoptimizer/fpoptimizer_grammar_data.cc: \
		fpoptimizer/grammar_parser \
		fpoptimizer/grammar_parser.y \
		fpoptimizer/fpoptimizer_treerules.dat
	fpoptimizer/grammar_parser < fpoptimizer/fpoptimizer_treerules.dat > $@

fpoptimizer/bytecoderules_parser: \
		fpoptimizer/bytecoderules_parser.o
	$(LD) -o $@ $^

fp_opcode_add.inc: fpoptimizer/bytecoderules_parser fpoptimizer/fparser_bytecoderules.dat
	fpoptimizer/bytecoderules_parser < fpoptimizer/fparser_bytecoderules.dat > $@

fpoptimizer.cc: \
		fpoptimizer/grammar_parser.y \
		fpoptimizer/grammar_parser.cc \
		fpoptimizer/fpoptimizer_autoptr.hh \
		fpoptimizer/fpoptimizer_codetree.hh \
		fpoptimizer/fpoptimizer_grammar.hh \
		fpoptimizer/fpoptimizer_consts.hh \
		fpoptimizer/fpoptimizer_optimize.hh \
		fpoptimizer/fpoptimizer_hash.hh \
		fpoptimizer/fpoptimizer_main.cc \
		fpoptimizer/fpoptimizer_hash.cc \
		fpoptimizer/fpoptimizer_codetree.cc \
		fpoptimizer/fpoptimizer_debug.cc \
		fpoptimizer/fpoptimizer_grammar.cc \
		fpoptimizer/fpoptimizer_grammar_data.cc \
		fpoptimizer/fpoptimizer_optimize.cc \
		fpoptimizer/fpoptimizer_optimize_match.cc \
		fpoptimizer/fpoptimizer_optimize_synth.cc \
		fpoptimizer/fpoptimizer_optimize_debug.cc \
		fpoptimizer/fpoptimizer_opcodename.cc \
		fpoptimizer/fpoptimizer_opcodename.hh \
		fpoptimizer/fpoptimizer_bytecodesynth.cc \
		fpoptimizer/fpoptimizer_bytecodesynth.hh \
		fpoptimizer/fpoptimizer_makebytecode.cc \
		fpoptimizer/fpoptimizer_readbytecode.cc \
		fpoptimizer/fpoptimizer_constantfolding.cc \
		fpoptimizer/fpoptimizer_rangeestimation.cc \
		fpoptimizer/fpoptimizer_transformations.cc \
		fpoptimizer/fpoptimizer_header.txt \
		fpoptimizer/fpoptimizer_footer.txt
	rm -f fpoptimizer.cc
	for file in \
	    fpoptimizer/fpoptimizer_header.txt \
	    fpoptimizer/fpoptimizer_hash.hh \
	    fpoptimizer/fpoptimizer_autoptr.hh \
	    fpoptimizer/fpoptimizer_codetree.hh \
	    fpoptimizer/fpoptimizer_grammar.hh \
	    fpoptimizer/fpoptimizer_consts.hh \
	    fpoptimizer/fpoptimizer_optimize.hh \
	    fpoptimizer/crc32.hh \
	    fpoptimizer/fpoptimizer_opcodename.hh \
	    fpoptimizer/fpoptimizer_opcodename.cc \
	    fpoptimizer/fpoptimizer_bytecodesynth.hh \
	    fpoptimizer/fpoptimizer_bytecodesynth.cc \
	    fpoptimizer/fpoptimizer_codetree.cc \
	    fpoptimizer/fpoptimizer_debug.cc \
	    fpoptimizer/fpoptimizer_grammar.cc \
	    fpoptimizer/fpoptimizer_grammar_data.cc \
	    fpoptimizer/fpoptimizer_optimize.cc \
	    fpoptimizer/fpoptimizer_optimize_match.cc \
	    fpoptimizer/fpoptimizer_optimize_synth.cc \
	    fpoptimizer/fpoptimizer_optimize_debug.cc \
	    fpoptimizer/fpoptimizer_main.cc \
	    fpoptimizer/fpoptimizer_hash.cc \
	    fpoptimizer/fpoptimizer_makebytecode.cc \
	    fpoptimizer/fpoptimizer_readbytecode.cc \
	    fpoptimizer/fpoptimizer_constantfolding.cc \
	    fpoptimizer/fpoptimizer_rangeestimation.cc \
	    fpoptimizer/fpoptimizer_transformations.cc \
	    fpoptimizer/fpoptimizer_footer.txt \
	; do \
		echo "#line 1 \"$$file\""; \
		sed -r "s@^(#include (\"fpoptimizer|\"crc32).*)@// line removed for fpoptimizer.cc: \\1@" < "$$file"; \
		echo; \
	done > $@

VersionChanger: VersionChanger.cc
	g++ -O3 $^ -s -o $@

create_testrules_for_optimization_rules: \
		create_testrules_for_optimization_rules.cc \
		fpoptimizer/fpoptimizer_grammar_data.o \
		fpoptimizer/fpoptimizer_opcodename.o \
		fpoptimizer/fpoptimizer_grammar.o
	g++ -O3 $^ -s -o $@

fpoptimizer_tests.sh: create_testrules_for_optimization_rules
	./$< > $@
	chmod +x $@

set_version_string: VersionChanger
	./VersionChanger $(RELEASE_VERSION) fparser.cc fparser.hh fpconfig.hh \
	fpoptimizer.cc fptypes.hh fp_opcode_add.inc \
	fpoptimizer/bytecoderules_parser.cc fparser.html webpage/index.html

pack: $(RELEASE_PACK_FILES) set_version_string
	zip -9 fparser$(RELEASE_VERSION).zip $(RELEASE_PACK_FILES)

devel_pack: set_version_string
	tar -cjvf fparser$(RELEASE_VERSION)_devel.tar.bz2 \
	Makefile example.cc fparser.cc fparser.hh fpconfig.hh \
	fptypes.hh fp_opcode_add.inc speedtest.cc testbed.cc \
	fparser.html style.css \
	fpoptimizer/*.hh fpoptimizer/*.cc \
	fpoptimizer/fparser_bytecoderules.dat \
	fpoptimizer/fpoptimizer_treerules.dat \
	fpoptimizer/*.txt fpoptimizer/grammar_parser.y \
	run_full_release_testing.sh VersionChanger.cc functioninfo.cc

clean:
	rm -f	testbed testbed_release speedtest speedtest_release \
		example ftest powi_speedtest \
		fpoptimizer/grammar_parser \
		*.o fpoptimizer/*.o .dep \
		fpoptimizer/grammar_parser.output

release_clean:
	rm -f testbed_release speedtest_release \
		testbed.o fparser.o fpoptimizer.o

distclean: clean
	rm -f	*~

.dep:
	g++ -MM $(CPPFLAGS) $(wildcard *.cc) > .dep
	g++ -MM $(CPPFLAGS) $(wildcard fpoptimizer/*.cc) | sed 's|^.*.o:|fpoptimizer/&|' >> .dep

-include .dep
