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
#define qI1 cInv:oJ
#define qH1 m7","a9
#define qG1 "cSub"
#define qF1 q41 a9
#define qE1 qG A);qC
#define qD1 <<q21 a9
#define qC1 m4 oN" "
#define qB1 qQ oC oJ
#define qA1 q8 g8 oJ
#define q91 qL o4 oJ
#define q81 qG dB gJ
#define q71 <<q21 aA
#define q61 q41 wM
#define q51 <<", C"m6
#define q41 <<","
#define q31 m7","aB
#define q21 ", "oL","
#define q11 "[DO_STACKPLUS1] "
#define q01 m7","aA
#define aZ g5==qR
#define aY "cNot"
#define aX "[x] "
#define aW "= "<<
#define aV qR oJ
#define aU w8" "
#define aT h7 oJ
#define aS qA q1
#define aR w7 w0
#define aQ cExp:oJ
#define aP }if(gE
#define aO "y "
#define aN "y"oN" "
#define aM }gF}qM
#define aL "cSin"
#define aK "cCsc"
#define aJ "cTan"
#define aI "cCot"
#define aH "cSec"
#define aG "cCeil"
#define aF ]q7 oJ
#define aE "[-x] "
#define aD )){oJ
#define aC hV oJ
#define aB " "w2
#define aA " "oO
#define a9 " "a7
#define a8 qK g8 oJ
#define a7 "B"m6
#define a6 w0" "
#define a5 o0 d6 qC
#define a4 x==aT
#define a3 q8 gX oJ
#define a2 qK hH oJ
#define a1 w1 oN" "
#define a0 hE oC oJ
#define mZ "cFloor"
#define mY "A"oN" "
#define mX "cExp"
#define mW qL g8 oJ
#define mV ==dY){oJ
#define mU q9-1)&&x
#define mT "A "wA
#define mS "cInv"
#define mR "cDiv"
#define mQ "B cSub"
#define mP "cRDiv"
#define mO wD" "
#define mN "cLess"
#define mM dT oJ
#define mL mP wN
#define mK "cMin"
#define mJ "cMax"
#define mI "cSqr "
#define mH " "oQ
#define mG ]q7 mM
#define mF oS" "
#define mE wC" x "w0
#define mD "cSqrt"
#define mC "B "mR
#define mB qU;qH oJ
#define mA qU;if(gH oJ
#define m9 mX" "
#define m8 oM w2
#define m7 oM oL
#define m6 " "oK
#define m5 "B cSqr"
#define m4 "[y*x]"
#define m3 "cNeg"
#define m2 m3" "
#define m1 qU=q6;
#define m0 "cLog"
#define wZ "cCos"
#define wY "cGreater"
#define wX " cPow"
#define wW "cAbsNot"
#define wV "cLessOrEq"
#define wU "cNotNot"
#define wT " "aY
#define wS "[ y+x] "
#define wR "x"wX
#define wQ m4 wX
#define wP "cAbs"
#define wO "cLog2"
#define wN " "w8
#define wM " "oL"\n"
#define wL q6:x=qE;oJ
#define wK "cExp2"
#define wJ "cCosh"
#define wI "cLog10"
#define wH aO a6
#define wG "cGreaterOrEq"
#define wF "cEqual"
#define wE dS==dB dU oJ
#define wD "B[B==A]"
#define wC "cRSub"
#define wB "[-x]"oN" "
#define wA "[x]"oN
#define w9 x);g7
#define w8 "x"oN
#define w7 "cDup "
#define w6 "cNEqual"
#define w5 "x[x!=Value_t(0)] "
#define w4 "B[B>=VarBegin] cSub x "w0
#define w3 "A[A>=VarBegin] "
#define w2 "A"m6
#define w1 "x[x>Value_t(0)]"
#define w0 "cAdd"
#define oZ "B[IsNeverNegativeValueOpcode(B)] "
#define oY "x[IsIntegerConst(x)]"wX
#define oX "x[x==Value_t(1)] "
#define oW "A[A>=VarBegin]"oN
#define oV <<"\n"
#define oU "B[IsBinaryOpcode(B)&&!HasInvalidRangesOpcode(B)] "oP
#define oT "A[IsNeverNegativeValueOpcode(A)] "
#define oS "x[x==Value_t(0)]"
#define oR m7"\n"
#define oQ "x A[IsComparisonOpcode(A)]"
#define oP "A[IsBinaryOpcode(A)&&!HasInvalidRangesOpcode(A)] "oS oN
#define oO aO aW y oV
#define oN " cMul"
#define oM "    with "
#define oL "x "aW x<<
#define oK aW FP_TRACE_OPCODENAME
#define oJ FP_TRACE_BYTECODE_OPTIMIZATION
#define oI cLog10
#define oH cTanh:
#define oG cSinh:
#define oF opcode
#define oE gP 4];
#define oD cAbs){
#define oC dI>qR
#define oB dI!=qR
#define oA for gA
#define o9 B hW
#define o8 Immed
#define o7 Lao;
#define o6 fp_pow(
#define o5 h0 gS qN
#define o4 q6:g2
#define o3 q1 A gJ
#define o2 cRDiv:
#define o1 gR 1;qP
#define o0 qB q1
#define dZ 4 qB qP
#define dY cDup
#define dX dB);gG
#define dW cPow:
#define dV gC qR
#define dU ){hJ q7
#define dT dS qF
#define dS qI-1]
#define dR gS dS==dL dU
#define dQ qD q6);
#define dP q0[0]=
#define dO cNEqual
#define dN cLess:
#define dM cEqual
#define dL cRSub
#define dK .size()
#define dJ 0.5)){
#define dI if(x
#define dH h0 if(gB
#define dG Default7
#define dF Default6
#define dE Default3
#define dD Default1
#define dC qK g9 2]
#define dB cMul
#define dA fp_log(
#define d9 cExp2
#define d8 cNotNot
#define d7 fp_log10(
#define d6 cAbs);
#define d5 gP 3]
#define d4 fp_abs(x)
#define d3 qP q4
#define d2 o0 A);
#define d1 qG cAdd);
#define d0 cLessOrEq
#define hZ fp_log2(
#define hY q9 1)
#define hX incStackPtr();qZ
#define hW ==A){
#define hV hM o9
#define hU hM B qY
#define hT q0 qT
#define hS qD q6 gJ
#define hR cGreater
#define hQ Value_t
#define hP cAbs:
#define hO q3 dZ
#define hN hQ>()
#define hM B qX
#define hL Default5 qL
#define hK d5;if(
#define hJ qI-2]
#define hI qV y+x;
#define hH cSqr:
#define hG qN gW hJ
#define hF switch(
#define hE qL g9 1]q7
#define hD q3 3 qB qP
#define hC case
#define hB TailCall_cSub
#define hA TailCall_cNeg
#define h9 unsigned
#define h8 data qO
#define h7 hY){
#define h6 g5==h7
#define h5 cGreaterOrEq
#define h4 q0-=1;d3
#define h3 GetDegreesToRadiansFactor gT
#define h2 GetRadiansToDegreesFactor gT
#define h1 =q0[0];
#define h0 g4 A h1
#define gZ ,x gJ o7
#define gY q3 2 qB qP
#define gX dY:
#define gW cAdd:
#define gV TailCall_cNotNot
#define gU gJ Lmm;
#define gT <hN){
#define gS if(A qY
#define gR =q6;q0-=
#define gQ gP 2]gR 2;
#define gP q0[-
#define gO ,x gJ Laq;
#define gN ,y gJ Laq;
#define gM qE=-x;
#define gL isEvenInteger(
#define gK o0 dB);
#define gJ );q4
#define gI ,y gJ o7
#define gH IsLogicalOpcode(A)){
#define gG oF=
#define gF break;
#define gE IsUnaryOpcode(
#define gD data->ByteCode
#define gC dI==
#define gB IsAlwaysIntegerOpcode(A)){
#define gA (h9 tmp=
#define g9 dB:qI-
#define g8 cNeg:
#define g7 return;
#define g6 oA 3 qB
#define g5 qI 0]q7 dI
#define g4 default:
#define g3 >=VarBegin&&gD dK>
#define g2 y=q2[-1];
#define g1 hF gP 2]){hC
#define g0 &&!HasInvalidRangesOpcode(
#define qZ --StackPtr;q4
#define qY >=VarBegin){
#define qX =gP 2];if(
#define qW hM IsNeverNegativeValueOpcode(B)){
#define qV q2[-1]=
#define qU gP 1]
#define qT -=1;qP gG
#define qS FP_ReDefinePointers();q4
#define qR q9 0)){
#define qQ qL q6:x=qE;
#define qP data->q5
#define qO ->o8.pop_back();
#define qN hF qU){hC
#define qM q4 Default0;
#define qL ;hC
#define qK }break qL
#define qJ }}qM
#define qI if(q0[
#define qH if(IsNeverNegativeValueOpcode(A)){
#define qG AddFunctionOpcode(
#define qF ==q6){g2
#define qE q2[0]
#define qD gD.push_back(
#define qC data->o8.push_back(
#define qB ;tmp-->0;)
#define qA oA 2 qB
#define q9 hQ(
#define q8 hF q0[0]){hC
#define q7 ==q6){x=qE;
#define q6 cImmed
#define q5 ByteCode.pop_back();
#define q4 goto
#define q3 h8 oA
#define q2 ImmedPtr
#define q1 qP qG
#define q0 ByteCodePtr
h9*q0;hQ*q2;
#define FP_ReDefinePointers() q0=!gD.empty()?&gD[0]+gD dK-1:0;q2=!data->o8.empty()?&data->o8[0]+data->o8 dK-1:0;
FP_ReDefinePointers();
#if(!FP_FLOAT_VERSION)
hQ
x;h9
A;hQ
y;h9
B;h9
C;h9
D;hF
oF){TailCall_cAbs:hC
hP
qA1(236,m2
wP,wP,);q4
Laa
qQ
oJ(40,"x "
wP,"[fp_abs(x)]"
,oR);q4
Lab;h0
qH
oJ(295,oT
wP,"A"
,m8(A)oV);g7
qJ
TailCall_cAdd:hC
gW
q8
gX
hG
mV(186,w7
a6
aR,"[Value_t(4)]"
oN,);q4
Lac;dC==cAdd){qI-3]mV(187,aR
oN" "
aR,"cMul [Value_t(4)]"
oN,);q4
Lad;}
}
gF}
q4
dD
hE
hJ
mV(189,w7
aU
w0,"[x+Value_t(1)]"
oN,oR);q4
Lae;}
}
q4
dD
mW(243,m2
w0,qG1,);q4
Laf
qQ
hG
qF
oJ(180,wH"x "
w0,"[y+x] "
w0,q01);q4
Lag;qK
dL:g1
gW
qI-3]qF
oJ(121,wH
mE,wS
a6
wC,q01);q4
Lah;a8(122,m2
mE,aE
a6
wC,oR);q4
Lai
q91(120,aO
mE,wS
wC,q01);q4
Laj;qK
cSub:hU
hF
d5){hC
gW
qI-4]qF
oJ(115,wH
w4,wS
a6
mQ,qH1(B)q41
aA);q4
Lak;a8(116,m2
w4,aE
a6
mQ,qH1(B)oV);q4
Lal
q91(114,aO
w4,wS
mQ,qH1(B)q41
aA);q4
Lam;}
qK
o4
oJ(76,aO"x "
w0,"[y+x]"
,q01);q4
Lan;}
dV
oJ(75,mF
w0,,oR);q4
o7}
gF
g4
dD:;A
h1
dR
oJ(113,"x "
wC" "
w3
w0,"[DO_STACKPLUS1] A "
aX
a6
wC,m8(A)q61);hX
Lap;}
oJ(118,wC" "
w3
w0,"[DO_STACKPLUS1] A "
a6
wC,m8(A)oV);hX
Laq;}
qJ
TailCall_cAnd:hC
cAnd:qI
0
mG(169,aO"x cAnd"
,"[fp_and(x,y)]"
,q01);q4
Lba;qJ
TailCall_cDiv:hC
cDiv:a3(159,w7
mR,"[Value_t(0)]"
oN" [Value_t(1)] "
w0,);q4
Lbb
qQ
oB
qN
g8
oJ(177,m2
w5
mR,aE
mR,oR);q4
Lbc
q91(73,aO
w5
mR,"[y/x]"
,q01);q4
Lbd;}
}
gC
aT(72,oX
mR,,oR);q4
o7
aM
TailCall_cEqual:hC
dM:h6
A=mA(323,"A[IsLogicalOpcode(A)] "
oX
wF,"A"
,q31(A)oV);q4
o7}
}
mM(167,aO"x "
wF,"[fp_equal(y,x)]"
,q01);q4
Lbe;}
dV
qN
hP
oJ(255,wP" "
mF
wF,aX
wF,oR);q4
Lbf
qL
hH
oJ(257,mI
mF
wF,aX
wF,oR);q4
Lbf;}
oJ(313,mF
wF,aY,oR);q4
Lbg;qJ
TailCall_cGreater:hC
hR:aZ
qN
hP
oJ(315,wP" "
mF
wY,wU,oR);q4
Lbh;g4
A=mB(319,oT
mF
wY,"A "
wU,q31(A)oV);q4
Lbi;}
}
}
mM(165,aO"x "
wY,"[fp_less(x,y)]"
,q01);q4
Lbj;qJ
TailCall_cGreaterOrEq:hC
h5:h6
qN
hP
oJ(316,wP" "
oX
wG,wU,oR);q4
Lbh;g4
A=mB(320,oT
oX
wG,"A "
wU,q31(A)oV);q4
Lbi;}
}
}
mM(166,aO"x "
wG,"[fp_lessOrEq(x,y)]"
,q01);q4
Lbk;qJ
TailCall_cInv:hC
cInv:q8
qI1(237,mS" "
mS,,);q4
Lbl
qL
cSqrt:oJ(67,mD" "
mS,"cRSqrt"
,);q4
Lbm
qQ
oB
oJ(69,w5
mS,"[Value_t(1)/x]"
,oR);q4
Lbn;aM
TailCall_cLess:hC
dN
aZ
A=mB(124,oT
mF
mN,mT,q31(A)oV);q4
Lbo;}
}
gC
h7
qN
hP
oJ(317,wP" "
oX
mN,aY,oR);q4
Lbp;g4
A=mB(321,oT
oX
mN,"A"
wT,q31(A)oV);q4
Lbg;}
}
}
mM(163,aO"x "
mN,"[fp_less(y,x)]"
,q01);q4
Lbq;qJ
TailCall_cLessOrEq:hC
d0:aZ
qN
hP
oJ(318,wP" "
mF
wV,aY,oR);q4
Lbp;g4
A=mB(322,oT
mF
wV,"A"
wT,q31(A)oV);q4
Lbg;}
}
}
mM(164,aO"x "
wV,"[fp_lessOrEq(y,x)]"
,q01);q4
Lca;qJ
TailCall_cMax:hC
cMax:a3(149,w7
mJ,,);q4
Lbl
qQ
mM(82,aO"x "
mJ,"[fp_max(x,y)]"
,q01);q4
Lcb;}
gF
o5
gX
aC(151,mO
w7
w3
mJ,"B cDup"
,m8(A)qF1(B)oV);q4
Lbl;qK
cMax:aC(153,mO
mJ" "
w3
mJ,"B "
mJ,m8(A)qF1(B)oV);q4
Lbl;}
gF}
qJ
TailCall_cMin:hC
cMin:a3(148,w7
mK,,);q4
Lbl
qQ
mM(81,aO"x "
mK,"[fp_min(x,y)]"
,q01);q4
Lcc;}
gF
o5
gX
aC(150,mO
w7
w3
mK,"B cDup"
,m8(A)qF1(B)oV);q4
Lbl;qK
cMin:aC(152,mO
mK" "
w3
mK,"B "
mK,m8(A)qF1(B)oV);q4
Lbl;}
gF}
qJ
TailCall_cMod:hC
cMod:g5!=qR
mM(74,aO
w5"cMod"
,"[fp_mod(y,x)]"
,q01);q4
Lcd;}
qJ
TailCall_cMul:hC
dB:a3(396,"cDup"
oN,"cSqr"
,);q4
Lce
qL
qI1(242,mS
oN,mR,);q4
Lcf
qL
g8
A=qU;gS
g1
dB:B=hK
o9
oJ(388,wD
oN" "
w3
m3
oN,m5
oN" "
m3,m8(A)qF1(B)oV);q4
Lcg;}
q4
Default2;g4
Default2:;aC(382,mO
w3
m3
oN,m5" "
m3,m8(A)qF1(B)oV);q4
Lch;}
}
}
q4
dE
qL
dW
dS
q7
hJ
mV(190,w7
wR
oN,"[x+Value_t(1)]"
wX,oR);q4
Lci;}
}
q4
dE
qQ
dV
qN
dB:A
qX
A
qY
oJ(133,oW" "
oS
oN,wA,q31(A)oV);q4
Lcj;}
q4
Default4;g4
Default4:;A=qU;if(IsBinaryOpcode(A)g0
A)){g1
o4
oJ(132,aO
oP,wA,q31(A)q41
aA);q4
Lck;g4
hM
IsBinaryOpcode(B)g0
B)){hF
d5){hC
o4
oJ(136,aO
oU,mT,q31(A)qF1(B)q41
aA);q4
Lcl;g4
C=hK
C
qY
oJ(134,"C[C>=VarBegin] "
oU,mT,q31(A)qF1(B)q51(C)oV);q4
Lcm;aP
C)g0
C
aD(135,"C[IsUnaryOpcode(C)&&!HasInvalidRangesOpcode(C)] "
oU,"B "
mT,q31(A)qF1(B)q51(C)oV);q4
Lcn;}
}
}
if(B
qY
oJ(130,"B[B>=VarBegin] "
oP,wA,q31(A)qF1(B)oV);q4
Lcj;aP
B)g0
B
aD(131,"B[IsUnaryOpcode(B)&&!HasInvalidRangesOpcode(B)] "
oP,mT,q31(A)qF1(B)oV);q4
Lco;}
}
}
gS
oJ(128,w3
oS
oN,"[x]"
,q31(A)oV);q4
Lcp;aP
A)g0
A
aD(129,"A[IsUnaryOpcode(A)&&!HasInvalidRangesOpcode(A)] "
oS
oN,wA,q31(A)oV);q4
Lcq;}
}
}
qN
gW
g1
gX
oJ(194,aR
wN,"[x+x]"
oN,oR);q4
Lda
qL
g9
3]qF
A=oE
gS
oJ(282,w3
aN
w0
wN,wA" A "
qC1
w0,m7", "
aO
aW
y
q41
aB(A)oV);q4
Ldb;}
qK
o4
oJ(281,wH
w8,wA" "
m4" "
w0,q01);q4
Ldc;qK
dB:g1
gW
qI-3]mV(196,aR
oN
wN,"cMul [x+x]"
oN,oR);q4
Ldd;qK
o4
if(y*a4(184,"y[y*x==Value_t(1)]"
oN
wN,,q01);q4
Lde;}
oJ(185,"y"
oN
wN,m4
oN,q01);q4
Ldf;a8(174,m3
wN,"[-x]"
oN,oR);q4
Ldg
qL
cSub:hJ==dB){qI-3]qF
A=oE
gS
oJ(283,w3
aN
qG1
wN,wA" A "
m4
oN" cSub"
,m7", "
aO
aW
y
q41
aB(A)oV);q4
Ldh;}
}
qK
o4
oJ(71,"y"
wN,m4,q01);q4
Ldi;}
gC
aT(70,"x[x==Value_t(1)]"
oN,,oR);q4
o7}
gC
q9
2
aD(139,"x[x==Value_t(2)]"
oN,aR,oR);q4
Ldj;}
gC
q9-1
aD(176,"x[x==Value_t(-1)]"
oN,m3,oR);q4
Ldk;}
gF
g4
dE:;A
h1
gS
qN
dB:g1
g8
B=hK
o9
oJ(385,mO
m3
oN" "
oW,m5
oN" "
m3,m8(A)qF1(B)oV);q4
Lcg;}
q4
Default5;g4
Default5:;aC(376,wD
oN" "
oW,m5
oN,m8(A)qF1(B)oV);q4
Ldl;}
}
q4
dF
qL
g8
aC(379,mO
m2
oW,m5" "
m3,m8(A)qF1(B)oV);q4
Lch;}
q4
dF;g4
dF:;B=qU;if(o9
oJ(373,mO
oW,m5,m8(A)qF1(B)oV);q4
Lce;}
}
aP
A)){B=qU;if(B
g3
1){hJ==dB){C=hK
C
hW
D=oE
if(D==B){oJ(395,"D[D==B] C[C==A]"
oN" B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]"
oN,"D C cSqr"
oN,m8(A)qF1(B)q51(C)<<", D"
m6(D)oV);q4
Ldm;}
}
}
}
qJ
TailCall_cNEqual:hC
dO:h6
A=mA(324,"A[IsLogicalOpcode(A)] "
oX
w6,"A"
wT,q31(A)oV);q4
Lbg;}
}
mM(168,aO"x "
w6,"[fp_nequal(y,x)]"
,q01);q4
Ldn;}
dV
qN
hP
oJ(256,wP" "
mF
w6,aX
w6,oR);q4
Ldo
qL
hH
oJ(258,mI
mF
w6,aX
w6,oR);q4
Ldo;}
oJ(314,mF
w6,wU,oR);q4
Lbi;qJ
hA:hC
g8
q8
g9
1
aF(175,aU
m3,"[-x]"
oN,oR);q4
Ldp;a8(238,m2
m3,,);q4
Lbl
qQ
oJ(68,"x "
m3,"[-x]"
,oR);q4
Ldq;}
qM
TailCall_cNot:hC
cNot:q8
hP
oJ(286,wP
wT,aY,);q4
Lea
qL
cAbsNot:A=mA(291,"A[IsLogicalOpcode(A)] "
wW
wT,"A"
,m8(A)oV);q4
Lbl;}
if(A!=q6){oJ(292,"A[A!=cImmed] "
wW
wT,"A cAbsNotNot"
,m8(A)oV);q4
Leb;}
q4
dG
qL
cAbsNotNot:oJ(289,"cAbsNotNot"
wT,wW,);q4
Lec
qL
gW
dS
q7
oJ(326,"x "
w0
wT,aX
wF,oR);q4
Led;}
q4
dG
qL
dM:oJ(252,wF
wT,w6,);q4
Lee
qL
hR:oJ(250,wY
wT,wV,);q4
Lef
qL
h5:oJ(251,wG
wT,mN,);q4
Leg
qL
dN
oJ(248,mN
wT,wG,);q4
Leh
qL
d0:oJ(249,wV
wT,wY,);q4
Lei
qL
dO:oJ(253,w6
wT,wF,);q4
Lej
mW(285,m3
wT,aY,);q4
Lea
qL
cNot:oJ(287,aY
wT,wU,);q4
Lek
qL
d8:oJ(288,wU
wT,aY,);q4
Lea
qQ
oJ(85,"x"
wT,"[fp_not(x)]"
,oR);q4
Lel;g4
dG:;A
h1
qH
oJ(293,oT
aY,"A "
wW,m8(A)oV);q4
Lem;qJ
gV:hC
d8:qI
0]==cAdd){dS
q7
oJ(325,"x "
a6
wU,aX
w6,oR);q4
Len;qJ
TailCall_cOr:hC
cOr:qI
0
mG(170,aO"x cOr"
,"[fp_or(x,y)]"
,q01);q4
Leo;qJ
TailCall_cRDiv:hC
o2
h6
oJ(91,oX
mP,mS,oR);q4
Lep;qJ
TailCall_cSqr:hC
hH
q8
hP
oJ(391,wP" cSqr"
,"cSqr"
,);q4
Leq
mW(390,m2"cSqr"
,"cSqr"
,);q4
Leq;}
qM
hB:hC
cSub:a3(191,w7
qG1,"[Value_t(0)]"
oN,);q4
Lfa
mW(244,m2
qG1,w0,);q4
Lfb
qQ
dV
oJ(77,mF
qG1,,oR);q4
o7}
mM(78,aO"x cSub"
,"[y-x]"
,q01);q4
Lfc;}
oJ(207,"x cSub"
,aE
w0,oR);q4
Lfd;h0
dR
oJ(112,"x "
wC" "
w3
qG1,"A "
a6
aX
wC,m8(A)q61);q4
Lfe;}
oJ(119,wC" "
w3
qG1,"[DO_STACKPLUS1] A cSub "
wC,m8(A)oV);hX
Lff;}
qJ
g4
Default0:;A=oF;if(IsComparisonOpcode(A)){qI
0]q7
hG
qF
oJ(260,wH
oQ,"[x-y] A"
,m8(A)q71);q4
Lfg;a8(261,m2
oQ,aE"{OppositeComparisonOpcode(A)}"
,m8(A)q61);q4
Lfh;}
}
}
if(A
g3
0){B
h1
if(o9
oJ(393,mO"A[A>=VarBegin&&data->ByteCode.size()>0]"
,"B cDup"
,m8(A)qF1(B)oV);q4
Lfi;}
aP
A)){B
h1
if(B
g3
1){C=qU;if(C
hW
D
qX
D==B){oJ(394,"D[D==B] C[C==A] B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]"
,"D C cDup"
,m8(A)qF1(B)q51(C)<<", D"
m6(D)oV);q4
Lfj;}
}
}
}
}
q4
Lfk;Laa:h4
TailCall_cAbs;Lab:qE=d4;g7
Lac:qC
q9
4));gQ
qA
qP
Lfl:gG
dB;Lfm:FP_ReDefinePointers();Lfn:q4
TailCall_cMul;Lad:oA
4
gK
qC
q9
4));hS
Lfl;Lae:qE=x+hY;gQ
qA
qP
Lbo:gG
dB;q4
Lfn;Laf:hT
cSub;Lfo:q4
hB;Lag:hI
gY
Lfp:FP_ReDefinePointers();Lfq:q4
TailCall_cAdd;Lah:hI
hD
Lga:d1
Lgb:qG
dL);g7
Lai:gM
gP
2]=q6;qA
d3
Lga;Laj:hI
gY
q4
Lgb;Lak:hI
hO
Lgc:d1
Lgd:qG
B);Lge:gG
cSub;qS
Lfo;Lal:gM
d5=q6;g6
d3
Lgc;Lam:hI
hD
q4
Lgd;Lan:hI
Lao:h8
Lbl:qP
g7
Lap:q3
3
d2
qC
x);hS
Lga;Laq:qA
o3
Lga;Lba:qV
fp_and(x
gI
Lbb:qC
q9
0));dP
q6;qG
dB);qC
hY);Lgf:dQ
Lgg:gG
cAdd;q4
Lfp;Lbc:gM
qU
o1
Lgh:q4
TailCall_cDiv;Lbd:qV
y/x;q4
o7
Lbe:qV
fp_equal(y
gZ
Lbf:qU
o1
Lgi:q4
TailCall_cEqual;Lbg:h8
qP
Lgj:gG
cNot;FP_ReDefinePointers();Lgk:q4
TailCall_cNot;Lbh:gY
Lgl:gG
d8;FP_ReDefinePointers();Lgm:q4
gV;Lbi:h8
d3
Lgl;Lbj:qV
fp_less(x
gI
Lbk:qV
fp_lessOrEq(x
gI
Lbm:q1
cRSqrt);g7
Lbn:qE=hY/x;g7
Lbp:gY
q4
Lgj;Lbq:qV
fp_less(y
gZ
Lca:qV
fp_lessOrEq(y
gZ
Lcb:qV
fp_max(x
gI
Lcc:qV
fp_min(x
gI
Lcd:qV
fp_mod(y
gZ
Lce:hT
cSqr;Lgn:q4
TailCall_cSqr;Lcf:hT
cDiv;q4
Lgh;Lcg:g6
q1
cSqr);qG
dB);Lgo:gG
cNeg;qS
hA;Lch:aS
cSqr
gJ
Lgo;Lci:qE=x+hY;gP
2]=q6;aS
cPow);g7
Lcj:gQ
qA
d3
Lfn;Lck:qV
x;Lgp:gY
q4
Lfm;Lcl:qA
h8
Lgq:oA
dZ
Lha:qE1
x);Lhb:hS
Lfm;Lcm:h8
q4
Lgq;Lcn:q3
4
o0
B
gJ
Lha;Lco:hD
q4
Lha;Lcp:m1
q4
Lbl;Lcq:qU
gR
1;d3
Lfn;Lda:qE=x+x;q4
Lcj;Ldb:qV
x;gP
4]=q6;q3
4
gK
qE1
y*x);dQ
q81
Lgg;Ldc:qV
x;q3
2
gK
qC
y*x
gJ
Lgf;Ldd:q3
4
gK
qC
x+x
gJ
Lhb;Lde:qA
hD
g7
Ldf:qV
y*x;q4
Lgp;Ldg:gM
q4
Lcq;Ldh:qV
x;gP
4]=q6;q3
4
gK
qE1
y*x);dQ
q81
Lge;Ldi:qV
y*x;q4
o7
Ldj:dP
dY;q2-=1;h8
Lhc:gG
cAdd;q4
Lfq;Ldk:h8
d3
Lgo;Ldl:qA
qP
Lhd:qG
cSqr
gJ
Lfm;Ldm:g6
d3
Lhd;Ldn:qV
fp_nequal(y
gZ
Ldo:qU
o1
Lhe:q4
TailCall_cNEqual;Ldp:gM
h4
Lbo;Ldq:gM
g7
Lea:h4
Lgk;Leb:q1
cAbsNotNot);g7
Lec:qP
Lem:qG
cAbsNot);g7
Led:qE=x;Lej:hT
dM;q4
Lgi;Lee:hT
dO;q4
Lhe;Lef:hT
d0;q4
TailCall_cLessOrEq;Leg:hT
cLess;q4
TailCall_cLess;Leh:hT
h5;q4
TailCall_cGreaterOrEq;Lei:hT
hR;q4
TailCall_cGreater;Lek:hT
d8;q4
Lgm;Lel:qE=fp_not(w9
Len:qE=x;q4
Lee;Leo:qV
fp_or(x
gI
Lep:h8
qP
gG
cInv;qS
TailCall_cInv;Leq:h4
Lgn;Lfa:qC
q9
0));dP
q6;q4
Lfl;Lfb:h4
Lhc;Lfc:qV
y-x;q4
o7
Lfd:gM
q4
Lhc;Lfe:q3
3
d2
d1
qC
x);hS
Lgb;Lff:aS
A);qG
cSub
gJ
Lgb;Lfg:qV
x-y;q3
2
d2
g7
Lfh:gM
m1
q1
OppositeComparisonOpcode(A));g7
Lfi:qD
dY);g7
Lfj:dP
dY;g7
Lfk:qD
oF);g7
g7
q4
TailCall_cAnd;q4
TailCall_cMax;q4
TailCall_cMin;q4
TailCall_cMod;q4
hA;q4
gV;q4
TailCall_cOr;q4
TailCall_cRDiv;q4
hB;
#endif
#if(FP_FLOAT_VERSION)
hQ
x;h9
A;hQ
y;h9
B;h9
C;h9
D;hF
oF){TailCall_cAbs:hC
hP
qA1(236,m2
wP,wP,);q4
Laa
qQ
oJ(40,"x "
wP,"[fp_abs(x)]"
,oR);q4
Lab;h0
qH
oJ(295,oT
wP,"A"
,m8(A)oV);g7
qJ
TailCall_cAcos:hC
cAcos:g5>=mU<=aT(44,"x[x>=Value_t(-1)&&x<=Value_t(1)] cAcos"
,"[fp_acos(x)]"
,oR);q4
Lac;qJ
TailCall_cAcosh:hC
cAcosh:g5>=aT(41,"x[x>=Value_t(1)] cAcosh"
,"[fp_acosh(x)]"
,oR);q4
Lad;qJ
TailCall_cAdd:hC
gW
q8
gX
hG
mV(186,w7
a6
aR,"[Value_t(4)]"
oN,);q4
Lae;dC==cAdd){qI-3]mV(187,aR
oN" "
aR,"cMul [Value_t(4)]"
oN,);q4
Laf;}
}
gF}
q4
dD
hE
hJ
mV(189,w7
aU
w0,"[x+Value_t(1)]"
oN,oR);q4
Lag;}
}
q4
dD
mW(243,m2
w0,qG1,);q4
Lah
qQ
hG
qF
oJ(180,wH"x "
w0,"[y+x] "
w0,q01);q4
Lai;qK
dL:g1
gW
qI-3]qF
oJ(121,wH
mE,wS
a6
wC,q01);q4
Laj;a8(122,m2
mE,aE
a6
wC,oR);q4
Lak
q91(120,aO
mE,wS
wC,q01);q4
Lal;qK
cSub:hU
hF
d5){hC
gW
qI-4]qF
oJ(115,wH
w4,wS
a6
mQ,qH1(B)q41
aA);q4
Lam;a8(116,m2
w4,aE
a6
mQ,qH1(B)oV);q4
Lan
q91(114,aO
w4,wS
mQ,qH1(B)q41
aA);q4
o7}
qK
o4
oJ(76,aO"x "
w0,"[y+x]"
,q01);q4
Lap;}
dV
oJ(75,mF
w0,,oR);q4
Laq;}
gF
g4
dD:;A
h1
dR
oJ(113,"x "
wC" "
w3
w0,"[DO_STACKPLUS1] A "
aX
a6
wC,m8(A)q61);hX
Lba;}
oJ(118,wC" "
w3
w0,"[DO_STACKPLUS1] A "
a6
wC,m8(A)oV);hX
Lbb;}
qJ
TailCall_cAnd:hC
cAnd:qI
0
mG(169,aO"x cAnd"
,"[fp_and(x,y)]"
,q01);q4
Lbc;qJ
TailCall_cAsin:hC
cAsin:g5>=mU<=aT(45,"x[x>=Value_t(-1)&&x<=Value_t(1)] cAsin"
,"[fp_asin(x)]"
,oR);q4
Lbd;qJ
TailCall_cAsinh:hC
cAsinh:qI
0
aF(42,"x cAsinh"
,"[fp_asinh(x)]"
,oR);q4
Lbe;}
qM
TailCall_cAtan:hC
cAtan:qI
0
aF(46,"x cAtan"
,"[fp_atan(x)]"
,oR);q4
Lbf;}
qM
TailCall_cAtan2:hC
cAtan2:qI
0
mG(83,aO"x cAtan2"
,"[fp_atan2(y,x)]"
,q01);q4
Lbg;qJ
TailCall_cAtanh:hC
cAtanh:g5>mU<aT(43,"x[x>Value_t(-1)&&x<Value_t(1)] cAtanh"
,"[fp_atanh(x)]"
,oR);q4
Lbh;qJ
TailCall_cCbrt:hC
cCbrt:qI
0
aF(47,"x cCbrt"
,"[fp_cbrt(x)]"
,oR);q4
Lbi;}
qM
TailCall_cCeil:hC
cCeil:qA1(304,m2
aG,mZ" "
m3,);q4
Lbj
qQ
oJ(48,"x "
aG,"[fp_ceil(x)]"
,oR);q4
Lbk;dH
oJ(298,"A[IsAlwaysIntegerOpcode(A)] "
aG,"A"
,m8(A)oV);g7
qJ
TailCall_cCos:hC
cCos:q8
hP
oJ(339,wP" "
wZ,wZ,);q4
Lbl
mW(341,m2
wZ,wZ,);q4
Lbl
qQ
oJ(49,"x "
wZ,"[fp_cos(x)]"
,oR);q4
Lbm;}
qM
TailCall_cCosh:hC
cCosh:q8
hP
oJ(340,wP" "
wJ,wJ,);q4
Lbn
qL
cAcosh:oJ(232,"cAcosh "
wJ,,);q4
Lbo
qL
cAsinh:oJ(353,"cAsinh "
wJ,q11
mI"[Value_t(1)] "
a6
mD,);hX
Lbp
mW(342,m2
wJ,wJ,);q4
Lbn
qQ
oJ(50,"x "
wJ,"[fp_cosh(x)]"
,oR);q4
Lbq;}
qM
TailCall_cDeg:hC
cDeg:qI
0
aF(65,"x cDeg"
,"[RadiansToDegrees(x)]"
,oR);q4
Lca;}
qM
TailCall_cDiv:hC
cDiv:q8
cCos:oJ(356,wZ" "
mR,aH
oN,);q4
Lcb
qL
cCot:oJ(360,aI" "
mR,aJ
oN,);q4
Lcc
qL
cCsc:oJ(358,aK" "
mR,aL
oN,);q4
Lcd
qL
gX
oJ(159,w7
mR,"[Value_t(0)]"
oN" [Value_t(1)] "
w0,);q4
Lce
qL
aQ(310,m9
mR,m2
mX
oN,);q4
Lcf
qL
d9:oJ(311,wK" "
mR,m2
wK
oN,);q4
Lcg
qL
qI1(241,mS" "
mR,"cMul"
,);q4
Lch
qL
dW
oJ(309,"cPow "
mR,m3
wX
oN,);q4
Lci
qL
cSec:oJ(359,aH" "
mR,wZ
oN,);q4
Lcj
qL
cSin:oJ(355,aL" "
mR,aK
oN,);q4
Lck
qL
cTan:oJ(357,aJ" "
mR,aI
oN,);q4
Lcl
qQ
gC
aT(72,oX
mR,,oR);q4
Laq;}
oB
qN
g9
2]qF
if((y/x)==h2
oJ(198,"y[(y/x)==GetRadiansToDegreesFactor<Value_t>()]"
oN" "
w5
mR,"cDeg"
,q01);q4
Lcm;}
if((y/x)==h3
oJ(199,"y[(y/x)==GetDegreesToRadiansFactor<Value_t>()]"
oN" "
w5
mR,"cRad"
,q01);q4
Lcn;}
oJ(200,aN
w5
mR,"[y/x]"
oN,q01);q4
Lco;a8(177,m2
w5
mR,aE
mR,oR);q4
Lcp
q91(73,aO
w5
mR,"[y/x]"
,q01);q4
Lcq;}
oJ(202,w5
mR,"[Value_t(1)/x]"
oN,oR);q4
Lda;}
gF
o5
cDiv:hU
oJ(94,"B[B>=VarBegin] "
mR" "
w3
mR,"[DO_STACKPLUS1] B "
mY
mR,m8(A)qF1(B)oV);hX
Ldb;qK
o2
g1
wL(89,"x "
mP" "
w3
mR,mY
aX
mP,m8(A)q61);q4
Ldc;g4
hU
oJ(88,"B[B>=VarBegin] "
mP" "
w3
mR,mY"B "
mP,m8(A)qF1(B)oV);q4
Ldd;}
}
gF}
qJ
TailCall_cEqual:hC
dM:aZ
qN
hP
oJ(255,wP" "
mF
wF,aX
wF,oR);q4
Lde
qL
hH
oJ(257,mI
mF
wF,aX
wF,oR);q4
Lde;}
}
mM(167,aO"x "
wF,"[fp_equal(y,x)]"
,q01);q4
Ldf;qJ
TailCall_cExp:hC
cExp:q8
gW
dS
q7
oJ(306,"x "
a6
mX,m9"[fp_exp(x)]"
oN,oR);q4
Ldg;qK
cLog:A=mB(222,oT
m0" "
mX,"A"
,m8(A)oV);q4
Lbo;qK
wL(51,"x "
mX,"[fp_exp(x)]"
,oR);q4
Ldh;}
qM
TailCall_cExp2:hC
d9:q8
gW
dS
q7
oJ(307,"x "
a6
wK,wK" [fp_exp2(x)]"
oN,oR);q4
Ldi;qK
cLog2:A=mB(223,oT
wO" "
wK,"A"
,m8(A)oV);q4
Lbo;qK
wL(52,"x "
wK,"[fp_exp2(x)]"
,oR);q4
Ldj;}
oJ(399,wK,"[DO_STACKPLUS1] [fp_log(Value_t(2))]"
oN" "
mX,);hX
Ldk;TailCall_cFloor:hC
cFloor:qA1(303,m2
mZ,aG" "
m3,);q4
Ldl
qQ
oJ(53,"x "
mZ,"[fp_floor(x)]"
,oR);q4
Ldm;dH
oJ(297,"A[IsAlwaysIntegerOpcode(A)] "
mZ,"A"
,m8(A)oV);g7
qJ
TailCall_cGreater:hC
hR:qI
0
mG(165,aO"x "
wY,"[fp_less(x,y)]"
,q01);q4
Ldn;}
gC
q9-dJ
oJ(333,"x[x==Value_t(-0.5)] "
wY,m2
wW,oR);q4
Ldo;qJ
TailCall_cGreaterOrEq:hC
h5:g5!=qR
dS==oD
oJ(329,wP" "
w5
wG,"[Value_t(0.5)/x]"
oN" "
wU,oR);q4
Ldp;}
}
mM(166,aO"x "
wG,"[fp_lessOrEq(x,y)]"
,q01);q4
Ldq;}
gC
q9
dJ
oJ(332,"x[x==Value_t(0.5)] "
wG,"cAbsNotNot"
,oR);q4
Lea;qJ
TailCall_cInt:hC
cInt:q8
wL(54,"x cInt"
,"[fp_int(x)]"
,oR);q4
Leb;dH
oJ(299,"A[IsAlwaysIntegerOpcode(A)] cInt"
,"A"
,m8(A)oV);g7
qJ
TailCall_cInv:hC
cInv:q8
cCos:oJ(362,wZ" "
mS,aH,);q4
Lec
qL
cCot:oJ(366,aI" "
mS,aJ,);q4
Led
qL
cCsc:oJ(364,aK" "
mS,aL,);q4
Lee
qL
qI1(237,mS" "
mS,,);q4
Lbo
qL
dW
oJ(239,"cPow "
mS,m3
wX,);q4
Lef
qL
cSec:oJ(365,aH" "
mS,wZ,);q4
Leg
qL
cSin:oJ(361,aL" "
mS,aK,);q4
Leh
qL
cSqrt:oJ(67,mD" "
mS,"cRSqrt"
,);q4
Lei
qL
cTan:oJ(363,aJ" "
mS,aI,);q4
Lej
qQ
oB
oJ(69,w5
mS,"[Value_t(1)/x]"
,oR);q4
Lek;aM
TailCall_cLess:hC
dN
aZ
A=mB(124,oT
mF
mN,mT,q31(A)oV);q4
Lel;}
}
oB
dS==oD
oJ(328,wP" "
w5
mN,"[Value_t(0.5)/x]"
oN
wT,oR);q4
Lem;}
}
mM(163,aO"x "
mN,"[fp_less(y,x)]"
,q01);q4
Len;}
gC
q9
dJ
oJ(331,"x[x==Value_t(0.5)] "
mN,wW,oR);q4
Leo;qJ
TailCall_cLessOrEq:hC
d0:qI
0
mG(164,aO"x "
wV,"[fp_lessOrEq(y,x)]"
,q01);q4
Lep;}
gC
q9-dJ
oJ(334,"x[x==Value_t(-0.5)] "
wV,m2"cAbsNotNot"
,oR);q4
Leq;qJ
TailCall_cLog:hC
cLog:q8
aQ(225,m9
m0,,);q4
Lbo
a0(411,a1
m0,m0" [fp_log(x)] "
w0,oR);q4
Lfa;}
a2(142,mI
m0,wP" "
m0" "
aR,);q4
Lfb
qB1(55,w1" "
m0,"[fp_log(x)]"
,oR);q4
Lfc;aM
TailCall_cLog10:hC
oI:q8
aQ(401,m9
wI,"[DO_STACKPLUS1] [fp_log10(fp_const_e<Value_t>())]"
oN,);hX
Lfd
a0(412,a1
wI,wI" [fp_log10(x)] "
w0,oR);q4
Lfe;}
a2(144,mI
wI,wP" "
wI" "
aR,);q4
Lff
qB1(56,w1" "
wI,"[fp_log10(x)]"
,oR);q4
Lfg;aM
TailCall_cLog2:hC
cLog2:q8
aQ(400,m9
wO,"[DO_STACKPLUS1] [fp_log2(fp_const_e<Value_t>())]"
oN,);hX
Lfh
qL
d9:oJ(226,wK" "
wO,,);q4
Lbo
a0(410,a1
wO,wO" [fp_log2(x)] "
w0,oR);q4
Lfi;}
a2(143,mI
wO,wP" "
wO" "
aR,);q4
Lfj
qB1(57,w1" "
wO,"[fp_log2(x)]"
,oR);q4
Lfk;aM
TailCall_cMax:hC
cMax:a3(149,w7
mJ,,);q4
Lbo
qQ
mM(82,aO"x "
mJ,"[fp_max(x,y)]"
,q01);q4
Lfl;}
gF
o5
gX
aC(151,mO
w7
w3
mJ,"B cDup"
,m8(A)qF1(B)oV);q4
Lbo;qK
cMax:aC(153,mO
mJ" "
w3
mJ,"B "
mJ,m8(A)qF1(B)oV);q4
Lbo;}
gF}
qJ
TailCall_cMin:hC
cMin:a3(148,w7
mK,,);q4
Lbo
qQ
mM(81,aO"x "
mK,"[fp_min(x,y)]"
,q01);q4
Lfm;}
gF
o5
gX
aC(150,mO
w7
w3
mK,"B cDup"
,m8(A)qF1(B)oV);q4
Lbo;qK
cMin:aC(152,mO
mK" "
w3
mK,"B "
mK,m8(A)qF1(B)oV);q4
Lbo;}
gF}
qJ
TailCall_cMod:hC
cMod:g5!=qR
mM(74,aO
w5"cMod"
,"[fp_mod(y,x)]"
,q01);q4
Lfn;}
qJ
TailCall_cMul:hC
dB:a3(396,"cDup"
oN,"cSqr"
,);q4
Lfo
qL
qI1(242,mS
oN,mR,);q4
Lfp
qL
g8
A=qU;gS
g1
dB:B=hK
o9
oJ(388,wD
oN" "
w3
m3
oN,m5
oN" "
m3,m8(A)qF1(B)oV);q4
Lfq;}
q4
Default2;g4
Default2:;aC(382,mO
w3
m3
oN,m5" "
m3,m8(A)qF1(B)oV);q4
Lga;}
}
}
q4
dE
qL
dW
dS
q7
hJ
mV(190,w7
wR
oN,"[x+Value_t(1)]"
wX,oR);q4
Lgb;}
}
q4
dE
qQ
dV
qN
dB:A
qX
A
qY
oJ(133,oW" "
oS
oN,wA,q31(A)oV);q4
Lgc;}
q4
Default4;g4
Default4:;A=qU;if(IsBinaryOpcode(A)g0
A)){g1
o4
oJ(132,aO
oP,wA,q31(A)q41
aA);q4
Lgd;g4
hM
IsBinaryOpcode(B)g0
B)){hF
d5){hC
o4
oJ(136,aO
oU,mT,q31(A)qF1(B)q41
aA);q4
Lge;g4
C=hK
C
qY
oJ(134,"C[C>=VarBegin] "
oU,mT,q31(A)qF1(B)q51(C)oV);q4
Lgf;aP
C)g0
C
aD(135,"C[IsUnaryOpcode(C)&&!HasInvalidRangesOpcode(C)] "
oU,"B "
mT,q31(A)qF1(B)q51(C)oV);q4
Lgg;}
}
}
if(B
qY
oJ(130,"B[B>=VarBegin] "
oP,wA,q31(A)qF1(B)oV);q4
Lgc;aP
B)g0
B
aD(131,"B[IsUnaryOpcode(B)&&!HasInvalidRangesOpcode(B)] "
oP,mT,q31(A)qF1(B)oV);q4
Lgh;}
}
}
gS
oJ(128,w3
oS
oN,"[x]"
,q31(A)oV);q4
Lgi;aP
A)g0
A
aD(129,"A[IsUnaryOpcode(A)&&!HasInvalidRangesOpcode(A)] "
oS
oN,wA,q31(A)oV);q4
Lgj;}
}
}
qN
gW
g1
gX
dI+a4(193,"cDup[x+x==Value_t(1)] "
w0
wN,,oR);q4
Lgk;}
oJ(194,aR
wN,"[x+x]"
oN,oR);q4
Lgl
qL
g9
3]qF
A=oE
gS
oJ(282,w3
aN
w0
wN,wA" A "
qC1
w0,m7", "
aO
aW
y
q41
aB(A)oV);q4
Lgm;}
qK
o4
oJ(281,wH
w8,wA" "
m4" "
w0,q01);q4
Lgn;qK
cDeg:oJ(145,"cDeg"
wN,"[RadiansToDegrees(x)]"
oN,oR);q4
Lgo
qL
cDiv:hU
hF
d5){hC
g9
4]qF
oJ(101,aN"B[B>=VarBegin] "
mR
wN,qC1
mC,qH1(B)q41
aA);q4
Lgp;a8(102,m2"B[B>=VarBegin] "
mR
wN,wB
mC,qH1(B)oV);q4
Lgq
q91(100,aO"B[B>=VarBegin] "
mR
wN,m4" "
mC,qH1(B)q41
aA);q4
Lha;}
qK
dB:g1
gW
qI-3]==dY){dI+a4(195,"cDup[x+x==Value_t(1)] "
w0
oN
wN,"cMul"
,oR);q4
Lhb;}
oJ(196,aR
oN
wN,"cMul [x+x]"
oN,oR);q4
Lhc;qK
o4
if((y*x)==h2
oJ(182,"y[(y*x)==GetRadiansToDegreesFactor<Value_t>()]"
oN
wN,"cDeg"
,q01);q4
Lcm;}
if((y*x)==h3
oJ(183,"y[(y*x)==GetDegreesToRadiansFactor<Value_t>()]"
oN
wN,"cRad"
,q01);q4
Lcn;}
if(y*a4(184,"y[y*x==Value_t(1)]"
oN
wN,,q01);q4
Lhd;}
oJ(185,"y"
oN
wN,m4
oN,q01);q4
Lhe;a8(174,m3
wN,"[-x]"
oN,oR);q4
Lhf
qL
o2
g1
g9
3]qF
oJ(108,aN
mL,qC1
mP,q01);q4
Lhg;a8(109,m2
mL,wB
mP,oR);q4
Lhh
q91(107,aO
mL,m4" "
mP,q01);q4
Lhi;qK
cRad:oJ(146,"cRad"
wN,"[DegreesToRadians(x)]"
oN,oR);q4
Lhj
qL
cSub:hJ==dB){qI-3]qF
A=oE
gS
oJ(283,w3
aN
qG1
wN,wA" A "
m4
oN" cSub"
,m7", "
aO
aW
y
q41
aB(A)oV);q4
Lhk;}
}
qK
o4
oJ(71,"y"
wN,m4,q01);q4
Lhl;}
gC
aT(70,"x[x==Value_t(1)]"
oN,,oR);q4
Laq;}
gC
q9
2
aD(139,"x[x==Value_t(2)]"
oN,aR,oR);q4
Lhm;}
gC
h2
oJ(140,"x[x==GetRadiansToDegreesFactor<Value_t>()]"
oN,"cDeg"
,oR);q4
Lhn;}
gC
h3
oJ(141,"x[x==GetDegreesToRadiansFactor<Value_t>()]"
oN,"cRad"
,oR);q4
Lho;}
gC
q9-1
aD(176,"x[x==Value_t(-1)]"
oN,m3,oR);q4
Lhp;}
gF
g4
dE:;A
h1
gS
qN
cDiv:hU
oJ(97,"B[B>=VarBegin] "
mR" "
oW,q11
mY
mC,m8(A)qF1(B)oV);hX
Lhq;}
q4
hL
dB:g1
g8
B=hK
o9
oJ(385,mO
m3
oN" "
oW,m5
oN" "
m3,m8(A)qF1(B)oV);q4
Lfq;}
q4
dF;g4
dF:;aC(376,wD
oN" "
oW,m5
oN,m8(A)qF1(B)oV);q4
Lia;}
}
q4
hL
g8
aC(379,mO
m2
oW,m5" "
m3,m8(A)qF1(B)oV);q4
Lga;}
q4
hL
o2
hJ
q7
oJ(90,"x "
mP" "
oW,q11
mT" "
mP,m8(A)q61);hX
Lib;}
oJ(104,mP" "
oW,q11
mY
mP,m8(A)oV);hX
Lic;g4
Default5:;B=qU;if(o9
oJ(373,mO
oW,m5,m8(A)qF1(B)oV);q4
Lfo;}
}
aP
A)){B=qU;if(B
g3
1){hJ==dB){C=hK
C
hW
D=oE
if(D==B){oJ(395,"D[D==B] C[C==A]"
oN" B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]"
oN,"D C cSqr"
oN,m8(A)qF1(B)q51(C)<<", D"
m6(D)oV);q4
Lid;}
}
}
}
qJ
TailCall_cNEqual:hC
dO:aZ
qN
hP
oJ(256,wP" "
mF
w6,aX
w6,oR);q4
Lie
qL
hH
oJ(258,mI
mF
w6,aX
w6,oR);q4
Lie;}
}
mM(168,aO"x "
w6,"[fp_nequal(y,x)]"
,q01);q4
Lif;qJ
hA:hC
g8
q8
g9
1
aF(175,aU
m3,"[-x]"
oN,oR);q4
Lig;a8(238,m2
m3,,);q4
Lbo
qL
cSin:wE(347,aU
aL" "
m3,wB
aL,oR);q4
Lih;}
qK
oG
wE(348,aU"cSinh "
m3,wB"cSinh"
,oR);q4
Lii;}
qK
cTan:wE(349,aU
aJ" "
m3,wB
aJ,oR);q4
Lij;}
qK
oH
wE(350,aU"cTanh "
m3,wB"cTanh"
,oR);q4
Lik;}
qK
wL(68,"x "
m3,"[-x]"
,oR);q4
Lil;}
qM
TailCall_cNot:hC
cNot:q8
hP
oJ(286,wP
wT,aY,);q4
Lim
qL
cAbsNot:A=mA(291,"A[IsLogicalOpcode(A)] "
wW
wT,"A"
,m8(A)oV);q4
Lbo;}
if(A!=q6){oJ(292,"A[A!=cImmed] "
wW
wT,"A cAbsNotNot"
,m8(A)oV);q4
Lin;}
q4
dG
qL
cAbsNotNot:oJ(289,"cAbsNotNot"
wT,wW,);q4
Lio
qL
dM:oJ(252,wF
wT,w6,);q4
Lip
qL
hR:oJ(250,wY
wT,wV,);q4
Liq
qL
h5:oJ(251,wG
wT,mN,);q4
Lja
qL
dN
oJ(248,mN
wT,wG,);q4
Ljb
qL
d0:oJ(249,wV
wT,wY,);q4
Ljc
qL
dO:oJ(253,w6
wT,wF,);q4
Ljd
mW(285,m3
wT,aY,);q4
Lim
qL
cNot:oJ(287,aY
wT,wU,);q4
Lje
qL
d8:oJ(288,wU
wT,aY,);q4
Lim
qQ
oJ(85,"x"
wT,"[fp_not(x)]"
,oR);q4
Ljf;g4
dG:;A
h1
qH
oJ(293,oT
aY,"A "
wW,m8(A)oV);q4
Ljg;qJ
TailCall_cOr:hC
cOr:qI
0
mG(170,aO"x cOr"
,"[fp_or(x,y)]"
,q01);q4
Ljh;qJ
TailCall_cPow:hC
dW
qI
0]q7
if(!gL
x+x)){dS==cSqr){oJ(14,mI"x[!isEvenInteger(x+x)]"
wX,wP" [x+x]"
wX,oR);q4
Lji;}
}
if(IsIntegerConst(x)){qN
aQ(35,m9
oY,wA" "
mX,oR);q4
Ljj
qL
d9:oJ(36,wK" "
oY,wA" "
wK,oR);q4
Ljk
qL
dW
hJ
qF
if(!IsIntegerConst(y
aD(34,"y[!IsIntegerConst(y)]"
wX" "
oY,wQ,q01);q4
Ljl;}
}
oJ(37,"cPow "
oY,wA
wX,oR);q4
Ljm;}
}
if(gL
x)){qN
hP
oJ(336,wP" x[isEvenInteger(x)]"
wX,"[x]"
wX,oR);q4
Ljn
qL
g9
2]==oD
oJ(337,wP
oN" x[isEvenInteger(x)]"
wX,"cMul [x]"
wX,oR);q4
Ljo;}
gF}
}
dV
oJ(161,oS
wX,"[Value_t(0)]"
oN" [Value_t(1)] "
w0,oR);q4
Ljp;}
gC
q9
dJ
oJ(214,"x[x==Value_t(0.5)]"
wX,mD,oR);q4
Ljq;}
gC
hY/q9
3
aD(215,"x[x==Value_t(1)/Value_t(3)]"
wX,"cCbrt"
,oR);q4
Lka;}
gC
hY/q9-3
aD(216,"x[x==Value_t(1)/Value_t(-3)]"
wX,"cCbrt "
mS,oR);q4
Lkb;}
gC
q9-dJ
oJ(217,"x[x==Value_t(-0.5)]"
wX,"cRSqrt"
,oR);q4
Lkc;}
gC
q9-1
aD(218,"x[x==Value_t(-1)]"
wX,mS,oR);q4
Lkd;}
qN
dW
hJ
qF
if(gL
y)&&!gL
x*y
aD(13,"y[isEvenInteger(y)&&!isEvenInteger(x*y)]"
wX" "
wR,wP" "
wQ,q01);q4
Lke;}
oJ(212,"y"
wX" "
wR,wQ,q01);q4
Ljl;a2(38,mI
wR,"[x+x]"
wX,oR);q4
Lkf
qL
o4
if(y!=q9
0)||x>=aV(84,"y[y!=Value_t(0)||x>=Value_t(0)] "
wR,"[fp_pow(y,x)]"
,q01);q4
Lkg;}
gF}
oJ(370,wR,"[DO_POWI]"
,oR);if(TryCompilePowi(x))g7}
qM
TailCall_cRDiv:hC
o2
h6
oJ(91,oX
mP,mS,oR);q4
Lkd;qJ
TailCall_cRad:hC
cRad:qI
0
aF(66,"x cRad"
,"[DegreesToRadians(x)]"
,oR);q4
Lkh;}
qM
TailCall_cSin:hC
cSin:qA1(343,m2
aL,aL" "
m3,);q4
Lki
qQ
oJ(58,"x "
aL,"[fp_sin(x)]"
,oR);q4
Lkj;}
qM
TailCall_cSinh:hC
oG
q8
cAcosh:oJ(352,"cAcosh cSinh"
,q11
mI"[Value_t(-1)] "
a6
mD,);hX
Lkk
qL
cAsinh:oJ(231,"cAsinh cSinh"
,,);q4
Lbo
mW(344,m2"cSinh"
,"cSinh "
m3,);q4
Lkl
qQ
oJ(59,"x cSinh"
,"[fp_sinh(x)]"
,oR);q4
Lkm;}
qM
TailCall_cSqr:hC
hH
q8
hP
oJ(391,wP" cSqr"
,"cSqr"
,);q4
Lkn
mW(390,m2"cSqr"
,"cSqr"
,);q4
Lkn
qL
cSqrt:A=mB(220,oT
mD" cSqr"
,"A"
,m8(A)oV);q4
Lbo;aM
TailCall_cSqrt:hC
cSqrt:q8
hH
oJ(15,mI
mD,wP,);q4
Lko
qQ
dI>=aV(60,"x[x>=Value_t(0)] "
mD,"[fp_sqrt(x)]"
,oR);q4
Lkp;aM
hB:hC
cSub:a3(191,w7
qG1,"[Value_t(0)]"
oN,);q4
Lkq
mW(244,m2
qG1,w0,);q4
Lla
qQ
dV
oJ(77,mF
qG1,,oR);q4
Laq;}
mM(78,aO"x cSub"
,"[y-x]"
,q01);q4
Llb;}
oJ(207,"x cSub"
,aE
w0,oR);q4
Llc;h0
dR
oJ(112,"x "
wC" "
w3
qG1,"A "
a6
aX
wC,m8(A)q61);q4
Lld;}
oJ(119,wC" "
w3
qG1,"[DO_STACKPLUS1] A cSub "
wC,m8(A)oV);hX
Lle;}
qJ
TailCall_cTan:hC
cTan:q8
cAtan2:oJ(235,"cAtan2 "
aJ,mR,);q4
Lfp
mW(345,m2
aJ,aJ" "
m3,);q4
Llf
qQ
oJ(62,"x "
aJ,"[fp_tan(x)]"
,oR);q4
Llg;}
qM
TailCall_cTanh:hC
oH
qA1(346,m2"cTanh"
,"cTanh "
m3,);q4
Llh
qQ
oJ(63,"x cTanh"
,"[fp_tanh(x)]"
,oR);q4
Lli;}
qM
TailCall_cTrunc:hC
cTrunc:q8
wL(64,"x cTrunc"
,"[fp_trunc(x)]"
,oR);q4
Llj;dH
oJ(296,"A[IsAlwaysIntegerOpcode(A)] cTrunc"
,"A"
,m8(A)oV);g7
qJ
g4
Default0:;A=oF;if(IsComparisonOpcode(A)){qI
0]q7
hG
qF
oJ(260,wH
oQ,"[x-y] A"
,m8(A)q71);q4
Llk;qK
cAtan:if(d4<fp_const_pi<hN*q9
dJ
oJ(276,"cAtan[fp_abs(x)<fp_const_pi<Value_t>()*Value_t(0.5)]"
mH,"[fp_tan(x)] A"
,m8(A)q61);q4
Lll;qK
cExp:oC
oJ(266,"cExp[x>Value_t(0)]"
mH,"[fp_log(x)] A"
,m8(A)q61);q4
Llm;qK
d9:oC
oJ(267,"cExp2[x>Value_t(0)]"
mH,"[fp_log2(x)] A"
,m8(A)q61);q4
Lln;qK
cLog:qW
oJ(269,oZ
m0
mH,"B [fp_exp(x)] A"
,m8(A)qD1(B)oV);q4
Llo;qK
oI:qW
oJ(271,oZ
wI
mH,"B [fp_pow(Value_t(10),x)] A"
,m8(A)qD1(B)oV);q4
Llp;qK
cLog2:qW
oJ(270,oZ
wO
mH,"B [fp_exp2(x)] A"
,m8(A)qD1(B)oV);q4
Llq;dC
qF
if(y>aV(262,"y[y>Value_t(0)]"
oN
mH,"[x/y] A"
,m8(A)q71);q4
Lma;}
if(y<aV(263,"y[y<Value_t(0)]"
oN
mH,"[x/y] {OppositeComparisonOpcode(A)}"
,m8(A)q71);q4
Lmb;}
a8(261,m2
oQ,aE"{OppositeComparisonOpcode(A)}"
,m8(A)q61);q4
Lmc
qL
dW
oC
hJ
qF
if(y>aV(264,"y[y>Value_t(0)] cPow[x>Value_t(0)]"
mH,"[fp_pow(x,1/y)] A"
,m8(A)q71);q4
Lmd;}
}
qK
oG
oJ(277,"cSinh"
mH,"[fp_asinh(x)] A"
,m8(A)q61);q4
Lme
qL
hH
oC
oJ(265,"cSqr[x>Value_t(0)]"
mH,wP" [fp_sqrt(x)] A"
,m8(A)q61);q4
Lmf;qK
oH
if(d4<aT(278,"cTanh[fp_abs(x)<Value_t(1)]"
mH,"[fp_atanh(x)] A"
,m8(A)q61);q4
Lmg;}
gF}
}
}
if(A
g3
0){B
h1
if(o9
oJ(393,mO"A[A>=VarBegin&&data->ByteCode.size()>0]"
,"B cDup"
,m8(A)qF1(B)oV);q4
Lmh;}
aP
A)){B
h1
if(B
g3
1){C=qU;if(C
hW
D
qX
D==B){oJ(394,"D[D==B] C[C==A] B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]"
,"D C cDup"
,m8(A)qF1(B)q51(C)<<", D"
m6(D)oV);q4
Lmi;}
}
}
}
}
q4
Lmj;Laa:q0-=1;qP
Lmk:q4
TailCall_cAbs;Lab:qE=d4;g7
Lac:qE=fp_acos(w9
Lad:qE=fp_acosh(w9
Lae:qC
q9
4));gQ
Lml:qA
qP
Lmm:gG
dB;Lmn:FP_ReDefinePointers();Lmo:q4
TailCall_cMul;Laf:oA
4
gK
qC
q9
4));Lmp:qD
q6
gU
Lag:qE=x+hY;gQ
qA
qP
Lel:gG
dB;q4
Lmo;Lah:hT
cSub;Lmq:q4
hB;Lai:hI
gY
Lna:FP_ReDefinePointers();Lnb:q4
TailCall_cAdd;Laj:hI
hD
Lnc:d1
Lnd:qG
dL);g7
Lak:gM
gP
2]=q6;qA
d3
Lnc;Lal:hI
gY
q4
Lnd;Lam:hI
hO
Lne:d1
Lnf:qG
B);Lng:gG
cSub;qS
Lmq;Lan:gM
d5=q6;g6
d3
Lne;Lao:hI
hD
q4
Lnf;Lap:hI
Laq:h8
Lbo:qP
g7
Lba:q3
3
d2
qC
x);hS
Lnc;Lbb:qA
o3
Lnc;Lbc:qV
fp_and(x
gN
Lbd:qE=fp_asin(w9
Lbe:qE=fp_asinh(w9
Lbf:qE=fp_atan(w9
Lbg:qV
fp_atan2(y
gO
Lbh:qE=fp_atanh(w9
Lbi:qE=fp_cbrt(w9
Lbj:q1
cFloor);Lnh:gG
cNeg;qS
hA;Lbk:qE=fp_ceil(w9
Lbl:q0-=1;qP
Lni:q4
TailCall_cCos;Lbm:qE=fp_cos(w9
Lbn:h4
TailCall_cCosh;Lbp:q1
cSqr);qC
hY);Lnj:dQ
d1
Lnk:gG
cSqrt;qS
TailCall_cSqrt;Lbq:qE=fp_cosh(w9
Lca:qE=RadiansToDegrees(w9
Lcb:q1
cSec
gU
Lcc:q1
cTan
gU
Lcd:q1
cSin
gU
Lce:qC
q9
0));dP
q6;Lnl:qG
dB);qC
hY);Lnm:dQ
Lnn:gG
cAdd;q4
Lna;Lcf:q1
cNeg);qG
cExp
gU
Lcg:q1
cNeg);qG
d9
gU
Lch:h4
Lel;Lci:q1
cNeg);qG
cPow
gU
Lcj:q1
cCos
gU
Lck:q1
cCsc
gU
Lcl:q1
cCot
gU
Lcm:qA
hD
Lno:gG
cDeg;qS
TailCall_cDeg;Lcn:qA
hD
Lnp:gG
cRad;qS
TailCall_cRad;Lco:qV
y/x;h8
q4
Lml;Lcp:gM
qU
o1
Lnq:q4
TailCall_cDiv;Lcq:qV
y/x;q4
Laq;Lda:qE=hY/x;q4
Lel;Ldb:aS
A);qG
dB);Loa:qS
Lnq;Ldc:q3
3
d2
qG
dB);qC
x);dQ
Lob:gG
cRDiv;qS
TailCall_cRDiv;Ldd:g6
q1
A);qG
dB);qG
B
gJ
Lob;Lde:qU
o1
Loc:q4
TailCall_cEqual;Ldf:qV
fp_equal(y
gO
Ldg:q3
2
o0
cExp);qC
fp_exp(x)gJ
Lmp;Ldh:qE=fp_exp(w9
Ldi:q3
2
o0
d9);qC
fp_exp2(x)gJ
Lmp;Ldj:qE=fp_exp2(w9
Ldk:qC
dA
q9
2)));dQ
Lod:qG
dX
cExp;qS
TailCall_cExp;Ldl:q1
cCeil
gJ
Lnh;Ldm:qE=fp_floor(w9
Ldn:qV
fp_less(x
gN
Ldo:h8
q1
cNeg);Ljg:qG
cAbsNot);g7
Ldp:qE=q9
0.5)/x;m1
q1
dB);Loe:qG
d8);g7
Ldq:qV
fp_lessOrEq(x
gN
Lea:h8
Lin:qP
Lof:qG
cAbsNotNot);g7
Leb:qE=fp_int(w9
Lec:q1
cSec);g7
Led:hT
cTan;Log:q4
TailCall_cTan;Lee:hT
cSin;Loh:q4
TailCall_cSin;Lef:q1
cNeg);gG
cPow;Loi:FP_ReDefinePointers();Loj:q4
TailCall_cPow;Leg:hT
cCos;q4
Lni;Leh:q1
cCsc);g7
Lei:q1
cRSqrt);g7
Lej:q1
cCot);g7
Lek:qE=hY/x;g7
Lem:qE=q9
0.5)/x;m1
q1
dX
cNot;FP_ReDefinePointers();Lok:q4
TailCall_cNot;Len:qV
fp_less(y
gO
Leo:h8
Lio:d3
Ljg;Lep:qV
fp_lessOrEq(y
gO
Leq:h8
q1
cNeg
gJ
Lof;Lfa:q3
2
o0
cLog);qC
dA
x)gJ
Lnm;Lfb:q1
d6
qG
cLog);Lol:qD
dY
gJ
Lnn;Lfc:qE=dA
w9
Lfd:qC
d7
fp_const_e<hN));Lom:dP
q6;q4
Lmm;Lfe:q3
2
o0
oI);qC
d7
x)gJ
Lnm;Lff:q1
d6
qG
oI
gJ
Lol;Lfg:qE=d7
w9
Lfh:qC
hZ
fp_const_e<hN)gJ
Lom;Lfi:q3
2
o0
cLog2);qC
hZ
x)gJ
Lnm;Lfj:q1
d6
qG
cLog2
gJ
Lol;Lfk:qE=hZ
w9
Lfl:qV
fp_max(x
gN
Lfm:qV
fp_min(x
gN
Lfn:qV
fp_mod(y
gO
Lfo:hT
cSqr;Lon:q4
TailCall_cSqr;Lfp:hT
cDiv;q4
Lnq;Lfq:g6
q1
cSqr);q81
Lnh;Lga:aS
cSqr
gJ
Lnh;Lgb:qE=x+hY;gQ
qA
qP
gG
cPow;q4
Loj;Lgc:gQ
qA
d3
Lmo;Lgd:qV
x;Loo:gY
q4
Lmn;Lge:qA
h8
Lop:oA
dZ
Loq:qE1
x);Lpa:hS
Lmn;Lgf:h8
q4
Lop;Lgg:q3
4
o0
B
gJ
Loq;Lgh:hD
q4
Loq;Lgi:m1
q4
Lbo;Lgj:qU
gR
1;d3
Lmo;Lgk:h8
Lpb:g6
qP
g7
Lgl:qE=x+x;q4
Lgc;Lgm:qV
x;gP
4]=q6;q3
4
gK
qE1
y*x);dQ
q81
Lnn;Lgn:qV
x;q3
2
gK
qC
y*x
gJ
Lnm;Lgo:qE=RadiansToDegrees(x
gJ
Lgj;Lgp:qV
y*x;hO
Lpc:qG
dB);Lpd:qG
B);gG
cDiv;q4
Loa;Lgq:gM
d5=q6;g6
d3
Lpc;Lha:qV
y*x;hD
q4
Lpd;Lhb:hO
q4
Lmn;Lhc:q3
4
gK
qC
x+x
gJ
Lpa;Lhd:qA
h8
q4
Lpb;Lhe:qV
y*x;q4
Loo;Lhf:gM
q4
Lgj;Lhg:qV
y*x;hD
Lpe:q81
Lob;Lhh:gM
gP
2]=q6;qA
d3
Lpe;Lhi:qV
y*x;gY
q4
Lob;Lhj:qE=DegreesToRadians(x
gJ
Lgj;Lhk:qV
x;gP
4]=q6;q3
4
gK
qE1
y*x);dQ
q81
Lng;Lhl:qV
y*x;q4
Laq;Lhm:dP
dY;q2-=1;h8
Lpf:gG
cAdd;q4
Lnb;Lhn:h8
d3
Lno;Lho:h8
d3
Lnp;Lhp:h8
d3
Lnh;Lhq:g6
o3
Lpc;Lia:qA
qP
Lpg:qG
cSqr
gJ
Lmn;Lib:q3
3
d2
qC
x);hS
Lpe;Lic:qA
o3
Lpe;Lid:g6
d3
Lpg;Lie:qU
o1
Lph:q4
TailCall_cNEqual;Lif:qV
fp_nequal(y
gO
Lig:gM
q4
Lch;Lih:gM
aS
dX
cSin;qS
Loh;Lii:gM
aS
dX
cSinh;qS
TailCall_cSinh;Lij:gM
aS
dX
cTan;qS
Log;Lik:gM
aS
dX
cTanh;qS
TailCall_cTanh;Lil:gM
g7
Lim:h4
Lok;Lip:hT
dO;q4
Lph;Liq:hT
d0;q4
TailCall_cLessOrEq;Lja:hT
cLess;q4
TailCall_cLess;Ljb:hT
h5;q4
TailCall_cGreaterOrEq;Ljc:hT
hR;q4
TailCall_cGreater;Ljd:hT
dM;q4
Loc;Lje:d3
Loe;Ljf:qE=fp_not(w9
Ljh:qV
fp_or(x
gN
Lji:q3
2
a5
x+x);Lpi:hS
Loi;Ljj:m1
d3
Lod;Ljk:m1
q1
dX
d9;qS
TailCall_cExp2;Ljl:qV
y*x;gY
q4
Loi;Ljm:m1
q1
dB
gJ
Loi;Ljn:qU
gR
1;d3
Loj;Ljo:q3
3
gK
qC
x
gJ
Lpi;Ljp:qE=q9
0
gJ
Lnl;Ljq:h8
d3
Lnk;Lka:h8
qP
gG
cCbrt;qS
TailCall_cCbrt;Lkb:h8
q1
cCbrt);Lpj:gG
cInv;qS
TailCall_cInv;Lkc:h8
q4
Lei;Lkd:h8
d3
Lpj;Lke:qA
q3
3
a5
y*x
gJ
Lpi;Lkf:qE=x+x;q4
Ljn;Lkg:qV
o6
y
gO
Lkh:qE=DegreesToRadians(w9
Lki:q1
cSin
gJ
Lnh;Lkj:qE=fp_sin(w9
Lkk:q1
cSqr);qC
q9-1)gJ
Lnj;Lkl:q1
cSinh
gJ
Lnh;Lkm:qE=fp_sinh(w9
Lkn:h4
Lon;Lko:hT
cAbs;q4
Lmk;Lkp:qE=fp_sqrt(w9
Lkq:qC
q9
0)gJ
Lom;Lla:h4
Lpf;Llb:qV
y-x;q4
Laq;Llc:gM
q4
Lpf;Lld:q3
3
d2
d1
qC
x);hS
Lnd;Lle:aS
A);qG
cSub
gJ
Lnd;Llf:q1
cTan
gJ
Lnh;Llg:qE=fp_tan(w9
Llh:q1
cTanh
gJ
Lnh;Lli:qE=fp_tanh(w9
Llj:qE=fp_trunc(w9
Llk:qV
x-y;Lpk:gY
Lpl:qG
A);g7
Lll:qE=fp_tan(x);Lpm:m1
d3
Lpl;Llm:qE=dA
x
gJ
Lpm;Lln:qE=hZ
x
gJ
Lpm;Llo:qE=fp_exp(x
gJ
Lpm;Llp:qE=o6
q9
10),x
gJ
Lpm;Llq:qE=fp_exp2(x
gJ
Lpm;Lma:qV
x/y;q4
Lpk;Lmb:qV
x/y;gY
Lpn:qG
OppositeComparisonOpcode(A));g7
Lmc:gM
m1
d3
Lpn;Lmd:qV
o6
x,1/y
gJ
Lpk;Lme:qE=fp_asinh(x
gJ
Lpm;Lmf:q3
2
a5
fp_sqrt(x));hS
Lpl;Lmg:qE=fp_atanh(x
gJ
Lpm;Lmh:qD
dY);g7
Lmi:dP
dY;g7
Lmj:qD
oF);g7
g7
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
TailCall_cRDiv;q4
TailCall_cRad;q4
TailCall_cSin;q4
TailCall_cSinh;q4
TailCall_cSqrt;q4
hB;q4
TailCall_cTan;q4
TailCall_cTanh;q4
TailCall_cTrunc;
#endif
#undef FP_ReDefinePointers
#undef FP_TRACE_BYTECODE_OPTIMIZATION
#undef FP_TRACE_OPCODENAME
