#===========================================================================
# This Makefile uses quite heavily GNU Make extensions, so it's probably
# hopeless to try to use it with other Make programs which do not have the
# same extensions.
#
# Also requires: rm, grep, sed and g++ (regardless of what CXX and LD are).
# The optimizer code generator requires bison.
#===========================================================================

RELEASE_VERSION=5.0.0

# The FP_FEATURE_FLAGS is set by run_full_release_testing.sh, but can be
# used otherwise as well.
ifeq ($(FP_FEATURE_FLAGS),)
FEATURE_FLAGS =
FEATURE_FLAGS += -DFP_ENABLE_EVAL
#FEATURE_FLAGS += -DFP_NO_SUPPORT_OPTIMIZER
FEATURE_FLAGS += -DFP_USE_THREAD_SAFE_EVAL
#FEATURE_FLAGS += -DFP_USE_THREAD_SAFE_EVAL_WITH_ALLOCA
#FEATURE_FLAGS += -D_GLIBCXX_DEBUG
#FEATURE_FLAGS += -DFP_DISABLE_SHORTCUT_LOGICAL_EVALUATION
FEATURE_FLAGS += -DFP_SUPPORT_FLOAT_TYPE
FEATURE_FLAGS += -DFP_SUPPORT_LONG_DOUBLE_TYPE
FEATURE_FLAGS += -DFP_SUPPORT_LONG_INT_TYPE
FEATURE_FLAGS += -DFP_SUPPORT_MPFR_FLOAT_TYPE
FEATURE_FLAGS += -DFP_SUPPORT_GMP_INT_TYPE
FEATURE_FLAGS += -DFP_SUPPORT_COMPLEX_DOUBLE_TYPE
FEATURE_FLAGS += -DFP_SUPPORT_COMPLEX_FLOAT_TYPE
FEATURE_FLAGS += -DFP_SUPPORT_COMPLEX_LONG_DOUBLE_TYPE
FEATURE_FLAGS += -DFP_USE_STRTOLD
FEATURE_FLAGS += -DFP_SUPPORT_CPLUSPLUS11_MATH_FUNCS
else
FEATURE_FLAGS = $(FP_FEATURE_FLAGS)
endif

OPTIMIZATION=-O3 -ffast-math -march=native
#       -ffunction-sections -fdata-sections

#OPTIMIZATION += -fmerge-all-constants

#OPTIMIZATION += -fipa-pta -fivopts
#CXXFLAGS += -fanalyzer
#CXXFLAGS += -mfpmath=sse+387

#OPTIMIZATION += -mlong-double-128

#OPTIMIZATION+=-g
#OPTIMIZATION=-g -O0 -fno-inline
#OPTIMIZATION=-g -O0 -fno-inline -fno-inline-functions -fno-default-inline
#OPTIMIZATION=-g -O2 -fno-inline -fno-inline-functions -fno-default-inline
#OPTIMIZATION=-g -pg -fprofile -fprofile-values -fprofile-generate -ftest-coverage
#OPTIMIZATION=-g -pg
#OPTIMIZATION=-g -O0 -fsanitize=thread
#LDFLAGS += -fsanitize=thread
#OPTIMIZATION=-g -O0 -fsanitize=address
#LDFLAGS += -fsanitize=address


CXX=g++
#CXX=clang++
# -m32 -mfpmath=sse
# -m32 -mfpmath=387

LD=g++
#LD=clang++
#LD=g++ -g
# -m32 -mfpmath=sse
# -m32 -mfpmath=387

#OPTIMIZATION += -finline -finline-functions -fdefault-inline
#OPTIMIZATION += -finline-limit=300000
#OPTIMIZATION += --param max-inline-insns-auto=300000
#OPTIMIZATION += --param max-inline-recursive-depth-auto=30
#OPTIMIZATION += --param max-inline-insns-single=300000
#OPTIMIZATION += --param inline-unit-growth=9000
#OPTIMIZATION += --param max-early-inliner-iterations=30
#OPTIMIZATION += --param early-inlining-insns=90000
#OPTIMIZATION += -fkeep-inline-functions
#OPTIMIZATION += -fimplement-inlines

FEATURE_FLAGS += -DFUNCTIONPARSER_SUPPORT_DEBUGGING

#LD +=  -fprofile -fprofile-values -fprofile-generate -ftest-coverage 

CPPFLAGS=$(FEATURE_FLAGS)
CXXFLAGS=-Wall -Wextra -Wno-psabi -pedantic $(OPTIMIZATION)
#CXXFLAGS += -Wunreachable-code

#CXXFLAGS += -Weffc++

ifneq (,$(findstring -DFP_SUPPORT_MPFR_FLOAT_TYPE,$(FEATURE_FLAGS)))
LDFLAGS += -lgmp -lmpfr
ADDITIONAL_MODULES = mpfr/MpfrFloat.o
ifneq (,$(findstring -DFP_SUPPORT_GMP_INT_TYPE,$(FEATURE_FLAGS)))
ADDITIONAL_MODULES += mpfr/GmpInt.o
endif
else
ifneq (,$(findstring -DFP_SUPPORT_GMP_INT_TYPE,$(FEATURE_FLAGS)))
LDFLAGS += -lgmp
ADDITIONAL_MODULES = mpfr/GmpInt.o
endif
endif

STD_OPTION := "$(shell /bin/sh -c './test_std_version.sh "$(CXX)"')"

ifeq ($(CXX_VERSION),)
CXX_VERSION := $(STD_OPTION)
else
CXX_VERSION := -std=$(CXX_VERSION)
endif

CXXFLAGS += $(CXX_VERSION)

CXXFLAGS_TESTBED := $(CXX_VERSION)

CXX := ./report_time.sh $(CXX)


#LD += -Xlinker --gc-sections
#LD += -Xlinker --print-gc-sections
# ^Use this option to list everything that GC removed.


# For compilation with ICC:
#OPTIMIZATION=-O3 -xT -inline-level=2 -w1 -openmp -mssse3
#CXX=icc
#LD=icc  -L/opt/intel/Compiler/11.1/059/bin/intel64/lib -lirc -lstdc++ -openmp -lguide -lpthread
#CXXFLAGS=-Wall $(OPTIMIZATION) $(FEATURE_FLAGS)

CPPFLAGS += -I"$(shell pwd)"

all: testbed speedtest functioninfo

FP_MODULES = 	fparser.o \
		fpoptimizer/grammar_data.o \
		fpoptimizer/optimize_main.o \
		fpoptimizer/readbytecode.o \
		fpoptimizer/makebytecode.o \
		fpoptimizer/codetree.o \
		fpoptimizer/grammar.o \
		fpoptimizer/optimize.o \
		fpoptimizer/optimize_match.o \
		fpoptimizer/optimize_synth.o \
		fpoptimizer/optimize_debug.o \
		fpoptimizer/constantfolding.o \
		fpoptimizer/valuerange.o \
		fpoptimizer/rangeestimation.o \
		fpoptimizer/opcodename.o \
		fpoptimizer/bytecodesynth.o \
		fpoptimizer/transformations.o \
		fpoptimizer/cse.o \
		fpoptimizer/debug.o \
		fpoptimizer/hash.o \
		$(ADDITIONAL_MODULES)

TESTBED_MODULES = \
	testbed.o \
	tests/testbed_alltests.o \
	tests/testbed_evaluate0.o
TESTBED_GENSRC = \
	tests/testbed_autogen.hh \
	tests/testbed_alltests.cc \
	tests/testbed_evaluate0.cc \
	tests/testbed_cpptest.hh
define testbed_n
TESTBED_MODULES += tests/testbed_evaluate$(N).o
TESTBED_MODULES += tests/testbed_testlist$(N).o
tests/testbed_evaluate$(N).o: tests/testbed_evaluate$(N).cc \
                              tests/testbed_autogen.hh extrasrc/fpaux.hh tests/testbed_defs.hh \
                              tests/testbed_const$(N).hh
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(CXXFLAGS_TESTBED) -c -o $$@ $$<
tests/testbed_testlist$(N).o: tests/testbed_testlist$(N).cc \
                              tests/testbed_autogen.hh extrasrc/fpaux.hh tests/testbed_defs.hh
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(CXXFLAGS_TESTBED) -c -o $$@ $$<
TESTBED_GENSRC += tests/testbed_evaluate$(N).cc
TESTBED_GENSRC += tests/testbed_testlist$(N).cc
TESTBED_GENSRC += tests/testbed_const$(N).hh

endef
$(eval $(foreach N,1 2 3 4 5 6 7 8 9,\
$(call testbed_n)))

RELEASE_PACK_FILES = examples/example.cc examples/example2.cc fparser.cc \
	fparser.hh fparser_mpfr.hh fparser_gmpint.hh \
	fpoptimizer.cc fpconfig.hh extrasrc/fptypes.hh extrasrc/fpaux.hh \
	mpfr/MpfrFloat.hh mpfr/MpfrFloat.cc mpfr/GmpInt.hh mpfr/GmpInt.cc \
	extrasrc/fp_opcode_add.inc \
	extrasrc/fp_identifier_parser.inc \
	docs/fparser.html docs/style.css docs/lgpl.txt docs/gpl.txt

testbed: $(TESTBED_MODULES) $(FP_MODULES) $(TESTBED_MODULES)
	$(LD) -o $@ $^ $(LDFLAGS) -lpthread

fpoptimizer.o: fpoptimizer.cc

extrasrc/fpaux.hh: util/build_fpaux $(wildcard extrasrc/functions/*.hh)
	util/build_fpaux -o "$@" extrasrc/functions/*.hh

util/build_fpaux: util/build_fpaux.o
	$(LD) -o "$@" "$^" $(LDFLAGS)

testbed.o: testbed.cc tests/testbed_autogen.hh tests/stringutil.hh tests/testbed_defs.hh
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(CXXFLAGS_TESTBED) -c -o $@ $<

testbed_release: $(TESTBED_MODULES) fparser.o fpoptimizer.o $(ADDITIONAL_MODULES)
	$(LD) -o $@ $^ $(LDFLAGS)

speedtest: util/speedtest.o $(FP_MODULES)
	$(LD) -o $@ $^ $(LDFLAGS)

speedtest_release: util/speedtest.o fparser.o fpoptimizer.o
	$(LD) -o $@ $^ $(LDFLAGS)

examples/example: examples/example.o $(FP_MODULES)
	$(LD) -o $@ $^ $(LDFLAGS)

examples/example2: examples/example2.o $(FP_MODULES)
	$(LD) -o $@ $^ $(LDFLAGS)

ftest: util/ftest.o $(FP_MODULES)
	$(LD) -o $@ $^ $(LDFLAGS)

powi_speedtest: util/powi_speedtest.o $(FP_MODULES)
	$(LD) -o $@ $^ $(LDFLAGS)

koe: koe.o $(FP_MODULES)
	$(LD) -o $@ $^ $(LDFLAGS)

functioninfo: util/functioninfo.o $(FP_MODULES)
	$(LD) -o $@ $^ $(LDFLAGS)

fpoptimizer/grammar_data.cc: \
		util/tree_grammar_parser \
		fpoptimizer/treerules.dat
	ASAN_OPTIONS=detect_leaks=0 util/tree_grammar_parser < fpoptimizer/treerules.dat > $@

extrasrc/fp_opcode_add.inc: \
		util/bytecoderules_parser \
		util/bytecoderules.dat \
		util/bytecoderules_header.txt
	cat util/bytecoderules_header.txt > $@
	ASAN_OPTIONS=detect_leaks=0 util/bytecoderules_parser \
		< util/bytecoderules.dat \
		>> $@

tests/make_tests: tests/make_tests.o
	$(LD) -o $@ $^ $(LDFLAGS)

tests/make_tests.o: tests/make_tests.cc tests/stringutil.hh

$(wordlist 2,99,$(TESTBED_GENSRC)) : $(word 1,$(TESTBED_GENSRC)) ;
$(word 1,$(TESTBED_GENSRC)) : tests/make_tests $(wildcard tests/*/*) $(wildcard tests/*/*/*)
	tests/make_tests \
		--ignore "*~" \
		--ignore "*.php" \
		--ignore "*/.*" \
		--ignore "*.cc" \
		--ignore "*.hh" \
		--strip_prefix "tests/" \
		tests/*/* tests/*/*/*

FPOPTIMIZER_CC_FILES=\
	    lib/autoptr.hh \
	    lib/functional.hh \
	    fpoptimizer/hash.hh \
	    fpoptimizer/codetree.hh \
	    fpoptimizer/grammar.hh \
	    fpoptimizer/consts.hh \
	    fpoptimizer/optimize.hh \
	    fpoptimizer/opcodename.hh \
	    fpoptimizer/opcodename.cc \
	    fpoptimizer/bytecodesynth.hh \
	    fpoptimizer/bytecodesynth.cc \
	    fpoptimizer/valuerange.hh \
	    fpoptimizer/rangeestimation.hh \
	    fpoptimizer/constantfolding.hh \
	    fpoptimizer/logic_boolgroups.hh \
	    fpoptimizer/logic_collections.hh \
	    fpoptimizer/logic_ifoperations.hh \
	    fpoptimizer/logic_powoperations.hh \
	    fpoptimizer/logic_comparisons.hh \
	    fpoptimizer/codetree.cc \
	    fpoptimizer/debug.cc \
	    fpoptimizer/grammar.cc \
	    fpoptimizer/grammar_data.cc \
	    fpoptimizer/optimize.cc \
	    fpoptimizer/optimize_match.cc \
	    fpoptimizer/optimize_synth.cc \
	    fpoptimizer/optimize_debug.cc \
	    fpoptimizer/hash.cc \
	    fpoptimizer/makebytecode.cc \
	    fpoptimizer/readbytecode.cc \
	    fpoptimizer/constantfolding.cc \
	    fpoptimizer/valuerange.cc \
	    fpoptimizer/rangeestimation.cc \
	    fpoptimizer/transformations.cc \
	    fpoptimizer/cse.cc \
	    fpoptimizer/optimize_main.cc

fpoptimizer.cc: fpoptimizer/fpoptimizer_header.txt \
		fpoptimizer/fpoptimizer_footer.txt \
		$(FPOPTIMIZER_CC_FILES)
	rm -f fpoptimizer.cc
	cat fpoptimizer/fpoptimizer_header.txt  > $@
	for file in $(FPOPTIMIZER_CC_FILES); do \
		echo "/* Sourced from $$file */"; \
		echo "#line 1 \"$$file\""; \
		sed -r "s@^(#include \".*)@// line removed for fpoptimizer.cc: \\1@" < "$$file"; \
		echo; \
	done | sed 's@BEGIN_EXPLICIT_INSTANTATION.*@@;s@.*END_EXPLICIT_INSTANTATION@@' \
	     >> $@
	cat fpoptimizer/fpoptimizer_footer.txt >> $@

util/tree_grammar_parser: \
		util/tree_grammar_parser.o \
		fpoptimizer/opcodename.o \
		$(ADDITIONAL_MODULES)
	$(LD) -o $@ $^ $(LDFLAGS)

util/tree_grammar_parser.cc: \
		util/tree_grammar_parser.y
	bison --output=$@ $<
	sed -i 's/ *$$//' $@

util/bytecoderules_parser: util/bytecoderules_parser.o
	$(LD) -o $@ $^ $(LDFLAGS)


util/version_changer: util/version_changer.cc
	g++ -O3 $^ -s -o $@ $(LDFLAGS) $(CXXFLAGS) $(CPPFLAGS)

util/make_function_name_parser: util/make_function_name_parser.cc
	g++ -O3 $^ -s -o $@ $(LDFLAGS) $(CXXFLAGS) $(CPPFLAGS)

util/powi_opt: \
		util/powi_opt.o \
		fpoptimizer/hash.o \
		fpoptimizer/constantfolding.o \
		fpoptimizer/codetree.o \
		fpoptimizer/valuerange.o \
		fpoptimizer/rangeestimation.o
	g++ -O3 $^ -s -o $@ $(LDFLAGS) $(CXXFLAGS) $(CPPFLAGS)

util/create_testrules_for_optimization_rules: \
		util/create_testrules_for_optimization_rules.cc \
		fpoptimizer/grammar_data.o \
		fpoptimizer/opcodename.o \
		fpoptimizer/grammar.o
	g++ -O3 $^ -s -o $@ $(LDFLAGS) $(CXXFLAGS) $(CPPFLAGS)

fpoptimizer_tests.sh: util/create_testrules_for_optimization_rules
	./$< > $@
	chmod +x $@

set_version_string: util/version_changer
	util/version_changer $(RELEASE_VERSION) fparser.cc \
		fparser.hh fparser_mpfr.hh fparser_gmpint.hh fpconfig.hh \
		extrasrc/fptypes.hh extrasrc/fpaux.hh \
		extrasrc/fp_opcode_add.inc \
		fpoptimizer/fpoptimizer_header.txt \
		util/bytecoderules_header.txt \
		docs/fparser.html webpage/index.html

pack: set_version_string distro_pack devel_pack

F=fparser$(RELEASE_VERSION).zip
T=fparser$(RELEASE_VERSION)-tmp.zip
T2=fparser$(RELEASE_VERSION)-tmp2.zip

distro_pack: $(F) ;
$(F): $(RELEASE_PACK_FILES) ; rm -f $@
	# Use advzip (advancecomp), 7zip, or Info-ZIP, in that order, depending which one is available
	# The list of programs to try is ordered from best compression to worst.
	7z -mx -tzip a "$@" $^ || zip -9 "$@" $^
	# Try further optimize the file with KZIP & ZIPMIX (from advsys.net/ken)
	- $(MAKE) distro_pack_optimize_kzip_options
	# Try further optimize the file with advzip (in-place)
	- advzip -k -z4 -i8 "$@" || advzip -z3 -i8 "$@"
	# Try further optimize the file with DeflOpt (in-place)
	- DeflOpt "$@"
distro_pack_optimize_kzip_options:
	# Make sure ZIPMIX exists, before trying to run KZIP
	zipmix /dev/null /dev/null
	# Run KZIP with a number of different block sizes
	$(MAKE) distro_pack_optimize_kzip KZIP_OPTION=-b0
	$(MAKE) distro_pack_optimize_kzip KZIP_OPTION=-b128
	$(MAKE) distro_pack_optimize_kzip KZIP_OPTION=-b256
	$(MAKE) distro_pack_optimize_kzip KZIP_OPTION=-b512
	$(MAKE) distro_pack_optimize_kzip KZIP_OPTION=-b1024
	# Run KZIP a couple of times with random options
	$(MAKE) distro_pack_optimize_kzip KZIP_OPTION=-rn
	$(MAKE) distro_pack_optimize_kzip KZIP_OPTION=-rn
	$(MAKE) distro_pack_optimize_kzip KZIP_OPTION=-rn
	$(MAKE) distro_pack_optimize_kzip KZIP_OPTION=-rn
	$(MAKE) distro_pack_optimize_kzip KZIP_OPTION=-rn
	$(MAKE) distro_pack_optimize_kzip KZIP_OPTION=-rn
	# Clean up
	- rm -f $(T) $(T2)
distro_pack_optimize_kzip:
	# Compress with KZIP into a temp file. Try also optimizing it with DeflOpt.
	kzip -r -y $(KZIP_OPTION) $(T) $(RELEASE_PACK_FILES)
	- DeflOpt $(T)
	# Mix best compressions from target and temp file
	zipmix -y $(F) $(T) $(T2)
	mv -f $(T2) $(F)

devel_pack:
	tar --exclude='*~' \
		--transform="s|^|fparser_$(RELEASE_VERSION)_devel/|" \
		-cjvf fparser$(RELEASE_VERSION)_devel.tar.bz2 \
		Makefile examples/example.cc examples/example2.cc fparser.cc \
		fparser.hh fparser_mpfr.hh fparser_gmpint.hh \
		fpconfig.hh extrasrc/fptypes.hh extrasrc/fpaux.hh \
		extrasrc/fp_opcode_add.inc \
		extrasrc/fp_identifier_parser.inc \
		tests/testbed_autogen.hh \
		util/speedtest.cc testbed.cc \
		tests/*.cc tests/*.txt tests/*/* \
		util/*.cc util/*.hh util/*.dat util/*.txt util/*.y \
		docs/fparser.html docs/style.css docs/lgpl.txt docs/gpl.txt \
		fpoptimizer/*.hh fpoptimizer/*.cc \
		fpoptimizer/*.dat \
		fpoptimizer/*.txt \
		lib/*.hh \
		mpfr/MpfrFloat.hh mpfr/MpfrFloat.cc \
		mpfr/GmpInt.hh mpfr/GmpInt.cc \
		run_full_release_testing.sh \
		util/functioninfo.cc

clean:
	rm -f	testbed testbed_release \
		speedtest speedtest_release \
		functioninfo \
		examples/example examples/example2 ftest powi_speedtest \
		util/tree_grammar_parser \
		tests/make_tests \
		util/bytecoderules_parser \
		util/make_function_name_parser \
		examples/*.o \
		fpoptimizer/*.o \
		tests/*.o \
		mpfr/*.o \
		util/*.o \
		*.o \
		.dep \
		util/tree_grammar_parser.output \
		$(TESTBED_GENSRC)

release_clean:
	rm -f testbed_release speedtest_release \
		$(TESTBED_MODULES) fparser.o fpoptimizer.o

distclean: clean
	rm -f	*~

.dep:
	echo -n '' > .dep
	- g++ -MM -MG $(CPPFLAGS) $(wildcard *.cc) >> .dep
	- g++ -MM $(CPPFLAGS) $(wildcard examples/*.cc) | sed 's|^.*.o:|examples/&|' >> .dep
	- g++ -MM $(CPPFLAGS) $(wildcard fpoptimizer/*.cc) | sed 's|^.*.o:|fpoptimizer/&|' >> .dep
	- g++ -MM $(CPPFLAGS) $(wildcard tests/*.cc) | sed 's|^.*.o:|tests/&|' >> .dep
	- g++ -MM $(CPPFLAGS) $(wildcard util/*.cc) | sed 's|^.*.o:|util/&|' >> .dep
	- g++ -MM $(CPPFLAGS) $(wildcard mpfr/*.cc) | sed 's|^.*.o:|mpfr/&|' >> .dep
	- g++ -MM $(CPPFLAGS) $(wildcard lib/*.cc) | sed 's|^.*.o:|lib/&|' >> .dep
	sed -i "s@`pwd`/@@" .dep

-include .dep
