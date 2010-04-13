#include "MpfrFloat.hh"
#include "GmpInt.hh"
#include <cmath>

int main()
{
    MpfrFloat x = MpfrFloat::parseString("0x123zzz", 0);
    std::cout << x << std::endl;
}
