%{

#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"

void yyerror(const YYLTYPE * location, const char * message) {}

%}

%define api.pure full
%define api.push-pull push
%define api.value.union.name SemanticValue
%define parse.error detailed
%locations

%union {
	signed int integer;
	TokenLabel token;
	char* string;

	Constant * constant;
	Expression * expression;
	Factor * factor;
	Program * program;
	Condition * condition;
}

%destructor { destroyConstant($$); } <constant>
%destructor { destroyExpression($$); } <expression>
%destructor { destroyFactor($$); } <factor>

%token <integer> INTEGER
%token <token> ADD
%token <token> CLOSE_BRACE
%token <token> CLOSE_COMMENT
%token <token> CLOSE_PARENTHESIS
%token <token> DIV
%token <token> MUL
%token <token> OPEN_BRACE
%token <token> OPEN_COMMENT
%token <token> OPEN_PARENTHESIS
%token <token> SUB

%token <token> LESS_THAN
%token <token> LESS_EQUAL
%token <token> GREATER_THAN
%token <token> GREATER_EQUAL
%token <token> NOT_EQUAL
%token <token> EQUALITY

%token <token> IGNORED
%token <token> UNKNOWN

%token <token> BOARD
%token <token> CELL
%token <token> LOOP
%token <token> GRAPH
%token <token> PLAYER
%token <token> DICE
%token <token> SIMULATE
%token <token> PRINT
%token <token> LOG
%token <token> INT
%token <token> STRING
%token <token> BOOL
%token <token> IF
%token <token> ELSE
%token <token> THEN
%token <token> FOR
%token <token> WHILE
%token <token> IN
%token <token> TURNS
%token <token> SIDES
%token <token> COST
%token <token> RENT
%token <token> MONEY
%token <token> POSITION
%token <token> TO
%token <token> STRATEGY
%token <token> RANDOM
%token <token> AGGRESSIVE
%token <token> SEMICOLON
%token <token> COMMA
%token <token> EQUALS
%token <token> OPEN_BRACKET
%token <token> CLOSE_BRACKET
%token <string> IDENTIFIER
%token <string> STRING_LITERAL

%type <constant> constant
%type <expression> expression
%type <factor> factor
%type <program> program
%type <program> boardsim_program
%type <token> declaration
%type <token> board_decl
%type <token> cell_decl
%type <token> player_decl
%type <string> player_strategy
%type <token> dice_decl
%type <token> simulate_block
%type <token> statements
%type <token> statement
%type <token> variable_decl
%type <token> if_statement
%type <condition> condition
%type <condition> comparison_expression
%type <token> loop_statement
%type <token> while_statement

%left ADD SUB
%left MUL DIV
%nonassoc LESS_THAN LESS_EQUAL GREATER_THAN GREATER_EQUAL NOT_EQUAL EQUAL

%%

program: expression											{ $$ = ExpressionProgramSemanticAction($1); }
	| boardsim_program										{ $$ = $1; }
	;

boardsim_program: declaration								{ $$ = NULL; }
	| boardsim_program declaration							{ $$ = $1; }
	;

declaration: board_decl										{ $$ = $1; }
	| cell_decl												{ $$ = $1; }
	| player_decl											{ $$ = $1; }
	| dice_decl												{ $$ = $1; }
	| simulate_block										{ $$ = $1; }
	;

board_decl: BOARD IDENTIFIER LOOP INTEGER SEMICOLON			{ $$ = (TokenLabel)BoardDefSemanticAction($2, $3, $4); }
	| BOARD IDENTIFIER GRAPH SEMICOLON						{ $$ = (TokenLabel)BoardDefSemanticAction($2, $3, 0); }
	;

cell_decl: CELL INTEGER STRING_LITERAL SEMICOLON			{ $$ = (TokenLabel)CellDefSemanticAction($2, $3, 0); }
	| CELL INTEGER STRING_LITERAL COST INTEGER SEMICOLON	{ $$ = (TokenLabel)CellDefSemanticAction($2, $3, $5); }
	| CELL INTEGER STRING_LITERAL COST INTEGER RENT INTEGER SEMICOLON	{ $$ = (TokenLabel)CellDefSemanticAction($2, $3, $5); }
	;

player_decl: PLAYER INTEGER MONEY INTEGER POSITION INTEGER player_strategy SEMICOLON	{ $$ = (TokenLabel)PlayerDefSemanticAction($2, $4, $6, $7); }
	;

player_strategy: 											{ $$ = NULL; }
	| STRATEGY STRING_LITERAL								{ $$ = $2; }
	;

dice_decl: DICE INTEGER SIDES SEMICOLON						{ $$ = (TokenLabel)DiceDefSemanticAction($2); }
	;

simulate_block: SIMULATE INTEGER TURNS OPEN_BRACE CLOSE_BRACE				{ $$ = (TokenLabel)SimulateBlockSemanticAction($2); }
	| SIMULATE INTEGER TURNS OPEN_BRACE statements CLOSE_BRACE				{ $$ = (TokenLabel)SimulateBlockSemanticAction($2); }
	;

statements: statement										{ $$ = $1; }
	| statements statement									{ $$ = $2; }
	;

statement: PRINT STRING_LITERAL SEMICOLON					{ $$ = (TokenLabel)PrintStatementSemanticAction($2); }
	| LOG STRING_LITERAL SEMICOLON							{ $$ = (TokenLabel)LogStatementSemanticAction($2); }
	| variable_decl											{ $$ = $1; }
	| if_statement											{ $$ = $1; }
	| loop_statement										{ $$ = $1; }
	| while_statement										{ $$ = $1; }
	;

if_statement: IF OPEN_PARENTHESIS condition CLOSE_PARENTHESIS THEN OPEN_BRACE statements CLOSE_BRACE	{ $$ = (TokenLabel)IfStatementSemanticAction($3, $7); }
	| IF OPEN_PARENTHESIS condition CLOSE_PARENTHESIS THEN OPEN_BRACE statements CLOSE_BRACE ELSE OPEN_BRACE statements CLOSE_BRACE	{ $$ = (TokenLabel)IfElseStatementSemanticAction($3, $7, $11); }
	;

condition: IDENTIFIER										{ $$ = IdentifierConditionSemanticAction($1); }
	| INTEGER												{ $$ = IntegerConditionSemanticAction($1); }
	| STRING_LITERAL										{ $$ = StringConditionSemanticAction($1); }
	| comparison_expression									{ $$ = $1; }
	;

comparison_expression: IDENTIFIER GREATER_THAN INTEGER		{ $$ = ComparisonExpressionSemanticAction($1, $3, GREATER_THAN); }
	| IDENTIFIER LESS_THAN INTEGER							{ $$ = ComparisonExpressionSemanticAction($1, $3, LESS_THAN); }
	| IDENTIFIER GREATER_EQUAL INTEGER						{ $$ = ComparisonExpressionSemanticAction($1, $3, GREATER_EQUAL); }
	| IDENTIFIER LESS_EQUAL INTEGER							{ $$ = ComparisonExpressionSemanticAction($1, $3, LESS_EQUAL); }
	| IDENTIFIER EQUALITY INTEGER							{ $$ = ComparisonExpressionSemanticAction($1, $3, EQUALITY); }
	| IDENTIFIER NOT_EQUAL INTEGER							{ $$ = ComparisonExpressionSemanticAction($1, $3, NOT_EQUAL); }
	;

loop_statement: FOR IDENTIFIER IN INTEGER TO INTEGER OPEN_BRACE statements CLOSE_BRACE	{ $$ = (TokenLabel)ForStatementSemanticAction($2, $4, $6, $8); }
	;

while_statement: WHILE OPEN_PARENTHESIS condition CLOSE_PARENTHESIS OPEN_BRACE statements CLOSE_BRACE	{ $$ = (TokenLabel)WhileStatementSemanticAction($3, $6); }
	;

variable_decl: INT IDENTIFIER EQUALS INTEGER SEMICOLON		{ $$ = (TokenLabel)IntVariableSemanticAction($2, $4); }
	| STRING IDENTIFIER EQUALS STRING_LITERAL SEMICOLON		{ $$ = (TokenLabel)StringVariableSemanticAction($2, $4); }
	| BOOL IDENTIFIER EQUALS IDENTIFIER SEMICOLON			{ $$ = (TokenLabel)BoolVariableSemanticAction($2, $4); }
	;

expression: expression[left] ADD expression[right]			{ $$ = ArithmeticExpressionSemanticAction($left, $right, ADDITION); }
	| expression[left] DIV expression[right]				{ $$ = ArithmeticExpressionSemanticAction($left, $right, DIVISION); }
	| expression[left] MUL expression[right]				{ $$ = ArithmeticExpressionSemanticAction($left, $right, MULTIPLICATION); }
	| expression[left] SUB expression[right]				{ $$ = ArithmeticExpressionSemanticAction($left, $right, SUBTRACTION); }
	| factor												{ $$ = FactorExpressionSemanticAction($1); }
	;

factor: OPEN_PARENTHESIS expression CLOSE_PARENTHESIS		{ $$ = ExpressionFactorSemanticAction($2); }
	| constant												{ $$ = ConstantFactorSemanticAction($1); }
	;

constant: INTEGER											{ $$ = IntegerConstantSemanticAction($1); }
	;

%%
