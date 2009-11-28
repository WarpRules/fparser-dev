#include "MpfrFloat.hh"
#include "GmpInt.hh"

int main()
{
    MpfrFloat a;
    a.parseValue("1234567890.1234567890123456789012345678901234567890");
    MpfrFloat b = 100.0;
    std::cout.precision(50);
    std::cout << "sqrt(" << a << " + " << b << ") = "
              << MpfrFloat::sqrt(a+b) << std::endl;

    GmpInt i1;
    i1.parseValue("-123456789012345678901234567890");
    GmpInt i2 = 1000;
    std::cout << i1 << " + " << i2 << " = " << i1 + i2 << std::endl;
}
