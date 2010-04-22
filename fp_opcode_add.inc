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
#define aW "cNeg"
#define aV aW
#define aU ", "mY
#define aT mA", "oN
#define aS <<","aC
#define aR <<","a0","
#define aQ <<","mE
#define aP oD", "oN
#define aO oD","wU
#define aN "cRad"
#define aM qM oC
#define aL dO!=qM
#define aK "cNot"
#define aJ w9" "
#define aI q0 qX
#define aH q0 gZ
#define aG ==A){
#define aF "cSub"
#define aE qH q7 oC
#define aD "A "wA oH
#define aC " "mG w8
#define aB " A"
#define aA qN if(
#define a9 cExp:oC
#define a8 cInv:oC
#define a7 "cSin"
#define a6 "cCsc"
#define a5 "cTan"
#define a4 "cCot"
#define a3 "cSec"
#define a2 wA" "oW
#define a1 mZ Llh
#define a0 " "mG w4
#define mZ );q3
#define mY "C "oE
#define mX "cLess"
#define mW aW" "
#define mV "cSinh"
#define mU )){oC
#define mT w3" "
#define mS gD oC
#define mR "cExp"
#define mQ q8-1)&&x
#define mP hF dP hG
#define mO qU d1 hO
#define mN "[-x]"
#define mM q0[-4]gX
#define mL wH"cDup"
#define mK "cInv"
#define mJ "= "<<
#define mI "cCeil"
#define mH " "oI
#define mG "x "
#define mF " "aK
#define mE " "wH oE
#define mD oJ" "
#define mC qG oC
#define mB g5 oC
#define mA mJ y<<
#define m9 "cMin"
#define m8 "cMax"
#define m7 "cDiv"
#define m6 "cSqrt"
#define m5 qU q7 oC
#define m4 dS){if(
#define m3 wB" x"oH
#define m2 mR" "
#define m1 "cFloor"
#define m0 gU q7 oC
#define wZ gB h2 oC
#define wY "cLog"
#define wX wM mG
#define wW q0[-3]
#define wV "cGreater"
#define wU " "oG
#define wT gX B aG oC
#define wS gC gM>aM
#define wR "cAbsNot"
#define wQ "[y*x]"
#define wP "cLessOrEq"
#define wO "cNotNot"
#define wN oK" "
#define wM "y "
#define wL "cCos"
#define wK "cAbs"
#define wJ "cCosh"
#define wI "cLog10"
#define wH "B "
#define wG "cLog2"
#define wF "cExp2"
#define wE g3 qH gI>aM
#define wD "cGreaterOrEq"
#define wC "cEqual"
#define wB "cAdd"
#define wA "[x]"
#define w9 "cSqr"
#define w8 mJ x oO
#define w7 (x);g1
#define w6 oH" "
#define w5 "A[IsAlwaysIntegerOpcode(A)] "
#define w4 mJ x<<
#define w3 "B[B==A]"
#define w2 " cPow"
#define w1 " "aW
#define w0 A=qH gX
#define oZ g0 w0 qO oC
#define oY "cDup "
#define oX "[DO_STACKPLUS1] "
#define oW "cNEqual"
#define oV "x[x!=Value_t(0)] "
#define oU "x[x>Value_t(0)]"
#define oT oD"\n"
#define oS "B[IsNeverNegativeValueOpcode(B)] "
#define oR "    with "oN
#define oQ "x[x==Value_t(1)] "
#define oP " x[IsIntegerConst(x)]"w2
#define oO <<"\n"
#define oN "A "oE
#define oM "B[IsBinaryOpcode(B)&&!HasInvalidRangesOpcode(B)] "oF
#define oL "A[IsNeverNegativeValueOpcode(A)] "
#define oK "x[x==Value_t(0)]"
#define oJ "A[A>=VarBegin]"
#define oI mG"A[IsComparisonOpcode(A)]"
#define oH " cMul"
#define oG wM mJ y oO
#define oF "A[IsBinaryOpcode(A)&&!HasInvalidRangesOpcode(A)] "oK oH
#define oE mJ FP_TRACE_OPCODENAME
#define oD "    with"a0
#define oC FP_TRACE_BYTECODE_OPTIMIZATION
#define oB cLog10
#define oA cAbs){
#define o9 size()
#define o8 dG gE
#define o7 q5:g2
#define o6 ,x)gH
#define o5 cLess
#define o4 g3 dT
#define o3 g1 qD
#define o2 cAdd
#define o1 fp_pow(
#define o0 q1 q3
#define dZ cSqr)
#define dY hY qZ
#define dX cNEqual
#define dW g6 g5
#define dV Immed.
#define dU TailCall_cEqual
#define dT qU==
#define dS cMul
#define dR TailCall_cTan
#define dQ ,x)gK
#define dP qJ 2;
#define dO gU gI
#define dN TailCall_cSin
#define dM g9 2;q6
#define dL h1 g9 3;
#define dK qK A);qI
#define dJ Default6
#define dI Default5
#define dH TailCall_cAbs
#define dG }if(
#define dF Lce;dG
#define dE cEqual
#define dD opcode
#define dC fp_exp(x)
#define dB fp_exp2(x)
#define dA fp_log(
#define d9 cExp2
#define d8 cNotNot
#define d7 cPow gT
#define d6 =q5;
#define d5 fp_log10(
#define d4 cAbs);
#define d3 fp_abs(x)
#define d2 hW qM
#define d1 d6 q0
#define d0 qH d1-=1;q1
#define hZ tmp-->0;)
#define hY Lbg;
#define hX fp_log2(
#define hW gM==
#define hV q9-2 g7
#define hU g9 2;qV
#define hT Default1
#define hS q5:y=qS
#define hR qC hS
#define hQ 0.5)){
#define hP q6 dS);
#define hO -=2;dP qV
#define hN cSqr:
#define hM Value_t
#define hL data->
#define hK cGreater
#define hJ ==cDup){
#define hI gU q7 qG
#define hH q8 1)
#define hG hP gP
#define hF qA=-x;
#define hE q5;qB dS);
#define hD =gV gX
#define hC 3]hJ
#define hB qK dS);
#define hA TailCall_cSub
#define h9 TailCall_cNeg
#define h8 cLessOrEq
#define h7 break;
#define h6 unsigned
#define h5 hH){
#define h4 gI==h5
#define h3 o2 gT
#define h2 cAbs:
#define h1 2;hZ
#define h0 hL qF
#define gZ -=1;o0
#define gY cGreaterOrEq
#define gX ;if(
#define gW g0 A hD
#define gV q0[0]
#define gU gT gV
#define gT :if(
#define gS gB h3 dT
#define gR cDup:
#define gQ TailCall_cNotNot
#define gP dD=
#define gO GetDegreesToRadiansFactor<hM>()){
#define gN GetRadiansToDegreesFactor<hM>()){
#define gM if(x
#define gL isEvenInteger(
#define gK ;q3 Lak;
#define gJ ,y)gK
#define gI q7 gM
#define gH ;q3 Lai;
#define gG ,y)gH
#define gF IsLogicalOpcode(A)){
#define gE IsUnaryOpcode(
#define gD B qR B aG
#define gC qZ o7
#define gB q9-1 g7
#define gA >=VarBegin&&qP o9>
#define g9 hL q4
#define g8 qP push_back(q5);
#define g7 ]){qZ
#define g6 q9 0 g7
#define g5 cNeg:
#define g4 IsAlwaysIntegerOpcode(A)){
#define g3 dS gT
#define g2 x=qA;
#define g1 return;
#define g0 default:
#define qZ case
#define qY incStackPtr();--StackPtr;q3
#define qX -=1;q1 gP
#define qW >=VarBegin){
#define qV hZ q1
#define qU q0[-2]
#define qT &&!HasInvalidRangesOpcode(
#define qS q2[-1]
#define qR =qU gX
#define qQ B qR IsNeverNegativeValueOpcode(B)){
#define qP hL ByteCode.
#define qO IsNeverNegativeValueOpcode(A)){
#define qN q5){y=qS;
#define qM q8 0)){
#define qL FP_ReDefinePointers();q3
#define qK AddFunctionOpcode(
#define qJ for(h6 tmp=
#define qI hL dV push_back(
#define qH q0[-1]
#define qG if(qH==qN
#define qF dV pop_back();
#define qE q3 Default0;
#define qD }}qE
#define qC }h7 qZ
#define qB q1 qK
#define qA q2[0]
#define q9 switch(q0[
#define q8 hM(
#define q7 ==q5){g2
#define q6 hZ qB
#define q5 cImmed
#define q4 qF qJ
#define q3 goto
#define q2 ImmedPtr
#define q1 qP pop_back();
#define q0 ByteCodePtr
h6*q0;hM*q2;
#define FP_ReDefinePointers() q0=!qP empty()?&hL ByteCode[0]+qP o9-1:0;q2=!hL dV empty()?&hL Immed[0]+hL dV o9-1:0;
FP_ReDefinePointers();
#if(!FP_FLOAT_VERSION)
hM
x;h6
A;hM
y;h6
B;h6
C;h6
D;switch(dD){dH:qZ
h2
dW
oC(197,mW
wK,wK,);q3
Laa;gC
oC(40,mG
wK,"[fp_abs(x)]"
,oT);q3
Lab;gW
qO
oC(258,oL
wK,"A"
,oR(A)oO);o3
TailCall_cAdd:qZ
o2:g6
gR
gS
cDup){oC(147,oY
wB" "
oY
wB,"[Value_t(4)]"
oH,);q3
Lac;qC
o4
o2){if(q0[-hC
oC(148,oY
wB
w6
oY
wB,"cMul [Value_t(4)]"
oH,);q3
Lad;}
}
h7
qC
g3
qH
q7
if(qU
hJ
oC(150,oY"x"
w6
wB,"[x+Value_t(1)]"
oH,oT);q3
Lae;}
qC
mB(204,mW
wB,aF,);q3
Laf;gC
gS
qN
oC(141,wM
wB" "
mG
wB,"[y+x] "
wB,aO);q3
Lag;hR;oC(76,wX
wB,"[y+x]"
,aO);q3
Lah;}
d2
oC(75,wN
wB,,oT)gH}
h7}
qE
TailCall_cAnd:qZ
cAnd
hI
oC(130,wX"cAnd"
,"[fp_and(x,y)]"
,aO);q3
Laj;qD
TailCall_cDiv:qZ
cDiv:g6
gR
oC(120,oY
m7,"[Value_t(0)]"
w6"[Value_t(1)] "
wB,)gK
gC
gM!=qM
gB
mB(138,mW
oV
m7,mN" "
m7,oT);q3
Lal;qZ
hS;oC(73,wM
oV
m7,"[y/x]"
,aO);q3
Lam;}
}
hW
h5
oC(72,oQ
m7,,oT)gH}
h7}
qE
dU:qZ
dE
gU
h4
w0
gF
oC(286,"A[IsLogicalOpcode(A)] "
oQ
wC,"A"
,aP(A)oO)gH}
}
mC(128,wX
wC,"[fp_equal(y,x)]"
,aO);q3
Lan;}
d2
wZ(216,wK" "
wN
wC,wA" "
wC,oT);q3
Lao;qZ
hN
oC(218,aJ
wN
wC,wA" "
wC,oT);q3
Lao;}
oC(276,wN
wC,aK,oT);q3
Lap;qD
TailCall_cGreater:qZ
hK
dO==qM
wZ(278,wK" "
wN
wV,wO,oT);q3
Laq;oZ(282,oL
wN
wV,"A "
wO,aP(A)oO);q3
Lba;}
}
}
mC(126,wX
wV,"[fp_less(x,y)]"
,aO);q3
Lbb;qD
TailCall_cGreaterOrEq:qZ
gY
gU
h4
wZ(279,wK" "
oQ
wD,wO,oT);q3
Laq;oZ(283,oL
oQ
wD,"A "
wO,aP(A)oO);q3
Lba;}
}
}
mC(127,wX
wD,"[fp_lessOrEq(x,y)]"
,aO);q3
Lbc;qD
TailCall_cInv:qZ
cInv:g6
a8(198,mK" "
mK,,);q3
Lbd;qZ
cSqrt:oC(67,m6" "
mK,"cRSqrt"
,);q3
Lbe;gC
gM!=aM(69,oV
mK,"[Value_t(1)/x]"
,oT);q3
Lbf;}
h7}
qE
TailCall_cLess:qZ
o5
gU
h4
wZ(280,wK" "
oQ
mX,aK,oT);q3
hY
oZ(284,oL
oQ
mX,"A"
mF,aP(A)oO);q3
Lap;}
}
}
mC(124,wX
mX,"[fp_less(y,x)]"
,aO);q3
Lbh;qD
TailCall_cLessOrEq:qZ
h8
dO==qM
wZ(281,wK" "
wN
wP,aK,oT);q3
hY
oZ(285,oL
wN
wP,"A"
mF,aP(A)oO);q3
Lap;}
}
}
mC(125,wX
wP,"[fp_lessOrEq(y,x)]"
,aO);q3
Lbi;qD
TailCall_cMax:qZ
cMax:g6
gR
oC(110,oY
m8,,);q3
Lbd;gC
mC(82,wX
m8,"[fp_max(x,y)]"
,aO);q3
Lbj;}
h7
gW
A
qW
gB
gR
mS(112,mT
oY
mD
m8,mL,oR(A)aQ(B)oO);q3
Lbd;qC
cMax:mS(114,mT
m8" "
mD
m8,wH
m8,oR(A)aQ(B)oO);q3
Lbd;}
h7}
qD
TailCall_cMin:qZ
cMin:g6
gR
oC(109,oY
m9,,);q3
Lbd;gC
mC(81,wX
m9,"[fp_min(x,y)]"
,aO);q3
Lbk;}
h7
gW
A
qW
gB
gR
mS(111,mT
oY
mD
m9,mL,oR(A)aQ(B)oO);q3
Lbd;qC
cMin:mS(113,mT
m9" "
mD
m9,wH
m9,oR(A)aQ(B)oO);q3
Lbd;}
h7}
qD
TailCall_cMod:qZ
cMod
aL
mC(74,wM
oV"cMod"
,"[fp_mod(y,x)]"
,aO);q3
Lbl;}
qD
TailCall_cMul:qZ
dS:g6
h3
qH==q5){y=qA;A
qR
A
qW
if(wW==q5){x=qS;oC(245,mG
mD
wM
wB
oH,wA
aB
w6
wQ" "
wB,"    with "
wM
aT(A)aS);q3
Lbm;}
}
}
q3
hT;qZ
gR
oC(359,"cDup"
oH,w9,);q3
Lbn;qZ
a8(203,mK
oH,m7,);q3
Lbo;qZ
g5
w0
A
qW
hV
dS:B=wW
wT(351,w3
w6
oJ
w1
oH,wH
w9
oH
w1,oR(A)aQ(B)oO);q3
Lbp;}
q3
Default2;g0
Default2:;mS(345,mT
oJ
w1
oH,wH
w9
w1,oR(A)aQ(B)oO);q3
Lbq;}
}
}
q3
hT;qZ
d7
qH
q7
if(qU
hJ
oC(151,oY"x"
w2
oH,"[x+Value_t(1)]"
w2,oT);q3
Lca;}
}
q3
hT;gC
d2
gB
dS:A
qR
A
qW
oC(94,oJ
w6
oK
oH,wA
oH,aP(A)oO);q3
Lcb;}
q3
Default3;g0
Default3:;w0
IsBinaryOpcode(A)qT
A)){hV
hS;oC(93,wM
oF,wA
oH,aP(A)<<","
wU);q3
Lcc;g0
B
qR
IsBinaryOpcode(B)qT
B)){q9-3
g7
hS;oC(97,wM
oM,aD,aP(A)aQ(B)<<","
wU);q3
Lcd;g0
C=wW
gX
C
qW
oC(95,"C[C>=VarBegin] "
oM,aD,aP(A)aQ(B)<<aU(C)oO);q3
dF
gE
C)qT
C
mU(96,"C[IsUnaryOpcode(C)&&!HasInvalidRangesOpcode(C)] "
oM,wH
aD,aP(A)aQ(B)<<aU(C)oO);q3
Lcf;}
}
dG
B
qW
oC(91,"B[B>=VarBegin] "
oF,wA
oH,aP(A)aQ(B)oO);q3
Lcb;o8
B)qT
B
mU(92,"B[IsUnaryOpcode(B)&&!HasInvalidRangesOpcode(B)] "
oF,aD,aP(A)aQ(B)oO);q3
Lcg;}
}
dG
A
qW
oC(89,mD
oK
oH,wA,aP(A)oO);q3
Lch;o8
A)qT
A
mU(90,"A[IsUnaryOpcode(A)&&!HasInvalidRangesOpcode(A)] "
oK
oH,wA
oH,aP(A)oO);q3
Lci;}
}
}
gB
o2:hV
gR
oC(155,oY
m3,"[x+x]"
oH,oT);q3
Lcj;qZ
g3
wW==qN
A=mM
A
qW
oC(243,mD"y"
w6
m3,wA
w6"A "
wQ
w6
wB,oD", "
wM
aT(A)oO);q3
Lck;}
hR;oC(242,wM
m3,wA
w6
wQ" "
wB,aO);q3
Lcl;qC
dS:hV
h3
q0[-hC
oC(157,oY
wB
w6"x"
oH,"cMul [x+x]"
oH,oT);q3
Lcm;hR
gX
y*x==h5
oC(145,"y[y*x==Value_t(1)]"
w6"x"
oH,,aO);q3
Lcn;}
oC(146,"y"
w6"x"
oH,wQ
oH,aO);q3
Lco;qC
mB(135,mW"x"
oH,mN
oH,oT);q3
Lcp;qZ
cSub
gT
dT
m4
wW==qN
A=mM
A
qW
oC(244,mD"y"
w6
aF" x"
oH,wA
w6"A "
wQ
w6
aF,oD", "
wM
aT(A)oO);q3
Lcq;}
}
hR;oC(71,wM"x"
oH,wQ,aO);q3
Lda;}
hW
h5
oC(70,"x[x==Value_t(1)]"
oH,,oT)gH}
hW
q8
2
mU(100,"x[x==Value_t(2)]"
oH,oY
wB,oT);q3
Ldb;}
hW
q8-1
mU(137,"x[x==Value_t(-1)]"
oH,aV,oT);q3
Ldc;}
h7
g0
hT:;A
hD
A
qW
gB
dS:hV
g5
B=wW
wT(348,w3
w1
w6
oJ
oH,wH
w9
oH
w1,oR(A)aQ(B)oO);q3
Lbp;}
q3
Default4;g0
Default4:;mS(339,w3
w6
oJ
oH,wH
w9
oH,oR(A)aQ(B)oO);q3
Ldd;}
}
q3
dI;qZ
g5
mS(342,w3
w1" "
oJ
oH,wH
w9
w1,oR(A)aQ(B)oO);q3
Lbq;}
q3
dI;g0
dI:;B=qH
wT(336,mT
oJ
oH,wH
w9,oR(A)aQ(B)oO);q3
Lbn;}
}
o8
A)){B=qH
gX
B
gA
1){if(dT
dS){C=wW
gX
C
aG
D=mM
D==B){oC(358,"D[D==B] C[C==A]"
w6"B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]"
oH,"D C "
w9
oH,oR(A)aQ(B)<<aU(C)<<", D "
oE(D)oO);q3
Lde;}
}
}
}
qD
TailCall_cNEqual:qZ
dX
gU
h4
w0
gF
oC(287,"A[IsLogicalOpcode(A)] "
oQ
oW,"A"
mF,aP(A)oO);q3
Lap;}
}
mC(129,wX
oW,"[fp_nequal(y,x)]"
,aO);q3
Ldf;}
d2
wZ(217,wK" "
wN
oW,a2,oT);q3
Ldg;qZ
hN
oC(219,aJ
wN
oW,a2,oT);q3
Ldg;}
oC(277,wN
oW,wO,oT);q3
Lba;qD
h9:qZ
g5
g6
g3
aE(136,"x"
oH
w1,mN
oH,oT);q3
Ldh;qC
mB(199,aV
w1,,);q3
Lbd;gC
oC(68,"x"
w1,mN,oT);q3
Ldi;}
qE
TailCall_cNot:qZ
cNot:g6
h2
oC(249,wK
mF,aK,);q3
Ldj;qZ
cAbsNot:w0
gF
oC(254,"A[IsLogicalOpcode(A)] "
wR
mF,"A"
,oR(A)oO);q3
Lbd;dG
A!=q5){oC(255,"A[A!=cImmed] "
wR
mF,"A cAbsNotNot"
,oR(A)oO);q3
Ldk;}
q3
dJ;qZ
cAbsNotNot:oC(252,"cAbsNotNot"
mF,wR,);q3
Ldl;qZ
h3
aE(289,mG
wB
mF,wA" "
wC,oT);q3
Ldm;}
q3
dJ;qZ
dE:oC(213,wC
mF,oW,);q3
Ldn;qZ
hK:oC(211,wV
mF,wP,);q3
Ldo;qZ
gY:oC(212,wD
mF,mX,);q3
Ldp;qZ
o5:oC(209,mX
mF,wD,);q3
Ldq;qZ
h8:oC(210,wP
mF,wV,);q3
Lea;qZ
dX:oC(214,oW
mF,wC,);q3
Leb;qZ
mB(248,aV
mF,aK,);q3
Ldj;qZ
cNot:oC(250,aK
mF,wO,);q3
Lec;qZ
d8:oC(251,wO
mF,aK,);q3
Ldj;gC
oC(85,"x"
mF,"[fp_not(x)]"
,oT);q3
Led;g0
dJ:;A
hD
qO
oC(256,oL
aK,"A "
wR,oR(A)oO);q3
Lee;qD
gQ:qZ
d8
gU==o2){if(aE(288,mG
wB" "
wO,a2,oT);q3
Lef;qD
TailCall_cOr:qZ
cOr
hI
oC(131,wX"cOr"
,"[fp_or(x,y)]"
,aO);q3
Leg;qD
TailCall_cSqr:qZ
hN
g6
h2
oC(354,wK" "
w9,w9,);q3
Leh;qZ
mB(353,mW
w9,w9,);q3
Leh;}
qE
hA:qZ
cSub:g6
gR
oC(152,oY
aF,"[Value_t(0)]"
oH,);q3
Lei;qZ
mB(205,mW
aF,wB,);q3
Lej;gC
d2
oC(77,wN
aF,,oT)gH}
mC(78,wX
aF,"[y-x]"
,aO);q3
Lek;}
oC(168,mG
aF,mN" "
wB,oT);q3
Lel;}
qE
g0
Default0:;A=dD
gX
IsComparisonOpcode(A)){if(gV
q7
gS
qN
oC(221,wM
wB
mH,"[x-y]"
aB,oR(A)aR
wU);q3
Lem;qC
mB(222,aV
mH,mN" {OppositeComparisonOpcode(A)}"
,oR(A)aS);q3
Len;}
}
dG
A
gA
0){B
hD
B
aG
oC(356,mT"A[A>=VarBegin&&data->ByteCode.size()>0]"
,mL,oR(A)aQ(B)oO);q3
Leo;}
o8
A)){B
hD
B
gA
1){C=qH
gX
C
aG
D
qR
D==B){oC(357,"D[D==B] C[C==A] B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]"
,"D C cDup"
,oR(A)aQ(B)<<aU(C)<<", D "
oE(D)oO);q3
Lep;}
}
}
}
}
q3
Leq;Laa:aH
dH;Lab:qA=d3;g1
Lac:qI
q8
4));mO
Lfa:gP
dS;Lfb:FP_ReDefinePointers();Lfc:q3
TailCall_cMul;Lad:qJ
4;hP
qI
q8
4));g8
q3
Lfa;Lae:qA=x+hH;mO
Lfd:gP
dS;q3
Lfc;Laf:aI
cSub;Lfe:q3
hA;Lag:qS=y+x;hU
Lff:FP_ReDefinePointers();Lfg:q3
TailCall_cAdd;Lah:qS=y+x;Lai:h0
Lbd:q1
g1
Laj:qS=fp_and(x
gG
Lak:qI
q8
0));gV
d6
hB
qI
hH);Lfh:g8
Lfi:gP
o2;q3
Lff;Lal:hF
d0
Lfj:q3
TailCall_cDiv;Lam:qS=y/x
gH
Lan:qS=fp_equal(y
o6
Lao:d0
Lfk:q3
dU;Lap:h0
q1
Lfl:gP
cNot;FP_ReDefinePointers();Lfm:q3
TailCall_cNot;Laq:hU
Lfn:gP
d8;FP_ReDefinePointers();Lfo:q3
gQ;Lba:h0
o0
Lfn;Lbb:qS=fp_less(x
gG
Lbc:qS=fp_lessOrEq(x
gG
Lbe:qB
cRSqrt);g1
Lbf:qA=hH/x;g1
Lbg:hU
q3
Lfl;Lbh:qS=fp_less(y
o6
Lbi:qS=fp_lessOrEq(y
o6
Lbj:qS=fp_max(x
gG
Lbk:qS=fp_min(x
gG
Lbl:qS=fp_mod(y
o6
Lbm:qS=x;g9
3;q6
A);Lfp:hB
qI
y*x
mZ
Lfh;Lbn:aI
cSqr;Lfq:q3
TailCall_cSqr;Lbo:aI
cDiv;q3
Lfj;Lbp:qJ
3;q6
dZ;hB
Lga:gP
cNeg;qL
h9;Lbq:dP
q6
dZ;q3
Lga;Lca:qA=x+hH;qU
d6
dP
q6
cPow);g1
Lcb:mO
q3
Lfc;Lcc:qS=x;Lgb:hU
q3
Lfb;Lcd:qJ
h1
h0
Lgc:qJ
4;qV
Lgd:dK
x);Lge:g8
q3
Lfb;Lce:h0
q3
Lgc;Lcf:g9
4;q6
B
mZ
Lgd;Lcg:g9
3;qV
q3
Lgd;Lch:qH
d6
q3
Lbd;Lci:qH
d1
gZ
Lfc;Lcj:qA=x+x;q3
Lcb;Lck:qS=x;q0[-4]d6
g9
4;hP
dK
y*x);g8
hB
q3
Lfi;Lcl:qS=x;hU
q3
Lfp;Lcm:g9
4;hP
qI
x+x
mZ
Lge;Lcn:qJ
dL
qV
g1
Lco:qS=y*x;q3
Lgb;Lcp:hF
q3
Lci;Lcq:qS=x;q0[-4]d6
g9
4;hP
dK
y*x);g8
hB
gP
cSub;qL
Lfe;Lda:qS=y*x
gH
Ldb:gV=cDup;q2-=1;h0
Lgf:gP
o2;q3
Lfg;Ldc:h0
o0
Lga;Ldd:dP
qV
Lgg:qK
dZ;q3
Lfb;Lde:qJ
3;qV
q3
Lgg;Ldf:qS=fp_nequal(y
o6
Ldg:d0
Lgh:q3
TailCall_cNEqual;Ldh:hF
aH
Lfd;Ldi:hF
g1
Ldj:aH
Lfm;Ldk:qB
cAbsNotNot);g1
Ldl:q1
Lee:qK
cAbsNot);g1
Ldm:qA=x;Leb:aI
dE;q3
Lfk;Ldn:aI
dX;q3
Lgh;Ldo:aI
h8;q3
TailCall_cLessOrEq;Ldp:aI
o5;q3
TailCall_cLess;Ldq:aI
gY;q3
TailCall_cGreaterOrEq;Lea:aI
hK;q3
TailCall_cGreater;Lec:aI
d8;q3
Lfo;Led:qA=fp_not
w7
Lef:qA=x;q3
Ldn;Leg:qS=fp_or(x
gG
Leh:aH
Lfq;Lei:qI
q8
0));gV
d6
q3
Lfa;Lej:aH
Lgf;Lek:qS=y-x
gH
Lel:hF
q3
Lgf;Lem:qS=x-y;dM
A);g1
Len:hF
qH
d6
qB
OppositeComparisonOpcode(A));g1
Leo:qP
push_back(cDup);g1
Lep:gV=cDup;g1
Leq:qP
push_back(dD);g1
g1
q3
TailCall_cAnd;q3
TailCall_cInv;q3
TailCall_cMax;q3
TailCall_cMin;q3
TailCall_cMod;q3
h9;q3
gQ;q3
TailCall_cOr;q3
hA;
#endif
#if(FP_FLOAT_VERSION)
hM
x;h6
A;hM
y;h6
B;h6
C;h6
D;switch(dD){dH:qZ
h2
dW
oC(197,mW
wK,wK,);q3
Laa;gC
oC(40,mG
wK,"[fp_abs(x)]"
,oT);q3
Lab;gW
qO
oC(258,oL
wK,"A"
,oR(A)oO);o3
TailCall_cAcos:qZ
cAcos
dO>=mQ<=h5
oC(44,"x[x>=Value_t(-1)&&x<=Value_t(1)] cAcos"
,"[fp_acos(x)]"
,oT);q3
Lac;qD
TailCall_cAcosh:qZ
cAcosh
dO>=h5
oC(41,"x[x>=Value_t(1)] cAcosh"
,"[fp_acosh(x)]"
,oT);q3
Lad;qD
TailCall_cAdd:qZ
o2:g6
gR
gS
cDup){oC(147,oY
wB" "
oY
wB,"[Value_t(4)]"
oH,);q3
Lae;qC
o4
o2){if(q0[-hC
oC(148,oY
wB
w6
oY
wB,"cMul [Value_t(4)]"
oH,);q3
Laf;}
}
h7
qC
g3
qH
q7
if(qU
hJ
oC(150,oY"x"
w6
wB,"[x+Value_t(1)]"
oH,oT);q3
Lag;}
qC
mB(204,mW
wB,aF,);q3
Lah;gC
gS
qN
oC(141,wM
wB" "
mG
wB,"[y+x] "
wB,aO)gH
hR;oC(76,wX
wB,"[y+x]"
,aO);q3
Laj;}
d2
oC(75,wN
wB,,oT)gK}
h7}
qE
TailCall_cAnd:qZ
cAnd
hI
oC(130,wX"cAnd"
,"[fp_and(x,y)]"
,aO);q3
Lal;qD
TailCall_cAsin:qZ
cAsin
dO>=mQ<=h5
oC(45,"x[x>=Value_t(-1)&&x<=Value_t(1)] cAsin"
,"[fp_asin(x)]"
,oT);q3
Lam;qD
TailCall_cAsinh:qZ
cAsinh
m0(42,mG"cAsinh"
,"[fp_asinh(x)]"
,oT);q3
Lan;}
qE
TailCall_cAtan:qZ
cAtan
m0(46,mG"cAtan"
,"[fp_atan(x)]"
,oT);q3
Lao;}
qE
TailCall_cAtan2:qZ
cAtan2
hI
oC(83,wX"cAtan2"
,"[fp_atan2(y,x)]"
,aO);q3
Lap;qD
TailCall_cAtanh:qZ
cAtanh
dO>mQ<h5
oC(43,"x[x>Value_t(-1)&&x<Value_t(1)] cAtanh"
,"[fp_atanh(x)]"
,oT);q3
Laq;qD
TailCall_cCbrt:qZ
cCbrt
m0(47,mG"cCbrt"
,"[fp_cbrt(x)]"
,oT);q3
Lba;}
qE
TailCall_cCeil:qZ
cCeil:dW
oC(267,mW
mI,m1
w1,);q3
Lbb;gC
oC(48,mG
mI,"[fp_ceil(x)]"
,oT);q3
Lbc;gW
g4
oC(261,w5
mI,"A"
,oR(A)oO);o3
TailCall_cCos:qZ
cCos:g6
h2
oC(302,wK" "
wL,wL,);q3
Lbd;qZ
mB(304,mW
wL,wL,);q3
Lbd;gC
oC(49,mG
wL,"[fp_cos(x)]"
,oT);q3
Lbe;}
qE
TailCall_cCosh:qZ
cCosh:g6
h2
oC(303,wK" "
wJ,wJ,);q3
Lbf;qZ
cAcosh:oC(193,"cAcosh "
wJ,,);q3
dY
cAsinh:oC(316,"cAsinh "
wJ,oX
aJ"[Value_t(1)] "
wB" "
m6,);qY
Lbh;qZ
mB(305,mW
wJ,wJ,);q3
Lbf;gC
oC(50,mG
wJ,"[fp_cosh(x)]"
,oT);q3
Lbi;}
qE
TailCall_cDeg:qZ
cDeg
m0(65,mG"cDeg"
,"[RadiansToDegrees(x)]"
,oT);q3
Lbj;}
qE
TailCall_cDiv:qZ
cDiv:g6
cCos:oC(319,wL" "
m7,a3
oH,);q3
Lbk;qZ
cCot:oC(323,a4" "
m7,a5
oH,);q3
Lbl;qZ
cCsc:oC(321,a6" "
m7,a7
oH,);q3
Lbm;qZ
gR
oC(120,oY
m7,"[Value_t(0)]"
w6"[Value_t(1)] "
wB,);q3
Lbn;qZ
a9(273,m2
m7,mW
mR
oH,);q3
Lbo;qZ
d9:oC(274,wF" "
m7,mW
wF
oH,);q3
Lbp;qZ
a8(202,mK" "
m7,"cMul"
,);q3
Lbq;qZ
cPow:oC(272,"cPow "
m7,aV
w2
oH,);q3
Lca;qZ
cSec:oC(322,a3" "
m7,wL
oH,);q3
Lcb;qZ
cSin:oC(318,a7" "
m7,a6
oH,);q3
Lcc;qZ
cTan:oC(320,a5" "
m7,a4
oH,);q3
Lcd;gC
hW
h5
oC(72,oQ
m7,,oT)gK}
gM!=qM
gB
o4
aA(y/x)==gN
oC(159,"y[(y/x)==GetRadiansToDegreesFactor<Value_t>()]"
w6
oV
m7,"cDeg"
,aO);q3
dF(y/x)==gO
oC(160,"y[(y/x)==GetDegreesToRadiansFactor<Value_t>()]"
w6
oV
m7,aN,aO);q3
Lcf;}
oC(161,"y"
w6
oV
m7,"[y/x]"
oH,aO);q3
Lcg;qC
mB(138,mW
oV
m7,mN" "
m7,oT);q3
Lch;qZ
hS;oC(73,wM
oV
m7,"[y/x]"
,aO);q3
Lci;}
oC(163,oV
m7,"[Value_t(1)/x]"
oH,oT);q3
Lcj;}
h7}
qE
dU:qZ
dE
dO==qM
wZ(216,wK" "
wN
wC,wA" "
wC,oT);q3
Lck;qZ
hN
oC(218,aJ
wN
wC,wA" "
wC,oT);q3
Lck;}
}
mC(128,wX
wC,"[fp_equal(y,x)]"
,aO);q3
Lcl;qD
TailCall_cExp:qZ
cExp:g6
h3
aE(269,mG
wB" "
mR,m2"[fp_exp(x)]"
oH,oT);q3
Lcm;qC
cLog:w0
qO
oC(183,oL
wY" "
mR,"A"
,oR(A)oO);q3
hY
qC
o7
oC(51,mG
mR,"[fp_exp(x)]"
,oT);q3
Lcn;}
qE
TailCall_cExp2:qZ
d9:g6
h3
aE(270,mG
wB" "
wF,wF" [fp_exp2(x)]"
oH,oT);q3
Lco;qC
cLog2:w0
qO
oC(184,oL
wG" "
wF,"A"
,oR(A)oO);q3
hY
qC
o7
oC(52,mG
wF,"[fp_exp2(x)]"
,oT);q3
Lcp;}
oC(362,wF,oX"[fp_log(Value_t(2))]"
w6
mR,);qY
Lcq;TailCall_cFloor:qZ
cFloor:dW
oC(266,mW
m1,mI
w1,);q3
Lda;gC
oC(53,mG
m1,"[fp_floor(x)]"
,oT);q3
Ldb;gW
g4
oC(260,w5
m1,"A"
,oR(A)oO);o3
TailCall_cGreater:qZ
hK
hI
oC(126,wX
wV,"[fp_less(x,y)]"
,aO);q3
Ldc;}
hW
q8-hQ
oC(296,"x[x==Value_t(-0.5)] "
wV,mW
wR,oT);q3
Ldd;qD
TailCall_cGreaterOrEq:qZ
gY
aL
if(qH==oA
oC(292,wK" "
oV
wD,"[Value_t(0.5)/x]"
w6
wO,oT);q3
Lde;}
}
mC(127,wX
wD,"[fp_lessOrEq(x,y)]"
,aO);q3
Ldf;}
hW
q8
hQ
oC(295,"x[x==Value_t(0.5)] "
wD,"cAbsNotNot"
,oT);q3
Ldg;qD
TailCall_cInt:qZ
cInt:g6
o7
oC(54,mG"cInt"
,"[fp_int(x)]"
,oT);q3
Ldh;gW
g4
oC(262,w5"cInt"
,"A"
,oR(A)oO);o3
TailCall_cInv:qZ
cInv:g6
cCos:oC(325,wL" "
mK,a3,);q3
Ldi;qZ
cCot:oC(329,a4" "
mK,a5,);q3
Ldj;qZ
cCsc:oC(327,a6" "
mK,a7,);q3
Ldk;qZ
a8(198,mK" "
mK,,);q3
dY
cPow:oC(200,"cPow "
mK,aV
w2,);q3
Ldl;qZ
cSec:oC(328,a3" "
mK,wL,);q3
Ldm;qZ
cSin:oC(324,a7" "
mK,a6,);q3
Ldn;qZ
cSqrt:oC(67,m6" "
mK,"cRSqrt"
,);q3
Ldo;qZ
cTan:oC(326,a5" "
mK,a4,);q3
Ldp;gC
gM!=aM(69,oV
mK,"[Value_t(1)/x]"
,oT);q3
Ldq;}
h7}
qE
TailCall_cLess:qZ
o5
aL
if(qH==oA
oC(291,wK" "
oV
mX,"[Value_t(0.5)/x]"
w6
aK,oT);q3
Lea;}
}
mC(124,wX
mX,"[fp_less(y,x)]"
,aO);q3
Leb;}
hW
q8
hQ
oC(294,"x[x==Value_t(0.5)] "
mX,wR,oT);q3
Lec;qD
TailCall_cLessOrEq:qZ
h8
hI
oC(125,wX
wP,"[fp_lessOrEq(y,x)]"
,aO);q3
Led;}
hW
q8-hQ
oC(297,"x[x==Value_t(-0.5)] "
wP,mW"cAbsNotNot"
,oT);q3
Lee;qD
TailCall_cLog:qZ
cLog:g6
a9(186,m2
wY,,);q3
dY
wE(374,oU
w6
wY,wY" [fp_log(x)] "
wB,oT);q3
Lef;}
qC
hN
oC(103,aJ
wY,wK" "
wY" "
oY
wB,);q3
Leg;wS(55,oU" "
wY,"[fp_log(x)]"
,oT);q3
Leh;}
h7}
qE
TailCall_cLog10:qZ
oB:g6
a9(364,m2
wI,oX"[fp_log10(fp_const_e<Value_t>())]"
oH,);qY
Lei;qZ
wE(375,oU
w6
wI,wI" [fp_log10(x)] "
wB,oT);q3
Lej;}
qC
hN
oC(105,aJ
wI,wK" "
wI" "
oY
wB,);q3
Lek;wS(56,oU" "
wI,"[fp_log10(x)]"
,oT);q3
Lel;}
h7}
qE
TailCall_cLog2:qZ
cLog2:g6
a9(363,m2
wG,oX"[fp_log2(fp_const_e<Value_t>())]"
oH,);qY
Lem;qZ
d9:oC(187,wF" "
wG,,);q3
dY
wE(373,oU
w6
wG,wG" [fp_log2(x)] "
wB,oT);q3
Len;}
qC
hN
oC(104,aJ
wG,wK" "
wG" "
oY
wB,);q3
Leo;wS(57,oU" "
wG,"[fp_log2(x)]"
,oT);q3
Lep;}
h7}
qE
TailCall_cMax:qZ
cMax:g6
gR
oC(110,oY
m8,,);q3
hY
gC
mC(82,wX
m8,"[fp_max(x,y)]"
,aO);q3
Leq;}
h7
gW
A
qW
gB
gR
mS(112,mT
oY
mD
m8,mL,oR(A)aQ(B)oO);q3
hY
qC
cMax:mS(114,mT
m8" "
mD
m8,wH
m8,oR(A)aQ(B)oO);q3
hY}
h7}
qD
TailCall_cMin:qZ
cMin:g6
gR
oC(109,oY
m9,,);q3
hY
gC
mC(81,wX
m9,"[fp_min(x,y)]"
,aO);q3
Lfa;}
h7
gW
A
qW
gB
gR
mS(111,mT
oY
mD
m9,mL,oR(A)aQ(B)oO);q3
hY
qC
cMin:mS(113,mT
m9" "
mD
m9,wH
m9,oR(A)aQ(B)oO);q3
hY}
h7}
qD
TailCall_cMod:qZ
cMod
aL
mC(74,wM
oV"cMod"
,"[fp_mod(y,x)]"
,aO);q3
Lfb;}
qD
TailCall_cMul:qZ
dS:g6
h3
qH==q5){y=qA;A
qR
A
qW
if(wW==q5){x=qS;oC(245,mG
mD
wM
wB
oH,wA
aB
w6
wQ" "
wB,"    with "
wM
aT(A)aS);q3
Lfc;}
}
}
q3
hT;qZ
gR
oC(359,"cDup"
oH,w9,);q3
Lfd;qZ
a8(203,mK
oH,m7,);q3
Lfe;qZ
g5
w0
A
qW
hV
dS:B=wW
wT(351,w3
w6
oJ
w1
oH,wH
w9
oH
w1,oR(A)aQ(B)oO);q3
Lff;}
q3
Default2;g0
Default2:;mS(345,mT
oJ
w1
oH,wH
w9
w1,oR(A)aQ(B)oO);q3
Lfg;}
}
}
q3
hT;qZ
d7
qH
q7
if(qU
hJ
oC(151,oY"x"
w2
oH,"[x+Value_t(1)]"
w2,oT);q3
Lfh;}
}
q3
hT;gC
d2
gB
dS:A
qR
A
qW
oC(94,oJ
w6
oK
oH,wA
oH,aP(A)oO);q3
Lfi;}
q3
Default3;g0
Default3:;w0
IsBinaryOpcode(A)qT
A)){hV
hS;oC(93,wM
oF,wA
oH,aP(A)<<","
wU);q3
Lfj;g0
B
qR
IsBinaryOpcode(B)qT
B)){q9-3
g7
hS;oC(97,wM
oM,aD,aP(A)aQ(B)<<","
wU);q3
Lfk;g0
C=wW
gX
C
qW
oC(95,"C[C>=VarBegin] "
oM,aD,aP(A)aQ(B)<<aU(C)oO);q3
Lfl;o8
C)qT
C
mU(96,"C[IsUnaryOpcode(C)&&!HasInvalidRangesOpcode(C)] "
oM,wH
aD,aP(A)aQ(B)<<aU(C)oO);q3
Lfm;}
}
dG
B
qW
oC(91,"B[B>=VarBegin] "
oF,wA
oH,aP(A)aQ(B)oO);q3
Lfi;o8
B)qT
B
mU(92,"B[IsUnaryOpcode(B)&&!HasInvalidRangesOpcode(B)] "
oF,aD,aP(A)aQ(B)oO);q3
Lfn;}
}
dG
A
qW
oC(89,mD
oK
oH,wA,aP(A)oO);q3
Lfo;o8
A)qT
A
mU(90,"A[IsUnaryOpcode(A)&&!HasInvalidRangesOpcode(A)] "
oK
oH,wA
oH,aP(A)oO);q3
Lfp;}
}
}
gB
o2:hV
gR
gM+x==h5
oC(154,"cDup[x+x==Value_t(1)] "
m3,,oT);q3
Lfq;}
oC(155,oY
m3,"[x+x]"
oH,oT);q3
Lga;qZ
g3
wW==qN
A=mM
A
qW
oC(243,mD"y"
w6
m3,wA
w6"A "
wQ
w6
wB,oD", "
wM
aT(A)oO);q3
Lgb;}
hR;oC(242,wM
m3,wA
w6
wQ" "
wB,aO);q3
Lgc;qC
cDeg:oC(106,"cDeg x"
oH,"[RadiansToDegrees(x)]"
oH,oT);q3
Lgd;qZ
dS:hV
h3
q0[-hC
gM+x==h5
oC(156,"cDup[x+x==Value_t(1)] "
wB
w6"x"
oH,"cMul"
,oT);q3
Lge;}
oC(157,oY
wB
w6"x"
oH,"cMul [x+x]"
oH,oT);q3
Lgf;hR
gX(y*x)==gN
oC(143,"y[(y*x)==GetRadiansToDegreesFactor<Value_t>()]"
w6"x"
oH,"cDeg"
,aO);q3
dF(y*x)==gO
oC(144,"y[(y*x)==GetDegreesToRadiansFactor<Value_t>()]"
w6"x"
oH,aN,aO);q3
Lcf;dG
y*x==h5
oC(145,"y[y*x==Value_t(1)]"
w6"x"
oH,,aO);q3
Lgg;}
oC(146,"y"
w6"x"
oH,wQ
oH,aO);q3
Lgh;qC
mB(135,mW"x"
oH,mN
oH,oT);q3
Lgi;qZ
cRad:oC(107,aN" x"
oH,"[DegreesToRadians(x)]"
oH,oT);q3
Lgj;qZ
cSub
gT
dT
m4
wW==qN
A=mM
A
qW
oC(244,mD"y"
w6
aF" x"
oH,wA
w6"A "
wQ
w6
aF,oD", "
wM
aT(A)oO);q3
Lgk;}
}
hR;oC(71,wM"x"
oH,wQ,aO);q3
Lgl;}
hW
h5
oC(70,"x[x==Value_t(1)]"
oH,,oT)gK}
hW
q8
2
mU(100,"x[x==Value_t(2)]"
oH,oY
wB,oT);q3
Lgm;}
hW
gN
oC(101,"x[x==GetRadiansToDegreesFactor<Value_t>()]"
oH,"cDeg"
,oT);q3
Lgn;}
hW
gO
oC(102,"x[x==GetDegreesToRadiansFactor<Value_t>()]"
oH,aN,oT);q3
Lgo;}
hW
q8-1
mU(137,"x[x==Value_t(-1)]"
oH,aV,oT);q3
Lgp;}
h7
g0
hT:;A
hD
A
qW
gB
dS:hV
g5
B=wW
wT(348,w3
w1
w6
oJ
oH,wH
w9
oH
w1,oR(A)aQ(B)oO);q3
Lff;}
q3
Default4;g0
Default4:;mS(339,w3
w6
oJ
oH,wH
w9
oH,oR(A)aQ(B)oO);q3
Lgq;}
}
q3
dI;qZ
g5
mS(342,w3
w1" "
oJ
oH,wH
w9
w1,oR(A)aQ(B)oO);q3
Lfg;}
q3
dI;g0
dI:;B=qH
wT(336,mT
oJ
oH,wH
w9,oR(A)aQ(B)oO);q3
Lfd;}
}
o8
A)){B=qH
gX
B
gA
1){if(dT
dS){C=wW
gX
C
aG
D=mM
D==B){oC(358,"D[D==B] C[C==A]"
w6"B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]"
oH,"D C "
w9
oH,oR(A)aQ(B)<<aU(C)<<", D "
oE(D)oO);q3
Lha;}
}
}
}
qD
TailCall_cNEqual:qZ
dX
dO==qM
wZ(217,wK" "
wN
oW,a2,oT);q3
Lhb;qZ
hN
oC(219,aJ
wN
oW,a2,oT);q3
Lhb;}
}
mC(129,wX
oW,"[fp_nequal(y,x)]"
,aO);q3
Lhc;qD
h9:qZ
g5
g6
g3
aE(136,"x"
oH
w1,mN
oH,oT);q3
Lhd;qC
mB(199,aV
w1,,);q3
dY
cSin
gT
qH==m4
m5(310,"x"
w6
a7
w1,mN
w6
a7,oT);q3
Lhe;}
qC
cSinh
gT
qH==m4
m5(311,"x"
w6
mV
w1,mN
w6
mV,oT);q3
Lhf;}
qC
cTan
gT
qH==m4
m5(312,"x"
w6
a5
w1,mN
w6
a5,oT);q3
Lhg;}
qC
cTanh
gT
qH==m4
m5(313,"x"
w6"cTanh"
w1,mN
w6"cTanh"
,oT);q3
Lhh;}
qC
o7
oC(68,"x"
w1,mN,oT);q3
Lhi;}
qE
TailCall_cNot:qZ
cNot:g6
h2
oC(249,wK
mF,aK,);q3
Lhj;qZ
cAbsNot:w0
gF
oC(254,"A[IsLogicalOpcode(A)] "
wR
mF,"A"
,oR(A)oO);q3
hY
dG
A!=q5){oC(255,"A[A!=cImmed] "
wR
mF,"A cAbsNotNot"
,oR(A)oO);q3
Lhk;}
q3
dJ;qZ
cAbsNotNot:oC(252,"cAbsNotNot"
mF,wR,);q3
Lhl;qZ
dE:oC(213,wC
mF,oW,);q3
Lhm;qZ
hK:oC(211,wV
mF,wP,);q3
Lhn;qZ
gY:oC(212,wD
mF,mX,);q3
Lho;qZ
o5:oC(209,mX
mF,wD,);q3
Lhp;qZ
h8:oC(210,wP
mF,wV,);q3
Lhq;qZ
dX:oC(214,oW
mF,wC,);q3
Lia;qZ
mB(248,aV
mF,aK,);q3
Lhj;qZ
cNot:oC(250,aK
mF,wO,);q3
Lib;qZ
d8:oC(251,wO
mF,aK,);q3
Lhj;gC
oC(85,"x"
mF,"[fp_not(x)]"
,oT);q3
Lic;g0
dJ:;A
hD
qO
oC(256,oL
aK,"A "
wR,oR(A)oO);q3
Lid;qD
TailCall_cOr:qZ
cOr
hI
oC(131,wX"cOr"
,"[fp_or(x,y)]"
,aO);q3
Lie;qD
TailCall_cPow:qZ
cPow
gU
q7
if(!gL
x+x)){if(qH==dZ{oC(14,aJ"x[!isEvenInteger(x+x)]"
w2,wK" [x+x]"
w2,oT);q3
Lif;}
dG
IsIntegerConst(x)){gB
a9(35,mR
oP,wA
w6
mR,oT);q3
Lig;qZ
d9:oC(36,wF
oP,wA
w6
wF,oT);q3
Lih;qZ
d7
dT
aA!IsIntegerConst(y
mU(34,"y[!IsIntegerConst(y)]"
w2
oP,wQ
w2,aO);q3
Lii;}
}
oC(37,"cPow"
oP,wA
oH
w2,oT);q3
Lij;}
dG
gL
x)){wZ(299,wK" x[isEvenInteger(x)]"
w2,wA
w2,oT);q3
Lik;qZ
o4
oA
oC(300,wK
w6"x[isEvenInteger(x)]"
w2,"cMul "
wA
w2,oT);q3
Lil;}
h7}
}
d2
oC(122,oK
w2,"[Value_t(0)]"
w6"[Value_t(1)] "
wB,oT);q3
Lim;}
hW
q8
hQ
oC(175,"x[x==Value_t(0.5)]"
w2,m6,oT);q3
Lin;}
hW
hH/q8
3
mU(176,"x[x==Value_t(1)/Value_t(3)]"
w2,"cCbrt"
,oT);q3
Lio;}
hW
hH/q8-3
mU(177,"x[x==Value_t(1)/Value_t(-3)]"
w2,"cCbrt "
mK,oT);q3
Lip;}
hW
q8-hQ
oC(178,"x[x==Value_t(-0.5)]"
w2,"cRSqrt"
,oT);q3
Liq;}
hW
q8-1
mU(179,"x[x==Value_t(-1)]"
w2,mK,oT);q3
Lja;}
gB
d7
dT
aA
gL
y)&&!gL
x*y
mU(13,"y[isEvenInteger(y)&&!isEvenInteger(x*y)]"
w2" x"
w2,wK" "
wQ
w2,aO);q3
Ljb;}
oC(173,"y"
w2" x"
w2,wQ
w2,aO);q3
Lii;qC
hN
oC(38,aJ"x"
w2,"[x+x]"
w2,oT);q3
Ljc;qZ
hS
gX
y!=q8
0)||x>=aM(84,"y[y!=Value_t(0)||x>=Value_t(0)] x"
w2,"[fp_pow(y,x)]"
,aO);q3
Ljd;}
h7}
oC(333,"x"
w2,"[DO_POWI]"
,oT)gX
TryCompilePowi(x))g1}
qE
TailCall_cRad:qZ
cRad
m0(66,mG
aN,"[DegreesToRadians(x)]"
,oT);q3
Lje;}
qE
dN:qZ
cSin:dW
oC(306,mW
a7,a7
w1,);q3
Ljf;gC
oC(58,mG
a7,"[fp_sin(x)]"
,oT);q3
Ljg;}
qE
TailCall_cSinh:qZ
cSinh:g6
cAcosh:oC(315,"cAcosh "
mV,oX
aJ"[Value_t(-1)] "
wB" "
m6,);qY
Ljh;qZ
cAsinh:oC(192,"cAsinh "
mV,,);q3
dY
mB(307,mW
mV,mV
w1,);q3
Lji;gC
oC(59,mG
mV,"[fp_sinh(x)]"
,oT);q3
Ljj;}
qE
TailCall_cSqr:qZ
hN
g6
h2
oC(354,wK" "
w9,w9,);q3
Ljk;qZ
mB(353,mW
w9,w9,);q3
Ljk;qZ
cSqrt:w0
qO
oC(181,oL
m6" "
w9,"A"
,oR(A)oO);q3
hY}
h7}
qE
TailCall_cSqrt:qZ
cSqrt:g6
hN
oC(15,aJ
m6,wK,);q3
Ljl;gC
gM>=aM(60,"x[x>=Value_t(0)] "
m6,"[fp_sqrt(x)]"
,oT);q3
Ljm;}
h7}
qE
hA:qZ
cSub:g6
gR
oC(152,oY
aF,"[Value_t(0)]"
oH,);q3
Ljn;qZ
mB(205,mW
aF,wB,);q3
Ljo;gC
d2
oC(77,wN
aF,,oT)gK}
mC(78,wX
aF,"[y-x]"
,aO);q3
Ljp;}
oC(168,mG
aF,mN" "
wB,oT);q3
Ljq;}
qE
dR:qZ
cTan:g6
cAtan2:oC(196,"cAtan2 "
a5,m7,);q3
Lfe;qZ
mB(308,mW
a5,a5
w1,);q3
Lka;gC
oC(62,mG
a5,"[fp_tan(x)]"
,oT);q3
Lkb;}
qE
TailCall_cTanh:qZ
cTanh:dW
oC(309,mW"cTanh"
,"cTanh"
w1,);q3
Lkc;gC
oC(63,mG"cTanh"
,"[fp_tanh(x)]"
,oT);q3
Lkd;}
qE
TailCall_cTrunc:qZ
cTrunc:g6
o7
oC(64,mG"cTrunc"
,"[fp_trunc(x)]"
,oT);q3
Lke;gW
g4
oC(259,w5"cTrunc"
,"A"
,oR(A)oO);o3
g0
Default0:;A=dD
gX
IsComparisonOpcode(A)){if(gV
q7
gS
qN
oC(221,wM
wB
mH,"[x-y]"
aB,oR(A)aR
wU);q3
Lkf;qC
cAtan
gT
d3<fp_const_pi<hM>()*q8
hQ
oC(237,"cAtan[fp_abs(x)<fp_const_pi<Value_t>()*Value_t(0.5)]"
mH,"[fp_tan(x)]"
aB,oR(A)aS);q3
Lkg;qC
cExp:gM>aM(227,"cExp[x>Value_t(0)]"
mH,"[fp_log(x)]"
aB,oR(A)aS);q3
Lkh;qC
d9:gM>aM(228,"cExp2[x>Value_t(0)]"
mH,"[fp_log2(x)]"
aB,oR(A)aS);q3
Lki;qC
cLog:qQ
oC(230,oS
wY
mH,wH"[fp_exp(x)]"
aB,oR(A)aR
mE(B)oO);q3
Lkj;qC
oB:qQ
oC(232,oS
wI
mH,wH"[fp_pow(Value_t(10),x)]"
aB,oR(A)aR
mE(B)oO);q3
Lkk;qC
cLog2:qQ
oC(231,oS
wG
mH,wH"[fp_exp2(x)]"
aB,oR(A)aR
mE(B)oO);q3
Lkl;qC
o4
aA
y>aM(223,"y[y>Value_t(0)]"
w6
oI,"[x/y]"
aB,oR(A)aR
wU);q3
Lkm;dG
y<aM(224,"y[y<Value_t(0)]"
w6
oI,"[x/y] {OppositeComparisonOpcode(A)}"
,oR(A)aR
wU);q3
Lkn;}
qC
mB(222,aV
mH,mN" {OppositeComparisonOpcode(A)}"
,oR(A)aS);q3
Lko;qZ
cPow:gM>qM
if(dT
aA
y>aM(225,"y[y>Value_t(0)] cPow[x>Value_t(0)]"
mH,"[fp_pow(x,1/y)]"
aB,oR(A)aR
wU);q3
Lkp;}
}
qC
cSinh:oC(238,mV
mH,"[fp_asinh(x)]"
aB,oR(A)aS);q3
Lkq;qZ
hN
gM>aM(226,"cSqr[x>Value_t(0)]"
mH,wK" [fp_sqrt(x)]"
aB,oR(A)aS);q3
Lla;qC
cTanh
gT
d3<h5
oC(239,"cTanh[fp_abs(x)<Value_t(1)]"
mH,"[fp_atanh(x)]"
aB,oR(A)aS);q3
Llb;}
h7}
}
dG
A
gA
0){B
hD
B
aG
oC(356,mT"A[A>=VarBegin&&data->ByteCode.size()>0]"
,mL,oR(A)aQ(B)oO);q3
Llc;}
o8
A)){B
hD
B
gA
1){C=qH
gX
C
aG
D
qR
D==B){oC(357,"D[D==B] C[C==A] B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]"
,"D C cDup"
,oR(A)aQ(B)<<aU(C)<<", D "
oE(D)oO);q3
Lld;}
}
}
}
}
q3
Lle;Laa:q0-=1;q1
Llf:q3
dH;Lab:qA=d3;g1
Lac:qA=fp_acos
w7
Lad:qA=fp_acosh
w7
Lae:qI
q8
4));qU
d1-=2;Llg:dP
qV
Llh:gP
dS;Lli:FP_ReDefinePointers();Llj:q3
TailCall_cMul;Laf:qJ
4;hP
qI
q8
4));Llk:g8
q3
Llh;Lag:qA=x+hH;mO
Lll:gP
dS;q3
Llj;Lah:aI
cSub;Llm:q3
hA;Lai:qS=y+x;hU
Lln:FP_ReDefinePointers();Llo:q3
TailCall_cAdd;Laj:qS=y+x;Lak:h0
Lbg:q1
g1
Lal:qS=fp_and(x
gJ
Lam:qA=fp_asin
w7
Lan:qA=fp_asinh
w7
Lao:qA=fp_atan
w7
Lap:qS=fp_atan2(y
dQ
Laq:qA=fp_atanh
w7
Lba:qA=fp_cbrt
w7
Lbb:qB
cFloor);Llp:gP
cNeg;qL
h9;Lbc:qA=fp_ceil
w7
Lbd:q0-=1;q1
Llq:q3
TailCall_cCos;Lbe:qA=fp_cos
w7
Lbf:aH
TailCall_cCosh;Lbh:qB
dZ;qI
hH);Lma:g8
qK
o2);Lmb:gP
cSqrt;qL
TailCall_cSqrt;Lbi:qA=fp_cosh
w7
Lbj:qA=RadiansToDegrees
w7
Lbk:qB
cSec
a1;Lbl:qB
cTan
a1;Lbm:qB
cSin
a1;Lbn:qI
q8
0));gV
d6
Lmc:hB
qI
hH);Lmd:g8
Lme:gP
o2;q3
Lln;Lbo:qB
cNeg);qK
cExp
a1;Lbp:qB
cNeg);qK
d9
a1;Lbq:aH
Lll;Lca:qB
cNeg);qK
cPow
a1;Lcb:qB
cCos
a1;Lcc:qB
cCsc
a1;Lcd:qB
cCot
a1;Lce:qJ
dL
qV
Lmf:gP
cDeg;qL
TailCall_cDeg;Lcf:qJ
dL
qV
Lmg:gP
cRad;qL
TailCall_cRad;Lcg:qS=y/x;h0
q3
Llg;Lch:hF
d0
Lmh:q3
TailCall_cDiv;Lci:qS=y/x
gK
Lcj:qA=hH/x;q3
Lll;Lck:d0
Lmi:q3
dU;Lcl:qS=fp_equal(y
dQ
Lcm:dM
cExp);qI
dC
mZ
Llk;Lcn:qA=dC;g1
Lco:dM
d9);qI
dB
mZ
Llk;Lcp:qA=dB;g1
Lcq:qI
dA
q8
2)));g8
Lmj:hB
gP
cExp;qL
TailCall_cExp;Lda:qB
cCeil
mZ
Llp;Ldb:qA=fp_floor
w7
Ldc:qS=fp_less(x
gJ
Ldd:h0
qB
cNeg);Lid:qK
cAbsNot);g1
Lde:qA=q8
0.5)/x;qH=hE
Lmk:qK
d8);g1
Ldf:qS=fp_lessOrEq(x
gJ
Ldg:h0
Lhk:q1
Lml:qK
cAbsNotNot);g1
Ldh:qA=fp_int
w7
Ldi:qB
cSec);g1
Ldj:aI
cTan;Lmm:q3
dR;Ldk:aI
cSin;Lmn:q3
dN;Ldl:qB
cNeg);gP
cPow;Lmo:FP_ReDefinePointers();Lmp:q3
TailCall_cPow;Ldm:aI
cCos;q3
Llq;Ldn:qB
cCsc);g1
Ldo:qB
cRSqrt);g1
Ldp:qB
cCot);g1
Ldq:qA=hH/x;g1
Lea:qA=q8
0.5)/x;qH=hE
gP
cNot;FP_ReDefinePointers();Lmq:q3
TailCall_cNot;Leb:qS=fp_less(y
dQ
Lec:h0
Lhl:o0
Lid;Led:qS=fp_lessOrEq(y
dQ
Lee:h0
qB
cNeg
mZ
Lml;Lef:dM
cLog);qI
dA
x)mZ
Lmd;Leg:qB
d4
qK
cLog);Lna:qP
push_back(cDup
mZ
Lme;Leh:qA=dA
x);g1
Lei:qI
d5
fp_const_e<hM>()));Lnb:gV
d6
q3
Llh;Lej:dM
oB);qI
d5
x)mZ
Lmd;Lek:qB
d4
qK
oB
mZ
Lna;Lel:qA=d5
x);g1
Lem:qI
hX
fp_const_e<hM>())mZ
Lnb;Len:dM
cLog2);qI
hX
x)mZ
Lmd;Leo:qB
d4
qK
cLog2
mZ
Lna;Lep:qA=hX
x);g1
Leq:qS=fp_max(x
gJ
Lfa:qS=fp_min(x
gJ
Lfb:qS=fp_mod(y
dQ
Lfc:qS=x;g9
3;q6
A);Lnc:hB
qI
y*x
mZ
Lmd;Lfd:aI
cSqr;Lnd:q3
TailCall_cSqr;Lfe:aI
cDiv;q3
Lmh;Lff:qJ
3;q6
dZ;hB
q3
Llp;Lfg:dP
q6
dZ;q3
Llp;Lfh:qA=x+hH;mO
gP
cPow;q3
Lmp;Lfi:mO
q3
Llj;Lfj:qS=x;Lne:hU
q3
Lli;Lfk:qJ
h1
h0
Lnf:qJ
4;qV
Lng:dK
x);Lnh:g8
q3
Lli;Lfl:h0
q3
Lnf;Lfm:g9
4;q6
B
mZ
Lng;Lfn:g9
3;qV
q3
Lng;Lfo:qH
d6
q3
hY
Lfp:qH
d1
gZ
Llj;Lfq:h0
Lni:qJ
3;qV
g1
Lga:qA=x+x;q3
Lfi;Lgb:qS=x;q0[-4]d6
g9
4;hP
dK
y*x);g8
hB
q3
Lme;Lgc:qS=x;hU
q3
Lnc;Lgd:qA=RadiansToDegrees(x
mZ
Lfp;Lge:g9
4;qV
q3
Lli;Lgf:g9
4;hP
qI
x+x
mZ
Lnh;Lgg:qJ
h1
h0
q3
Lni;Lgh:qS=y*x;q3
Lne;Lgi:hF
q3
Lfp;Lgj:qA=DegreesToRadians(x
mZ
Lfp;Lgk:qS=x;q0[-4]d6
g9
4;hP
dK
y*x);g8
hB
gP
cSub;qL
Llm;Lgl:qS=y*x
gK
Lgm:gV=cDup;q2-=1;h0
Lnj:gP
o2;q3
Llo;Lgn:h0
o0
Lmf;Lgo:h0
o0
Lmg;Lgp:h0
o0
Llp;Lgq:dP
qV
Lnk:qK
dZ;q3
Lli;Lha:qJ
3;qV
q3
Lnk;Lhb:d0
Lnl:q3
TailCall_cNEqual;Lhc:qS=fp_nequal(y
dQ
Lhd:hF
q3
Lbq;Lhe:mP
cSin;qL
Lmn;Lhf:mP
cSinh;qL
TailCall_cSinh;Lhg:mP
cTan;qL
Lmm;Lhh:mP
cTanh;qL
TailCall_cTanh;Lhi:hF
g1
Lhj:aH
Lmq;Lhm:aI
dX;q3
Lnl;Lhn:aI
h8;q3
TailCall_cLessOrEq;Lho:aI
o5;q3
TailCall_cLess;Lhp:aI
gY;q3
TailCall_cGreaterOrEq;Lhq:aI
hK;q3
TailCall_cGreater;Lia:aI
dE;q3
Lmi;Lib:o0
Lmk;Lic:qA=fp_not
w7
Lie:qS=fp_or(x
gJ
Lif:dM
d4
qI
x+x);Lnm:g8
q3
Lmo;Lig:qH
d6
o0
Lmj;Lih:qH=hE
gP
d9;qL
TailCall_cExp2;Lii:qS=y*x;hU
q3
Lmo;Lij:qH=hE
q3
Lmo;Lik:qH
d1
gZ
Lmp;Lil:g9
3;hP
qI
x
mZ
Lnm;Lim:qA=q8
0
mZ
Lmc;Lin:h0
o0
Lmb;Lio:h0
q1
gP
cCbrt;qL
TailCall_cCbrt;Lip:h0
qB
cCbrt);Lnn:gP
cInv;qL
TailCall_cInv;Liq:h0
q3
Ldo;Lja:h0
o0
Lnn;Ljb:qJ
dL
q6
d4
qI
y*x
mZ
Lnm;Ljc:qA=x+x;q3
Lik;Ljd:qS=o1
y
dQ
Lje:qA=DegreesToRadians
w7
Ljf:qB
cSin
mZ
Llp;Ljg:qA=fp_sin
w7
Ljh:qB
dZ;qI
q8-1)mZ
Lma;Lji:qB
cSinh
mZ
Llp;Ljj:qA=fp_sinh
w7
Ljk:aH
Lnd;Ljl:aI
cAbs;q3
Llf;Ljm:qA=fp_sqrt
w7
Ljn:qI
q8
0)mZ
Lnb;Ljo:aH
Lnj;Ljp:qS=y-x
gK
Ljq:hF
q3
Lnj;Lka:qB
cTan
mZ
Llp;Lkb:qA=fp_tan
w7
Lkc:qB
cTanh
mZ
Llp;Lkd:qA=fp_tanh
w7
Lke:qA=fp_trunc
w7
Lkf:qS=x-y;Lno:hU
Lnp:qK
A);g1
Lkg:qA=fp_tan(x);Lnq:qH
d6
o0
Lnp;Lkh:qA=dA
x
mZ
Lnq;Lki:qA=hX
x
mZ
Lnq;Lkj:qA=dC;q3
Lnq;Lkk:qA=o1
q8
10),x
mZ
Lnq;Lkl:qA=dB;q3
Lnq;Lkm:qS=x/y;q3
Lno;Lkn:qS=x/y;hU
Loa:qK
OppositeComparisonOpcode(A));g1
Lko:hF
qH
d6
o0
Loa;Lkp:qS=o1
x,1/y
mZ
Lno;Lkq:qA=fp_asinh(x
mZ
Lnq;Lla:dM
d4
qI
fp_sqrt(x));g8
q3
Lnp;Llb:qA=fp_atanh(x
mZ
Lnq;Llc:qP
push_back(cDup);g1
Lld:gV=cDup;g1
Lle:qP
push_back(dD);g1
g1
q3
TailCall_cAcos;q3
TailCall_cAcosh;q3
TailCall_cAnd;q3
TailCall_cAsin;q3
TailCall_cAsinh;q3
TailCall_cAtan;q3
TailCall_cAtan2;q3
TailCall_cAtanh;q3
TailCall_cCeil;q3
TailCall_cFloor;q3
TailCall_cInt;q3
TailCall_cLog;q3
TailCall_cLog10;q3
TailCall_cLog2;q3
TailCall_cMax;q3
TailCall_cMin;q3
TailCall_cMod;q3
TailCall_cOr;q3
TailCall_cRad;q3
dN;q3
TailCall_cSinh;q3
TailCall_cSqrt;q3
hA;q3
dR;q3
TailCall_cTanh;q3
TailCall_cTrunc;
#endif
#undef FP_ReDefinePointers
#undef FP_TRACE_BYTECODE_OPTIMIZATION
#undef FP_TRACE_OPCODENAME
