// Simple example file for the function parser
// ===========================================

/* Try for example with these values:

f(x) = x^2
min x: -5
max x: 5
step: 1

*/

#include "fparser.hh"

#include <iostream>
#include <string>

int main()
{
    std::string function;
    double minx, maxx, step;
    FunctionParser fparser;

    while(true)
    {
        std::cout << "f(x) = ";
        std::getline(std::cin, function);
        int res = fparser.Parse(function, "x");
        if(res < 0) break;

        for(int i = 0; i < res+7; ++i) std::cout << " ";
        std::cout << "^\n" << fparser.ErrorMsg() << "\n\n";
    }

    std::cout << "min x: ";
    std::cin >> minx;
    std::cout << "max x: ";
    std::cin >> maxx;
    std::cout << "step: ";
    std::cin >> step;

    double vals[1];
    for(vals[0] = minx; vals[0] <= maxx; vals[0] += step)
        std::cout << "f(" << vals[0] << ") = " << fparser.Eval(vals)
                  << std::endl;

    return 0;
}
