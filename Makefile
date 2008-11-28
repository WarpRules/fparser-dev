CXX=g++ -Wall -W -pedantic -ansi -O3 -ffast-math -march=native -DFUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT -DFP_ENABLE_EVAL
#CXX=g++ -Wall -W -pedantic -ansi -g -DFUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT -DFP_ENABLE_EVAL
#LD=g++ -s
LD=g++

all: testbed speedtest example ftest powi_speedtest

FP_MODULES=\
		fparser.o \
		fpoptimizer.o \
		fpoptimizer_bytecode_to_codetree.o \
		fpoptimizer_codetree_to_bytecode.o \
		fpoptimizer_codetree.o

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

pack:\
		example.cc fparser.cc fparser.hh fparser.txt fpconfig.hh \
		fptypes.hh \
		fpoptimizer.hh \
		fpoptimizer_consts.hh \
		fpoptimizer.cc \
		fpoptimizer_codetree.cc \
		fpoptimizer_codetree_to_bytecode.cc \
		fpoptimizer_bytecode_to_codetree.cc
	zip -9 fparser3.0.3.zip $^

#%.o:		%.cc fparser.hh
#	$(CXX) -c $<

clean:
	rm *.o

.dep:
	g++ -MM $(wildcard *.cc) > .dep

-include .dep
