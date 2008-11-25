#define FP_GENERATING_POWI_TABLE
extern unsigned char powi_table[256];

#include "fpoptimizer.cc"

int main()
{
    for(long exponent = 2; exponent < 256; ++exponent)
    {
        CodeTree ct;
        SubTree  st;

        double bestres = 0;
        long bestp = 1;
        
        for(long p=1; p<=exponent/2; ++p)
        {
            std::vector<unsigned> byteCode;
            std::vector<double>   immed;
            size_t stacktop_cur=1;
            size_t stacktop_max=1;
            
            powi_table[exponent] = p;
            
            //fprintf(stderr, "For %ld, trying %ld\n", exponent, p);
            
            ct.AssembleSequence(st,
                exponent,
                1,0,0,
                byteCode,immed,stacktop_cur,stacktop_max);
        
            double res = 0;
            for(size_t a=0; a<byteCode.size(); ++a)
            {
                if(byteCode[a] == cMul)
                    res += 5;
                else if(byteCode[a] == cDup)
                    res += 1;
                else if(byteCode[a] == cPopNMov)
                    { res += 3.5; a += 2; } 
                else if(byteCode[a] == cFetch)
                    { res += 2.5; a += 1; }
            }
            
            if(res <= bestres || p == 1)
            {
                bestp   = p;
                bestres = res;
            }
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
