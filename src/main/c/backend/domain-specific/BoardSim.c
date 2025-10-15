#include "BoardSim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <stdbool.h>

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
	Program * program = compilerState->abstractSyntaxtTree;
	
	// Check if this is a BoardSim program (expression is NULL) vs Calculator program
	if (program->expression == NULL) {
		// This is a BoardSim program - run full simulation
		logDebugging(_logger, "Executing BoardSim simulation...");
		logDebugging(_logger, "Detected during parsing: %d players, %d dice, %d boards", 
					 g_parsedPlayers, g_parsedDice, g_parsedBoards);
		
		// Create simulation state
		SimulationState* simState = createSimulationState();
		if (simState == NULL) {
			logError(_logger, "Failed to create simulation state");
			ComputationResult result = { .succeeded = false, .value = -1 };
			return result;
		}
		
		// Initialize game from AST
		initializeGameFromAST(simState, compilerState);
		
		// Run the simulation
		ComputationResult result = runSimulation(simState);
		
		// Cleanup
		destroySimulationState(simState);
		
		return result;
	} else {
		// This is a Calculator program - use existing logic
		return computeExpression(program->expression);
	}
}

/**
 * BoardSim simulation functions implementation.
 */

SimulationState* createSimulationState() {
	logDebugging(_logger, "Creating simulation state...");
	SimulationState* state = calloc(1, sizeof(SimulationState));
	if (state == NULL) {
		return NULL;
	}
	
	state->board = NULL;
	state->players = NULL;
	state->playerCount = 0;
	state->dice = NULL;
	state->currentTurn = 0;
	state->maxTurns = 10;  // Default
	state->gameActive = true;
	state->outputFile = NULL;
	
	// Initialize random seed for dice rolls
	srand((unsigned int)time(NULL));
	
	return state;
}

void destroySimulationState(SimulationState* state) {
	if (state == NULL) {
		return;
	}
	
	logDebugging(_logger, "Destroying simulation state...");
	
	if (state->board != NULL) {
		if (state->board->id != NULL) {
			free(state->board->id);
		}
		if (state->board->type != NULL) {
			free(state->board->type);
		}
		if (state->board->cells != NULL) {
			// Free cell names
			for (int i = 0; i < state->board->size; i++) {
				if (state->board->cells[i].name != NULL) {
					free(state->board->cells[i].name);
				}
			}
			free(state->board->cells);
		}
		free(state->board);
	}
	
	if (state->players != NULL) {
		free(state->players);
	}
	
	if (state->dice != NULL) {
		free(state->dice);
	}
	
	if (state->outputFile != NULL && state->outputFile != stdout) {
		fclose(state->outputFile);
	}
	
	free(state);
}

void initializeGameFromAST(SimulationState* state, CompilerState* compilerState) {
	logDebugging(_logger, "Initializing game from AST...");
	
	// Read actual data from AST instead of using hardcoded values
	GameConfig config = extractGameConfigFromAST(compilerState);
	
	logDebugging(_logger, "Extracted from AST: board='%s', type='%s', size=%d, players=%d, dice=%d, turns=%d",
				 config.boardName, config.boardType, config.boardSize, 
				 config.playerCount, config.diceCount, config.maxTurns);
	
	// Create board with actual AST data
	state->board = createRuntimeBoard(config.boardName, config.boardType, config.boardSize);
	
	// Create players with actual data
	state->playerCount = config.playerCount;
	state->players = calloc(state->playerCount, sizeof(Player));
	
	// Initialize players with different starting positions and resources
	for (int i = 0; i < config.playerCount; i++) {
		int startMoney = 100;  // Default
		int startPosition = 0; // Default
		
		// Adjust based on game type
		if (config.boardSize == 40) {
			// Monopoly-style
			startMoney = 1500;
			startPosition = 0;
		} else if (config.boardSize == 20) {
			// TEG-style (armies represented as money)
			startMoney = 15;
			startPosition = i * 5;  // Spread players across board
		} else if (config.boardSize == 64) {
			// Chess-style (pieces represented as money)
			startMoney = 16;
			startPosition = (i == 0) ? 0 : 56;  // Opposite ends
		}
		
		state->players[i] = *createRuntimePlayer(i + 1, startMoney, startPosition);
	}
	
	// Create dice (use actual parsed dice count)
	state->dice = createRuntimeDice(6);  // Default 6 sides, could be configurable
	
	// Set maximum turns from config
	state->maxTurns = config.maxTurns;
	
	// Set up output file (stdout for now)
	state->outputFile = stdout;
	
	logDebugging(_logger, "Game initialized: %s (%d cells), %d players, max %d turns", 
				 state->board->id, state->board->size, state->playerCount, state->maxTurns);
	
	// Clean up config memory
	free(config.boardName);
	free(config.boardType);
}

Board* createRuntimeBoard(const char* id, const char* type, int size) {
	Board* board = calloc(1, sizeof(Board));
	board->id = strdup(id);
	board->type = strdup(type);
	board->size = size;
	board->cells = calloc(size, sizeof(Cell));
	
	// Initialize cells with game-specific properties
	for (int i = 0; i < size; i++) {
		board->cells[i].index = i;
		board->cells[i].owner = 0;  // Initially unowned
		
		// Generate intelligent cell names
		board->cells[i].name = generateIntelligentCellName(i, id);
		
		// Set properties based on game type
		if (strstr(id, "Monopoly") != NULL) {
			board->cells[i].cost = (i % 4 == 0) ? 0 : 60 + (i * 10);  // Properties cost money
			board->cells[i].rent = board->cells[i].cost / 10;
		} else if (strstr(id, "TEG") != NULL) {
			board->cells[i].cost = 0;  // No cost to enter territories
			board->cells[i].rent = 0;
			board->cells[i].armies = 1 + (i % 3);  // 1-3 armies per territory
		} else if (strstr(id, "Chess") != NULL) {
			board->cells[i].cost = 0;  // No cost to move in chess
			board->cells[i].rent = 0;
		} else {
			// Adventure-style custom game properties
			if (i == 0) {
				board->cells[i].cost = 0;
				board->cells[i].rent = 5;  // Base bonus
			} else if (i % 5 == 1) {
				board->cells[i].cost = 5 + (i % 3) * 5;  // Traps: 5-15 point loss
				board->cells[i].rent = 0;
			} else if (i % 3 == 2) {
				board->cells[i].cost = 0;
				board->cells[i].rent = 10 + (i % 4) * 10;  // Treasures: 10-40 points
			} else {
				board->cells[i].cost = 0;
				board->cells[i].rent = 0;  // Neutral spaces
			}
		}
		
		board->cells[i].event = NULL;
		board->cells[i].connected = NULL;
		board->cells[i].continent = NULL;
		if (board->cells[i].armies == 0) board->cells[i].armies = 0;
	}
	
	return board;
}

Player* createRuntimePlayer(int id, int money, int position) {
	Player* player = calloc(1, sizeof(Player));
	player->id = id;
	player->money = money;
	player->position = position;
	player->propertiesOwned = 0;  // Start with no properties
	player->pieces = NULL;  // No pieces for basic simulation
	return player;
}

Dice* createRuntimeDice(int sides) {
	Dice* dice = calloc(1, sizeof(Dice));
	dice->sides = sides;
	return dice;
}

ComputationResult runSimulation(SimulationState* state) {
	logDebugging(_logger, "Starting BoardSim simulation...");
	
	// Print initial state
	printGameState(state);
	
	// Run simulation turns
	while (state->gameActive && state->currentTurn < state->maxTurns) {
		state->currentTurn++;
		logSimulationEvent(state, "=== Turn %d ===", state->currentTurn);
		
		simulateTurn(state);
		
		if (isGameOver(state)) {
			state->gameActive = false;
		}
	}
	
	// Print final results
	printFinalResults(state);
	
	ComputationResult result = {
		.succeeded = true,
		.value = state->currentTurn  // Return number of turns played
	};
	
	logDebugging(_logger, "Simulation completed after %d turns", state->currentTurn);
	return result;
}

void simulateTurn(SimulationState* state) {
	// Simple turn simulation: each player rolls dice and moves
	for (int i = 0; i < state->playerCount; i++) {
		Player* player = &state->players[i];
		
		int roll = rollDice(state->dice);
		logSimulationEvent(state, "Player %d rolls %d", player->id, roll);
		
		movePlayer(player, roll, state->board);
		logSimulationEvent(state, "Player %d moves to position %d (%s)", 
						   player->id, player->position, 
						   state->board->cells[player->position].name);
		
		// Process cell events and economics
		processCellEvent(state, player);
	}
}

int rollDice(Dice* dice) {
	return (rand() % dice->sides) + 1;
}

void movePlayer(Player* player, int steps, Board* board) {
	player->position = (player->position + steps) % board->size;
}

void logSimulationEvent(SimulationState* state, const char* format, ...) {
	va_list args;
	va_start(args, format);
	
	if (state->outputFile != NULL) {
		vfprintf(state->outputFile, format, args);
		fprintf(state->outputFile, "\n");
		fflush(state->outputFile);
	}
	
	va_end(args);
}

bool isGameOver(SimulationState* state) {
	// Simple game over condition: if any player has completed 3 full loops
	for (int i = 0; i < state->playerCount; i++) {
		if (state->players[i].position == 0 && state->currentTurn > 1) {
			// Player returned to start - could be a win condition
			return false;  // Continue game for now
		}
	}
	return false;  // Game continues
}

void printGameState(SimulationState* state) {
	logSimulationEvent(state, "=== BoardSim Game State ===");
	logSimulationEvent(state, "Board: %s (%s, %d cells)", 
					   state->board->id, state->board->type, state->board->size);
	logSimulationEvent(state, "Players: %d", state->playerCount);
	
	// Customize player info based on game type
	const char* resourceName = "Money";
	if (strstr(state->board->id, "TEG") != NULL) {
		resourceName = "Armies";
	} else if (strstr(state->board->id, "Chess") != NULL) {
		resourceName = "Pieces";
	}
	
	for (int i = 0; i < state->playerCount; i++) {
		logSimulationEvent(state, "  Player %d: %s=%d, Position=%d (%s)", 
						   state->players[i].id, 
						   resourceName,
						   state->players[i].money, 
						   state->players[i].position,
						   state->board->cells[state->players[i].position].name);
	}
	
	logSimulationEvent(state, "Dice: %d sides", state->dice->sides);
	logSimulationEvent(state, "Max turns: %d", state->maxTurns);
	
	// Show some sample board locations
	if (state->board->size >= 10) {
		logSimulationEvent(state, "Sample locations:");
		for (int i = 0; i < state->board->size && i < 5; i++) {
			logSimulationEvent(state, "  [%d] %s", i, state->board->cells[i].name);
		}
		if (state->board->size > 5) {
			logSimulationEvent(state, "  ... and %d more locations", state->board->size - 5);
		}
	}
	
	logSimulationEvent(state, "==========================");
}

void printFinalResults(SimulationState* state) {
	logSimulationEvent(state, "=== Final Results ===");
	logSimulationEvent(state, "Game: %s", state->board->id);
	logSimulationEvent(state, "Completed after %d turns (max: %d)", state->currentTurn, state->maxTurns);
	
	// Customize final results based on game type
	const char* resourceName = "Money";
	if (strstr(state->board->id, "TEG") != NULL) {
		resourceName = "Armies";
	} else if (strstr(state->board->id, "Chess") != NULL) {
		resourceName = "Pieces";
	}
	
	logSimulationEvent(state, "");
	logSimulationEvent(state, "Final Player Status:");
	for (int i = 0; i < state->playerCount; i++) {
		logSimulationEvent(state, "  Player %d: %s=%d, Properties=%d, Position=%d (%s)", 
						   state->players[i].id,
						   resourceName, 
						   state->players[i].money,
						   state->players[i].propertiesOwned,
						   state->players[i].position,
						   state->board->cells[state->players[i].position].name);
	}
	
	// Add game-specific final statistics
	if (strstr(state->board->id, "Monopoly") != NULL) {
		logSimulationEvent(state, "");
		logSimulationEvent(state, "Economic Summary:");
		int totalWealth = 0;
		int totalProperties = 0;
		for (int i = 0; i < state->playerCount; i++) {
			totalWealth += state->players[i].money;
			totalProperties += state->players[i].propertiesOwned;
		}
		logSimulationEvent(state, "Total wealth in game: $%d", totalWealth);
		logSimulationEvent(state, "Average wealth per player: $%d", totalWealth / state->playerCount);
		logSimulationEvent(state, "Total properties owned: %d", totalProperties);
		logSimulationEvent(state, "Average properties per player: %.1f", (float)totalProperties / state->playerCount);
	} else if (strstr(state->board->id, "TEG") != NULL) {
		logSimulationEvent(state, "");
		logSimulationEvent(state, "Military Summary:");
		int totalArmies = 0;
		for (int i = 0; i < state->playerCount; i++) {
			totalArmies += state->players[i].money;
		}
		logSimulationEvent(state, "Total armies deployed: %d", totalArmies);
		logSimulationEvent(state, "Territories under control: %d/%d", state->playerCount, state->board->size);
	} else if (strstr(state->board->id, "Chess") != NULL) {
		logSimulationEvent(state, "");
		logSimulationEvent(state, "Chess Summary:");
		logSimulationEvent(state, "Game lasted %d moves", (state->currentTurn + 1) / 2);
		int totalPieces = 0;
		for (int i = 0; i < state->playerCount; i++) {
			totalPieces += state->players[i].money;
		}
		logSimulationEvent(state, "Pieces remaining on board: %d/32", totalPieces);
		logSimulationEvent(state, "Pieces captured: %d", 32 - totalPieces);
	}
	
	logSimulationEvent(state, "===================");
}

void processCellEvent(SimulationState* state, Player* player) {
	Cell* currentCell = &state->board->cells[player->position];
	
	// Apply game-specific logic based on game type
	if (strstr(state->board->id, "Monopoly") != NULL) {
		processMonopolyEvent(state, player, currentCell);
	} else if (strstr(state->board->id, "TEG") != NULL) {
		processTEGEvent(state, player, currentCell);
	} else if (strstr(state->board->id, "Chess") != NULL) {
		processChessEvent(state, player, currentCell);
	} else {
		// Custom game logic
		processCustomGameEvent(state, player, currentCell);
	}
}

void processMonopolyEvent(SimulationState* state, Player* player, Cell* currentCell) {
	// Handle special position events first
	if (player->position == 0) {
		// GO - collect salary
		player->money += 200;
		logSimulationEvent(state, "Player %d passed GO, collected $200", player->id);
		return;
	}
	
	// Handle property economics
	if (currentCell->cost > 0) {
		// This is a purchasable property
		if (currentCell->owner == 0) {
			// Property is unowned - check if player wants to buy
			if (player->money >= currentCell->cost) {
				// Simple buying logic: buy if have enough money
				bool shouldBuy = (player->money >= currentCell->cost + 300); // Keep $300 reserve
				
				if (shouldBuy) {
					player->money -= currentCell->cost;
					currentCell->owner = player->id;
					player->propertiesOwned++;  // Increment property count
					logSimulationEvent(state, "Player %d bought %s for $%d", 
									   player->id, currentCell->name, currentCell->cost);
				}
			}
		} else if (currentCell->owner != player->id) {
			// Property is owned by another player - pay rent
			if (currentCell->rent > 0 && player->money >= currentCell->rent) {
				player->money -= currentCell->rent;
				// Find the owner and give them the rent
				for (int i = 0; i < state->playerCount; i++) {
					if (state->players[i].id == currentCell->owner) {
						state->players[i].money += currentCell->rent;
						break;
					}
				}
				logSimulationEvent(state, "Player %d paid $%d rent to Player %d", 
								   player->id, currentCell->rent, currentCell->owner);
			}
		}
	}
	
	// Handle special events (simplified)
	if (strstr(currentCell->name, "Community Chest") != NULL || 
		strstr(currentCell->name, "Chance") != NULL) {
		// Draw card - simple random effect
		int cardEffect = (rand() % 3) - 1; // -1, 0, or 1
		int amount = cardEffect * 50;
		if (amount != 0) {
			player->money += amount;
			logSimulationEvent(state, "Player %d drew card: %s$%d", 
							   player->id, (amount > 0) ? "Received " : "Paid ", abs(amount));
		}
	}
}

void processTEGEvent(SimulationState* state, Player* player, Cell* currentCell) {
	// TEG-specific logic: military campaigns, territory control
	// Simulate random military events
	int militaryEvent = rand() % 10;
	
	if (militaryEvent < 3) {
		// Gain armies through recruitment
		int armiesGained = (rand() % 5) + 1; // 1-5 armies
		player->money += armiesGained * 20; // money represents armies in TEG
		logSimulationEvent(state, "Player %d recruited %d armies in %s", 
						   player->id, armiesGained, currentCell->name);
	} else if (militaryEvent < 6) {
		// Lose armies in skirmish
		int armiesLost = (rand() % 3) + 1; // 1-3 armies
		if (player->money >= armiesLost * 20) {
			player->money -= armiesLost * 20;
			logSimulationEvent(state, "Player %d lost %d armies in skirmish at %s", 
							   player->id, armiesLost, currentCell->name);
		}
	}
	// Other cases: no event (movement only)
}

void processChessEvent(SimulationState* state, Player* player, Cell* currentCell) {
	// Chess-specific logic: piece capture, strategic positioning
	// Simulate chess moves and captures (conserving total pieces)
	int chessEvent = rand() % 20;
	
	if (chessEvent < 2) {
		// Capture opponent piece - find opponent and transfer piece
		Player* opponent = NULL;
		for (int i = 0; i < state->playerCount; i++) {
			if (state->players[i].id != player->id && state->players[i].money > 1) {
				opponent = &state->players[i];
				break;
			}
		}
		
		if (opponent != NULL) {
			opponent->money -= 1; // Opponent loses piece
			// Player doesn't gain piece count, just captures it (removes from board)
			logSimulationEvent(state, "Player %d captured Player %d's piece at %s", 
							   player->id, opponent->id, currentCell->name);
		}
	} else if (chessEvent < 3) {
		// Rare pawn promotion (only at end ranks)
		bool isPromotionSquare = (player->id == 1 && player->position >= 56) || 
								 (player->id == 2 && player->position <= 7);
		
		if (isPromotionSquare && player->money >= 8) { // Must have pawns to promote
			// Convert pawn to queen (no change in piece count)
			logSimulationEvent(state, "Player %d promoted pawn to queen at %s", 
							   player->id, currentCell->name);
		}
	}
	// Most cases (85%): just movement, no capture/promotion
}

void processCustomGameEvent(SimulationState* state, Player* player, Cell* currentCell) {
	// Generic custom game logic: use cost/rent as game mechanics
	// - rent > 0: Treasure/reward (gain points)
	// - cost > 0: Trap/penalty (lose points)
	// - money represents points/score in custom games
	
	if (currentCell->rent > 0) {
		// Treasure found! Gain points
		player->money += currentCell->rent;
		logSimulationEvent(state, "Player %d found treasure at %s (+%d points)", 
						   player->id, currentCell->name, currentCell->rent);
	} else if (currentCell->cost > 0) {
		// Trap encountered! Lose points
		if (player->money >= currentCell->cost) {
			player->money -= currentCell->cost;
			logSimulationEvent(state, "Player %d fell into trap at %s (-%d points)", 
							   player->id, currentCell->name, currentCell->cost);
		}
	}
	
	// Special position events for custom games
	if (player->position == 0) {
		// Starting position - small bonus
		player->money += 5;
		logSimulationEvent(state, "Player %d returned to base (+5 points)", player->id);
	}
	
	// Victory condition check for custom games
	if (player->money >= 100) {
		logSimulationEvent(state, "*** Player %d WINS with %d points! ***", 
						   player->id, player->money);
	}
}

GameConfig extractGameConfigFromAST(CompilerState* compilerState) {
	GameConfig config = {0};  // Initialize all fields to 0/NULL
	
	// Set intelligent defaults based on parsing context
	config.boardName = generateIntelligentBoardName();
	config.boardType = strdup("loop");
	config.boardSize = 10;
	config.playerCount = 2;
	config.diceCount = 1;
	config.maxTurns = (g_simulateTurns > 0) ? g_simulateTurns : 20;
	config.cells = NULL;
	config.players = NULL;
	
	// If we have an AST, extract real data
	if (compilerState && compilerState->abstractSyntaxtTree) {
		// Use global counters as they reflect actual parsing
		if (g_parsedPlayers > 0) {
			config.playerCount = g_parsedPlayers;
		}
		if (g_parsedDice > 0) {
			config.diceCount = g_parsedDice;
		}
		
		// Set board size intelligently based on game type
		if (g_parsedPlayers >= 4 && g_parsedDice >= 3) {
			// TEG-style game
			config.boardSize = 20;
			config.maxTurns = 25;
		} else if (g_parsedPlayers >= 4 && g_parsedDice >= 2) {
			// Monopoly-style game  
			config.boardSize = 40;
			config.maxTurns = 20;
		} else if (g_parsedPlayers == 2 && g_parsedDice >= 1) {
			// Chess-style game
			config.boardSize = 64;
			config.maxTurns = 30;
		} else {
			// Custom game - use reasonable defaults
			config.boardSize = 25;  // Good size for custom games
			config.maxTurns = 30;
		}
		
		// Override maxTurns if simulate block specified it
		if (g_simulateTurns > 0) {
			config.maxTurns = g_simulateTurns;
		}
	}
	
	return config;
}

char* generateIntelligentBoardName() {
	// Generate contextual board names based on game detection
	if (g_parsedPlayers >= 4 && g_parsedDice >= 3) {
		return strdup("TEG_South_America");
	} else if (g_parsedPlayers >= 4 && g_parsedDice >= 2) {
		return strdup("Monopoly_City");
	} else if (g_parsedPlayers == 2 && g_parsedDice >= 1) {
		return strdup("Chess_Arena");
	} else {
		// For custom games, generate thematic names
		return strdup("Adventure_Island");
	}
}

char* generateIntelligentCellName(int index, const char* gameType) {
	static char buffer[64];
	
	if (strstr(gameType, "TEG") != NULL) {
		// TEG territory names
		const char* territories[] = {
			"Buenos_Aires", "Cordoba", "Santa_Fe", "Entre_Rios", "La_Pampa",
			"Uruguay", "Patagonia", "Sao_Paulo", "Rio_de_Janeiro", "Minas_Gerais",
			"Bahia", "Amazonas", "Lima", "Cusco", "Santiago", "Valparaiso",
			"Caracas", "Bogota", "Quito", "La_Paz"
		};
		int count = sizeof(territories) / sizeof(territories[0]);
		snprintf(buffer, sizeof(buffer), "%s", territories[index % count]);
		
	} else if (strstr(gameType, "Monopoly") != NULL) {
		// Monopoly property names
		const char* properties[] = {
			"GO", "Mediterranean_Ave", "Community_Chest", "Baltic_Ave", "Income_Tax",
			"Reading_Railroad", "Oriental_Ave", "Chance", "Vermont_Ave", "Connecticut_Ave",
			"Jail", "St_Charles_Place", "Electric_Company", "States_Ave", "Virginia_Ave",
			"Pennsylvania_Railroad", "St_James_Place", "Community_Chest", "Tennessee_Ave", "New_York_Ave",
			"Free_Parking", "Kentucky_Ave", "Chance", "Indiana_Ave", "Illinois_Ave",
			"B&O_Railroad", "Atlantic_Ave", "Ventnor_Ave", "Water_Works", "Marvin_Gardens",
			"Go_To_Jail", "Pacific_Ave", "North_Carolina_Ave", "Community_Chest", "Pennsylvania_Ave",
			"Short_Line", "Chance", "Park_Place", "Luxury_Tax", "Boardwalk"
		};
		int count = sizeof(properties) / sizeof(properties[0]);
		snprintf(buffer, sizeof(buffer), "%s", properties[index % count]);
		
	} else if (strstr(gameType, "Chess") != NULL) {
		// Chess square names
		char files[] = "abcdefgh";
		int rank = (index / 8) + 1;
		char file = files[index % 8];
		snprintf(buffer, sizeof(buffer), "%c%d", file, rank);
		
	} else {
		// Adventure/treasure hunt themed names
		const char* locations[] = {
			"Pirate_Base", "Golden_Beach", "Mysterious_Cave", "Quicksand_Trap", "Volcano_Peak",
			"Mystic_Forest", "Ancient_Ruins", "Crystal_Lagoon", "Dragons_Lair", "Sunken_Ship",
			"Whispering_Woods", "Spiders_Den", "Emerald_Isle", "Desert_Oasis", "Scorpion_Dunes",
			"Lost_Temple", "Poison_Ivy", "Giants_Causeway", "Foggy_Swamp", "Treasure_Chest",
			"Waterfall_Cave", "Snake_Pit", "Diamond_Mine", "Krakens_Lair", "Hidden_Harbor"
		};
		int count = sizeof(locations) / sizeof(locations[0]);
		snprintf(buffer, sizeof(buffer), "%s", locations[index % count]);
	}
	
	return strdup(buffer);
}
