#define FP_TRACE_BYTECODE_OPTIMIZATION(from,to)
//#define FP_TRACE_BYTECODE_OPTIMIZATION(from,to) std::cout << "Changing \"" from "\"\n    into\"" to "\"\n"
inline void FunctionParser::AddFunctionOpcode(unsigned opcode)
{
    size_t blen = data->ByteCode.size();
    size_t ilen = data->Immed.size();
    switch(opcode)
    {
    case cPow:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cImmed:
          {
            double x = data->Immed.back();
            if(!isEvenInteger(x*2))
            {
                unsigned op_2 = data->ByteCode[blen - 2];
                switch(op_2)
                {
                case cSqr:
                  {
                    FP_TRACE_BYTECODE_OPTIMIZATION("cSqr x[!isEvenInteger(x*2)] cPow", "cAbs [x*2] cPow");
                    data->ByteCode.resize(blen - 2);
                    data->Immed.pop_back();
                    AddFunctionOpcode(cAbs);
                    data->Immed.push_back(x*2);
                    data->ByteCode.push_back(cImmed);
                    AddFunctionOpcode(cPow);
                    return;
                  }
                  break;
                }
            }
            if(IsIntegerConst(x))
            {
                unsigned op_2 = data->ByteCode[blen - 2];
                switch(op_2)
                {
                case cExp:
                  {
                    unsigned op_3 = data->ByteCode[blen - 3];
                    switch(op_3)
                    {
                    case cImmed:
                      {
                        double y = data->Immed[ilen - 2];
                        if(!IsIntegerConst(y))
                        {
                            FP_TRACE_BYTECODE_OPTIMIZATION("y[!IsIntegerConst(y)] cExp x[IsIntegerConst(x)] cPow", "[y*x] cExp");
                            data->Immed[ilen - 2] = y*x;
                            /* data->ByteCode[blen - 3] = cImmed; */ // redundant, matches y @ 3
                            /* data->ByteCode[blen - 2] = cExp; */ // redundant, matches cExp @ 2
                            data->ByteCode.pop_back();
                            data->Immed.pop_back();
                            return;
                        }
                        break;
                      }
                    }
                    FP_TRACE_BYTECODE_OPTIMIZATION("cExp x[IsIntegerConst(x)] cPow", "[x] cMul cExp");
                    data->Immed.back() = x;
                    data->ByteCode[blen - 2] = cImmed;
                    data->ByteCode.pop_back();
                    AddFunctionOpcode(cMul);
                    AddFunctionOpcode(cExp);
                    return;
                  }
                  break;
                case cExp2:
                  {
                    unsigned op_3 = data->ByteCode[blen - 3];
                    switch(op_3)
                    {
                    case cImmed:
                      {
                        double y = data->Immed[ilen - 2];
                        if(!IsIntegerConst(y))
                        {
                            FP_TRACE_BYTECODE_OPTIMIZATION("y[!IsIntegerConst(y)] cExp2 x[IsIntegerConst(x)] cPow", "[y*x] cExp2");
                            data->Immed[ilen - 2] = y*x;
                            /* data->ByteCode[blen - 3] = cImmed; */ // redundant, matches y @ 3
                            /* data->ByteCode[blen - 2] = cExp2; */ // redundant, matches cExp2 @ 2
                            data->ByteCode.pop_back();
                            data->Immed.pop_back();
                            return;
                        }
                        break;
                      }
                    }
                    FP_TRACE_BYTECODE_OPTIMIZATION("cExp2 x[IsIntegerConst(x)] cPow", "[x] cMul cExp2");
                    data->Immed.back() = x;
                    data->ByteCode[blen - 2] = cImmed;
                    data->ByteCode.pop_back();
                    AddFunctionOpcode(cMul);
                    AddFunctionOpcode(cExp2);
                    return;
                  }
                  break;
                case cPow:
                  {
                    unsigned op_3 = data->ByteCode[blen - 3];
                    switch(op_3)
                    {
                    case cImmed:
                      {
                        double y = data->Immed[ilen - 2];
                        if(!IsIntegerConst(y))
                        {
                            FP_TRACE_BYTECODE_OPTIMIZATION("y[!IsIntegerConst(y)] cPow x[IsIntegerConst(x)] cPow", "[y*x] cPow");
                            data->Immed[ilen - 2] = y*x;
                            /* data->ByteCode[blen - 3] = cImmed; */ // redundant, matches y @ 3
                            /* data->ByteCode[blen - 2] = cPow; */ // redundant, matches cPow @ 2
                            data->ByteCode.pop_back();
                            data->Immed.pop_back();
                            return;
                        }
                        break;
                      }
                    }
                    FP_TRACE_BYTECODE_OPTIMIZATION("cPow x[IsIntegerConst(x)] cPow", "[x] cMul cPow");
                    data->Immed.back() = x;
                    data->ByteCode[blen - 2] = cImmed;
                    data->ByteCode.pop_back();
                    AddFunctionOpcode(cMul);
                    AddFunctionOpcode(cPow);
                    return;
                  }
                  break;
                }
            }
            if(x==0.5)
            {
                FP_TRACE_BYTECODE_OPTIMIZATION("x[x==0.5] cPow", "cSqrt");
                data->ByteCode.pop_back();
                data->Immed.pop_back();
                AddFunctionOpcode(cSqrt);
                return;
            }
            if(x==-0.5)
            {
                FP_TRACE_BYTECODE_OPTIMIZATION("x[x==-0.5] cPow", "cRSqrt");
                data->ByteCode.back() = cRSqrt;
                data->Immed.pop_back();
                return;
            }
            if(x==-1.0)
            {
                FP_TRACE_BYTECODE_OPTIMIZATION("x[x==-1.0] cPow", "cInv");
                data->ByteCode.pop_back();
                data->Immed.pop_back();
                AddFunctionOpcode(cInv);
                return;
            }
            if(TryCompilePowi(x))
                return;
            break;
          }
        }
      }
      break;
    case cSqrt:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cSqr:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cSqr cSqrt", "cAbs");
            data->ByteCode.pop_back();
            AddFunctionOpcode(cAbs);
            return;
          }
          break;
        case cImmed:
          {
            double x = data->Immed.back();
            if(x>=0.0)
            {
                FP_TRACE_BYTECODE_OPTIMIZATION("x[x>=0.0] cSqrt", "[sqrt(x)]");
                data->Immed.back() = sqrt(x);
                /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
                return;
            }
            break;
          }
        }
      }
      break;
    case cAbs:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cNeg:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cNeg cAbs", "cAbs");
            data->ByteCode.pop_back();
            AddFunctionOpcode(cAbs);
            return;
          }
          break;
        case cImmed:
          {
            double x = data->Immed.back();
            FP_TRACE_BYTECODE_OPTIMIZATION("x cAbs", "[fabs(x)]");
            data->Immed.back() = fabs(x);
            /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
            return;
            break;
          }
        default:
            unsigned X = op_1; X=X;
            if(IsNeverNegativeValueOpcode(X))
            {
                FP_TRACE_BYTECODE_OPTIMIZATION("X[IsNeverNegativeValueOpcode(X)] cAbs", "X");
                /* data->ByteCode.back() = X; */ // redundant, matches X @ 1
                return;
            }
        }
      }
      break;
    case cAcos:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cImmed:
          {
            double x = data->Immed.back();
            FP_TRACE_BYTECODE_OPTIMIZATION("x cAcos", "[acos(x)]");
            data->Immed.back() = acos(x);
            /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
            return;
            break;
          }
        }
      }
      break;
    case cAcosh:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cImmed:
          {
            double x = data->Immed.back();
            if(x>=-1.0&&x<=1.0)
            {
                FP_TRACE_BYTECODE_OPTIMIZATION("x[x>=-1.0&&x<=1.0] cAcosh", "[fp_acosh(x)]");
                data->Immed.back() = fp_acosh(x);
                /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
                return;
            }
            break;
          }
        }
      }
      break;
    case cAsinh:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cImmed:
          {
            double x = data->Immed.back();
            if(x>=-1.0&&x<=1.0)
            {
                FP_TRACE_BYTECODE_OPTIMIZATION("x[x>=-1.0&&x<=1.0] cAsinh", "[fp_asinh(x)]");
                data->Immed.back() = fp_asinh(x);
                /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
                return;
            }
            break;
          }
        }
      }
      break;
    case cAtan:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cImmed:
          {
            double x = data->Immed.back();
            FP_TRACE_BYTECODE_OPTIMIZATION("x cAtan", "[atan(x)]");
            data->Immed.back() = atan(x);
            /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
            return;
            break;
          }
        }
      }
      break;
    case cAtanh:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cImmed:
          {
            double x = data->Immed.back();
            FP_TRACE_BYTECODE_OPTIMIZATION("x cAtanh", "[fp_atanh(x)]");
            data->Immed.back() = fp_atanh(x);
            /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
            return;
            break;
          }
        }
      }
      break;
    case cCeil:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cImmed:
          {
            double x = data->Immed.back();
            FP_TRACE_BYTECODE_OPTIMIZATION("x cCeil", "[ceil(x)]");
            data->Immed.back() = ceil(x);
            /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
            return;
            break;
          }
        default:
            unsigned X = op_1; X=X;
            if(IsAlwaysIntegerOpcode(X))
            {
                FP_TRACE_BYTECODE_OPTIMIZATION("X[IsAlwaysIntegerOpcode(X)] cCeil", "X");
                /* data->ByteCode.back() = X; */ // redundant, matches X @ 1
                return;
            }
        }
      }
      break;
    case cCos:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cAcos:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cAcos cCos", "");
            data->ByteCode.pop_back();
            return;
          }
          break;
        case cImmed:
          {
            double x = data->Immed.back();
            FP_TRACE_BYTECODE_OPTIMIZATION("x cCos", "[cos(x)]");
            data->Immed.back() = cos(x);
            /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
            return;
            break;
          }
        }
      }
      break;
    case cCosh:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cImmed:
          {
            double x = data->Immed.back();
            FP_TRACE_BYTECODE_OPTIMIZATION("x cCosh", "[cosh(x)]");
            data->Immed.back() = cosh(x);
            /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
            return;
            break;
          }
        }
      }
      break;
    case cExp:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cLog:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cLog cExp", "");
            data->ByteCode.pop_back();
            return;
          }
          break;
        case cAdd:
          {
            unsigned op_2 = data->ByteCode[blen - 2];
            switch(op_2)
            {
            case cImmed:
              {
                double x = data->Immed.back();
                FP_TRACE_BYTECODE_OPTIMIZATION("x cAdd cExp", "cExp [exp(x)] cMul");
                data->ByteCode.resize(blen - 2);
                data->Immed.pop_back();
                AddFunctionOpcode(cExp);
                data->Immed.push_back(exp(x));
                data->ByteCode.push_back(cImmed);
                AddFunctionOpcode(cMul);
                return;
                break;
              }
            }
          }
          break;
        case cImmed:
          {
            double x = data->Immed.back();
            FP_TRACE_BYTECODE_OPTIMIZATION("x cExp", "[exp(x)]");
            data->Immed.back() = exp(x);
            /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
            return;
            break;
          }
        }
      }
      break;
    case cExp2:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cLog2:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cLog2 cExp2", "");
            data->ByteCode.pop_back();
            return;
          }
          break;
        case cAdd:
          {
            unsigned op_2 = data->ByteCode[blen - 2];
            switch(op_2)
            {
            case cImmed:
              {
                double x = data->Immed.back();
                FP_TRACE_BYTECODE_OPTIMIZATION("x cAdd cExp2", "cExp2 [fp_pow(2.0,x)] cMul");
                data->ByteCode.resize(blen - 2);
                data->Immed.pop_back();
                AddFunctionOpcode(cExp2);
                data->Immed.push_back(fp_pow(2.0,x));
                data->ByteCode.push_back(cImmed);
                AddFunctionOpcode(cMul);
                return;
                break;
              }
            }
          }
          break;
        case cImmed:
          {
            double x = data->Immed.back();
            FP_TRACE_BYTECODE_OPTIMIZATION("x cExp2", "[fp_pow(2.0,x)]");
            data->Immed.back() = fp_pow(2.0,x);
            /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
            return;
            break;
          }
        }
      }
      break;
    case cFloor:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cImmed:
          {
            double x = data->Immed.back();
            FP_TRACE_BYTECODE_OPTIMIZATION("x cFloor", "[floor(x)]");
            data->Immed.back() = floor(x);
            /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
            return;
            break;
          }
        default:
            unsigned X = op_1; X=X;
            if(IsAlwaysIntegerOpcode(X))
            {
                FP_TRACE_BYTECODE_OPTIMIZATION("X[IsAlwaysIntegerOpcode(X)] cFloor", "X");
                /* data->ByteCode.back() = X; */ // redundant, matches X @ 1
                return;
            }
        }
      }
      break;
    case cInt:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cImmed:
          {
            double x = data->Immed.back();
            FP_TRACE_BYTECODE_OPTIMIZATION("x cInt", "[floor(x+0.5)]");
            data->Immed.back() = floor(x+0.5);
            /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
            return;
            break;
          }
        default:
            unsigned X = op_1; X=X;
            if(IsAlwaysIntegerOpcode(X))
            {
                FP_TRACE_BYTECODE_OPTIMIZATION("X[IsAlwaysIntegerOpcode(X)] cInt", "X");
                /* data->ByteCode.back() = X; */ // redundant, matches X @ 1
                return;
            }
        }
      }
      break;
    case cLog:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cExp:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cExp cLog", "");
            data->ByteCode.pop_back();
            return;
          }
          break;
        case cImmed:
          {
            double x = data->Immed.back();
            if(x>=0.0)
            {
                FP_TRACE_BYTECODE_OPTIMIZATION("x[x>=0.0] cLog", "[log(x)]");
                data->Immed.back() = log(x);
                /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
                return;
            }
            break;
          }
        }
      }
      break;
    case cLog10:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cImmed:
          {
            double x = data->Immed.back();
            if(x>=0.0)
            {
                FP_TRACE_BYTECODE_OPTIMIZATION("x[x>=0.0] cLog10", "[log10(x)]");
                data->Immed.back() = log10(x);
                /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
                return;
            }
            break;
          }
        }
      }
      break;
    case cLog2:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cExp2:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cExp2 cLog2", "");
            data->ByteCode.pop_back();
            return;
          }
          break;
        case cImmed:
          {
            double x = data->Immed.back();
            if(x>=0.0)
            {
                FP_TRACE_BYTECODE_OPTIMIZATION("x[x>=0.0] cLog2", "[log(x)*1.4426950408889634074]");
                data->Immed.back() = log(x)*1.4426950408889634074;
                /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
                return;
            }
            break;
          }
        }
      }
      break;
    case cSin:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cAsin:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cAsin cSin", "");
            data->ByteCode.pop_back();
            return;
          }
          break;
        case cImmed:
          {
            double x = data->Immed.back();
            FP_TRACE_BYTECODE_OPTIMIZATION("x cSin", "[sin(x)]");
            data->Immed.back() = sin(x);
            /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
            return;
            break;
          }
        }
      }
      break;
    case cSinh:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cImmed:
          {
            double x = data->Immed.back();
            FP_TRACE_BYTECODE_OPTIMIZATION("x cSinh", "[sinh(x)]");
            data->Immed.back() = sinh(x);
            /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
            return;
            break;
          }
        }
      }
      break;
    case cTan:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cImmed:
          {
            double x = data->Immed.back();
            FP_TRACE_BYTECODE_OPTIMIZATION("x cTan", "[tan(x)]");
            data->Immed.back() = tan(x);
            /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
            return;
            break;
          }
        }
      }
      break;
    case cTanh:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cImmed:
          {
            double x = data->Immed.back();
            FP_TRACE_BYTECODE_OPTIMIZATION("x cTanh", "[tanh(x)]");
            data->Immed.back() = tanh(x);
            /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
            return;
            break;
          }
        }
      }
      break;
    case cTrunc:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cImmed:
          {
            double x = data->Immed.back();
            FP_TRACE_BYTECODE_OPTIMIZATION("x cTrunc", "[trunc(x)]");
            data->Immed.back() = trunc(x);
            /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
            return;
            break;
          }
        default:
            unsigned X = op_1; X=X;
            if(IsAlwaysIntegerOpcode(X))
            {
                FP_TRACE_BYTECODE_OPTIMIZATION("X[IsAlwaysIntegerOpcode(X)] cTrunc", "X");
                /* data->ByteCode.back() = X; */ // redundant, matches X @ 1
                return;
            }
        }
      }
      break;
    case cDeg:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cImmed:
          {
            double x = data->Immed.back();
            FP_TRACE_BYTECODE_OPTIMIZATION("x cDeg", "[RadiansToDegrees(x)]");
            data->Immed.back() = RadiansToDegrees(x);
            /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
            return;
            break;
          }
        }
      }
      break;
    case cRad:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cImmed:
          {
            double x = data->Immed.back();
            FP_TRACE_BYTECODE_OPTIMIZATION("x cRad", "[DegreesToRadians(x)]");
            data->Immed.back() = DegreesToRadians(x);
            /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
            return;
            break;
          }
        }
      }
      break;
    case cNeg:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cMul:
          {
            unsigned op_2 = data->ByteCode[blen - 2];
            switch(op_2)
            {
            case cImmed:
              {
                double x = data->Immed.back();
                FP_TRACE_BYTECODE_OPTIMIZATION("x cMul cNeg", "[-x] cMul");
                data->Immed.back() = -x;
                /* data->ByteCode[blen - 2] = cImmed; */ // redundant, matches x @ 2
                /* data->ByteCode.back() = cMul; */ // redundant, matches cMul @ 1
                return;
                break;
              }
            }
          }
          break;
        case cNeg:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cNeg cNeg", "");
            data->ByteCode.pop_back();
            return;
          }
          break;
        case cFloor:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cFloor cNeg", "cNeg cCeil");
            data->ByteCode.pop_back();
            AddFunctionOpcode(cNeg);
            AddFunctionOpcode(cCeil);
            return;
          }
          break;
        case cCeil:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cCeil cNeg", "cNeg cFloor");
            data->ByteCode.pop_back();
            AddFunctionOpcode(cNeg);
            AddFunctionOpcode(cFloor);
            return;
          }
          break;
        case cImmed:
          {
            double x = data->Immed.back();
            FP_TRACE_BYTECODE_OPTIMIZATION("x cNeg", "[-x]");
            data->Immed.back() = -x;
            /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
            return;
            break;
          }
        }
      }
      break;
    case cInv:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cInv:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cInv cInv", "");
            data->ByteCode.pop_back();
            return;
          }
          break;
        case cPow:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cPow cInv", "cNeg cPow");
            data->ByteCode.pop_back();
            AddFunctionOpcode(cNeg);
            AddFunctionOpcode(cPow);
            return;
          }
          break;
        case cImmed:
          {
            double x = data->Immed.back();
            if(x!=0.0)
            {
                FP_TRACE_BYTECODE_OPTIMIZATION("x[x!=0.0] cInv", "[1.0/x]");
                data->Immed.back() = 1.0/x;
                /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
                return;
            }
            break;
          }
        }
      }
      break;
    case cMul:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cInv:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cInv cMul", "cDiv");
            data->ByteCode.pop_back();
            AddFunctionOpcode(cDiv);
            return;
          }
          break;
        case cPow:
          {
            unsigned op_2 = data->ByteCode[blen - 2];
            switch(op_2)
            {
            case cImmed:
              {
                double x = data->Immed.back();
                if(x<0)
                {
                    FP_TRACE_BYTECODE_OPTIMIZATION("x[x<0] cPow cMul", "[-x] cPow cDiv");
                    data->Immed.back() = -x;
                    /* data->ByteCode[blen - 2] = cImmed; */ // redundant, matches x @ 2
                    /* data->ByteCode.back() = cPow; */ // redundant, matches cPow @ 1
                    AddFunctionOpcode(cDiv);
                    return;
                }
                break;
              }
            }
          }
          break;
        case cImmed:
          {
            double x = data->Immed.back();
            unsigned op_2 = data->ByteCode[blen - 2];
            switch(op_2)
            {
            case cNeg:
              {
                FP_TRACE_BYTECODE_OPTIMIZATION("cNeg x cMul", "[-x] cMul");
                data->Immed.back() = -x;
                data->ByteCode[blen - 2] = cImmed;
                data->ByteCode.pop_back();
                AddFunctionOpcode(cMul);
                return;
              }
              break;
            case cMul:
              {
                unsigned op_3 = data->ByteCode[blen - 3];
                switch(op_3)
                {
                case cImmed:
                  {
                    double y = data->Immed[ilen - 2];
                    FP_TRACE_BYTECODE_OPTIMIZATION("y cMul x cMul", "[y*x] cMul");
                    data->Immed[ilen - 2] = y*x;
                    /* data->ByteCode[blen - 3] = cImmed; */ // redundant, matches y @ 3
                    /* data->ByteCode[blen - 2] = cMul; */ // redundant, matches cMul @ 2
                    data->ByteCode.pop_back();
                    data->Immed.pop_back();
                    return;
                    break;
                  }
                }
              }
              break;
            case cImmed:
              {
                double y = data->Immed[ilen - 2];
                FP_TRACE_BYTECODE_OPTIMIZATION("y x cMul", "[x*y]");
                data->Immed[ilen - 2] = x*y;
                /* data->ByteCode[blen - 2] = cImmed; */ // redundant, matches y @ 2
                data->ByteCode.pop_back();
                data->Immed.pop_back();
                return;
                break;
              }
            }
            if(x==1)
            {
                FP_TRACE_BYTECODE_OPTIMIZATION("x[x==1] cMul", "");
                data->ByteCode.pop_back();
                data->Immed.pop_back();
                return;
            }
            break;
          }
        }
      }
      break;
    case cDiv:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cInv:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cInv cDiv", "cMul");
            data->ByteCode.pop_back();
            AddFunctionOpcode(cMul);
            return;
          }
          break;
        case cExp:
          {
            unsigned op_2 = data->ByteCode[blen - 2];
            switch(op_2)
            {
            case cMul:
              {
                unsigned op_3 = data->ByteCode[blen - 3];
                switch(op_3)
                {
                case cImmed:
                  {
                    double x = data->Immed.back();
                    FP_TRACE_BYTECODE_OPTIMIZATION("x cMul cExp cDiv", "[-x] cMul cExp cMul");
                    data->Immed.back() = -x;
                    /* data->ByteCode[blen - 3] = cImmed; */ // redundant, matches x @ 3
                    /* data->ByteCode[blen - 2] = cMul; */ // redundant, matches cMul @ 2
                    /* data->ByteCode.back() = cExp; */ // redundant, matches cExp @ 1
                    AddFunctionOpcode(cMul);
                    return;
                    break;
                  }
                }
              }
              break;
            }
          }
          break;
        case cImmed:
          {
            double x = data->Immed.back();
            unsigned op_2 = data->ByteCode[blen - 2];
            switch(op_2)
            {
            case cNeg:
              {
                FP_TRACE_BYTECODE_OPTIMIZATION("cNeg x cDiv", "[-x] cDiv");
                data->Immed.back() = -x;
                data->ByteCode[blen - 2] = cImmed;
                data->ByteCode.pop_back();
                AddFunctionOpcode(cDiv);
                return;
              }
              break;
            }
            if(x!=0.0)
            {
                FP_TRACE_BYTECODE_OPTIMIZATION("x[x!=0.0] cDiv", "[1.0/x] cMul");
                data->Immed.back() = 1.0/x;
                /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
                AddFunctionOpcode(cMul);
                return;
            }
            if(x==1)
            {
                FP_TRACE_BYTECODE_OPTIMIZATION("x[x==1] cDiv", "");
                data->ByteCode.pop_back();
                data->Immed.pop_back();
                return;
            }
            break;
          }
        }
      }
      break;
    case cAdd:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cNeg:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cNeg cAdd", "cSub");
            data->ByteCode.pop_back();
            AddFunctionOpcode(cSub);
            return;
          }
          break;
        case cImmed:
          {
            double x = data->Immed.back();
            unsigned op_2 = data->ByteCode[blen - 2];
            switch(op_2)
            {
            case cAdd:
              {
                unsigned op_3 = data->ByteCode[blen - 3];
                switch(op_3)
                {
                case cImmed:
                  {
                    double y = data->Immed[ilen - 2];
                    FP_TRACE_BYTECODE_OPTIMIZATION("y cAdd x cAdd", "[y+x] cAdd");
                    data->Immed[ilen - 2] = y+x;
                    /* data->ByteCode[blen - 3] = cImmed; */ // redundant, matches y @ 3
                    /* data->ByteCode[blen - 2] = cAdd; */ // redundant, matches cAdd @ 2
                    data->ByteCode.pop_back();
                    data->Immed.pop_back();
                    return;
                    break;
                  }
                }
              }
              break;
            case cSub:
              {
                unsigned op_3 = data->ByteCode[blen - 3];
                switch(op_3)
                {
                case cImmed:
                  {
                    double y = data->Immed[ilen - 2];
                    FP_TRACE_BYTECODE_OPTIMIZATION("y cSub x cAdd", "[x-y] cAdd");
                    data->Immed[ilen - 2] = x-y;
                    /* data->ByteCode[blen - 3] = cImmed; */ // redundant, matches y @ 3
                    data->ByteCode.resize(blen - 2);
                    data->Immed.pop_back();
                    AddFunctionOpcode(cAdd);
                    return;
                    break;
                  }
                }
              }
              break;
            case cImmed:
              {
                double y = data->Immed[ilen - 2];
                FP_TRACE_BYTECODE_OPTIMIZATION("y x cAdd", "[y+x]");
                data->Immed[ilen - 2] = y+x;
                /* data->ByteCode[blen - 2] = cImmed; */ // redundant, matches y @ 2
                data->ByteCode.pop_back();
                data->Immed.pop_back();
                return;
                break;
              }
            }
            if(x==0)
            {
                FP_TRACE_BYTECODE_OPTIMIZATION("x[x==0] cAdd", "");
                data->ByteCode.pop_back();
                data->Immed.pop_back();
                return;
            }
            break;
          }
        }
      }
      break;
    case cSub:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cNeg:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cNeg cSub", "cAdd");
            data->ByteCode.pop_back();
            AddFunctionOpcode(cAdd);
            return;
          }
          break;
        case cImmed:
          {
            double x = data->Immed.back();
            unsigned op_2 = data->ByteCode[blen - 2];
            switch(op_2)
            {
            case cSub:
              {
                unsigned op_3 = data->ByteCode[blen - 3];
                switch(op_3)
                {
                case cImmed:
                  {
                    double y = data->Immed[ilen - 2];
                    FP_TRACE_BYTECODE_OPTIMIZATION("y cSub x cSub", "[x+y] cSub");
                    data->Immed[ilen - 2] = x+y;
                    /* data->ByteCode[blen - 3] = cImmed; */ // redundant, matches y @ 3
                    /* data->ByteCode[blen - 2] = cSub; */ // redundant, matches cSub @ 2
                    data->ByteCode.pop_back();
                    data->Immed.pop_back();
                    return;
                    break;
                  }
                }
              }
              break;
            case cAdd:
              {
                unsigned op_3 = data->ByteCode[blen - 3];
                switch(op_3)
                {
                case cImmed:
                  {
                    double y = data->Immed[ilen - 2];
                    FP_TRACE_BYTECODE_OPTIMIZATION("y cAdd x cSub", "[y-x] cAdd");
                    data->Immed[ilen - 2] = y-x;
                    /* data->ByteCode[blen - 3] = cImmed; */ // redundant, matches y @ 3
                    /* data->ByteCode[blen - 2] = cAdd; */ // redundant, matches cAdd @ 2
                    data->ByteCode.pop_back();
                    data->Immed.pop_back();
                    return;
                    break;
                  }
                }
              }
              break;
            case cImmed:
              {
                double y = data->Immed[ilen - 2];
                FP_TRACE_BYTECODE_OPTIMIZATION("y x cSub", "[y-x]");
                data->Immed[ilen - 2] = y-x;
                /* data->ByteCode[blen - 2] = cImmed; */ // redundant, matches y @ 2
                data->ByteCode.pop_back();
                data->Immed.pop_back();
                return;
                break;
              }
            }
            if(x==0)
            {
                FP_TRACE_BYTECODE_OPTIMIZATION("x[x==0] cSub", "");
                data->ByteCode.pop_back();
                data->Immed.pop_back();
                return;
            }
            break;
          }
        }
      }
      break;
    case cMin:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cImmed:
          {
            double x = data->Immed.back();
            unsigned op_2 = data->ByteCode[blen - 2];
            switch(op_2)
            {
            case cImmed:
              {
                double y = data->Immed[ilen - 2];
                FP_TRACE_BYTECODE_OPTIMIZATION("y x cMin", "[Min(x,y)]");
                data->Immed[ilen - 2] = Min(x,y);
                /* data->ByteCode[blen - 2] = cImmed; */ // redundant, matches y @ 2
                data->ByteCode.pop_back();
                data->Immed.pop_back();
                return;
                break;
              }
            }
            break;
          }
        }
      }
      break;
    case cMax:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cImmed:
          {
            double x = data->Immed.back();
            unsigned op_2 = data->ByteCode[blen - 2];
            switch(op_2)
            {
            case cImmed:
              {
                double y = data->Immed[ilen - 2];
                FP_TRACE_BYTECODE_OPTIMIZATION("y x cMax", "[Max(x,y)]");
                data->Immed[ilen - 2] = Max(x,y);
                /* data->ByteCode[blen - 2] = cImmed; */ // redundant, matches y @ 2
                data->ByteCode.pop_back();
                data->Immed.pop_back();
                return;
                break;
              }
            }
            break;
          }
        }
      }
      break;
    case cAtan2:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cImmed:
          {
            double x = data->Immed.back();
            unsigned op_2 = data->ByteCode[blen - 2];
            switch(op_2)
            {
            case cImmed:
              {
                double y = data->Immed[ilen - 2];
                FP_TRACE_BYTECODE_OPTIMIZATION("y x cAtan2", "[atan2(y,x)]");
                data->Immed[ilen - 2] = atan2(y,x);
                /* data->ByteCode[blen - 2] = cImmed; */ // redundant, matches y @ 2
                data->ByteCode.pop_back();
                data->Immed.pop_back();
                return;
                break;
              }
            }
            break;
          }
        }
      }
      break;
    case cNot:
      {
        unsigned op_1 = data->ByteCode.back();
        switch(op_1)
        {
        case cLess:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cLess cNot", "cGreaterOrEq");
            data->ByteCode.back() = cGreaterOrEq;
            return;
          }
          break;
        case cLessOrEq:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cLessOrEq cNot", "cGreater");
            data->ByteCode.back() = cGreater;
            return;
          }
          break;
        case cGreater:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cGreater cNot", "cLessOrEq");
            data->ByteCode.back() = cLessOrEq;
            return;
          }
          break;
        case cGreaterOrEq:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cGreaterOrEq cNot", "cLess");
            data->ByteCode.back() = cLess;
            return;
          }
          break;
        case cEqual:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cEqual cNot", "cNEqual");
            data->ByteCode.back() = cNEqual;
            return;
          }
          break;
        case cNEqual:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cNEqual cNot", "cEqual");
            data->ByteCode.back() = cEqual;
            return;
          }
          break;
        case cNeg:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cNeg cNot", "cNot");
            data->ByteCode.pop_back();
            AddFunctionOpcode(cNot);
            return;
          }
          break;
        case cAbs:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cAbs cNot", "cNot");
            data->ByteCode.pop_back();
            AddFunctionOpcode(cNot);
            return;
          }
          break;
        case cNot:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cNot cNot", "cNotNot");
            data->ByteCode.back() = cNotNot;
            return;
          }
          break;
        case cNotNot:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cNotNot cNot", "cNot");
            data->ByteCode.pop_back();
            AddFunctionOpcode(cNot);
            return;
          }
          break;
        case cAbsNotNot:
          {
            FP_TRACE_BYTECODE_OPTIMIZATION("cAbsNotNot cNot", "cAbsNot");
            data->ByteCode.back() = cAbsNot;
            return;
          }
          break;
        case cAbsNot:
          {
            unsigned op_2 = data->ByteCode[blen - 2];
            switch(op_2)
            {
            case cImmed: break;
            default:
                unsigned X = op_2; X=X;
                if(IsLogicalOpcode(X))
                {
                    FP_TRACE_BYTECODE_OPTIMIZATION("X[IsLogicalOpcode(X)] cAbsNot cNot", "X");
                    /* data->ByteCode[blen - 2] = X; */ // redundant, matches X @ 2
                    data->ByteCode.pop_back();
                    return;
                }
                FP_TRACE_BYTECODE_OPTIMIZATION("X cAbsNot cNot", "X cAbsNotNot");
                /* data->ByteCode[blen - 2] = X; */ // redundant, matches X @ 2
                data->ByteCode.back() = cAbsNotNot;
                return;
            }
          }
          break;
        case cImmed:
          {
            double x = data->Immed.back();
            FP_TRACE_BYTECODE_OPTIMIZATION("x cNot", "[!truthValue(x)]");
            data->Immed.back() = !truthValue(x);
            /* data->ByteCode.back() = cImmed; */ // redundant, matches x @ 1
            return;
            break;
          }
        default:
            unsigned X = op_1; X=X;
            if(IsNeverNegativeValueOpcode(X))
            {
                FP_TRACE_BYTECODE_OPTIMIZATION("X[IsNeverNegativeValueOpcode(X)] cNot", "X cAbsNot");
                /* data->ByteCode.back() = X; */ // redundant, matches X @ 1
                data->ByteCode.push_back(cAbsNot);
                return;
            }
        }
      }
      break;
    }
    data->ByteCode.push_back(opcode);
}
