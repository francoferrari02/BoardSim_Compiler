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
 * Enum representing the different node types for generic linked list.
 */
typedef enum {
    NODE_TYPE_BOARD_DEF,
    NODE_TYPE_PIECE_DEF,
    NODE_TYPE_SIMULATE_BLOCK,
    NODE_TYPE_CELL_DEF,
    NODE_TYPE_PLAYER_DEF,
    NODE_TYPE_DICE_DEF,
    NODE_TYPE_RULE_DEF,
    NODE_TYPE_STATEMENT,
    NODE_TYPE_EXPRESSION
} NodeType;

/**
 * Additional BoardSim structures.
 */
struct CellDef {
	int index;
	char* name;
	int cost;     // Optional cost attribute
	int rent;     // Optional rent attribute
	char* event;  // Optional event reference
};

struct PlayerDef {
	int id;
	int money;
	int position;
};

struct DiceDef {
	int sides;
};

struct RuleDef {
	char* name;
};

/**
 * BoardSim Program structure that can hold multiple declarations.
 */
struct BoardSimProgram {
	struct ASTNode* declarations;  // Linked list of declarations
	struct ASTNode* statements;    // Linked list of statements from simulate blocks
};

/**
 * Typedefs for the new structures.
 */
typedef struct CellDef CellDef;
typedef struct PlayerDef PlayerDef;
typedef struct DiceDef DiceDef;
typedef struct RuleDef RuleDef;
typedef struct BoardSimProgram BoardSimProgram;

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

// Additional BoardSim node creators/destructors
CellDef* createCellDef(int index, char* name, int cost, int rent, char* event);
void destroyCellDef(CellDef* cellDef);

PlayerDef* createPlayerDef(int id, int money, int position);
void destroyPlayerDef(PlayerDef* playerDef);

DiceDef* createDiceDef(int sides);
void destroyDiceDef(DiceDef* diceDef);

RuleDef* createRuleDef(char* name);
void destroyRuleDef(RuleDef* ruleDef);

BoardSimProgram* createBoardSimProgram();
void destroyBoardSimProgram(BoardSimProgram* program);

#endif
