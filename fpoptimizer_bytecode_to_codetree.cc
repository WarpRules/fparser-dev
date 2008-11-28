#include <cmath>
#include <list>
#include <cassert>

#include "fpoptimizer_codetree.hh"
#include "fptypes.hh"

#include "fpoptimizer_consts.hh"
#include "fparser.hh"


using namespace FUNCTIONPARSERTYPES;
//using namespace FPoptimizer_Grammar;


namespace FPoptimizer_CodeTree
{
    class CodeTreeParserData
    {
    private:
        std::vector<CodeTree*> stack;
    public:
        CodeTreeParserData() : stack() { }
        ~CodeTreeParserData()
        {
            for(size_t a=0; a<stack.size(); ++a)
                delete stack[a];
        }
        
        void Eat(unsigned nparams, OPCODE opcode)
        {
            CodeTree* newnode = new CodeTree;
            newnode->Opcode = opcode;
            size_t stackhead = stack.size() - nparams;
            for(unsigned a=0; a<nparams; ++a)
            {
                CodeTree::Param param;
                param.param = stack[stackhead + a];
                param.sign  = false;
                param.param->Parent = newnode;
                newnode->Params.push_back(param);
            }
            stack.resize(stackhead);
            stack.push_back(newnode);
        }
        
        void EatFunc(unsigned params, OPCODE opcode, unsigned funcno)
        {
            Eat(params, opcode);
            stack.back()->Funcno = funcno;
        }
        
        void AddConst(double value)
        {
            CodeTree* newnode = new CodeTree;
            newnode->Opcode = cImmed;
            newnode->Value  = value;
            stack.push_back(newnode);
        }
        
        void AddVar(unsigned varno)
        {
            CodeTree* newnode = new CodeTree;
            newnode->Opcode = cVar;
            newnode->Var    = varno;
            stack.push_back(newnode);
        }
        
        void SetLastOpParamSign(unsigned paramno)
        {
            stack.back()->Params[paramno].sign = true;
        }
        
        void SwapLastTwoInStack()
        {
            std::swap(stack[stack.size()-1],
                      stack[stack.size()-2]);
        }
        
        void Dup()
        {
            stack.push_back(stack.back()->Clone());
        }
        
        CodeTree* PullResult()
        {
            CodeTree* result = stack.back();
            stack.resize(stack.size()-1);
            result->Rehash(false);
            return result;
        }
        
        void CheckConst()
        {
            // Check if the last token on stack can be optimized with constant math
            CodeTree* result = stack.back();
            result->ConstantFolding();
        }
    private:
        CodeTreeParserData(const CodeTreeParserData&);
        CodeTreeParserData& operator=(const CodeTreeParserData&);
    };

    CodeTree* CodeTree::GenerateFrom(
        const std::vector<unsigned>& ByteCode,
        const std::vector<double>& Immed,
        const FunctionParser::Data& fpdata)
    {
        CodeTreeParserData data;
        std::list<size_t> labels;
        
        for(size_t IP=0, DP=0; ; ++IP)
        {
            while(!labels.empty() && *labels.begin() == IP)
            {
                // The "else" of an "if" ends here
                data.Eat(3, cIf);
                labels.erase(labels.begin());
            }
            if(IP >= ByteCode.size()) break;
            
            unsigned opcode = ByteCode[IP];
            if(OPCODE(opcode) >= VarBegin)
            {
                data.AddVar(opcode);
            }
            else
            {
                switch(opcode)
                {
                    // Specials
                    case cIf:
                        IP += 2;
                        continue;
                    case cJump:
                        labels.push_front(ByteCode[IP+1]+1);
                        IP += 2;
                        continue;
                    case cImmed:
                        data.AddConst(Immed[DP++]);
                        break;
                    case cDup:
                        data.Dup();
                        break;
                    case cNop:
                        break;
                    case cFCall:
                    {
                        unsigned funcno = ByteCode[++IP];
                        unsigned params = fpdata.FuncPtrs[funcno].params;
                        data.EatFunc(params, OPCODE(opcode), funcno);
                        break;
                    }
                    case cPCall:
                    {
                        unsigned funcno = ByteCode[++IP];
                        unsigned params = fpdata.FuncParsers[funcno].params;
                        data.EatFunc(params, OPCODE(opcode), funcno);
                        break;
                    }
                    // Unary operators requiring special attention
                    case cInv:
                        data.Eat(1, cMul); // Unary division is inverse multiplying
                        data.SetLastOpParamSign(0);
                        break;
                    case cNeg:
                        data.Eat(1, cAdd); // Unary minus is negative adding.
                        data.SetLastOpParamSign(0);
                        break;
                    case cSqr:
                        data.Dup();
                        data.Eat(2, cMul);
                        break;
                    // Unary functions requiring special attention
                    case cDeg:
                        data.AddConst(CONSTANT_DR);
                        data.Eat(2, cMul);
                        break;
                    case cRad:
                        data.AddConst(CONSTANT_RD);
                        data.Eat(2, cMul);
                        break;
                    case cExp:
                        data.AddConst(CONSTANT_E);
                        data.SwapLastTwoInStack();
                        data.Eat(2, cPow);
                        break;
                    case cSqrt:
                        data.AddConst(0.5);
                        data.Eat(2, cPow);
                        break;
                    case cCot:
                        data.Eat(1, cTan);
                        data.Eat(1, cMul);
                        data.SetLastOpParamSign(0);
                        break;
                    case cCsc:
                        data.Eat(1, cSin);
                        data.Eat(1, cMul);
                        data.SetLastOpParamSign(0);
                        break;
                    case cSec:
                        data.Eat(1, cCos);
                        data.Eat(1, cMul);
                        data.SetLastOpParamSign(0);
                        break;
                    case cLog10:
                        data.Eat(1, cLog);
                        data.AddConst(CONSTANT_L10I);
                        data.Eat(2, cMul);
                        break;
                    // Binary operators requiring special attention
                    case cSub:
                        data.Eat(2, cAdd); // Minus is negative adding
                        data.SetLastOpParamSign(1);
                        break;
                    case cRSub:
                        data.Eat(2, cAdd);
                        data.SetLastOpParamSign(0);
                        break;
                    case cDiv:
                        data.Eat(2, cMul); // Divide is inverse multiply
                        data.SetLastOpParamSign(1);
                        break;
                    case cRDiv:
                        data.Eat(2, cMul);
                        data.SetLastOpParamSign(0);
                        break;
                    // Binary operators not requiring special attention
                    case cAdd: case cMul:
                    case cMod: case cPow:
                    case cEqual: case cLess: case cGreater:
                    case cNEqual: case cLessOrEq: case cGreaterOrEq:
                    case cAnd: case cOr:
                        data.Eat(2, OPCODE(opcode));
                        break;
                    // Unary operators not requiring special attention
                    case cNot:
                        data.Eat(1, OPCODE(opcode));
                        break;
                    // Other functions
#ifndef FP_DISABLE_EVAL
                    case cEval:
                    {
                        unsigned paramcount = fpdata.variableRefs.size();
                        data.Eat(paramcount, OPCODE(opcode));
                        break;
                    }
#endif
                    default:
                        unsigned funcno = opcode-cAbs;
                        assert(funcno < FUNC_AMOUNT);
                        const FuncDefinition& func = Functions[funcno];
                        data.Eat(func.params, OPCODE(opcode));
                        break;
                }
            }
            data.CheckConst();
        }
        return data.PullResult();
    }
}
