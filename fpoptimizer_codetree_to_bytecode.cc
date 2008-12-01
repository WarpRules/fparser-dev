#include <cmath>
#include <list>
#include <cassert>

#include "fpoptimizer_codetree.hh"
#include "fptypes.hh"
#include "fpoptimizer_consts.hh"


using namespace FUNCTIONPARSERTYPES;
//using namespace FPoptimizer_Grammar;

#ifndef FP_GENERATING_POWI_TABLE
static const unsigned MAX_POWI_BYTECODE_LENGTH = 15;
#else
static const unsigned MAX_POWI_BYTECODE_LENGTH = 999;
#endif
static const unsigned MAX_MULI_BYTECODE_LENGTH = 5;

#define POWI_TABLE_SIZE 256
#define POWI_WINDOW_SIZE 3
#ifndef FP_GENERATING_POWI_TABLE
static const
#endif
signed char powi_table[POWI_TABLE_SIZE] =
{
      0,   1,   1,   1,   2,   1,   3,   1, /*   0 -   7 */
      4,   1,   5,   1,   6,   1,  -2,   5, /*   8 -  15 */
      8,   1,   9,   1,  10,  -3,  11,   1, /*  16 -  23 */
     12,   5,  13,   9,  14,   1,  15,   1, /*  24 -  31 */
     16,   1,  17,  -5,  18,   1,  19,  13, /*  32 -  39 */
     20,   1,  21,   1,  22,   9,  -2,   1, /*  40 -  47 */
     24,   1,  25,  17,  26,   1,  27,  11, /*  48 -  55 */
     28,   1,  29,   8,  30,   1,  -2,   1, /*  56 -  63 */
     32,   1,  33,   1,  34,   1,  35,   1, /*  64 -  71 */
     36,   1,  37,  25,  38, -11,  39,   1, /*  72 -  79 */
     40,   9,  41,   1,  42,  17,   1,  29, /*  80 -  87 */
     44,   1,  45,   1,  46,  -3,  32,  19, /*  88 -  95 */
     48,   1,  49,  33,  50,   1,  51,   1, /*  96 - 103 */
     52,  35,  53,   8,  54,   1,  55,  37, /* 104 - 111 */
     56,   1,  57,  -5,  58,  13,  59, -17, /* 112 - 119 */
     60,   1,  61,  41,  62,  25,  -2,   1, /* 120 - 127 */
     64,   1,  65,   1,  66,   1,  67,  45, /* 128 - 135 */
     68,   1,  69,   1,  70,  48,  16,   8, /* 136 - 143 */
     72,   1,  73,  49,  74,   1,  75,   1, /* 144 - 151 */
     76,  17,   1,  -5,  78,   1,  32,  53, /* 152 - 159 */
     80,   1,  81,   1,  82,  33,   1,   2, /* 160 - 167 */
     84,   1,  85,  57,  86,   8,  87,  35, /* 168 - 175 */
     88,   1,  89,   1,  90,   1,  91,  61, /* 176 - 183 */
     92,  37,  93,  17,  94,  -3,  64,   2, /* 184 - 191 */
     96,   1,  97,  65,  98,   1,  99,   1, /* 192 - 199 */
    100,  67, 101,   8, 102,  41, 103,  69, /* 200 - 207 */
    104,   1, 105,  16, 106,  24, 107,   1, /* 208 - 215 */
    108,   1, 109,  73, 110,  17, 111,   1, /* 216 - 223 */
    112,  45, 113,  32, 114,   1, 115, -33, /* 224 - 231 */
    116,   1, 117,  -5, 118,  48, 119,   1, /* 232 - 239 */
    120,   1, 121,  81, 122,  49, 123,  13, /* 240 - 247 */
    124,   1, 125,   1, 126,   1,  -2,  85  /* 248 - 255 */
}; /* as in gcc, but custom-optimized for stack calculation */
static const int POWI_CACHE_SIZE = 256;

static const struct SequenceOpCode
{
    double basevalue;
    unsigned op_flip;
    unsigned op_normal, op_normal_flip;
    unsigned op_inverse, op_inverse_flip;
} AddSequence = {0.0, cNeg, cAdd, cAdd, cSub, cRSub },
  MulSequence = {1.0, cInv, cMul, cMul, cDiv, cRDiv };

class FPoptimizer_CodeTree::CodeTree::ByteCodeSynth
{
public:
    ByteCodeSynth()
        : ByteCode(), Immed(), StackTop(0), StackMax(0)
    {
    }

    void Pull(std::vector<unsigned>& bc,
              std::vector<double>&   imm,
              size_t& StackTop_max)
    {
        ByteCode.swap(bc);
        Immed.swap(imm);
        StackTop_max = StackMax;
    }

    size_t GetByteCodeSize() const { return ByteCode.size(); }
    size_t GetStackTop()     const { return StackTop; }

    void PushVar(unsigned varno)
    {
        ByteCode.push_back(varno);
        SetStackTop(StackTop+1);
    }

    void PushImmed(double immed)
    {
        ByteCode.push_back(cImmed);
        Immed.push_back(immed);
        SetStackTop(StackTop+1);
    }

    void StackTopIs(uint_fast64_t hash)
    {
        if(StackTop > 0)
        {
            StackHash[StackTop-1].first = true;
            StackHash[StackTop-1].second = hash;
        }
    }

    void AddOperation(unsigned opcode, unsigned eat_count, unsigned produce_count = 1)
    {
        SetStackTop(StackTop - eat_count);

        if(opcode == cMul && ByteCode.back() == cDup)
            ByteCode.back() = cSqr;
        else
            ByteCode.push_back(opcode);
        SetStackTop(StackTop + produce_count);
    }

    void DoPopNMov(size_t targetpos, size_t srcpos)
    {
        ByteCode.push_back(cPopNMov);
        ByteCode.push_back(targetpos);
        ByteCode.push_back(srcpos);
        
        SetStackTop(srcpos+1);
        StackHash[targetpos] = StackHash[srcpos];
        SetStackTop(targetpos+1);
    }

    void DoDup(size_t src_pos)
    {
        if(src_pos == StackTop-1)
        {
            ByteCode.push_back(cDup);
        }
        else
        {
            ByteCode.push_back(cFetch);
            ByteCode.push_back(src_pos);
        }
        SetStackTop(StackTop + 1);
        StackHash[StackTop-1] = StackHash[src_pos];
    }

    bool FindAndDup(uint_fast64_t hash)
    {
        for(size_t a=StackHash.size(); a-->0; )
        {
            if(StackHash[a].first && StackHash[a].second == hash)
            {
                DoDup(a);
                return true;
            }
        }
        return false;
    }

    void SynthIfStep1(size_t& ofs)
    {
        SetStackTop(StackTop-1); // the If condition was popped.

        ofs = ByteCode.size();
        ByteCode.push_back(cIf);
        ByteCode.push_back(0); // code index
        ByteCode.push_back(0); // Immed index
    }
    void SynthIfStep2(size_t& ofs)
    {
        SetStackTop(StackTop-1); // ignore the pushed then-branch result.

        ByteCode[ofs+1] = ByteCode.size()+2;
        ByteCode[ofs+2] = Immed.size();

        ofs = ByteCode.size();
        ByteCode.push_back(cJump);
        ByteCode.push_back(0); // code index
        ByteCode.push_back(0); // Immed index
    }
    void SynthIfStep3(size_t& ofs)
    {
        SetStackTop(StackTop-1); // ignore the pushed else-branch result.

        ByteCode[ofs+1] = ByteCode.size()-1;
        ByteCode[ofs+2] = Immed.size();

        SetStackTop(StackTop+1); // one or the other was pushed.
    }

private:
    void SetStackTop(size_t value)
    {
        StackTop = value;
        if(StackTop > StackMax) StackMax = StackTop;
        StackHash.resize(value);
    }

private:
    std::vector<unsigned> ByteCode;
    std::vector<double>   Immed;

    std::vector<std::pair<bool/*known*/, uint_fast64_t/*hash*/> > StackHash;
    size_t StackTop;
    size_t StackMax;
};

namespace
{
    using namespace FPoptimizer_CodeTree;

    bool AssembleSequence(
                  CodeTree& tree, long count,
                  const SequenceOpCode& sequencing,
                  CodeTree::ByteCodeSynth& synth,
                  size_t max_bytecode_grow_length);

    struct Subdivide_result
    {
        size_t stackpos; // Stack offset where it is found
        int cache_val;   // Which value from cache is it, -1 = none

        Subdivide_result(size_t s,int c=-1) : stackpos(s), cache_val(c) { }
        Subdivide_result() : stackpos(),cache_val() { }
    };

    Subdivide_result AssembleSequence_Subdivide(
                  long count,
                  int cache[POWI_CACHE_SIZE], int cache_needed[POWI_CACHE_SIZE],
                  const SequenceOpCode& sequencing,
                  CodeTree::ByteCodeSynth& synth);

    Subdivide_result Subdivide_MakeResult(
                  const Subdivide_result& a,
                  const Subdivide_result& b,
                  int cache_needed[POWI_CACHE_SIZE],

                  unsigned cumulation_opcode,
                  unsigned cimulation_opcode_flip,

                  CodeTree::ByteCodeSynth& synth);
}

namespace
{
    typedef
        std::map<uint_fast64_t,  std::pair<size_t, CodeTree*> >
        TreeCountType;
    
    void FindTreeCounts(TreeCountType& TreeCounts, CodeTree* tree)
    {
        std::pair<size_t, CodeTree*>& p = TreeCounts[tree->Hash];
        ++p.first;
        p.second = tree;
        
        for(size_t a=0; a<tree->Params.size(); ++a)
            FindTreeCounts(TreeCounts, tree->Params[a].param);
    }
    
    void RememberRecursivelyHashList(std::set<uint_fast64_t>& hashlist,
                                     CodeTree* tree)
    {
        hashlist.insert(tree->Hash);
        for(size_t a=0; a<tree->Params.size(); ++a)
            RememberRecursivelyHashList(hashlist, tree->Params[a].param);
    }
}

namespace FPoptimizer_CodeTree
{
    void CodeTree::SynthesizeByteCode(
        std::vector<unsigned>& ByteCode,
        std::vector<double>&   Immed,
        size_t& stacktop_max)
    {
        ByteCodeSynth synth;

        /* Find common subtrees */
        TreeCountType TreeCounts;
        FindTreeCounts(TreeCounts, this);
        
        /* Synthesize some of the most common ones */
        std::set<uint_fast64_t> AlreadyDoneTrees;
    FindMore: ;
        size_t best_score = 0;
        TreeCountType::const_iterator synth_it;
        for(TreeCountType::const_iterator
            i = TreeCounts.begin();
            i != TreeCounts.end();
            ++i)
        {
            size_t score = i->second.first;
            if(score < 2) continue; // It must always occur at least twice
            if(i->second.second->Depth < 2) continue; // And it must not be a simple expression
            if(AlreadyDoneTrees.find(i->first) 
            != AlreadyDoneTrees.end()) continue; // And it must not yet have been synthesized
            score *= i->second.second->Depth;
            if(score > best_score)
                { best_score = score; synth_it = i; }
        }
        if(best_score > 0)
        {
            /* Synthesize the selected tree */
            synth_it->second.second->SynthesizeByteCode(synth);
            /* Add the tree and all its children to the AlreadyDoneTrees list,
             * to prevent it from being re-synthesized
             */
            RememberRecursivelyHashList(AlreadyDoneTrees, synth_it->second.second);
            goto FindMore;
        }

        /* Then synthesize the actual expression */
        SynthesizeByteCode(synth);
      #ifndef FP_DISABLE_EVAL
        /* Ensure that the expression result is
         * the only thing that remains in the stack
         */
        /* Removed: Fparser does not seem to care! */
        /* But if cEval is supported, it still needs to be done. */
        if(synth.GetStackTop() > 1)
            synth.DoPopNMov(0, synth.GetStackTop()-1);
      #endif
        synth.Pull(ByteCode, Immed, stacktop_max);
    }

    void CodeTree::SynthesizeByteCode(ByteCodeSynth& synth)
    {
        // If the synth can already locate our operand in the stack,
        // never mind synthesizing it again, just dup it.
        if(synth.FindAndDup(Hash))
        {
            return;
        }

        switch(Opcode)
        {
            case cVar:
                synth.PushVar(GetVar());
                synth.StackTopIs(Hash);
                break;
            case cImmed:
                synth.PushImmed(GetImmed());
                synth.StackTopIs(Hash);
                break;
            case cAdd:
            case cMul:
            case cMin:
            case cMax:
            case cAnd:
            case cOr:
            {
                // Operand re-sorting:
                // If the first param has a sign, try to find a param
                // that does _not_ have a sign and put it first.
                // This can be done because params are commutative
                // when they are grouped with their signs.
                if(!Params.empty() && Params[0].sign)
                {
                    for(size_t a=1; a<Params.size(); ++a)
                        if(!Params[a].sign)
                        {
                            std::swap(Params[0], Params[a]);
                            break;
                        }
                }

                // Try to ensure that Immeds don't have a sign
                for(size_t a=0; a<Params.size(); ++a)
                {
                    CodeTree*& param = Params[a].param;
                    if(Params[a].sign && param->IsImmed())
                        switch(Opcode)
                        {
                            case cAdd: param->NegateImmed(); Params[a].sign=false; break;
                            case cMul: if(param->GetImmed() == 0.0) break;
                                       param->InvertImmed(); Params[a].sign=false; break;
                            case cAnd:
                            case cOr:  param->NotTheImmed(); Params[a].sign=false; break;
                        }
                }

                if(Opcode == cMul) // Special treatment for cMul sequences
                {
                    // If the paramlist contains an Immed, and that Immed
                    // fits in a long-integer, try to synthesize it
                    // as add-sequences instead.
                    for(size_t a=0; a<Params.size(); ++a)
                    {
                        Param p = Params[a];
                        CodeTree*& param = p.param;
                        if(!p.sign && param->IsLongIntegerImmed())
                        {
                            long value = param->GetLongIntegerImmed();
                            Params.erase(Params.begin()+a);

                            bool success = AssembleSequence(
                                *this, value, AddSequence,
                                synth,
                                MAX_MULI_BYTECODE_LENGTH);

                            // Readd the token so that we don't need
                            // to deal with allocationd/deallocation here.
                            Params.insert(Params.begin()+a, p);

                            if(success)
                            {
                                // this tree was treated just fine
                                synth.StackTopIs(Hash);
                                return;
                            }
                        }
                    }
                }

                int n_stacked = 0;
                for(size_t a=0; a<Params.size(); ++a)
                {
                    CodeTree*const & param = Params[a].param;
                    bool              sign = Params[a].sign;

                    param->SynthesizeByteCode(synth);
                    ++n_stacked;

                    if(sign) // Is the operand negated/inverted?
                    {
                        if(n_stacked == 1)
                        {
                            // Needs unary negation/invertion. Decide how to accomplish it.
                            switch(Opcode)
                            {
                                case cAdd:
                                    synth.AddOperation(cNeg, 1); // stack state: -1+1 = +0
                                    break;
                                case cMul:
                                    synth.AddOperation(cInv, 1); // stack state: -1+1 = +0
                                    break;
                                case cAnd:
                                case cOr:
                                    synth.AddOperation(cNot, 1); // stack state: -1+1 = +0
                                    break;
                            }
                            // Note: We could use RDiv or RSub when the first
                            // token is negated/inverted and the second is not, to
                            // avoid cNeg/cInv/cNot, but thanks to the operand
                            // re-sorting in the beginning of this code, this
                            // situation never arises.
                            // cNeg/cInv/cNot is only synthesized when the group
                            // consists entirely of negated/inverted items.
                        }
                        else
                        {
                            // Needs binary negation/invertion. Decide how to accomplish it.
                            switch(Opcode)
                            {
                                case cAdd:
                                    synth.AddOperation(cSub, 2); // stack state: -2+1 = -1
                                    break;
                                case cMul:
                                    synth.AddOperation(cDiv, 2); // stack state: -2+1 = -1
                                    break;
                                case cAnd:
                                case cOr:
                                    synth.AddOperation(cNot,   1);   // stack state: -1+1 = +0
                                    synth.AddOperation(Opcode, 2); // stack state: -2+1 = -1
                                    break;
                            }
                            n_stacked = n_stacked - 2 + 1;
                        }
                    }
                    else if(n_stacked > 1)
                    {
                        // Cumulate at the earliest opportunity.
                        synth.AddOperation(Opcode, 2); // stack state: -2+1 = -1
                        n_stacked = n_stacked - 2 + 1;
                    }
                }
                if(n_stacked == 0)
                {
                    // Uh, we got an empty cAdd/cMul/whatever...
                    // Synthesize a default value.
                    // This should never happen.
                    switch(Opcode)
                    {
                        case cAdd:
                        case cOr:
                            synth.PushImmed(0);
                            break;
                        case cMul:
                        case cAnd:
                            synth.PushImmed(1);
                            break;
                        case cMin:
                        case cMax:
                            //synth.PushImmed(NaN);
                            synth.PushImmed(0);
                            break;
                    }
                    ++n_stacked;
                }
                assert(n_stacked == 1);
                break;
            }
            case cPow:
            {
                const Param& p0 = Params[0];
                const Param& p1 = Params[1];

                if(!p1.param->IsLongIntegerImmed()
                || !AssembleSequence( /* Optimize integer exponents */
                        *p0.param, p1.param->GetLongIntegerImmed(),
                        MulSequence,
                        synth,
                        MAX_POWI_BYTECODE_LENGTH)
                  )
                {
                    p0.param->SynthesizeByteCode(synth);
                    p1.param->SynthesizeByteCode(synth);
                    synth.AddOperation(Opcode, 2);
                }
                break;
            }
            case cIf:
            {
                size_t ofs;
                // If the parameter amount is != 3, we're screwed.
                Params[0].param->SynthesizeByteCode(synth); // expression
                synth.SynthIfStep1(ofs);
                Params[1].param->SynthesizeByteCode(synth); // true branch
                synth.SynthIfStep2(ofs);
                Params[2].param->SynthesizeByteCode(synth); // false branch
                synth.SynthIfStep3(ofs);
                break;
            }
            case cFCall:
            {
                // If the parameter count is invalid, we're screwed.
                for(size_t a=0; a<Params.size(); ++a)
                    Params[a].param->SynthesizeByteCode(synth);
                synth.AddOperation(Opcode, Params.size());
                synth.AddOperation(Funcno, 0, 0);
                break;
            }
            case cPCall:
            {
                // If the parameter count is invalid, we're screwed.
                for(size_t a=0; a<Params.size(); ++a)
                    Params[a].param->SynthesizeByteCode(synth);
                synth.AddOperation(Opcode, Params.size());
                synth.AddOperation(Funcno, 0, 0);
                break;
            }
            default:
            {
                // If the parameter count is invalid, we're screwed.
                for(size_t a=0; a<Params.size(); ++a)
                    Params[a].param->SynthesizeByteCode(synth);
                synth.AddOperation(Opcode, Params.size());
                break;
            }
        }
        synth.StackTopIs(Hash);
    }
}

namespace
{
    #define FPO(x) /**/
    //#define FPO(x) x

    void PlanNtimesCache
        (long count,
         int cache[POWI_CACHE_SIZE],
         int cache_needed[POWI_CACHE_SIZE],
         int need_count,
         int recursioncount=0)
    {
        if(count < 1) return;

    #ifdef FP_GENERATING_POWI_TABLE
        if(recursioncount > 32) throw false;
    #endif

        if(count < POWI_CACHE_SIZE)
        {
            //FPO(fprintf(stderr, "%ld will be needed %d times more\n", count, need_count));
            cache_needed[count] += need_count;
            if(cache[count]) return;
        }

        long half = 1;
        if(count < POWI_TABLE_SIZE)
        {
            half = powi_table[count];
        }
        else if(count & 1)
        {
            half = count & ((1 << POWI_WINDOW_SIZE) - 1); // that is, count & 7
        }
        else
        {
            half = count / 2;
        }

        long otherhalf = count-half;
        if(half > otherhalf || half<0) std::swap(half,otherhalf);

        FPO(fprintf(stderr, "count=%ld, half=%ld, otherhalf=%ld\n", count,half,otherhalf));

        if(half == otherhalf)
        {
            PlanNtimesCache(half,      cache, cache_needed, 2, recursioncount+1);
        }
        else
        {
            PlanNtimesCache(half,      cache, cache_needed, 1, recursioncount+1);
            PlanNtimesCache(otherhalf>0?otherhalf:-otherhalf,
                                       cache, cache_needed, 1, recursioncount+1);
        }

        if(count < POWI_CACHE_SIZE)
            cache[count] = 1; // This value has been generated
    }

    bool AssembleSequence(
        CodeTree& tree, long count,
        const SequenceOpCode& sequencing,
        CodeTree::ByteCodeSynth& synth,
        size_t max_bytecode_grow_length)
    {
        CodeTree::ByteCodeSynth backup = synth;
        const size_t bytecodesize_backup = synth.GetByteCodeSize();

        if(count == 0)
        {
            synth.PushImmed(sequencing.basevalue);
        }
        else
        {
            tree.SynthesizeByteCode(synth);

            if(count < 0)
            {
                synth.AddOperation(sequencing.op_flip, 1);
                count = -count;
            }

            if(count > 1)
            {
                /* To prevent calculating the same factors over and over again,
                 * we use a cache. */
                int cache[POWI_CACHE_SIZE], cache_needed[POWI_CACHE_SIZE];

                /* Assume we have no factors in the cache */
                for(int n=0; n<POWI_CACHE_SIZE; ++n) { cache[n] = 0; cache_needed[n] = 0; }


                /* Decide which factors we would need multiple times.
                 * Output:
                 *   cache[]        = these factors were generated
                 *   cache_needed[] = number of times these factors were desired
                 */
                cache[1] = 1; // We have this value already.
                PlanNtimesCache(count, cache, cache_needed, 1);

                cache[1] = synth.GetStackTop()-1;
                for(int n=2; n<POWI_CACHE_SIZE; ++n)
                    cache[n] = -1; /* Stack location for each component */

                size_t stacktop_desired = synth.GetStackTop();

                /*// Cache all the required components
                for(int n=2; n<POWI_CACHE_SIZE; ++n)
                    if(cache_needed[n] > 1)
                    {
                        FPO(fprintf(stderr, "Will need %d, %d times, caching...\n", n, cache_needed[n]));
                        Subdivide_result res = AssembleSequence_Subdivide(
                            n, cache, cache_needed, sequencing,
                            synth);
                        FPO(fprintf(stderr, "Cache[%d] = %u,%d\n",
                            n, (unsigned)res.stackpos, res.cache_val));
                        cache[n] = res.stackpos;
                    }*/

                for(int a=1; a<POWI_CACHE_SIZE; ++a)
                    if(cache[a] >= 0 || cache_needed[a] > 0)
                    {
                        FPO(fprintf(stderr, "== cache: sp=%d, val=%d, needs=%d\n",
                            cache[a], a, cache_needed[a]));
                    }

                FPO(fprintf(stderr, "Calculating result for %ld...\n", count));
                Subdivide_result res = AssembleSequence_Subdivide(
                    count, cache, cache_needed, sequencing,
                    synth);

                size_t n_excess = synth.GetStackTop() - stacktop_desired;
                if(n_excess > 0 || res.stackpos != stacktop_desired-1)
                {
                    // Remove the cache values
                    synth.DoPopNMov(stacktop_desired-1, res.stackpos);
                }
            }
        }

        size_t bytecode_grow_amount = synth.GetByteCodeSize() - bytecodesize_backup;
        if(bytecode_grow_amount > max_bytecode_grow_length)
        {
            synth = backup;
            return false;
        }
        return true;
    }

    Subdivide_result
    AssembleSequence_Subdivide(
        long count,
        int cache[POWI_CACHE_SIZE], int cache_needed[POWI_CACHE_SIZE],
        const SequenceOpCode& sequencing,
        CodeTree::ByteCodeSynth& synth)
    {
        if(count < POWI_CACHE_SIZE)
        {
            if(cache[count] >= 0)
            {
                // found from the cache
                FPO(fprintf(stderr, "* I found %ld from cache (%u,%d)\n",
                    count, (unsigned)cache[count], cache_needed[count]));
                return Subdivide_result(cache[count], count);
            }
        }

        long half = 1;

        if(count < POWI_TABLE_SIZE)
        {
            half = powi_table[count];
        }
        else if(count & 1)
        {
            half = count & ((1 << POWI_WINDOW_SIZE) - 1); // that is, count & 7
        }
        else
        {
            half = count / 2;
        }
        long otherhalf = count-half;
        if(half > otherhalf || half<0) std::swap(half,otherhalf);

        FPO(fprintf(stderr, "* I want %ld, my plan is %ld + %ld\n", count, half, count-half));

        Subdivide_result res;
        if(half == otherhalf)
        {
            Subdivide_result half_res = AssembleSequence_Subdivide(
                half,
                cache, cache_needed, sequencing,
                synth);

            // self-cumulate the subdivide result
            res = Subdivide_MakeResult(half_res, half_res, cache_needed,
                sequencing.op_normal, sequencing.op_normal_flip,
                synth);
        }
        else
        {
            Subdivide_result half_res = AssembleSequence_Subdivide(
                half,
                cache, cache_needed, sequencing,
                synth);

            Subdivide_result otherhalf_res = AssembleSequence_Subdivide(
                otherhalf>0?otherhalf:-otherhalf,
                cache, cache_needed, sequencing,
                synth);

            FPO(fprintf(stderr, "Subdivide(%ld: %ld, %ld)\n", count, half, otherhalf));

            res = Subdivide_MakeResult(half_res,otherhalf_res, cache_needed,
                otherhalf>0 ? sequencing.op_normal      : sequencing.op_inverse,
                otherhalf>0 ? sequencing.op_normal_flip : sequencing.op_inverse_flip,
                synth);
        }

        if(res.cache_val < 0 && count < POWI_CACHE_SIZE)
        {
            FPO(fprintf(stderr, "* Remembering that %ld can be found at %u (%d uses remain)\n",
                count, (unsigned)res.stackpos, cache_needed[count]));
            cache[count] = res.stackpos;
            res.cache_val = count;
        }
        for(int a=1; a<POWI_CACHE_SIZE; ++a)
            if(cache[a] >= 0 || cache_needed[a] > 0)
            {
                FPO(fprintf(stderr, "== cache: sp=%d, val=%d, needs=%d\n",
                    cache[a], a, cache_needed[a]));
            }
        return res;
    }

    Subdivide_result
    Subdivide_MakeResult(
        const Subdivide_result& a,
        const Subdivide_result& b,
        int cache_needed[POWI_CACHE_SIZE],
        unsigned cumulation_opcode,
        unsigned cumulation_opcode_flip,
        CodeTree::ByteCodeSynth& synth)
    {
        FPO(fprintf(stderr, "== making result for (sp=%u, val=%d, needs=%d) and (sp=%u, val=%d, needs=%d), stacktop=%u\n",
            (unsigned)a.stackpos, a.cache_val, a.cache_val>=0 ? cache_needed[a.cache_val] : -1,
            (unsigned)b.stackpos, b.cache_val, b.cache_val>=0 ? cache_needed[b.cache_val] : -1,
            (unsigned)synth.GetStackTop()));

        // Figure out whether we can trample a and b
        int a_needed = 0;
        int b_needed = 0;

        if(a.cache_val >= 0) a_needed = --cache_needed[a.cache_val];
        if(b.cache_val >= 0) b_needed = --cache_needed[b.cache_val];

        size_t apos = a.stackpos, bpos = b.stackpos;

        bool flipped = false;

        #define DUP_BOTH() do { \
            if(apos < bpos) { size_t tmp=apos; apos=bpos; bpos=tmp; flipped=!flipped; } \
            FPO(fprintf(stderr, "-> dup(%u) dup(%u) op\n", (unsigned)apos, (unsigned)bpos)); \
            synth.DoDup(apos); \
            synth.DoDup(apos==bpos ? synth.GetStackTop()-1 : bpos); } while(0)
        #define DUP_ONE(p) do { \
            FPO(fprintf(stderr, "-> dup(%u) op\n", (unsigned)p)); \
            synth.DoDup(p); \
        } while(0)

        if(a_needed > 0 && b_needed > 0)
        {
            // If they must both be preserved, make duplicates
            // First push the one that is at the larger stack
            // address. This increases the odds of possibly using cDup.
            DUP_BOTH();

            //SCENARIO 1:
            // Input:  x B A x x
            // Temp:   x B A x x A B
            // Output: x B A x x R
            //SCENARIO 2:
            // Input:  x A B x x
            // Temp:   x A B x x B A
            // Output: x A B x x R
        }
        // So, either one could be trampled over
        else if(a_needed > 0)
        {
            // A must be preserved, but B can be trampled over

            // SCENARIO 1:
            //  Input:  x B x x A
            //   Temp:  x B x x A A B   (dup both, later first)
            //  Output: x B x x A R
            // SCENARIO 2:
            //  Input:  x A x x B
            //   Temp:  x A x x B A
            //  Output: x A x x R       -- only commutative cases
            // SCENARIO 3:
            //  Input:  x x x B A
            //   Temp:  x x x B A A B   (dup both, later first)
            //  Output: x x x B A R
            // SCENARIO 4:
            //  Input:  x x x A B
            //   Temp:  x x x A B A     -- only commutative cases
            //  Output: x x x A R
            // SCENARIO 5:
            //  Input:  x A B x x
            //   Temp:  x A B x x A B   (dup both, later first)
            //  Output: x A B x x R

            // if B is not at the top, dup both.
            if(bpos != synth.GetStackTop()-1)
                DUP_BOTH();    // dup both
            else
            {
                DUP_ONE(apos); // just dup A
                flipped=!flipped;
            }
        }
        else if(b_needed > 0)
        {
            // B must be preserved, but A can be trampled over
            // This is a mirror image of the a_needed>0 case, so I'll cut the chase
            if(apos != synth.GetStackTop()-1)
                DUP_BOTH();
            else
                DUP_ONE(bpos);
        }
        else
        {
            // Both can be trampled over.
            // SCENARIO 1:
            //  Input:  x B x x A
            //   Temp:  x B x x A B
            //  Output: x B x x R
            // SCENARIO 2:
            //  Input:  x A x x B
            //   Temp:  x A x x B A
            //  Output: x A x x R       -- only commutative cases
            // SCENARIO 3:
            //  Input:  x x x B A
            //  Output: x x x R         -- only commutative cases
            // SCENARIO 4:
            //  Input:  x x x A B
            //  Output: x x x R
            // SCENARIO 5:
            //  Input:  x A B x x
            //   Temp:  x A B x x A B   (dup both, later first)
            //  Output: x A B x x R
            // SCENARIO 6:
            //  Input:  x x x C
            //   Temp:  x x x C C   (c is both A and B)
            //  Output: x x x R

            if(apos == bpos && apos == synth.GetStackTop()-1)
                DUP_ONE(apos); // scenario 6
            else if(apos == synth.GetStackTop()-1 && bpos == synth.GetStackTop()-2)
            {
                FPO(fprintf(stderr, "-> op\n")); // scenario 3
                flipped=!flipped;
            }
            else if(apos == synth.GetStackTop()-2 && bpos == synth.GetStackTop()-1)
                FPO(fprintf(stderr, "-> op\n")); // scenario 4
            else if(apos == synth.GetStackTop()-1)
                DUP_ONE(bpos); // scenario 1
            else if(bpos == synth.GetStackTop()-1)
            {
                DUP_ONE(apos); // scenario 2
                flipped=!flipped;
            }
            else
                DUP_BOTH(); // scenario 5
        }
        // Add them together.
        synth.AddOperation(flipped ? cumulation_opcode_flip : cumulation_opcode, 2);
        // The return value will not need to be preserved.
        int cache_val = -1;
        FPO(fprintf(stderr, "== producing %u:%d\n", (unsigned)(synth.GetStackTop()-1), cache_val));
        return Subdivide_result(synth.GetStackTop()-1, cache_val);
    }
}
