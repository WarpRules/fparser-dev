CXX=g++ -Wall -W -pedantic -ansi -O3 -ffast-math -march=pentium4 -DFUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT -DFP_ENABLE_EVAL
#CXX=g++ -Wall -W -pedantic -ansi -g -O0
LD=g++ -s

testbed: testbed.o fparser.o fpoptimizer.o
	$(LD) -o $@ $^

speedtest: speedtest.o fparser.o fpoptimizer.o
	$(LD) -o $@ $^

example: example.o fparser.o fpoptimizer.o
	$(LD) -o $@ $^

pack: example.cc fparser.cc fparser.hh fparser.txt fpconfig.hh fpoptimizer.cc fptypes.hh
	zip -9 fparser284.zip $^

#%.o: %.cc fparser.hh
#	$(CXX) -c $<

clean:
	rm *.o

.dep:
	g++ -MM $(wildcard *.cc) > .dep

-include .dep
