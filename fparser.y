/*

Priority levels:

1  ,
2  |
3  &
4  = < > != <= >=
5  + -    (BINARY)
6  * / %
7  ! -    (UNARY)
8  ^        (note: right-associative)
9  units
10 func

*/
%left OrOp
%left AndOp
%left CompOp
%left Plus Minus
%left TimesMulModOp
%left Bang
%right Pow
%token NumConst  If  Eval
%token PreFunc1  PreFunc2  PreFunc3
%left Identifier
%token LParens RParens Comma
%token Garbage /* This is for anychar{} in re2c syntax */

%%

exp:
		NumConst
	|	PreFunc1 LParens exp RParens
	|	PreFunc2 LParens exp Comma exp RParens
	|	PreFunc3 LParens exp Comma exp Comma exp RParens
	|	Eval	 f_parms_list_0
	|	If       LParens exp { xx } Comma exp { yy } Comma exp RParens
	|	Identifier f_parms_list_0   /* fcall/pcall */
	|	Identifier                  /* var / const */
	|	exp Plus   exp
	|	exp Minus  exp
	|	exp AndOp  exp
	|	exp OrOp   exp
	|	exp CompOp exp
	|	exp TimesMulModOp exp
	|	exp Pow exp            /* cPow */
	|	Bang exp               /* cNot */
	|	Minus exp  %prec Bang  /* cNeg */
	|	LParens exp RParens    /* parenthesized exp */
	|	exp unit_name          /* exp followed by unit */
;

/* A parenthesized optional parameter list */
f_parms_list_0:
		LParens f_parms_0 RParens
;

/* Zero or more parameters */
f_parms_0:
		f_parms
	|	/* nothing */
;

/* One or more parameters */
f_parms:
		f_parms Comma exp
	|	exp
;

unit_name: Identifier ;
