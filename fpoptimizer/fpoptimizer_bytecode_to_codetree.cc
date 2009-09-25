#include <cmath>
#include <cassert>

#include "fpoptimizer_codetree.hh"
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
    private:
        std::vector<CodeTree> stack;
    public:
        CodeTreeParserData() : stack() { }

        void Eat(size_t nparams, OPCODE opcode)
        {
            CodeTree newnode;
            newnode.BeginChanging();
            newnode.SetOpcode(opcode);
            size_t stackhead = stack.size() - nparams;
            for(size_t a=0; a<nparams; ++a)
                newnode.AddParam( stack[stackhead + a] );
            newnode.ConstantFolding();
            newnode.FinishChanging();
            stack.resize(stackhead);
            stack.push_back(newnode);
        }

        void EatFunc(size_t nparams, OPCODE opcode, unsigned funcno)
        {
            CodeTree newnode;
            newnode.BeginChanging();
            newnode.SetFuncOpcode(opcode, funcno);
            size_t stackhead = stack.size() - nparams;
            for(size_t a=0; a<nparams; ++a)
                newnode.AddParam( stack[stackhead + a] );
            newnode.ConstantFolding();
            newnode.FinishChanging();
            stack.resize(stackhead);
            stack.push_back(newnode);
        }

        void AddConst(double value)
        {
            stack.push_back( CodeTree(value) );
        }

        void AddVar(unsigned varno)
        {
            CodeTree newnode;
            newnode.BeginChanging();
            newnode.SetVar(varno);
            newnode.FinishChanging();
            stack.push_back(newnode);
        }

        void SwapLastTwoInStack()
        {
            std::swap(stack[stack.size()-1],
                      stack[stack.size()-2]);
        }

        void Dup()
        {
            Fetch(stack.size()-1);
        }

        void Fetch(size_t which)
        {
            stack.push_back(stack[which]);
        }

        void PopNMov(size_t target, size_t source)
        {
            stack[target] = stack[source];
            stack.resize(target+1);
        }

        CodeTree PullResult()
        {
            CodeTree result = stack.back();
            stack.resize(stack.size()-1);
            return result;
        }
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
    }
}

#endif
