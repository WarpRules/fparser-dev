#include <cstdio>
#include "cpp_compress.hh"

int main(int argc, char* argv[])
{
    std::string out;
    for(;;)
    {
        char Buf[32768];
        if(!std::fgets(Buf, sizeof(Buf), stdin)) break;
        Buf[32767] = '\0';
        out += Buf;
    }
    CPPcompressor Compressor;

    //outStream << out.str();
    std::cout << Compressor.Compress(out);
}
