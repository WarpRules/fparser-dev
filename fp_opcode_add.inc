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
#define mS "cNeg"
#define mR "cSqr"
#define mQ " "wS
#define mP "[x]"oV
#define mO "cTan"
#define mN "cSin"
#define mM -1)&&x
#define mL mR
#define mK mS
#define mJ dV mB
#define mI (x gM
#define mH "cTanh"
#define mG "cSinh"
#define mF "cSub"
#define mE oQ" "
#define mD "[x]"dX
#define mC <<", "wP
#define mB ","o3
#define mA <<","wY","
#define m9 <<","oU
#define m8 dV","wD
#define m7 "cRad"
#define m6 qQ q8:
#define m5 =q0-
#define m4 "cNot"
#define m3 (y gN
#define m2 "x"oH
#define m1 "x"oV
#define m0 oJ" "
#define wZ gE dQ
#define wY " "wO
#define wX )){dQ
#define wW ==d8 dQ
#define wV mR" "
#define wU "cCeil"
#define wT "cLess"
#define wS "cInv"
#define wR mS" "
#define wQ "= "<<
#define wP wN oG
#define wO wN oF
#define wN "x "
#define wM qB dH dQ
#define wL "cFloor"
#define wK 2]=q8;gK
#define wJ "[x] "oB
#define wI "cDiv"
#define wH g2 dQ
#define wG hJ dQ
#define wF "cLog"
#define wE " [x]"dX
#define wD " "dW
#define wC " "m4
#define wB " "mS
#define wA "x"dX
#define w9 "cMin"
#define w8 "cMax"
#define w7 "cSqrt"
#define w6 "B "mR
#define w5 "[y*x]"
#define w4 dY" "
#define w3 "cExp"
#define w2 "cCos"
#define w1 "cGreater"
#define w0 "y "
#define oZ "cAbsNot"
#define oY "cLessOrEq"
#define oX "cNotNot"
#define oW "cExp2"
#define oV dX" "
#define oU " B "dS
#define oT "cDup "oI
#define oS ;qH dQ
#define oR "[-x]"dX
#define oQ "cAbs"
#define oP "cLog2"
#define oO "cCosh"
#define oN "cLog10"
#define oM gZ-1]oS
#define oL "cGreaterOrEq"
#define oK "cEqual"
#define oJ "B[B==A]"
#define oI "cAdd"
#define oH " cPow"
#define oG wQ x o4
#define oF wQ x<<
#define oE dU" with"o3
#define oD "x[x!=Value_t(0)] "
#define oC "x[x==Value_t(0)]"dX
#define oB "cNEqual"
#define oA q0-1]=q8;
#define o9 "x[x>Value_t(0)]"
#define o8 dV"\n"
#define o7 "B[IsNeverNegativeValueOpcode(B)] "
#define o6 "x[IsIntegerConst(x)]"oH
#define o5 "x[x==Value_t(1)] "
#define o4 <<"\n"
#define o3 " A "dS
#define o2 "A[IsNeverNegativeValueOpcode(A)] "
#define o1 "x[x==Value_t(0)] "
#define o0 "B[IsBinaryOpcode(B)&&!HasInvalidRangesOpcode(B)] "dT
#define dZ " "wN"A[IsComparisonOpcode(A)]"
#define dY "A[A>=VarBegin]"
#define dX " cMul"
#define dW w0 wQ y o4
#define dV dU" "dR
#define dU "   "
#define dT "A[IsBinaryOpcode(A)&&!HasInvalidRangesOpcode(A)] "oC
#define dS wQ FP_TRACE_OPCODENAME
#define dR "with"wY
#define dQ FP_TRACE_BYTECODE_OPTIMIZATION
#define dP fp_log
#define dO cTanh:
#define dN cSinh:
#define dM cExp2:
#define dL size()
#define dK q2[-1]
#define dJ TailCall_cNeg
#define dI 1)){
#define dH cExp:
#define dG qX q6
#define dF gC q8
#define dE qC qI
#define dD if(gF
#define dC gD g2
#define dB cAbs
#define dA Lai;
#define d9 fp_pow(
#define d8 qI dI
#define d7 0.5)){
#define d6 qL A);qC
#define d5 qD q1 gT
#define d4 );g7
#define d3 (x d4
#define d2 TailCall_cNotNot
#define d1 qL h9);
#define d0 qG q3
#define hZ Immed.
#define hY for q7
#define hX fp_log2(
#define hW qD qX h2
#define hV gD qI
#define hU qQ cSqr:
#define hT ==cDup){
#define hS qK x>=qI
#define hR Default6
#define hQ Default5
#define hP q8:x=qE;
#define hO qO if(x
#define hN cNotNot
#define hM cAdd
#define hL fp_log10(
#define hK fp_abs(x)
#define hJ qS 1]qN
#define hI g0 B==A){
#define hH hG h9){qS 2]q5
#define hG qS 1]==
#define hF cDup:
#define hE pop_back(
#define hD tmp-->0;)
#define hC Default1
#define hB Value_t
#define hA data->
#define h9 cMul
#define h8 g5 Lnq;
#define h7 qG qT
#define h6 g0 A qY
#define h5 hD q1
#define h4 q0 0]
#define h3 q1 dB);
#define h2 hY 3
#define h1 case
#define h0 if(h4 q5
#define gZ g6 A=q0
#define gY :qS 2]
#define gX g9 hM gY
#define gW unsigned
#define gV qJ hE);
#define gU ,x g5 dA
#define gT h9);qT
#define gS g5 Llh;
#define gR <hB>()){
#define gQ cNeg:
#define gP qM-2]){h1
#define gO ByteCodePtr
#define gN ,x g5 Lak;
#define gM ,y g5 Lak;
#define gL ,y g5 dA
#define gK gO-=2;q6
#define gJ break;
#define gI y=dK;
#define gH isEvenInteger(
#define gG :qS 1]q5
#define gF IsUnaryOpcode(
#define gE B qV B==A){
#define gD if(x==
#define gC qJ push_back(
#define gB >=VarBegin&&qJ dL>
#define gA qR h9 gG if(x>g2
#define g9 qM-1]){h1
#define g8 g0 IsAlwaysIntegerOpcode(A)){
#define g7 return;
#define g6 default:
#define g5 );q3
#define g4 hE g5
#define g3 hY 4;h5 h9);
#define g2 qI 0)){
#define g1 dK=
#define g0 ];if(
#define qZ incStackPtr();--StackPtr;q3
#define qY >=VarBegin){
#define qX hA qA
#define qW &&!HasInvalidRangesOpcode(
#define qV m5 2 g0
#define qU B qV IsNeverNegativeValueOpcode(B)){
#define qT opcode=
#define qS if(q0-
#define qR ;h1
#define qQ }break qR
#define qP FP_ReDefinePointers();q3
#define qO h1 hP
#define qN ==q8){gI
#define qM switch(q0
#define qL AddFunctionOpcode(
#define qK h0 if(
#define qJ hA ByteCode.
#define qI hB(
#define qH if(IsNeverNegativeValueOpcode(A)){
#define qG gO q4
#define qF }q3 Default0;
#define qE q2[0]
#define qD hY 2;hD
#define qC hA hZ push_back(
#define qB qM 0]){h1
#define qA hZ hE);
#define q9 ;hD gV
#define q8 cImmed
#define q7 (gW tmp=
#define q6 hY 2 q9
#define q5 ==q8){x=qE;
#define q4 -=1;gV
#define q3 goto
#define q2 ImmedPtr
#define q1 gV qL
#define q0 gO[
gW*gO;hB*q2;
#define FP_ReDefinePointers() gO=!qJ empty()?&hA ByteCode[0]+qJ dL-1:0;q2=!hA hZ empty()?&hA Immed[0]+hA hZ dL-1:0;
FP_ReDefinePointers();
#if(!FP_FLOAT_VERSION)
hB
x;gW
A;hB
y;gW
B;gW
C;gW
D;switch(opcode){TailCall_cAbs:h1
dB:qB
gQ
dQ(197,wR
oQ,oQ,);q3
Laa;qO
dQ(40,wN
oQ,"[fp_abs(x)]"
,o8);q3
Lab;gZ
0]oS(258,o2
oQ,"A"
,oE(A)o4);g7}
qF
TailCall_cAdd:h1
hM:qB
hF
gX
hT
dQ(147,oT" "
oT,"[Value_t(4)]"
dX,);q3
Lac;qQ
h9
gY==hM){qS
3]hT
dQ(148,oT
oV
oT,"cMul [Value_t(4)]"
dX,);q3
Lad;}
}
gJ
qQ
h9
gG
qS
2]hT
dQ(150,"cDup "
m1
oI,"[x+Value_t(1)]"
dX,o8);q3
Lae;}
qQ
gQ
dQ(204,wR
oI,mF,);q3
Laf;qO
gX
qN
dQ(141,w0
oI" "
wN
oI,"[y+x] "
oI,m8);q3
Lag;m6
gI
dQ(76,w0
wN
oI,"[y+x]"
,m8);q3
Lah;}
dC
dQ(75,o1
oI,,o8);q3
dA}
gJ
qF
TailCall_cAnd:h1
cAnd:qK
q0-1]qN
dQ(130,w0
wN"cAnd"
,"[fp_and(x,y)]"
,m8);q3
Laj;}
qF
TailCall_cDiv:h1
cDiv:qB
hF
dQ(120,"cDup "
wI,"[Value_t(0)]"
oV"[Value_t(1)] "
oI,);q3
Lak;hO!=g2
g9
gQ
dQ(138,wR
oD
wI,"[-x] "
wI,o8);q3
Lal
qR
q8:gI
dQ(73,w0
oD
wI,"[y/x]"
,m8);q3
Lam;}
}
hV
dI
dQ(72,o5
wI,,o8);q3
dA}
gJ
qF
TailCall_cEqual:h1
cEqual:qK
x==d8
A
m5
1
g0
IsLogicalOpcode(A
wX(286,"A[IsLogicalOpcode(A)] "
o5
oK,"A"
,mJ(A)o4);q3
dA}
}
wG(128,w0
wN
oK,"[fp_equal(y,x)]"
,m8);q3
Lan;}
dC
g9
dB:dQ(216,mE
o1
oK,"[x] "
oK,o8);q3
Lao
qR
cSqr:dQ(218,wV
o1
oK,"[x] "
oK,o8);q3
Lao;}
dQ(276,o1
oK,m4,o8);q3
Lap;}
qF
TailCall_cGreater:h1
cGreater:qK
x==g2
g9
dB:dQ(278,mE
o1
w1,oX,o8);q3
Laq;oM(282,o2
o1
w1,"A "
oX,mJ(A)o4);q3
Lba;}
}
}
wG(126,w0
wN
w1,"[fp_less(x,y)]"
,m8);q3
Lbb;}
qF
TailCall_cGreaterOrEq:h1
cGreaterOrEq:qK
x==d8
g9
dB:dQ(279,mE
o5
oL,oX,o8);q3
Laq;oM(283,o2
o5
oL,"A "
oX,mJ(A)o4);q3
Lba;}
}
}
wG(127,w0
wN
oL,"[fp_lessOrEq(x,y)]"
,m8);q3
Lbc;}
qF
TailCall_cInv:h1
cInv:qB
cInv:dQ(198,wS
mQ,,);q3
Lbd
qR
cSqrt:dQ(67,w7
mQ,"cRSqrt"
,);q3
Lbe;hO!=wH(69,oD
wS,"[Value_t(1)/x]"
,o8);q3
Lbf;}
gJ
qF
TailCall_cLess:h1
cLess:qK
x==d8
g9
dB:dQ(280,mE
o5
wT,m4,o8);q3
Lbg;oM(284,o2
o5
wT,"A"
wC,mJ(A)o4);q3
Lap;}
}
}
wG(124,w0
wN
wT,"[fp_less(y,x)]"
,m8);q3
Lbh;}
qF
TailCall_cLessOrEq:h1
cLessOrEq:qK
x==g2
g9
dB:dQ(281,mE
o1
oY,m4,o8);q3
Lbg;oM(285,o2
o1
oY,"A"
wC,mJ(A)o4);q3
Lap;}
}
}
wG(125,w0
wN
oY,"[fp_lessOrEq(y,x)]"
,m8);q3
Lbi;}
qF
TailCall_cMax:h1
cMax:qB
hF
dQ(110,"cDup "
w8,,);q3
Lbd;qO
wG(82,w0
wN
w8,"[fp_max(x,y)]"
,m8);q3
Lbj;}
gJ
gZ
0
h6
g9
hF
wZ(112,m0"cDup "
w4
w8,"B cDup"
,oE(A)m9(B)o4);q3
Lbd;qQ
cMax:wZ(114,m0
w8" "
w4
w8,"B "
w8,oE(A)m9(B)o4);q3
Lbd;}
gJ}
}
qF
TailCall_cMin:h1
cMin:qB
hF
dQ(109,"cDup "
w9,,);q3
Lbd;qO
wG(81,w0
wN
w9,"[fp_min(x,y)]"
,m8);q3
Lbk;}
gJ
gZ
0
h6
g9
hF
wZ(111,m0"cDup "
w4
w9,"B cDup"
,oE(A)m9(B)o4);q3
Lbd;qQ
cMin:wZ(113,m0
w9" "
w4
w9,"B "
w9,oE(A)m9(B)o4);q3
Lbd;}
gJ}
}
qF
TailCall_cMod:h1
cMod:qK
x!=g2
wG(74,w0
oD"cMod"
,"[fp_mod(y,x)]"
,m8);q3
Lbl;}
}
qF
TailCall_cMul:h1
h9:qB
hM:hG
q8){y=qE;A
qV
A
qY
qS
3]==q8){x=dK;dQ(245,wN
w4
w0
oI
dX,"[x] A"
oV
w5" "
oI,dU" with "
w0
wQ
y<<mB(A)mC);q3
Lbm;}
}
}
q3
hC
qR
hF
dQ(359,"cDup"
dX,mL,);q3
Lbn
qR
cInv:dQ(203,wS
dX,wI,);q3
Lbo
qR
gQ
A
m5
1
h6
gP
h9:B
m5
3
hI
dQ(351,oJ
oV
w4
mK
dX,w6
oV
mK,oE(A)m9(B)o4);q3
Lbp;}
q3
Default2;g6
Default2:;wZ(345,m0
w4
mK
dX,w6
wB,oE(A)m9(B)o4);q3
Lbq;}
}
}
q3
hC
qR
cPow
gG
qS
2]hT
dQ(151,"cDup "
m2
dX,"[x+Value_t(1)]"
oH,o8);q3
Lca;}
}
q3
hC;qO
dC
g9
h9:A
qV
A
qY
dQ(94,dY
oV
oC,mD,mJ(A)o4);q3
Lcb;}
q3
Default3;g6
Default3:;A
m5
1
g0
IsBinaryOpcode(A)qW
A)){gP
q8:gI
dQ(93,w0
dT,mD,mJ(A)<<","
wD);q3
Lcc;g6
B
qV
IsBinaryOpcode(B)qW
B)){qM-3]){h1
q8:gI
dQ(97,w0
o0,"A"
wE,mJ(A)m9(B)<<","
wD);q3
Lcd;g6
C
m5
3
g0
C
qY
dQ(95,"C[C>=VarBegin] "
o0,"A"
wE,mJ(A)m9(B)<<", C "
dS(C)o4);q3
Lce;}
dD
C)qW
C
wX(96,"C[IsUnaryOpcode(C)&&!HasInvalidRangesOpcode(C)] "
o0,"B A"
wE,mJ(A)m9(B)<<", C "
dS(C)o4);q3
Lcf;}
}
}
if(B
qY
dQ(91,"B[B>=VarBegin] "
dT,mD,mJ(A)m9(B)o4);q3
Lcb;}
dD
B)qW
B
wX(92,"B[IsUnaryOpcode(B)&&!HasInvalidRangesOpcode(B)] "
dT,"A"
wE,mJ(A)m9(B)o4);q3
Lcg;}
}
}
if(A
qY
dQ(89,w4
oC,"[x]"
,mJ(A)o4);q3
Lch;}
dD
A)qW
A
wX(90,"A[IsUnaryOpcode(A)&&!HasInvalidRangesOpcode(A)] "
oC,mD,mJ(A)o4);q3
Lci;}
}
}
g9
hM:gP
hF
dQ(155,oT" "
wA,"[x+x]"
dX,o8);q3
Lcj
qR
h9:qS
3]qN
A
m5
4
h6
dQ(243,w4"y"
oV
oI" "
wA,mP"A "
w5
oV
oI,dV", "
w0
wQ
y<<mB(A)o4);q3
Lck;}
m6
gI
dQ(242,w0
oI" "
wA,mP
w5" "
oI,m8);q3
Lcl;qQ
h9:gP
hM:qS
3]hT
dQ(157,oT
oV
wA,"cMul [x+x]"
dX,o8);q3
Lcm;m6
y=q2[-1
g0
y*x
wW(145,"y[y*x==Value_t(1)]"
oV
wA,,m8);q3
Lcn;}
dQ(146,"y"
oV
wA,w5
dX,m8);q3
Lco;qQ
gQ
dQ(135,wR
wA,oR,o8);q3
Lcp
qR
cSub
gY==h9){qS
3]qN
A
m5
4
h6
dQ(244,w4"y"
oV"cSub "
wA,mP"A "
w5
oV
mF,dV", "
w0
wQ
y<<mB(A)o4);q3
Lcq;}
}
m6
gI
dQ(71,w0
wA,w5,m8);q3
Lda;}
hV
dI
dQ(70,"x[x==Value_t(1)]"
dX,,o8);q3
dA}
hV
2
wX(100,"x[x==Value_t(2)]"
dX,oT,o8);q3
Ldb;}
hV-dI
dQ(137,"x[x==Value_t(-1)]"
dX,mK,o8);q3
Ldc;}
gJ
g6
hC:;A=q0
0
h6
g9
h9:gP
gQ
B
m5
3
hI
dQ(348,oJ
wB
oV
dY
dX,w6
oV
mK,oE(A)m9(B)o4);q3
Lbp;}
q3
Default4;g6
Default4:;wZ(339,oJ
oV
dY
dX,w6
dX,oE(A)m9(B)o4);q3
Ldd;}
}
q3
hQ
qR
gQ
wZ(342,oJ
wB" "
dY
dX,w6
wB,oE(A)m9(B)o4);q3
Lbq;}
q3
hQ;g6
hQ:;B
m5
1
hI
dQ(336,m0
dY
dX,w6,oE(A)m9(B)o4);q3
Lbn;}
}
}
dD
A)){B
m5
1
g0
B
gB
1){qS
2]==h9){C
m5
3
g0
C==A){D
m5
4
g0
D==B){dQ(358,"D[D==B] C[C==A]"
oV"B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]"
dX,"D C "
mR
dX,oE(A)m9(B)<<", C "
dS(C)<<", D "
dS(D)o4);q3
Lde;}
}
}
}
}
qF
TailCall_cNEqual:h1
cNEqual:qK
x==d8
A
m5
1
g0
IsLogicalOpcode(A
wX(287,"A[IsLogicalOpcode(A)] "
o5
oB,"A"
wC,mJ(A)o4);q3
Lap;}
}
wG(129,w0
wN
oB,"[fp_nequal(y,x)]"
,m8);q3
Ldf;}
dC
g9
dB:dQ(217,mE
o1
oB,wJ,o8);q3
Ldg
qR
cSqr:dQ(219,wV
o1
oB,wJ,o8);q3
Ldg;}
dQ(277,o1
oB,oX,o8);q3
Lba;}
qF
dJ:h1
gQ
qB
h9
gG
dQ(136,m1
mK,oR,o8);q3
Ldh;qQ
gQ
dQ(199,mK
wB,,);q3
Lbd;qO
dQ(68,"x"
wB,"[-x]"
,o8);q3
Ldi;qF
TailCall_cNot:h1
cNot:qB
dB:dQ(249,oQ
wC,m4,);q3
Ldj
qR
cAbsNot:A
m5
1
g0
IsLogicalOpcode(A
wX(254,"A[IsLogicalOpcode(A)] "
oZ
wC,"A"
,oE(A)o4);q3
Lbd;}
if(A!=q8){dQ(255,"A[A!=cImmed] "
oZ
wC,"A cAbsNotNot"
,oE(A)o4);q3
Ldk;}
q3
hR
qR
cAbsNotNot:dQ(252,"cAbsNotNot"
wC,oZ,);q3
Ldl
qR
hM
gG
dQ(289,wN
oI
wC,"[x] "
oK,o8);q3
Ldm;}
q3
hR
qR
cEqual:dQ(213,oK
wC,oB,);q3
Ldn
qR
cGreater:dQ(211,w1
wC,oY,);q3
Ldo
qR
cGreaterOrEq:dQ(212,oL
wC,wT,);q3
Ldp
qR
cLess:dQ(209,wT
wC,oL,);q3
Ldq
qR
cLessOrEq:dQ(210,oY
wC,w1,);q3
Lea
qR
cNEqual:dQ(214,oB
wC,oK,);q3
Leb
qR
gQ
dQ(248,mK
wC,m4,);q3
Ldj
qR
cNot:dQ(250,m4
wC,oX,);q3
Lec
qR
hN:dQ(251,oX
wC,m4,);q3
Ldj;qO
dQ(85,"x"
wC,"[fp_not(x)]"
,o8);q3
Led;g6
hR:;A=h4
oS(256,o2
m4,"A "
oZ,oE(A)o4);q3
Lee;}
qF
d2:h1
hN:if(h4==hM){qS
1]q5
dQ(288,wN
oI" "
oX,wJ,o8);q3
Lef;}
qF
TailCall_cOr:h1
cOr:qK
q0-1]qN
dQ(131,w0
wN"cOr"
,"[fp_or(x,y)]"
,m8);q3
Leg;}
qF
TailCall_cSqr:h1
cSqr:qB
dB:dQ(354,oQ" "
mR,mL,);q3
Leh
qR
gQ
dQ(353,wR
mL,mL,);q3
Leh;qF
TailCall_cSub:h1
cSub:qB
hF
dQ(152,"cDup cSub"
,"[Value_t(0)]"
dX,);q3
Lei
qR
gQ
dQ(205,wR
mF,oI,);q3
Lej;qO
dC
dQ(77,o1
mF,,o8);q3
dA}
wG(78,w0
wN
mF,"[y-x]"
,m8);q3
Lek;}
dQ(168,wN
mF,"[-x] "
oI,o8);q3
Lel;qF
g6
Default0:;A=opcode;if(IsComparisonOpcode(A)){h0
gX
qN
dQ(221,w0
oI
dZ,"[x-y] A"
,oE(A)mA
wD);q3
Lem;qQ
gQ
dQ(222,mK
dZ,"[-x] {OppositeComparisonOpcode(A)}"
,oE(A)mC);q3
Len;}
}
}
if(A
gB
0){B=q0
0
hI
dQ(356,m0"A[A>=VarBegin&&data->ByteCode.size()>0]"
,"B cDup"
,oE(A)m9(B)o4);q3
Leo;}
}
dD
A)){B=q0
0
g0
B
gB
1){C
m5
1
g0
C==A){D
qV
D==B){dQ(357,"D[D==B] C[C==A] B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]"
,"D C cDup"
,oE(A)m9(B)<<", C "
dS(C)<<", D "
dS(D)o4);q3
Lep;}
}
}
}
}
q3
Leq;Laa:d0
TailCall_cAbs;Lab:qE=hK;g7
Lac:dE
4));q0-wK
Lfa:qT
h9;Lfb:FP_ReDefinePointers();Lfc:q3
TailCall_cMul;Lad:g3
dE
4));dF
g5
Lfa;Lae:qE=x+qI
1);q0-wK
Lfd:qT
h9;q3
Lfc;Laf:h7
cSub;Lfe:q3
TailCall_cSub;Lag:g1
y+x;dG
Lff:FP_ReDefinePointers();Lfg:q3
TailCall_cAdd;Lah:g1
y+x;Lai:qX
Lbd:gV
g7
Laj:g1
fp_and(x
gL
Lak:dE
0));h4=q8;d1
dE
1));Lfh:dF);Lfi:qT
hM;q3
Lff;Lal:qE=-x;oA
qG
Lfj:q3
TailCall_cDiv;Lam:g1
y/x;q3
dA
Lan:g1
fp_equal(y
gU
Lao:oA
qG
Lfk:q3
TailCall_cEqual;Lap:qX
gV
Lfl:qT
cNot;FP_ReDefinePointers();Lfm:q3
TailCall_cNot;Laq:dG
Lfn:qT
hN;FP_ReDefinePointers();Lfo:q3
d2;Lba:qX
qJ
g4
Lfn;Lbb:g1
fp_less(x
gL
Lbc:g1
fp_lessOrEq(x
gL
Lbe:q1
cRSqrt
d4
Lbf:qE=qI
1)/x;g7
Lbg:dG
q3
Lfl;Lbh:g1
fp_less(y
gU
Lbi:g1
fp_lessOrEq(y
gU
Lbj:g1
fp_max(x
gL
Lbk:g1
fp_min(x
gL
Lbl:g1
fp_mod(y
gU
Lbm:g1
x;qX
h2;h5
A);Lfp:d1
qC
y*x
g5
Lfh;Lbn:h7
cSqr;Lfq:q3
TailCall_cSqr;Lbo:h7
cDiv;q3
Lfj;Lbp:h2;h5
cSqr);d1
Lga:qT
cNeg;qP
dJ;Lbq:qD
q1
cSqr
g5
Lga;Lca:qE=x+qI
1);q0-2]=q8;qD
q1
cPow
d4
Lcb:q0-wK
q3
Lfc;Lcc:g1
x;Lgb:dG
q3
Lfb;Lcd:qD
qX
Lgc:hY
4
q9
Lgd:d6
x);Lge:dF
g5
Lfb;Lce:qX
q3
Lgc;Lcf:qX
hY
4;h5
B
g5
Lgd;Lcg:qX
h2
q9
q3
Lgd;Lch:oA
q3
Lbd;Lci:oA
d0
Lfc;Lcj:qE=x+x;q3
Lcb;Lck:g1
x;q0-4]=q8;qX
g3
d6
y*x);dF);qL
h9
g5
Lfi;Lcl:g1
x;dG
q3
Lfp;Lcm:qX
g3
qC
x+x
g5
Lge;Lcn:hW
q9
g7
Lco:g1
y*x;q3
Lgb;Lcp:qE=-x;q3
Lci;Lcq:g1
x;q0-4]=q8;qX
g3
d6
y*x);dF);qL
gT
cSub;qP
Lfe;Lda:g1
y*x;q3
dA
Ldb:h4=cDup;q2-=1;qX
Lgf:qT
hM;q3
Lfg;Ldc:qX
qJ
g4
Lga;Ldd:q6
Lgg:qL
cSqr
g5
Lfb;Lde:h2
q9
q3
Lgg;Ldf:g1
fp_nequal(y
gU
Ldg:oA
qG
Lgh:q3
TailCall_cNEqual;Ldh:qE=-x;d0
Lfd;Ldi:qE=-x;g7
Ldj:d0
Lfm;Ldk:q1
cAbsNotNot
d4
Ldl:gV
Lee:qL
cAbsNot
d4
Ldm:qE=x;Leb:h7
cEqual;q3
Lfk;Ldn:h7
cNEqual;q3
Lgh;Ldo:h7
cLessOrEq;q3
TailCall_cLessOrEq;Ldp:h7
cLess;q3
TailCall_cLess;Ldq:h7
cGreaterOrEq;q3
TailCall_cGreaterOrEq;Lea:h7
cGreater;q3
TailCall_cGreater;Lec:h7
hN;q3
Lfo;Led:qE=fp_not
d3
Lef:qE=x;q3
Ldn;Leg:g1
fp_or(x
gL
Leh:d0
Lfq;Lei:dE
0));h4=q8;q3
Lfa;Lej:d0
Lgf;Lek:g1
y-x;q3
dA
Lel:qE=-x;q3
Lgf;Lem:g1
x-y;qX
qD
q1
A
d4
Len:qE=-x;oA
q1
OppositeComparisonOpcode(A)d4
Leo:gC
cDup
d4
Lep:h4=cDup;g7
Leq:gC
opcode
d4
g7
q3
TailCall_cAnd;q3
TailCall_cInv;q3
TailCall_cMax;q3
TailCall_cMin;q3
TailCall_cMod;q3
dJ;q3
d2;q3
TailCall_cOr;q3
TailCall_cSub;
#endif
#if(FP_FLOAT_VERSION)
hB
x;gW
A;hB
y;gW
B;gW
C;gW
D;switch(opcode){TailCall_cAbs:h1
dB:qB
gQ
dQ(197,wR
oQ,oQ,);q3
Laa;qO
dQ(40,wN
oQ,"[fp_abs(x)]"
,o8);q3
Lab;gZ
0]oS(258,o2
oQ,"A"
,oE(A)o4);g7}
qF
TailCall_cAcos:h1
cAcos:hS
mM<=d8
dQ(44,"x[x>=Value_t(-1)&&x<=Value_t(1)] cAcos"
,"[fp_acos(x)]"
,o8);q3
Lac;}
qF
TailCall_cAcosh:h1
cAcosh:hS
dI
dQ(41,"x[x>=Value_t(1)] cAcosh"
,"[fp_acosh(x)]"
,o8);q3
Lad;}
qF
TailCall_cAdd:h1
hM:qB
hF
gX
hT
dQ(147,oT" "
oT,"[Value_t(4)]"
dX,);q3
Lae;qQ
h9
gY==hM){qS
3]hT
dQ(148,oT
oV
oT,"cMul [Value_t(4)]"
dX,);q3
Laf;}
}
gJ
qQ
h9
gG
qS
2]hT
dQ(150,"cDup "
m1
oI,"[x+Value_t(1)]"
dX,o8);q3
Lag;}
qQ
gQ
dQ(204,wR
oI,mF,);q3
Lah;qO
gX
qN
dQ(141,w0
oI" "
wN
oI,"[y+x] "
oI,m8);q3
dA
m6
gI
dQ(76,w0
wN
oI,"[y+x]"
,m8);q3
Laj;}
dC
dQ(75,o1
oI,,o8);q3
Lak;}
gJ
qF
TailCall_cAnd:h1
cAnd:qK
q0-1]qN
dQ(130,w0
wN"cAnd"
,"[fp_and(x,y)]"
,m8);q3
Lal;}
qF
TailCall_cAsin:h1
cAsin:hS
mM<=d8
dQ(45,"x[x>=Value_t(-1)&&x<=Value_t(1)] cAsin"
,"[fp_asin(x)]"
,o8);q3
Lam;}
qF
TailCall_cAsinh:h1
cAsinh:h0
dQ(42,wN"cAsinh"
,"[fp_asinh(x)]"
,o8);q3
Lan;qF
TailCall_cAtan:h1
cAtan:h0
dQ(46,wN"cAtan"
,"[fp_atan(x)]"
,o8);q3
Lao;qF
TailCall_cAtan2:h1
cAtan2:qK
q0-1]qN
dQ(83,w0
wN"cAtan2"
,"[fp_atan2(y,x)]"
,m8);q3
Lap;}
qF
TailCall_cAtanh:h1
cAtanh:qK
x>qI
mM<d8
dQ(43,"x[x>Value_t(-1)&&x<Value_t(1)] cAtanh"
,"[fp_atanh(x)]"
,o8);q3
Laq;}
qF
TailCall_cCbrt:h1
cCbrt:h0
dQ(47,wN"cCbrt"
,"[fp_cbrt(x)]"
,o8);q3
Lba;qF
TailCall_cCeil:h1
cCeil:qB
gQ
dQ(267,wR
wU,wL
wB,);q3
Lbb;qO
dQ(48,wN
wU,"[fp_ceil(x)]"
,o8);q3
Lbc;gZ
0
g8
dQ(261,"A[IsAlwaysIntegerOpcode(A)] "
wU,"A"
,oE(A)o4);g7}
qF
TailCall_cCos:h1
cCos:qB
dB:dQ(302,mE
w2,w2,);q3
Lbd
qR
gQ
dQ(304,wR
w2,w2,);q3
Lbd;qO
dQ(49,wN
w2,"[fp_cos(x)]"
,o8);q3
Lbe;qF
TailCall_cCosh:h1
cCosh:qB
dB:dQ(303,mE
oO,oO,);q3
Lbf
qR
cAcosh:dQ(193,"cAcosh "
oO,,);q3
Lbg
qR
cAsinh:dQ(316,"cAsinh "
oO,"[DO_STACKPLUS1] "
wV"[Value_t(1)] "
oI" "
w7,);qZ
Lbh
qR
gQ
dQ(305,wR
oO,oO,);q3
Lbf;qO
dQ(50,wN
oO,"[fp_cosh(x)]"
,o8);q3
Lbi;qF
TailCall_cDeg:h1
cDeg:h0
dQ(65,wN"cDeg"
,"[RadiansToDegrees(x)]"
,o8);q3
Lbj;qF
TailCall_cDiv:h1
cDiv:qB
cCos:dQ(319,w2" "
wI,"cSec"
dX,);q3
Lbk
qR
cCot:dQ(323,"cCot "
wI,mO
dX,);q3
Lbl
qR
cCsc:dQ(321,"cCsc "
wI,mN
dX,);q3
Lbm
qR
hF
dQ(120,"cDup "
wI,"[Value_t(0)]"
oV"[Value_t(1)] "
oI,);q3
Lbn
qR
dH
dQ(273,w3" "
wI,wR
w3
dX,);q3
Lbo
qR
dM
dQ(274,oW" "
wI,wR
oW
dX,);q3
Lbp
qR
cInv:dQ(202,wS" "
wI,"cMul"
,);q3
Lbq
qR
cPow:dQ(272,"cPow "
wI,mK
oH
dX,);q3
Lca
qR
cSec:dQ(322,"cSec "
wI,w2
dX,);q3
Lcb
qR
cSin:dQ(318,"cSin "
wI,"cCsc"
dX,);q3
Lcc
qR
cTan:dQ(320,"cTan "
wI,"cCot"
dX,);q3
Lcd;qO
hV
dI
dQ(72,o5
wI,,o8);q3
Lak;}
if(x!=g2
g9
h9
gY
qN
if((y/x)==GetRadiansToDegreesFactor
gR
dQ(159,"y[(y/x)==GetRadiansToDegreesFactor<Value_t>()]"
oV
oD
wI,"cDeg"
,m8);q3
Lce;}
if((y/x)==GetDegreesToRadiansFactor
gR
dQ(160,"y[(y/x)==GetDegreesToRadiansFactor<Value_t>()]"
oV
oD
wI,m7,m8);q3
Lcf;}
dQ(161,"y"
oV
oD
wI,"[y/x]"
dX,m8);q3
Lcg;qQ
gQ
dQ(138,wR
oD
wI,"[-x] "
wI,o8);q3
Lch
qR
q8:gI
dQ(73,w0
oD
wI,"[y/x]"
,m8);q3
Lci;}
dQ(163,oD
wI,"[Value_t(1)/x]"
dX,o8);q3
Lcj;}
gJ
qF
TailCall_cEqual:h1
cEqual:qK
x==g2
g9
dB:dQ(216,mE
o1
oK,"[x] "
oK,o8);q3
Lck
qR
cSqr:dQ(218,wV
o1
oK,"[x] "
oK,o8);q3
Lck;}
}
wG(128,w0
wN
oK,"[fp_equal(y,x)]"
,m8);q3
Lcl;}
qF
TailCall_cExp:h1
dH
qB
hM
gG
dQ(269,wN
oI" "
w3,w3" [fp_exp(x)]"
dX,o8);q3
Lcm;qQ
cLog:A
m5
1]oS(183,o2
wF" "
w3,"A"
,oE(A)o4);q3
Lbg;}
gJ
qO
dQ(51,wN
w3,"[fp_exp(x)]"
,o8);q3
Lcn;qF
TailCall_cExp2:h1
dM
qB
hM
gG
dQ(270,wN
oI" "
oW,oW" [fp_exp2(x)]"
dX,o8);q3
Lco;qQ
cLog2:A
m5
1]oS(184,o2
oP" "
oW,"A"
,oE(A)o4);q3
Lbg;}
gJ
qO
dQ(52,wN
oW,"[fp_exp2(x)]"
,o8);q3
Lcp;}
dQ(362,oW,"[DO_STACKPLUS1] [fp_log(Value_t(2))]"
oV
w3,);qZ
Lcq;TailCall_cFloor:h1
cFloor:qB
gQ
dQ(266,wR
wL,wU
wB,);q3
Lda;qO
dQ(53,wN
wL,"[fp_floor(x)]"
,o8);q3
Ldb;gZ
0
g8
dQ(260,"A[IsAlwaysIntegerOpcode(A)] "
wL,"A"
,oE(A)o4);g7}
qF
TailCall_cGreater:h1
cGreater:qK
q0-1]qN
dQ(126,w0
wN
w1,"[fp_less(x,y)]"
,m8);q3
Ldc;}
hV-d7
dQ(296,"x[x==Value_t(-0.5)] "
w1,wR
oZ,o8);q3
Ldd;}
qF
TailCall_cGreaterOrEq:h1
cGreaterOrEq:qK
x!=g2
hG
dB){dQ(292,mE
oD
oL,"[Value_t(0.5)/x]"
oV
oX,o8);q3
Lde;}
}
wG(127,w0
wN
oL,"[fp_lessOrEq(x,y)]"
,m8);q3
Ldf;}
hV
d7
dQ(295,"x[x==Value_t(0.5)] "
oL,"cAbsNotNot"
,o8);q3
Ldg;}
qF
TailCall_cInt:h1
cInt:qB
hP
dQ(54,wN"cInt"
,"[fp_int(x)]"
,o8);q3
Ldh;gZ
0
g8
dQ(262,"A[IsAlwaysIntegerOpcode(A)] cInt"
,"A"
,oE(A)o4);g7}
qF
TailCall_cInv:h1
cInv:qB
cCos:dQ(325,w2
mQ,"cSec"
,);q3
Ldi
qR
cCot:dQ(329,"cCot"
mQ,mO,);q3
Ldj
qR
cCsc:dQ(327,"cCsc"
mQ,mN,);q3
Ldk
qR
cInv:dQ(198,wS
mQ,,);q3
Lbg
qR
cPow:dQ(200,"cPow"
mQ,mK
oH,);q3
Ldl
qR
cSec:dQ(328,"cSec"
mQ,w2,);q3
Ldm
qR
cSin:dQ(324,"cSin"
mQ,"cCsc"
,);q3
Ldn
qR
cSqrt:dQ(67,w7
mQ,"cRSqrt"
,);q3
Ldo
qR
cTan:dQ(326,"cTan"
mQ,"cCot"
,);q3
Ldp;hO!=wH(69,oD
wS,"[Value_t(1)/x]"
,o8);q3
Ldq;}
gJ
qF
TailCall_cLess:h1
cLess:qK
x!=g2
hG
dB){dQ(291,mE
oD
wT,"[Value_t(0.5)/x]"
oV
m4,o8);q3
Lea;}
}
wG(124,w0
wN
wT,"[fp_less(y,x)]"
,m8);q3
Leb;}
hV
d7
dQ(294,"x[x==Value_t(0.5)] "
wT,oZ,o8);q3
Lec;}
qF
TailCall_cLessOrEq:h1
cLessOrEq:qK
q0-1]qN
dQ(125,w0
wN
oY,"[fp_lessOrEq(y,x)]"
,m8);q3
Led;}
hV-d7
dQ(297,"x[x==Value_t(-0.5)] "
oY,wR"cAbsNotNot"
,o8);q3
Lee;}
qF
TailCall_cLog:h1
cLog:wM(186,w3" "
wF,,);q3
Lbg
gA
dQ(374,o9
oV
wF,wF" [fp_log(x)] "
oI,o8);q3
Lef;}
hU
dQ(103,wV
wF,mE
wF" "
oT,);q3
Leg;hO>wH(55,o9" "
wF,"[fp_log(x)]"
,o8);q3
Leh;}
gJ
qF
TailCall_cLog10:h1
cLog10:wM(364,w3" "
oN,"[DO_STACKPLUS1] [fp_log10(fp_const_e<Value_t>())]"
dX,);qZ
Lei
gA
dQ(375,o9
oV
oN,oN" [fp_log10(x)] "
oI,o8);q3
Lej;}
hU
dQ(105,wV
oN,mE
oN" "
oT,);q3
Lek;hO>wH(56,o9" "
oN,"[fp_log10(x)]"
,o8);q3
Lel;}
gJ
qF
TailCall_cLog2:h1
cLog2:wM(363,w3" "
oP,"[DO_STACKPLUS1] [fp_log2(fp_const_e<Value_t>())]"
dX,);qZ
Lem
qR
dM
dQ(187,oW" "
oP,,);q3
Lbg
gA
dQ(373,o9
oV
oP,oP" [fp_log2(x)] "
oI,o8);q3
Len;}
hU
dQ(104,wV
oP,mE
oP" "
oT,);q3
Leo;hO>wH(57,o9" "
oP,"[fp_log2(x)]"
,o8);q3
Lep;}
gJ
qF
TailCall_cMax:h1
cMax:qB
hF
dQ(110,"cDup "
w8,,);q3
Lbg;qO
wG(82,w0
wN
w8,"[fp_max(x,y)]"
,m8);q3
Leq;}
gJ
gZ
0
h6
g9
hF
wZ(112,m0"cDup "
w4
w8,"B cDup"
,oE(A)m9(B)o4);q3
Lbg;qQ
cMax:wZ(114,m0
w8" "
w4
w8,"B "
w8,oE(A)m9(B)o4);q3
Lbg;}
gJ}
}
qF
TailCall_cMin:h1
cMin:qB
hF
dQ(109,"cDup "
w9,,);q3
Lbg;qO
wG(81,w0
wN
w9,"[fp_min(x,y)]"
,m8);q3
Lfa;}
gJ
gZ
0
h6
g9
hF
wZ(111,m0"cDup "
w4
w9,"B cDup"
,oE(A)m9(B)o4);q3
Lbg;qQ
cMin:wZ(113,m0
w9" "
w4
w9,"B "
w9,oE(A)m9(B)o4);q3
Lbg;}
gJ}
}
qF
TailCall_cMod:h1
cMod:qK
x!=g2
wG(74,w0
oD"cMod"
,"[fp_mod(y,x)]"
,m8);q3
Lfb;}
}
qF
TailCall_cMul:h1
h9:qB
hM:hG
q8){y=qE;A
qV
A
qY
qS
3]==q8){x=dK;dQ(245,wN
w4
w0
oI
dX,"[x] A"
oV
w5" "
oI,dU" with "
w0
wQ
y<<mB(A)mC);q3
Lfc;}
}
}
q3
hC
qR
hF
dQ(359,"cDup"
dX,mL,);q3
Lfd
qR
cInv:dQ(203,wS
dX,wI,);q3
Lfe
qR
gQ
A
m5
1
h6
gP
h9:B
m5
3
hI
dQ(351,oJ
oV
w4
mK
dX,w6
oV
mK,oE(A)m9(B)o4);q3
Lff;}
q3
Default2;g6
Default2:;wZ(345,m0
w4
mK
dX,w6
wB,oE(A)m9(B)o4);q3
Lfg;}
}
}
q3
hC
qR
cPow
gG
qS
2]hT
dQ(151,"cDup "
m2
dX,"[x+Value_t(1)]"
oH,o8);q3
Lfh;}
}
q3
hC;qO
dC
g9
h9:A
qV
A
qY
dQ(94,dY
oV
oC,mD,mJ(A)o4);q3
Lfi;}
q3
Default3;g6
Default3:;A
m5
1
g0
IsBinaryOpcode(A)qW
A)){gP
q8:gI
dQ(93,w0
dT,mD,mJ(A)<<","
wD);q3
Lfj;g6
B
qV
IsBinaryOpcode(B)qW
B)){qM-3]){h1
q8:gI
dQ(97,w0
o0,"A"
wE,mJ(A)m9(B)<<","
wD);q3
Lfk;g6
C
m5
3
g0
C
qY
dQ(95,"C[C>=VarBegin] "
o0,"A"
wE,mJ(A)m9(B)<<", C "
dS(C)o4);q3
Lfl;}
dD
C)qW
C
wX(96,"C[IsUnaryOpcode(C)&&!HasInvalidRangesOpcode(C)] "
o0,"B A"
wE,mJ(A)m9(B)<<", C "
dS(C)o4);q3
Lfm;}
}
}
if(B
qY
dQ(91,"B[B>=VarBegin] "
dT,mD,mJ(A)m9(B)o4);q3
Lfi;}
dD
B)qW
B
wX(92,"B[IsUnaryOpcode(B)&&!HasInvalidRangesOpcode(B)] "
dT,"A"
wE,mJ(A)m9(B)o4);q3
Lfn;}
}
}
if(A
qY
dQ(89,w4
oC,"[x]"
,mJ(A)o4);q3
Lfo;}
dD
A)qW
A
wX(90,"A[IsUnaryOpcode(A)&&!HasInvalidRangesOpcode(A)] "
oC,mD,mJ(A)o4);q3
Lfp;}
}
}
g9
hM:gP
hF
if(x+x
wW(154,"cDup[x+x==Value_t(1)] "
oI" "
wA,,o8);q3
Lfq;}
dQ(155,oT" "
wA,"[x+x]"
dX,o8);q3
Lga
qR
h9:qS
3]qN
A
m5
4
h6
dQ(243,w4"y"
oV
oI" "
wA,mP"A "
w5
oV
oI,dV", "
w0
wQ
y<<mB(A)o4);q3
Lgb;}
m6
gI
dQ(242,w0
oI" "
wA,mP
w5" "
oI,m8);q3
Lgc;qQ
cDeg:dQ(106,"cDeg "
wA,"[RadiansToDegrees(x)]"
dX,o8);q3
Lgd
qR
h9:gP
hM:qS
3]hT
if(x+x
wW(156,"cDup[x+x==Value_t(1)] "
oI
oV
wA,"cMul"
,o8);q3
Lge;}
dQ(157,oT
oV
wA,"cMul [x+x]"
dX,o8);q3
Lgf;m6
y=q2[-1
g0(y*x)==GetRadiansToDegreesFactor
gR
dQ(143,"y[(y*x)==GetRadiansToDegreesFactor<Value_t>()]"
oV
wA,"cDeg"
,m8);q3
Lce;}
if((y*x)==GetDegreesToRadiansFactor
gR
dQ(144,"y[(y*x)==GetDegreesToRadiansFactor<Value_t>()]"
oV
wA,m7,m8);q3
Lcf;}
if(y*x
wW(145,"y[y*x==Value_t(1)]"
oV
wA,,m8);q3
Lgg;}
dQ(146,"y"
oV
wA,w5
dX,m8);q3
Lgh;qQ
gQ
dQ(135,wR
wA,oR,o8);q3
Lgi
qR
cRad:dQ(107,m7" "
wA,"[DegreesToRadians(x)]"
dX,o8);q3
Lgj
qR
cSub
gY==h9){qS
3]qN
A
m5
4
h6
dQ(244,w4"y"
oV"cSub "
wA,mP"A "
w5
oV
mF,dV", "
w0
wQ
y<<mB(A)o4);q3
Lgk;}
}
m6
gI
dQ(71,w0
wA,w5,m8);q3
Lgl;}
hV
dI
dQ(70,"x[x==Value_t(1)]"
dX,,o8);q3
Lak;}
hV
2
wX(100,"x[x==Value_t(2)]"
dX,oT,o8);q3
Lgm;}
gD
GetRadiansToDegreesFactor
gR
dQ(101,"x[x==GetRadiansToDegreesFactor<Value_t>()]"
dX,"cDeg"
,o8);q3
Lgn;}
gD
GetDegreesToRadiansFactor
gR
dQ(102,"x[x==GetDegreesToRadiansFactor<Value_t>()]"
dX,m7,o8);q3
Lgo;}
hV-dI
dQ(137,"x[x==Value_t(-1)]"
dX,mK,o8);q3
Lgp;}
gJ
g6
hC:;A=q0
0
h6
g9
h9:gP
gQ
B
m5
3
hI
dQ(348,oJ
wB
oV
dY
dX,w6
oV
mK,oE(A)m9(B)o4);q3
Lff;}
q3
Default4;g6
Default4:;wZ(339,oJ
oV
dY
dX,w6
dX,oE(A)m9(B)o4);q3
Lgq;}
}
q3
hQ
qR
gQ
wZ(342,oJ
wB" "
dY
dX,w6
wB,oE(A)m9(B)o4);q3
Lfg;}
q3
hQ;g6
hQ:;B
m5
1
hI
dQ(336,m0
dY
dX,w6,oE(A)m9(B)o4);q3
Lfd;}
}
}
dD
A)){B
m5
1
g0
B
gB
1){qS
2]==h9){C
m5
3
g0
C==A){D
m5
4
g0
D==B){dQ(358,"D[D==B] C[C==A]"
oV"B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]"
dX,"D C "
mR
dX,oE(A)m9(B)<<", C "
dS(C)<<", D "
dS(D)o4);q3
Lha;}
}
}
}
}
qF
TailCall_cNEqual:h1
cNEqual:qK
x==g2
g9
dB:dQ(217,mE
o1
oB,wJ,o8);q3
Lhb
qR
cSqr:dQ(219,wV
o1
oB,wJ,o8);q3
Lhb;}
}
wG(129,w0
wN
oB,"[fp_nequal(y,x)]"
,m8);q3
Lhc;}
qF
dJ:h1
gQ
qB
h9
gG
dQ(136,m1
mK,oR,o8);q3
Lhd;qQ
gQ
dQ(199,mK
wB,,);q3
Lbg
qR
cSin:hH
dQ(310,m1
mN
wB,oR" cSin"
,o8);q3
Lhe;}
qQ
dN
hH
dQ(311,m1
mG
wB,oR" cSinh"
,o8);q3
Lhf;}
qQ
cTan:hH
dQ(312,m1
mO
wB,oR" cTan"
,o8);q3
Lhg;}
qQ
dO
hH
dQ(313,m1
mH
wB,oR" cTanh"
,o8);q3
Lhh;}
}
gJ
qO
dQ(68,"x"
wB,"[-x]"
,o8);q3
Lhi;qF
TailCall_cNot:h1
cNot:qB
dB:dQ(249,oQ
wC,m4,);q3
Lhj
qR
cAbsNot:A
m5
1
g0
IsLogicalOpcode(A
wX(254,"A[IsLogicalOpcode(A)] "
oZ
wC,"A"
,oE(A)o4);q3
Lbg;}
if(A!=q8){dQ(255,"A[A!=cImmed] "
oZ
wC,"A cAbsNotNot"
,oE(A)o4);q3
Lhk;}
q3
hR
qR
cAbsNotNot:dQ(252,"cAbsNotNot"
wC,oZ,);q3
Lhl
qR
cEqual:dQ(213,oK
wC,oB,);q3
Lhm
qR
cGreater:dQ(211,w1
wC,oY,);q3
Lhn
qR
cGreaterOrEq:dQ(212,oL
wC,wT,);q3
Lho
qR
cLess:dQ(209,wT
wC,oL,);q3
Lhp
qR
cLessOrEq:dQ(210,oY
wC,w1,);q3
Lhq
qR
cNEqual:dQ(214,oB
wC,oK,);q3
Lia
qR
gQ
dQ(248,mK
wC,m4,);q3
Lhj
qR
cNot:dQ(250,m4
wC,oX,);q3
Lib
qR
hN:dQ(251,oX
wC,m4,);q3
Lhj;qO
dQ(85,"x"
wC,"[fp_not(x)]"
,o8);q3
Lic;g6
hR:;A=h4
oS(256,o2
m4,"A "
oZ,oE(A)o4);q3
Lid;}
qF
TailCall_cOr:h1
cOr:qK
q0-1]qN
dQ(131,w0
wN"cOr"
,"[fp_or(x,y)]"
,m8);q3
Lie;}
qF
TailCall_cPow:h1
cPow:qK!gH
x+x)){hG
cSqr){dQ(14,wV"x[!isEvenInteger(x+x)]"
oH,oQ" [x+x]"
oH,o8);q3
Lif;}
}
if(IsIntegerConst(x)){g9
dH
dQ(35,w3" "
o6,mP
w3,o8);q3
Lig
qR
dM
dQ(36,oW" "
o6,mP
oW,o8);q3
Lih
qR
cPow
gY
qN
if(!IsIntegerConst(y
wX(34,"y[!IsIntegerConst(y)]"
oH" "
o6,w5
oH,m8);q3
Lii;}
}
dQ(37,"cPow "
o6,mD
oH,o8);q3
Lij;}
}
if(gH
x)){g9
dB:dQ(299,oQ" x[isEvenInteger(x)]"
oH,"[x]"
oH,o8);q3
Lik
qR
h9
gY==dB){dQ(300,oQ
oV"x[isEvenInteger(x)]"
oH,"cMul [x]"
oH,o8);q3
Lil;}
gJ}
}
dC
dQ(122,o1"cPow"
,"[Value_t(0)]"
oV"[Value_t(1)] "
oI,o8);q3
Lim;}
hV
d7
dQ(175,"x[x==Value_t(0.5)]"
oH,w7,o8);q3
Lin;}
hV
1)/qI
3
wX(176,"x[x==Value_t(1)/Value_t(3)]"
oH,"cCbrt"
,o8);q3
Lio;}
hV
1)/qI-3
wX(177,"x[x==Value_t(1)/Value_t(-3)]"
oH,"cCbrt"
mQ,o8);q3
Lip;}
hV-d7
dQ(178,"x[x==Value_t(-0.5)]"
oH,"cRSqrt"
,o8);q3
Liq;}
hV-dI
dQ(179,"x[x==Value_t(-1)]"
oH,wS,o8);q3
Lja;}
g9
cPow
gY
qN
if(gH
y)&&!gH
x*y
wX(13,"y[isEvenInteger(y)&&!isEvenInteger(x*y)]"
oH" "
m2,mE
w5
oH,m8);q3
Ljb;}
dQ(173,"y"
oH" "
m2,w5
oH,m8);q3
Lii;hU
dQ(38,wV
m2,"[x+x]"
oH,o8);q3
Ljc
qR
q8:y=q2[-1
g0
y!=qI
0)||x>=wH(84,"y[y!=Value_t(0)||x>=Value_t(0)] "
m2,"[fp_pow(y,x)]"
,m8);q3
Ljd;}
gJ}
dQ(333,m2,"[DO_POWI]"
,o8);if(TryCompilePowi(x))g7
qF
TailCall_cRad:h1
cRad:h0
dQ(66,wN
m7,"[DegreesToRadians(x)]"
,o8);q3
Lje;qF
TailCall_cSin:h1
cSin:qB
gQ
dQ(306,wR
mN,mN
wB,);q3
Ljf;qO
dQ(58,wN
mN,"[fp_sin(x)]"
,o8);q3
Ljg;qF
TailCall_cSinh:h1
dN
qB
cAcosh:dQ(315,"cAcosh cSinh"
,"[DO_STACKPLUS1] "
wV"[Value_t(-1)] "
oI" "
w7,);qZ
Ljh
qR
cAsinh:dQ(192,"cAsinh cSinh"
,,);q3
Lbg
qR
gQ
dQ(307,wR
mG,mG
wB,);q3
Lji;qO
dQ(59,wN
mG,"[fp_sinh(x)]"
,o8);q3
Ljj;qF
TailCall_cSqr:h1
cSqr:qB
dB:dQ(354,oQ" "
mR,mL,);q3
Ljk
qR
gQ
dQ(353,wR
mL,mL,);q3
Ljk
qR
cSqrt:A
m5
1]oS(181,o2
w7" "
mR,"A"
,oE(A)o4);q3
Lbg;}
gJ
qF
TailCall_cSqrt:h1
cSqrt:qB
cSqr:dQ(15,wV
w7,oQ,);q3
Ljl;hO>=wH(60,"x[x>=Value_t(0)] "
w7,"[fp_sqrt(x)]"
,o8);q3
Ljm;}
gJ
qF
TailCall_cSub:h1
cSub:qB
hF
dQ(152,"cDup cSub"
,"[Value_t(0)]"
dX,);q3
Ljn
qR
gQ
dQ(205,wR
mF,oI,);q3
Ljo;qO
dC
dQ(77,o1
mF,,o8);q3
Lak;}
wG(78,w0
wN
mF,"[y-x]"
,m8);q3
Ljp;}
dQ(168,wN
mF,"[-x] "
oI,o8);q3
Ljq;qF
TailCall_cTan:h1
cTan:qB
cAtan2:dQ(196,"cAtan2 cTan"
,wI,);q3
Lfe
qR
gQ
dQ(308,wR
mO,mO
wB,);q3
Lka;qO
dQ(62,wN
mO,"[fp_tan(x)]"
,o8);q3
Lkb;qF
TailCall_cTanh:h1
dO
qB
gQ
dQ(309,wR
mH,mH
wB,);q3
Lkc;qO
dQ(63,wN
mH,"[fp_tanh(x)]"
,o8);q3
Lkd;qF
TailCall_cTrunc:h1
cTrunc:qB
hP
dQ(64,wN"cTrunc"
,"[fp_trunc(x)]"
,o8);q3
Lke;gZ
0
g8
dQ(259,"A[IsAlwaysIntegerOpcode(A)] cTrunc"
,"A"
,oE(A)o4);g7}
qF
g6
Default0:;A=opcode;if(IsComparisonOpcode(A)){h0
gX
qN
dQ(221,w0
oI
dZ,"[x-y] A"
,oE(A)mA
wD);q3
Lkf;qQ
cAtan:if(hK<fp_const_pi<hB>()*qI
d7
dQ(237,"cAtan[fp_abs(x)<fp_const_pi<Value_t>()*Value_t(0.5)]"
dZ,"[fp_tan(x)] A"
,oE(A)mC);q3
Lkg;qQ
dH
if(x>wH(227,"cExp[x>Value_t(0)]"
dZ,"[fp_log(x)] A"
,oE(A)mC);q3
Lkh;qQ
dM
if(x>wH(228,"cExp2[x>Value_t(0)]"
dZ,"[fp_log2(x)] A"
,oE(A)mC);q3
Lki;qQ
cLog:qU
dQ(230,o7
wF
dZ,"B [fp_exp(x)] A"
,oE(A)mA
oU(B)o4);q3
Lkj;qQ
cLog10:qU
dQ(232,o7
oN
dZ,"B [fp_pow(Value_t(10),x)] A"
,oE(A)mA
oU(B)o4);q3
Lkk;qQ
cLog2:qU
dQ(231,o7
oP
dZ,"B [fp_exp2(x)] A"
,oE(A)mA
oU(B)o4);q3
Lkl;qQ
h9
gY
qN
if(y>wH(223,"y[y>Value_t(0)]"
dX
dZ,"[x/y] A"
,oE(A)mA
wD);q3
Lkm;}
if(y<wH(224,"y[y<Value_t(0)]"
dX
dZ,"[x/y] {OppositeComparisonOpcode(A)}"
,oE(A)mA
wD);q3
Lkn;}
qQ
gQ
dQ(222,mK
dZ,"[-x] {OppositeComparisonOpcode(A)}"
,oE(A)mC);q3
Lko
qR
cPow:if(x>g2
qS
2]qN
if(y>wH(225,"y[y>Value_t(0)] cPow[x>Value_t(0)]"
dZ,"[fp_pow(x,1/y)] A"
,oE(A)mA
wD);q3
Lkp;}
}
qQ
dN
dQ(238,mG
dZ,"[fp_asinh(x)] A"
,oE(A)mC);q3
Lkq
qR
cSqr:if(x>wH(226,"cSqr[x>Value_t(0)]"
dZ,oQ" [fp_sqrt(x)] A"
,oE(A)mC);q3
Lla;qQ
dO
if(hK<d8
dQ(239,"cTanh[fp_abs(x)<Value_t(1)]"
dZ,"[fp_atanh(x)] A"
,oE(A)mC);q3
Llb;}
gJ}
}
}
if(A
gB
0){B=q0
0
hI
dQ(356,m0"A[A>=VarBegin&&data->ByteCode.size()>0]"
,"B cDup"
,oE(A)m9(B)o4);q3
Llc;}
}
dD
A)){B=q0
0
g0
B
gB
1){C
m5
1
g0
C==A){D
qV
D==B){dQ(357,"D[D==B] C[C==A] B[B>=VarBegin&&data->ByteCode.size()>1] A[IsUnaryOpcode(A)]"
,"D C cDup"
,oE(A)m9(B)<<", C "
dS(C)<<", D "
dS(D)o4);q3
Lld;}
}
}
}
}
q3
Lle;Laa:qG
Llf:q3
TailCall_cAbs;Lab:qE=hK;g7
Lac:qE=fp_acos
d3
Lad:qE=fp_acosh
d3
Lae:dE
4));q0-2]=q8;gO-=2;Llg:q6
Llh:qT
h9;Lli:FP_ReDefinePointers();Llj:q3
TailCall_cMul;Laf:g3
dE
4));Llk:dF
gS
Lag:qE=x+qI
1);q0-wK
Lll:qT
h9;q3
Llj;Lah:h7
cSub;Llm:q3
TailCall_cSub;Lai:g1
y+x;dG
Lln:FP_ReDefinePointers();Llo:q3
TailCall_cAdd;Laj:g1
y+x;Lak:qX
Lbg:gV
g7
Lal:g1
fp_and
mI
Lam:qE=fp_asin
d3
Lan:qE=fp_asinh
d3
Lao:qE=fp_atan
d3
Lap:g1
fp_atan2
m3
Laq:qE=fp_atanh
d3
Lba:qE=fp_cbrt
d3
Lbb:q1
cFloor);Llp:qT
cNeg;qP
dJ;Lbc:qE=fp_ceil
d3
Lbd:qG
Llq:q3
TailCall_cCos;Lbe:qE=fp_cos
d3
Lbf:d0
TailCall_cCosh;Lbh:q1
cSqr);dE
1));Lma:dF);qL
hM);Lmb:qT
cSqrt;qP
TailCall_cSqrt;Lbi:qE=fp_cosh
d3
Lbj:qE=RadiansToDegrees
d3
Lbk:q1
cSec
gS
Lbl:q1
cTan
gS
Lbm:q1
cSin
gS
Lbn:dE
0));h4=q8;Lmc:d1
dE
1));Lmd:dF);Lme:qT
hM;q3
Lln;Lbo:q1
cNeg);qL
cExp
gS
Lbp:q1
cNeg);qL
cExp2
gS
Lbq:d0
Lll;Lca:q1
cNeg);qL
cPow
gS
Lcb:q1
cCos
gS
Lcc:q1
cCsc
gS
Lcd:q1
cCot
gS
Lce:hW
q9
Lmf:qT
cDeg;qP
TailCall_cDeg;Lcf:hW
q9
Lmg:qT
cRad;qP
TailCall_cRad;Lcg:g1
y/x;qX
q3
Llg;Lch:qE=-x;oA
qG
Lmh:q3
TailCall_cDiv;Lci:g1
y/x;q3
Lak;Lcj:qE=qI
1)/x;q3
Lll;Lck:oA
qG
Lmi:q3
TailCall_cEqual;Lcl:g1
fp_equal
m3
Lcm:qX
qD
q1
cExp);qC
fp_exp(x)g5
Llk;Lcn:qE=fp_exp
d3
Lco:qX
qD
q1
cExp2);qC
fp_exp2(x)g5
Llk;Lcp:qE=fp_exp2
d3
Lcq:qC
dP(qI
2)));dF);Lmj:qL
gT
cExp;qP
TailCall_cExp;Lda:q1
cCeil
g5
Llp;Ldb:qE=fp_floor
d3
Ldc:g1
fp_less
mI
Ldd:qX
q1
cNeg);Lid:qL
cAbsNot
d4
Lde:qE=qI
0.5)/x;oA
q1
h9);Lmk:qL
hN
d4
Ldf:g1
fp_lessOrEq
mI
Ldg:qX
Lhk:gV
Lml:qL
cAbsNotNot
d4
Ldh:qE=fp_int
d3
Ldi:q1
cSec
d4
Ldj:h7
cTan;Lmm:q3
TailCall_cTan;Ldk:h7
cSin;Lmn:q3
TailCall_cSin;Ldl:q1
cNeg);qT
cPow;Lmo:FP_ReDefinePointers();Lmp:q3
TailCall_cPow;Ldm:h7
cCos;q3
Llq;Ldn:q1
cCsc
d4
Ldo:q1
cRSqrt
d4
Ldp:q1
cCot
d4
Ldq:qE=qI
1)/x;g7
Lea:qE=qI
0.5)/x;oA
q1
gT
cNot;FP_ReDefinePointers();Lmq:q3
TailCall_cNot;Leb:g1
fp_less
m3
Lec:qX
Lhl:qJ
g4
Lid;Led:g1
fp_lessOrEq
m3
Lee:qX
q1
cNeg
g5
Lml;Lef:qX
qD
q1
cLog);qC
dP(x)g5
Lmd;Leg:h3
qL
cLog);Lna:gC
cDup
g5
Lme;Leh:qE=dP
d3
Lei:qC
hL
fp_const_e<hB>()));Lnb:h4=q8;q3
Llh;Lej:qX
qD
q1
cLog10);qC
hL
x)g5
Lmd;Lek:h3
qL
cLog10
g5
Lna;Lel:qE=hL
x
d4
Lem:qC
hX
fp_const_e<hB>())g5
Lnb;Len:qX
qD
q1
cLog2);qC
hX
x)g5
Lmd;Leo:h3
qL
cLog2
g5
Lna;Lep:qE=hX
x
d4
Leq:g1
fp_max
mI
Lfa:g1
fp_min
mI
Lfb:g1
fp_mod
m3
Lfc:g1
x;qX
h2;h5
A);Lnc:d1
qC
y*x
g5
Lmd;Lfd:h7
cSqr;Lnd:q3
TailCall_cSqr;Lfe:h7
cDiv;q3
Lmh;Lff:h2;h5
cSqr);qL
h9
g5
Llp;Lfg:qD
q1
cSqr
g5
Llp;Lfh:qE=x+qI
1);q0-wK
qT
cPow;q3
Lmp;Lfi:q0-wK
q3
Llj;Lfj:g1
x;Lne:dG
q3
Lli;Lfk:qD
qX
Lnf:hY
4
q9
Lng:d6
x);Lnh:dF
g5
Lli;Lfl:qX
q3
Lnf;Lfm:qX
hY
4;h5
B
g5
Lng;Lfn:qX
h2
q9
q3
Lng;Lfo:oA
q3
Lbg;Lfp:oA
d0
Llj;Lfq:qX
Lni:h2
q9
g7
Lga:qE=x+x;q3
Lfi;Lgb:g1
x;q0-4]=q8;qX
g3
d6
y*x);dF);qL
h9
g5
Lme;Lgc:g1
x;dG
q3
Lnc;Lgd:qE=RadiansToDegrees(x
g5
Lfp;Lge:qX
hY
4
q9
q3
Lli;Lgf:qX
g3
qC
x+x
g5
Lnh;Lgg:qD
qX
q3
Lni;Lgh:g1
y*x;q3
Lne;Lgi:qE=-x;q3
Lfp;Lgj:qE=DegreesToRadians(x
g5
Lfp;Lgk:g1
x;q0-4]=q8;qX
g3
d6
y*x);dF);qL
gT
cSub;qP
Llm;Lgl:g1
y*x;q3
Lak;Lgm:h4=cDup;q2-=1;qX
Lnj:qT
hM;q3
Llo;Lgn:qX
qJ
g4
Lmf;Lgo:qX
qJ
g4
Lmg;Lgp:qX
qJ
g4
Llp;Lgq:q6
Lnk:qL
cSqr
g5
Lli;Lha:h2
q9
q3
Lnk;Lhb:oA
qG
Lnl:q3
TailCall_cNEqual;Lhc:g1
fp_nequal
m3
Lhd:qE=-x;q3
Lbq;Lhe:qE=-x;d5
cSin;qP
Lmn;Lhf:qE=-x;d5
cSinh;qP
TailCall_cSinh;Lhg:qE=-x;d5
cTan;qP
Lmm;Lhh:qE=-x;d5
cTanh;qP
TailCall_cTanh;Lhi:qE=-x;g7
Lhj:d0
Lmq;Lhm:h7
cNEqual;q3
Lnl;Lhn:h7
cLessOrEq;q3
TailCall_cLessOrEq;Lho:h7
cLess;q3
TailCall_cLess;Lhp:h7
cGreaterOrEq;q3
TailCall_cGreaterOrEq;Lhq:h7
cGreater;q3
TailCall_cGreater;Lia:h7
cEqual;q3
Lmi;Lib:qJ
g4
Lmk;Lic:qE=fp_not
d3
Lie:g1
fp_or
mI
Lif:qX
qD
h3
qC
x+x);Lnm:dF
g5
Lmo;Lig:oA
qJ
g4
Lmj;Lih:oA
q1
gT
cExp2;qP
TailCall_cExp2;Lii:g1
y*x;dG
q3
Lmo;Lij:oA
q1
h9
g5
Lmo;Lik:oA
d0
Lmp;Lil:qX
h2;h5
h9);qC
x
g5
Lnm;Lim:qE=qI
0
g5
Lmc;Lin:qX
qJ
g4
Lmb;Lio:qX
gV
qT
cCbrt;qP
TailCall_cCbrt;Lip:qX
q1
cCbrt);Lnn:qT
cInv;qP
TailCall_cInv;Liq:qX
q3
Ldo;Lja:qX
qJ
g4
Lnn;Ljb:hW;hD
h3
qC
y*x
g5
Lnm;Ljc:qE=x+x;q3
Lik;Ljd:g1
d9
y
gN
Lje:qE=DegreesToRadians
d3
Ljf:q1
cSin
g5
Llp;Ljg:qE=fp_sin
d3
Ljh:q1
cSqr);dE-1)g5
Lma;Lji:q1
cSinh
g5
Llp;Ljj:qE=fp_sinh
d3
Ljk:d0
Lnd;Ljl:h7
dB;q3
Llf;Ljm:qE=fp_sqrt
d3
Ljn:dE
0)g5
Lnb;Ljo:d0
Lnj;Ljp:g1
y-x;q3
Lak;Ljq:qE=-x;q3
Lnj;Lka:q1
cTan
g5
Llp;Lkb:qE=fp_tan
d3
Lkc:q1
cTanh
g5
Llp;Lkd:qE=fp_tanh
d3
Lke:qE=fp_trunc
d3
Lkf:g1
x-y;Lno:dG
Lnp:qL
A
d4
Lkg:qE=fp_tan(x);Lnq:oA
qJ
g4
Lnp;Lkh:qE=dP(x
h8
Lki:qE=hX
x
h8
Lkj:qE=fp_exp(x
h8
Lkk:qE=d9
qI
10),x
h8
Lkl:qE=fp_exp2(x
h8
Lkm:g1
x/y;q3
Lno;Lkn:g1
x/y;dG
Loa:qL
OppositeComparisonOpcode(A)d4
Lko:qE=-x;oA
qJ
g4
Loa;Lkp:g1
d9
x,1/y
g5
Lno;Lkq:qE=fp_asinh(x
h8
Lla:qX
qD
h3
qC
fp_sqrt(x));dF
g5
Lnp;Llb:qE=fp_atanh(x
h8
Llc:gC
cDup
d4
Lld:h4=cDup;g7
Lle:gC
opcode
d4
g7
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
TailCall_cSin;q3
TailCall_cSinh;q3
TailCall_cSqrt;q3
TailCall_cSub;q3
TailCall_cTan;q3
TailCall_cTanh;q3
TailCall_cTrunc;
#endif
#undef FP_ReDefinePointers
#undef FP_TRACE_BYTECODE_OPTIMIZATION
#undef FP_TRACE_OPCODENAME
