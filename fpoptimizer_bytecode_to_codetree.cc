#include <cmath>
#include <list>
#include <cassert>

#include "fpoptimizer.hh"
#include "fptypes.hh"

#include "fpoptimizer_consts.hh"


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
        
        bool& GetLastOpParamSign(unsigned paramno)
        {
            return stack.back()->Params[paramno].sign;
        }
        
        void CloneLastOpLastParam()
        {
            CodeTree& lastop = *stack.back();
            const CodeTree::Param& refparam = lastop.Params.back();
            
            CodeTree::Param param;
            param.param = refparam.param->Clone();
            param.sign  = refparam.sign;
            lastop.Params.push_back(param);
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
        unsigned n_vars)
    {
        CodeTreeParserData data;
        std::list<size_t> labels;
        
        for(size_t IP=0, DP=0; ; )
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
                    // Unary operators requiring special attention
                    case cInv:
                        data.Eat(1, cMul); // Unary division is inverse multiplying
                        data.GetLastOpParamSign(0) = true;
                        break;
                    case cNeg:
                        data.Eat(1, cAdd); // Unary minus is negative adding.
                        data.GetLastOpParamSign(0) = true;
                        break;
                    case cSqr:
                        data.Eat(1, cMul);
                        data.CloneLastOpLastParam();
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
                        data.GetLastOpParamSign(0) = true;
                        break;
                    case cCsc:
                        data.Eat(1, cSin);
                        data.Eat(1, cMul);
                        data.GetLastOpParamSign(0) = true;
                        break;
                    case cSec:
                        data.Eat(1, cCos);
                        data.Eat(1, cMul);
                        data.GetLastOpParamSign(0) = true;
                        break;
                    case cLog10:
                        data.Eat(1, cLog);
                        data.AddConst(CONSTANT_L10I);
                        data.Eat(2, cMul);
                        break;
                    // Binary operators requiring special attention
                    case cSub:
                        data.Eat(2, cAdd); // Minus is negative adding
                        data.GetLastOpParamSign(1) = true;
                        break;
                    case cRSub:
                        data.Eat(2, cAdd);
                        data.GetLastOpParamSign(0) = true;
                        break;
                    case cDiv:
                        data.Eat(2, cMul); // Divide is inverse multiply
                        data.GetLastOpParamSign(1) = true;
                        break;
                    case cRDiv:
                        data.Eat(2, cMul);
                        data.GetLastOpParamSign(0) = true;
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
                        unsigned paramcount = unsigned(n_vars);
                        data.Eat(paramcount, OPCODE(opcode));
                        break;
                    }
    #endif
                    default:
                        unsigned funcno = opcode-cAbs;
                        assert(funcno < sizeof(Functions)/sizeof(Functions[0]));
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
