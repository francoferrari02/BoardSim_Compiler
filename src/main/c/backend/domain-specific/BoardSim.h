#ifndef BOARDSIM_HEADER
#define BOARDSIM_HEADER

/**
 * We reuse the types from the AST for convenience, but you should separate
 * the layers of the backend and frontend using another group of
 * domain-specific models or DTOs (Data Transfer Objects).
 */
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include <limits.h>
#include <stdbool.h>

/** Initialize module's internal state. */
ModuleDestructor initializeBoardSimModule();

/**
 * Types for BoardSim domain (Type enum defined in CompilerState.h)
 */

/**
 * Runtime structures for BoardSim simulation
 */
typedef struct Event Event;
typedef struct Cell Cell;

struct Event {
	char* name;
	char* trigger;
	// Function pointer for event action will be added in Paso 5
};

struct Cell {
	int index;
	char* name;
	int cost;
	int rent;
	Event* event;
	Cell* connected;  // for graph boards
	char* continent;
	int armies;
};

typedef struct {
	char* id;
	char* type;  // "loop" or "graph" 
	int size;
	Cell* cells;
} Board;

typedef struct {
	char* id;
	char* owns;
	int armies;
	// Add more piece properties as needed
} Piece;

typedef struct {
	int id;
	int money;
	int position;
	Piece* pieces;
	// Add resources map later
} Player;

typedef struct {
	int sides;
} Dice;

typedef struct {
	char* name;
	// Rule check/apply functions will be added in Paso 5
} Rule;

/**
 * The result of a computation. It's considered valid only if "succeed" is
 * true.
 */
typedef struct {
	bool succeeded;
	int value;
} ComputationResult;

typedef ComputationResult (*BinaryOperator)(const int, const int);

/** Arithmetic operations. */

ComputationResult add(const int leftAddend, const int rightAddend);
ComputationResult divide(const int dividend, const int divisor);
ComputationResult multiply(const int multiplicand, const int multiplier);
ComputationResult subtract(const int minuend, const int subtract);

/**
 * Computes the final value of a mathematical constant.
 */
ComputationResult computeConstant(Constant * constant);

/**
 * Computes the final value of a mathematical expression.
 */
ComputationResult computeExpression(Expression * expression);

/**
 * Computes the final value of a mathematical factor.
 */
ComputationResult computeFactor(Factor * factor);

/**
 * Computes the program value using the current compiler state.
 */
ComputationResult executeBoardSim(CompilerState * compilerState);

#endif
