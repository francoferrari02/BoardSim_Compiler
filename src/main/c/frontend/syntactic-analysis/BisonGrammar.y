%{

#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"

/**
 * BoardSim Grammar (EBNF) - IMPLEMENTED FEATURES ONLY:
 * 
 * Program = Expression | BoardSimProgram;
 * BoardSimProgram = {Declaration}*;
 * Declaration = BoardDecl | CellDecl | PlayerDecl | DiceDecl | SimulateBlock;
 * BoardDecl = 'board' ID ( 'loop' INT | 'graph' ) ';';
 * CellDecl = 'cell' INT STRING [ 'cost' INT [ 'rent' INT ] ] ';';
 * PlayerDecl = 'player' INT 'money' INT 'position' INT [ 'strategy' STRING ] ';';
 * DiceDecl = 'dice' INT 'sides' ';';
 * SimulateBlock = 'simulate' INT 'turns' '{' '}';
 * Expression = ArithmeticExpression;
 * ArithmeticExpression = Term [('+' | '-' | '*' | '/') ArithmeticExpression];
 * Term = Factor;
 * Factor = '(' Expression ')' | Constant;
 * Constant = INTEGER;
 */

/**
 * The error reporting function for Bison parser.
 *
 * @todo Add location to the grammar and "pushToken" API function.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Error-Reporting-Function.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Tracking-Locations.html
 */
void yyerror(const YYLTYPE * location, const char * message) {}

%}

// You touch this, and you die.
%define api.pure full
%define api.push-pull push
%define api.value.union.name SemanticValue
%define parse.error detailed
%locations

%union {
	/** Terminals. */

	signed int integer;
	TokenLabel token;

	/** Non-terminals. */

	Constant * constant;
	Expression * expression;
	Factor * factor;
	Program * program;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parsing succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { destroyConstant($$); } <constant>
%destructor { destroyExpression($$); } <expression>
%destructor { destroyFactor($$); } <factor>

/** Terminals. */
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

%token <token> IGNORED
%token <token> UNKNOWN

/** BoardSim tokens. */
%token <token> BOARD
%token <token> CELL  

%token <token> LOOP
%token <token> GRAPH
%token <token> PLAYER

%token <token> DICE
%token <token> SIMULATE

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
%token <token> IDENTIFIER
%token <token> STRING_LITERAL

/** Extended BoardSim tokens for complex features - TODO: uncomment when ready */
/* %token <token> CONTINENT */
/* %token <token> ARMIES */
/* %token <token> CONNECTED */
/* %token <token> OWNS */
/* %token <token> TERRITORIES */
/* %token <token> OBJECTIVE */
/* %token <token> RATING */
/* %token <token> PIECES */
%token <token> OPEN_BRACKET
%token <token> CLOSE_BRACKET
%token <token> EQUALS
/* %token <token> CONSERVATIVE */
/* %token <token> BALANCED */
/* %token <token> POSITIONAL */
/* %token <token> TACTICAL */

/** Non-terminals. */
%type <constant> constant
%type <expression> expression
%type <factor> factor
%type <program> program

/** BoardSim non-terminals. */
%type <program> boardsim_program
%type <token> board_decl
%type <token> cell_decl
%type <token> player_decl
%type <token> dice_decl
%type <token> simulate_block

/**
 * Precedence and associativity.
 *
 * @see https://en.cppreference.com/w/cpp/language/operator_precedence.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
%left ADD SUB
%left MUL DIV

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: expression											{ $$ = ExpressionProgramSemanticAction($1); }
	| boardsim_program										{ $$ = $1; }
	;

boardsim_program: board_decl								{ $$ = BoardSimProgramSemanticAction($1); }
	| cell_decl												{ $$ = BoardSimProgramSemanticAction($1); }
	| player_decl											{ $$ = BoardSimProgramSemanticAction($1); }
	| dice_decl												{ $$ = BoardSimProgramSemanticAction($1); }
	| simulate_block										{ $$ = BoardSimProgramSemanticAction($1); }
	| boardsim_program board_decl							{ $$ = $1; /* TODO: append declarations */ }
	| boardsim_program cell_decl							{ $$ = $1; /* TODO: append declarations */ }
	| boardsim_program player_decl							{ $$ = $1; /* TODO: append declarations */ }
	| boardsim_program dice_decl							{ $$ = $1; /* TODO: append declarations */ }
	| boardsim_program simulate_block						{ $$ = $1; /* TODO: append blocks */ }
	;

board_decl: BOARD IDENTIFIER LOOP INTEGER SEMICOLON		{ $$ = (TokenLabel)BoardDefSemanticAction($2, $3, $4); }
	| BOARD IDENTIFIER GRAPH SEMICOLON						{ $$ = (TokenLabel)BoardDefSemanticAction($2, $3, 0); }
	;

cell_decl: CELL INTEGER STRING_LITERAL SEMICOLON								{ $$ = (TokenLabel)CellDefSemanticAction($2, NULL, 0); }
	| CELL INTEGER STRING_LITERAL COST INTEGER SEMICOLON						{ $$ = (TokenLabel)CellDefSemanticAction($2, NULL, $5); }

	| CELL INTEGER STRING_LITERAL COST INTEGER RENT INTEGER SEMICOLON			{ $$ = (TokenLabel)CellDefSemanticAction($2, NULL, $5); }

	;





player_decl: PLAYER INTEGER MONEY INTEGER POSITION INTEGER SEMICOLON							{ $$ = (TokenLabel)PlayerDefSemanticAction($2, $4, $6); }
	| PLAYER INTEGER MONEY INTEGER POSITION INTEGER STRATEGY STRING_LITERAL SEMICOLON	{ $$ = (TokenLabel)PlayerDefSemanticAction($2, $4, $6); }
	;



dice_decl: DICE INTEGER SIDES SEMICOLON								{ $$ = (TokenLabel)DiceDefSemanticAction($2); }
	;

simulate_block: SIMULATE INTEGER TURNS OPEN_BRACE CLOSE_BRACE			{ $$ = (TokenLabel)SimulateBlockSemanticAction($2); }
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
