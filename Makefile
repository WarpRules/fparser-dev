CXX=g++ -Wall -W -pedantic -ansi -O3 -ffast-math -march=native -DFUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT -DFP_ENABLE_EVAL
#CXX=g++ -Wall -W -pedantic -ansi -g -DFUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT -DFP_ENABLE_EVAL
#LD=g++ -s
LD=g++

all: testbed speedtest example ftest powi_speedtest

testbed: testbed.o fparser.o fpoptimizer.o
	$(LD) -o $@ $^

speedtest: speedtest.o fparser.o fpoptimizer.o
	$(LD) -o $@ $^

example: example.o fparser.o fpoptimizer.o
	$(LD) -o $@ $^

ftest: ftest.o fparser.o fpoptimizer.o
	$(LD) -o $@ $^

powi_speedtest: powi_speedtest.o fparser.o fpoptimizer.o
	$(LD) -o $@ $^

pack: example.cc fparser.cc fparser.hh fparser.txt fpconfig.hh fpoptimizer.cc fptypes.hh
	zip -9 fparser3.0.3.zip $^

#%.o: %.cc fparser.hh
#	$(CXX) -c $<

clean:
	rm *.o

.dep:
	g++ -MM $(wildcard *.cc) > .dep

-include .dep
