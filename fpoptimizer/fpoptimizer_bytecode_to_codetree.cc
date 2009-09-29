#include <cmath>
#include <cassert>

#include "fpoptimizer_codetree.hh"
#include "fpoptimizer_optimize.hh"
#include "fpoptimizer_opcodename.hh"
#include "fpoptimizer_grammar.hh"
#include "fptypes.hh"

#include "fpoptimizer_consts.hh"
#include "fparser.hh"


#ifdef FP_SUPPORT_OPTIMIZER

using namespace FUNCTIONPARSERTYPES;
//using namespace FPoptimizer_Grammar;

namespace
{
    long ParsePowiSequence(const std::vector<unsigned>& ByteCode, size_t& IP)
    {
        long result = 1.0;
        while(IP < ByteCode.size() && ByteCode[IP] == cSqr)
        {
            result *= 2;
            ++IP;
        }
        if(IP < ByteCode.size() && ByteCode[IP] == cDup)
        {
            size_t dup_pos = IP;
            ++IP;
            long subexponent = ParsePowiSequence(ByteCode, IP);
            if(IP >= ByteCode.size() || ByteCode[IP] != cMul)
            {
                // It wasn't a powi-dup after all
                IP = dup_pos;
            }
            else
            {
                ++IP; // skip cMul
                result *= 1 + subexponent;
            }
        }
        return result;
    }
    long ParseMuliSequence(const std::vector<unsigned>& ByteCode, size_t& IP)
    {
        long result = 1.0;
        if(IP < ByteCode.size() && ByteCode[IP] == cDup)
        {
            size_t dup_pos = IP;
            ++IP;
            long subfactor = ParseMuliSequence(ByteCode, IP);
            if(IP >= ByteCode.size() || ByteCode[IP] != cAdd)
            {
                // It wasn't a muli-dup after all
                IP = dup_pos;
            }
            else
            {
                ++IP; // skip cAdd
                result *= 1 + subfactor;
            }
        }
        return result;
    }
}

namespace FPoptimizer_CodeTree
{
    class CodeTreeParserData
    {
    public:
        CodeTreeParserData() : stack() { }

        void Eat(size_t nparams, OPCODE opcode)
        {
            CodeTree newnode;
            newnode.SetOpcode(opcode);
            size_t stackhead = stack.size() - nparams;
            for(size_t a=0; a<nparams; ++a)
                newnode.AddParamMove( stack[stackhead + a] );

            switch(opcode)
            {
                //        asinh: log(x + sqrt(x*x + 1))
                //cAsinh [x] -> cLog (cAdd x (cPow (cAdd (cPow x 2) 1) 0.5))
                // Note: ^ Replacement function refers to x twice

                //        acosh: log(x + sqrt(x*x - 1))
                //cAcosh [x] -> cLog (cAdd x (cPow (cAdd (cPow x 2) -1) 0.5))

                //        atanh: log( (1+x) / (1-x)) / 2
                //cAtanh [x] -> cMul (cLog (cMul (cAdd 1 x) (cPow (cAdd 1 (cMul -1 x)) -1))) 0.5

                //     The hyperbolic functions themselves are:
                //        sinh: (exp(x)-exp(-x)) / 2  = exp(-x) * (exp(2*x)-1) / 2
                //cSinh [x] -> cMul 0.5 (cPow [CONSTANT_EI x]) (cAdd [-1 (cPow [CONSTANT_2E x])])

                //        cosh: (exp(x)+exp(-x)) / 2  = exp(-x) * (exp(2*x)+1) / 2
                //        cosh(-x) = cosh(x)
                //cCosh [x] -> cMul 0.5 (cPow [CONSTANT_EI x]) (cAdd [ 1 (cPow [CONSTANT_2E x])])

                //        tanh: sinh/cosh = (exp(2*x)-1) / (exp(2*x)+1)
                //cTanh [x] -> (cMul (cAdd {(cPow [CONSTANT_2E x]) -1}) (cPow [(cAdd {(cPow [CONSTANT_2E x]) 1}) -1]))
                case cTanh:
                {
                    CodeTree sinh, cosh;
                    sinh.SetOpcode(cSinh); sinh.AddParam(newnode.GetParam(0)); sinh.Rehash();
                    cosh.SetOpcode(cCosh); cosh.AddParamMove(newnode.GetParam(0)); cosh.Rehash();
                    CodeTree pow;
                    pow.SetOpcode(cPow);
                    pow.AddParamMove(cosh);
                    pow.AddParam(CodeTree(-1.0));
                    pow.Rehash();
                    newnode.SetOpcode(cMul);
                    newnode.SetParamMove(0, sinh);
                    newnode.AddParamMove(pow);
                    break;
                }

                //        tan: sin/cos
                //cTan [x] -> (cMul (cSin [x]) (cPow [(cCos [x]) -1]))
                case cTan:
                {
                    CodeTree sin, cos;
                    sin.SetOpcode(cSin); sin.AddParam(newnode.GetParam(0)); sin.Rehash();
                    cos.SetOpcode(cCos); cos.AddParamMove(newnode.GetParam(0)); cos.Rehash();
                    CodeTree pow;
                    pow.SetOpcode(cPow);
                    pow.AddParamMove(cos);
                    pow.AddParam(CodeTree(-1.0));
                    pow.Rehash();
                    newnode.SetOpcode(cMul);
                    newnode.SetParamMove(0, sin);
                    newnode.AddParamMove(pow);
                    break;
                }

                // Should we change sin(x) into cos(pi/2-x)
                //               or cos(x) into sin(pi/2-x)?
                //                        note: cos(x-pi/2) = cos(pi/2-x) = sin(x)
                //                        note: sin(x-pi/2) = -sin(pi/2-x) = -cos(x)
                default: break;
            }

            newnode.Rehash();
        /*
            using namespace FPoptimizer_Grammar;
            bool recurse = false;
            while(ApplyGrammar(pack.glist[0], newnode, recurse)) // intermediate
            { //std::cout << "Rerunning 1\n";
                FixIncompleteHashes(newnode);
                recurse = true;
            }
        */
            FindClone(newnode, false);
        #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "POP " << nparams << ", " << FP_GetOpcodeName(opcode)
                      << "->" << FP_GetOpcodeName(newnode.GetOpcode())
                      << ": PUSH ";
            FPoptimizer_Grammar::DumpTree(newnode);
            std::cout <<std::endl;
        #endif
            stack.resize(stackhead+1);
            stack.back().swap(newnode);
        }

        void EatFunc(size_t nparams, OPCODE opcode, unsigned funcno)
        {
            CodeTree newnode;
            newnode.SetFuncOpcode(opcode, funcno);
            size_t stackhead = stack.size() - nparams;
            for(size_t a=0; a<nparams; ++a)
                newnode.AddParamMove( stack[stackhead + a] );
            newnode.Rehash(false);
        #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "POP " << nparams << ", PUSH ";
            FPoptimizer_Grammar::DumpTree(newnode);
            std::cout << std::endl;
        #endif
            FindClone(newnode);
            stack.resize(stackhead+1);
            stack.back().swap(newnode);
        }

        void AddConst(double value)
        {
            CodeTree newnode(value);
            FindClone(newnode);
            stack.resize(stack.size()+1);
            stack.back().swap(newnode);
        #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "PUSH ";
            FPoptimizer_Grammar::DumpTree(stack.back());
            std::cout << std::endl;
        #endif
        }

        void AddVar(unsigned varno)
        {
            CodeTree newnode(varno, CodeTree::VarTag());
            FindClone(newnode);
            stack.resize(stack.size()+1);
            stack.back().swap(newnode);
        #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "PUSH ";
            FPoptimizer_Grammar::DumpTree(stack.back());
            std::cout << std::endl;
        #endif
        }

        void SwapLastTwoInStack()
        {
            stack[stack.size()-1].swap( stack[stack.size()-2] );
        }

        void Dup()
        {
            Fetch(stack.size()-1);
        }

        void Fetch(size_t which)
        {
            stack.push_back(stack[which]);
        #ifdef DEBUG_SUBSTITUTIONS
            std::cout << "PUSH ";
            FPoptimizer_Grammar::DumpTree(stack.back());
            std::cout << std::endl;
        #endif
        }

        void PopNMov(size_t target, size_t source)
        {
            stack[target] = stack[source];
            stack.resize(target+1);
        }

        CodeTree PullResult()
        {
            clones.clear();
            CodeTree result = stack.back();
            stack.resize(stack.size()-1);
            return result;
        }
    private:
        void FindClone(CodeTree& tree, bool recurse = true)
        {
            std::multimap<fphash_t, CodeTree>::const_iterator
                i = clones.lower_bound(tree.GetHash());
            for(; i != clones.end() && i->first == tree.GetHash(); ++i)
            {
                if(i->second.IsIdenticalTo(tree))
                    tree.Become(i->second);
            }
            if(recurse)
                for(size_t a=0; a<tree.GetParamCount(); ++a)
                    FindClone(tree.GetParam(a));
            clones.insert(std::make_pair(tree.GetHash(), tree));
        }
    private:
        std::vector<CodeTree> stack;
        std::multimap<fphash_t, CodeTree> clones;

    private:
        CodeTreeParserData(const CodeTreeParserData&);
        CodeTreeParserData& operator=(const CodeTreeParserData&);
    };

    void CodeTree::GenerateFrom(
        const std::vector<unsigned>& ByteCode,
        const std::vector<double>& Immed,
        const FunctionParser::Data& fpdata)
    {
        CodeTreeParserData sim;
        std::vector<size_t> labels;

        for(size_t IP=0, DP=0; ; ++IP)
        {
            while(!labels.empty() && labels.back() == IP)
            {
                // The "else" of an "if" ends here
                sim.Eat(3, cIf);
                labels.erase(labels.end()-1);
            }
        after_powi:
            if(IP >= ByteCode.size()) break;

            unsigned opcode = ByteCode[IP];
            if(opcode == cSqr || opcode == cDup)
            {
                // Parse a powi sequence
                //size_t was_ip = IP;
                long exponent = ParsePowiSequence(ByteCode, IP);
                if(exponent != 1)
                {
                    //std::cout << "Found exponent at " << was_ip << ": " << exponent << "\n";
                    sim.AddConst( (double) exponent);
                    sim.Eat(2, cPow);
                    goto after_powi;
                }
                if(opcode == cDup)
                {
                    long factor = ParseMuliSequence(ByteCode, IP);
                    if(factor != 1)
                    {
                        //std::cout << "Found factor at " << was_ip << ": " << factor << "\n";
                        sim.AddConst( (double) factor);
                        sim.Eat(2, cMul);
                        goto after_powi;
                    }
                }
            }
            if(OPCODE(opcode) >= VarBegin)
            {
                sim.AddVar(opcode);
            }
            else
            {
                switch( OPCODE(opcode) )
                {
                    // Specials
                    case cIf:
                        IP += 2;
                        continue;
                    case cJump:
                        labels.push_back(ByteCode[IP+1]+1);
                        IP += 2;
                        continue;
                    case cImmed:
                        sim.AddConst(Immed[DP++]);
                        break;
                    case cDup:
                        sim.Dup();
                        break;
                    case cNop:
                        break;
                    case cFCall:
                    {
                        unsigned funcno = ByteCode[++IP];
                        unsigned params = fpdata.FuncPtrs[funcno].params;
                        sim.EatFunc(params, OPCODE(opcode), funcno);
                        break;
                    }
                    case cPCall:
                    {
                        unsigned funcno = ByteCode[++IP];
                        unsigned params = fpdata.FuncParsers[funcno].params;
                        sim.EatFunc(params, OPCODE(opcode), funcno);
                        break;
                    }
                    // Unary operators requiring special attention
                    case cInv: // from fpoptimizer
                        sim.AddConst(-1);
                        sim.Eat(2, cPow); // 1/x is x^-1
                        break;
                    case cNeg:
                        sim.AddConst(-1);
                        sim.Eat(2, cMul); // -x is x*-1
                        break;
                    case cSqr: // from fpoptimizer
                        sim.AddConst(2.0);
                        sim.Eat(2, cPow);
                        break;
                    // Unary functions requiring special attention
                    case cDeg:
                        sim.AddConst(CONSTANT_DR);
                        sim.Eat(2, cMul);
                        break;
                    case cRad:
                        sim.AddConst(CONSTANT_RD);
                        sim.Eat(2, cMul);
                        break;
                    case cExp:
                        sim.AddConst(CONSTANT_E);
                        sim.SwapLastTwoInStack();
                        sim.Eat(2, cPow);
                        break;
                    case cExp2: // from fpoptimizer
                        sim.AddConst(2.0);
                        sim.SwapLastTwoInStack();
                        sim.Eat(2, cPow);
                        break;
                    case cSqrt:
                        sim.AddConst(0.5);
                        sim.Eat(2, cPow);
                        break;
                    case cCot:
                        sim.Eat(1, cTan);
                        sim.AddConst(-1);
                        sim.Eat(2, cPow);
                        break;
                    case cCsc:
                        sim.Eat(1, cSin);
                        sim.AddConst(-1);
                        sim.Eat(2, cPow);
                        break;
                    case cSec:
                        sim.Eat(1, cCos);
                        sim.AddConst(-1);
                        sim.Eat(2, cPow);
                        break;
                    case cLog10:
                        sim.Eat(1, cLog);
                        sim.AddConst(CONSTANT_L10I);
                        sim.Eat(2, cMul);
                        break;
                    case cLog2:
                        sim.Eat(1, cLog);
                        sim.AddConst(CONSTANT_L2I);
                        sim.Eat(2, cMul);
                        break;
                    //case cLog:
                    //    sim.Eat(1, cLog2);
                    //    sim.AddConst(CONSTANT_L2);
                    //    sim.Eat(2, cMul);
                    //    break;
                    // Binary operators requiring special attention
                    case cSub:
                        sim.AddConst(-1);
                        sim.Eat(2, cMul); // -x is x*-1
                        sim.Eat(2, cAdd); // Minus is negative adding
                        break;
                    case cRSub: // from fpoptimizer
                        sim.SwapLastTwoInStack();
                        sim.AddConst(-1);
                        sim.Eat(2, cMul); // -x is x*-1
                        sim.Eat(2, cAdd);
                        break;
                    case cDiv:
                        sim.AddConst(-1);
                        sim.Eat(2, cPow); // 1/x is x^-1
                        sim.Eat(2, cMul); // Divide is inverse multiply
                        break;
                    case cRDiv: // from fpoptimizer
                        sim.SwapLastTwoInStack();
                        sim.AddConst(-1);
                        sim.Eat(2, cPow); // 1/x is x^-1
                        sim.Eat(2, cMul); // Divide is inverse multiply
                        break;
                    case cRPow:
                        sim.SwapLastTwoInStack();
                        sim.Eat(2, cPow);
                        break;
                    case cRSqrt: // from fpoptimizer
                        sim.AddConst(-0.5);
                        sim.Eat(2, cPow);
                        break;
                    // Binary operators not requiring special attention
                    case cAdd: case cMul:
                    case cMod: case cPow:
                    case cEqual: case cLess: case cGreater:
                    case cNEqual: case cLessOrEq: case cGreaterOrEq:
                    case cAnd: case cOr:
                        sim.Eat(2, OPCODE(opcode));
                        break;
                    // Unary operators not requiring special attention
                    case cNot:
                    case cNotNot: // from fpoptimizer
                        sim.Eat(1, OPCODE(opcode));
                        break;
                    // Special opcodes generated by fpoptimizer itself
                    case cFetch:
                        sim.Fetch(ByteCode[++IP]);
                        break;
                    case cPopNMov:
                    {
                        unsigned stackOffs_target = ByteCode[++IP];
                        unsigned stackOffs_source = ByteCode[++IP];
                        sim.PopNMov(stackOffs_target, stackOffs_source);
                        break;
                    }
                    // Note: cVar should never be encountered in bytecode.
                    // Other functions
#ifndef FP_DISABLE_EVAL
                    case cEval:
                    {
                        size_t paramcount = fpdata.variableRefs.size();
                        sim.Eat(paramcount, OPCODE(opcode));
                        break;
                    }
#endif
                    default:
                        unsigned funcno = opcode-cAbs;
                        assert(funcno < FUNC_AMOUNT);
                        const FuncDefinition& func = Functions[funcno];
                        sim.Eat(func.params, OPCODE(opcode));
                        break;
                }
            }
        }
        Become(sim.PullResult());
    #ifdef DEBUG_SUBSTITUTIONS
        std::cout << "Produced tree:\n";
        FPoptimizer_Grammar::DumpTreeWithIndent(*this);
    #endif
    }
}

#endif
