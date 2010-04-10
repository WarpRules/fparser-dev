#include "MpfrFloat.hh"
#include "GmpInt.hh"
#include <cmath>

int main()
{
    const double x = -0.99;
    std::cout << asinh(x) << "\n"
              << MpfrFloat::asinh(x) << std::endl;
}
