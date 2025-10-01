%{

#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"

/**
 * BoardSim Grammar (EBNF) based on PDF sections 6-8:
 * 
 * Program = {Declaration | Statement}*;
 * Declaration = BoardDecl | PieceDecl | PlayerDecl | EventDecl | DiceDecl | RuleDecl;
 * BoardDecl = 'board' ID ( 'loop' INT | 'graph' ) ';' {CellOrNodeDecl}*;
 * CellOrNodeDecl = ('cell' | 'node') INT STRING {Attribute}*;
 * Attribute = 'event' ID | 'cost' INT | 'rent' INT | 'continent' STRING | 'armies' INT | 'connected' '[' ID {',' ID}* ']';
 * Simulate = 'simulate' INT 'turns' { 'strategy' ID } '{' {Statement}* '}';
 * Statement = MoveStmt | ApplyStmt | IfStmt | ForStmt | SwitchStmt | LogStmt | PrintStmt | ExportStmt | Assignment;
 * MoveStmt = 'move' ID 'to' Expr 'if' Expr ';';
 * ApplyStmt = 'apply' ID 'if' Expr ';';
 * IfStmt = 'if' '(' Expr ')' '{' Statement* '}' ['else' '{' Statement* '}'];
 * ForStmt = 'for' '(' Assignment ';' Expr ';' Assignment ')' '{' Statement* '}';
 * SwitchStmt = 'switch' '(' Expr ')' '{' {CaseStmt}* [DefaultStmt] '}';
 * LogStmt = 'log' STRING ';' | 'log' Expr ';';
 * PrintStmt = 'print' STRING ';' | 'print' Expr ';';
 * ExportStmt = 'export' ID 'to' STRING ';';
 * Assignment = ID '=' Expr ';';
 * Expr = LogicalExpr;
 * LogicalExpr = RelationalExpr [('&&' | '||') LogicalExpr];
 * RelationalExpr = ArithmeticExpr [('==' | '!=' | '<' | '>' | '<=' | '>=') ArithmeticExpr];
 * ArithmeticExpr = Term [('+' | '-') ArithmeticExpr];
 * Term = Factor [('*' | '/' | '%') Term];
 * Factor = ID | INT | STRING | BOOL | '(' Expr ')' | FunctionCall;
 * FunctionCall = ID '(' [Expr {',' Expr}*] ')';
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

/** Non-terminals. */
%type <constant> constant
%type <expression> expression
%type <factor> factor
%type <program> program

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
