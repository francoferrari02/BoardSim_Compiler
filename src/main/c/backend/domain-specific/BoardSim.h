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
	int owner;   // Player ID who owns this cell (0 = unowned)
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
	int propertiesOwned;  // Count of properties owned by this player
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
 * BoardSim simulation state - holds all runtime game data.
 */
typedef struct {
	Board* board;
	Player* players;
	int playerCount;
	Dice* dice;
	int currentTurn;
	int maxTurns;
	bool gameActive;
	FILE* outputFile;  // For logging simulation results
} SimulationState;

typedef struct {
	char* boardName;
	char* boardType;
	int boardSize;
	int playerCount;
	int diceCount;
	int maxTurns;
	ASTNode* cells;    // List of cell definitions from AST
	ASTNode* players;  // List of player definitions from AST
} GameConfig;

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

/**
 * Global counters for parsed entities (simple detection).
 */
extern int g_parsedPlayers;
extern int g_parsedDice;
extern int g_parsedBoards;
extern int g_simulateTurns;

/**
 * BoardSim simulation functions.
 */

// Simulation state management
SimulationState* createSimulationState();
void destroySimulationState(SimulationState* state);
void initializeGameFromAST(SimulationState* state, CompilerState* compilerState);
GameConfig extractGameConfigFromAST(CompilerState* compilerState);
char* generateIntelligentBoardName();
char* generateIntelligentCellName(int index, const char* gameType);

// Game entity creation
Board* createRuntimeBoard(const char* id, const char* type, int size);
Player* createRuntimePlayer(int id, int money, int position);
Dice* createRuntimeDice(int sides);

// Simulation execution
ComputationResult runSimulation(SimulationState* state, CompilerState* compilerState);
void simulateTurn(SimulationState* state, CompilerState* compilerState);
void processCellEvent(SimulationState* state, Player* player);
void logSimulationEvent(SimulationState* state, const char* format, ...);

// Game logic helpers
bool isGameOver(SimulationState* state);
void printGameState(SimulationState* state);
void printFinalResults(SimulationState* state);

// Statement execution
void executeStatementsFromAST(SimulationState* state, CompilerState* compilerState);

#endif