%header{
#include "fpoptimizer_grammar.hh"
%}

%{
#include "fpconfig.hh"
#include "fptypes.hh"

#include <cstdio>

using namespace FPoptimizer_Grammar;
%}

%name FPoptimizerGrammarParser
%pure_parser

%union {
    Grammar*       g;
    Rule*          r;
    FunctionType*  f;
    MatchedParams* p;
    ParamSpec*     a;
    
    double         num;
    std::string*   name;
    unsigned       index;
    OpcodeType     opcode;
}

%token <num>    NUMERIC_CONSTANT
%token <name>   PARAMETER_TOKEN
%token <index>  PLACEHOLDER_TOKEN
%token <index>  IMMED_TOKEN
%token <opcode> BUILTIN_FUNC_NAME
%token <opcode> OPCODE_NOTINV
%token <opcode> OPCODE_MAYBEINV
%token <opcode> GROUP_CONSTANT_OPERATOR
%token <opcode> UNARY_CONSTANT_OPERATOR
%token NEWLINE

%token SUBST_OP_COLON
%token SUBST_OP_ARROW

%type <g> grammar
%type <r> substitution
%type <f> function function_notinv function_maybeinv
%type <p> paramsmatchingspec param_maybeinv_list paramlist
%type <a> maybeinv_param param paramtoken

%%
    grammar:
      grammar substitution
      {
        $$ = $1;
        $$->AddRule(*$2);
        delete $2;
      }
    | grammar NEWLINE
      {
        $$ = $1;
      }
    | /* empty */
      {
        $$ = new Grammar;
      }
    ;

    substitution:
      function SUBST_OP_ARROW paramtoken NEWLINE
      /* Entire function is changed into the particular param */
      {
        $$ = new Rule(Rule::ProduceNewTree, *$1, $3);
        delete $1;
      }

    | function SUBST_OP_ARROW function NEWLINE
      /* Entire function changes, the paramlist is rewritten */
      /* NOTE: "p x -> o y"  is a shortcut for "p x -> (o y)"  */
      {
        $$ = new Rule(Rule::ProduceNewTree, *$1, new ParamSpec_Function($3));
        delete $1;
      }

    | function_maybeinv SUBST_OP_COLON  param_maybeinv_list NEWLINE
      /* The params provided are replaced with the new param_maybeinv_list */
      {
        $$ = new Rule(Rule::ReplaceParams, *$1, *$3);
        delete $1;
        delete $3;
      }

    | function_notinv   SUBST_OP_COLON  paramlist NEWLINE
      /* The params provided are replaced with the new param_maybeinv_list */
      {
        $$ = new Rule(Rule::ReplaceParams, *$1, *$3);
        delete $1;
        delete $3;
      }

    ;
    
    function:
       function_notinv
       { $$ = $1; }
    |  function_maybeinv
       { $$ = $1; }
    ;
 
    function_notinv:
       OPCODE_NOTINV paramsmatchingspec
       /* Match a function with opcode=opcode and the given way of matching params */
       {
         $$ = new FunctionType($1, *$2);
         delete $2;
       }
    ;

    function_maybeinv:
       OPCODE_MAYBEINV paramsmatchingspec
       /* Match a function with opcode=opcode and the given way of matching params */
       {
         $$ = new FunctionType($1, *$2);
         delete $2;
       }
    ;

    paramsmatchingspec:
       '[' param_maybeinv_list ']'  /* match this exact paramlist */
        {
          $$ = $2;
          $$->SetType(MatchedParams::PositionalParams);
        }
     |  param_maybeinv_list         /* find the specified params */
        {
          $$ = $1;
          $$->SetType(MatchedParams::AnyParams);
        }
    ;
    
    param_maybeinv_list: /* left-recursive list of 0-n params with no delimiter */
        param_maybeinv_list maybeinv_param
        {
          $$ = $1;
          $$->AddParam($2);
        }
      | /* empty */
        {
          $$ = new MatchedParams;
        }
    ;
    maybeinv_param:
       '~' param    /* negated/inverted param (negations&inversions only exist with cMul and cAdd) */
       {
         $$ = $2;
         $$->Negated = true;
       }
     | param        /* non-negated/non-inverted param */
       {
         $$ = $1;
       }
    ;

    paramlist: /* left-recursive list of 0-n params with no delimiter */
        paramlist param
        {
          $$ = $1;
          $$->AddParam($2);
        }
      | /* empty */
        {
          $$ = new MatchedParams;
        }
    ;

    param:
       paramtoken
       {
         $$ = $1;
       }
    |  paramtoken '+'  /* In matching, matches TWO or more identical repetitions of namedparam */
                       /* In substitution, yields an immed containing the number of repetitions */
       {
         $$ = $1;
         $$->MinimumRepeat = 2;
         $$->AnyRepetition = true;
       }
    |  paramtoken '*'  /* In matching, matches ONE or more identical repetitions of namedparam */
                       /* In substitution, yields an immed containing the number of repetitions */
       {
         $$ = $1;
         $$->MinimumRepeat = 1;
         $$->AnyRepetition = true;
       }
    ;

    paramtoken:
       NUMERIC_CONSTANT         /* particular immed */
       {
         $$ = new ParamSpec_NumConstant($1);
       }
    |  IMMED_TOKEN              /* a placeholder for some immed */
       {
         $$ = new ParamSpec_ImmedHolder($1);
       }
    |  PLACEHOLDER_TOKEN        /* a placeholder for all params */
       {
         $$ = new ParamSpec_RestHolder($1);
       }
    |  '(' function ')'         /* a subtree */
       {
         $$ = new ParamSpec_Function($2);
       }
    |  GROUP_CONSTANT_OPERATOR '(' paramlist ')'    /* the literal sum/product/minimum/maximum of the provided immed-type params */
       {
         $$ = new ParamSpec_GroupFunction($1, $3->GetParams());
         delete $3;
       }
    |  UNARY_CONSTANT_OPERATOR paramtoken           /* the negated/inverted literal value of the paramtoken */
       {
         $$ = $2;
         using namespace FUNCTIONPARSERTYPES;
         switch($1)
         {
           case cNeg: $$->Transformation = ParamSpec::Negate; break;
           case cInv: $$->Transformation = ParamSpec::Invert; break;
         }
       }
    |  BUILTIN_FUNC_NAME '(' paramlist ')'  /* literal logarithm/sin/etc. of the provided immed-type params */
       {
         $$ = new ParamSpec_GroupFunction($1, $3->GetParams());
         delete $3;
       }
    |  PARAMETER_TOKEN          /* any expression */
       {
         $$ = new ParamSpec_NamedHolder(*$1);
         delete $1;
       }
    ;

%%


void FPoptimizerGrammarParser::yyerror(char*)
{
}

int FPoptimizerGrammarParser::yylex(yy_FPoptimizerGrammarParser_stype* lval)
{
    int c = std::fgetc(stdin);
    switch(c)
    {
    	case '#':
            while(c != EOF && c != '\n') c = std::fgetc(stdin);
            return NEWLINE;
        case '~':
        case '[':
        case ']':
        case '+':
        case '*':
        case '(':
        case ')':
            return c;
        case ' ':
        case '\t':
        case '\v':
        case '\r':
            return yylex(lval); // Counts as tail recursion, I hope
        case ':':
            return SUBST_OP_COLON;
        case '-':
        {
            int c2 = std::fgetc(stdin);
            if(c2 == '>')
                return SUBST_OP_ARROW;
            std::ungetc(c2, stdin);
            return c;
        }
        case '#': { lval->index = 0; return IMMED_TOKEN; }
        case '&': { lval->index = 1; return IMMED_TOKEN; }
        case '<':
        {
            lval->index  = 0;
            for(;;)
            {
                c = std::fgetc(stdin);
                if(c < '0' || c > '9') { std::ungetc(c, stdin); break; }
                lval->index = lval->index * 10 + (c-'0');
            }
            c = std::fgetc(stdin);
            if(c != '>') std::ungetc(c, stdin);
            return PLACEHOLDER_TOKEN;
        }
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        {
            std::string NumBuf;
            NumBuf += (char)c;
            bool had_comma = false;
            for(;;)
            {
                c = std::fgetc(stdin);
                if(c >= '0' && c <= '9')  { NumBuf += (char)c; continue; }
                if(c == '.' && !had_comma){ had_comma = true; NumBuf += (char)c; continue; }
                std::ungetc(c, stdin);
                break;
            }
            lval->num = strtod(NumBuf.c_str(), 0);
            return NUMERIC_CONSTANT;
        }
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
        case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
        case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
        case 'Y': case 'Z': case '_':
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
        case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
        case 's': case 't': case 'u': case 'v': case 'w': case 'x':
        case 'y': case 'z':
        {
            std::string IdBuf;
            IdBuf += (char)c;
            for(;;)
            {
                c = std::fgetc(stdin);
                if((c >= '0' && c <= '9')
                || (c >= 'a' && c <= 'z')
                || (c >= 'A' && c <= 'Z')) { IdBuf += (char)c; continue; }
                std::ungetc(c, stdin);
                break;
            }
            /* TODO: figure out if this is a named constant,
                     an opcode, or a parse-time function name,
                     or just an identifier
             */
        }
    }
    return EOF;
}

int main()
{
    FPoptimizerGrammarParser x;
    x.yyparse();
}
