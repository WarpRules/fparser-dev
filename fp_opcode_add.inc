#define q1 ByteCodePtr[
#define q3 data->ByteCode.
#define qL pop_back();
#define q81 unsigned
#define q9 for(q81 tmp=2;tmp-->0;)q3 qL
#define qM AddFunctionOpcode
#define qQ :ImmedPtr[
#define q2 qQ 0]=-x;q9 qM(cMul);opcode=
#define qD1 :q3 qL qM
#define q4 qD1
#define qF =ImmedPtr[
#define qJ cImmed
#define qF1 q1-2]==
#define q5 :if(q1-1]==cMul){if(qF1 qJ){x qF 0];
#define qU >Value_t(0)
#define q6 x qF 0];if(x qU){
#define q7 if(IsNeverNegativeValueOpcode(A)){
#define q8 :B=q1-2];if(IsNeverNegativeValueOpcode(B)){
#define qD goto
#define qP data->Immed.
#define qK :qP qL
#define qA qK q3 qL qD
#define qB &&!HasInvalidRangesOpcode
#define qI ;qM(cMul);
#define qC q3 pop_back()qI
#define qV qJ:x qF 0];
#define qE ;case qV
#define qH1 Default0
#define qG }}qD qH1;
#define qH q4(cNeg);qM
#define qN ;qD Llh;
#define qO }qD Default1;case
#define qR q1 0]
#define qS }break;case
#define qB1 ;}break;
#define qT qB1}qD qH1;
#define qW {case cDup:
#define qX (x);qD Lnq;
#define qY ;qD Lbd;
#define q01 B=q1-2];if(B==A){
#define q11 IsUnaryOpcode
#define q21 (y,x);qD Lai;
#define q31 (x,y);qD Lai;
#define q41 x==Value_t
#define qL1 {switch
#define q51 qL1(q1-1]){case cAbs:
#define q61 ;case qJ:q6
#define q71 (x);return;
#define q91 ;}qS cSqr:
#define qA1 y qF-1];
#define qC1 qP qL for
#define qE1 ;Value_t
#define qG1 Default5
#define qI1 fp_log10
#define qJ1 qQ-1]=
#define qK1 :switch
#define qM1 q1-3]==
#define qN1 qS qJ:qA1
#define qO1 fp_log2
#define qP1 qD Lmo;

#define FP_TRACE_OPCODENAME(op) \
    (op < VarBegin \
        ? FP_GetOpcodeName(OPCODE(op)) \
        : findName(data->namePtrs,op,NameData<Value_t>::VARIABLE))


#define FP_TRACE_BYTECODE_OPTIMIZATION(srcline,from,to,with) \
    /*std::cout << "Changing \"" from "\"\t(line " #srcline ")\n" \
                   "    into \"" to "\"\n" with*/

unsigned*ByteCodePtr;Value_t*ImmedPtr;
#define FP_ReDefinePointers() \
    ByteCodePtr = !data->ByteCode.empty() ? &data->ByteCode[0] + data->ByteCode.size() - 1 : 0; \
    ImmedPtr    = !data->Immed.empty()    ? &data->Immed[0]    + data->Immed.size()    - 1 : 0;

FP_ReDefinePointers();
#if(!FP_FLOAT_VERSION)

Value_t
x;q81
A
qE1
y;q81
B;q81
C;q81
D;switch(opcode){TailCall_cAbs:case
cAbs:switch(ByteCodePtr[0]){case
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(197,"cNeg cAbs","cAbs","");goto
Laa;case
cImmed:x=ImmedPtr[0];FP_TRACE_BYTECODE_OPTIMIZATION(40,"x cAbs","[fp_abs(x)]","    with x = "<<x<<"\n");qD
Lab;default:A=q1
0];if(IsNeverNegativeValueOpcode(A)){FP_TRACE_BYTECODE_OPTIMIZATION(258,"A[IsNeverNegativeValueOpcode(A)] cAbs","A","    with A = "<<FP_TRACE_OPCODENAME(A)<<"\n");return;}
}
qD
Default0;TailCall_cAdd:case
cAdd
qK1(qR){case
cDup
qK1(q1-1]){case
cAdd:if(q1-2]==cDup){FP_TRACE_BYTECODE_OPTIMIZATION(147,"cDup cAdd cDup cAdd","[Value_t(4)] cMul","");qD
Lac;}
break;case
cMul:if(qF1
cAdd){if(q1-3]==cDup){FP_TRACE_BYTECODE_OPTIMIZATION(148,"cDup cAdd cMul cDup cAdd","cMul [Value_t(4)] cMul","");qD
Lad;}
}
break;qS
cMul:if(q1-1]==cImmed){x=ImmedPtr[0];if(qF1
cDup){FP_TRACE_BYTECODE_OPTIMIZATION(150,"cDup x cMul cAdd","[x+Value_t(1)] cMul","    with x = "<<x<<"\n");qD
Lae;}
qS
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(204,"cNeg cAdd","cSub","");qD
Laf;case
qJ:x
qF
0];switch(q1-1]){case
cAdd:if(qF1
qJ){y
qF-1];FP_TRACE_BYTECODE_OPTIMIZATION(141,"y cAdd x cAdd","[y+x] cAdd","    with x = "<<x<<", y = "<<y<<"\n");qD
Lag;qS
qJ:qA1
FP_TRACE_BYTECODE_OPTIMIZATION(76,"y x cAdd","[y+x]","    with x = "<<x<<", y = "<<y<<"\n");qD
Lah;}
if(x==Value_t(0)){FP_TRACE_BYTECODE_OPTIMIZATION(75,"x[x==Value_t(0)] cAdd","","    with x = "<<x<<"\n");qD
Lai;}
break;}
qD
Default0;TailCall_cAnd:case
cAnd:if(qR==qJ){x
qF
0];if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(130,"y x cAnd","[fp_and(x,y)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Laj;qG
TailCall_cDiv:case
cDiv
qK1(qR){case
cDup:FP_TRACE_BYTECODE_OPTIMIZATION(120,"cDup cDiv","[Value_t(0)] cMul [Value_t(1)] cAdd","");qD
Lak;case
qV
if(x!=Value_t(0)){switch(q1-1]){case
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(138,"cNeg x[x!=Value_t(0)] cDiv","[-x] cDiv","    with x = "<<x<<"\n");qD
Lal;case
qJ:qA1
FP_TRACE_BYTECODE_OPTIMIZATION(73,"y x[x!=Value_t(0)] cDiv","[y/x]","    with x = "<<x<<", y = "<<y<<"\n");qD
Lam;}
}
if(x==Value_t(1)){FP_TRACE_BYTECODE_OPTIMIZATION(72,"x[x==Value_t(1)] cDiv","","    with x = "<<x<<"\n");qD
Lai;}
break;}
qD
Default0;TailCall_cEqual:case
cEqual:if(qR==qJ){x
qF
0];if(q41(1)){A=q1-1];if(IsLogicalOpcode(A)){FP_TRACE_BYTECODE_OPTIMIZATION(286,"A[IsLogicalOpcode(A)] x[x==Value_t(1)] cEqual","A","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lai;}
}
if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(128,"y x cEqual","[fp_equal(y,x)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Lan;}
if(q41(0)){switch(q1-1]){case
cAbs:FP_TRACE_BYTECODE_OPTIMIZATION(216,"cAbs x[x==Value_t(0)] cEqual","[x] cEqual","    with x = "<<x<<"\n");qD
Lao;case
cSqr:FP_TRACE_BYTECODE_OPTIMIZATION(218,"cSqr x[x==Value_t(0)] cEqual","[x] cEqual","    with x = "<<x<<"\n");qD
Lao;}
FP_TRACE_BYTECODE_OPTIMIZATION(276,"x[x==Value_t(0)] cEqual","cNot","    with x = "<<x<<"\n");qD
Lap;qG
TailCall_cGreater:case
cGreater:if(qR==qJ){x
qF
0];if(q41(0))q51
FP_TRACE_BYTECODE_OPTIMIZATION(278,"cAbs x[x==Value_t(0)] cGreater","cNotNot","    with x = "<<x<<"\n");qD
Laq;default:A=q1-1];q7
FP_TRACE_BYTECODE_OPTIMIZATION(282,"A[IsNeverNegativeValueOpcode(A)] x[x==Value_t(0)] cGreater","A cNotNot","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lba;}
}
}
if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(126,"y x cGreater","[fp_less(x,y)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Lbb;qG
TailCall_cGreaterOrEq:case
cGreaterOrEq:if(qR==qJ){x
qF
0];if(q41(1))q51
FP_TRACE_BYTECODE_OPTIMIZATION(279,"cAbs x[x==Value_t(1)] cGreaterOrEq","cNotNot","    with x = "<<x<<"\n");qD
Laq;default:A=q1-1];q7
FP_TRACE_BYTECODE_OPTIMIZATION(283,"A[IsNeverNegativeValueOpcode(A)] x[x==Value_t(1)] cGreaterOrEq","A cNotNot","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lba;}
}
}
if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(127,"y x cGreaterOrEq","[fp_lessOrEq(x,y)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Lbc;qG
TailCall_cInv:case
cInv
qK1(qR){case
cInv:FP_TRACE_BYTECODE_OPTIMIZATION(198,"cInv cInv","","");qD
Lbd;case
cSqrt:FP_TRACE_BYTECODE_OPTIMIZATION(67,"cSqrt cInv","cRSqrt","");qD
Lbe;case
qV
if(x!=Value_t(0)){FP_TRACE_BYTECODE_OPTIMIZATION(69,"x[x!=Value_t(0)] cInv","[Value_t(1)/x]","    with x = "<<x<<"\n");qD
Lbf;}
break;}
qD
Default0;TailCall_cLess:case
cLess:if(qR==qJ){x
qF
0];if(q41(1))q51
FP_TRACE_BYTECODE_OPTIMIZATION(280,"cAbs x[x==Value_t(1)] cLess","cNot","    with x = "<<x<<"\n");qD
Lbg;default:A=q1-1];q7
FP_TRACE_BYTECODE_OPTIMIZATION(284,"A[IsNeverNegativeValueOpcode(A)] x[x==Value_t(1)] cLess","A cNot","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lap;}
}
}
if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(124,"y x cLess","[fp_less(y,x)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Lbh;qG
TailCall_cLessOrEq:case
cLessOrEq:if(qR==qJ){x
qF
0];if(q41(0))q51
FP_TRACE_BYTECODE_OPTIMIZATION(281,"cAbs x[x==Value_t(0)] cLessOrEq","cNot","    with x = "<<x<<"\n");qD
Lbg;default:A=q1-1];q7
FP_TRACE_BYTECODE_OPTIMIZATION(285,"A[IsNeverNegativeValueOpcode(A)] x[x==Value_t(0)] cLessOrEq","A cNot","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lap;}
}
}
if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(125,"y x cLessOrEq","[fp_lessOrEq(y,x)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Lbi;qG
TailCall_cMax:case
cMax
qK1(qR){case
cDup:FP_TRACE_BYTECODE_OPTIMIZATION(110,"cDup cMax","","");qD
Lbd;case
qV
if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(82,"y x cMax","[fp_max(x,y)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Lbj;}
break;default:A=qR;if(A>=VarBegin){switch(q1-1])qW
B=q1-2];if(B==A){FP_TRACE_BYTECODE_OPTIMIZATION(112,"B[B==A] cDup A[A>=VarBegin] cMax","B cDup","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n")qY
qS
cMax:q01
FP_TRACE_BYTECODE_OPTIMIZATION(114,"B[B==A] cMax A[A>=VarBegin] cMax","B cMax","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n")qY}
break;}
qG
TailCall_cMin:case
cMin
qK1(qR)qW
FP_TRACE_BYTECODE_OPTIMIZATION(109,"cDup cMin","","")qY
case
qV
if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(81,"y x cMin","[fp_min(x,y)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Lbk;}
break;default:A=qR;if(A>=VarBegin){switch(q1-1])qW
q01
FP_TRACE_BYTECODE_OPTIMIZATION(111,"B[B==A] cDup A[A>=VarBegin] cMin","B cDup","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n")qY
qS
cMin:q01
FP_TRACE_BYTECODE_OPTIMIZATION(113,"B[B==A] cMin A[A>=VarBegin] cMin","B cMin","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n")qY}
break;}
qG
TailCall_cMod:case
cMod:if(qR==qJ){x
qF
0];if(x!=Value_t(0)){if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(74,"y x[x!=Value_t(0)] cMod","[fp_mod(y,x)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Lbl;}
qG
TailCall_cMul:case
cMul
qK1(qR){case
cAdd:if(q1-1]==qJ){y
qF
0];A=q1-2];if(A>=VarBegin){if(q1-3]==qJ){x
qF-1];FP_TRACE_BYTECODE_OPTIMIZATION(245,"x A[A>=VarBegin] y cAdd cMul","[x] A cMul [y*x] cAdd","    with y = "<<y<<", A = "<<FP_TRACE_OPCODENAME(A)<<", x = "<<x<<"\n");qD
Lbm;}
}
}
qD
Default1;case
cDup:FP_TRACE_BYTECODE_OPTIMIZATION(359,"cDup cMul","cSqr","");qD
Lbn;case
cInv:FP_TRACE_BYTECODE_OPTIMIZATION(203,"cInv cMul","cDiv","");qD
Lbo;case
cNeg:A=q1-1];if(A>=VarBegin){switch(q1-2]){case
cMul:B=q1-3];if(B==A){FP_TRACE_BYTECODE_OPTIMIZATION(351,"B[B==A] cMul A[A>=VarBegin] cNeg cMul","B cSqr cMul cNeg","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lbp;}
qD
Default2;default:Default2:;q01
FP_TRACE_BYTECODE_OPTIMIZATION(345,"B[B==A] A[A>=VarBegin] cNeg cMul","B cSqr cNeg","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lbq;}
}
qO
cPow:if(q1-1]==qJ){x
qF
0];if(qF1
cDup){FP_TRACE_BYTECODE_OPTIMIZATION(151,"cDup x cPow cMul","[x+Value_t(1)] cPow","    with x = "<<x<<"\n");qD
Lca;}
qO
qV
if(q41(0)){switch(q1-1]){case
cMul:A=q1-2];if(A>=VarBegin){FP_TRACE_BYTECODE_OPTIMIZATION(94,"A[A>=VarBegin] cMul x[x==Value_t(0)] cMul","[x] cMul","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lcb;}
qD
Default3;default:Default3:;A=q1-1];if(IsBinaryOpcode(A)&&!HasInvalidRangesOpcode(A))qL1(q1-2]){case
qJ:qA1
FP_TRACE_BYTECODE_OPTIMIZATION(93,"y A[IsBinaryOpcode(A)&&!HasInvalidRangesOpcode(A)] x[x==Value_t(0)] cMul","[x] cMul","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<", y = "<<y<<"\n");qD
Lcc;default:B=q1-2];if(IsBinaryOpcode(B)qB(B))qL1(q1-3]){case
qJ:qA1
FP_TRACE_BYTECODE_OPTIMIZATION(97,"y B[IsBinaryOpcode(B)&&!HasInvalidRangesOpcode(B)] A[IsBinaryOpcode(A)&&!HasInvalidRangesOpcode(A)] x[x==Value_t(0)] cMul","A [x] cMul","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<", y = "<<y<<"\n");qD
Lcd;default:C=q1-3];if(C>=VarBegin){FP_TRACE_BYTECODE_OPTIMIZATION(95,"C[C>=VarBegin] B[IsBinaryOpcode(B)&&!HasInvalidRangesOpcode(B)] A[IsBinaryOpcode(A)&&!HasInvalidRangesOpcode(A)] x[x==Value_t(0)] cMul","A [x] cMul","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<", C = "<<FP_TRACE_OPCODENAME(C)<<"\n");qD
Lce;}
if(IsUnaryOpcode(C)qB(C)){FP_TRACE_BYTECODE_OPTIMIZATION(96,"C[IsUnaryOpcode(C)&&!HasInvalidRangesOpcode(C)] B[IsBinaryOpcode(B)&&!HasInvalidRangesOpcode(B)] A[IsBinaryOpcode(A)&&!HasInvalidRangesOpcode(A)] x[x==Value_t(0)] cMul","B A [x] cMul","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<", C = "<<FP_TRACE_OPCODENAME(C)<<"\n");qD
Lcf;}
}
}
if(B>=VarBegin){FP_TRACE_BYTECODE_OPTIMIZATION(91,"B[B>=VarBegin] A[IsBinaryOpcode(A)&&!HasInvalidRangesOpcode(A)] x[x==Value_t(0)] cMul","[x] cMul","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lcb;}
if(q11(B)qB(B)){FP_TRACE_BYTECODE_OPTIMIZATION(92,"B[IsUnaryOpcode(B)&&!HasInvalidRangesOpcode(B)] A[IsBinaryOpcode(A)&&!HasInvalidRangesOpcode(A)] x[x==Value_t(0)] cMul","A [x] cMul","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lcg;}
}
}
if(A>=VarBegin){FP_TRACE_BYTECODE_OPTIMIZATION(89,"A[A>=VarBegin] x[x==Value_t(0)] cMul","[x]","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lch;}
if(q11(A)qB(A)){FP_TRACE_BYTECODE_OPTIMIZATION(90,"A[IsUnaryOpcode(A)&&!HasInvalidRangesOpcode(A)] x[x==Value_t(0)] cMul","[x] cMul","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lci;}
}
}
switch(q1-1]){case
cAdd
qK1(q1-2])qW
FP_TRACE_BYTECODE_OPTIMIZATION(155,"cDup cAdd x cMul","[x+x] cMul","    with x = "<<x<<"\n");qD
Lcj;case
cMul:if(q1-3]==qJ){qA1
A=q1-4];if(A>=VarBegin){FP_TRACE_BYTECODE_OPTIMIZATION(243,"A[A>=VarBegin] y cMul cAdd x cMul","[x] cMul A [y*x] cMul cAdd","    with x = "<<x<<", y = "<<y<<", A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lck;}
qS
qJ:qA1
FP_TRACE_BYTECODE_OPTIMIZATION(242,"y cAdd x cMul","[x] cMul [y*x] cAdd","    with x = "<<x<<", y = "<<y<<"\n");qD
Lcl;qS
cMul
qK1(q1-2]){case
cAdd:if(qM1
cDup){FP_TRACE_BYTECODE_OPTIMIZATION(157,"cDup cAdd cMul x cMul","cMul [x+x] cMul","    with x = "<<x<<"\n");qD
Lcm;qN1
if(y*q41(1)){FP_TRACE_BYTECODE_OPTIMIZATION(145,"y[y*x==Value_t(1)] cMul x cMul","","    with x = "<<x<<", y = "<<y<<"\n");qD
Lcn;}
FP_TRACE_BYTECODE_OPTIMIZATION(146,"y cMul x cMul","[y*x] cMul","    with x = "<<x<<", y = "<<y<<"\n");qD
Lco;qS
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(135,"cNeg x cMul","[-x] cMul","    with x = "<<x<<"\n");qD
Lcp;case
cSub:if(qF1
cMul){if(qM1
qJ){qA1
A=q1-4];if(A>=VarBegin){FP_TRACE_BYTECODE_OPTIMIZATION(244,"A[A>=VarBegin] y cMul cSub x cMul","[x] cMul A [y*x] cMul cSub","    with x = "<<x<<", y = "<<y<<", A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lcq;}
}
qN1
FP_TRACE_BYTECODE_OPTIMIZATION(71,"y x cMul","[y*x]","    with x = "<<x<<", y = "<<y<<"\n");qD
Lda;}
if(q41(1)){FP_TRACE_BYTECODE_OPTIMIZATION(70,"x[x==Value_t(1)] cMul","","    with x = "<<x<<"\n");qD
Lai;}
if(q41(2)){FP_TRACE_BYTECODE_OPTIMIZATION(100,"x[x==Value_t(2)] cMul","cDup cAdd","    with x = "<<x<<"\n");qD
Ldb;}
if(q41(-1)){FP_TRACE_BYTECODE_OPTIMIZATION(137,"x[x==Value_t(-1)] cMul","cNeg","    with x = "<<x<<"\n");qD
Ldc;}
break;default:Default1:;A=qR;if(A>=VarBegin)qL1(q1-1]){case
cMul
qK1(q1-2]){case
cNeg:B=q1-3];if(B==A){FP_TRACE_BYTECODE_OPTIMIZATION(348,"B[B==A] cNeg cMul A[A>=VarBegin] cMul","B cSqr cMul cNeg","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lbp;}
qD
Default4;default:Default4:;q01
FP_TRACE_BYTECODE_OPTIMIZATION(339,"B[B==A] cMul A[A>=VarBegin] cMul","B cSqr cMul","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Ldd;}
}
qD
Default5;case
cNeg:q01
FP_TRACE_BYTECODE_OPTIMIZATION(342,"B[B==A] cNeg A[A>=VarBegin] cMul","B cSqr cNeg","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lbq;}
qD
qG1;default:qG1:;B=q1-1];if(B==A){FP_TRACE_BYTECODE_OPTIMIZATION(336,"B[B==A] A[A>=VarBegin] cMul","B cSqr","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lbn;}
}
}
if(q11(A)){B=q1-1];if(B>=VarBegin&&data->ByteCode.size()>1){if(qF1
cMul){C=q1-3];if(C==A){D=q1-4];if(D==B){FP_TRACE_BYTECODE_OPTIMIZATION(358,"D[D==B] C[C==A] cMul B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)] cMul","D C cSqr cMul","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<", C = "<<FP_TRACE_OPCODENAME(C)<<", D = "<<FP_TRACE_OPCODENAME(D)<<"\n");qD
Lde;}
}
}
}
qG
TailCall_cNEqual:case
cNEqual:if(qR==qJ){x
qF
0];if(q41(1)){A=q1-1];if(IsLogicalOpcode(A)){FP_TRACE_BYTECODE_OPTIMIZATION(287,"A[IsLogicalOpcode(A)] x[x==Value_t(1)] cNEqual","A cNot","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lap;}
}
if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(129,"y x cNEqual","[fp_nequal(y,x)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Ldf;}
if(q41(0))q51
FP_TRACE_BYTECODE_OPTIMIZATION(217,"cAbs x[x==Value_t(0)] cNEqual","[x] cNEqual","    with x = "<<x<<"\n");qD
Ldg;case
cSqr:FP_TRACE_BYTECODE_OPTIMIZATION(219,"cSqr x[x==Value_t(0)] cNEqual","[x] cNEqual","    with x = "<<x<<"\n");qD
Ldg;}
FP_TRACE_BYTECODE_OPTIMIZATION(277,"x[x==Value_t(0)] cNEqual","cNotNot","    with x = "<<x<<"\n");qD
Lba;qG
TailCall_cNeg:case
cNeg
qK1(qR){case
cMul:if(q1-1]==qJ){x
qF
0];FP_TRACE_BYTECODE_OPTIMIZATION(136,"x cMul cNeg","[-x] cMul","    with x = "<<x<<"\n");qD
Ldh;qS
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(199,"cNeg cNeg","","")qY
case
qV
FP_TRACE_BYTECODE_OPTIMIZATION(68,"x cNeg","[-x]","    with x = "<<x<<"\n");qD
Ldi;}
qD
Default0;TailCall_cNot:case
cNot
qK1(qR){case
cAbs:FP_TRACE_BYTECODE_OPTIMIZATION(249,"cAbs cNot","cNot","");qD
Ldj;case
cAbsNot:A=q1-1];if(IsLogicalOpcode(A)){FP_TRACE_BYTECODE_OPTIMIZATION(254,"A[IsLogicalOpcode(A)] cAbsNot cNot","A","    with A = "<<FP_TRACE_OPCODENAME(A)<<"\n")qY}
if(A!=qJ){FP_TRACE_BYTECODE_OPTIMIZATION(255,"A[A!=cImmed] cAbsNot cNot","A cAbsNotNot","    with A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Ldk;}
qD
Default6;case
cAbsNotNot:FP_TRACE_BYTECODE_OPTIMIZATION(252,"cAbsNotNot cNot","cAbsNot","");qD
Ldl;case
cAdd:if(q1-1]==qJ){x
qF
0];FP_TRACE_BYTECODE_OPTIMIZATION(289,"x cAdd cNot","[x] cEqual","    with x = "<<x<<"\n");qD
Ldm;}
qD
Default6;case
cEqual:FP_TRACE_BYTECODE_OPTIMIZATION(213,"cEqual cNot","cNEqual","");qD
Ldn;case
cGreater:FP_TRACE_BYTECODE_OPTIMIZATION(211,"cGreater cNot","cLessOrEq","");qD
Ldo;case
cGreaterOrEq:FP_TRACE_BYTECODE_OPTIMIZATION(212,"cGreaterOrEq cNot","cLess","");qD
Ldp;case
cLess:FP_TRACE_BYTECODE_OPTIMIZATION(209,"cLess cNot","cGreaterOrEq","");qD
Ldq;case
cLessOrEq:FP_TRACE_BYTECODE_OPTIMIZATION(210,"cLessOrEq cNot","cGreater","");qD
Lea;case
cNEqual:FP_TRACE_BYTECODE_OPTIMIZATION(214,"cNEqual cNot","cEqual","");qD
Leb;case
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(248,"cNeg cNot","cNot","");qD
Ldj;case
cNot:FP_TRACE_BYTECODE_OPTIMIZATION(250,"cNot cNot","cNotNot","");qD
Lec;case
cNotNot:FP_TRACE_BYTECODE_OPTIMIZATION(251,"cNotNot cNot","cNot","");qD
Ldj;case
qV
FP_TRACE_BYTECODE_OPTIMIZATION(85,"x cNot","[fp_not(x)]","    with x = "<<x<<"\n");qD
Led;default:Default6:;A=qR;q7
FP_TRACE_BYTECODE_OPTIMIZATION(256,"A[IsNeverNegativeValueOpcode(A)] cNot","A cAbsNot","    with A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lee;qG
TailCall_cNotNot:case
cNotNot:if(qR==cAdd){if(q1-1]==qJ){x
qF
0];FP_TRACE_BYTECODE_OPTIMIZATION(288,"x cAdd cNotNot","[x] cNEqual","    with x = "<<x<<"\n");qD
Lef;qG
TailCall_cOr:case
cOr:if(qR==qJ){x
qF
0];if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(131,"y x cOr","[fp_or(x,y)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Leg;qG
TailCall_cSqr:case
cSqr
qK1(qR){case
cAbs:FP_TRACE_BYTECODE_OPTIMIZATION(354,"cAbs cSqr","cSqr","");qD
Leh;case
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(353,"cNeg cSqr","cSqr","");qD
Leh;}
qD
Default0;TailCall_cSub:case
cSub
qK1(qR)qW
FP_TRACE_BYTECODE_OPTIMIZATION(152,"cDup cSub","[Value_t(0)] cMul","");qD
Lei;case
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(205,"cNeg cSub","cAdd","");qD
Lej;case
qV
if(q41(0)){FP_TRACE_BYTECODE_OPTIMIZATION(77,"x[x==Value_t(0)] cSub","","    with x = "<<x<<"\n");qD
Lai;}
if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(78,"y x cSub","[y-x]","    with x = "<<x<<", y = "<<y<<"\n");qD
Lek;}
FP_TRACE_BYTECODE_OPTIMIZATION(168,"x cSub","[-x] cAdd","    with x = "<<x<<"\n");qD
Lel;}
qD
qH1;default:qH1:;A=opcode;if(IsComparisonOpcode(A)){if(qR==qJ){x
qF
0];switch(q1-1]){case
cAdd:if(qF1
qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(221,"y cAdd x A[IsComparisonOpcode(A)]","[x-y] A","    with A = "<<FP_TRACE_OPCODENAME(A)<<", x = "<<x<<", y = "<<y<<"\n");qD
Lem;qS
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(222,"cNeg x A[IsComparisonOpcode(A)]","[-x] {OppositeComparisonOpcode(A)}","    with A = "<<FP_TRACE_OPCODENAME(A)<<", x = "<<x<<"\n");qD
Len;}
}
}
if(A>=VarBegin&&data->ByteCode.size()>0){B=qR;if(B==A){FP_TRACE_BYTECODE_OPTIMIZATION(356,"B[B==A] A[A>=VarBegin&&data->ByteCode.size()>0]","B cDup","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Leo;}
}
if(q11(A)){B=qR;if(B>=VarBegin&&q3
size()>1){C=q1-1];if(C==A){D=q1-2];if(D==B){FP_TRACE_BYTECODE_OPTIMIZATION(357,"D[D==B] C[C==A] B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]","D C cDup","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<", C = "<<FP_TRACE_OPCODENAME(C)<<", D = "<<FP_TRACE_OPCODENAME(D)<<"\n");qD
Lep;}
}
}
}
}
qD
Leq;Laa:ByteCodePtr-=1;q3
pop_back();qD
TailCall_cAbs;Lab:ImmedPtr[0]=fp_abs(x);return;Lac:data->Immed.push_back(Value_t(4));q1-2]=qJ;ByteCodePtr-=2;for(q81
tmp=2;tmp-->0;)q3
qL
Lfa:opcode=cMul;Lfb:FP_ReDefinePointers();Lfc:qD
TailCall_cMul;Lad:for(q81
tmp=4;tmp-->0;)q3
qL
AddFunctionOpcode(cMul);data->Immed.push_back(Value_t(4));q3
push_back(qJ);qD
Lfa;Lae:ImmedPtr[0]=x+Value_t(1);q1-2]=qJ;ByteCodePtr-=2;q9
Lfd:opcode=cMul;qD
Lfc;Laf:ByteCodePtr-=1;q3
qL
opcode=cSub;Lfe:qD
TailCall_cSub;Lag
qQ-1]=y+x;data->Immed.qL
q9
Lff:FP_ReDefinePointers();Lfg:qD
TailCall_cAdd;Lah
qJ1
y+x;Lai:qP
qL
Lbd:q3
qL
return;Laj
qJ1
fp_and(x,y);qD
Lai;Lak:qP
push_back(Value_t(0));qR=qJ;AddFunctionOpcode(cMul);qP
push_back(Value_t(1));Lfh:q3
push_back(qJ);Lfi:opcode=cAdd;qD
Lff;Lal
qQ
0]=-x;q1-1]=qJ;ByteCodePtr-=1;q3
qL
Lfj:qD
TailCall_cDiv;Lam
qJ1
y/x;qD
Lai;Lan
qJ1
fp_equal(y,x);qD
Lai;Lao:q1-1]=qJ;ByteCodePtr-=1;q3
qL
Lfk:qD
TailCall_cEqual;Lap:qP
qL
q3
qL
Lfl:opcode=cNot;FP_ReDefinePointers();Lfm:qD
TailCall_cNot;Laq
qK
q9
Lfn:opcode=cNotNot;FP_ReDefinePointers();Lfo:qD
TailCall_cNotNot;Lba
qK
q3
qL
qD
Lfn;Lbb
qJ1
fp_less(x,y);qD
Lai;Lbc
qJ1
fp_lessOrEq
q31
Lbe:q3
qL
AddFunctionOpcode(cRSqrt);return;Lbf
qQ
0]=Value_t(1)/x;return;Lbg
qK
q9
qD
Lfl;Lbh
qJ1
fp_less(y,x);qD
Lai;Lbi
qJ1
fp_lessOrEq
q21
Lbj
qJ1
fp_max
q31
Lbk
qJ1
fp_min
q31
Lbl
qJ1
fp_mod
q21
Lbm
qJ1
x;qP
qL
for(q81
tmp=3;tmp-->0;)q3
qL
AddFunctionOpcode(A);Lfp:qM(cMul);qP
push_back(y*x);qD
Lfh;Lbn:ByteCodePtr-=1;q3
qL
opcode=cSqr;Lfq:qD
TailCall_cSqr;Lbo:ByteCodePtr-=1;q3
qL
opcode=cDiv;qD
Lfj;Lbp:for(q81
tmp=3;tmp-->0;)q3
qL
qM(cSqr);qM(cMul);Lga:opcode=cNeg;FP_ReDefinePointers();qD
TailCall_cNeg;Lbq:q9
qM(cSqr);qD
Lga;Lca
qQ
0]=x+Value_t(1);q1-2]=qJ;q9
qM(cPow);return;Lcb:q1-2]=qJ;ByteCodePtr-=2;q9
qD
Lfc;Lcc
qJ1
x;Lgb
qK
q9
qD
Lfb;Lcd:for(q81
tmp=2;tmp-->0;)qP
qL
Lgc:for(q81
tmp=4;tmp-->0;)q3
qL
Lgd:qM(A);qP
push_back(x);Lge:q3
push_back(qJ);qD
Lfb;Lce
qK
qD
Lgc;Lcf
qK
for(q81
tmp=4;tmp-->0;)q3
qL
qM(B);qD
Lgd;Lcg
qK
for(q81
tmp=3;tmp-->0;)q3
qL
qD
Lgd;Lch:q1-1]=qJ
qY
Lci:q1-1]=qJ;ByteCodePtr-=1;q3
qL
qD
Lfc;Lcj
qQ
0]=x+x;qD
Lcb;Lck
qJ1
x;q1-4]=qJ;qP
qL
for(q81
tmp=4;tmp-->0;)q3
qL
qM(cMul);qM(A);qP
push_back(y*x);q3
push_back(qJ)qI
qD
Lfi;Lcl
qJ1
x;qP
qL
q9
qD
Lfp;Lcm
qK
for(q81
tmp=4;tmp-->0;)q3
pop_back()qI
qP
push_back(x+x);qD
Lge;Lcn:for(q81
tmp=2;tmp-->0;)qC1(q81
tmp=3;tmp-->0;)q3
qL
return;Lco
qJ1
y*x;qD
Lgb;Lcp
qQ
0]=-x;qD
Lci;Lcq
qJ1
x;q1-4]=qJ;qC1(q81
tmp=4;tmp-->0;)q3
pop_back()qI
qM(A);qP
push_back(y*x);q3
push_back(qJ)qI
opcode=cSub;FP_ReDefinePointers();qD
Lfe;Lda
qJ1
y*x;qD
Lai;Ldb:qR=cDup;ImmedPtr-=1;qP
qL
Lgf:opcode=cAdd;qD
Lfg;Ldc
qK
q3
qL
qD
Lga;Ldd:q9
Lgg:qM(cSqr);qD
Lfb;Lde:for(q81
tmp=3;tmp-->0;)q3
qL
qD
Lgg;Ldf
qJ1
fp_nequal
q21
Ldg:q1-1]=qJ;ByteCodePtr-=1;q3
qL
Lgh:qD
TailCall_cNEqual;Ldh
qQ
0]=-x;ByteCodePtr-=1;q3
qL
qD
Lfd;Ldi
qQ
0]=-x;return;Ldj:ByteCodePtr-=1;q3
qL
qD
Lfm;Ldk:q3
qL
qM(cAbsNotNot);return;Ldl:q3
qL
Lee:qM(cAbsNot);return;Ldm
qQ
0]=x;Leb:ByteCodePtr-=1;q3
qL
opcode=cEqual;qD
Lfk;Ldn:ByteCodePtr-=1;q3
qL
opcode=cNEqual;qD
Lgh;Ldo:ByteCodePtr-=1;q3
qL
opcode=cLessOrEq;qD
TailCall_cLessOrEq;Ldp:ByteCodePtr-=1;q3
qL
opcode=cLess;qD
TailCall_cLess;Ldq:ByteCodePtr-=1;q3
qL
opcode=cGreaterOrEq;qD
TailCall_cGreaterOrEq;Lea:ByteCodePtr-=1;q3
qL
opcode=cGreater;qD
TailCall_cGreater;Lec:ByteCodePtr-=1;q3
qL
opcode=cNotNot;qD
Lfo;Led
qQ
0]=fp_not(x);return;Lef
qQ
0]=x;qD
Ldn;Leg
qJ1
fp_or
q31
Leh:ByteCodePtr-=1;q3
qL
qD
Lfq;Lei:qP
push_back(Value_t(0));qR=qJ;qD
Lfa;Lej:ByteCodePtr-=1;q3
qL
qD
Lgf;Lek
qJ1
y-x;qD
Lai;Lel
qQ
0]=-x;qD
Lgf;Lem
qJ1
x-y;qP
qL
q9
qM(A);return;Len
qQ
0]=-x;q1-1]=qJ;q3
qL
qM(OppositeComparisonOpcode(A));return;Leo:q3
push_back(cDup);return;Lep:qR=cDup;return;Leq:q3
push_back(opcode);return;return;qD
TailCall_cAnd;qD
TailCall_cInv;qD
TailCall_cMax;qD
TailCall_cMin;qD
TailCall_cMod;qD
TailCall_cNeg;qD
TailCall_cNotNot;qD
TailCall_cOr;qD
TailCall_cSub;
#endif


#if(FP_FLOAT_VERSION)

Value_t
x;q81
A
qE1
y;q81
B;q81
C;q81
D;switch(opcode){TailCall_cAbs:case
cAbs
qK1(qR){case
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(197,"cNeg cAbs","cAbs","");qD
Laa;case
qV
FP_TRACE_BYTECODE_OPTIMIZATION(40,"x cAbs","[fp_abs(x)]","    with x = "<<x<<"\n");qD
Lab;default:A=qR;q7
FP_TRACE_BYTECODE_OPTIMIZATION(258,"A[IsNeverNegativeValueOpcode(A)] cAbs","A","    with A = "<<FP_TRACE_OPCODENAME(A)<<"\n");return;qG
TailCall_cAcos:case
cAcos:if(qR==qJ){x
qF
0];if(x>=Value_t(-1)&&x<=Value_t(1)){FP_TRACE_BYTECODE_OPTIMIZATION(44,"x[x>=Value_t(-1)&&x<=Value_t(1)] cAcos","[fp_acos(x)]","    with x = "<<x<<"\n");qD
Lac;qG
TailCall_cAcosh:case
cAcosh:if(qR==qJ){x
qF
0];if(x>=Value_t(1)){FP_TRACE_BYTECODE_OPTIMIZATION(41,"x[x>=Value_t(1)] cAcosh","[fp_acosh(x)]","    with x = "<<x<<"\n");qD
Lad;qG
TailCall_cAdd:case
cAdd
qK1(qR)qW
switch(q1-1]){case
cAdd:if(qF1
cDup){FP_TRACE_BYTECODE_OPTIMIZATION(147,"cDup cAdd cDup cAdd","[Value_t(4)] cMul","");qD
Lae;qS
cMul:if(qF1
cAdd){if(qM1
cDup){FP_TRACE_BYTECODE_OPTIMIZATION(148,"cDup cAdd cMul cDup cAdd","cMul [Value_t(4)] cMul","");qD
Laf;}
}
break;qS
cMul:if(q1-1]==qJ){x
qF
0];if(qF1
cDup){FP_TRACE_BYTECODE_OPTIMIZATION(150,"cDup x cMul cAdd","[x+Value_t(1)] cMul","    with x = "<<x<<"\n");qD
Lag;}
qS
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(204,"cNeg cAdd","cSub","");qD
Lah;case
qV
switch(q1-1]){case
cAdd:if(qF1
qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(141,"y cAdd x cAdd","[y+x] cAdd","    with x = "<<x<<", y = "<<y<<"\n");qD
Lai;qN1
FP_TRACE_BYTECODE_OPTIMIZATION(76,"y x cAdd","[y+x]","    with x = "<<x<<", y = "<<y<<"\n");qD
Laj;}
if(q41(0)){FP_TRACE_BYTECODE_OPTIMIZATION(75,"x[x==Value_t(0)] cAdd","","    with x = "<<x<<"\n");qD
Lak;}
break;}
qD
qH1;TailCall_cAnd:case
cAnd:if(qR==qJ){x
qF
0];if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(130,"y x cAnd","[fp_and(x,y)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Lal;qG
TailCall_cAsin:case
cAsin:if(qR==qJ){x
qF
0];if(x>=Value_t(-1)&&x<=Value_t(1)){FP_TRACE_BYTECODE_OPTIMIZATION(45,"x[x>=Value_t(-1)&&x<=Value_t(1)] cAsin","[fp_asin(x)]","    with x = "<<x<<"\n");qD
Lam;qG
TailCall_cAsinh:case
cAsinh:if(qR==qJ){x
qF
0];FP_TRACE_BYTECODE_OPTIMIZATION(42,"x cAsinh","[fp_asinh(x)]","    with x = "<<x<<"\n");qD
Lan;}
qD
qH1;TailCall_cAtan:case
cAtan:if(qR==qJ){x
qF
0];FP_TRACE_BYTECODE_OPTIMIZATION(46,"x cAtan","[fp_atan(x)]","    with x = "<<x<<"\n");qD
Lao;}
qD
qH1;TailCall_cAtan2:case
cAtan2:if(qR==qJ){x
qF
0];if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(83,"y x cAtan2","[fp_atan2(y,x)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Lap;qG
TailCall_cAtanh:case
cAtanh:if(qR==qJ){x
qF
0];if(x>Value_t(-1)&&x<Value_t(1)){FP_TRACE_BYTECODE_OPTIMIZATION(43,"x[x>Value_t(-1)&&x<Value_t(1)] cAtanh","[fp_atanh(x)]","    with x = "<<x<<"\n");qD
Laq;qG
TailCall_cCbrt:case
cCbrt:if(qR==qJ){x
qF
0];FP_TRACE_BYTECODE_OPTIMIZATION(47,"x cCbrt","[fp_cbrt(x)]","    with x = "<<x<<"\n");qD
Lba;}
qD
qH1;TailCall_cCeil:case
cCeil
qK1(qR){case
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(267,"cNeg cCeil","cFloor cNeg","");qD
Lbb;case
qV
FP_TRACE_BYTECODE_OPTIMIZATION(48,"x cCeil","[fp_ceil(x)]","    with x = "<<x<<"\n");qD
Lbc;default:A=qR;if(IsAlwaysIntegerOpcode(A)){FP_TRACE_BYTECODE_OPTIMIZATION(261,"A[IsAlwaysIntegerOpcode(A)] cCeil","A","    with A = "<<FP_TRACE_OPCODENAME(A)<<"\n");return;qG
TailCall_cCos:case
cCos
qK1(qR){case
cAbs:FP_TRACE_BYTECODE_OPTIMIZATION(302,"cAbs cCos","cCos","")qY
case
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(304,"cNeg cCos","cCos","");qD
Lbd
qE
FP_TRACE_BYTECODE_OPTIMIZATION(49,"x cCos","[fp_cos(x)]","    with x = "<<x<<"\n");qD
Lbe;}
qD
qH1;TailCall_cCosh:case
cCosh
qK1(qR){case
cAbs:FP_TRACE_BYTECODE_OPTIMIZATION(303,"cAbs cCosh","cCosh","");qD
Lbf;case
cAcosh:FP_TRACE_BYTECODE_OPTIMIZATION(193,"cAcosh cCosh","","");qD
Lbg;case
cAsinh:FP_TRACE_BYTECODE_OPTIMIZATION(316,"cAsinh cCosh","[DO_STACKPLUS1] cSqr [Value_t(1)] cAdd cSqrt","");incStackPtr();--StackPtr;qD
Lbh;case
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(305,"cNeg cCosh","cCosh","");qD
Lbf
qE
FP_TRACE_BYTECODE_OPTIMIZATION(50,"x cCosh","[fp_cosh(x)]","    with x = "<<x<<"\n");qD
Lbi;}
qD
qH1;TailCall_cDeg:case
cDeg:if(qR==qJ){x
qF
0];FP_TRACE_BYTECODE_OPTIMIZATION(65,"x cDeg","[RadiansToDegrees(x)]","    with x = "<<x<<"\n");qD
Lbj;}
qD
qH1;TailCall_cDiv:case
cDiv
qK1(qR){case
cCos:FP_TRACE_BYTECODE_OPTIMIZATION(319,"cCos cDiv","cSec cMul","");qD
Lbk;case
cCot:FP_TRACE_BYTECODE_OPTIMIZATION(323,"cCot cDiv","cTan cMul","");qD
Lbl;case
cCsc:FP_TRACE_BYTECODE_OPTIMIZATION(321,"cCsc cDiv","cSin cMul","");qD
Lbm;case
cDup:FP_TRACE_BYTECODE_OPTIMIZATION(120,"cDup cDiv","[Value_t(0)] cMul [Value_t(1)] cAdd","");qD
Lbn;case
cExp:FP_TRACE_BYTECODE_OPTIMIZATION(273,"cExp cDiv","cNeg cExp cMul","");qD
Lbo;case
cExp2:FP_TRACE_BYTECODE_OPTIMIZATION(274,"cExp2 cDiv","cNeg cExp2 cMul","");qD
Lbp;case
cInv:FP_TRACE_BYTECODE_OPTIMIZATION(202,"cInv cDiv","cMul","");qD
Lbq;case
cPow:FP_TRACE_BYTECODE_OPTIMIZATION(272,"cPow cDiv","cNeg cPow cMul","");qD
Lca;case
cSec:FP_TRACE_BYTECODE_OPTIMIZATION(322,"cSec cDiv","cCos cMul","");qD
Lcb;case
cSin:FP_TRACE_BYTECODE_OPTIMIZATION(318,"cSin cDiv","cCsc cMul","");qD
Lcc;case
cTan:FP_TRACE_BYTECODE_OPTIMIZATION(320,"cTan cDiv","cCot cMul","");qD
Lcd
qE
if(q41(1)){FP_TRACE_BYTECODE_OPTIMIZATION(72,"x[x==Value_t(1)] cDiv","","    with x = "<<x<<"\n");qD
Lak;}
if(x!=Value_t(0))qL1(q1-1]){case
cMul:if(qF1
qJ){qA1
if((y/x)==GetRadiansToDegreesFactor<Value_t>()){FP_TRACE_BYTECODE_OPTIMIZATION(159,"y[(y/x)==GetRadiansToDegreesFactor<Value_t>()] cMul x[x!=Value_t(0)] cDiv","cDeg","    with x = "<<x<<", y = "<<y<<"\n");qD
Lce;}
if((y/x)==GetDegreesToRadiansFactor<Value_t>()){FP_TRACE_BYTECODE_OPTIMIZATION(160,"y[(y/x)==GetDegreesToRadiansFactor<Value_t>()] cMul x[x!=Value_t(0)] cDiv","cRad","    with x = "<<x<<", y = "<<y<<"\n");qD
Lcf;}
FP_TRACE_BYTECODE_OPTIMIZATION(161,"y cMul x[x!=Value_t(0)] cDiv","[y/x] cMul","    with x = "<<x<<", y = "<<y<<"\n");qD
Lcg;qS
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(138,"cNeg x[x!=Value_t(0)] cDiv","[-x] cDiv","    with x = "<<x<<"\n");qD
Lch;case
qJ:qA1
FP_TRACE_BYTECODE_OPTIMIZATION(73,"y x[x!=Value_t(0)] cDiv","[y/x]","    with x = "<<x<<", y = "<<y<<"\n");qD
Lci;}
FP_TRACE_BYTECODE_OPTIMIZATION(163,"x[x!=Value_t(0)] cDiv","[Value_t(1)/x] cMul","    with x = "<<x<<"\n");qD
Lcj;}
break;}
qD
qH1;TailCall_cEqual:case
cEqual:if(qR==qJ){x
qF
0];if(q41(0))q51
FP_TRACE_BYTECODE_OPTIMIZATION(216,"cAbs x[x==Value_t(0)] cEqual","[x] cEqual","    with x = "<<x<<"\n");qD
Lck;case
cSqr:FP_TRACE_BYTECODE_OPTIMIZATION(218,"cSqr x[x==Value_t(0)] cEqual","[x] cEqual","    with x = "<<x<<"\n");qD
Lck;}
}
if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(128,"y x cEqual","[fp_equal(y,x)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Lcl;qG
TailCall_cExp:case
cExp
qK1(qR){case
cAdd:if(q1-1]==qJ){x
qF
0];FP_TRACE_BYTECODE_OPTIMIZATION(269,"x cAdd cExp","cExp [fp_exp(x)] cMul","    with x = "<<x<<"\n");qD
Lcm;qS
cLog:A=q1-1];q7
FP_TRACE_BYTECODE_OPTIMIZATION(183,"A[IsNeverNegativeValueOpcode(A)] cLog cExp","A","    with A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lbg;}
break
qE
FP_TRACE_BYTECODE_OPTIMIZATION(51,"x cExp","[fp_exp(x)]","    with x = "<<x<<"\n");qD
Lcn;}
qD
qH1;TailCall_cExp2:case
cExp2
qK1(qR){case
cAdd:if(q1-1]==qJ){x
qF
0];FP_TRACE_BYTECODE_OPTIMIZATION(270,"x cAdd cExp2","cExp2 [fp_exp2(x)] cMul","    with x = "<<x<<"\n");qD
Lco;qS
cLog2:A=q1-1];q7
FP_TRACE_BYTECODE_OPTIMIZATION(184,"A[IsNeverNegativeValueOpcode(A)] cLog2 cExp2","A","    with A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lbg;}
break
qE
FP_TRACE_BYTECODE_OPTIMIZATION(52,"x cExp2","[fp_exp2(x)]","    with x = "<<x<<"\n");qD
Lcp;}
FP_TRACE_BYTECODE_OPTIMIZATION(362,"cExp2","[DO_STACKPLUS1] [fp_log(Value_t(2))] cMul cExp","");incStackPtr();--StackPtr;qD
Lcq;TailCall_cFloor:case
cFloor
qK1(qR){case
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(266,"cNeg cFloor","cCeil cNeg","");qD
Lda
qE
FP_TRACE_BYTECODE_OPTIMIZATION(53,"x cFloor","[fp_floor(x)]","    with x = "<<x<<"\n");qD
Ldb;default:A=qR;if(IsAlwaysIntegerOpcode(A)){FP_TRACE_BYTECODE_OPTIMIZATION(260,"A[IsAlwaysIntegerOpcode(A)] cFloor","A","    with A = "<<FP_TRACE_OPCODENAME(A)<<"\n");return;qG
TailCall_cGreater:case
cGreater:if(qR==qJ){x
qF
0];if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(126,"y x cGreater","[fp_less(x,y)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Ldc;}
if(q41(-0.5)){FP_TRACE_BYTECODE_OPTIMIZATION(296,"x[x==Value_t(-0.5)] cGreater","cNeg cAbsNot","    with x = "<<x<<"\n");qD
Ldd;qG
TailCall_cGreaterOrEq:case
cGreaterOrEq:if(qR==qJ){x
qF
0];if(x!=Value_t(0)){if(q1-1]==cAbs){FP_TRACE_BYTECODE_OPTIMIZATION(292,"cAbs x[x!=Value_t(0)] cGreaterOrEq","[Value_t(0.5)/x] cMul cNotNot","    with x = "<<x<<"\n");qD
Lde;}
}
if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(127,"y x cGreaterOrEq","[fp_lessOrEq(x,y)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Ldf;}
if(q41(0.5)){FP_TRACE_BYTECODE_OPTIMIZATION(295,"x[x==Value_t(0.5)] cGreaterOrEq","cAbsNotNot","    with x = "<<x<<"\n");qD
Ldg;qG
TailCall_cInt:case
cInt
qK1(qR){case
qV
FP_TRACE_BYTECODE_OPTIMIZATION(54,"x cInt","[fp_int(x)]","    with x = "<<x<<"\n");qD
Ldh;default:A=qR;if(IsAlwaysIntegerOpcode(A)){FP_TRACE_BYTECODE_OPTIMIZATION(262,"A[IsAlwaysIntegerOpcode(A)] cInt","A","    with A = "<<FP_TRACE_OPCODENAME(A)<<"\n");return;qG
TailCall_cInv:case
cInv
qK1(qR){case
cCos:FP_TRACE_BYTECODE_OPTIMIZATION(325,"cCos cInv","cSec","");qD
Ldi;case
cCot:FP_TRACE_BYTECODE_OPTIMIZATION(329,"cCot cInv","cTan","");qD
Ldj;case
cCsc:FP_TRACE_BYTECODE_OPTIMIZATION(327,"cCsc cInv","cSin","");qD
Ldk;case
cInv:FP_TRACE_BYTECODE_OPTIMIZATION(198,"cInv cInv","","");qD
Lbg;case
cPow:FP_TRACE_BYTECODE_OPTIMIZATION(200,"cPow cInv","cNeg cPow","");qD
Ldl;case
cSec:FP_TRACE_BYTECODE_OPTIMIZATION(328,"cSec cInv","cCos","");qD
Ldm;case
cSin:FP_TRACE_BYTECODE_OPTIMIZATION(324,"cSin cInv","cCsc","");qD
Ldn;case
cSqrt:FP_TRACE_BYTECODE_OPTIMIZATION(67,"cSqrt cInv","cRSqrt","");qD
Ldo;case
cTan:FP_TRACE_BYTECODE_OPTIMIZATION(326,"cTan cInv","cCot","");qD
Ldp
qE
if(x!=Value_t(0)){FP_TRACE_BYTECODE_OPTIMIZATION(69,"x[x!=Value_t(0)] cInv","[Value_t(1)/x]","    with x = "<<x<<"\n");qD
Ldq;}
break;}
qD
qH1;TailCall_cLess:case
cLess:if(qR==qJ){x
qF
0];if(x!=Value_t(0)){if(q1-1]==cAbs){FP_TRACE_BYTECODE_OPTIMIZATION(291,"cAbs x[x!=Value_t(0)] cLess","[Value_t(0.5)/x] cMul cNot","    with x = "<<x<<"\n");qD
Lea;}
}
if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(124,"y x cLess","[fp_less(y,x)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Leb;}
if(q41(0.5)){FP_TRACE_BYTECODE_OPTIMIZATION(294,"x[x==Value_t(0.5)] cLess","cAbsNot","    with x = "<<x<<"\n");qD
Lec;qG
TailCall_cLessOrEq:case
cLessOrEq:if(qR==qJ){x
qF
0];if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(125,"y x cLessOrEq","[fp_lessOrEq(y,x)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Led;}
if(q41(-0.5)){FP_TRACE_BYTECODE_OPTIMIZATION(297,"x[x==Value_t(-0.5)] cLessOrEq","cNeg cAbsNotNot","    with x = "<<x<<"\n");qD
Lee;qG
TailCall_cLog:case
cLog
qK1(qR){case
cExp:FP_TRACE_BYTECODE_OPTIMIZATION(186,"cExp cLog","","");qD
Lbg;case
cMul:if(q1-1]==qJ){x
qF
0];if(x>Value_t(0)){FP_TRACE_BYTECODE_OPTIMIZATION(374,"x[x>Value_t(0)] cMul cLog","cLog [fp_log(x)] cAdd","    with x = "<<x<<"\n");qD
Lef;}
qS
cSqr:FP_TRACE_BYTECODE_OPTIMIZATION(103,"cSqr cLog","cAbs cLog cDup cAdd","");qD
Leg;case
qJ:q6
FP_TRACE_BYTECODE_OPTIMIZATION(55,"x[x>Value_t(0)] cLog","[fp_log(x)]","    with x = "<<x<<"\n");qD
Leh;}
break;}
qD
qH1;TailCall_cLog10:case
cLog10
qK1(qR){case
cExp:FP_TRACE_BYTECODE_OPTIMIZATION(364,"cExp cLog10","[DO_STACKPLUS1] [fp_log10(fp_const_e<Value_t>())] cMul","");incStackPtr();--StackPtr;qD
Lei;case
cMul:if(q1-1]==qJ){q6
FP_TRACE_BYTECODE_OPTIMIZATION(375,"x[x>Value_t(0)] cMul cLog10","cLog10 [fp_log10(x)] cAdd","    with x = "<<x<<"\n");qD
Lej
q91
FP_TRACE_BYTECODE_OPTIMIZATION(105,"cSqr cLog10","cAbs cLog10 cDup cAdd","");qD
Lek
q61
FP_TRACE_BYTECODE_OPTIMIZATION(56,"x[x>Value_t(0)] cLog10","[fp_log10(x)]","    with x = "<<x<<"\n");qD
Lel
qT
TailCall_cLog2:case
cLog2
qK1(qR){case
cExp:FP_TRACE_BYTECODE_OPTIMIZATION(363,"cExp cLog2","[DO_STACKPLUS1] [fp_log2(fp_const_e<Value_t>())] cMul","");incStackPtr();--StackPtr;qD
Lem;case
cExp2:FP_TRACE_BYTECODE_OPTIMIZATION(187,"cExp2 cLog2","","");qD
Lbg;case
cMul:if(q1-1]==qJ){q6
FP_TRACE_BYTECODE_OPTIMIZATION(373,"x[x>Value_t(0)] cMul cLog2","cLog2 [fp_log2(x)] cAdd","    with x = "<<x<<"\n");qD
Len
q91
FP_TRACE_BYTECODE_OPTIMIZATION(104,"cSqr cLog2","cAbs cLog2 cDup cAdd","");qD
Leo
q61
FP_TRACE_BYTECODE_OPTIMIZATION(57,"x[x>Value_t(0)] cLog2","[fp_log2(x)]","    with x = "<<x<<"\n");qD
Lep
qT
TailCall_cMax:case
cMax
qK1(qR)qW
FP_TRACE_BYTECODE_OPTIMIZATION(110,"cDup cMax","","");qD
Lbg
qE
if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(82,"y x cMax","[fp_max(x,y)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Leq;}
break;default:A=qR;if(A>=VarBegin)qL1(q1-1])qW
q01
FP_TRACE_BYTECODE_OPTIMIZATION(112,"B[B==A] cDup A[A>=VarBegin] cMax","B cDup","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lbg;qS
cMax:q01
FP_TRACE_BYTECODE_OPTIMIZATION(114,"B[B==A] cMax A[A>=VarBegin] cMax","B cMax","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lbg
qB1}
qG
TailCall_cMin:case
cMin
qK1(qR)qW
FP_TRACE_BYTECODE_OPTIMIZATION(109,"cDup cMin","","");qD
Lbg
qE
if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(81,"y x cMin","[fp_min(x,y)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Lfa
qB1
default:A=qR;if(A>=VarBegin)qL1(q1-1])qW
q01
FP_TRACE_BYTECODE_OPTIMIZATION(111,"B[B==A] cDup A[A>=VarBegin] cMin","B cDup","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lbg;qS
cMin:q01
FP_TRACE_BYTECODE_OPTIMIZATION(113,"B[B==A] cMin A[A>=VarBegin] cMin","B cMin","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lbg
qB1}
qG
TailCall_cMod:case
cMod:if(qR==qJ){x
qF
0];if(x!=Value_t(0)){if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(74,"y x[x!=Value_t(0)] cMod","[fp_mod(y,x)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Lfb;}
qG
TailCall_cMul:case
cMul
qK1(qR){case
cAdd:if(q1-1]==qJ){y
qF
0];A=q1-2];if(A>=VarBegin){if(qM1
qJ){x
qF-1];FP_TRACE_BYTECODE_OPTIMIZATION(245,"x A[A>=VarBegin] y cAdd cMul","[x] A cMul [y*x] cAdd","    with y = "<<y<<", A = "<<FP_TRACE_OPCODENAME(A)<<", x = "<<x<<"\n");qD
Lfc;}
}
qO
cDup:FP_TRACE_BYTECODE_OPTIMIZATION(359,"cDup cMul","cSqr","");qD
Lfd;case
cInv:FP_TRACE_BYTECODE_OPTIMIZATION(203,"cInv cMul","cDiv","");qD
Lfe;case
cNeg:A=q1-1];if(A>=VarBegin)qL1(q1-2]){case
cMul:B=q1-3];if(B==A){FP_TRACE_BYTECODE_OPTIMIZATION(351,"B[B==A] cMul A[A>=VarBegin] cNeg cMul","B cSqr cMul cNeg","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lff;}
qD
Default2;default:Default2:;q01
FP_TRACE_BYTECODE_OPTIMIZATION(345,"B[B==A] A[A>=VarBegin] cNeg cMul","B cSqr cNeg","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lfg;}
}
qO
cPow:if(q1-1]==qJ){x
qF
0];if(qF1
cDup){FP_TRACE_BYTECODE_OPTIMIZATION(151,"cDup x cPow cMul","[x+Value_t(1)] cPow","    with x = "<<x<<"\n");qD
Lfh;}
}
qD
Default1
qE
if(q41(0))qL1(q1-1]){case
cMul:A=q1-2];if(A>=VarBegin){FP_TRACE_BYTECODE_OPTIMIZATION(94,"A[A>=VarBegin] cMul x[x==Value_t(0)] cMul","[x] cMul","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lfi;}
qD
Default3;default:Default3:;A=q1-1];if(IsBinaryOpcode(A)qB(A))qL1(q1-2]){case
qJ:qA1
FP_TRACE_BYTECODE_OPTIMIZATION(93,"y A[IsBinaryOpcode(A)&&!HasInvalidRangesOpcode(A)] x[x==Value_t(0)] cMul","[x] cMul","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<", y = "<<y<<"\n");qD
Lfj;default:B=q1-2];if(IsBinaryOpcode(B)qB(B))qL1(q1-3]){case
qJ:qA1
FP_TRACE_BYTECODE_OPTIMIZATION(97,"y B[IsBinaryOpcode(B)&&!HasInvalidRangesOpcode(B)] A[IsBinaryOpcode(A)&&!HasInvalidRangesOpcode(A)] x[x==Value_t(0)] cMul","A [x] cMul","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<", y = "<<y<<"\n");qD
Lfk;default:C=q1-3];if(C>=VarBegin){FP_TRACE_BYTECODE_OPTIMIZATION(95,"C[C>=VarBegin] B[IsBinaryOpcode(B)&&!HasInvalidRangesOpcode(B)] A[IsBinaryOpcode(A)&&!HasInvalidRangesOpcode(A)] x[x==Value_t(0)] cMul","A [x] cMul","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<", C = "<<FP_TRACE_OPCODENAME(C)<<"\n");qD
Lfl;}
if(q11(C)qB(C)){FP_TRACE_BYTECODE_OPTIMIZATION(96,"C[IsUnaryOpcode(C)&&!HasInvalidRangesOpcode(C)] B[IsBinaryOpcode(B)&&!HasInvalidRangesOpcode(B)] A[IsBinaryOpcode(A)&&!HasInvalidRangesOpcode(A)] x[x==Value_t(0)] cMul","B A [x] cMul","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<", C = "<<FP_TRACE_OPCODENAME(C)<<"\n");qD
Lfm;}
}
}
if(B>=VarBegin){FP_TRACE_BYTECODE_OPTIMIZATION(91,"B[B>=VarBegin] A[IsBinaryOpcode(A)&&!HasInvalidRangesOpcode(A)] x[x==Value_t(0)] cMul","[x] cMul","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lfi;}
if(q11(B)qB(B)){FP_TRACE_BYTECODE_OPTIMIZATION(92,"B[IsUnaryOpcode(B)&&!HasInvalidRangesOpcode(B)] A[IsBinaryOpcode(A)&&!HasInvalidRangesOpcode(A)] x[x==Value_t(0)] cMul","A [x] cMul","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lfn;}
}
}
if(A>=VarBegin){FP_TRACE_BYTECODE_OPTIMIZATION(89,"A[A>=VarBegin] x[x==Value_t(0)] cMul","[x]","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lfo;}
if(q11(A)qB(A)){FP_TRACE_BYTECODE_OPTIMIZATION(90,"A[IsUnaryOpcode(A)&&!HasInvalidRangesOpcode(A)] x[x==Value_t(0)] cMul","[x] cMul","    with x = "<<x<<", A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lfp;}
}
}
switch(q1-1]){case
cAdd
qK1(q1-2])qW
if(x+q41(1)){FP_TRACE_BYTECODE_OPTIMIZATION(154,"cDup[x+x==Value_t(1)] cAdd x cMul","","    with x = "<<x<<"\n");qD
Lfq;}
FP_TRACE_BYTECODE_OPTIMIZATION(155,"cDup cAdd x cMul","[x+x] cMul","    with x = "<<x<<"\n");qD
Lga;case
cMul:if(qM1
qJ){qA1
A=q1-4];if(A>=VarBegin){FP_TRACE_BYTECODE_OPTIMIZATION(243,"A[A>=VarBegin] y cMul cAdd x cMul","[x] cMul A [y*x] cMul cAdd","    with x = "<<x<<", y = "<<y<<", A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lgb;}
qN1
FP_TRACE_BYTECODE_OPTIMIZATION(242,"y cAdd x cMul","[x] cMul [y*x] cAdd","    with x = "<<x<<", y = "<<y<<"\n");qD
Lgc;qS
cDeg:FP_TRACE_BYTECODE_OPTIMIZATION(106,"cDeg x cMul","[RadiansToDegrees(x)] cMul","    with x = "<<x<<"\n");qD
Lgd;case
cMul
qK1(q1-2]){case
cAdd:if(qM1
cDup){if(x+q41(1)){FP_TRACE_BYTECODE_OPTIMIZATION(156,"cDup[x+x==Value_t(1)] cAdd cMul x cMul","cMul","    with x = "<<x<<"\n");qD
Lge;}
FP_TRACE_BYTECODE_OPTIMIZATION(157,"cDup cAdd cMul x cMul","cMul [x+x] cMul","    with x = "<<x<<"\n");qD
Lgf;qN1
if((y*x)==GetRadiansToDegreesFactor<Value_t>()){FP_TRACE_BYTECODE_OPTIMIZATION(143,"y[(y*x)==GetRadiansToDegreesFactor<Value_t>()] cMul x cMul","cDeg","    with x = "<<x<<", y = "<<y<<"\n");qD
Lce;}
if((y*x)==GetDegreesToRadiansFactor<Value_t>()){FP_TRACE_BYTECODE_OPTIMIZATION(144,"y[(y*x)==GetDegreesToRadiansFactor<Value_t>()] cMul x cMul","cRad","    with x = "<<x<<", y = "<<y<<"\n");qD
Lcf;}
if(y*q41(1)){FP_TRACE_BYTECODE_OPTIMIZATION(145,"y[y*x==Value_t(1)] cMul x cMul","","    with x = "<<x<<", y = "<<y<<"\n");qD
Lgg;}
FP_TRACE_BYTECODE_OPTIMIZATION(146,"y cMul x cMul","[y*x] cMul","    with x = "<<x<<", y = "<<y<<"\n");qD
Lgh;qS
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(135,"cNeg x cMul","[-x] cMul","    with x = "<<x<<"\n");qD
Lgi;case
cRad:FP_TRACE_BYTECODE_OPTIMIZATION(107,"cRad x cMul","[DegreesToRadians(x)] cMul","    with x = "<<x<<"\n");qD
Lgj;case
cSub:if(qF1
cMul){if(qM1
qJ){qA1
A=q1-4];if(A>=VarBegin){FP_TRACE_BYTECODE_OPTIMIZATION(244,"A[A>=VarBegin] y cMul cSub x cMul","[x] cMul A [y*x] cMul cSub","    with x = "<<x<<", y = "<<y<<", A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lgk;}
}
qN1
FP_TRACE_BYTECODE_OPTIMIZATION(71,"y x cMul","[y*x]","    with x = "<<x<<", y = "<<y<<"\n");qD
Lgl;}
if(q41(1)){FP_TRACE_BYTECODE_OPTIMIZATION(70,"x[x==Value_t(1)] cMul","","    with x = "<<x<<"\n");qD
Lak;}
if(q41(2)){FP_TRACE_BYTECODE_OPTIMIZATION(100,"x[x==Value_t(2)] cMul","cDup cAdd","    with x = "<<x<<"\n");qD
Lgm;}
if(x==GetRadiansToDegreesFactor<Value_t>()){FP_TRACE_BYTECODE_OPTIMIZATION(101,"x[x==GetRadiansToDegreesFactor<Value_t>()] cMul","cDeg","    with x = "<<x<<"\n");qD
Lgn;}
if(x==GetDegreesToRadiansFactor<Value_t>()){FP_TRACE_BYTECODE_OPTIMIZATION(102,"x[x==GetDegreesToRadiansFactor<Value_t>()] cMul","cRad","    with x = "<<x<<"\n");qD
Lgo;}
if(q41(-1)){FP_TRACE_BYTECODE_OPTIMIZATION(137,"x[x==Value_t(-1)] cMul","cNeg","    with x = "<<x<<"\n");qD
Lgp
qB1
default:Default1:;A=qR;if(A>=VarBegin)qL1(q1-1]){case
cMul
qK1(q1-2]){case
cNeg:B=q1-3];if(B==A){FP_TRACE_BYTECODE_OPTIMIZATION(348,"B[B==A] cNeg cMul A[A>=VarBegin] cMul","B cSqr cMul cNeg","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lff;}
qD
Default4;default:Default4:;q01
FP_TRACE_BYTECODE_OPTIMIZATION(339,"B[B==A] cMul A[A>=VarBegin] cMul","B cSqr cMul","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lgq;}
}
qD
qG1;case
cNeg:q01
FP_TRACE_BYTECODE_OPTIMIZATION(342,"B[B==A] cNeg A[A>=VarBegin] cMul","B cSqr cNeg","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lfg;}
qD
qG1;default:qG1:;B=q1-1];if(B==A){FP_TRACE_BYTECODE_OPTIMIZATION(336,"B[B==A] A[A>=VarBegin] cMul","B cSqr","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lfd;}
}
}
if(q11(A)){B=q1-1];if(B>=VarBegin&&q3
size()>1){if(qF1
cMul){C=q1-3];if(C==A){D=q1-4];if(D==B){FP_TRACE_BYTECODE_OPTIMIZATION(358,"D[D==B] C[C==A] cMul B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)] cMul","D C cSqr cMul","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<", C = "<<FP_TRACE_OPCODENAME(C)<<", D = "<<FP_TRACE_OPCODENAME(D)<<"\n");qD
Lha;}
}
}
}
qG
TailCall_cNEqual:case
cNEqual:if(qR==qJ){x
qF
0];if(q41(0))q51
FP_TRACE_BYTECODE_OPTIMIZATION(217,"cAbs x[x==Value_t(0)] cNEqual","[x] cNEqual","    with x = "<<x<<"\n");qD
Lhb;case
cSqr:FP_TRACE_BYTECODE_OPTIMIZATION(219,"cSqr x[x==Value_t(0)] cNEqual","[x] cNEqual","    with x = "<<x<<"\n");qD
Lhb;}
}
if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(129,"y x cNEqual","[fp_nequal(y,x)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Lhc;qG
TailCall_cNeg:case
cNeg
qK1(qR){case
cMul:if(q1-1]==qJ){x
qF
0];FP_TRACE_BYTECODE_OPTIMIZATION(136,"x cMul cNeg","[-x] cMul","    with x = "<<x<<"\n");qD
Lhd;qS
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(199,"cNeg cNeg","","");qD
Lbg;case
cSin:if(q1-1]==cMul){if(qF1
qJ){x
qF
0];FP_TRACE_BYTECODE_OPTIMIZATION(310,"x cMul cSin cNeg","[-x] cMul cSin","    with x = "<<x<<"\n");qD
Lhe;}
qS
cSinh
q5
FP_TRACE_BYTECODE_OPTIMIZATION(311,"x cMul cSinh cNeg","[-x] cMul cSinh","    with x = "<<x<<"\n");qD
Lhf;}
qS
cTan
q5
FP_TRACE_BYTECODE_OPTIMIZATION(312,"x cMul cTan cNeg","[-x] cMul cTan","    with x = "<<x<<"\n");qD
Lhg;}
qS
cTanh
q5
FP_TRACE_BYTECODE_OPTIMIZATION(313,"x cMul cTanh cNeg","[-x] cMul cTanh","    with x = "<<x<<"\n");qD
Lhh;}
}
break
qE
FP_TRACE_BYTECODE_OPTIMIZATION(68,"x cNeg","[-x]","    with x = "<<x<<"\n");qD
Lhi;}
qD
qH1;TailCall_cNot:case
cNot
qK1(qR){case
cAbs:FP_TRACE_BYTECODE_OPTIMIZATION(249,"cAbs cNot","cNot","");qD
Lhj;case
cAbsNot:A=q1-1];if(IsLogicalOpcode(A)){FP_TRACE_BYTECODE_OPTIMIZATION(254,"A[IsLogicalOpcode(A)] cAbsNot cNot","A","    with A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lbg;}
if(A!=qJ){FP_TRACE_BYTECODE_OPTIMIZATION(255,"A[A!=cImmed] cAbsNot cNot","A cAbsNotNot","    with A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lhk;}
qD
Default6;case
cAbsNotNot:FP_TRACE_BYTECODE_OPTIMIZATION(252,"cAbsNotNot cNot","cAbsNot","");qD
Lhl;case
cEqual:FP_TRACE_BYTECODE_OPTIMIZATION(213,"cEqual cNot","cNEqual","");qD
Lhm;case
cGreater:FP_TRACE_BYTECODE_OPTIMIZATION(211,"cGreater cNot","cLessOrEq","");qD
Lhn;case
cGreaterOrEq:FP_TRACE_BYTECODE_OPTIMIZATION(212,"cGreaterOrEq cNot","cLess","");qD
Lho;case
cLess:FP_TRACE_BYTECODE_OPTIMIZATION(209,"cLess cNot","cGreaterOrEq","");qD
Lhp;case
cLessOrEq:FP_TRACE_BYTECODE_OPTIMIZATION(210,"cLessOrEq cNot","cGreater","");qD
Lhq;case
cNEqual:FP_TRACE_BYTECODE_OPTIMIZATION(214,"cNEqual cNot","cEqual","");qD
Lia;case
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(248,"cNeg cNot","cNot","");qD
Lhj;case
cNot:FP_TRACE_BYTECODE_OPTIMIZATION(250,"cNot cNot","cNotNot","");qD
Lib;case
cNotNot:FP_TRACE_BYTECODE_OPTIMIZATION(251,"cNotNot cNot","cNot","");qD
Lhj
qE
FP_TRACE_BYTECODE_OPTIMIZATION(85,"x cNot","[fp_not(x)]","    with x = "<<x<<"\n");qD
Lic;default:Default6:;A=qR;q7
FP_TRACE_BYTECODE_OPTIMIZATION(256,"A[IsNeverNegativeValueOpcode(A)] cNot","A cAbsNot","    with A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lid;qG
TailCall_cOr:case
cOr:if(qR==qJ){x
qF
0];if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(131,"y x cOr","[fp_or(x,y)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Lie;qG
TailCall_cPow:case
cPow:if(qR==qJ){x
qF
0];if(!isEvenInteger(x+x)){if(q1-1]==cSqr){FP_TRACE_BYTECODE_OPTIMIZATION(14,"cSqr x[!isEvenInteger(x+x)] cPow","cAbs [x+x] cPow","    with x = "<<x<<"\n");qD
Lif;}
}
if(IsIntegerConst(x))qL1(q1-1]){case
cExp:FP_TRACE_BYTECODE_OPTIMIZATION(35,"cExp x[IsIntegerConst(x)] cPow","[x] cMul cExp","    with x = "<<x<<"\n");qD
Lig;case
cExp2:FP_TRACE_BYTECODE_OPTIMIZATION(36,"cExp2 x[IsIntegerConst(x)] cPow","[x] cMul cExp2","    with x = "<<x<<"\n");qD
Lih;case
cPow:if(qF1
qJ){qA1
if(!IsIntegerConst(y)){FP_TRACE_BYTECODE_OPTIMIZATION(34,"y[!IsIntegerConst(y)] cPow x[IsIntegerConst(x)] cPow","[y*x] cPow","    with x = "<<x<<", y = "<<y<<"\n");qD
Lii;}
}
FP_TRACE_BYTECODE_OPTIMIZATION(37,"cPow x[IsIntegerConst(x)] cPow","[x] cMul cPow","    with x = "<<x<<"\n");qD
Lij;}
}
if(isEvenInteger(x))q51
FP_TRACE_BYTECODE_OPTIMIZATION(299,"cAbs x[isEvenInteger(x)] cPow","[x] cPow","    with x = "<<x<<"\n");qD
Lik;case
cMul:if(qF1
cAbs){FP_TRACE_BYTECODE_OPTIMIZATION(300,"cAbs cMul x[isEvenInteger(x)] cPow","cMul [x] cPow","    with x = "<<x<<"\n");qD
Lil
qB1}
}
if(q41(0)){FP_TRACE_BYTECODE_OPTIMIZATION(122,"x[x==Value_t(0)] cPow","[Value_t(0)] cMul [Value_t(1)] cAdd","    with x = "<<x<<"\n");qD
Lim;}
if(q41(0.5)){FP_TRACE_BYTECODE_OPTIMIZATION(175,"x[x==Value_t(0.5)] cPow","cSqrt","    with x = "<<x<<"\n");qD
Lin;}
if(q41(1)/Value_t(3)){FP_TRACE_BYTECODE_OPTIMIZATION(176,"x[x==Value_t(1)/Value_t(3)] cPow","cCbrt","    with x = "<<x<<"\n");qD
Lio;}
if(q41(1)/Value_t(-3)){FP_TRACE_BYTECODE_OPTIMIZATION(177,"x[x==Value_t(1)/Value_t(-3)] cPow","cCbrt cInv","    with x = "<<x<<"\n");qD
Lip;}
if(q41(-0.5)){FP_TRACE_BYTECODE_OPTIMIZATION(178,"x[x==Value_t(-0.5)] cPow","cRSqrt","    with x = "<<x<<"\n");qD
Liq;}
if(q41(-1)){FP_TRACE_BYTECODE_OPTIMIZATION(179,"x[x==Value_t(-1)] cPow","cInv","    with x = "<<x<<"\n");qD
Lja;}
switch(q1-1]){case
cPow:if(qF1
qJ){qA1
if(isEvenInteger(y)&&!isEvenInteger(x*y)){FP_TRACE_BYTECODE_OPTIMIZATION(13,"y[isEvenInteger(y)&&!isEvenInteger(x*y)] cPow x cPow","cAbs [y*x] cPow","    with x = "<<x<<", y = "<<y<<"\n");qD
Ljb;}
FP_TRACE_BYTECODE_OPTIMIZATION(173,"y cPow x cPow","[y*x] cPow","    with x = "<<x<<", y = "<<y<<"\n");qD
Lii;qS
cSqr:FP_TRACE_BYTECODE_OPTIMIZATION(38,"cSqr x cPow","[x+x] cPow","    with x = "<<x<<"\n");qD
Ljc;case
qJ:qA1
if(y!=Value_t(0)||x>=Value_t(0)){FP_TRACE_BYTECODE_OPTIMIZATION(84,"y[y!=Value_t(0)||x>=Value_t(0)] x cPow","[fp_pow(y,x)]","    with x = "<<x<<", y = "<<y<<"\n");qD
Ljd
qB1}
FP_TRACE_BYTECODE_OPTIMIZATION(333,"x cPow","[DO_POWI]","    with x = "<<x<<"\n");if(TryCompilePowi(x))return;}
qD
qH1;TailCall_cRad:case
cRad:if(qR==qJ){x
qF
0];FP_TRACE_BYTECODE_OPTIMIZATION(66,"x cRad","[DegreesToRadians(x)]","    with x = "<<x<<"\n");qD
Lje;}
qD
qH1;TailCall_cSin:case
cSin
qK1(qR){case
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(306,"cNeg cSin","cSin cNeg","");qD
Ljf
qE
FP_TRACE_BYTECODE_OPTIMIZATION(58,"x cSin","[fp_sin(x)]","    with x = "<<x<<"\n");qD
Ljg;}
qD
qH1;TailCall_cSinh:case
cSinh
qK1(qR){case
cAcosh:FP_TRACE_BYTECODE_OPTIMIZATION(315,"cAcosh cSinh","[DO_STACKPLUS1] cSqr [Value_t(-1)] cAdd cSqrt","");incStackPtr();--StackPtr;qD
Ljh;case
cAsinh:FP_TRACE_BYTECODE_OPTIMIZATION(192,"cAsinh cSinh","","");qD
Lbg;case
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(307,"cNeg cSinh","cSinh cNeg","");qD
Lji
qE
FP_TRACE_BYTECODE_OPTIMIZATION(59,"x cSinh","[fp_sinh(x)]","    with x = "<<x<<"\n");qD
Ljj;}
qD
qH1;TailCall_cSqr:case
cSqr
qK1(qR){case
cAbs:FP_TRACE_BYTECODE_OPTIMIZATION(354,"cAbs cSqr","cSqr","");qD
Ljk;case
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(353,"cNeg cSqr","cSqr","");qD
Ljk;case
cSqrt:A=q1-1];q7
FP_TRACE_BYTECODE_OPTIMIZATION(181,"A[IsNeverNegativeValueOpcode(A)] cSqrt cSqr","A","    with A = "<<FP_TRACE_OPCODENAME(A)<<"\n");qD
Lbg
qT
TailCall_cSqrt:case
cSqrt
qK1(qR){case
cSqr:FP_TRACE_BYTECODE_OPTIMIZATION(15,"cSqr cSqrt","cAbs","");qD
Ljl
qE
if(x>=Value_t(0)){FP_TRACE_BYTECODE_OPTIMIZATION(60,"x[x>=Value_t(0)] cSqrt","[fp_sqrt(x)]","    with x = "<<x<<"\n");qD
Ljm
qT
TailCall_cSub:case
cSub
qK1(qR)qW
FP_TRACE_BYTECODE_OPTIMIZATION(152,"cDup cSub","[Value_t(0)] cMul","");qD
Ljn;case
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(205,"cNeg cSub","cAdd","");qD
Ljo
qE
if(q41(0)){FP_TRACE_BYTECODE_OPTIMIZATION(77,"x[x==Value_t(0)] cSub","","    with x = "<<x<<"\n");qD
Lak;}
if(q1-1]==qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(78,"y x cSub","[y-x]","    with x = "<<x<<", y = "<<y<<"\n");qD
Ljp;}
FP_TRACE_BYTECODE_OPTIMIZATION(168,"x cSub","[-x] cAdd","    with x = "<<x<<"\n");qD
Ljq;}
qD
qH1;TailCall_cTan:case
cTan
qK1(qR){case
cAtan2:FP_TRACE_BYTECODE_OPTIMIZATION(196,"cAtan2 cTan","cDiv","");qD
Lfe;case
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(308,"cNeg cTan","cTan cNeg","");qD
Lka
qE
FP_TRACE_BYTECODE_OPTIMIZATION(62,"x cTan","[fp_tan(x)]","    with x = "<<x<<"\n");qD
Lkb;}
qD
qH1;TailCall_cTanh:case
cTanh
qK1(qR){case
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(309,"cNeg cTanh","cTanh cNeg","");qD
Lkc
qE
FP_TRACE_BYTECODE_OPTIMIZATION(63,"x cTanh","[fp_tanh(x)]","    with x = "<<x<<"\n");qD
Lkd;}
qD
qH1;TailCall_cTrunc:case
cTrunc
qK1(qR){case
qV
FP_TRACE_BYTECODE_OPTIMIZATION(64,"x cTrunc","[fp_trunc(x)]","    with x = "<<x<<"\n");qD
Lke;default:A=qR;if(IsAlwaysIntegerOpcode(A)){FP_TRACE_BYTECODE_OPTIMIZATION(259,"A[IsAlwaysIntegerOpcode(A)] cTrunc","A","    with A = "<<FP_TRACE_OPCODENAME(A)<<"\n");return;qG
default:qH1:;A=opcode;if(IsComparisonOpcode(A)){if(qR==qJ){x
qF
0];switch(q1-1]){case
cAdd:if(qF1
qJ){qA1
FP_TRACE_BYTECODE_OPTIMIZATION(221,"y cAdd x A[IsComparisonOpcode(A)]","[x-y] A","    with A = "<<FP_TRACE_OPCODENAME(A)<<", x = "<<x<<", y = "<<y<<"\n");qD
Lkf;qS
cAtan:if(fp_abs(x)<fp_const_pi<Value_t>()*Value_t(0.5)){FP_TRACE_BYTECODE_OPTIMIZATION(237,"cAtan[fp_abs(x)<fp_const_pi<Value_t>()*Value_t(0.5)] x A[IsComparisonOpcode(A)]","[fp_tan(x)] A","    with A = "<<FP_TRACE_OPCODENAME(A)<<", x = "<<x<<"\n");qD
Lkg;qS
cExp:if(x>Value_t(0)){FP_TRACE_BYTECODE_OPTIMIZATION(227,"cExp[x>Value_t(0)] x A[IsComparisonOpcode(A)]","[fp_log(x)] A","    with A = "<<FP_TRACE_OPCODENAME(A)<<", x = "<<x<<"\n");qD
Lkh;qS
cExp2:if(x
qU){FP_TRACE_BYTECODE_OPTIMIZATION(228,"cExp2[x>Value_t(0)] x A[IsComparisonOpcode(A)]","[fp_log2(x)] A","    with A = "<<FP_TRACE_OPCODENAME(A)<<", x = "<<x<<"\n");qD
Lki;qS
cLog:B=q1-2];if(IsNeverNegativeValueOpcode(B)){FP_TRACE_BYTECODE_OPTIMIZATION(230,"B[IsNeverNegativeValueOpcode(B)] cLog x A[IsComparisonOpcode(A)]","B [fp_exp(x)] A","    with A = "<<FP_TRACE_OPCODENAME(A)<<", x = "<<x<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lkj;qS
cLog10
q8
FP_TRACE_BYTECODE_OPTIMIZATION(232,"B[IsNeverNegativeValueOpcode(B)] cLog10 x A[IsComparisonOpcode(A)]","B [fp_pow(Value_t(10),x)] A","    with A = "<<FP_TRACE_OPCODENAME(A)<<", x = "<<x<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lkk;qS
cLog2
q8
FP_TRACE_BYTECODE_OPTIMIZATION(231,"B[IsNeverNegativeValueOpcode(B)] cLog2 x A[IsComparisonOpcode(A)]","B [fp_exp2(x)] A","    with A = "<<FP_TRACE_OPCODENAME(A)<<", x = "<<x<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Lkl;qS
cMul:if(qF1
qJ){qA1
if(y
qU){FP_TRACE_BYTECODE_OPTIMIZATION(223,"y[y>Value_t(0)] cMul x A[IsComparisonOpcode(A)]","[x/y] A","    with A = "<<FP_TRACE_OPCODENAME(A)<<", x = "<<x<<", y = "<<y<<"\n");qD
Lkm;}
if(y<Value_t(0)){FP_TRACE_BYTECODE_OPTIMIZATION(224,"y[y<Value_t(0)] cMul x A[IsComparisonOpcode(A)]","[x/y] {OppositeComparisonOpcode(A)}","    with A = "<<FP_TRACE_OPCODENAME(A)<<", x = "<<x<<", y = "<<y<<"\n");qD
Lkn;}
qS
cNeg:FP_TRACE_BYTECODE_OPTIMIZATION(222,"cNeg x A[IsComparisonOpcode(A)]","[-x] {OppositeComparisonOpcode(A)}","    with A = "<<FP_TRACE_OPCODENAME(A)<<", x = "<<x<<"\n");qD
Lko;case
cPow:if(x
qU){if(qF1
qJ){qA1
if(y
qU){FP_TRACE_BYTECODE_OPTIMIZATION(225,"y[y>Value_t(0)] cPow[x>Value_t(0)] x A[IsComparisonOpcode(A)]","[fp_pow(x,1/y)] A","    with A = "<<FP_TRACE_OPCODENAME(A)<<", x = "<<x<<", y = "<<y<<"\n");qD
Lkp;}
}
qS
cSinh:FP_TRACE_BYTECODE_OPTIMIZATION(238,"cSinh x A[IsComparisonOpcode(A)]","[fp_asinh(x)] A","    with A = "<<FP_TRACE_OPCODENAME(A)<<", x = "<<x<<"\n");qD
Lkq;case
cSqr:if(x
qU){FP_TRACE_BYTECODE_OPTIMIZATION(226,"cSqr[x>Value_t(0)] x A[IsComparisonOpcode(A)]","cAbs [fp_sqrt(x)] A","    with A = "<<FP_TRACE_OPCODENAME(A)<<", x = "<<x<<"\n");qD
Lla;qS
cTanh:if(fp_abs(x)<Value_t(1)){FP_TRACE_BYTECODE_OPTIMIZATION(239,"cTanh[fp_abs(x)<Value_t(1)] x A[IsComparisonOpcode(A)]","[fp_atanh(x)] A","    with A = "<<FP_TRACE_OPCODENAME(A)<<", x = "<<x<<"\n");qD
Llb
qB1}
}
}
if(A>=VarBegin&&q3
size()>0){B=qR;if(B==A){FP_TRACE_BYTECODE_OPTIMIZATION(356,"B[B==A] A[A>=VarBegin&&data->ByteCode.size()>0]","B cDup","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<"\n");qD
Llc;}
}
if(q11(A)){B=qR;if(B>=VarBegin&&q3
size()>1){C=q1-1];if(C==A){D=q1-2];if(D==B){FP_TRACE_BYTECODE_OPTIMIZATION(357,"D[D==B] C[C==A] B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]","D C cDup","    with A = "<<FP_TRACE_OPCODENAME(A)<<", B = "<<FP_TRACE_OPCODENAME(B)<<", C = "<<FP_TRACE_OPCODENAME(C)<<", D = "<<FP_TRACE_OPCODENAME(D)<<"\n");qD
Lld;}
}
}
}
}
qD
Lle;Laa:ByteCodePtr-=1;q3
qL
Llf:qD
TailCall_cAbs;Lab
qQ
0]=fp_abs(x);return;Lac
qQ
0]=fp_acos
q71
Lad
qQ
0]=fp_acosh
q71
Lae:qP
push_back(Value_t(4));q1-2]=qJ;ByteCodePtr-=2;Llg:q9
Llh:opcode=cMul;Lli:FP_ReDefinePointers();Llj:qD
TailCall_cMul;Laf:for(q81
tmp=4;tmp-->0;)q3
pop_back()qI
qP
push_back(Value_t(4));Llk:q3
push_back(qJ);qD
Llh;Lag
qQ
0]=x+Value_t(1);q1-2]=qJ;ByteCodePtr-=2;q9
Lll:opcode=cMul;qD
Llj;Lah:ByteCodePtr-=1;q3
qL
opcode=cSub;Llm:qD
TailCall_cSub;Lai
qJ1
y+x;qP
qL
q9
Lln:FP_ReDefinePointers();Llo:qD
TailCall_cAdd;Laj
qJ1
y+x;Lak
qK
Lbg:q3
qL
return;Lal
qJ1
fp_and(x,y);qD
Lak;Lam
qQ
0]=fp_asin
q71
Lan
qQ
0]=fp_asinh
q71
Lao
qQ
0]=fp_atan
q71
Lap
qJ1
fp_atan2(y,x);qD
Lak;Laq
qQ
0]=fp_atanh
q71
Lba
qQ
0]=fp_cbrt
q71
Lbb:q3
qL
qM(cFloor);Llp:opcode=cNeg;FP_ReDefinePointers();qD
TailCall_cNeg;Lbc
qQ
0]=fp_ceil
q71
Lbd:ByteCodePtr-=1;q3
qL
Llq:qD
TailCall_cCos;Lbe
qQ
0]=fp_cos
q71
Lbf:ByteCodePtr-=1;q3
qL
qD
TailCall_cCosh;Lbh
qD1(cSqr);qP
push_back(Value_t(1));Lma:q3
push_back(qJ);qM(cAdd);Lmb:opcode=cSqrt;FP_ReDefinePointers();qD
TailCall_cSqrt;Lbi
qQ
0]=fp_cosh
q71
Lbj
qQ
0]=RadiansToDegrees
q71
Lbk
qD1(cSec);qD
Llh;Lbl
q4(cTan)qN
Lbm
q4(cSin)qN
Lbn:qP
push_back(Value_t(0));qR=qJ;Lmc:qM(cMul);qP
push_back(Value_t(1));Lmd:q3
push_back(qJ);Lme:opcode=cAdd;qD
Lln;Lbo
q4(cNeg);qM(cExp)qN
Lbp
qH(cExp2)qN
Lbq:ByteCodePtr-=1;q3
qL
qD
Lll;Lca
qH(cPow)qN
Lcb
q4(cCos)qN
Lcc
q4(cCsc)qN
Lcd
q4(cCot)qN
Lce:for(q81
tmp=2;tmp-->0;)qC1(q81
tmp=3;tmp-->0;)q3
qL
Lmf:opcode=cDeg;FP_ReDefinePointers();qD
TailCall_cDeg;Lcf:for(q81
tmp=2;tmp-->0;)qC1(q81
tmp=3;tmp-->0;)q3
qL
Lmg:opcode=cRad;FP_ReDefinePointers();qD
TailCall_cRad;Lcg
qJ1
y/x;qP
qL
qD
Llg;Lch
qQ
0]=-x;q1-1]=qJ;ByteCodePtr-=1;q3
qL
Lmh:qD
TailCall_cDiv;Lci
qJ1
y/x;qD
Lak;Lcj
qQ
0]=Value_t(1)/x;qD
Lll;Lck:q1-1]=qJ;ByteCodePtr-=1;q3
qL
Lmi:qD
TailCall_cEqual;Lcl
qJ1
fp_equal(y,x);qD
Lak;Lcm
qK
q9
qM(cExp);qP
push_back(fp_exp(x));qD
Llk;Lcn
qQ
0]=fp_exp
q71
Lco
qK
q9
qM(cExp2);qP
push_back(fp_exp2(x));qD
Llk;Lcp
qQ
0]=fp_exp2
q71
Lcq:qP
push_back(fp_log(Value_t(2)));q3
push_back(qJ);Lmj:qM(cMul);opcode=cExp;FP_ReDefinePointers();qD
TailCall_cExp;Lda
q4(cCeil);qD
Llp;Ldb
qQ
0]=fp_floor
q71
Ldc
qJ1
fp_less(x,y);qD
Lak;Ldd
qK
q3
qL
qM(cNeg);Lid:qM(cAbsNot);return;Lde
qQ
0]=Value_t(0.5)/x;q1-1]=qJ;q3
pop_back()qI
Lmk:qM(cNotNot);return;Ldf
qJ1
fp_lessOrEq(x,y);qD
Lak;Ldg
qK
Lhk:q3
qL
Lml:qM(cAbsNotNot);return;Ldh
qQ
0]=fp_int
q71
Ldi
q4(cSec);return;Ldj:ByteCodePtr-=1;q3
qL
opcode=cTan;Lmm:qD
TailCall_cTan;Ldk:ByteCodePtr-=1;q3
qL
opcode=cSin;Lmn:qD
TailCall_cSin;Ldl
q4(cNeg);opcode=cPow;Lmo:FP_ReDefinePointers();Lmp:qD
TailCall_cPow;Ldm:ByteCodePtr-=1;q3
qL
opcode=cCos;qD
Llq;Ldn
q4(cCsc);return;Ldo
q4(cRSqrt);return;Ldp
q4(cCot);return;Ldq
qQ
0]=Value_t(1)/x;return;Lea
qQ
0]=Value_t(0.5)/x;q1-1]=qJ;q3
pop_back()qI
opcode=cNot;FP_ReDefinePointers();Lmq:qD
TailCall_cNot;Leb
qJ1
fp_less(y,x);qD
Lak;Lec
qK
Lhl:q3
qL
qD
Lid;Led
qJ1
fp_lessOrEq(y,x);qD
Lak;Lee
qK
q3
qL
qM(cNeg);qD
Lml;Lef
qK
q9
qM(cLog);qP
push_back(fp_log(x));qD
Lmd;Leg
q4(cAbs);qM(cLog);Lna:q3
push_back(cDup);qD
Lme;Leh
qQ
0]=fp_log
q71
Lei:qP
push_back(fp_log10(fp_const_e<Value_t>()));Lnb:qR=qJ
qN
Lej
qK
q9
qM(cLog10);qP
push_back(qI1(x));qD
Lmd;Lek
q4(cAbs);qM(cLog10);qD
Lna;Lel
qQ
0]=qI1
q71
Lem:qP
push_back(fp_log2(fp_const_e<Value_t>()));qD
Lnb;Len
qK
q9
qM(cLog2);qP
push_back(qO1(x));qD
Lmd;Leo
q4(cAbs);qM(cLog2);qD
Lna;Lep
qQ
0]=qO1
q71
Leq
qJ1
fp_max(x,y);qD
Lak;Lfa
qJ1
fp_min(x,y);qD
Lak;Lfb
qJ1
fp_mod(y,x);qD
Lak;Lfc
qJ1
x;qC1(q81
tmp=3;tmp-->0;)q3
qL
qM(A);Lnc:qM(cMul);qP
push_back(y*x);qD
Lmd;Lfd:ByteCodePtr-=1;q3
qL
opcode=cSqr;Lnd:qD
TailCall_cSqr;Lfe:ByteCodePtr-=1;q3
qL
opcode=cDiv;qD
Lmh;Lff:for(q81
tmp=3;tmp-->0;)q3
qL
qM(cSqr)qI
qD
Llp;Lfg:q9
qM(cSqr);qD
Llp;Lfh
qQ
0]=x+Value_t(1);q1-2]=qJ;ByteCodePtr-=2;q9
opcode=cPow;qD
Lmp;Lfi:q1-2]=qJ;ByteCodePtr-=2;q9
qD
Llj;Lfj
qJ1
x;Lne
qK
q9
qD
Lli;Lfk:for(q81
tmp=2;tmp-->0;)qP
qL
Lnf:for(q81
tmp=4;tmp-->0;)q3
qL
Lng:qM(A);qP
push_back(x);Lnh:q3
push_back(qJ);qD
Lli;Lfl
qK
qD
Lnf;Lfm
qK
for(q81
tmp=4;tmp-->0;)q3
qL
qM(B);qD
Lng;Lfn
qK
for(q81
tmp=3;tmp-->0;)q3
qL
qD
Lng;Lfo:q1-1]=qJ;qD
Lbg;Lfp:q1-1]=qJ;ByteCodePtr-=1;q3
qL
qD
Llj;Lfq
qK
Lni:for(q81
tmp=3;tmp-->0;)q3
qL
return;Lga
qQ
0]=x+x;qD
Lfi;Lgb
qJ1
x;q1-4]=qJ;qC1(q81
tmp=4;tmp-->0;)q3
pop_back()qI
qM(A);qP
push_back(y*x);q3
push_back(qJ)qI
qD
Lme;Lgc
qJ1
x;qP
qL
q9
qD
Lnc;Lgd
qQ
0]=RadiansToDegrees(x);qD
Lfp;Lge
qK
for(q81
tmp=4;tmp-->0;)q3
qL
qD
Lli;Lgf
qK
for(q81
tmp=4;tmp-->0;)q3
pop_back()qI
qP
push_back(x+x);qD
Lnh;Lgg:for(q81
tmp=2;tmp-->0;)qP
qL
qD
Lni;Lgh
qJ1
y*x;qD
Lne;Lgi
qQ
0]=-x;qD
Lfp;Lgj
qQ
0]=DegreesToRadians(x);qD
Lfp;Lgk
qJ1
x;q1-4]=qJ;qC1(q81
tmp=4;tmp-->0;)q3
pop_back()qI
qM(A);qP
push_back(y*x);q3
push_back(qJ)qI
opcode=cSub;FP_ReDefinePointers();qD
Llm;Lgl
qJ1
y*x;qD
Lak;Lgm:qR=cDup;ImmedPtr-=1;qP
qL
Lnj:opcode=cAdd;qD
Llo;Lgn
qK
q3
qL
qD
Lmf;Lgo
qA
Lmg;Lgp
qA
Llp;Lgq:q9
Lnk:qM(cSqr);qD
Lli;Lha:for(q81
tmp=3;tmp-->0;)q3
qL
qD
Lnk;Lhb:q1-1]=qJ;ByteCodePtr-=1;q3
qL
Lnl:qD
TailCall_cNEqual;Lhc
qJ1
fp_nequal(y,x);qD
Lak;Lhd
qQ
0]=-x;qD
Lbq;Lhe
qQ
0]=-x;q9
qM(cMul);opcode=cSin;FP_ReDefinePointers();qD
Lmn;Lhf
q2
cSinh;FP_ReDefinePointers();qD
TailCall_cSinh;Lhg
q2
cTan;FP_ReDefinePointers();qD
Lmm;Lhh
q2
cTanh;FP_ReDefinePointers();qD
TailCall_cTanh;Lhi
qQ
0]=-x;return;Lhj:ByteCodePtr-=1;q3
qL
qD
Lmq;Lhm:ByteCodePtr-=1;q3
qL
opcode=cNEqual;qD
Lnl;Lhn:ByteCodePtr-=1;q3
qL
opcode=cLessOrEq;qD
TailCall_cLessOrEq;Lho:ByteCodePtr-=1;q3
qL
opcode=cLess;qD
TailCall_cLess;Lhp:ByteCodePtr-=1;q3
qL
opcode=cGreaterOrEq;qD
TailCall_cGreaterOrEq;Lhq:ByteCodePtr-=1;q3
qL
opcode=cGreater;qD
TailCall_cGreater;Lia:ByteCodePtr-=1;q3
qL
opcode=cEqual;qD
Lmi;Lib:q3
qL
qD
Lmk;Lic
qQ
0]=fp_not
q71
Lie
qJ1
fp_or(x,y);qD
Lak;Lif
qK
q9
qM(cAbs);qP
push_back(x+x);Lnm:q3
push_back(qJ);qD
Lmo;Lig:q1-1]=qJ;q3
qL
qD
Lmj;Lih:q1-1]=qJ;q3
pop_back()qI
opcode=cExp2;FP_ReDefinePointers();qD
TailCall_cExp2;Lii
qJ1
y*x;qP
qL
q9
qP1
Lij:q1-1]=qJ;qC
qP1
Lik:q1-1]=qJ;ByteCodePtr-=1;q3
qL
qD
Lmp;Lil
qK
for(q81
tmp=3;tmp-->0;)qC
qP
push_back(x);qD
Lnm;Lim
qQ
0]=Value_t(0);qD
Lmc;Lin
qA
Lmb;Lio
qK
q3
qL
opcode=cCbrt;FP_ReDefinePointers();qD
TailCall_cCbrt;Lip
qK
q3
qL
qM(cCbrt);Lnn:opcode=cInv;FP_ReDefinePointers();qD
TailCall_cInv;Liq
qK
qD
Ldo;Lja
qA
Lnn;Ljb:for(q81
tmp=2;tmp-->0;)qC1(q81
tmp=3;tmp-->0;)q3
qL
qM(cAbs);qP
push_back(y*x);qD
Lnm;Ljc
qQ
0]=x+x;qD
Lik;Ljd
qJ1
fp_pow(y,x);qD
Lak;Lje
qQ
0]=DegreesToRadians
q71
Ljf
q4(cSin);qD
Llp;Ljg
qQ
0]=fp_sin
q71
Ljh
q4(cSqr);qP
push_back(Value_t(-1));qD
Lma;Lji
q4(cSinh);qD
Llp;Ljj
qQ
0]=fp_sinh
q71
Ljk:ByteCodePtr-=1;q3
qL
qD
Lnd;Ljl:ByteCodePtr-=1;q3
qL
opcode=cAbs;qD
Llf;Ljm
qQ
0]=fp_sqrt
q71
Ljn:qP
push_back(Value_t(0));qD
Lnb;Ljo:ByteCodePtr-=1;q3
qL
qD
Lnj;Ljp
qJ1
y-x;qD
Lak;Ljq
qQ
0]=-x;qD
Lnj;Lka
q4(cTan);qD
Llp;Lkb
qQ
0]=fp_tan
q71
Lkc
q4(cTanh);qD
Llp;Lkd
qQ
0]=fp_tanh
q71
Lke
qQ
0]=fp_trunc
q71
Lkf
qJ1
x-y;Lno
qK
q9
Lnp:qM(A);return;Lkg
qQ
0]=fp_tan(x);Lnq:q1-1]=qJ;q3
qL
qD
Lnp;Lkh
qQ
0]=fp_log(x);qD
Lnq;Lki
qQ
0]=qO1
qX
Lkj
qQ
0]=fp_exp
qX
Lkk
qQ
0]=fp_pow(Value_t(10),x);qD
Lnq;Lkl
qQ
0]=fp_exp2
qX
Lkm
qJ1
x/y;qD
Lno;Lkn
qJ1
x/y;qP
qL
q9
Loa:qM(OppositeComparisonOpcode(A));return;Lko
qQ
0]=-x;q1-1]=qJ;q3
qL
qD
Loa;Lkp
qJ1
fp_pow(x,1/y);qD
Lno;Lkq
qQ
0]=fp_asinh
qX
Lla
qK
q9
qM(cAbs);qP
push_back(fp_sqrt(x));q3
push_back(qJ);qD
Lnp;Llb
qQ
0]=fp_atanh
qX
Llc:q3
push_back(cDup);return;Lld:qR=cDup;return;Lle:q3
push_back(opcode);return;return;qD
TailCall_cAcos;qD
TailCall_cAcosh;qD
TailCall_cAnd;qD
TailCall_cAsin;qD
TailCall_cAsinh;qD
TailCall_cAtan;qD
TailCall_cAtan2;qD
TailCall_cAtanh;qD
TailCall_cCeil;qD
TailCall_cFloor;qD
TailCall_cInt;qD
TailCall_cLog;qD
TailCall_cLog10;qD
TailCall_cLog2;qD
TailCall_cMax;qD
TailCall_cMin;qD
TailCall_cMod;qD
TailCall_cOr;qD
TailCall_cRad;qD
TailCall_cSin;qD
TailCall_cSinh;qD
TailCall_cSqrt;qD
TailCall_cSub;qD
TailCall_cTan;qD
TailCall_cTanh;qD
TailCall_cTrunc;
#endif


#undef FP_ReDefinePointers


#undef FP_TRACE_BYTECODE_OPTIMIZATION


#undef FP_TRACE_OPCODENAME

