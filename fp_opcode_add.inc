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
#define qF1 "B[B>=VarBegin]"qD1
#define qE1 " "oS
#define qD1 " "mU
#define qC1 cInv:oO
#define qB1 ", C"oP
#define qA1 qF dR gK
#define q91 qF A);q8
#define q81 0;) q3 3
#define q71 qP oJ oO
#define q61 w3 wD mU
#define q51 0]q9 oO
#define q41 "y x "
#define q31 ","mW
#define q21 w8","mM
#define q11 ", "wS","
#define q01 w8","wO
#define aZ g4==qR
#define aY "cNot"
#define aX "= "<<
#define aW wG" "
#define aV mN" "
#define aU qT hZ
#define aT q3 4;qQ
#define aS (y gP
#define aR " "mF
#define aQ (x);g8
#define aP }if(gG
#define aO cExp:oO
#define aN }gH}qM
#define aM "cSin"
#define aL "cCsc"
#define aK "cTan"
#define aJ "cCot"
#define aI "cSec"
#define aH "cCeil"
#define aG " "wS"\n"
#define aF "[-x] "
#define aE mX qQ
#define aD )){oO
#define aC hU oO
#define aB w7" "
#define aA x==h7 oO
#define a9 " "wH
#define a8 qK g9 oO
#define a7 qK hI oO
#define a6 hF oJ oO
#define a5 "cFloor"
#define a4 "cExp"
#define a3 ==o6){oO
#define a2 qB-1)&&x
#define a1 "cInv"
#define a0 qT h0
#define mZ gK Lpm;
#define mY gK Lnh;
#define mX q3 2;
#define mW " "mR
#define mV "cRDiv"
#define mU "cDiv"
#define mT wJ" "
#define mS "cLess"
#define mR "B"oP
#define mQ =qT;qG oO
#define mP "cMin"
#define mO "cMax"
#define mN "cSqr"
#define mM " "m8
#define mL o3 oO
#define mK wI" x "w7
#define mJ "cSqrt"
#define mI "B"qD1
#define mH a4" "
#define mG "[y*x]"
#define mF "cNeg"
#define mE mF" "
#define mD "cLog"
#define mC "cCos"
#define mB "cGreater"
#define mA w3 mV
#define m9 "A "wG oU
#define m8 "A"oP
#define m7 wB" "
#define m6 a0 q1 h2
#define m5 " cPow"
#define m4 " cSub"
#define m3 "cAbsNot"
#define m2 "cLessOrEq"
#define m1 "cNotNot"
#define m0 "[ y+x] "
#define wZ "x"m5
#define wY oX" "
#define wX mG m5
#define wW "cAbs"
#define wV "cLog2"
#define wU "cCosh"
#define wT "cExp2"
#define wS "x "aX x<<
#define wR " "aY
#define wQ "[-x]"w3
#define wP "cLog10"
#define wO " "oT
#define wN "y "aB
#define wM "cGreaterOrEq"
#define wL "cEqual"
#define wK o1==dR o4 oO
#define wJ "B[B==A]"
#define wI "cRSub"
#define wH "x"oU
#define wG "[x]"
#define wF "cDup "
#define wE "cNEqual"
#define wD "x[x!=Value_t(0)] "
#define wC oV oW
#define wB "A[A>=VarBegin]"
#define wA "B[B>=VarBegin]"m4" x "w7
#define w9 "x[x>Value_t(0)]"
#define w8 oY oQ
#define w7 "cAdd"
#define w6 wB oU
#define w5 "B[IsNeverNegativeValueOpcode(B)] "
#define w4 "x[IsIntegerConst(x)]"m5
#define w3 oU" "
#define w2 "x[x==Value_t(1)] "
#define w1 "B[IsBinaryOpcode(B)&&!HasInvalidRangesOpcode(B)] "oR
#define w0 "A[IsNeverNegativeValueOpcode(A)] "
#define oZ oY"with"mM
#define oY oV" "
#define oX "x[x==Value_t(0)]"
#define oW " "oQ"\n"
#define oV "   "
#define oU " cMul"
#define oT "y "aX y<<"\n"
#define oS "x A[IsComparisonOpcode(A)]"
#define oR "A[IsBinaryOpcode(A)&&!HasInvalidRangesOpcode(A)] "oX oU
#define oQ "with "wS
#define oP " "aX FP_TRACE_OPCODENAME
#define oO FP_TRACE_BYTECODE_OPTIMIZATION
#define oN cLog10
#define oM cTanh:
#define oL cSinh:
#define oK cAbs){
#define oJ dQ>qR
#define oI dQ!=qR
#define oH qQ A);
#define oG size()
#define oF for gC
#define oE B hW
#define oD d1 qO
#define oC Immed
#define oB Lao;
#define oA fp_pow(
#define o9 cRDiv:
#define o8 opcode
#define o7 qA gZ
#define o6 cDup
#define o5 gE qR
#define o4 ){hN q9
#define o3 o1 qH
#define o2 q6:x=qE;
#define o1 qI-1]
#define o0 gU o1==dT o4
#define dZ qC q6);
#define dY h8 g0
#define dX h8 q5
#define dW cNEqual
#define dV cLess:
#define dU cEqual
#define dT cRSub
#define dS qA g9
#define dR cMul
#define dQ if(x
#define dP h3 if(gD
#define dO qQ h2 q8
#define dN cPow:
#define dM gS qD q4
#define dL Default7
#define dK Default6
#define dJ Default3
#define dI g4==h7
#define dH Default1
#define dG qK gA 2]
#define dF fp_log(
#define dE cExp2
#define dD cNotNot
#define dC fp_log10(
#define dB gT oD
#define dA cAbs);
#define d9 fp_abs(x)
#define d8 gR 3]
#define d7 0.5)){
#define d6 q6:g5
#define d5 qL d6
#define d4 qL g9
#define d3 qF cAdd);
#define d2 cLessOrEq
#define d1 data->
#define d0 incStackPtr();g6
#define hZ ;if(
#define hY fp_log2(
#define hX qB 1)
#define hW ==A){
#define hV cAbs:
#define hU hM oE
#define hT hM B qX
#define hS qC q6 gK
#define hR cGreater
#define hQ Value_t
#define hP hQ>()
#define hO .pop_back(
#define hN qI-2]
#define hM B qV
#define hL Default5 qL
#define hK d8 hZ
#define hJ tmp--q4
#define hI cSqr:
#define hH qN gY hN
#define hG switch(
#define hF qL gA 1]q9
#define hE case
#define hD qW y+x;q3
#define hC TailCall_cSub
#define hB TailCall_cNeg
#define hA unsigned
#define h9 cGreaterOrEq
#define h8 q0 q7
#define h7 hX){
#define h6 GetDegreesToRadiansFactor gV
#define h5 GetRadiansToDegreesFactor gV
#define h4 =q0[0];
#define h3 g3 A h4
#define h2 dR);
#define h1 ,x gK oB
#define h0 =q6;
#define gZ o6:
#define gY cAdd:
#define gX TailCall_cNotNot
#define gW gK Lmm;
#define gV <hP){
#define gU if(A qX
#define gT d1 qZ
#define gS gR 2]h0 q0-=2;
#define gR q0[-
#define gQ qD>0;) q1
#define gP ,x gK Laq;
#define gO ,y gK Laq;
#define gN qE=-x;
#define gM isEvenInteger(
#define gL g7 hO);
#define gK );q5
#define gJ ,y gK oB
#define gI IsLogicalOpcode(A)){
#define gH break;
#define gG IsUnaryOpcode(
#define gF hJ q5
#define gE dQ==
#define gD IsAlwaysIntegerOpcode(A)){
#define gC (hA tmp=
#define gB oF 3;
#define gA dR:qI-
#define g9 cNeg:
#define g8 return;
#define g7 d1 ByteCode
#define g6 --StackPtr;q5
#define g5 y=q2[-1];
#define g4 qI 0]q9 dQ
#define g3 default:
#define g2 >=VarBegin&&g7.oG>
#define g1 hG gR 2]){hE
#define g0 o8=
#define qZ oC hO);
#define qY &&!HasInvalidRangesOpcode(
#define qX >=VarBegin){
#define qW q2[-1]=
#define qV =gR 2]hZ
#define qU hM IsNeverNegativeValueOpcode(B)){
#define qT gR 1]
#define qS FP_ReDefinePointers();q5
#define qR qB 0)){
#define qQ tmp-->0;) q1
#define qP qL o2
#define qO ByteCode hO gK
#define qN hG qT){hE
#define qM q5 Default0;
#define qL ;hE
#define qK }break qL
#define qJ }}qM
#define qI if(q0[
#define qH ==q6){g5
#define qG if(IsNeverNegativeValueOpcode(A)){
#define qF AddFunctionOpcode(
#define qE q2[0]
#define qD oF 2;tmp--
#define qC g7.push_back(
#define qB hQ(
#define qA hG q0[0]){hE
#define q9 ==q6){x=qE;
#define q8 d1 oC.push_back(
#define q7 -=1;gL
#define q6 cImmed
#define q5 goto
#define q4 >0;) gL
#define q3 gT oF
#define q2 ImmedPtr
#define q1 gL qF
#define q0 ByteCodePtr
hA*q0;hQ*q2;
#define FP_ReDefinePointers() q0=!g7.empty()?&g7[0]+g7.oG-1:0;q2=!d1 oC.empty()?&d1 oC[0]+d1 oC.oG-1:0;
FP_ReDefinePointers();
#if(!FP_FLOAT_VERSION)
hQ
x;hA
A;hQ
y;hA
B;hA
C;hA
D;hG
o8){TailCall_cAbs:hE
hV
dS
oO(236,mE
wW,wW,);q5
Laa
qP
oO(40,"x "
wW,"[fp_abs(x)]"
,wC);q5
Lab;h3
qG
oO(295,w0
wW,"A"
,oZ(A)<<"\n"
);g8
qJ
TailCall_cAdd:hE
gY
o7
hH
a3(186,wF
aB
wF
w7,"[Value_t(4)]"
oU,);q5
Lac;dG==cAdd){qI-3]a3(187,wF
w7
w3
wF
w7,"cMul [Value_t(4)]"
oU,);q5
Lad;}
}
gH}
q5
dH
hF
hN
a3(189,wF"x"
w3
w7,"[x+Value_t(1)]"
oU,wC);q5
Lae;}
}
q5
dH
d4
oO(243,mE
w7,"cSub"
,);q5
Laf
qP
hH
qH
oO(180,wN"x "
w7,"[y+x] "
w7,q01);q5
Lag;qK
dT:g1
gY
qI-3]qH
oO(121,wN
mK,m0
aB
wI,q01);q5
Lah;a8(122,mE
mK,aF
aB
wI,wC);q5
Lai
d5
oO(120,"y "
mK,m0
wI,q01);q5
Laj;qK
cSub:hT
hG
d8){hE
gY
qI-4]qH
oO(115,wN
wA,m0
aB"B"
m4,w8
q31(B)<<","
wO);q5
Lak;a8(116,mE
wA,aF
aB"B"
m4,w8
q31(B)<<"\n"
);q5
Lal
d5
oO(114,"y "
wA,m0"B"
m4,w8
q31(B)<<","
wO);q5
Lam;}
qK
d6
oO(76,q41
w7,"[y+x]"
,q01);q5
Lan;}
o5
oO(75,wY
w7,,wC);q5
oB}
gH
g3
dH:;A
h4
o0
oO(113,"x "
wI" "
m7
w7,"[DO_STACKPLUS1] A "
aW
aB
wI,oZ(A)<<","
aG);d0
Lap;}
oO(118,wI" "
m7
w7,"[DO_STACKPLUS1] A "
aB
wI,oZ(A)<<"\n"
);d0
Laq;}
qJ
TailCall_cAnd:hE
cAnd:qI
0]q9
mL(169,"y x cAnd"
,"[fp_and(x,y)]"
,q01);q5
Lba;qJ
TailCall_cDiv:hE
cDiv:o7
oO(159,wF
mU,"[Value_t(0)]"
w3"[Value_t(1)] "
w7,);q5
Lbb
qP
oI
qN
g9
oO(177,mE
wD
mU,aF
mU,wC);q5
Lbc
d5
oO(73,"y "
wD
mU,"[y/x]"
,q01);q5
Lbd;}
}
gE
h7
oO(72,w2
mU,,wC);q5
oB
aN
TailCall_cEqual:hE
dU:dI
A=aU
gI
oO(323,"A[IsLogicalOpcode(A)] "
w2
wL,"A"
,q21(A)<<"\n"
);q5
oB}
}
mL(167,q41
wL,"[fp_equal(y,x)]"
,q01);q5
Lbe;}
o5
qN
hV
oO(255,wW" "
wY
wL,aW
wL,wC);q5
Lbf
qL
hI
oO(257,aV
wY
wL,aW
wL,wC);q5
Lbf;}
oO(313,wY
wL,aY,wC);q5
Lbg;qJ
TailCall_cGreater:hE
hR:aZ
qN
hV
oO(315,wW" "
wY
mB,m1,wC);q5
Lbh;g3
A
mQ(319,w0
wY
mB,"A "
m1,q21(A)<<"\n"
);q5
Lbi;}
}
}
mL(165,q41
mB,"[fp_less(x,y)]"
,q01);q5
Lbj;qJ
TailCall_cGreaterOrEq:hE
h9:dI
qN
hV
oO(316,wW" "
w2
wM,m1,wC);q5
Lbh;g3
A
mQ(320,w0
w2
wM,"A "
m1,q21(A)<<"\n"
);q5
Lbi;}
}
}
mL(166,q41
wM,"[fp_lessOrEq(x,y)]"
,q01);q5
Lbk;qJ
TailCall_cInv:hE
cInv:qA
qC1(237,a1" "
a1,,);q5
Lbl
qL
cSqrt:oO(67,mJ" "
a1,"cRSqrt"
,);q5
Lbm
qP
oI
oO(69,wD
a1,"[Value_t(1)/x]"
,wC);q5
Lbn;aN
TailCall_cLess:hE
dV
aZ
A
mQ(124,w0
wY
mS,m9,q21(A)<<"\n"
);q5
Lbo;}
}
gE
h7
qN
hV
oO(317,wW" "
w2
mS,aY,wC);q5
Lbp;g3
A
mQ(321,w0
w2
mS,"A"
wR,q21(A)<<"\n"
);q5
Lbg;}
}
}
mL(163,q41
mS,"[fp_less(y,x)]"
,q01);q5
Lbq;qJ
TailCall_cLessOrEq:hE
d2:aZ
qN
hV
oO(318,wW" "
wY
m2,aY,wC);q5
Lbp;g3
A
mQ(322,w0
wY
m2,"A"
wR,q21(A)<<"\n"
);q5
Lbg;}
}
}
mL(164,q41
m2,"[fp_lessOrEq(y,x)]"
,q01);q5
Lca;qJ
TailCall_cMax:hE
cMax:o7
oO(149,wF
mO,,);q5
Lbl
qP
mL(82,q41
mO,"[fp_max(x,y)]"
,q01);q5
Lcb;}
gH
h3
gU
qN
gZ
aC(151,mT
wF
m7
mO,"B cDup"
,oZ(A)<<q31(B)<<"\n"
);q5
Lbl;qK
cMax:aC(153,mT
mO" "
m7
mO,"B "
mO,oZ(A)<<q31(B)<<"\n"
);q5
Lbl;}
gH}
qJ
TailCall_cMin:hE
cMin:o7
oO(148,wF
mP,,);q5
Lbl
qP
mL(81,q41
mP,"[fp_min(x,y)]"
,q01);q5
Lcc;}
gH
h3
gU
qN
gZ
aC(150,mT
wF
m7
mP,"B cDup"
,oZ(A)<<q31(B)<<"\n"
);q5
Lbl;qK
cMin:aC(152,mT
mP" "
m7
mP,"B "
mP,oZ(A)<<q31(B)<<"\n"
);q5
Lbl;}
gH}
qJ
TailCall_cMod:hE
cMod:g4!=qR
mL(74,"y "
wD"cMod"
,"[fp_mod(y,x)]"
,q01);q5
Lcd;}
qJ
TailCall_cMul:hE
dR:o7
oO(396,"cDup"
oU,mN,);q5
Lce
qL
qC1(242,a1
oU,mU,);q5
Lcf
d4
A=qT;gU
g1
dR:B=hK
oE
oO(388,wJ
w3
m7
mF
oU,"B "
mN
w3
mF,oZ(A)<<q31(B)<<"\n"
);q5
Lcg;}
q5
Default2;g3
Default2:;aC(382,mT
m7
mF
oU,"B "
mN
aR,oZ(A)<<q31(B)<<"\n"
);q5
Lch;}
}
}
q5
dJ
qL
dN
o1
q9
hN
a3(190,wF
wZ
oU,"[x+Value_t(1)]"
m5,wC);q5
Lci;}
}
q5
dJ
qP
o5
qN
dR:A
qV
A
qX
oO(133,wB
w3
oX
oU,wG
oU,q21(A)<<"\n"
);q5
Lcj;}
q5
Default4;g3
Default4:;A=aU
IsBinaryOpcode(A)
qY
A)){g1
d6
oO(132,"y "
oR,wG
oU,q21(A)<<","
wO);q5
Lck;g3
hM
IsBinaryOpcode(B)
qY
B)){hG
d8){hE
d6
oO(136,"y "
w1,m9,q21(A)<<q31(B)<<","
wO);q5
Lcl;g3
C=hK
C
qX
oO(134,"C[C>=VarBegin] "
w1,m9,q21(A)<<q31(B)<<qB1(C)<<"\n"
);q5
Lcm;aP
C)
qY
C
aD(135,"C[IsUnaryOpcode(C)&&!HasInvalidRangesOpcode(C)] "
w1,"B "
m9,q21(A)<<q31(B)<<qB1(C)<<"\n"
);q5
Lcn;}
}
}
if(B
qX
oO(130,"B[B>=VarBegin] "
oR,wG
oU,q21(A)<<q31(B)<<"\n"
);q5
Lcj;aP
B)
qY
B
aD(131,"B[IsUnaryOpcode(B)&&!HasInvalidRangesOpcode(B)] "
oR,m9,q21(A)<<q31(B)<<"\n"
);q5
Lco;}
}
}
gU
oO(128,m7
oX
oU,wG,q21(A)<<"\n"
);q5
Lcp;aP
A)
qY
A
aD(129,"A[IsUnaryOpcode(A)&&!HasInvalidRangesOpcode(A)] "
oX
oU,wG
oU,q21(A)<<"\n"
);q5
Lcq;}
}
}
qN
gY
g1
gZ
oO(194,wF
w7
a9,"[x+x]"
oU,wC);q5
Lda
qL
gA
3]qH
A=gR
4];gU
oO(282,m7"y"
w3
w7
a9,wG
w3"A "
mG
w3
w7,w8", y "
aX
y<<","
mM(A)<<"\n"
);q5
Ldb;}
qK
d6
oO(281,wN
wH,wG
w3
mG" "
w7,q01);q5
Ldc;qK
dR:g1
gY
qI-3]a3(196,wF
w7
w3
wH,"cMul [x+x]"
oU,wC);q5
Ldd;qK
d6
if(y*aA(184,"y[y*x==Value_t(1)]"
w3
wH,,q01);q5
Lde;}
oO(185,"y"
w3
wH,mG
oU,q01);q5
Ldf;a8(174,mE
wH,"[-x]"
oU,wC);q5
Ldg
qL
cSub:hN==dR){qI-3]qH
A=gR
4];gU
oO(283,m7"y"
w3"cSub"
a9,wG
w3"A "
mG
w3"cSub"
,w8", y "
aX
y<<","
mM(A)<<"\n"
);q5
Ldh;}
}
qK
d6
oO(71,"y"
a9,mG,q01);q5
Ldi;}
gE
h7
oO(70,"x[x==Value_t(1)]"
oU,,wC);q5
oB}
gE
qB
2
aD(139,"x[x==Value_t(2)]"
oU,wF
w7,wC);q5
Ldj;}
gE
qB-1
aD(176,"x[x==Value_t(-1)]"
oU,mF,wC);q5
Ldk;}
gH
g3
dJ:;A
h4
gU
qN
dR:g1
g9
B=hK
oE
oO(385,mT
mF
w3
w6,"B "
mN
w3
mF,oZ(A)<<q31(B)<<"\n"
);q5
Lcg;}
q5
Default5;g3
Default5:;aC(376,wJ
w3
w6,"B "
mN
oU,oZ(A)<<q31(B)<<"\n"
);q5
Ldl;}
}
q5
dK
d4
aC(379,mT
mE
w6,"B "
mN
aR,oZ(A)<<q31(B)<<"\n"
);q5
Lch;}
q5
dK;g3
dK:;B=aU
oE
oO(373,mT
w6,"B "
mN,oZ(A)<<q31(B)<<"\n"
);q5
Lce;}
}
aP
A)){B=aU
B
g2
1){hN==dR){C=hK
C
hW
D=gR
4]hZ
D==B){oO(395,"D[D==B] C[C==A]"
w3"B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]"
oU,"D C "
mN
oU,oZ(A)<<q31(B)<<qB1(C)<<", D"
oP(D)<<"\n"
);q5
Ldm;}
}
}
}
qJ
TailCall_cNEqual:hE
dW:dI
A=aU
gI
oO(324,"A[IsLogicalOpcode(A)] "
w2
wE,"A"
wR,q21(A)<<"\n"
);q5
Lbg;}
}
mL(168,q41
wE,"[fp_nequal(y,x)]"
,q01);q5
Ldn;}
o5
qN
hV
oO(256,wW" "
wY
wE,aW
wE,wC);q5
Ldo
qL
hI
oO(258,aV
wY
wE,aW
wE,wC);q5
Ldo;}
oO(314,wY
wE,m1,wC);q5
Lbi;qJ
hB:hE
g9
qA
gA
1]q9
oO(175,"x"
w3
mF,"[-x]"
oU,wC);q5
Ldp;a8(238,mE
mF,,);q5
Lbl
qP
oO(68,"x"
aR,"[-x]"
,wC);q5
Ldq;}
qM
TailCall_cNot:hE
cNot:qA
hV
oO(286,wW
wR,aY,);q5
Lea
qL
cAbsNot:A=aU
gI
oO(291,"A[IsLogicalOpcode(A)] "
m3
wR,"A"
,oZ(A)<<"\n"
);q5
Lbl;}
if(A!=q6){oO(292,"A[A!=cImmed] "
m3
wR,"A cAbsNotNot"
,oZ(A)<<"\n"
);q5
Leb;}
q5
dL
qL
cAbsNotNot:oO(289,"cAbsNotNot"
wR,m3,);q5
Lec
qL
gY
o1
q9
oO(326,"x "
w7
wR,aW
wL,wC);q5
Led;}
q5
dL
qL
dU:oO(252,wL
wR,wE,);q5
Lee
qL
hR:oO(250,mB
wR,m2,);q5
Lef
qL
h9:oO(251,wM
wR,mS,);q5
Leg
qL
dV
oO(248,mS
wR,wM,);q5
Leh
qL
d2:oO(249,m2
wR,mB,);q5
Lei
qL
dW:oO(253,wE
wR,wL,);q5
Lej
d4
oO(285,mF
wR,aY,);q5
Lea
qL
cNot:oO(287,aY
wR,m1,);q5
Lek
qL
dD:oO(288,m1
wR,aY,);q5
Lea
qP
oO(85,"x"
wR,"[fp_not(x)]"
,wC);q5
Lel;g3
dL:;A
h4
qG
oO(293,w0
aY,"A "
m3,oZ(A)<<"\n"
);q5
Lem;qJ
gX:hE
dD:qI
0]==cAdd){o1
q9
oO(325,"x "
aB
m1,aW
wE,wC);q5
Len;qJ
TailCall_cOr:hE
cOr:qI
0]q9
mL(170,"y x cOr"
,"[fp_or(x,y)]"
,q01);q5
Leo;qJ
TailCall_cRDiv:hE
o9
dI
oO(91,w2
mV,a1,wC);q5
Lep;qJ
TailCall_cSqr:hE
hI
qA
hV
oO(391,wW" "
mN,mN,);q5
Leq
d4
oO(390,mE
mN,mN,);q5
Leq;}
qM
hC:hE
cSub:o7
oO(191,wF"cSub"
,"[Value_t(0)]"
oU,);q5
Lfa
d4
oO(244,mF
m4,w7,);q5
Lfb
qP
o5
oO(77,wY"cSub"
,,wC);q5
oB}
mL(78,"y x"
m4,"[y-x]"
,q01);q5
Lfc;}
oO(207,"x"
m4,aF
w7,wC);q5
Lfd;h3
o0
oO(112,"x "
wI" "
wB
m4,"A "
aB
aW
wI,oZ(A)<<","
aG);q5
Lfe;}
oO(119,wI" "
wB
m4,"[DO_STACKPLUS1] A"
m4" "
wI,oZ(A)<<"\n"
);d0
Lff;}
qJ
g3
Default0:;A=o8
hZ
IsComparisonOpcode(A)){qI
0]q9
hH
qH
oO(260,wN
oS,"[x-y] A"
,oZ(A)<<q11
wO);q5
Lfg;a8(261,mE
oS,aF"{OppositeComparisonOpcode(A)}"
,oZ(A)<<","
aG);q5
Lfh;}
}
}
if(A
g2
0){B
h4
if(oE
oO(393,mT"A[A>=VarBegin&&data->ByteCode.size()>0]"
,"B cDup"
,oZ(A)<<q31(B)<<"\n"
);q5
Lfi;}
aP
A)){B
h4
if(B
g2
1){C=aU
C
hW
D
qV
D==B){oO(394,"D[D==B] C[C==A] B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]"
,"D C cDup"
,oZ(A)<<q31(B)<<qB1(C)<<", D"
oP(D)<<"\n"
);q5
Lfj;}
}
}
}
}
q5
Lfk;Laa:dX
TailCall_cAbs;Lab:qE=d9;g8
Lac:q8
qB
4));dM
Lfl:g0
dR;Lfm:FP_ReDefinePointers();Lfn:q5
TailCall_cMul;Lad:oF
4;dO
qB
4));hS
Lfl;Lae:qE=x+hX;dM
Lbo:g0
dR;q5
Lfn;Laf:dY
cSub;Lfo:q5
hC;Lag:hD
2;hJ
Lfp:FP_ReDefinePointers();Lfq:q5
TailCall_cAdd;Lah:hD
3;hJ
Lga:d3
Lgb:qF
dT);g8
Lai:gN
gR
2]h0
qD
q4
q5
Lga;Laj:hD
2;gF
Lgb;Lak:hD
4;hJ
Lgc:d3
Lgd:qF
B);Lge:g0
cSub;qS
Lfo;Lal:gN
d8
h0
gB
gF
Lgc;Lam:hD
3;gF
Lgd;Lan:qW
y+x;Lao:gT
Lbl:gL
g8
Lap:q3
3;oH
q8
x);hS
Lga;Laq:gQ
A
gK
Lga;Lba:qW
fp_and(x
gJ
Lbb:q8
qB
0));q0[0]h0
qF
h2
q8
hX);Lgf:dZ
Lgg:g0
cAdd;q5
Lfp;Lbc:gN
a0
h8
Lgh:q5
TailCall_cDiv;Lbd:qW
y/x;q5
oB
Lbe:qW
fp_equal(y
h1
Lbf:a0
h8
Lgi:q5
TailCall_cEqual;Lbg:gT
gL
Lgj:g0
cNot;FP_ReDefinePointers();Lgk:q5
TailCall_cNot;Lbh:mX
hJ
Lgl:g0
dD;FP_ReDefinePointers();Lgm:q5
gX;Lbi:dB
Lgl;Lbj:qW
fp_less(x
gJ
Lbk:qW
fp_lessOrEq(x
gJ
Lbm:q1
cRSqrt);g8
Lbn:qE=hX/x;g8
Lbp:mX
gF
Lgj;Lbq:qW
fp_less(y
h1
Lca:qW
fp_lessOrEq(y
h1
Lcb:qW
fp_max(x
gJ
Lcc:qW
fp_min(x
gJ
Lcd:qW
fp_mod(y
h1
Lce:dY
cSqr;Lgn:q5
TailCall_cSqr;Lcf:dY
cDiv;q5
Lgh;Lcg:gB
qQ
cSqr);qF
h2
Lgo:g0
cNeg;qS
hB;Lch:gQ
cSqr
gK
Lgo;Lci:qE=x+hX;gR
2]h0
gQ
cPow);g8
Lcj:dM
q5
Lfn;Lck:qW
x;Lgp:mX
gF
Lfm;Lcl:qD>0;)
gT
Lgq:oF
4;hJ
Lha:q91
x);Lhb:hS
Lfm;Lcm:gT
q5
Lgq;Lcn:aT
B
gK
Lha;Lco:q3
3;gF
Lha;Lcp:a0
q5
Lbl;Lcq:a0
dX
Lfn;Lda:qE=x+x;q5
Lcj;Ldb:qW
x;gR
4]h0
aT
h2
q91
y*x);dZ
qA1
Lgg;Ldc:qW
x;mX
dO
y*x
gK
Lgf;Ldd:q3
4;dO
x+x
gK
Lhb;Lde:qD>q81;hJ
g8
Ldf:qW
y*x;q5
Lgp;Ldg:gN
q5
Lcq;Ldh:qW
x;gR
4]h0
aT
h2
q91
y*x);dZ
qA1
Lge;Ldi:qW
y*x;q5
oB
Ldj:q0[0]=o6;q2-=1;gT
Lhc:g0
cAdd;q5
Lfq;Ldk:dB
Lgo;Ldl:qD
q4
Lhd:qF
cSqr
gK
Lfm;Ldm:gB
gF
Lhd;Ldn:qW
fp_nequal(y
h1
Ldo:a0
h8
Lhe:q5
TailCall_cNEqual;Ldp:gN
dX
Lbo;Ldq:gN
g8
Lea:dX
Lgk;Leb:q1
cAbsNotNot);g8
Lec:gL
Lem:qF
cAbsNot);g8
Led:qE=x;Lej:dY
dU;q5
Lgi;Lee:dY
dW;q5
Lhe;Lef:dY
d2;q5
TailCall_cLessOrEq;Leg:dY
cLess;q5
TailCall_cLess;Leh:dY
h9;q5
TailCall_cGreaterOrEq;Lei:dY
hR;q5
TailCall_cGreater;Lek:dY
dD;q5
Lgm;Lel:qE=fp_not
aQ
Len:qE=x;q5
Lee;Leo:qW
fp_or(x
gJ
Lep:gT
gL
g0
cInv;qS
TailCall_cInv;Leq:dX
Lgn;Lfa:q8
qB
0));q0[0]h0
q5
Lfl;Lfb:dX
Lhc;Lfc:qW
y-x;q5
oB
Lfd:gN
q5
Lhc;Lfe:q3
3;oH
d3
q8
x);hS
Lgb;Lff:gQ
A);qF
cSub
gK
Lgb;Lfg:qW
x-y;mX
oH
g8
Lfh:gN
a0
q1
OppositeComparisonOpcode(A));g8
Lfi:qC
o6);g8
Lfj:q0[0]=o6;g8
Lfk:qC
o8);g8
g8
q5
TailCall_cAnd;q5
TailCall_cMax;q5
TailCall_cMin;q5
TailCall_cMod;q5
hB;q5
gX;q5
TailCall_cOr;q5
TailCall_cRDiv;q5
hC;
#endif
#if(FP_FLOAT_VERSION)
hQ
x;hA
A;hQ
y;hA
B;hA
C;hA
D;hG
o8){TailCall_cAbs:hE
hV
dS
oO(236,mE
wW,wW,);q5
Laa
qP
oO(40,"x "
wW,"[fp_abs(x)]"
,wC);q5
Lab;h3
qG
oO(295,w0
wW,"A"
,oZ(A)<<"\n"
);g8
qJ
TailCall_cAcos:hE
cAcos:g4>=a2<=h7
oO(44,"x[x>=Value_t(-1)&&x<=Value_t(1)] cAcos"
,"[fp_acos(x)]"
,wC);q5
Lac;qJ
TailCall_cAcosh:hE
cAcosh:g4>=h7
oO(41,"x[x>=Value_t(1)] cAcosh"
,"[fp_acosh(x)]"
,wC);q5
Lad;qJ
TailCall_cAdd:hE
gY
o7
hH
a3(186,wF
aB
wF
w7,"[Value_t(4)]"
oU,);q5
Lae;dG==cAdd){qI-3]a3(187,wF
w7
w3
wF
w7,"cMul [Value_t(4)]"
oU,);q5
Laf;}
}
gH}
q5
dH
hF
hN
a3(189,wF"x"
w3
w7,"[x+Value_t(1)]"
oU,wC);q5
Lag;}
}
q5
dH
d4
oO(243,mE
w7,"cSub"
,);q5
Lah
qP
hH
qH
oO(180,wN"x "
w7,"[y+x] "
w7,q01);q5
Lai;qK
dT:g1
gY
qI-3]qH
oO(121,wN
mK,m0
aB
wI,q01);q5
Laj;a8(122,mE
mK,aF
aB
wI,wC);q5
Lak
d5
oO(120,"y "
mK,m0
wI,q01);q5
Lal;qK
cSub:hT
hG
d8){hE
gY
qI-4]qH
oO(115,wN
wA,m0
aB"B"
m4,w8
q31(B)<<","
wO);q5
Lam;a8(116,mE
wA,aF
aB"B"
m4,w8
q31(B)<<"\n"
);q5
Lan
d5
oO(114,"y "
wA,m0"B"
m4,w8
q31(B)<<","
wO);q5
oB}
qK
d6
oO(76,q41
w7,"[y+x]"
,q01);q5
Lap;}
o5
oO(75,wY
w7,,wC);q5
Laq;}
gH
g3
dH:;A
h4
o0
oO(113,"x "
wI" "
m7
w7,"[DO_STACKPLUS1] A "
aW
aB
wI,oZ(A)<<","
aG);d0
Lba;}
oO(118,wI" "
m7
w7,"[DO_STACKPLUS1] A "
aB
wI,oZ(A)<<"\n"
);d0
Lbb;}
qJ
TailCall_cAnd:hE
cAnd:qI
0]q9
mL(169,"y x cAnd"
,"[fp_and(x,y)]"
,q01);q5
Lbc;qJ
TailCall_cAsin:hE
cAsin:g4>=a2<=h7
oO(45,"x[x>=Value_t(-1)&&x<=Value_t(1)] cAsin"
,"[fp_asin(x)]"
,wC);q5
Lbd;qJ
TailCall_cAsinh:hE
cAsinh:qI
q51(42,"x cAsinh"
,"[fp_asinh(x)]"
,wC);q5
Lbe;}
qM
TailCall_cAtan:hE
cAtan:qI
q51(46,"x cAtan"
,"[fp_atan(x)]"
,wC);q5
Lbf;}
qM
TailCall_cAtan2:hE
cAtan2:qI
0]q9
mL(83,"y x cAtan2"
,"[fp_atan2(y,x)]"
,q01);q5
Lbg;qJ
TailCall_cAtanh:hE
cAtanh:g4>a2<h7
oO(43,"x[x>Value_t(-1)&&x<Value_t(1)] cAtanh"
,"[fp_atanh(x)]"
,wC);q5
Lbh;qJ
TailCall_cCbrt:hE
cCbrt:qI
q51(47,"x cCbrt"
,"[fp_cbrt(x)]"
,wC);q5
Lbi;}
qM
TailCall_cCeil:hE
cCeil:dS
oO(304,mE
aH,a5
aR,);q5
Lbj
qP
oO(48,"x "
aH,"[fp_ceil(x)]"
,wC);q5
Lbk;dP
oO(298,"A[IsAlwaysIntegerOpcode(A)] "
aH,"A"
,oZ(A)<<"\n"
);g8
qJ
TailCall_cCos:hE
cCos:qA
hV
oO(339,wW" "
mC,mC,);q5
Lbl
d4
oO(341,mE
mC,mC,);q5
Lbl
qP
oO(49,"x "
mC,"[fp_cos(x)]"
,wC);q5
Lbm;}
qM
TailCall_cCosh:hE
cCosh:qA
hV
oO(340,wW" "
wU,wU,);q5
Lbn
qL
cAcosh:oO(232,"cAcosh "
wU,,);q5
Lbo
qL
cAsinh:oO(353,"cAsinh "
wU,"[DO_STACKPLUS1] "
aV"[Value_t(1)] "
aB
mJ,);d0
Lbp
d4
oO(342,mE
wU,wU,);q5
Lbn
qP
oO(50,"x "
wU,"[fp_cosh(x)]"
,wC);q5
Lbq;}
qM
TailCall_cDeg:hE
cDeg:qI
q51(65,"x cDeg"
,"[RadiansToDegrees(x)]"
,wC);q5
Lca;}
qM
TailCall_cDiv:hE
cDiv:qA
cCos:oO(356,mC
qD1,aI
oU,);q5
Lcb
qL
cCot:oO(360,aJ
qD1,aK
oU,);q5
Lcc
qL
cCsc:oO(358,aL
qD1,aM
oU,);q5
Lcd
qL
gZ
oO(159,wF
mU,"[Value_t(0)]"
w3"[Value_t(1)] "
w7,);q5
Lce
qL
aO(310,mH
mU,mE
a4
oU,);q5
Lcf
qL
dE:oO(311,wT
qD1,mE
wT
oU,);q5
Lcg
qL
qC1(241,a1
qD1,"cMul"
,);q5
Lch
qL
dN
oO(309,"cPow"
qD1,mF
m5
oU,);q5
Lci
qL
cSec:oO(359,aI
qD1,mC
oU,);q5
Lcj
qL
cSin:oO(355,aM
qD1,aL
oU,);q5
Lck
qL
cTan:oO(357,aK
qD1,aJ
oU,);q5
Lcl
qP
gE
h7
oO(72,w2
mU,,wC);q5
Laq;}
oI
qN
gA
2]qH
if((y/x)==h5
oO(198,"y[(y/x)==GetRadiansToDegreesFactor<Value_t>()]"
q61,"cDeg"
,q01);q5
Lcm;}
if((y/x)==h6
oO(199,"y[(y/x)==GetDegreesToRadiansFactor<Value_t>()]"
q61,"cRad"
,q01);q5
Lcn;}
oO(200,"y"
q61,"[y/x]"
oU,q01);q5
Lco;a8(177,mE
wD
mU,aF
mU,wC);q5
Lcp
d5
oO(73,"y "
wD
mU,"[y/x]"
,q01);q5
Lcq;}
oO(202,wD
mU,"[Value_t(1)/x]"
oU,wC);q5
Lda;}
gH
h3
gU
qN
cDiv:hT
oO(94,qF1" "
m7
mU,"[DO_STACKPLUS1] B A"
w3
mU,oZ(A)<<q31(B)<<"\n"
);d0
Ldb;qK
o9
g1
o2
oO(89,"x "
mV" "
m7
mU,"A"
w3
aW
mV,oZ(A)<<","
aG);q5
Ldc;g3
hT
oO(88,"B[B>=VarBegin] "
mV" "
m7
mU,"A"
w3"B "
mV,oZ(A)<<q31(B)<<"\n"
);q5
Ldd;}
}
gH}
qJ
TailCall_cEqual:hE
dU:aZ
qN
hV
oO(255,wW" "
wY
wL,aW
wL,wC);q5
Lde
qL
hI
oO(257,aV
wY
wL,aW
wL,wC);q5
Lde;}
}
mL(167,q41
wL,"[fp_equal(y,x)]"
,q01);q5
Ldf;qJ
TailCall_cExp:hE
cExp:qA
gY
o1
q9
oO(306,"x "
aB
a4,mH"[fp_exp(x)]"
oU,wC);q5
Ldg;qK
cLog:A
mQ(222,w0
mD" "
a4,"A"
,oZ(A)<<"\n"
);q5
Lbo;qK
o2
oO(51,"x "
a4,"[fp_exp(x)]"
,wC);q5
Ldh;}
qM
TailCall_cExp2:hE
dE:qA
gY
o1
q9
oO(307,"x "
aB
wT,wT" [fp_exp2(x)]"
oU,wC);q5
Ldi;qK
cLog2:A
mQ(223,w0
wV" "
wT,"A"
,oZ(A)<<"\n"
);q5
Lbo;qK
o2
oO(52,"x "
wT,"[fp_exp2(x)]"
,wC);q5
Ldj;}
oO(399,wT,"[DO_STACKPLUS1] [fp_log(Value_t(2))]"
w3
a4,);d0
Ldk;TailCall_cFloor:hE
cFloor:dS
oO(303,mE
a5,aH
aR,);q5
Ldl
qP
oO(53,"x "
a5,"[fp_floor(x)]"
,wC);q5
Ldm;dP
oO(297,"A[IsAlwaysIntegerOpcode(A)] "
a5,"A"
,oZ(A)<<"\n"
);g8
qJ
TailCall_cGreater:hE
hR:qI
0]q9
mL(165,q41
mB,"[fp_less(x,y)]"
,q01);q5
Ldn;}
gE
qB-d7
oO(333,"x[x==Value_t(-0.5)] "
mB,mE
m3,wC);q5
Ldo;qJ
TailCall_cGreaterOrEq:hE
h9:g4!=qR
o1==oK
oO(329,wW" "
wD
wM,"[Value_t(0.5)/x]"
w3
m1,wC);q5
Ldp;}
}
mL(166,q41
wM,"[fp_lessOrEq(x,y)]"
,q01);q5
Ldq;}
gE
qB
d7
oO(332,"x[x==Value_t(0.5)] "
wM,"cAbsNotNot"
,wC);q5
Lea;qJ
TailCall_cInt:hE
cInt:qA
o2
oO(54,"x cInt"
,"[fp_int(x)]"
,wC);q5
Leb;dP
oO(299,"A[IsAlwaysIntegerOpcode(A)] cInt"
,"A"
,oZ(A)<<"\n"
);g8
qJ
TailCall_cInv:hE
cInv:qA
cCos:oO(362,mC" "
a1,aI,);q5
Lec
qL
cCot:oO(366,aJ" "
a1,aK,);q5
Led
qL
cCsc:oO(364,aL" "
a1,aM,);q5
Lee
qL
qC1(237,a1" "
a1,,);q5
Lbo
qL
dN
oO(239,"cPow "
a1,mF
m5,);q5
Lef
qL
cSec:oO(365,aI" "
a1,mC,);q5
Leg
qL
cSin:oO(361,aM" "
a1,aL,);q5
Leh
qL
cSqrt:oO(67,mJ" "
a1,"cRSqrt"
,);q5
Lei
qL
cTan:oO(363,aK" "
a1,aJ,);q5
Lej
qP
oI
oO(69,wD
a1,"[Value_t(1)/x]"
,wC);q5
Lek;aN
TailCall_cLess:hE
dV
aZ
A
mQ(124,w0
wY
mS,m9,q21(A)<<"\n"
);q5
Lel;}
}
oI
o1==oK
oO(328,wW" "
wD
mS,"[Value_t(0.5)/x]"
w3
aY,wC);q5
Lem;}
}
mL(163,q41
mS,"[fp_less(y,x)]"
,q01);q5
Len;}
gE
qB
d7
oO(331,"x[x==Value_t(0.5)] "
mS,m3,wC);q5
Leo;qJ
TailCall_cLessOrEq:hE
d2:qI
0]q9
mL(164,q41
m2,"[fp_lessOrEq(y,x)]"
,q01);q5
Lep;}
gE
qB-d7
oO(334,"x[x==Value_t(-0.5)] "
m2,mE"cAbsNotNot"
,wC);q5
Leq;qJ
TailCall_cLog:hE
cLog:qA
aO(225,mH
mD,,);q5
Lbo
a6(411,w9
w3
mD,mD" [fp_log(x)] "
w7,wC);q5
Lfa;}
a7(142,aV
mD,wW" "
mD" "
wF
w7,);q5
Lfb
q71(55,w9" "
mD,"[fp_log(x)]"
,wC);q5
Lfc;aN
TailCall_cLog10:hE
oN:qA
aO(401,mH
wP,"[DO_STACKPLUS1] [fp_log10(fp_const_e<Value_t>())]"
oU,);d0
Lfd
a6(412,w9
w3
wP,wP" [fp_log10(x)] "
w7,wC);q5
Lfe;}
a7(144,aV
wP,wW" "
wP" "
wF
w7,);q5
Lff
q71(56,w9" "
wP,"[fp_log10(x)]"
,wC);q5
Lfg;aN
TailCall_cLog2:hE
cLog2:qA
aO(400,mH
wV,"[DO_STACKPLUS1] [fp_log2(fp_const_e<Value_t>())]"
oU,);d0
Lfh
qL
dE:oO(226,wT" "
wV,,);q5
Lbo
a6(410,w9
w3
wV,wV" [fp_log2(x)] "
w7,wC);q5
Lfi;}
a7(143,aV
wV,wW" "
wV" "
wF
w7,);q5
Lfj
q71(57,w9" "
wV,"[fp_log2(x)]"
,wC);q5
Lfk;aN
TailCall_cMax:hE
cMax:o7
oO(149,wF
mO,,);q5
Lbo
qP
mL(82,q41
mO,"[fp_max(x,y)]"
,q01);q5
Lfl;}
gH
h3
gU
qN
gZ
aC(151,mT
wF
m7
mO,"B cDup"
,oZ(A)<<q31(B)<<"\n"
);q5
Lbo;qK
cMax:aC(153,mT
mO" "
m7
mO,"B "
mO,oZ(A)<<q31(B)<<"\n"
);q5
Lbo;}
gH}
qJ
TailCall_cMin:hE
cMin:o7
oO(148,wF
mP,,);q5
Lbo
qP
mL(81,q41
mP,"[fp_min(x,y)]"
,q01);q5
Lfm;}
gH
h3
gU
qN
gZ
aC(150,mT
wF
m7
mP,"B cDup"
,oZ(A)<<q31(B)<<"\n"
);q5
Lbo;qK
cMin:aC(152,mT
mP" "
m7
mP,"B "
mP,oZ(A)<<q31(B)<<"\n"
);q5
Lbo;}
gH}
qJ
TailCall_cMod:hE
cMod:g4!=qR
mL(74,"y "
wD"cMod"
,"[fp_mod(y,x)]"
,q01);q5
Lfn;}
qJ
TailCall_cMul:hE
dR:o7
oO(396,"cDup"
oU,mN,);q5
Lfo
qL
qC1(242,a1
oU,mU,);q5
Lfp
d4
A=qT;gU
g1
dR:B=hK
oE
oO(388,wJ
w3
m7
mF
oU,"B "
mN
w3
mF,oZ(A)<<q31(B)<<"\n"
);q5
Lfq;}
q5
Default2;g3
Default2:;aC(382,mT
m7
mF
oU,"B "
mN
aR,oZ(A)<<q31(B)<<"\n"
);q5
Lga;}
}
}
q5
dJ
qL
dN
o1
q9
hN
a3(190,wF
wZ
oU,"[x+Value_t(1)]"
m5,wC);q5
Lgb;}
}
q5
dJ
qP
o5
qN
dR:A
qV
A
qX
oO(133,wB
w3
oX
oU,wG
oU,q21(A)<<"\n"
);q5
Lgc;}
q5
Default4;g3
Default4:;A=aU
IsBinaryOpcode(A)
qY
A)){g1
d6
oO(132,"y "
oR,wG
oU,q21(A)<<","
wO);q5
Lgd;g3
hM
IsBinaryOpcode(B)
qY
B)){hG
d8){hE
d6
oO(136,"y "
w1,m9,q21(A)<<q31(B)<<","
wO);q5
Lge;g3
C=hK
C
qX
oO(134,"C[C>=VarBegin] "
w1,m9,q21(A)<<q31(B)<<qB1(C)<<"\n"
);q5
Lgf;aP
C)
qY
C
aD(135,"C[IsUnaryOpcode(C)&&!HasInvalidRangesOpcode(C)] "
w1,"B "
m9,q21(A)<<q31(B)<<qB1(C)<<"\n"
);q5
Lgg;}
}
}
if(B
qX
oO(130,"B[B>=VarBegin] "
oR,wG
oU,q21(A)<<q31(B)<<"\n"
);q5
Lgc;aP
B)
qY
B
aD(131,"B[IsUnaryOpcode(B)&&!HasInvalidRangesOpcode(B)] "
oR,m9,q21(A)<<q31(B)<<"\n"
);q5
Lgh;}
}
}
gU
oO(128,m7
oX
oU,wG,q21(A)<<"\n"
);q5
Lgi;aP
A)
qY
A
aD(129,"A[IsUnaryOpcode(A)&&!HasInvalidRangesOpcode(A)] "
oX
oU,wG
oU,q21(A)<<"\n"
);q5
Lgj;}
}
}
qN
gY
g1
gZ
dQ+aA(193,"cDup[x+x==Value_t(1)] "
w7
a9,,wC);q5
Lgk;}
oO(194,wF
w7
a9,"[x+x]"
oU,wC);q5
Lgl
qL
gA
3]qH
A=gR
4];gU
oO(282,m7"y"
w3
w7
a9,wG
w3"A "
mG
w3
w7,w8", y "
aX
y<<","
mM(A)<<"\n"
);q5
Lgm;}
qK
d6
oO(281,wN
wH,wG
w3
mG" "
w7,q01);q5
Lgn;qK
cDeg:oO(145,"cDeg"
a9,"[RadiansToDegrees(x)]"
oU,wC);q5
Lgo
qL
cDiv:hT
hG
d8){hE
gA
4]qH
oO(101,"y"
w3
qF1
a9,mG
w3
mI,w8
q31(B)<<","
wO);q5
Lgp;a8(102,mE
qF1
a9,wQ
mI,w8
q31(B)<<"\n"
);q5
Lgq
d5
oO(100,"y B[B>=VarBegin]"
qD1
a9,mG" "
mI,w8
q31(B)<<","
wO);q5
Lha;}
qK
dR:g1
gY
qI-3]==o6){dQ+aA(195,"cDup[x+x==Value_t(1)] "
w7
w3
wH,"cMul"
,wC);q5
Lhb;}
oO(196,wF
w7
w3
wH,"cMul [x+x]"
oU,wC);q5
Lhc;qK
d6
if((y*x)==h5
oO(182,"y[(y*x)==GetRadiansToDegreesFactor<Value_t>()]"
w3
wH,"cDeg"
,q01);q5
Lcm;}
if((y*x)==h6
oO(183,"y[(y*x)==GetDegreesToRadiansFactor<Value_t>()]"
w3
wH,"cRad"
,q01);q5
Lcn;}
if(y*aA(184,"y[y*x==Value_t(1)]"
w3
wH,,q01);q5
Lhd;}
oO(185,"y"
w3
wH,mG
oU,q01);q5
Lhe;a8(174,mE
wH,"[-x]"
oU,wC);q5
Lhf
qL
o9
g1
gA
3]qH
oO(108,"y"
mA
a9,mG
mA,q01);q5
Lhg;a8(109,mE
mV
a9,wQ
mV,wC);q5
Lhh
d5
oO(107,"y "
mV
a9,mG" "
mV,q01);q5
Lhi;qK
cRad:oO(146,"cRad"
a9,"[DegreesToRadians(x)]"
oU,wC);q5
Lhj
qL
cSub:hN==dR){qI-3]qH
A=gR
4];gU
oO(283,m7"y"
w3"cSub"
a9,wG
w3"A "
mG
w3"cSub"
,w8", y "
aX
y<<","
mM(A)<<"\n"
);q5
Lhk;}
}
qK
d6
oO(71,"y"
a9,mG,q01);q5
Lhl;}
gE
h7
oO(70,"x[x==Value_t(1)]"
oU,,wC);q5
Laq;}
gE
qB
2
aD(139,"x[x==Value_t(2)]"
oU,wF
w7,wC);q5
Lhm;}
gE
h5
oO(140,"x[x==GetRadiansToDegreesFactor<Value_t>()]"
oU,"cDeg"
,wC);q5
Lhn;}
gE
h6
oO(141,"x[x==GetDegreesToRadiansFactor<Value_t>()]"
oU,"cRad"
,wC);q5
Lho;}
gE
qB-1
aD(176,"x[x==Value_t(-1)]"
oU,mF,wC);q5
Lhp;}
gH
g3
dJ:;A
h4
gU
qN
cDiv:hT
oO(97,qF1" "
w6,"[DO_STACKPLUS1] A"
w3
mI,oZ(A)<<q31(B)<<"\n"
);d0
Lhq;}
q5
hL
dR:g1
g9
B=hK
oE
oO(385,mT
mF
w3
w6,"B "
mN
w3
mF,oZ(A)<<q31(B)<<"\n"
);q5
Lfq;}
q5
dK;g3
dK:;aC(376,wJ
w3
w6,"B "
mN
oU,oZ(A)<<q31(B)<<"\n"
);q5
Lia;}
}
q5
hL
g9
aC(379,mT
mE
w6,"B "
mN
aR,oZ(A)<<q31(B)<<"\n"
);q5
Lga;}
q5
hL
o9
hN
q9
oO(90,"x "
mV" "
w6,"[DO_STACKPLUS1] A "
wG
mA,oZ(A)<<","
aG);d0
Lib;}
oO(104,mV" "
w6,"[DO_STACKPLUS1] A"
mA,oZ(A)<<"\n"
);d0
Lic;g3
Default5:;B=aU
oE
oO(373,mT
w6,"B "
mN,oZ(A)<<q31(B)<<"\n"
);q5
Lfo;}
}
aP
A)){B=aU
B
g2
1){hN==dR){C=hK
C
hW
D=gR
4]hZ
D==B){oO(395,"D[D==B] C[C==A]"
w3"B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]"
oU,"D C "
mN
oU,oZ(A)<<q31(B)<<qB1(C)<<", D"
oP(D)<<"\n"
);q5
Lid;}
}
}
}
qJ
TailCall_cNEqual:hE
dW:aZ
qN
hV
oO(256,wW" "
wY
wE,aW
wE,wC);q5
Lie
qL
hI
oO(258,aV
wY
wE,aW
wE,wC);q5
Lie;}
}
mL(168,q41
wE,"[fp_nequal(y,x)]"
,q01);q5
Lif;qJ
hB:hE
g9
qA
gA
1]q9
oO(175,"x"
w3
mF,"[-x]"
oU,wC);q5
Lig;a8(238,mE
mF,,);q5
Lbo
qL
cSin:wK(347,"x"
w3
aM
aR,wQ
aM,wC);q5
Lih;}
qK
oL
wK(348,"x"
w3"cSinh"
aR,wQ"cSinh"
,wC);q5
Lii;}
qK
cTan:wK(349,"x"
w3
aK
aR,wQ
aK,wC);q5
Lij;}
qK
oM
wK(350,"x"
w3"cTanh"
aR,wQ"cTanh"
,wC);q5
Lik;}
qK
o2
oO(68,"x"
aR,"[-x]"
,wC);q5
Lil;}
qM
TailCall_cNot:hE
cNot:qA
hV
oO(286,wW
wR,aY,);q5
Lim
qL
cAbsNot:A=aU
gI
oO(291,"A[IsLogicalOpcode(A)] "
m3
wR,"A"
,oZ(A)<<"\n"
);q5
Lbo;}
if(A!=q6){oO(292,"A[A!=cImmed] "
m3
wR,"A cAbsNotNot"
,oZ(A)<<"\n"
);q5
Lin;}
q5
dL
qL
cAbsNotNot:oO(289,"cAbsNotNot"
wR,m3,);q5
Lio
qL
dU:oO(252,wL
wR,wE,);q5
Lip
qL
hR:oO(250,mB
wR,m2,);q5
Liq
qL
h9:oO(251,wM
wR,mS,);q5
Lja
qL
dV
oO(248,mS
wR,wM,);q5
Ljb
qL
d2:oO(249,m2
wR,mB,);q5
Ljc
qL
dW:oO(253,wE
wR,wL,);q5
Ljd
d4
oO(285,mF
wR,aY,);q5
Lim
qL
cNot:oO(287,aY
wR,m1,);q5
Lje
qL
dD:oO(288,m1
wR,aY,);q5
Lim
qP
oO(85,"x"
wR,"[fp_not(x)]"
,wC);q5
Ljf;g3
dL:;A
h4
qG
oO(293,w0
aY,"A "
m3,oZ(A)<<"\n"
);q5
Ljg;qJ
TailCall_cOr:hE
cOr:qI
0]q9
mL(170,"y x cOr"
,"[fp_or(x,y)]"
,q01);q5
Ljh;qJ
TailCall_cPow:hE
dN
qI
0]q9
if(!gM
x+x)){o1==cSqr){oO(14,aV"x[!isEvenInteger(x+x)]"
m5,wW" [x+x]"
m5,wC);q5
Lji;}
}
if(IsIntegerConst(x)){qN
aO(35,mH
w4,wG
w3
a4,wC);q5
Ljj
qL
dE:oO(36,wT" "
w4,wG
w3
wT,wC);q5
Ljk
qL
dN
hN
qH
if(!IsIntegerConst(y
aD(34,"y[!IsIntegerConst(y)]"
m5" "
w4,wX,q01);q5
Ljl;}
}
oO(37,"cPow "
w4,wG
w3"cPow"
,wC);q5
Ljm;}
}
if(gM
x)){qN
hV
oO(336,wW" x[isEvenInteger(x)]"
m5,wG
m5,wC);q5
Ljn
qL
gA
2]==oK
oO(337,wW
w3"x[isEvenInteger(x)]"
m5,"cMul "
wG
m5,wC);q5
Ljo;}
gH}
}
o5
oO(161,wY"cPow"
,"[Value_t(0)]"
w3"[Value_t(1)] "
w7,wC);q5
Ljp;}
gE
qB
d7
oO(214,"x[x==Value_t(0.5)]"
m5,mJ,wC);q5
Ljq;}
gE
hX/qB
3
aD(215,"x[x==Value_t(1)/Value_t(3)]"
m5,"cCbrt"
,wC);q5
Lka;}
gE
hX/qB-3
aD(216,"x[x==Value_t(1)/Value_t(-3)]"
m5,"cCbrt "
a1,wC);q5
Lkb;}
gE
qB-d7
oO(217,"x[x==Value_t(-0.5)]"
m5,"cRSqrt"
,wC);q5
Lkc;}
gE
qB-1
aD(218,"x[x==Value_t(-1)]"
m5,a1,wC);q5
Lkd;}
qN
dN
hN
qH
if(gM
y)&&!gM
x*y
aD(13,"y[isEvenInteger(y)&&!isEvenInteger(x*y)]"
m5" "
wZ,wW" "
wX,q01);q5
Lke;}
oO(212,"y"
m5" "
wZ,wX,q01);q5
Ljl;a7(38,aV
wZ,"[x+x]"
m5,wC);q5
Lkf
d5
if(y!=qB
0)||x>=qR
oO(84,"y[y!=Value_t(0)||x>=Value_t(0)] "
wZ,"[fp_pow(y,x)]"
,q01);q5
Lkg;}
gH}
oO(370,wZ,"[DO_POWI]"
,wC)
hZ
TryCompilePowi(x))
g8}
qM
TailCall_cRDiv:hE
o9
dI
oO(91,w2
mV,a1,wC);q5
Lkd;qJ
TailCall_cRad:hE
cRad:qI
q51(66,"x cRad"
,"[DegreesToRadians(x)]"
,wC);q5
Lkh;}
qM
TailCall_cSin:hE
cSin:dS
oO(343,mE
aM,aM
aR,);q5
Lki
qP
oO(58,"x "
aM,"[fp_sin(x)]"
,wC);q5
Lkj;}
qM
TailCall_cSinh:hE
oL
qA
cAcosh:oO(352,"cAcosh cSinh"
,"[DO_STACKPLUS1] "
aV"[Value_t(-1)] "
aB
mJ,);d0
Lkk
qL
cAsinh:oO(231,"cAsinh cSinh"
,,);q5
Lbo
d4
oO(344,mE"cSinh"
,"cSinh"
aR,);q5
Lkl
qP
oO(59,"x cSinh"
,"[fp_sinh(x)]"
,wC);q5
Lkm;}
qM
TailCall_cSqr:hE
hI
qA
hV
oO(391,wW" "
mN,mN,);q5
Lkn
d4
oO(390,mE
mN,mN,);q5
Lkn
qL
cSqrt:A
mQ(220,w0
mJ" "
mN,"A"
,oZ(A)<<"\n"
);q5
Lbo;aN
TailCall_cSqrt:hE
cSqrt:qA
hI
oO(15,aV
mJ,wW,);q5
Lko
qP
dQ>=qR
oO(60,"x[x>=Value_t(0)] "
mJ,"[fp_sqrt(x)]"
,wC);q5
Lkp;aN
hC:hE
cSub:o7
oO(191,wF"cSub"
,"[Value_t(0)]"
oU,);q5
Lkq
d4
oO(244,mF
m4,w7,);q5
Lla
qP
o5
oO(77,wY"cSub"
,,wC);q5
Laq;}
mL(78,"y x"
m4,"[y-x]"
,q01);q5
Llb;}
oO(207,"x"
m4,aF
w7,wC);q5
Llc;h3
o0
oO(112,"x "
wI" "
wB
m4,"A "
aB
aW
wI,oZ(A)<<","
aG);q5
Lld;}
oO(119,wI" "
wB
m4,"[DO_STACKPLUS1] A"
m4" "
wI,oZ(A)<<"\n"
);d0
Lle;}
qJ
TailCall_cTan:hE
cTan:qA
cAtan2:oO(235,"cAtan2 "
aK,mU,);q5
Lfp
d4
oO(345,mE
aK,aK
aR,);q5
Llf
qP
oO(62,"x "
aK,"[fp_tan(x)]"
,wC);q5
Llg;}
qM
TailCall_cTanh:hE
oM
dS
oO(346,mE"cTanh"
,"cTanh"
aR,);q5
Llh
qP
oO(63,"x cTanh"
,"[fp_tanh(x)]"
,wC);q5
Lli;}
qM
TailCall_cTrunc:hE
cTrunc:qA
o2
oO(64,"x cTrunc"
,"[fp_trunc(x)]"
,wC);q5
Llj;dP
oO(296,"A[IsAlwaysIntegerOpcode(A)] cTrunc"
,"A"
,oZ(A)<<"\n"
);g8
qJ
g3
Default0:;A=o8
hZ
IsComparisonOpcode(A)){qI
0]q9
hH
qH
oO(260,wN
oS,"[x-y] A"
,oZ(A)<<q11
wO);q5
Llk;qK
cAtan:if(d9<fp_const_pi<hP*qB
d7
oO(276,"cAtan[fp_abs(x)<fp_const_pi<Value_t>()*Value_t(0.5)]"
qE1,"[fp_tan(x)] A"
,oZ(A)<<","
aG);q5
Lll;qK
cExp:oJ
oO(266,"cExp[x>Value_t(0)]"
qE1,"[fp_log(x)] A"
,oZ(A)<<","
aG);q5
Llm;qK
dE:oJ
oO(267,"cExp2[x>Value_t(0)]"
qE1,"[fp_log2(x)] A"
,oZ(A)<<","
aG);q5
Lln;qK
cLog:qU
oO(269,w5
mD
qE1,"B [fp_exp(x)] A"
,oZ(A)<<q11
mW(B)<<"\n"
);q5
Llo;qK
oN:qU
oO(271,w5
wP
qE1,"B [fp_pow(Value_t(10),x)] A"
,oZ(A)<<q11
mW(B)<<"\n"
);q5
Llp;qK
cLog2:qU
oO(270,w5
wV
qE1,"B [fp_exp2(x)] A"
,oZ(A)<<q11
mW(B)<<"\n"
);q5
Llq;dG
qH
if(y>qR
oO(262,"y[y>Value_t(0)]"
w3
oS,"[x/y] A"
,oZ(A)<<q11
wO);q5
Lma;}
if(y<qR
oO(263,"y[y<Value_t(0)]"
w3
oS,"[x/y] {OppositeComparisonOpcode(A)}"
,oZ(A)<<q11
wO);q5
Lmb;}
a8(261,mE
oS,aF"{OppositeComparisonOpcode(A)}"
,oZ(A)<<","
aG);q5
Lmc
qL
dN
oJ
hN
qH
if(y>qR
oO(264,"y[y>Value_t(0)] cPow[x>Value_t(0)]"
qE1,"[fp_pow(x,1/y)] A"
,oZ(A)<<q11
wO);q5
Lmd;}
}
qK
oL
oO(277,"cSinh"
qE1,"[fp_asinh(x)] A"
,oZ(A)<<","
aG);q5
Lme
qL
hI
oJ
oO(265,"cSqr[x>Value_t(0)]"
qE1,wW" [fp_sqrt(x)] A"
,oZ(A)<<","
aG);q5
Lmf;qK
oM
if(d9<h7
oO(278,"cTanh[fp_abs(x)<Value_t(1)]"
qE1,"[fp_atanh(x)] A"
,oZ(A)<<","
aG);q5
Lmg;}
gH}
}
}
if(A
g2
0){B
h4
if(oE
oO(393,mT"A[A>=VarBegin&&data->ByteCode.size()>0]"
,"B cDup"
,oZ(A)<<q31(B)<<"\n"
);q5
Lmh;}
aP
A)){B
h4
if(B
g2
1){C=aU
C
hW
D
qV
D==B){oO(394,"D[D==B] C[C==A] B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]"
,"D C cDup"
,oZ(A)<<q31(B)<<qB1(C)<<", D"
oP(D)<<"\n"
);q5
Lmi;}
}
}
}
}
q5
Lmj;Laa:h8
Lmk:q5
TailCall_cAbs;Lab:qE=d9;g8
Lac:qE=fp_acos
aQ
Lad:qE=fp_acosh
aQ
Lae:q8
qB
4));gS
Lml:qD
q4
Lmm:g0
dR;Lmn:FP_ReDefinePointers();Lmo:q5
TailCall_cMul;Laf:oF
4;dO
qB
4));Lmp:qC
q6
gW
Lag:qE=x+hX;dM
Lel:g0
dR;q5
Lmo;Lah:dY
cSub;Lmq:q5
hC;Lai:hD
2;hJ
Lna:FP_ReDefinePointers();Lnb:q5
TailCall_cAdd;Laj:hD
3;hJ
Lnc:d3
Lnd:qF
dT);g8
Lak:gN
gR
2]h0
qD
q4
q5
Lnc;Lal:hD
2;gF
Lnd;Lam:hD
4;hJ
Lne:d3
Lnf:qF
B);Lng:g0
cSub;qS
Lmq;Lan:gN
d8
h0
gB
gF
Lne;Lao:hD
3;gF
Lnf;Lap:qW
y+x;Laq:gT
Lbo:gL
g8
Lba:q3
3;oH
q8
x);hS
Lnc;Lbb:gQ
A
gK
Lnc;Lbc:qW
fp_and(x
gO
Lbd:qE=fp_asin
aQ
Lbe:qE=fp_asinh
aQ
Lbf:qE=fp_atan
aQ
Lbg:qW
fp_atan2
aS
Lbh:qE=fp_atanh
aQ
Lbi:qE=fp_cbrt
aQ
Lbj:q1
cFloor);Lnh:g0
cNeg;qS
hB;Lbk:qE=fp_ceil
aQ
Lbl:h8
Lni:q5
TailCall_cCos;Lbm:qE=fp_cos
aQ
Lbn:dX
TailCall_cCosh;Lbp:q1
cSqr);q8
hX);Lnj:dZ
d3
Lnk:g0
cSqrt;qS
TailCall_cSqrt;Lbq:qE=fp_cosh
aQ
Lca:qE=RadiansToDegrees
aQ
Lcb:q1
cSec
gW
Lcc:q1
cTan
gW
Lcd:q1
cSin
gW
Lce:q8
qB
0));q0[0]h0
Lnl:qF
h2
q8
hX);Lnm:dZ
Lnn:g0
cAdd;q5
Lna;Lcf:q1
cNeg);qF
cExp
gW
Lcg:q1
cNeg);qF
dE
gW
Lch:dX
Lel;Lci:q1
cNeg);qF
cPow
gW
Lcj:q1
cCos
gW
Lck:q1
cCsc
gW
Lcl:q1
cCot
gW
Lcm:qD>q81;hJ
Lno:g0
cDeg;qS
TailCall_cDeg;Lcn:qD>q81;hJ
Lnp:g0
cRad;qS
TailCall_cRad;Lco:qW
y/x;gT
q5
Lml;Lcp:gN
a0
h8
Lnq:q5
TailCall_cDiv;Lcq:qW
y/x;q5
Laq;Lda:qE=hX/x;q5
Lel;Ldb:gQ
A);qF
h2
Loa:qS
Lnq;Ldc:q3
3;oH
qF
h2
q8
x);dZ
Lob:g0
cRDiv;qS
TailCall_cRDiv;Ldd:gB
oH
qF
h2
qF
B
gK
Lob;Lde:a0
h8
Loc:q5
TailCall_cEqual;Ldf:qW
fp_equal
aS
Ldg:aE
cExp);q8
fp_exp(x)
gK
Lmp;Ldh:qE=fp_exp
aQ
Ldi:aE
dE);q8
fp_exp2(x)
gK
Lmp;Ldj:qE=fp_exp2
aQ
Ldk:q8
dF
qB
2)));dZ
Lod:qF
h2
g0
cExp;qS
TailCall_cExp;Ldl:q1
cCeil
mY
Ldm:qE=fp_floor
aQ
Ldn:qW
fp_less(x
gO
Ldo:gT
q1
cNeg);Ljg:qF
cAbsNot);g8
Ldp:qE=qB
0.5)/x;m6
Loe:qF
dD);g8
Ldq:qW
fp_lessOrEq(x
gO
Lea:gT
Lin:gL
Lof:qF
cAbsNotNot);g8
Leb:qE=fp_int
aQ
Lec:q1
cSec);g8
Led:dY
cTan;Log:q5
TailCall_cTan;Lee:dY
cSin;Loh:q5
TailCall_cSin;Lef:q1
cNeg);g0
cPow;Loi:FP_ReDefinePointers();Loj:q5
TailCall_cPow;Leg:dY
cCos;q5
Lni;Leh:q1
cCsc);g8
Lei:q1
cRSqrt);g8
Lej:q1
cCot);g8
Lek:qE=hX/x;g8
Lem:qE=qB
0.5)/x;m6
g0
cNot;FP_ReDefinePointers();Lok:q5
TailCall_cNot;Len:qW
fp_less
aS
Leo:gT
Lio:oD
Ljg;Lep:qW
fp_lessOrEq
aS
Leq:gT
q1
cNeg
gK
Lof;Lfa:aE
cLog);q8
dF
x)
gK
Lnm;Lfb:q1
dA
qF
cLog);Lol:qC
o6
gK
Lnn;Lfc:qE=dF
x);g8
Lfd:q8
dC
fp_const_e<hP));Lom:q0[0]h0
q5
Lmm;Lfe:aE
oN);q8
dC
x)
gK
Lnm;Lff:q1
dA
qF
oN
gK
Lol;Lfg:qE=dC
x);g8
Lfh:q8
hY
fp_const_e<hP)
gK
Lom;Lfi:aE
cLog2);q8
hY
x)
gK
Lnm;Lfj:q1
dA
qF
cLog2
gK
Lol;Lfk:qE=hY
x);g8
Lfl:qW
fp_max(x
gO
Lfm:qW
fp_min(x
gO
Lfn:qW
fp_mod
aS
Lfo:dY
cSqr;Lon:q5
TailCall_cSqr;Lfp:dY
cDiv;q5
Lnq;Lfq:gB
qQ
cSqr);qF
dR
mY
Lga:gQ
cSqr
mY
Lgb:qE=x+hX;dM
g0
cPow;q5
Loj;Lgc:dM
q5
Lmo;Lgd:qW
x;Loo:mX
gF
Lmn;Lge:qD>0;)
gT
Lop:oF
4;hJ
Loq:q91
x);Lpa:hS
Lmn;Lgf:gT
q5
Lop;Lgg:aT
B
gK
Loq;Lgh:q3
3;gF
Loq;Lgi:a0
q5
Lbo;Lgj:a0
dX
Lmo;Lgk:gT
Lpb:gB
hJ
g8
Lgl:qE=x+x;q5
Lgc;Lgm:qW
x;gR
4]h0
aT
h2
q91
y*x);dZ
qA1
Lnn;Lgn:qW
x;mX
dO
y*x
gK
Lnm;Lgo:qE=RadiansToDegrees(x
gK
Lgj;Lgp:qW
y*x;q3
4;hJ
Lpc:qF
h2
Lpd:qF
B);g0
cDiv;q5
Loa;Lgq:gN
d8
h0
gB
gF
Lpc;Lha:qW
y*x;q3
3;gF
Lpd;Lhb:q3
4;gF
Lmn;Lhc:q3
4;dO
x+x
gK
Lpa;Lhd:qD>0;)
gT
q5
Lpb;Lhe:qW
y*x;q5
Loo;Lhf:gN
q5
Lgj;Lhg:qW
y*x;q3
3;hJ
Lpe:qA1
Lob;Lhh:gN
gR
2]h0
qD
q4
q5
Lpe;Lhi:qW
y*x;mX
gF
Lob;Lhj:qE=DegreesToRadians(x
gK
Lgj;Lhk:qW
x;gR
4]h0
aT
h2
q91
y*x);dZ
qA1
Lng;Lhl:qW
y*x;q5
Laq;Lhm:q0[0]=o6;q2-=1;gT
Lpf:g0
cAdd;q5
Lnb;Lhn:dB
Lno;Lho:dB
Lnp;Lhp:dB
Lnh;Lhq:gB
qQ
A
gK
Lpc;Lia:qD
q4
Lpg:qF
cSqr
gK
Lmn;Lib:q3
3;oH
q8
x);hS
Lpe;Lic:gQ
A
gK
Lpe;Lid:gB
gF
Lpg;Lie:a0
h8
Lph:q5
TailCall_cNEqual;Lif:qW
fp_nequal
aS
Lig:gN
q5
Lch;Lih:gN
gQ
h2
g0
cSin;qS
Loh;Lii:gN
gQ
h2
g0
cSinh;qS
TailCall_cSinh;Lij:gN
gQ
h2
g0
cTan;qS
Log;Lik:gN
gQ
h2
g0
cTanh;qS
TailCall_cTanh;Lil:gN
g8
Lim:dX
Lok;Lip:dY
dW;q5
Lph;Liq:dY
d2;q5
TailCall_cLessOrEq;Lja:dY
cLess;q5
TailCall_cLess;Ljb:dY
h9;q5
TailCall_cGreaterOrEq;Ljc:dY
hR;q5
TailCall_cGreater;Ljd:dY
dU;q5
Loc;Lje:oD
Loe;Ljf:qE=fp_not
aQ
Ljh:qW
fp_or(x
gO
Lji:aE
dA
q8
x+x);Lpi:hS
Loi;Ljj:a0
oD
Lod;Ljk:m6
g0
dE;qS
TailCall_cExp2;Ljl:qW
y*x;mX
gF
Loi;Ljm:a0
q1
dR
gK
Loi;Ljn:a0
dX
Loj;Ljo:q3
3;dO
x
gK
Lpi;Ljp:qE=qB
0
gK
Lnl;Ljq:dB
Lnk;Lka:gT
gL
g0
cCbrt;qS
TailCall_cCbrt;Lkb:gT
q1
cCbrt);Lpj:g0
cInv;qS
TailCall_cInv;Lkc:gT
q5
Lei;Lkd:dB
Lpj;Lke:qD>q81;qQ
dA
q8
y*x
gK
Lpi;Lkf:qE=x+x;q5
Ljn;Lkg:qW
oA
y
gP
Lkh:qE=DegreesToRadians
aQ
Lki:q1
cSin
mY
Lkj:qE=fp_sin
aQ
Lkk:q1
cSqr);q8
qB-1)
gK
Lnj;Lkl:q1
cSinh
mY
Lkm:qE=fp_sinh
aQ
Lkn:dX
Lon;Lko:dY
cAbs;q5
Lmk;Lkp:qE=fp_sqrt
aQ
Lkq:q8
qB
0)
gK
Lom;Lla:dX
Lpf;Llb:qW
y-x;q5
Laq;Llc:gN
q5
Lpf;Lld:q3
3;oH
d3
q8
x);hS
Lnd;Lle:gQ
A);qF
cSub
gK
Lnd;Llf:q1
cTan
mY
Llg:qE=fp_tan
aQ
Llh:q1
cTanh
mY
Lli:qE=fp_tanh
aQ
Llj:qE=fp_trunc
aQ
Llk:qW
x-y;Lpk:mX
hJ
Lpl:qF
A);g8
Lll:qE=fp_tan(x);Lpm:a0
oD
Lpl;Llm:qE=dF
x
mZ
Lln:qE=hY
x
mZ
Llo:qE=fp_exp(x
mZ
Llp:qE=oA
qB
10),x
mZ
Llq:qE=fp_exp2(x
mZ
Lma:qW
x/y;q5
Lpk;Lmb:qW
x/y;mX
hJ
Lpn:qF
OppositeComparisonOpcode(A));g8
Lmc:gN
a0
oD
Lpn;Lmd:qW
oA
x,1/y
gK
Lpk;Lme:qE=fp_asinh(x
mZ
Lmf:aE
dA
q8
fp_sqrt(x));hS
Lpl;Lmg:qE=fp_atanh(x
mZ
Lmh:qC
o6);g8
Lmi:q0[0]=o6;g8
Lmj:qC
o8);g8
g8
q5
TailCall_cAcos;q5
TailCall_cAcosh;q5
TailCall_cAnd;q5
TailCall_cAsin;q5
TailCall_cAsinh;q5
TailCall_cAtan;q5
TailCall_cAtan2;q5
TailCall_cAtanh;q5
TailCall_cCeil;q5
TailCall_cFloor;q5
TailCall_cInt;q5
TailCall_cLog;q5
TailCall_cLog10;q5
TailCall_cLog2;q5
TailCall_cMax;q5
TailCall_cMin;q5
TailCall_cMod;q5
TailCall_cOr;q5
TailCall_cRDiv;q5
TailCall_cRad;q5
TailCall_cSin;q5
TailCall_cSinh;q5
TailCall_cSqrt;q5
hC;q5
TailCall_cTan;q5
TailCall_cTanh;q5
TailCall_cTrunc;
#endif
#undef FP_ReDefinePointers
#undef FP_TRACE_BYTECODE_OPTIMIZATION
#undef FP_TRACE_OPCODENAME
