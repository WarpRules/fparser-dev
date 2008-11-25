#include "fparser.hh"

#include <iostream>
#include <cstdio>
#include <string>
#include <sstream>

unsigned generateOpcodesForExp(unsigned n, bool print)
{
    unsigned retval = 0;
    if(n > 1)
    {
        if(n % 2 == 1)
        {
            if(print) std::cout << "dup ";
            retval = 2 + generateOpcodesForExp(n-1, print);
            if(print) std::cout << "mul ";
        }
        else
        {
            retval = 2 + generateOpcodesForExp(n/2, print);
            if(print) std::cout << "dup mul ";
        }
    }
    return retval;
}

unsigned getParserOpcodesAmount(const std::string& func)
{
    FunctionParser fp;
    std::string line;

    fp.Parse(func, "x");
    fp.Optimize();
    std::ostringstream buf;
    fp.PrintByteCode(buf);
    std::istringstream lines(buf.str());
    unsigned linesAmount = 0;
    while(std::getline(lines, line).good()) ++linesAmount;
    return linesAmount - 1;
}

int main()
{
    std::printf
        ("Number of opcodes generated:\n"
         "Func Naive Bisq    Func Naive Bisq    Func Naive Bisq    Func Naive Bisq\n"
         "---- ----- ----    ---- ----- ----    ---- ----- ----    ---- ----- ----\n");

    for(unsigned i = 0; i < 100; ++i)
    {
        for(unsigned col = 0; col < 4; ++col)
        {
            const unsigned exponent = i + 100*col;

            std::ostringstream funcStream;
            if(exponent < 10) funcStream << " ";
            funcStream << "x^" << exponent;
            const std::string func = funcStream.str();

            const unsigned naiveOpcodes =
                exponent < 2 ? 1 : generateOpcodesForExp(exponent, false)+1;

            const unsigned bisqOpcodes = getParserOpcodesAmount(func);

            std::printf("%s: %4u %4u   ", func.c_str(),
                        naiveOpcodes, bisqOpcodes);
        }
        std::printf("\n");
    }
    return 0;

    for(unsigned i = 2; i < 20; ++i)
    {
        std::cout << "x^" << i << ": ";
        unsigned amount = generateOpcodesForExp(i, true);
        std::cout << ": " << amount << "\n";
    }
    return 0;


    std::string function;
    FunctionParser fparser;

    while(true)
    {
        std::cout << "f(x) = ";
        std::getline(std::cin, function);
        int res = fparser.Parse(function, "x");

        if(res >= 0)
            std::cout << std::string(res+7, ' ') << "^\n"
                      << fparser.ErrorMsg() << "\n\n";
        else
        {
            std::cout << "------- Normal: -------\n";
            fparser.PrintByteCode(std::cout);
            std::cout << "------- Optimized: -------\n";
            fparser.Optimize();
            fparser.PrintByteCode(std::cout);
        }
    }
}
