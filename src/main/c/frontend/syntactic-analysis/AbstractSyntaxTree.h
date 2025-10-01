#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeAbstractSyntaxTreeModule();

/**
 * This type definitions allows self-referencing types (e.g., an expression
 * that is made of another expressions, such as talking about you in 3rd
 * person, but without the madness).
 */

typedef enum ExpressionType ExpressionType;
typedef enum FactorType FactorType;

typedef struct Constant Constant;
typedef struct Expression Expression;
typedef struct Factor Factor;
typedef struct Program Program;

typedef struct BoardDef BoardDef;
typedef struct PieceDef PieceDef;
typedef struct SimulateBlock SimulateBlock;
typedef struct ASTNode ASTNode;

/**
 * Generic AST node for linked lists
 */
struct ASTNode {
	void* data;           // Points to specific node type (BoardDef, PieceDef, etc.)
	int nodeType;         // Identifies the type of node
	struct ASTNode* next; // For linked lists
};

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

enum ExpressionType {
	ADDITION,
	DIVISION,
	FACTOR,
	MULTIPLICATION,
	SUBTRACTION
};

enum FactorType {
	CONSTANT,
	EXPRESSION
};

struct Constant {
	int value;
};

struct Factor {
	union {
		Constant * constant;
		Expression * expression;
	};
	FactorType type;
};

struct Expression {
	union {
		Factor * factor;
		struct {
			Expression * leftExpression;
			Expression * rightExpression;
		};
	};
	ExpressionType type;
};

struct Program {
	Expression * expression;
};

struct BoardDef {
	char* id;
	char* type;  // "loop" or "graph"
	int size;
	struct ASTNode* cells;  // Linked list of cell declarations
};

struct PieceDef {
	char* id;
	char* owns;
	int armies;
	struct ASTNode* properties;  // Additional properties
};

struct SimulateBlock {
	int turns;
	char* strategy;  // "random", "aggressive", etc.
	struct ASTNode* statements;  // Linked list of statements
};

/**
 * Node recursive super-duper-trambolik-destructors.
 */

void destroyConstant(Constant * constant);
void destroyExpression(Expression * expression);
void destroyFactor(Factor * factor);
void destroyProgram(Program * program);

BoardDef* createBoardDef(char* id, char* type, int size);
void destroyBoardDef(BoardDef* boardDef);

PieceDef* createPieceDef(char* id, char* owns, int armies);
void destroyPieceDef(PieceDef* pieceDef);

SimulateBlock* createSimulateBlock(int turns, char* strategy);
void destroySimulateBlock(SimulateBlock* simulateBlock);

ASTNode* createASTNode(void* data, int nodeType);
void destroyASTNode(ASTNode* node);

#endif
