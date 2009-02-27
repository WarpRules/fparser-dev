CXX=g++ -Wall -W -pedantic -ansi -O3 -ffast-math -march=native -DFUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT -DFP_ENABLE_EVAL
#CXX=g++ -Wall -W -pedantic -ansi -O3 -ffast-math -march=native -DFUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT -DFP_ENABLE_EVAL -DFP_SUPPORT_ASINH
#CXX=g++ -Wall -W -pedantic -ansi -g -DFUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT -DFP_ENABLE_EVAL -DFP_SUPPORT_ASINH
#LD=g++ -s
LD=g++

#CXX += -g
#LD  += -g

#CXX += -pg
#LD  += -pg

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

fpoptimizer.cc: fpoptimizer/fpoptimizer_grammar_gen.y \
		fpoptimizer/fpoptimizer_grammar_gen.cc \
		fpoptimizer/fpoptimizer_codetree.hh \
		fpoptimizer/fpoptimizer_grammar.hh \
		fpoptimizer/fpoptimizer_consts.hh \
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

pack: example.cc fparser.cc fparser.hh fpoptimizer.cc fparser.txt \
	fpconfig.hh fptypes.hh fparser.html style.css
	zip -9 fparser3.1b.zip $^

devel_pack:
	tar -cjvf fparser3.1b_devel.tar.bz2 \
	Makefile example.cc fparser.cc fparser.hh fparser.txt fpconfig.hh \
	fptypes.hh speedtest.cc testbed.cc fparser.html style.css \
	fpoptimizer/*.hh fpoptimizer/*.cc fpoptimizer/fpoptimizer.dat \
	fpoptimizer/*.txt fpoptimizer/fpoptimizer_grammar_gen.y

clean:
	rm -f	testbed testbed_release speedtest example ftest \
		powi_speedtest fpoptimizer/fpoptimizer_grammar_gen \
		*.o fpoptimizer/*.o .dep \
		fpoptimizer/fpoptimizer_grammar_gen.output

distclean: clean
	rm -f	*~

.dep:
	g++ -MM $(wildcard *.cc) > .dep
	g++ -MM $(wildcard fpoptimizer/*.cc) | sed 's|^.*.o:|fpoptimizer/&|' >> .dep

-include .dep
