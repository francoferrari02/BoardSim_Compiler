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

// Game types for different board game mechanics
typedef enum {
    GAME_TYPE_MONOPOLY,    // Money-based, dice movement, costs/rents
    GAME_TYPE_CHESS,       // Piece-based, strategic movement, no money/dice
    GAME_TYPE_ADVENTURE,   // Resource-based, exploration, events
    GAME_TYPE_GRAPH,       // Graph-based, network navigation, connections
    GAME_TYPE_GENERIC      // Default fallback
} GameType;

/**
 * Runtime structures for BoardSim simulation
 */
typedef struct Event Event;
typedef struct Cell Cell;

struct Event {
	char* name;
	char* trigger;
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
} Piece;

typedef struct {
	int id;
	int money;
	int position;
	int propertiesOwned;  // Count of properties owned by this player
	int captures;         // Count of pieces captured (for Chess)
	char* strategy;       // Player strategy
	Piece* pieces;
} Player;

typedef struct {
	int sides;
} Dice;

typedef struct {
	char* name;
} Rule;

/**
 * Simple variable storage for user-declared variables.
 */
typedef struct {
	char* name;
	int intValue;
	char* stringValue;
	bool boolValue;
	int type;  // 0=int, 1=string, 2=bool
} RuntimeVariable;

#define MAX_RUNTIME_VARIABLES 64

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
	RuntimeVariable variables[MAX_RUNTIME_VARIABLES];
	int variableCount;
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
Cell* getCellAtPosition(Board* board, int position);

// Simulation execution
ComputationResult runSimulation(SimulationState* state, CompilerState* compilerState);
void simulateTurn(SimulationState* state, CompilerState* compilerState);
void processCellEvent(SimulationState* state, Player* player);
int rollDice(Dice* dice);
void logSimulationEvent(SimulationState* state, const char* format, ...);

// Game logic helpers
bool isGameOver(SimulationState* state);
void printGameState(SimulationState* state);
void printFinalResults(SimulationState* state);

// Game type detection
GameType detectGameType(GameConfig config);
void simulateTurnByGameType(SimulationState* state, CompilerState* compilerState, GameType gameType);
void simulateMonopolyTurn(SimulationState* state, CompilerState* compilerState);
void simulateChessTurn(SimulationState* state, CompilerState* compilerState);
void simulateAdventureTurn(SimulationState* state, CompilerState* compilerState);
void simulateGraphTurn(SimulationState* state, CompilerState* compilerState);

// Chess utility functions
char* getChessCoordinate(int position);

// Statement execution
void executeStatementsFromAST(SimulationState* state, CompilerState* compilerState);

#endif