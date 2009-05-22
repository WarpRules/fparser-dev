#include "fparser.hh"

#include <iostream>
#include <string>

int main()
{
    std::string function;
    FunctionParser fparser;

    while(std::cin)
    {
        while(true)
        {
            std::cout << "f(x,y,z) = ";
            std::getline(std::cin, function);
            if(std::cin.fail()) return 0;

            int res = fparser.Parse(function, "x,y,z");
            if(res < 0) break;

            std::cout << std::string(res+11, ' ') << "^\n"
                      << fparser.ErrorMsg() << "\n\n";
        }

        std::cout << "----- ORIGINAL -----\n";
        fparser.PrintByteCode(std::cout);
        std::cout << "----- OPTIMIZED -----\n";
        fparser.Optimize();
        fparser.PrintByteCode(std::cout);
    }
}
