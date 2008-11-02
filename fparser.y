/*

Priority levels:

10 func
9  units
8  ^        (note: right-associative)
7  ! -    (UNARY)
6  * / %
5  + -    (BINARY)
4  = < > != <= >=
3  &
2  |
1  ,

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

%%

exp:
		NumConst
	|	PreFunc1 LParens func_params_opt RParens
	|	PreFunc2 LParens func_params_opt RParens
	|	PreFunc3 LParens func_params_opt RParens
	|	Eval     LParens func_params_opt RParens
	|	If       LParens exp { xx } Comma exp { yy } Comma exp RParens
	|	Identifier LParens func_params_opt RParens   /* fcall/pcall */
	|	Identifier /* var / const */
	|	exp Plus exp
	|	exp Minus exp
	|	exp AndOp exp
	|	exp OrOp exp
	|	exp CompOp exp
	|	exp TimesMulModOp exp
	|	exp Pow exp     /* cPow */
	|	Minus exp  %prec Bang /* negation */
	|	LParens exp RParens /* parenthesized exp */
	|	Bang exp        /* cNot */
	|	exp Identifier  /* exp followed by unit */
;

func_params_opt:
		func_params
	|	/* nothing */
;
func_params:
		func_params Comma exp
	|	exp
;
