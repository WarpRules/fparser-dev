#include "MpfrFloat.hh"

int main()
{
    MpfrFloat a("1234567890.1234567890123456789012345678901234567890");
    MpfrFloat b = 100;
    std::cout << a+b << std::endl;
}
