CXX=g++ -Wall -W -pedantic -ansi -O3 -ffast-math -march=native -DFUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT -DFP_ENABLE_EVAL -g -Weffc++
#CXX=g++ -Wall -W -pedantic -ansi -g -DFUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT -DFP_ENABLE_EVAL
#LD=g++ -s
LD=g++ -g

all: testbed speedtest example ftest powi_speedtest

FP_MODULES=\
		fparser.o \
		fpoptimizer.o \
		fpoptimizer_bytecode_to_codetree.o \
		fpoptimizer_codetree_to_bytecode.o \
		fpoptimizer_codetree.o \
		fpoptimizer_grammar.o \
		fpoptimizer_optimize.o

testbed:\
		testbed.o $(FP_MODULES)
	$(LD) -o $@ $^

speedtest:\
		speedtest.o $(FP_MODULES)
	$(LD) -o $@ $^

example:\
		example.o $(FP_MODULES)
	$(LD) -o $@ $^

ftest:\
		ftest.o $(FP_MODULES)
	$(LD) -o $@ $^

powi_speedtest:\
		powi_speedtest.o $(FP_MODULES)
	$(LD) -o $@ $^


fpoptimizer_grammar_gen: \
		fpoptimizer_grammar_gen.o
	$(LD) -o $@ $^

fpoptimizer_grammar_gen.cc: \
		fpoptimizer_grammar_gen.y
	bison++ --output=$@ $<

fpoptimizer_grammar.cc: \
		fpoptimizer_grammar_gen \
		fpoptimizer.dat
	./$<  < fpoptimizer.dat  > $@

pack:\
		example.cc fparser.cc fparser.hh fparser.txt fpconfig.hh \
		fptypes.hh \
		fpoptimizer_grammar_gen.y \
		fpoptimizer_grammar_gen.cc \
		fpoptimizer_grammar_gen.h \
		fpoptimizer.hh \
		fpoptimizer_consts.hh \
		fpoptimizer.cc \
		fpoptimizer_codetree.cc \
		fpoptimizer_grammar.cc \
		fpoptimizer_optimize.cc \
		fpoptimizer_codetree_to_bytecode.cc \
		fpoptimizer_bytecode_to_codetree.cc
	zip -9 fparser3.0.3.zip $^

#%.o:		%.cc fparser.hh
#	$(CXX) -c $<

clean:
	rm -f	testbed speedtest example ftest \
		powi_speedtest fpoptimizer_grammar_gen \
		*.o \
		fpoptimizer_grammar_gen.cc \
		fpoptimizer_grammar_gen.output \
		fpoptimizer_grammar_gen.h \
		fpoptimizer_grammar.cc
distclean: clean
	rm -f	*~

.dep:
	g++ -MM $(wildcard *.cc) > .dep

-include .dep
