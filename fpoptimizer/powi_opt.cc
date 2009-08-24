#include <iostream>

#define FP_GENERATING_POWI_TABLE
extern signed char powi_table[256];

#include "fpoptimizer_synth.hh"

#include <iomanip>
namespace
{
    inline void printHex(std::ostream& dest, unsigned n)
    {
        dest.width(8); dest.fill('0'); std::hex(dest); //uppercase(dest);
        dest << n;
    }
}

static
void PrintByteCode(const std::vector<unsigned>& ByteCode,
                   const std::vector<double>& Immed,
                   std::ostream& dest)
{
    for(unsigned IP = 0, DP = 0; IP < ByteCode.size(); ++IP)
    {
        printHex(dest, IP);
        dest << ": ";

        unsigned opcode = ByteCode[IP];

        switch(opcode)
        {
          case cIf:
              dest << "jz\t";
              printHex(dest, ByteCode[IP+1]+1);
              dest << endl;
              IP += 2;
              break;

          case cJump:
              dest << "jump\t";
              printHex(dest, ByteCode[IP+1]+1);
              dest << endl;
              IP += 2;
              break;
          case cImmed:
              dest.precision(10);
              dest << "push\t" << Immed[DP++] << endl;
              break;

          default:
              if(OPCODE(opcode) < VarBegin)
              {
                  string n;
                  unsigned params = 1;
                  switch(opcode)
                  {
                    case cNeg: n = "neg"; break;
                    case cAdd: n = "add"; break;
                    case cSub: n = "sub"; break;
                    case cMul: n = "mul"; break;
                    case cDiv: n = "div"; break;
                    case cMod: n = "mod"; break;
                    case cPow: n = "pow"; break;
                    case cEqual: n = "eq"; break;
                    case cNEqual: n = "neq"; break;
                    case cLess: n = "lt"; break;
                    case cLessOrEq: n = "le"; break;
                    case cGreater: n = "gt"; break;
                    case cGreaterOrEq: n = "ge"; break;
                    case cAnd: n = "and"; break;
                    case cOr: n = "or"; break;
                    case cNot: n = "not"; break;
                    case cDeg: n = "deg"; break;
                    case cRad: n = "rad"; break;

#ifndef FP_DISABLE_EVAL
                    case cEval: n = "call\t0"; break;
#endif

#ifdef FP_SUPPORT_OPTIMIZER
                    case cVar: n = "(var)"; break;
                    case cDup: n = "dup"; break;
                    case cInv: n = "inv"; break;
                    case cSqr: n = "sqr"; break;
                    case cFetch:
                        dest << "cFetch(" << ByteCode[++IP] << ")";
                        break;
                    case cPopNMov:
                    {
                        size_t a = ByteCode[++IP];
                        size_t b = ByteCode[++IP];
                        dest << "cPopNMov(" << a << ", " << b << ")";
                        break;
                    }
#endif

                    default:
                        n = Functions[opcode-cAbs].name;
                        params = Functions[opcode-cAbs].params;
                  }
                  dest << n;
                  if(params != 1) dest << " (" << params << ")";
                  dest << endl;
              }
              else
              {
                  dest << "push\tVar" << opcode-VarBegin << endl;
              }
        }
    }
}

int main()
{
    for(long exponent = 2; exponent < 256; ++exponent)
    {
        CodeTree ct;
        SubTree  st;

        double bestres = 0;
        long bestp = -1;

        /* x^40 / x^5 (rdiv) cannot be used when x=0 */
        for(long p=1/*-128*/; p<=exponent/2; ++p)
        {
            if(!p) continue;

            std::vector<unsigned> byteCode;
            std::vector<double>   immed;
            size_t stacktop_cur=1;
            size_t stacktop_max=1;

            powi_table[exponent] = p;

            fprintf(stderr, "For %ld, trying %ld... ", exponent, p);

            try {
                ct.AssembleSequence(st,
                    exponent,
                    MulSequence,
                    byteCode,immed,stacktop_cur,stacktop_max);
            }
            catch(bool)
            {
                fprintf(stderr, "Didn't work\n");
                continue;
            }

            double res = 0;
            for(size_t a=0; a<byteCode.size(); ++a)
            {
                if(byteCode[a] == cMul)
                    res += 5;
                else if(byteCode[a] == cDiv || byteCode[a] == cRDiv)
                    res += 5 * 1.25;
                else if(byteCode[a] == cSqr)
                    res += 5;
                else if(byteCode[a] == cDup)
                    res += 1;
                else if(byteCode[a] == cPopNMov)
                    { res += 3.5; a += 2; }
                else if(byteCode[a] == cFetch)
                    { res += 2.5; a += 1; }
            }

            fprintf(stderr, "gets %g", res);
            if(res <= bestres || bestp == -1)
            {
                fprintf(stderr, " -- beats %g (%ld)\n", bestres, bestp);
                bestp   = p;
                bestres = res;
            }
            fprintf(stderr, "\n");
            fflush(stderr);

            PrintByteCode(byteCode, immed, std::cout);
        }
        powi_table[exponent] = bestp;
    }
    for(unsigned n=0; n<256; ++n)
    {
        if(n%8 == 0) printf("   ");
        printf("%4d,", powi_table[n]);
        if(n%8 == 7) printf(" /*%4d -%4d */\n", n&~7, (n&~7)|7);
    }
}
