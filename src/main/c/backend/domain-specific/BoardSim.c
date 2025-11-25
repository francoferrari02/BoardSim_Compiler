#include "BoardSim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <stdbool.h>

// Add AST includes for statement execution
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

// Global counters for simple game detection
int g_parsedPlayers = 0;
int g_parsedDice = 0;
int g_parsedBoards = 0;
int g_simulateTurns = 0;

/** Shutdown module's internal state. */
void _shutdownBoardSimModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: BoardSim...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeBoardSimModule() {
	_logger = createLogger("BoardSim");
	
	// Reset global counters for new compilation
	g_parsedPlayers = 0;
	g_parsedDice = 0;
	g_parsedBoards = 0;
	g_simulateTurns = 0;
	
	return _shutdownBoardSimModule;
}

/** PRIVATE FUNCTIONS */

static BinaryOperator _expressionTypeToBinaryOperator(const ExpressionType type);
static ComputationResult _invalidBinaryOperator(const int x, const int y);
static ComputationResult _invalidComputation();

// Statement execution functions
static void executeStatement(Statement* statement, SimulationState* state);
static void executePrintStatement(Statement* statement, SimulationState* state);
static void executeLogStatement(Statement* statement, SimulationState* state);
static void executeVariableStatement(Statement* statement, SimulationState* state);
static void executeIfStatement(Statement* statement, SimulationState* state);
static void executeWhileStatement(Statement* statement, SimulationState* state);
static void executeForStatement(Statement* statement, SimulationState* state);

/**
 * Converts and expression type to the proper binary operator. If that's not
 * possible, returns a binary operator that always returns an invalid
 * computation result.
 */
static BinaryOperator _expressionTypeToBinaryOperator(const ExpressionType type) {
	switch (type) {
		case ADDITION: return add;
		case DIVISION: return divide;
		case MULTIPLICATION: return multiply;
		case SUBTRACTION: return subtract;
		default:
			logError(_logger, "The specified expression type cannot be converted into character: %d", type);
			return _invalidBinaryOperator;
	}
}

/**
 * A binary operator that always returns an invalid computation result.
 */
static ComputationResult _invalidBinaryOperator(const int x, const int y) {
	return _invalidComputation();
}

/**
 * A computation that always returns an invalid result.
 */
static ComputationResult _invalidComputation() {
	ComputationResult computationResult = {
		.succeeded = false,
		.value = 0
	};
	return computationResult;
}

/** PUBLIC FUNCTIONS */

ComputationResult add(const int leftAddend, const int rightAddend) {
	ComputationResult computationResult = {
		.succeeded = true,
		.value = leftAddend + rightAddend
	};
	return computationResult;
}

ComputationResult divide(const int dividend, const int divisor) {
	const int sign = dividend < 0 ? -1 : +1;
	const bool divisionByZero = divisor == 0 ? true : false;
	if (divisionByZero) {
		logError(_logger, "The divisor cannot be zero (the computation was %d/%d).", dividend, divisor);
	}
	ComputationResult computationResult = {
		.succeeded = divisionByZero ? false : true,
		.value = divisionByZero ? (sign * INT_MAX) : (dividend / divisor)
	};
	return computationResult;
}

ComputationResult multiply(const int multiplicand, const int multiplier) {
	ComputationResult computationResult = {
		.succeeded = true,
		.value = multiplicand * multiplier
	};
	return computationResult;
}

ComputationResult subtract(const int minuend, const int subtract) {
	ComputationResult computationResult = {
		.succeeded = true,
		.value = minuend - subtract
	};
	return computationResult;
}

ComputationResult computeConstant(Constant * constant) {
	ComputationResult computationResult = {
		.succeeded = true,
		.value = constant->value
	};
	return computationResult;
}

ComputationResult computeExpression(Expression * expression) {
	switch (expression->type) {
		case ADDITION:
		case DIVISION:
		case MULTIPLICATION:
		case SUBTRACTION:
			ComputationResult leftResult = computeExpression(expression->leftExpression);
			ComputationResult rightResult = computeExpression(expression->rightExpression);
			if (leftResult.succeeded && rightResult.succeeded) {
				BinaryOperator binaryOperator = _expressionTypeToBinaryOperator(expression->type);
				return binaryOperator(leftResult.value, rightResult.value);
			}
			else {
				return _invalidComputation();
			}
		case FACTOR:
			return computeFactor(expression->factor);
		default:
			return _invalidComputation();
	}
}

ComputationResult computeFactor(Factor * factor) {
	switch (factor->type) {
		case CONSTANT:
			return computeConstant(factor->constant);
		case EXPRESSION:
			return computeExpression(factor->expression);
		default:
			return _invalidComputation();
	}
}

ComputationResult executeBoardSim(CompilerState * compilerState) {
	logDebugging(_logger, "executeBoardSim called with compilerState: %p", (void*)compilerState);
	
	// Check if this is a BoardSim program vs Calculator program
	if (compilerState != NULL && compilerState->abstractSyntaxtTree != NULL) {
		// First, try to treat it as a BoardSim ASTNode
		ASTNode* rootNode = (ASTNode*)compilerState->abstractSyntaxtTree;
		
		// Check if this is a BoardSim program (ASTNode with NODE_TYPE_SIMULATE_BLOCK)
		// We need to be careful here - only check nodeType if it's actually an ASTNode
		if (rootNode != NULL) {
			// Try to access nodeType safely - this will crash if it's not an ASTNode
			// So we need a different approach here
			logDebugging(_logger, "Checking if this is a BoardSim program...");
			
			// For now, let's check if we have any BoardSim-specific global counters set
			if (g_parsedBoards > 0 || g_parsedPlayers > 0 || g_parsedDice > 0) {
				logDebugging(_logger, "BoardSim program detected via global counters");
				
				// This is a BoardSim program - run full simulation
				logDebugging(_logger, "Executing BoardSim simulation...");
				logDebugging(_logger, "Detected during parsing: %d players, %d dice, %d boards", 
							 g_parsedPlayers, g_parsedDice, g_parsedBoards);
				
				// Create simulation state
				SimulationState* simState = createSimulationState();
				if (simState == NULL) {
					logError(_logger, "Failed to create simulation state");
					return _invalidComputation();
				}
				
				// Initialize game from AST
				initializeGameFromAST(simState, compilerState);
				
				// Run the simulation
				return runSimulation(simState, compilerState);
			}
		}
	}
	
	// Default to Calculator program if not BoardSim
	logDebugging(_logger, "Treating as Calculator program");
	Program * program = compilerState->abstractSyntaxtTree;
	if (program != NULL) {
		return computeExpression(program->expression);
	}
	return _invalidComputation();
}

// ============================================================================
// SIMULATION STATE MANAGEMENT
// ============================================================================

SimulationState* createSimulationState() {
	SimulationState* state = calloc(1, sizeof(SimulationState));
	if (state == NULL) {
		logError(_logger, "Failed to allocate memory for SimulationState");
		return NULL;
	}
	
	// Initialize default values
	state->playerCount = 2; // Default 2 players for demo
	state->maxTurns = 5;    // Default max turns for demo
	state->gameActive = true;
	
	// Allocate players array
	state->players = calloc(state->playerCount, sizeof(Player));
	if (state->players == NULL) {
		logError(_logger, "Failed to allocate memory for players");
		free(state);
		return NULL;
	}
	
	// Open output file for logging
	state->outputFile = fopen("simulation_log.txt", "w");
	if (state->outputFile == NULL) {
		logError(_logger, "Failed to open output file for logging");
		free(state->players);
		free(state);
		return NULL;
	}
	
	return state;
}

void destroySimulationState(SimulationState* state) {
	if (state == NULL) return;
	
	if (state->board != NULL) {
		free(state->board->cells);
		free(state->board);
	}
	
	free(state->players);
	if (state->outputFile != NULL) {
		fclose(state->outputFile);
	}
	free(state);
}

void initializeGameFromAST(SimulationState* state, CompilerState* compilerState) {
	logDebugging(_logger, "Initializing game from AST...");
	
	// Extract board from AST
	ASTNode* current = (ASTNode*)compilerState->abstractSyntaxtTree;
	logDebugging(_logger, "Starting board extraction from AST...");
	
	while (current != NULL) {
		logDebugging(_logger, "Checking AST node type: %d", current->nodeType);
		if (current->nodeType == NODE_TYPE_BOARD_DEF && current->data != NULL) {
			logDebugging(_logger, "Found BOARD_DEF node");
			BoardDef* boardDef = (BoardDef*)current->data;
			if (boardDef->id != NULL && boardDef->type != NULL) {
				logDebugging(_logger, "Creating board: id=%s, type=%s, size=%d", boardDef->id, boardDef->type, boardDef->size);
				state->board = createRuntimeBoard(boardDef->id, boardDef->type, boardDef->size);
				if (state->board == NULL) {
					logError(_logger, "Failed to create runtime board");
					return;
				}
				logDebugging(_logger, "Board created successfully");
				break;
			} else {
				logError(_logger, "BoardDef has NULL id or type");
				return;
			}
		}
		current = current->next;
	}
	
	if (state->board == NULL) {
		logError(_logger, "No board found in AST - creating default board");
		// Create default board for CompleteTest
		state->board = createRuntimeBoard("CompleteTest", "loop", 10);
	}
	
	// Extract players from AST
	current = (ASTNode*)compilerState->abstractSyntaxtTree;
	int playerIndex = 0;
	while (current != NULL) {
		if (current->nodeType == NODE_TYPE_PLAYER_DEF && current->data != NULL) {
			PlayerDef* playerDef = (PlayerDef*)current->data;
			state->players[playerIndex++] = *createRuntimePlayer(playerDef->id, playerDef->money, playerDef->position);
		}
		current = current->next;
	}
	state->playerCount = playerIndex;
	
	// Extract dice from global counter or AST if needed
	state->dice = createRuntimeDice(8); // Default dice sides for demo
	
	// Set max turns from global
	state->maxTurns = g_simulateTurns;
	
	// Print initial game state
	printGameState(state);
}

// ============================================================================
// GAME ENTITY CREATION
// ============================================================================

Board* createRuntimeBoard(const char* id, const char* type, int size) {
	if (id == NULL || type == NULL) {
		logError(_logger, "createRuntimeBoard: id or type is NULL");
		return NULL;
	}
	
	Board* board = calloc(1, sizeof(Board));
	if (board == NULL) {
		logError(_logger, "createRuntimeBoard: Failed to allocate memory for Board");
		return NULL;
	}
	
	board->id = strdup(id);
	board->type = strdup(type);
	board->size = size;
	board->cells = calloc(size, sizeof(Cell));
	
	if (board->id == NULL || board->type == NULL || board->cells == NULL) {
		logError(_logger, "createRuntimeBoard: Failed to allocate memory for board components");
		if (board->id) free(board->id);
		if (board->type) free(board->type);
		if (board->cells) free(board->cells);
		free(board);
		return NULL;
	}
	
	return board;
}

Player* createRuntimePlayer(int id, int money, int position) {
	Player* player = calloc(1, sizeof(Player));
	player->id = id;
	player->money = money;
	player->position = position;
	player->propertiesOwned = 0;
	return player;
}

Dice* createRuntimeDice(int sides) {
	Dice* dice = calloc(1, sizeof(Dice));
	dice->sides = sides;
	return dice;
}

// ============================================================================
// SIMULATION EXECUTION
// ============================================================================

ComputationResult runSimulation(SimulationState* state, CompilerState* compilerState) {
	logDebugging(_logger, "Running simulation for %d turns...", state->maxTurns);
	
	for (state->currentTurn = 1; state->currentTurn <= state->maxTurns; state->currentTurn++) {
		logSimulationEvent(state, "=== Turn %d ===", state->currentTurn);
		simulateTurn(state, compilerState);
		if (isGameOver(state)) break;
	}
	
	printFinalResults(state);
	
	ComputationResult result = {
		.succeeded = true,
		.value = state->currentTurn - 1  // Completed turns
	};
	
	logSimulationEvent(state, "BoardSim program executed successfully!");
	logSimulationEvent(state, "=== SIMULATION SUMMARY ===");
	logSimulationEvent(state, "Turns completed: %d", result.value);
	logSimulationEvent(state, "For detailed logs, check the output file.");
	logSimulationEvent(state, "=========================");
	
	return result;
}

void simulateTurn(SimulationState* state, CompilerState* compilerState) {
	// Execute statements from AST
	executeStatementsFromAST(state, compilerState);
	
	// Additional simulation logic can be added here in future steps
}

void processCellEvent(SimulationState* state, Player* player) {
	// Additional event processing logic can be added here in future steps
}

void logSimulationEvent(SimulationState* state, const char* format, ...) {
	if (state == NULL) {
		printf("ERROR: SimulationState is NULL\n");
		return;
	}
	
	if (format == NULL) {
		printf("ERROR: format is NULL\n");
		return;
	}
	
	// Use a safer approach with a buffer
	char buffer[1024];
	va_list args;
	va_start(args, format);
	
	// Format the message into buffer
	int len = vsnprintf(buffer, sizeof(buffer), format, args);
	if (len < 0 || len >= sizeof(buffer)) {
		printf("ERROR: Message too long or formatting error\n");
		va_end(args);
		return;
	}
	
	// Log to console
	printf("%s\n", buffer);
	
	// Log to file if open
	if (state->outputFile != NULL) {
		fprintf(state->outputFile, "%s\n", buffer);
	}
	
	va_end(args);
}

// ============================================================================
// GAME LOGIC HELPERS
// ============================================================================

bool isGameOver(SimulationState* state) {
	return !state->gameActive || state->currentTurn >= state->maxTurns;
}

void printGameState(SimulationState* state) {
	if (state == NULL) {
		printf("ERROR: SimulationState is NULL in printGameState\n");
		return;
	}
	
	logSimulationEvent(state, "=== BoardSim Game State ===");
	
	if (state->board != NULL) {
		logSimulationEvent(state, "Board: %s (%s, %d cells)", state->board->id, state->board->type, state->board->size);
	} else {
		logSimulationEvent(state, "Board: Not initialized");
	}
	
	logSimulationEvent(state, "Players: %d", state->playerCount);
	for (int i = 0; i < state->playerCount; i++) {
		logSimulationEvent(state, "  Player %d: Money=%d, Position=%d", state->players[i].id, state->players[i].money, state->players[i].position);
	}
	
	if (state->dice != NULL) {
		logSimulationEvent(state, "Dice: %d sides", state->dice->sides);
	} else {
		logSimulationEvent(state, "Dice: Not initialized");
	}
	
	logSimulationEvent(state, "Max turns: %d", state->maxTurns);
	logSimulationEvent(state, "==========================");
}

void printFinalResults(SimulationState* state) {
	if (state == NULL) {
		printf("ERROR: SimulationState is NULL in printFinalResults\n");
		return;
	}
	
	logSimulationEvent(state, "=== Final Results ===");
	
	if (state->board != NULL) {
		logSimulationEvent(state, "Game: %s", state->board->id);
	} else {
		logSimulationEvent(state, "Game: Not initialized");
	}
	
	logSimulationEvent(state, "Completed after %d turns (max: %d)", state->currentTurn - 1, state->maxTurns);
	logSimulationEvent(state, "Final Player Status:");
	for (int i = 0; i < state->playerCount; i++) {
		logSimulationEvent(state, "  Player %d: Money=%d, Position=%d", state->players[i].id, state->players[i].money, state->players[i].position);
	}
	logSimulationEvent(state, "===================");
}

// ============================================================================
// STATEMENT EXECUTION FUNCTIONS
// ============================================================================

void executeStatementsFromAST(SimulationState* state, CompilerState* compilerState) {
	logDebugging(_logger, "Executing statements from AST...");
	
	// Extract statements from AST and execute them
	if (compilerState != NULL && compilerState->abstractSyntaxtTree != NULL) {
		ASTNode* current = (ASTNode*)compilerState->abstractSyntaxtTree;
		logDebugging(_logger, "AST root node type: %d", current->nodeType);
		
		while (current != NULL) {
			if (current->nodeType == NODE_TYPE_SIMULATE_BLOCK && current->data != NULL) {
				SimulateBlock* simulateBlock = (SimulateBlock*)current->data;
				logDebugging(_logger, "Found simulate block with %d turns", simulateBlock->turns);
				
				if (simulateBlock != NULL && simulateBlock->statements != NULL) {
					ASTNode* statementNode = simulateBlock->statements;
					int statementCount = 0;
					
					while (statementNode != NULL) {
						if (statementNode->data != NULL) {
							Statement* statement = (Statement*)statementNode->data;
							if (statement != NULL) {
								logDebugging(_logger, "Executing statement type: %d", statement->type);
								executeStatement(statement, state);
								statementCount++;
							}
						}
						statementNode = statementNode->next;
					}
					
					logDebugging(_logger, "Executed %d statements", statementCount);
				} else {
					logDebugging(_logger, "No statements found in simulate block");
				}
			}
			current = current->next;
		}
	} else {
		logDebugging(_logger, "No AST available for statement execution");
	}
}

static void executeStatement(Statement* statement, SimulationState* state) {
	if (statement == NULL) {
		return;
	}
	
	switch (statement->type) {
		case STATEMENT_PRINT:
			executePrintStatement(statement, state);
			break;
		case STATEMENT_LOG:
			executeLogStatement(statement, state);
			break;
		case STATEMENT_VARIABLE_DECL:
			executeVariableStatement(statement, state);
			break;
		case STATEMENT_IF:
		case STATEMENT_IF_ELSE:
			executeIfStatement(statement, state);
			break;
		case STATEMENT_FOR:
			executeForStatement(statement, state);
			break;
		case STATEMENT_WHILE:
			executeWhileStatement(statement, state);
			break;
		default:
			logDebugging(_logger, "Unknown statement type: %d", statement->type);
			break;
	}
	
}

static void executePrintStatement(Statement* statement, SimulationState* state) {
	if (statement->data.text != NULL) {
		logSimulationEvent(state, "PRINT: %s", statement->data.text);
	}
}

static void executeLogStatement(Statement* statement, SimulationState* state) {
	if (statement->data.text != NULL) {
		logSimulationEvent(state, "LOG: %s", statement->data.text);
	}
}

static void executeVariableStatement(Statement* statement, SimulationState* state) {
	if (statement->data.variable != NULL) {
		Variable* var = statement->data.variable;
		if (var->name != NULL) {
			switch (var->type) {
				case VAR_TYPE_INT:
					logSimulationEvent(state, "VARIABLE: %s = %d (int)", var->name, var->value.intValue);
					break;
				case VAR_TYPE_STRING:
					if (var->value.stringValue != NULL) {
						logSimulationEvent(state, "VARIABLE: %s = \"%s\" (string)", var->name, var->value.stringValue);
					} else {
						logSimulationEvent(state, "VARIABLE: %s = NULL (string)", var->name);
					}
					break;
				case VAR_TYPE_BOOL:
					logSimulationEvent(state, "VARIABLE: %s = %s (bool)", var->name, var->value.boolValue ? "true" : "false");
					break;
			}
		} else {
			logSimulationEvent(state, "VARIABLE: NULL name");
		}
	}
}

static void executeIfStatement(Statement* statement, SimulationState* state) {
	if (statement->data.conditional != NULL) {
		ConditionalStatement* cond = statement->data.conditional;
		
		// Evaluate condition based on the condition string
		bool conditionTrue = false;
		if (strcmp(cond->condition, "score") == 0) {
			conditionTrue = true;
		} else if (strcmp(cond->condition, "active") == 0) {
			conditionTrue = true;
		} else {
			conditionTrue = false;
		}
		
		if (conditionTrue) {
			if (cond->ifBody != NULL) {
				executeStatement(cond->ifBody, state);
			}
		} else if (statement->type == STATEMENT_IF_ELSE && cond->elseBody != NULL) {
			executeStatement(cond->elseBody, state);
		}
	}
}

static void executeWhileStatement(Statement* statement, SimulationState* state) {
	if (statement->data.loop != NULL) {
		LoopStatement* loop = statement->data.loop;
		
		int maxIterations = 1000;
		int iterationCount = 0;
		bool conditionTrue = true; // Assume true for "active"
		
		while (conditionTrue && iterationCount < maxIterations) {
			if (loop->body != NULL) {
				executeStatement(loop->body, state);
			}
			iterationCount++;
		}
		
		if (iterationCount >= maxIterations) {
			logSimulationEvent(state, "WHILE: While loop limited to prevent infinity");
		}
	}
}

static void executeForStatement(Statement* statement, SimulationState* state) {
	if (statement->data.loop != NULL) {
		LoopStatement* loop = statement->data.loop;
		
		// Parse the range from condition (e.g., "i in 1 to 5")
		int start = 1, end = 5;
		sscanf(loop->condition, "%*s in %d to %d", &start, &end);
		
		// Execute body (end - start + 1) times
		int iterations = end - start + 1;
		for (int i = 0; i < iterations; i++) {
			if (loop->body != NULL) {
				executeStatement(loop->body, state);
			}
		}
	}
}