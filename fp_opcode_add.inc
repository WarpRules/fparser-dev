/* Function Parser for C++ v4.0.5   

   Note: This file contains generated code and is thus not
   intended to be to be modified by hand. It was generated
   by util/bytecoderules_parser , which is available in the
   development package.

   Note: You don't need to add this file to your project. It's
   enough for it to be in the same directory as fparser.cc. This
   file is internally #included by fparser.cc.
*/
#define FP_TRACE_BYTECODE_OPTIMIZATION(srcline,from,to,with) \
    /*std::cout << "Changing \"" from "\"\t(line " #srcline ")\n" \
                   "    into \"" to "\"\n" with*/
#define FP_TRACE_OPCODENAME(op) \
    (op < VarBegin \
        ? FP_GetOpcodeName(OPCODE(op)) \
        : findName(data->namePtrs,op,NameData<Value_t>::VARIABLE))
#define qH1 "B[B>=VarBegin]"qG1
#define qG1 " "a0
#define qF1 " cSub"
#define qE1 q3 d6 q8
#define qD1 q3 dJ gC
#define qC1 cInv:w1
#define qB1 w6","mL
#define qA1 qE A);q8
#define q91 if(g9 w1
#define q81 "cSub"
#define q71 ", C "wB
#define q61 "cNeg"
#define q51 oN oD gD w1
#define q41 w6","wR
#define q31 <<","mL
#define q21 <<","a6<<","
#define q11 <<","a6 wD
#define q01 w6","m2
#define aZ wX wM
#define aY m9" "
#define aX hG w1
#define aW x>qR w1
#define aV " "wC
#define aU "[DO_STACKPLUS1] "
#define aT " A"
#define aS ==aX
#define aR wU wO
#define aQ cExp:w1
#define aP "cSin"
#define aO "cCsc"
#define aN "cTan"
#define aM "cCot"
#define aL "cSec"
#define aK "cCeil"
#define aJ =hP oD
#define aI wZ" "
#define aH "[-x] "
#define aG d0 w1
#define aF qK hI w1
#define aE "cFloor"
#define aD "cExp"
#define aC ==dN){w1
#define aB "cInv"
#define aA m6 q81
#define a9 qQ if(x
#define a8 "cRDiv"
#define a7 "cLess"
#define a6 " "wV
#define a5 h7 w1
#define a4 wX" "
#define a3 a8 m8
#define a2 "cMin"
#define a1 "cMax"
#define a0 "cDiv"
#define mZ wM wS a0
#define mY wY" x "wO
#define mX "cSqrt"
#define mW aD" "
#define mV "[y*x]"
#define mU wO" "
#define mT "cNeg "
#define mS "cLog"
#define mR "cGreater"
#define mQ g7 w1
#define mP "A "wX wH
#define mO "cAbsNot"
#define mN "cLessOrEq"
#define mM "cNotNot"
#define mL " "m6 wB
#define mK "[ y+x] "
#define mJ "y "
#define mI x);g5
#define mH "cCos"
#define mG "cAbs"
#define mF "cCosh"
#define mE "[-x]"wM
#define mD "cLog10"
#define mC dX d8 dZ w1
#define mB "cLog2"
#define mA "cExp2"
#define m9 "cSqr"
#define m8 " x"wH
#define m7 " "wI
#define m6 "B "
#define m5 h0 qO;qF w1
#define m4 "cGreaterOrEq"
#define m3 " cNot"
#define m2 " "wF
#define m1 "cEqual"
#define m0 " cNeg"
#define wZ "B[B==A]"
#define wY "cRSub"
#define wX "[x]"
#define wW " cPow"
#define wV "x = "<<x
#define wU "cDup "
#define wT "cNEqual"
#define wS "x[x!=Value_t(0)] "
#define wR aT" "wB
#define wQ "B[B>=VarBegin]"qF1" x "wO
#define wP "x[x>Value_t(0)]"
#define wO "cAdd"
#define wN w6"\n"
#define wM wH" "
#define wL "B[IsNeverNegativeValueOpcode(B)] "
#define wK "x[IsIntegerConst(x)]"wW
#define wJ "A[A>=VarBegin] "
#define wI "   with"wR
#define wH " "w7
#define wG "x[x==Value_t(1)] "
#define wF mJ w5
#define wE "A[A>=VarBegin]"wH
#define wD <<"\n"
#define wC "x A[IsComparisonOpcode(A)]"
#define wB "="w3
#define wA "B[IsBinaryOpcode(B)&&!HasInvalidRangesOpcode(B)] "w4
#define w9 "A[IsNeverNegativeValueOpcode(A)] "
#define w8 "x[x==Value_t(0)] "
#define w7 "cMul"
#define w6 "   "w2
#define w5 "= "<<y wD
#define w4 "A[IsBinaryOpcode(A)&&!HasInvalidRangesOpcode(A)] "w8 w7
#define w3 " "<<FP_TRACE_OPCODENAME
#define w2 " with"a6<<
#define w1 FP_TRACE_BYTECODE_OPTIMIZATION
#define w0 cLog10
#define oZ cTanh:
#define oY cSinh:
#define oX q0 dH
#define oW TailCall_cSin
#define oV TailCall_cAbs
#define oU q0[0]
#define oT q0[-4]
#define oS cAbs){
#define oR cRDiv:
#define oQ oH qM
#define oP qB-1)
#define oO cSub:
#define oN hD qO
#define oM fp_exp(x
#define oL fp_exp2(x
#define oK q0 qU
#define oJ oN;qF
#define oI hD[0];
#define oH ;}gB}
#define oG Lbo oH
#define oF oP&&x
#define oE opcode
#define oD ;if(
#define oC hS oD
#define oB gF Lpm
#define oA d8 gF
#define o9 Immed
#define o8 Lao;
#define o7 fp_pow(
#define o6 q3 A gF
#define o5 dL q3 dJ
#define o4 q5:x=qD;
#define o3 qC q5);
#define o2 q3 A);
#define o1 qG-1]
#define o0 gV qR
#define dZ ){hB q9
#define dY g4 q7 4 qI
#define dX o1==
#define dW gW dX dQ dZ
#define dV g4 gG
#define dU qA gX
#define dT cNEqual
#define dS cLess:
#define dR cEqual
#define dQ cRSub
#define dP .size()
#define dO 4 qI q3
#define dN cDup
#define dM =q5;
#define dL hS dM
#define dK cPow:
#define dJ d8);
#define dI cExp2:
#define dH -=1;h1
#define dG Default7
#define dF Default6
#define dE Default3
#define dD Default1
#define dC qK g8 2]
#define dB fp_log(
#define dA q7 qH
#define d9 cNotNot
#define d8 cMul
#define d7 fp_log10(
#define d6 cAbs);
#define d5 fp_abs(x)
#define d4 qE cAdd);
#define d3 fp_log2(
#define d2 0.5)){
#define d1 B==A){
#define d0 hL d1
#define hZ hL B qX
#define hY qC q5 gF
#define hX cGreater
#define hW Value_t
#define hV hW>()
#define hU hS gR 1;
#define hT cLessOrEq
#define hS q0 qO
#define hR cAbs:
#define hQ incStackPtr();qY
#define hP q0[-3]
#define hO qN gT hB
#define hN h0[0];
#define hM qB 1)
#define hL B qW
#define hK Default5 qL
#define hJ g3=y+x;
#define hI cSqr:
#define hH q5:y=g3;
#define hG hM){
#define hF gU==hG
#define hE switch(
#define hD A=q0
#define hC case
#define hB qG-2]
#define hA cGreaterOrEq
#define h9 GetDegreesToRadiansFactor gM
#define h8 GetRadiansToDegreesFactor gM
#define h7 o1 qP
#define h6 g6 h7
#define h5 TailCall_cSub
#define h4 TailCall_cNeg
#define h3 unsigned
#define h2 ByteCode
#define h1 q1 q4
#define h0 g2 hD
#define gZ data->
#define gY ,x gF o8
#define gX dN:
#define gW if(A qX
#define gV if(x==
#define gU g6 if(x
#define gT cAdd:
#define gS TailCall_cNotNot
#define gR dM q0-=
#define gQ q7 dO dJ
#define gP gO gR 2;dA q1
#define gO q0[-2]
#define gN gF Lmm;
#define gM <hV){
#define gL ,x gF Laq;
#define gK ,y gF Laq;
#define gJ qD=-x;
#define gI isEvenInteger(
#define gH qL g8 1]q9 if(x>qR
#define gG q7 3 qI
#define gF );q4
#define gE ,y gF o8
#define gD IsLogicalOpcode(A)){
#define gC oE=
#define gB break;
#define gA IsUnaryOpcode(
#define g9 IsAlwaysIntegerOpcode(A)){
#define g8 d8:qG-
#define g7 cNeg:
#define g6 qG 0]q9
#define g5 return;
#define g4 gZ q6
#define g3 q2 qO
#define g2 default:
#define g1 >=VarBegin&&gZ h2 dP>
#define g0 hE gO){hC
#define qZ &&!HasInvalidRangesOpcode(
#define qY --StackPtr;q4
#define qX >=VarBegin){
#define qW =gO oD
#define qV hL IsNeverNegativeValueOpcode(B)){
#define qU -=1;q1 gC
#define qT g4 dA
#define qS FP_ReDefinePointers();q4
#define qR qB 0)){
#define qQ qL o4
#define qP ==q5){y=g3;
#define qO [-1]
#define qN hE hS){hC
#define qM q4 Default0;
#define qL ;hC
#define qK }break qL
#define qJ }}qM
#define qI ;tmp-->0;)
#define qH 2 qI
#define qG if(q0[
#define qF if(IsNeverNegativeValueOpcode(A)){
#define qE AddFunctionOpcode(
#define qD q2[0]
#define qC gZ h2.push_back(
#define qB hW(
#define qA hE oU){hC
#define q9 ==q5){x=qD;
#define q8 gZ o9.push_back(
#define q7 for(h3 tmp=
#define q6 o9.pop_back();
#define q5 cImmed
#define q4 goto
#define q3 q1 qE
#define q2 ImmedPtr
#define q1 gZ h2.pop_back();
#define q0 ByteCodePtr
h3*q0;hW*q2;
#define FP_ReDefinePointers() q0=!gZ h2.empty()?&gZ h2[0]+gZ h2 dP-1:0;q2=!gZ o9.empty()?&gZ o9[0]+gZ o9 dP-1:0;
FP_ReDefinePointers();
#if(!FP_FLOAT_VERSION)
hW
x;h3
A;hW
y;h3
B;h3
C;h3
D;hE
oE){oV:hC
hR
qA
mQ(235,mT
mG,mG,);q4
Laa
qQ
w1(40,"x "
mG,"[fp_abs(x)]"
,wN);q4
Lab;hN
qF
w1(294,w9
mG,"A"
,m7(A)wD);g5
qJ
TailCall_cAdd:hC
gT
dU
hO
aC(185,wU
mU
aR,"[Value_t(4)]"
wH,);q4
Lac;dC==cAdd){qG-3]aC(186,aR
wM
aR,w7" [Value_t(4)]"
wH,);q4
Lad;}
}
gB}
q4
dD
qL
g8
1]q9
hB
aC(188,wU"x"
wM
wO,"[x+Value_t(1)]"
wH,wN);q4
Lae;}
}
q4
dD
qL
mQ(242,mT
wO,q81,);q4
Laf
qQ
hO
qP
w1(179,mJ
mU"x "
wO,"[y+x] "
wO,q01);q4
Lag;qK
dQ:g0
gT
qG-3]qP
w1(120,mJ
mU
mY,mK
mU
wY,q01);q4
Lah;qK
mQ(121,mT
mY,aH
mU
wY,wN);q4
Lai
qL
hH
w1(119,mJ
mY,mK
wY,q01);q4
Laj;qK
oO
hZ
hE
hP){hC
gT
qG-4]qP
w1(114,mJ
mU
wQ,mK
mU
aA,qB1(B)<<","
m2);q4
Lak;qK
mQ(115,mT
wQ,aH
mU
aA,qB1(B)wD);q4
Lal
qL
hH
w1(113,mJ
wQ,mK
aA,qB1(B)<<","
m2);q4
Lam;}
qK
hH
w1(76,mJ"x "
wO,"[y+x]"
,q01);q4
Lan;}
o0
w1(75,w8
wO,,wN);q4
o8}
gB
g2
dD:;oI
dW
w1(112,"x "
wY" "
wJ
wO,"[DO_STACKPLUS1]"
aT" "
a4
mU
wY,m7(A)q11);hQ
Lap;}
w1(117,wY" "
wJ
wO,"[DO_STACKPLUS1]"
aT" "
mU
wY,m7(A)wD);hQ
Laq;}
qJ
TailCall_cAnd:hC
cAnd:h6
w1(168,mJ"x cAnd"
,"[fp_and(x,y)]"
,q01);q4
Lba;qJ
TailCall_cDiv:hC
cDiv:dU
w1(158,wU
a0,"[Value_t(0)]"
wM"[Value_t(1)] "
wO,);q4
Lbb
a9!=qR
qN
mQ(176,mT
wS
a0,aH
a0,wN);q4
Lbc
qL
hH
w1(73,mJ
wS
a0,"[y/x]"
,q01);q4
Lbd;}
}
gV
aX(72,wG
a0,,wN);q4
o8}
gB}
qM
TailCall_cEqual:hC
dR:hF
q51(322,"A[IsLogicalOpcode(A)] "
wG
m1,"A"
,q41(A)wD);q4
o8}
}
a5(166,mJ"x "
m1,"[fp_equal(y,x)]"
,q01);q4
Lbe;}
o0
qN
hR
w1(254,mG" "
w8
m1,a4
m1,wN);q4
Lbf
qL
hI
w1(256,aY
w8
m1,a4
m1,wN);q4
Lbf;}
w1(312,w8
m1,"cNot"
,wN);q4
Lbg;qJ
TailCall_cGreater:hC
hX:gU==qR
qN
hR
w1(314,mG" "
w8
mR,mM,wN);q4
Lbh;m5(318,w9
w8
mR,"A "
mM,q41(A)wD);q4
Lbi;}
}
}
a5(164,mJ"x "
mR,"[fp_less(x,y)]"
,q01);q4
Lbj;qJ
TailCall_cGreaterOrEq:hC
hA:hF
qN
hR
w1(315,mG" "
wG
m4,mM,wN);q4
Lbh;m5(319,w9
wG
m4,"A "
mM,q41(A)wD);q4
Lbi;}
}
}
a5(165,mJ"x "
m4,"[fp_lessOrEq(x,y)]"
,q01);q4
Lbk;qJ
TailCall_cInv:hC
cInv:qA
qC1(236,aB" "
aB,,);q4
Lbl
qL
cSqrt:w1(67,mX" "
aB,"cRSqrt"
,);q4
Lbm
a9!=qR
w1(69,wS
aB,"[Value_t(1)/x]"
,wN);q4
Lbn
oQ
TailCall_cLess:hC
dS
gU==qR
oJ
w1(123,w9
w8
a7,mP,q41(A)wD);q4
Lbo;}
}
gV
hG
qN
hR
w1(316,mG" "
wG
a7,"cNot"
,wN);q4
Lbp;m5(320,w9
wG
a7,"A"
m3,q41(A)wD);q4
Lbg;}
}
}
a5(162,mJ"x "
a7,"[fp_less(y,x)]"
,q01);q4
Lbq;qJ
TailCall_cLessOrEq:hC
hT:gU==qR
qN
hR
w1(317,mG" "
w8
mN,"cNot"
,wN);q4
Lbp;m5(321,w9
w8
mN,"A"
m3,q41(A)wD);q4
Lbg;}
}
}
a5(163,mJ"x "
mN,"[fp_lessOrEq(y,x)]"
,q01);q4
Lca;qJ
TailCall_cMax:hC
cMax:dU
w1(148,wU
a1,,);q4
Lbl
qQ
a5(82,mJ"x "
a1,"[fp_max(x,y)]"
,q01);q4
Lcb;}
gB
hN
gW
qN
gX
aG(150,aI
wU
wJ
a1,m6"cDup"
,m7(A)q31(B)wD);q4
Lbl;qK
cMax:aG(152,aI
a1" "
wJ
a1,m6
a1,m7(A)q31(B)wD);q4
Lbl
oH
qJ
TailCall_cMin:hC
cMin:dU
w1(147,wU
a2,,);q4
Lbl
qQ
a5(81,mJ"x "
a2,"[fp_min(x,y)]"
,q01);q4
Lcc;}
gB
hN
gW
qN
gX
aG(149,aI
wU
wJ
a2,m6"cDup"
,m7(A)q31(B)wD);q4
Lbl;qK
cMin:aG(151,aI
a2" "
wJ
a2,m6
a2,m7(A)q31(B)wD);q4
Lbl
oH
qJ
TailCall_cMod:hC
cMod:gU!=qR
a5(74,mJ
wS"cMod"
,"[fp_mod(y,x)]"
,q01);q4
Lcd;}
qJ
TailCall_cMul:hC
d8:dU
w1(395,"cDup"
wH,m9,);q4
Lce
qL
qC1(241,aB
wH,a0,);q4
Lcf
qL
g7
oN;gW
g0
d8:B
aJ
d1
w1(387,wZ
wM
wJ
q61
wH,m6
m9
wM
q61,m7(A)q31(B)wD);q4
Lcg;}
q4
Default2;g2
Default2:;aG(381,aI
wJ
q61
wH,m6
m9
m0,m7(A)q31(B)wD);q4
Lch;}
}
}
q4
dE
qL
dK
o1
q9
hB
aC(189,wU"x"
wW
wH,"[x+Value_t(1)]"
wW,wN);q4
Lci;}
}
q4
dE
qQ
o0
qN
d8:A
qW
A
qX
w1(132,wE" "
w8
w7,wX
wH,q41(A)wD);q4
Lcj;}
q4
Default4;g2
Default4:;oN
oD
IsBinaryOpcode(A)qZ
A)){g0
hH
w1(131,mJ
w4,wX
wH,q41(A)<<","
m2);q4
Lck;g2
hL
IsBinaryOpcode(B)qZ
B)){hE
hP){hC
hH
w1(135,mJ
wA,mP,q41(A)q31(B)<<","
m2);q4
Lcl;g2
C
aJ
C
qX
w1(133,"C[C>=VarBegin] "
wA,mP,q41(A)q31(B)<<q71(C)wD);q4
Lcm;}
if(gA
C)qZ
C)){w1(134,"C[IsUnaryOpcode(C)&&!HasInvalidRangesOpcode(C)] "
wA,m6
mP,q41(A)q31(B)<<q71(C)wD);q4
Lcn;}
}
}
if(B
qX
w1(129,"B[B>=VarBegin] "
w4,wX
wH,q41(A)q31(B)wD);q4
Lcj;}
if(gA
B)qZ
B)){w1(130,"B[IsUnaryOpcode(B)&&!HasInvalidRangesOpcode(B)] "
w4,mP,q41(A)q31(B)wD);q4
Lco;}
}
}
gW
w1(127,wJ
w8
w7,wX,q41(A)wD);q4
Lcp;}
if(gA
A)qZ
A)){w1(128,"A[IsUnaryOpcode(A)&&!HasInvalidRangesOpcode(A)] "
w8
w7,wX
wH,q41(A)wD);q4
Lcq;}
}
}
qN
gT
g0
gX
w1(193,aR
m8,"[x+x]"
wH,wN);q4
Lda
qL
g8
3]qP
hD[-4];gW
w1(281,wJ"y"
wM
wO
m8,aZ"A "
mV
wM
wO,w6", "
mJ"= "
<<y<<","
wR(A)wD);q4
Ldb;}
qK
hH
w1(280,mJ
wO
m8,aZ
mV" "
wO,q01);q4
Ldc;qK
d8:g0
gT
qG-3]aC(195,aR
wM"x"
wH,w7" [x+x]"
wH,wN);q4
Ldd;qK
hH
if(y*x
aS(183,"y[y*x==Value_t(1)]"
wM"x"
wH,,q01);q4
Lde;}
w1(184,"y"
wM"x"
wH,mV
wH,q01);q4
Ldf;qK
mQ(173,q61
m8,"[-x]"
wH,wN);q4
Ldg
qL
oO
hB==d8){qG-3]qP
hD[-4];gW
w1(282,wJ"y"
wM
q81
m8,aZ"A "
mV
wM
q81,w6", "
mJ"= "
<<y<<","
wR(A)wD);q4
Ldh;}
}
qK
hH
w1(71,"y"
m8,mV,q01);q4
Ldi;}
gV
aX(70,wG
w7,,wN);q4
o8}
gV
qB
2)){w1(138,"x[x==Value_t(2)]"
wH,aR,wN);q4
Ldj;}
gV
oP){w1(175,"x[x==Value_t(-1)]"
wH,q61,wN);q4
Ldk;}
gB
g2
dE:;oI
gW
qN
d8:g0
g7
B
aJ
d1
w1(384,wZ
m0
wM
wE,m6
m9
wM
q61,m7(A)q31(B)wD);q4
Lcg;}
q4
Default5;g2
Default5:;aG(375,wZ
wM
wE,m6
m9
wH,m7(A)q31(B)wD);q4
Ldl;}
}
q4
dF
qL
g7
aG(378,wZ
m0" "
wE,m6
m9
m0,m7(A)q31(B)wD);q4
Lch;}
q4
dF;g2
dF:;B=oC
d1
w1(372,aI
wE,m6
m9,m7(A)q31(B)wD);q4
Lce;}
}
}
if(gA
A)){B=oC
B
g1
1){hB==d8){C
aJ
C==A){D=oT
oD
D==B){w1(394,"D[D==B] C[C==A]"
wM"B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]"
wH,"D C "
m9
wH,m7(A)q31(B)<<q71(C)<<", D "
wB(D)wD);q4
Ldm;}
}
}
}
qJ
TailCall_cNEqual:hC
dT:hF
q51(323,"A[IsLogicalOpcode(A)] "
wG
wT,"A"
m3,q41(A)wD);q4
Lbg;}
}
a5(167,mJ"x "
wT,"[fp_nequal(y,x)]"
,q01);q4
Ldn;}
o0
qN
hR
w1(255,mG" "
w8
wT,a4
wT,wN);q4
Ldo
qL
hI
w1(257,aY
w8
wT,a4
wT,wN);q4
Ldo;}
w1(313,w8
wT,mM,wN);q4
Lbi;qJ
h4:hC
g7
qA
g8
1]q9
w1(174,"x"
wM
q61,"[-x]"
wH,wN);q4
Ldp;qK
mQ(237,q61
m0,,);q4
Lbl
qQ
w1(68,"x"
m0,"[-x]"
,wN);q4
Ldq;}
qM
TailCall_cNot:hC
cNot:qA
hR
w1(285,mG
m3,"cNot"
,);q4
Lea
qL
cAbsNot:q51(290,"A[IsLogicalOpcode(A)] "
mO
m3,"A"
,m7(A)wD);q4
Lbl;}
if(A!=q5){w1(291,"A[A!=cImmed] "
mO
m3,"A cAbsNotNot"
,m7(A)wD);q4
Leb;}
q4
dG
qL
cAbsNotNot:w1(288,"cAbsNotNot"
m3,mO,);q4
Lec
qL
gT
o1
q9
w1(325,"x "
wO
m3,a4
m1,wN);q4
Led;}
q4
dG
qL
dR:w1(251,m1
m3,wT,);q4
Lee
qL
hX:w1(249,mR
m3,mN,);q4
Lef
qL
hA:w1(250,m4
m3,a7,);q4
Leg
qL
dS
w1(247,a7
m3,m4,);q4
Leh
qL
hT:w1(248,mN
m3,mR,);q4
Lei
qL
dT:w1(252,wT
m3,m1,);q4
Lej
qL
mQ(284,q61
m3,"cNot"
,);q4
Lea
qL
cNot:w1(286,"cNot"
m3,mM,);q4
Lek
qL
d9:w1(287,mM
m3,"cNot"
,);q4
Lea
qQ
w1(85,"x"
m3,"[fp_not(x)]"
,wN);q4
Lel;g2
dG:;oI
qF
w1(292,w9"cNot"
,"A "
mO,m7(A)wD);q4
Lem;qJ
gS:hC
d9:qG
0]==cAdd){o1
q9
w1(324,"x "
mU
mM,a4
wT,wN);q4
Len;qJ
TailCall_cOr:hC
cOr:h6
w1(169,mJ"x cOr"
,"[fp_or(x,y)]"
,q01);q4
Leo;qJ
TailCall_cSqr:hC
hI
qA
hR
w1(390,mG" "
m9,m9,);q4
Lep
qL
mQ(389,mT
m9,m9,);q4
Lep;}
qM
h5:hC
oO
dU
w1(190,wU
q81,"[Value_t(0)]"
wH,);q4
Leq
qL
mQ(243,mT
q81,wO,);q4
Lfa
qQ
o0
w1(77,w8
q81,,wN);q4
o8}
a5(78,mJ"x"
qF1,"[y-x]"
,q01);q4
Lfb;}
w1(206,"x"
qF1,aH
wO,wN);q4
Lfc;hN
dW
w1(111,"x "
wY" "
wJ
q81,"A "
mU
a4
wY,m7(A)q11);q4
Lfd;}
w1(118,wY" "
wJ
q81,"[DO_STACKPLUS1]"
aT
qF1" "
wY,m7(A)wD);hQ
Lfe;}
qJ
g2
Default0:;A=oE
oD
IsComparisonOpcode(A)){g6
hO
qP
w1(259,mJ
mU
wC,"[x-y]"
aT,m7(A)q21
m2);q4
Lff;qK
mQ(260,mT
wC,aH"{OppositeComparisonOpcode(A)}"
,m7(A)q11);q4
Lfg;}
}
}
if(A
g1
0){B=oU
oD
d1
w1(392,aI"A[A>=VarBegin&&data->ByteCode.size()>0]"
,m6"cDup"
,m7(A)q31(B)wD);q4
Lfh;}
}
if(gA
A)){B=oU
oD
B
g1
1){C=oC
C==A){D
qW
D==B){w1(393,"D[D==B] C[C==A] B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]"
,"D C cDup"
,m7(A)q31(B)<<q71(C)<<", D "
wB(D)wD);q4
Lfi;}
}
}
}
}
q4
Lfj;Laa:oX
oV;Lab:qD=d5;g5
Lac:q8
qB
4));gP
Lfk:gC
d8;Lfl:FP_ReDefinePointers();Lfm:q4
TailCall_cMul;Lad:gQ
q8
qB
4));hY
Lfk;Lae:qD=x+hM;gP
Lbo:gC
d8;q4
Lfm;Laf:oK
cSub;Lfn:q4
h5;Lag:hJ
qT
q1
Lfo:FP_ReDefinePointers();Lfp:q4
TailCall_cAdd;Lah:hJ
dV
q1
Lfq:d4
Lga:qE
dQ);g5
Lai:gJ
gO
dM
dA
h1
Lfq;Laj:hJ
qT
h1
Lga;Lak:hJ
dY
q1
Lgb:d4
Lgc:qE
B);Lgd:gC
cSub;qS
Lfn;Lal:gJ
hP
dM
gG
h1
Lgb;Lam:hJ
dV
h1
Lgc;Lan:hJ
Lao:g4
Lbl:q1
g5
Lap:dV
o2
q8
x);hY
Lfq;Laq:dA
o6
Lfq;Lba:g3=fp_and(x
gE
Lbb:q8
qB
0));oU
dM
qE
dJ
q8
hM);Lge:o3
Lgf:gC
cAdd;q4
Lfo;Lbc:gJ
hU
q1
Lgg:q4
TailCall_cDiv;Lbd:g3=y/x;q4
o8
Lbe:g3=fp_equal(y
gY
Lbf:hU
q1
Lgh:q4
TailCall_cEqual;Lbg:g4
q1
Lgi:gC
cNot;FP_ReDefinePointers();Lgj:q4
TailCall_cNot;Lbh:qT
q1
Lgk:gC
d9;FP_ReDefinePointers();Lgl:q4
gS;Lbi:g4
h1
Lgk;Lbj:g3=fp_less(x
gE
Lbk:g3=fp_lessOrEq(x
gE
Lbm:q3
cRSqrt);g5
Lbn:qD=hM/x;g5
Lbp:qT
h1
Lgi;Lbq:g3=fp_less(y
gY
Lca:g3=fp_lessOrEq(y
gY
Lcb:g3=fp_max(x
gE
Lcc:g3=fp_min(x
gE
Lcd:g3=fp_mod(y
gY
Lce:oK
cSqr;Lgm:q4
TailCall_cSqr;Lcf:oK
cDiv;q4
Lgg;Lcg:gG
q3
cSqr);qE
dJ
Lgn:gC
cNeg;qS
h4;Lch:dA
q3
cSqr
gF
Lgn;Lci:qD=x+hM;gO
dM
dA
q3
cPow);g5
Lcj:gP
q4
Lfm;Lck:g3=x;Lgo:qT
h1
Lfl;Lcl:dA
g4
Lgp:q7
4
qI
q1
Lgq:qA1
x);Lha:hY
Lfl;Lcm:g4
q4
Lgp;Lcn:g4
q7
dO
B
gF
Lgq;Lco:dV
h1
Lgq;Lcp:dL
q4
Lbl;Lcq:hU
h1
Lfm;Lda:qD=x+x;q4
Lcj;Ldb:g3=x;oT
dM
g4
gQ
qA1
y*x);o3
qE
oA
Lgf;Ldc:g3=x;qT
q3
dJ
q8
y*x
gF
Lge;Ldd:g4
gQ
q8
x+x
gF
Lha;Lde:dA
dV
q1
g5
Ldf:g3=y*x;q4
Lgo;Ldg:gJ
q4
Lcq;Ldh:g3=x;oT
dM
g4
gQ
qA1
y*x);o3
qE
oA
Lgd;Ldi:g3=y*x;q4
o8
Ldj:oU=dN;q2-=1;g4
Lhb:gC
cAdd;q4
Lfp;Ldk:g4
h1
Lgn;Ldl:dA
q1
Lhc:qE
cSqr
gF
Lfl;Ldm:gG
h1
Lhc;Ldn:g3=fp_nequal(y
gY
Ldo:hU
q1
Lhd:q4
TailCall_cNEqual;Ldp:gJ
oX
Lbo;Ldq:gJ
g5
Lea:oX
Lgj;Leb:q3
cAbsNotNot);g5
Lec:q1
Lem:qE
cAbsNot);g5
Led:qD=x;Lej:oK
dR;q4
Lgh;Lee:oK
dT;q4
Lhd;Lef:oK
hT;q4
TailCall_cLessOrEq;Leg:oK
cLess;q4
TailCall_cLess;Leh:oK
hA;q4
TailCall_cGreaterOrEq;Lei:oK
hX;q4
TailCall_cGreater;Lek:oK
d9;q4
Lgl;Lel:qD=fp_not(mI
Len:qD=x;q4
Lee;Leo:g3=fp_or(x
gE
Lep:oX
Lgm;Leq:q8
qB
0));oU
dM
q4
Lfk;Lfa:oX
Lhb;Lfb:g3=y-x;q4
o8
Lfc:gJ
q4
Lhb;Lfd:dV
o2
d4
q8
x);hY
Lga;Lfe:dA
o2
qE
cSub
gF
Lga;Lff:g3=x-y;qT
o2
g5
Lfg:gJ
dL
q3
OppositeComparisonOpcode(A));g5
Lfh:qC
dN);g5
Lfi:oU=dN;g5
Lfj:qC
oE);g5
g5
q4
TailCall_cAnd;q4
TailCall_cInv;q4
TailCall_cMax;q4
TailCall_cMin;q4
TailCall_cMod;q4
h4;q4
gS;q4
TailCall_cOr;q4
h5;
#endif
#if(FP_FLOAT_VERSION)
hW
x;h3
A;hW
y;h3
B;h3
C;h3
D;hE
oE){oV:hC
hR
qA
mQ(235,mT
mG,mG,);q4
Laa
qQ
w1(40,"x "
mG,"[fp_abs(x)]"
,wN);q4
Lab;hN
qF
w1(294,w9
mG,"A"
,m7(A)wD);g5
qJ
TailCall_cAcos:hC
cAcos:gU>=oF<=aX(44,"x[x>=Value_t(-1)&&x<=Value_t(1)] cAcos"
,"[fp_acos(x)]"
,wN);q4
Lac;qJ
TailCall_cAcosh:hC
cAcosh:gU>=aX(41,"x[x>=Value_t(1)] cAcosh"
,"[fp_acosh(x)]"
,wN);q4
Lad;qJ
TailCall_cAdd:hC
gT
dU
hO
aC(185,wU
mU
aR,"[Value_t(4)]"
wH,);q4
Lae;dC==cAdd){qG-3]aC(186,aR
wM
aR,w7" [Value_t(4)]"
wH,);q4
Laf;}
}
gB}
q4
dD
qL
g8
1]q9
hB
aC(188,wU"x"
wM
wO,"[x+Value_t(1)]"
wH,wN);q4
Lag;}
}
q4
dD
qL
mQ(242,mT
wO,q81,);q4
Lah
qQ
hO
qP
w1(179,mJ
mU"x "
wO,"[y+x] "
wO,q01);q4
Lai;qK
dQ:g0
gT
qG-3]qP
w1(120,mJ
mU
mY,mK
mU
wY,q01);q4
Laj;qK
mQ(121,mT
mY,aH
mU
wY,wN);q4
Lak
qL
hH
w1(119,mJ
mY,mK
wY,q01);q4
Lal;qK
oO
hZ
hE
hP){hC
gT
qG-4]qP
w1(114,mJ
mU
wQ,mK
mU
aA,qB1(B)<<","
m2);q4
Lam;qK
mQ(115,mT
wQ,aH
mU
aA,qB1(B)wD);q4
Lan
qL
hH
w1(113,mJ
wQ,mK
aA,qB1(B)<<","
m2);q4
o8}
qK
hH
w1(76,mJ"x "
wO,"[y+x]"
,q01);q4
Lap;}
o0
w1(75,w8
wO,,wN);q4
Laq;}
gB
g2
dD:;oI
dW
w1(112,"x "
wY" "
wJ
wO,"[DO_STACKPLUS1]"
aT" "
a4
mU
wY,m7(A)q11);hQ
Lba;}
w1(117,wY" "
wJ
wO,"[DO_STACKPLUS1]"
aT" "
mU
wY,m7(A)wD);hQ
Lbb;}
qJ
TailCall_cAnd:hC
cAnd:h6
w1(168,mJ"x cAnd"
,"[fp_and(x,y)]"
,q01);q4
Lbc;qJ
TailCall_cAsin:hC
cAsin:gU>=oF<=aX(45,"x[x>=Value_t(-1)&&x<=Value_t(1)] cAsin"
,"[fp_asin(x)]"
,wN);q4
Lbd;qJ
TailCall_cAsinh:hC
cAsinh:g6
w1(42,"x cAsinh"
,"[fp_asinh(x)]"
,wN);q4
Lbe;}
qM
TailCall_cAtan:hC
cAtan:g6
w1(46,"x cAtan"
,"[fp_atan(x)]"
,wN);q4
Lbf;}
qM
TailCall_cAtan2:hC
cAtan2:h6
w1(83,mJ"x cAtan2"
,"[fp_atan2(y,x)]"
,q01);q4
Lbg;qJ
TailCall_cAtanh:hC
cAtanh:gU>oF<aX(43,"x[x>Value_t(-1)&&x<Value_t(1)] cAtanh"
,"[fp_atanh(x)]"
,wN);q4
Lbh;qJ
TailCall_cCbrt:hC
cCbrt:g6
w1(47,"x cCbrt"
,"[fp_cbrt(x)]"
,wN);q4
Lbi;}
qM
TailCall_cCeil:hC
cCeil:qA
mQ(303,mT
aK,aE
m0,);q4
Lbj
qQ
w1(48,"x "
aK,"[fp_ceil(x)]"
,wN);q4
Lbk;hN
q91(297,"A[IsAlwaysIntegerOpcode(A)] "
aK,"A"
,m7(A)wD);g5
qJ
TailCall_cCos:hC
cCos:qA
hR
w1(338,mG" "
mH,mH,);q4
Lbl
qL
mQ(340,mT
mH,mH,);q4
Lbl
qQ
w1(49,"x "
mH,"[fp_cos(x)]"
,wN);q4
Lbm;}
qM
TailCall_cCosh:hC
cCosh:qA
hR
w1(339,mG" "
mF,mF,);q4
Lbn
qL
cAcosh:w1(231,"cAcosh "
mF,,);q4
Lbo
qL
cAsinh:w1(352,"cAsinh "
mF,aU
aY"[Value_t(1)] "
mU
mX,);hQ
Lbp
qL
mQ(341,mT
mF,mF,);q4
Lbn
qQ
w1(50,"x "
mF,"[fp_cosh(x)]"
,wN);q4
Lbq;}
qM
TailCall_cDeg:hC
cDeg:g6
w1(65,"x cDeg"
,"[RadiansToDegrees(x)]"
,wN);q4
Lca;}
qM
TailCall_cDiv:hC
cDiv:qA
cCos:w1(355,mH
qG1,aL
wH,);q4
Lcb
qL
cCot:w1(359,aM
qG1,aN
wH,);q4
Lcc
qL
cCsc:w1(357,aO
qG1,aP
wH,);q4
Lcd
qL
gX
w1(158,wU
a0,"[Value_t(0)]"
wM"[Value_t(1)] "
wO,);q4
Lce
qL
aQ(309,mW
a0,mT
aD
wH,);q4
Lcf
qL
dI
w1(310,mA
qG1,mT
mA
wH,);q4
Lcg
qL
qC1(240,aB
qG1,w7,);q4
Lch
qL
dK
w1(308,"cPow"
qG1,q61
wW
wH,);q4
Lci
qL
cSec:w1(358,aL
qG1,mH
wH,);q4
Lcj
qL
cSin:w1(354,aP
qG1,aO
wH,);q4
Lck
qL
cTan:w1(356,aN
qG1,aM
wH,);q4
Lcl
qQ
gV
aX(72,wG
a0,,wN);q4
Laq;}
if(x!=qR
qN
g8
2]qP
if((y/x)==h8
w1(197,"y[(y/x)==GetRadiansToDegreesFactor<Value_t>()]"
mZ,"cDeg"
,q01);q4
Lcm;}
if((y/x)==h9
w1(198,"y[(y/x)==GetDegreesToRadiansFactor<Value_t>()]"
mZ,"cRad"
,q01);q4
Lcn;}
w1(199,"y"
mZ,"[y/x]"
wH,q01);q4
Lco;qK
mQ(176,mT
wS
a0,aH
a0,wN);q4
Lcp
qL
hH
w1(73,mJ
wS
a0,"[y/x]"
,q01);q4
Lcq;}
w1(201,wS
a0,"[Value_t(1)/x]"
wH,wN);q4
Lda;}
gB
hN
gW
qN
cDiv:hZ
w1(93,qH1" "
wJ
a0,aU
m6"A"
wM
a0,m7(A)q31(B)wD);hQ
Ldb;qK
oR
g0
o4
w1(89,"x "
a8" "
wJ
a0,"A"
wM
a4
a8,m7(A)q11);q4
Ldc;g2
hZ
w1(88,"B[B>=VarBegin] "
a8" "
wJ
a0,"A"
wM
m6
a8,m7(A)q31(B)wD);q4
Ldd;}
}
gB}
qJ
TailCall_cEqual:hC
dR:gU==qR
qN
hR
w1(254,mG" "
w8
m1,a4
m1,wN);q4
Lde
qL
hI
w1(256,aY
w8
m1,a4
m1,wN);q4
Lde;}
}
a5(166,mJ"x "
m1,"[fp_equal(y,x)]"
,q01);q4
Ldf;qJ
TailCall_cExp:hC
cExp:qA
gT
o1
q9
w1(305,"x "
mU
aD,mW"[fp_exp(x)]"
wH,wN);q4
Ldg;qK
cLog:oJ
w1(221,w9
mS" "
aD,"A"
,m7(A)wD);q4
Lbo;qK
o4
w1(51,"x "
aD,"[fp_exp(x)]"
,wN);q4
Ldh;}
qM
TailCall_cExp2:hC
dI
qA
gT
o1
q9
w1(306,"x "
mU
mA,mA" [fp_exp2(x)]"
wH,wN);q4
Ldi;qK
cLog2:oJ
w1(222,w9
mB" "
mA,"A"
,m7(A)wD);q4
Lbo;qK
o4
w1(52,"x "
mA,"[fp_exp2(x)]"
,wN);q4
Ldj;}
w1(398,mA,aU"[fp_log(Value_t(2))]"
wM
aD,);hQ
Ldk;TailCall_cFloor:hC
cFloor:qA
mQ(302,mT
aE,aK
m0,);q4
Ldl
qQ
w1(53,"x "
aE,"[fp_floor(x)]"
,wN);q4
Ldm;hN
q91(296,"A[IsAlwaysIntegerOpcode(A)] "
aE,"A"
,m7(A)wD);g5
qJ
TailCall_cGreater:hC
hX:h6
w1(164,mJ"x "
mR,"[fp_less(x,y)]"
,q01);q4
Ldn;}
gV
qB-d2
w1(332,"x[x==Value_t(-0.5)] "
mR,mT
mO,wN);q4
Ldo;qJ
TailCall_cGreaterOrEq:hC
hA:gU!=qR
dX
oS
w1(328,mG" "
wS
m4,"[Value_t(0.5)/x]"
wM
mM,wN);q4
Ldp;}
}
a5(165,mJ"x "
m4,"[fp_lessOrEq(x,y)]"
,q01);q4
Ldq;}
gV
qB
d2
w1(331,"x[x==Value_t(0.5)] "
m4,"cAbsNotNot"
,wN);q4
Lea;qJ
TailCall_cInt:hC
cInt:qA
o4
w1(54,"x cInt"
,"[fp_int(x)]"
,wN);q4
Leb;hN
q91(298,"A[IsAlwaysIntegerOpcode(A)] cInt"
,"A"
,m7(A)wD);g5
qJ
TailCall_cInv:hC
cInv:qA
cCos:w1(361,mH" "
aB,aL,);q4
Lec
qL
cCot:w1(365,aM" "
aB,aN,);q4
Led
qL
cCsc:w1(363,aO" "
aB,aP,);q4
Lee
qL
qC1(236,aB" "
aB,,);q4
Lbo
qL
dK
w1(238,"cPow "
aB,q61
wW,);q4
Lef
qL
cSec:w1(364,aL" "
aB,mH,);q4
Leg
qL
cSin:w1(360,aP" "
aB,aO,);q4
Leh
qL
cSqrt:w1(67,mX" "
aB,"cRSqrt"
,);q4
Lei
qL
cTan:w1(362,aN" "
aB,aM,);q4
Lej
a9!=qR
w1(69,wS
aB,"[Value_t(1)/x]"
,wN);q4
Lek
oQ
TailCall_cLess:hC
dS
gU==qR
oJ
w1(123,w9
w8
a7,mP,q41(A)wD);q4
Lel;}
}
if(x!=qR
dX
oS
w1(327,mG" "
wS
a7,"[Value_t(0.5)/x]"
wM"cNot"
,wN);q4
Lem;}
}
a5(162,mJ"x "
a7,"[fp_less(y,x)]"
,q01);q4
Len;}
gV
qB
d2
w1(330,"x[x==Value_t(0.5)] "
a7,mO,wN);q4
Leo;qJ
TailCall_cLessOrEq:hC
hT:h6
w1(163,mJ"x "
mN,"[fp_lessOrEq(y,x)]"
,q01);q4
Lep;}
gV
qB-d2
w1(333,"x[x==Value_t(-0.5)] "
mN,mT"cAbsNotNot"
,wN);q4
Leq;qJ
TailCall_cLog:hC
cLog:qA
aQ(224,mW
mS,,);q4
Lbo
gH
w1(410,wP
wM
mS,mS" [fp_log(x)] "
wO,wN);q4
Lfa;}
aF(141,aY
mS,mG" "
mS" "
aR,);q4
Lfb
a9>qR
w1(55,wP" "
mS,"[fp_log(x)]"
,wN);q4
Lfc
oQ
TailCall_cLog10:hC
w0:qA
aQ(400,mW
mD,aU"[fp_log10(fp_const_e<Value_t>())]"
wH,);hQ
Lfd
gH
w1(411,wP
wM
mD,mD" [fp_log10(x)] "
wO,wN);q4
Lfe;}
aF(143,aY
mD,mG" "
mD" "
aR,);q4
Lff
a9>qR
w1(56,wP" "
mD,"[fp_log10(x)]"
,wN);q4
Lfg
oQ
TailCall_cLog2:hC
cLog2:qA
aQ(399,mW
mB,aU"[fp_log2(fp_const_e<Value_t>())]"
wH,);hQ
Lfh
qL
dI
w1(225,mA" "
mB,,);q4
Lbo
gH
w1(409,wP
wM
mB,mB" [fp_log2(x)] "
wO,wN);q4
Lfi;}
aF(142,aY
mB,mG" "
mB" "
aR,);q4
Lfj
a9>qR
w1(57,wP" "
mB,"[fp_log2(x)]"
,wN);q4
Lfk
oQ
TailCall_cMax:hC
cMax:dU
w1(148,wU
a1,,);q4
Lbo
qQ
a5(82,mJ"x "
a1,"[fp_max(x,y)]"
,q01);q4
Lfl;}
gB
hN
gW
qN
gX
aG(150,aI
wU
wJ
a1,m6"cDup"
,m7(A)q31(B)wD);q4
Lbo;qK
cMax:aG(152,aI
a1" "
wJ
a1,m6
a1,m7(A)q31(B)wD);q4
oG
qJ
TailCall_cMin:hC
cMin:dU
w1(147,wU
a2,,);q4
Lbo
qQ
a5(81,mJ"x "
a2,"[fp_min(x,y)]"
,q01);q4
Lfm;}
gB
hN
gW
qN
gX
aG(149,aI
wU
wJ
a2,m6"cDup"
,m7(A)q31(B)wD);q4
Lbo;qK
cMin:aG(151,aI
a2" "
wJ
a2,m6
a2,m7(A)q31(B)wD);q4
oG
qJ
TailCall_cMod:hC
cMod:gU!=qR
a5(74,mJ
wS"cMod"
,"[fp_mod(y,x)]"
,q01);q4
Lfn;}
qJ
TailCall_cMul:hC
d8:dU
w1(395,"cDup"
wH,m9,);q4
Lfo
qL
qC1(241,aB
wH,a0,);q4
Lfp
qL
g7
oN;gW
g0
d8:B
aJ
d1
w1(387,wZ
wM
wJ
q61
wH,m6
m9
wM
q61,m7(A)q31(B)wD);q4
Lfq;}
q4
Default2;g2
Default2:;aG(381,aI
wJ
q61
wH,m6
m9
m0,m7(A)q31(B)wD);q4
Lga;}
}
}
q4
dE
qL
dK
o1
q9
hB
aC(189,wU"x"
wW
wH,"[x+Value_t(1)]"
wW,wN);q4
Lgb;}
}
q4
dE
qQ
o0
qN
d8:A
qW
A
qX
w1(132,wE" "
w8
w7,wX
wH,q41(A)wD);q4
Lgc;}
q4
Default4;g2
Default4:;oN
oD
IsBinaryOpcode(A)qZ
A)){g0
hH
w1(131,mJ
w4,wX
wH,q41(A)<<","
m2);q4
Lgd;g2
hL
IsBinaryOpcode(B)qZ
B)){hE
hP){hC
hH
w1(135,mJ
wA,mP,q41(A)q31(B)<<","
m2);q4
Lge;g2
C
aJ
C
qX
w1(133,"C[C>=VarBegin] "
wA,mP,q41(A)q31(B)<<q71(C)wD);q4
Lgf;}
if(gA
C)qZ
C)){w1(134,"C[IsUnaryOpcode(C)&&!HasInvalidRangesOpcode(C)] "
wA,m6
mP,q41(A)q31(B)<<q71(C)wD);q4
Lgg;}
}
}
if(B
qX
w1(129,"B[B>=VarBegin] "
w4,wX
wH,q41(A)q31(B)wD);q4
Lgc;}
if(gA
B)qZ
B)){w1(130,"B[IsUnaryOpcode(B)&&!HasInvalidRangesOpcode(B)] "
w4,mP,q41(A)q31(B)wD);q4
Lgh;}
}
}
gW
w1(127,wJ
w8
w7,wX,q41(A)wD);q4
Lgi;}
if(gA
A)qZ
A)){w1(128,"A[IsUnaryOpcode(A)&&!HasInvalidRangesOpcode(A)] "
w8
w7,wX
wH,q41(A)wD);q4
Lgj;}
}
}
qN
gT
g0
gX
if(x+x
aS(192,"cDup[x+x==Value_t(1)] "
wO
m8,,wN);q4
Lgk;}
w1(193,aR
m8,"[x+x]"
wH,wN);q4
Lgl
qL
g8
3]qP
hD[-4];gW
w1(281,wJ"y"
wM
wO
m8,aZ"A "
mV
wM
wO,w6", "
mJ"= "
<<y<<","
wR(A)wD);q4
Lgm;}
qK
hH
w1(280,mJ
wO
m8,aZ
mV" "
wO,q01);q4
Lgn;qK
cDeg:w1(144,"cDeg"
m8,"[RadiansToDegrees(x)]"
wH,wN);q4
Lgo
qL
cDiv:hZ
hE
hP){hC
g8
4]qP
w1(100,"y"
wM
qH1
m8,mV
wM
m6
a0,qB1(B)<<","
m2);q4
Lgp;qK
mQ(101,mT
qH1
m8,mE
m6
a0,qB1(B)wD);q4
Lgq
qL
hH
w1(99,mJ
qH1
m8,mV" "
m6
a0,qB1(B)<<","
m2);q4
Lha;}
qK
d8:g0
gT
qG-3]==dN){if(x+x
aS(194,"cDup[x+x==Value_t(1)] "
wO
wM"x"
wH,w7,wN);q4
Lhb;}
w1(195,aR
wM"x"
wH,w7" [x+x]"
wH,wN);q4
Lhc;qK
hH
if((y*x)==h8
w1(181,"y[(y*x)==GetRadiansToDegreesFactor<Value_t>()]"
wM"x"
wH,"cDeg"
,q01);q4
Lcm;}
if((y*x)==h9
w1(182,"y[(y*x)==GetDegreesToRadiansFactor<Value_t>()]"
wM"x"
wH,"cRad"
,q01);q4
Lcn;}
if(y*x
aS(183,"y[y*x==Value_t(1)]"
wM"x"
wH,,q01);q4
Lhd;}
w1(184,"y"
wM"x"
wH,mV
wH,q01);q4
Lhe;qK
mQ(173,q61
m8,"[-x]"
wH,wN);q4
Lhf
qL
oR
g0
g8
3]qP
w1(107,"y"
wM
a3,mV
wM
a8,q01);q4
Lhg;qK
mQ(108,mT
a3,mE
a8,wN);q4
Lhh
qL
hH
w1(106,mJ
a3,mV" "
a8,q01);q4
Lhi;qK
cRad:w1(145,"cRad"
m8,"[DegreesToRadians(x)]"
wH,wN);q4
Lhj
qL
oO
hB==d8){qG-3]qP
hD[-4];gW
w1(282,wJ"y"
wM
q81
m8,aZ"A "
mV
wM
q81,w6", "
mJ"= "
<<y<<","
wR(A)wD);q4
Lhk;}
}
qK
hH
w1(71,"y"
m8,mV,q01);q4
Lhl;}
gV
aX(70,wG
w7,,wN);q4
Laq;}
gV
qB
2)){w1(138,"x[x==Value_t(2)]"
wH,aR,wN);q4
Lhm;}
gV
h8
w1(139,"x[x==GetRadiansToDegreesFactor<Value_t>()]"
wH,"cDeg"
,wN);q4
Lhn;}
gV
h9
w1(140,"x[x==GetDegreesToRadiansFactor<Value_t>()]"
wH,"cRad"
,wN);q4
Lho;}
gV
oP){w1(175,"x[x==Value_t(-1)]"
wH,q61,wN);q4
Lhp;}
gB
g2
dE:;oI
gW
qN
cDiv:hZ
w1(96,qH1" "
wE,"[DO_STACKPLUS1]"
aT
wM
m6
a0,m7(A)q31(B)wD);hQ
Lhq;}
q4
hK
d8:g0
g7
B
aJ
d1
w1(384,wZ
m0
wM
wE,m6
m9
wM
q61,m7(A)q31(B)wD);q4
Lfq;}
q4
dF;g2
dF:;aG(375,wZ
wM
wE,m6
m9
wH,m7(A)q31(B)wD);q4
Lia;}
}
q4
hK
g7
aG(378,wZ
m0" "
wE,m6
m9
m0,m7(A)q31(B)wD);q4
Lga;}
q4
hK
oR
hB
q9
w1(90,"x "
a8" "
wE,"[DO_STACKPLUS1]"
aT" "
aZ
a8,m7(A)q11);hQ
Lib;}
w1(103,a8" "
wE,"[DO_STACKPLUS1]"
aT
wM
a8,m7(A)wD);hQ
Lic;g2
Default5:;B=oC
d1
w1(372,aI
wE,m6
m9,m7(A)q31(B)wD);q4
Lfo;}
}
}
if(gA
A)){B=oC
B
g1
1){hB==d8){C
aJ
C==A){D=oT
oD
D==B){w1(394,"D[D==B] C[C==A]"
wM"B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]"
wH,"D C "
m9
wH,m7(A)q31(B)<<q71(C)<<", D "
wB(D)wD);q4
Lid;}
}
}
}
qJ
TailCall_cNEqual:hC
dT:gU==qR
qN
hR
w1(255,mG" "
w8
wT,a4
wT,wN);q4
Lie
qL
hI
w1(257,aY
w8
wT,a4
wT,wN);q4
Lie;}
}
a5(167,mJ"x "
wT,"[fp_nequal(y,x)]"
,q01);q4
Lif;qJ
h4:hC
g7
qA
g8
1]q9
w1(174,"x"
wM
q61,"[-x]"
wH,wN);q4
Lig;qK
mQ(237,q61
m0,,);q4
Lbo
qL
cSin:mC(346,"x"
wM
aP
m0,mE
aP,wN);q4
Lih;}
qK
oY
mC(347,"x"
wM"cSinh"
m0,mE"cSinh"
,wN);q4
Lii;}
qK
cTan:mC(348,"x"
wM
aN
m0,mE
aN,wN);q4
Lij;}
qK
oZ
mC(349,"x"
wM"cTanh"
m0,mE"cTanh"
,wN);q4
Lik;}
qK
o4
w1(68,"x"
m0,"[-x]"
,wN);q4
Lil;}
qM
TailCall_cNot:hC
cNot:qA
hR
w1(285,mG
m3,"cNot"
,);q4
Lim
qL
cAbsNot:q51(290,"A[IsLogicalOpcode(A)] "
mO
m3,"A"
,m7(A)wD);q4
Lbo;}
if(A!=q5){w1(291,"A[A!=cImmed] "
mO
m3,"A cAbsNotNot"
,m7(A)wD);q4
Lin;}
q4
dG
qL
cAbsNotNot:w1(288,"cAbsNotNot"
m3,mO,);q4
Lio
qL
dR:w1(251,m1
m3,wT,);q4
Lip
qL
hX:w1(249,mR
m3,mN,);q4
Liq
qL
hA:w1(250,m4
m3,a7,);q4
Lja
qL
dS
w1(247,a7
m3,m4,);q4
Ljb
qL
hT:w1(248,mN
m3,mR,);q4
Ljc
qL
dT:w1(252,wT
m3,m1,);q4
Ljd
qL
mQ(284,q61
m3,"cNot"
,);q4
Lim
qL
cNot:w1(286,"cNot"
m3,mM,);q4
Lje
qL
d9:w1(287,mM
m3,"cNot"
,);q4
Lim
qQ
w1(85,"x"
m3,"[fp_not(x)]"
,wN);q4
Ljf;g2
dG:;oI
qF
w1(292,w9"cNot"
,"A "
mO,m7(A)wD);q4
Ljg;qJ
TailCall_cOr:hC
cOr:h6
w1(169,mJ"x cOr"
,"[fp_or(x,y)]"
,q01);q4
Ljh;qJ
TailCall_cPow:hC
dK
g6
if(!gI
x+x)){dX
cSqr){w1(14,aY"x[!isEvenInteger(x+x)]"
wW,mG" [x+x]"
wW,wN);q4
Lji;}
}
if(IsIntegerConst(x)){qN
aQ(35,mW
wK,aZ
aD,wN);q4
Ljj
qL
dI
w1(36,mA" "
wK,aZ
mA,wN);q4
Ljk
qL
dK
hB
qP
if(!IsIntegerConst(y)){w1(34,"y[!IsIntegerConst(y)]"
wW" "
wK,mV
wW,q01);q4
Ljl;}
}
w1(37,"cPow "
wK,aZ"cPow"
,wN);q4
Ljm;}
}
if(gI
x)){qN
hR
w1(335,mG" x[isEvenInteger(x)]"
wW,wX
wW,wN);q4
Ljn
qL
g8
2]==oS
w1(336,mG
wM"x[isEvenInteger(x)]"
wW,w7" "
wX
wW,wN);q4
Ljo
oH}
o0
w1(160,w8"cPow"
,"[Value_t(0)]"
wM"[Value_t(1)] "
wO,wN);q4
Ljp;}
gV
qB
d2
w1(213,"x[x==Value_t(0.5)]"
wW,mX,wN);q4
Ljq;}
gV
hM/qB
3)){w1(214,"x[x==Value_t(1)/Value_t(3)]"
wW,"cCbrt"
,wN);q4
Lka;}
gV
hM/qB-3)){w1(215,"x[x==Value_t(1)/Value_t(-3)]"
wW,"cCbrt "
aB,wN);q4
Lkb;}
gV
qB-d2
w1(216,"x[x==Value_t(-0.5)]"
wW,"cRSqrt"
,wN);q4
Lkc;}
gV
oP){w1(217,"x[x==Value_t(-1)]"
wW,aB,wN);q4
Lkd;}
qN
dK
hB
qP
if(gI
y)&&!gI
x*y)){w1(13,"y[isEvenInteger(y)&&!isEvenInteger(x*y)]"
wW" x"
wW,mG" "
mV
wW,q01);q4
Lke;}
w1(211,"y"
wW" x"
wW,mV
wW,q01);q4
Ljl;aF(38,aY"x"
wW,"[x+x]"
wW,wN);q4
Lkf
qL
hH
if(y!=qB
0)||x>=qR
w1(84,"y[y!=Value_t(0)||x>=Value_t(0)] x"
wW,"[fp_pow(y,x)]"
,q01);q4
Lkg
oH
w1(369,"x"
wW,"[DO_POWI]"
,wN)oD
TryCompilePowi(x))g5}
qM
TailCall_cRad:hC
cRad:g6
w1(66,"x cRad"
,"[DegreesToRadians(x)]"
,wN);q4
Lkh;}
qM
oW:hC
cSin:qA
mQ(342,mT
aP,aP
m0,);q4
Lki
qQ
w1(58,"x "
aP,"[fp_sin(x)]"
,wN);q4
Lkj;}
qM
TailCall_cSinh:hC
oY
qA
cAcosh:w1(351,"cAcosh cSinh"
,aU
aY"[Value_t(-1)] "
mU
mX,);hQ
Lkk
qL
cAsinh:w1(230,"cAsinh cSinh"
,,);q4
Lbo
qL
mQ(343,mT"cSinh"
,"cSinh"
m0,);q4
Lkl
qQ
w1(59,"x cSinh"
,"[fp_sinh(x)]"
,wN);q4
Lkm;}
qM
TailCall_cSqr:hC
hI
qA
hR
w1(390,mG" "
m9,m9,);q4
Lkn
qL
mQ(389,mT
m9,m9,);q4
Lkn
qL
cSqrt:oJ
w1(219,w9
mX" "
m9,"A"
,m7(A)wD);q4
oG
qM
TailCall_cSqrt:hC
cSqrt:qA
hI
w1(15,aY
mX,mG,);q4
Lko
a9>=qR
w1(60,"x[x>=Value_t(0)] "
mX,"[fp_sqrt(x)]"
,wN);q4
Lkp
oQ
h5:hC
oO
dU
w1(190,wU
q81,"[Value_t(0)]"
wH,);q4
Lkq
qL
mQ(243,mT
q81,wO,);q4
Lla
qQ
o0
w1(77,w8
q81,,wN);q4
Laq;}
a5(78,mJ"x"
qF1,"[y-x]"
,q01);q4
Llb;}
w1(206,"x"
qF1,aH
wO,wN);q4
Llc;hN
dW
w1(111,"x "
wY" "
wJ
q81,"A "
mU
a4
wY,m7(A)q11);q4
Lld;}
w1(118,wY" "
wJ
q81,"[DO_STACKPLUS1]"
aT
qF1" "
wY,m7(A)wD);hQ
Lle;}
qJ
TailCall_cTan:hC
cTan:qA
cAtan2:w1(234,"cAtan2 "
aN,a0,);q4
Lfp
qL
mQ(344,mT
aN,aN
m0,);q4
Llf
qQ
w1(62,"x "
aN,"[fp_tan(x)]"
,wN);q4
Llg;}
qM
TailCall_cTanh:hC
oZ
qA
mQ(345,mT"cTanh"
,"cTanh"
m0,);q4
Llh
qQ
w1(63,"x cTanh"
,"[fp_tanh(x)]"
,wN);q4
Lli;}
qM
TailCall_cTrunc:hC
cTrunc:qA
o4
w1(64,"x cTrunc"
,"[fp_trunc(x)]"
,wN);q4
Llj;hN
q91(295,"A[IsAlwaysIntegerOpcode(A)] cTrunc"
,"A"
,m7(A)wD);g5
qJ
g2
Default0:;A=oE
oD
IsComparisonOpcode(A)){g6
hO
qP
w1(259,mJ
mU
wC,"[x-y]"
aT,m7(A)q21
m2);q4
Llk;qK
cAtan:if(d5<fp_const_pi<hV*qB
d2
w1(275,"cAtan[fp_abs(x)<fp_const_pi<Value_t>()*Value_t(0.5)]"
aV,"[fp_tan(x)]"
aT,m7(A)q11);q4
Lll;qK
cExp:if(aW(265,"cExp[x>Value_t(0)]"
aV,"[fp_log(x)]"
aT,m7(A)q11);q4
Llm;qK
dI
if(aW(266,"cExp2[x>Value_t(0)]"
aV,"[fp_log2(x)]"
aT,m7(A)q11);q4
Lln;qK
cLog:qV
w1(268,wL
mS
aV,m6"[fp_exp(x)]"
aT,m7(A)q21
mL(B)wD);q4
Llo;qK
w0:qV
w1(270,wL
mD
aV,m6"[fp_pow(Value_t(10),x)]"
aT,m7(A)q21
mL(B)wD);q4
Llp;qK
cLog2:qV
w1(269,wL
mB
aV,m6"[fp_exp2(x)]"
aT,m7(A)q21
mL(B)wD);q4
Llq;dC
qP
if(y>qR
w1(261,"y[y>Value_t(0)]"
wM
wC,"[x/y]"
aT,m7(A)q21
m2);q4
Lma;}
if(y<qR
w1(262,"y[y<Value_t(0)]"
wM
wC,"[x/y] {OppositeComparisonOpcode(A)}"
,m7(A)q21
m2);q4
Lmb;}
qK
mQ(260,mT
wC,aH"{OppositeComparisonOpcode(A)}"
,m7(A)q11);q4
Lmc
qL
dK
if(x>qR
hB
qP
if(y>qR
w1(263,"y[y>Value_t(0)] cPow[x>Value_t(0)]"
aV,"[fp_pow(x,1/y)]"
aT,m7(A)q21
m2);q4
Lmd;}
}
qK
oY
w1(276,"cSinh"
aV,"[fp_asinh(x)]"
aT,m7(A)q11);q4
Lme
qL
hI
if(aW(264,"cSqr[x>Value_t(0)]"
aV,mG" [fp_sqrt(x)]"
aT,m7(A)q11);q4
Lmf;qK
oZ
if(d5<aX(277,"cTanh[fp_abs(x)<Value_t(1)]"
aV,"[fp_atanh(x)]"
aT,m7(A)q11);q4
Lmg
oH}
}
if(A
g1
0){B=oU
oD
d1
w1(392,aI"A[A>=VarBegin&&data->ByteCode.size()>0]"
,m6"cDup"
,m7(A)q31(B)wD);q4
Lmh;}
}
if(gA
A)){B=oU
oD
B
g1
1){C=oC
C==A){D
qW
D==B){w1(393,"D[D==B] C[C==A] B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]"
,"D C cDup"
,m7(A)q31(B)<<q71(C)<<", D "
wB(D)wD);q4
Lmi;}
}
}
}
}
q4
Lmj;Laa:q0-=1;q1
Lmk:q4
oV;Lab:qD=d5;g5
Lac:qD=fp_acos(mI
Lad:qD=fp_acosh(mI
Lae:q8
qB
4));gO
gR
2;Lml:dA
q1
Lmm:gC
d8;Lmn:FP_ReDefinePointers();Lmo:q4
TailCall_cMul;Laf:gQ
q8
qB
4));Lmp:qC
q5
gN
Lag:qD=x+hM;gP
Lel:gC
d8;q4
Lmo;Lah:oK
cSub;Lmq:q4
h5;Lai:hJ
qT
q1
Lna:FP_ReDefinePointers();Lnb:q4
TailCall_cAdd;Laj:hJ
dV
q1
Lnc:d4
Lnd:qE
dQ);g5
Lak:gJ
gO
dM
dA
h1
Lnc;Lal:hJ
qT
h1
Lnd;Lam:hJ
dY
q1
Lne:d4
Lnf:qE
B);Lng:gC
cSub;qS
Lmq;Lan:gJ
hP
dM
gG
h1
Lne;Lao:hJ
dV
h1
Lnf;Lap:hJ
Laq:g4
Lbo:q1
g5
Lba:dV
o2
q8
x);hY
Lnc;Lbb:dA
o6
Lnc;Lbc:g3=fp_and(x
gK
Lbd:qD=fp_asin(mI
Lbe:qD=fp_asinh(mI
Lbf:qD=fp_atan(mI
Lbg:g3=fp_atan2(y
gL
Lbh:qD=fp_atanh(mI
Lbi:qD=fp_cbrt(mI
Lbj:q3
cFloor);Lnh:gC
cNeg;qS
h4;Lbk:qD=fp_ceil(mI
Lbl:q0-=1;q1
Lni:q4
TailCall_cCos;Lbm:qD=fp_cos(mI
Lbn:oX
TailCall_cCosh;Lbp:q3
cSqr);q8
hM);Lnj:o3
d4
Lnk:gC
cSqrt;qS
TailCall_cSqrt;Lbq:qD=fp_cosh(mI
Lca:qD=RadiansToDegrees(mI
Lcb:q3
cSec
gN
Lcc:q3
cTan
gN
Lcd:q3
cSin
gN
Lce:q8
qB
0));oU
dM
Lnl:qE
dJ
q8
hM);Lnm:o3
Lnn:gC
cAdd;q4
Lna;Lcf:q3
cNeg);qE
cExp
gN
Lcg:q3
cNeg);qE
cExp2
gN
Lch:oX
Lel;Lci:q3
cNeg);qE
cPow
gN
Lcj:q3
cCos
gN
Lck:q3
cCsc
gN
Lcl:q3
cCot
gN
Lcm:dA
dV
q1
Lno:gC
cDeg;qS
TailCall_cDeg;Lcn:dA
dV
q1
Lnp:gC
cRad;qS
TailCall_cRad;Lco:g3=y/x;g4
q4
Lml;Lcp:gJ
hU
q1
Lnq:q4
TailCall_cDiv;Lcq:g3=y/x;q4
Laq;Lda:qD=hM/x;q4
Lel;Ldb:dA
o2
qE
dJ
Loa:qS
Lnq;Ldc:dV
o2
qE
dJ
q8
x);o3
Lob:qE
cRDiv);g5
Ldd:gG
o2
qE
dJ
qE
B
gF
Lob;Lde:hU
q1
Loc:q4
TailCall_cEqual;Ldf:g3=fp_equal(y
gL
Ldg:qT
q3
cExp);q8
oM)gF
Lmp;Ldh:qD=oM);g5
Ldi:qT
q3
cExp2);q8
oL)gF
Lmp;Ldj:qD=oL);g5
Ldk:q8
dB
qB
2)));o3
Lod:qE
dJ
gC
cExp;qS
TailCall_cExp;Ldl:q3
cCeil
gF
Lnh;Ldm:qD=fp_floor(mI
Ldn:g3=fp_less(x
gK
Ldo:g4
q3
cNeg);Ljg:qE
cAbsNot);g5
Ldp:qD=qB
0.5)/x;o5
Loe:qE
d9);g5
Ldq:g3=fp_lessOrEq(x
gK
Lea:g4
Lin:q1
Lof:qE
cAbsNotNot);g5
Leb:qD=fp_int(mI
Lec:q3
cSec);g5
Led:oK
cTan;Log:q4
TailCall_cTan;Lee:oK
cSin;Loh:q4
oW;Lef:q3
cNeg);gC
cPow;Loi:FP_ReDefinePointers();Loj:q4
TailCall_cPow;Leg:oK
cCos;q4
Lni;Leh:q3
cCsc);g5
Lei:q3
cRSqrt);g5
Lej:q3
cCot);g5
Lek:qD=hM/x;g5
Lem:qD=qB
0.5)/x;o5
gC
cNot;FP_ReDefinePointers();Lok:q4
TailCall_cNot;Len:g3=fp_less(y
gL
Leo:g4
Lio:h1
Ljg;Lep:g3=fp_lessOrEq(y
gL
Leq:g4
q3
cNeg
gF
Lof;Lfa:qT
q3
cLog);q8
dB
x)gF
Lnm;Lfb:q3
d6
qE
cLog);Lol:qC
dN
gF
Lnn;Lfc:qD=dB
mI
Lfd:q8
d7
fp_const_e<hV));Lom:oU
dM
q4
Lmm;Lfe:qT
q3
w0);q8
d7
x)gF
Lnm;Lff:q3
d6
qE
w0
gF
Lol;Lfg:qD=d7
mI
Lfh:q8
d3
fp_const_e<hV)gF
Lom;Lfi:qT
q3
cLog2);q8
d3
x)gF
Lnm;Lfj:q3
d6
qE
cLog2
gF
Lol;Lfk:qD=d3
mI
Lfl:g3=fp_max(x
gK
Lfm:g3=fp_min(x
gK
Lfn:g3=fp_mod(y
gL
Lfo:oK
cSqr;Lon:q4
TailCall_cSqr;Lfp:oK
cDiv;q4
Lnq;Lfq:gG
q3
cSqr);qE
oA
Lnh;Lga:dA
q3
cSqr
gF
Lnh;Lgb:qD=x+hM;gP
gC
cPow;q4
Loj;Lgc:gP
q4
Lmo;Lgd:g3=x;Loo:qT
h1
Lmn;Lge:dA
g4
Lop:q7
4
qI
q1
Loq:qA1
x);Lpa:hY
Lmn;Lgf:g4
q4
Lop;Lgg:g4
q7
dO
B
gF
Loq;Lgh:dV
h1
Loq;Lgi:dL
q4
Lbo;Lgj:hU
h1
Lmo;Lgk:g4
Lpb:gG
q1
g5
Lgl:qD=x+x;q4
Lgc;Lgm:g3=x;oT
dM
g4
gQ
qA1
y*x);o3
qE
oA
Lnn;Lgn:g3=x;qT
q3
dJ
q8
y*x
gF
Lnm;Lgo:qD=RadiansToDegrees(x
gF
Lgj;Lgp:g3=y*x;dY
q1
Lpc:qE
dJ
Lpd:qE
B);gC
cDiv;q4
Loa;Lgq:gJ
hP
dM
gG
h1
Lpc;Lha:g3=y*x;dV
h1
Lpd;Lhb:dY
h1
Lmn;Lhc:g4
gQ
q8
x+x
gF
Lpa;Lhd:dA
g4
q4
Lpb;Lhe:g3=y*x;q4
Loo;Lhf:gJ
q4
Lgj;Lhg:g3=y*x;dV
q1
Lpe:qE
oA
Lob;Lhh:gJ
gO
dM
dA
h1
Lpe;Lhi:g3=y*x;qT
h1
Lob;Lhj:qD=DegreesToRadians(x
gF
Lgj;Lhk:g3=x;oT
dM
g4
gQ
qA1
y*x);o3
qE
oA
Lng;Lhl:g3=y*x;q4
Laq;Lhm:oU=dN;q2-=1;g4
Lpf:gC
cAdd;q4
Lnb;Lhn:g4
h1
Lno;Lho:g4
h1
Lnp;Lhp:g4
h1
Lnh;Lhq:gG
o6
Lpc;Lia:dA
q1
Lpg:qE
cSqr
gF
Lmn;Lib:dV
o2
q8
x);hY
Lpe;Lic:dA
o6
Lpe;Lid:gG
h1
Lpg;Lie:hU
q1
Lph:q4
TailCall_cNEqual;Lif:g3=fp_nequal(y
gL
Lig:gJ
q4
Lch;Lih:gJ
dA
qD1
cSin;qS
Loh;Lii:gJ
dA
qD1
cSinh;qS
TailCall_cSinh;Lij:gJ
dA
qD1
cTan;qS
Log;Lik:gJ
dA
qD1
cTanh;qS
TailCall_cTanh;Lil:gJ
g5
Lim:oX
Lok;Lip:oK
dT;q4
Lph;Liq:oK
hT;q4
TailCall_cLessOrEq;Lja:oK
cLess;q4
TailCall_cLess;Ljb:oK
hA;q4
TailCall_cGreaterOrEq;Ljc:oK
hX;q4
TailCall_cGreater;Ljd:oK
dR;q4
Loc;Lje:h1
Loe;Ljf:qD=fp_not(mI
Ljh:g3=fp_or(x
gK
Lji:qT
qE1
x+x);Lpi:hY
Loi;Ljj:dL
h1
Lod;Ljk:o5
gC
cExp2;qS
TailCall_cExp2;Ljl:g3=y*x;qT
h1
Loi;Ljm:dL
q3
oA
Loi;Ljn:hU
h1
Loj;Ljo:dV
q3
dJ
q8
x
gF
Lpi;Ljp:qD=qB
0
gF
Lnl;Ljq:g4
h1
Lnk;Lka:g4
q1
gC
cCbrt;qS
TailCall_cCbrt;Lkb:g4
q3
cCbrt);Lpj:gC
cInv;qS
TailCall_cInv;Lkc:g4
q4
Lei;Lkd:g4
h1
Lpj;Lke:dA
dV
qE1
y*x
gF
Lpi;Lkf:qD=x+x;q4
Ljn;Lkg:g3=o7
y
gL
Lkh:qD=DegreesToRadians(mI
Lki:q3
cSin
gF
Lnh;Lkj:qD=fp_sin(mI
Lkk:q3
cSqr);q8
oP
gF
Lnj;Lkl:q3
cSinh
gF
Lnh;Lkm:qD=fp_sinh(mI
Lkn:oX
Lon;Lko:oK
cAbs;q4
Lmk;Lkp:qD=fp_sqrt(mI
Lkq:q8
qB
0)gF
Lom;Lla:oX
Lpf;Llb:g3=y-x;q4
Laq;Llc:gJ
q4
Lpf;Lld:dV
o2
d4
q8
x);hY
Lnd;Lle:dA
o2
qE
cSub
gF
Lnd;Llf:q3
cTan
gF
Lnh;Llg:qD=fp_tan(mI
Llh:q3
cTanh
gF
Lnh;Lli:qD=fp_tanh(mI
Llj:qD=fp_trunc(mI
Llk:g3=x-y;Lpk:qT
q1
Lpl:qE
A);g5
Lll:qD=fp_tan(x);Lpm:dL
h1
Lpl;Llm:qD=dB
x
oB;Lln:qD=d3
x
oB;Llo:qD=oM
oB;Llp:qD=o7
qB
10),x
oB;Llq:qD=oL
oB;Lma:g3=x/y;q4
Lpk;Lmb:g3=x/y;qT
q1
Lpn:qE
OppositeComparisonOpcode(A));g5
Lmc:gJ
dL
h1
Lpn;Lmd:g3=o7
x,1/y
gF
Lpk;Lme:qD=fp_asinh(x
oB;Lmf:qT
qE1
fp_sqrt(x));hY
Lpl;Lmg:qD=fp_atanh(x
oB;Lmh:qC
dN);g5
Lmi:oU=dN;g5
Lmj:qC
oE);g5
g5
q4
TailCall_cAcos;q4
TailCall_cAcosh;q4
TailCall_cAnd;q4
TailCall_cAsin;q4
TailCall_cAsinh;q4
TailCall_cAtan;q4
TailCall_cAtan2;q4
TailCall_cAtanh;q4
TailCall_cCeil;q4
TailCall_cFloor;q4
TailCall_cInt;q4
TailCall_cLog;q4
TailCall_cLog10;q4
TailCall_cLog2;q4
TailCall_cMax;q4
TailCall_cMin;q4
TailCall_cMod;q4
TailCall_cOr;q4
TailCall_cRad;q4
oW;q4
TailCall_cSinh;q4
TailCall_cSqrt;q4
h5;q4
TailCall_cTan;q4
TailCall_cTanh;q4
TailCall_cTrunc;
#endif
#undef FP_ReDefinePointers
#undef FP_TRACE_BYTECODE_OPTIMIZATION
#undef FP_TRACE_OPCODENAME
