#include "fparser.hh"

#include <iostream>
#include <cstdio>
#include <string>
#include <sstream>

struct Counts { unsigned opcodes, muls; };

Counts generateOpcodesForExp(unsigned n, bool print)
{
    Counts retval = { 0, 0 };
    if(n > 1)
    {
        if(n % 2 == 1)
        {
            if(print) std::cout << "dup ";
            retval = generateOpcodesForExp(n-1, print);
            retval.opcodes += 2;
            ++retval.muls;
            if(print) std::cout << "mul ";
        }
        else
        {
            retval = generateOpcodesForExp(n/2, print);
            ++retval.opcodes;
            ++retval.muls;
            if(print) std::cout << "sqr ";
        }
    }
    return retval;
}

Counts getParserOpcodesAmount(const std::string& func)
{
    FunctionParser fp;
    std::string line;

    fp.Parse(func, "x");
    fp.Optimize();
    std::ostringstream buf;
    fp.PrintByteCode(buf);
    std::istringstream lines(buf.str());

    Counts counts = { 0, 0 };
    while(std::getline(lines, line).good())
    {
        ++counts.opcodes;
        if(line.substr(line.size()-3) == "mul") ++counts.muls;
    }
    --counts.opcodes;
    return counts;
}

int main()
{
    std::printf
        ("Number of opcodes generated:\n"
         "Func     Naive     Bisq   Func      Naive     Bisq   Func      Naive     Bisq   Func      Naive     Bisq\n"
         "----     -----     ----   ----      -----     ----   ----      -----     ----   ----      -----     ----\n");

    const Counts minimum = { 0, 0 };

    for(unsigned i = 0; i < 100; ++i)
    {
        for(unsigned col = 0; col < 4; ++col)
        {
            const unsigned exponent = i + 100*col;

            std::ostringstream funcStream;
            if(exponent < 10) funcStream << " ";
            funcStream << "x^" << exponent;
            const std::string func = funcStream.str();

            Counts naiveOpcodes = exponent < 2 ? minimum :
                generateOpcodesForExp(exponent, false);
            ++naiveOpcodes.opcodes;

            const Counts bisqOpcodes = getParserOpcodesAmount(func);

            std::printf("%s: %3u (%2u) %3u (%2u)   ", func.c_str(),
                        naiveOpcodes.opcodes, naiveOpcodes.muls,
                        bisqOpcodes.opcodes, bisqOpcodes.muls);
        }
        std::printf("\n");
    }
    return 0;

    for(unsigned i = 2; i < 20; ++i)
    {
        std::cout << "x^" << i << ": ";
        unsigned amount = generateOpcodesForExp(i, true).opcodes;
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
