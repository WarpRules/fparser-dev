#CXX=g++ -Wall -W -pedantic -ansi -O3 -ffast-math -m32 -march=pentium4 -DFUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT -DFP_ENABLE_EVAL
CXX=g++ -Wall -W -pedantic -ansi -O3 -ffast-math -DFUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT -DFP_ENABLE_EVAL
#CXX=g++ -Wall -W -pedantic -ansi -g -O0 -DFUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT -DFP_ENABLE_EVAL
LD=g++ -s

testbed: testbed.o fparser.o fpoptimizer.o
	$(LD) -o $@ $^

speedtest: speedtest.o fparser.o fpoptimizer.o
	$(LD) -o $@ $^

example: example.o fparser.o fpoptimizer.o
	$(LD) -o $@ $^

pack: example.cc fparser.cc fparser.hh fparser.txt fpconfig.hh fpoptimizer.cc fptypes.hh
	zip -9 fparser284.zip $^

fparser.cc: fparser.cc.re fparser-parsingdefs.inc Makefile
	re2c -bs $< \
	| sed 's/static unsigned char yybm/static const unsigned char yybm/' \
	| sed "s@<<PARSING_DEFS_PLACEHOLDER>>@`tr '\012' '§' < fparser-parsingdefs.inc`@" \
	| tr '§' '\012' \
	> $@

fparser-parsingtree.output: fparser.y
	bison $< -dv -o fparser-parsingtree.c
	rm -f fparser-parsingtree.[ch]

fparser-parsingdefs.inc: \
		bison-parser-convert.php fparser-parsingtree.output  /usr/bin/php
	php -q $< fparser-parsingtree.output > $@

#%.o: %.cc fparser.hh
#	$(CXX) -c $<

clean:
	rm *.o

.dep:
	g++ -MM $(wildcard *.cc) > .dep

-include .dep
