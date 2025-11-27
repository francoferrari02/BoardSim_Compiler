#include "BisonActions.h"
#include "../../backend/domain-specific/BoardSim.h"

/* MODULE INTERNAL STATE */

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

// Global variable to store pending statements
static Statement* g_pendingStatements[100];
static int g_pendingStatementCount = 0;

// Forward declarations for helper functions
static void storeStatementForLater(Statement* statement);
static void addPendingStatementsToSimulateBlock(SimulateBlock* simulateBlock);

/** Shutdown module's internal state. */
void _shutdownBisonActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: BisonActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	_compilerState = NULL;
}

// Helper function to store statements for later addition
static void storeStatementForLater(Statement* statement) {
	if (g_pendingStatementCount < 100) {
		g_pendingStatements[g_pendingStatementCount++] = statement;
		logDebugging(_logger, "Stored statement %d for later addition", g_pendingStatementCount);
	}
}

// Helper function to add all pending statements to simulate block
static void addPendingStatementsToSimulateBlock(SimulateBlock* simulateBlock) {
	logDebugging(_logger, "Adding %d pending statements to simulate block", g_pendingStatementCount);
	
	int addedCount = 0;
	for (int i = 0; i < g_pendingStatementCount; i++) {
		Statement* statement = g_pendingStatements[i];
		if (statement != NULL) {
			ASTNode* statementNode = createASTNode(statement, NODE_TYPE_STATEMENT);
			
			// Add to statements list
			if (simulateBlock->statements == NULL) {
				simulateBlock->statements = statementNode;
			} else {
				// Add to end of list
				ASTNode* current = simulateBlock->statements;
				while (current->next != NULL) {
					current = current->next;
				}
				current->next = statementNode;
			}
			addedCount++;
			logDebugging(_logger, "Added pending statement %d to simulate block", i + 1);
		}
	}
	
	// Clear pending statements
	g_pendingStatementCount = 0;
	
	logDebugging(_logger, "Successfully added %d statements to simulate block", addedCount);
}

ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState) {
	_compilerState = compilerState;
	_logger = createLogger("BisonActions");
	return _shutdownBisonActionsModule;
}

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */

Constant * IntegerConstantSemanticAction(const int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Constant * constant = calloc(1, sizeof(Constant));
	constant->value = value;
	return constant;
}

Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = type;
	return expression;
}

Expression * FactorExpressionSemanticAction(Factor * factor) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->factor = factor;
	expression->type = FACTOR;
	return expression;
}

Factor * ConstantFactorSemanticAction(Constant * constant) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->constant = constant;
	factor->type = CONSTANT;
	return factor;
}

Factor * ExpressionFactorSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->expression = expression;
	factor->type = EXPRESSION;
	return factor;
}

Program * ExpressionProgramSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->expression = expression;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}

/**
 * BoardSim semantic actions.
 */

// Helper function to add statements to AST
static void addStatementToAST(Statement* statement) {
	logDebugging(_logger, "addStatementToAST called with statement type: %d", statement->type);
	
	// Store for later addition to simulate block
	storeStatementForLater(statement);
}

Program * BoardSimProgramSemanticAction(TokenLabel token) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	
	// Only initialize AST root if it doesn't exist
	if (_compilerState->abstractSyntaxtTree == NULL) {
		// Create a proper AST structure for BoardSim programs
		ASTNode* rootNode = createASTNode(NULL, NODE_TYPE_SIMULATE_BLOCK);
		logDebugging(_logger, "Created BoardSim AST root node with type: %d", rootNode->nodeType);
		
		// Store the AST root in compiler state
		_compilerState->abstractSyntaxtTree = rootNode;
		logDebugging(_logger, "Stored AST root in compiler state");
	} else {
		logDebugging(_logger, "AST root already exists, not overwriting");
	}
	
	// Create a dummy program for compatibility
	Program* program = calloc(1, sizeof(Program));
	return program;
}

BoardDef * BoardDefSemanticAction(char* identifier, TokenLabel boardType, int size) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logError(_logger, "BoardDefSemanticAction called with identifier: %s, boardType: %d, size: %d", identifier ? identifier : "NULL", boardType, size);
	
	// Determine board type string based on token
	// GRAPH token has a specific value, LOOP has another
	// If size is 0 and we have a graph board declaration, use "graph"
	char* boardTypeStr;
	if (size == 0) {
		boardTypeStr = "graph";
	} else {
		boardTypeStr = "loop";
	}
	
	// Create board with actual data from parser
	BoardDef* boardDef = createBoardDef(identifier ? strdup(identifier) : strdup("DefaultBoard"), boardTypeStr, size);
	
	// Add to AST - initialize if not exists
	if (_compilerState != NULL) {
		if (_compilerState->abstractSyntaxtTree == NULL) {
			// Initialize AST root if not exists
			ASTNode* rootNode = createASTNode(NULL, NODE_TYPE_SIMULATE_BLOCK);
			_compilerState->abstractSyntaxtTree = rootNode;
			logError(_logger, "Initialized AST root in BoardDefSemanticAction");
		}
		
		ASTNode* rootNode = (ASTNode*)_compilerState->abstractSyntaxtTree;
		ASTNode* boardNode = createASTNode(boardDef, NODE_TYPE_BOARD_DEF);
		
		// Add to end of AST
		if (rootNode->next == NULL) {
			rootNode->next = boardNode;
		} else {
			ASTNode* current = rootNode;
			while (current->next != NULL) {
				current = current->next;
			}
			current->next = boardNode;
		}
		
		logError(_logger, "Added board definition to AST: %s type %s size %d", identifier, boardTypeStr, size);
		
		// Increment global counter for game detection
		g_parsedBoards++;
	} else {
		logError(_logger, "Failed to add board to AST - compiler state is NULL");
	}
	
	return boardDef;
}

CellDef * CellDefSemanticAction(int index, char* name, int cost) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logError(_logger, "CellDefSemanticAction called with index: %d, name: %s, cost: %d", index, name ? name : "NULL", cost);
	
	// Create cell with actual name from parser
	CellDef* cellDef = createCellDef(index, name ? strdup(name) : strdup("Cell"), cost, 0, NULL);
	
	// Add to AST
	if (_compilerState != NULL && _compilerState->abstractSyntaxtTree != NULL) {
		ASTNode* rootNode = (ASTNode*)_compilerState->abstractSyntaxtTree;
		ASTNode* cellNode = createASTNode(cellDef, NODE_TYPE_CELL_DEF);
		
		// Add to end of AST
		if (rootNode->next == NULL) {
			rootNode->next = cellNode;
		} else {
			ASTNode* current = rootNode;
			while (current->next != NULL) {
				current = current->next;
			}
			current->next = cellNode;
		}
		
		logDebugging(_logger, "Added cell definition to AST: %d - %s (cost=%d)", index, name, cost);
	}
	
	return cellDef;
}

PlayerDef * PlayerDefSemanticAction(int id, int money, int position, char* strategy) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logError(_logger, "PlayerDefSemanticAction called with id: %d, strategy: %s", id, strategy ? strategy : "NULL");
	
	// Count the player for game detection
	g_parsedPlayers++;
	
	// Create player with actual data from parser
	PlayerDef* playerDef = createPlayerDef(id, money, position);
	
	// Set strategy if provided (directly from parser)
	if (strategy != NULL) {
		playerDef->strategy = strdup(strategy);
		logError(_logger, "PlayerDefSemanticAction: Set strategy to %s for player %d", strategy, id);
	} else {
		logError(_logger, "PlayerDefSemanticAction: No strategy provided for player %d", id);
	}
	
	// Add to AST
	if (_compilerState != NULL && _compilerState->abstractSyntaxtTree != NULL) {
		ASTNode* rootNode = (ASTNode*)_compilerState->abstractSyntaxtTree;
		ASTNode* playerNode = createASTNode(playerDef, NODE_TYPE_PLAYER_DEF);
		
		// Add to end of AST
		if (rootNode->next == NULL) {
			rootNode->next = playerNode;
		} else {
			ASTNode* current = rootNode;
			while (current->next != NULL) {
				current = current->next;
			}
			current->next = playerNode;
		}
		
		logDebugging(_logger, "Added player definition to AST: %d (money=%d, position=%d, strategy=%s)", id, money, position, playerDef->strategy ? playerDef->strategy : "none");
	}
	
	return playerDef;
}

DiceDef * DiceDefSemanticAction(int sides) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	
	// Count the dice for game detection
	g_parsedDice++;
	
	// Create the dice definition
	DiceDef* diceDef = createDiceDef(sides);
	
	// Add the dice definition to the AST
	if (_compilerState != NULL) {
		// Initialize AST root if it doesn't exist
		if (_compilerState->abstractSyntaxtTree == NULL) {
			ASTNode* rootNode = createASTNode(NULL, NODE_TYPE_SIMULATE_BLOCK);
			_compilerState->abstractSyntaxtTree = rootNode;
			logError(_logger, "Initialized AST root in DiceDefSemanticAction");
		}
		
		// Create AST node for dice definition
		ASTNode* diceNode = createASTNode(diceDef, NODE_TYPE_DICE_DEF);
		
		// Add to AST
		ASTNode* rootNode = (ASTNode*)_compilerState->abstractSyntaxtTree;
		if (rootNode->next == NULL) {
			rootNode->next = diceNode;
		} else {
			ASTNode* current = rootNode;
			while (current->next != NULL) {
				current = current->next;
			}
			current->next = diceNode;
		}
		
		logError(_logger, "Added dice definition to AST: %d sides", sides);
	} else {
		logError(_logger, "Failed to add dice to AST - compiler state is NULL");
	}
	
	return diceDef;
}

SimulateBlock * SimulateBlockSemanticAction(int turns) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	g_simulateTurns = turns;  // Store the parsed simulate turns globally
	
	// Create the simulate block
	SimulateBlock* simulateBlock = createSimulateBlock(turns, NULL);
	
	// Add the simulate block as a separate node to the AST
	if (_compilerState != NULL && _compilerState->abstractSyntaxtTree != NULL) {
		ASTNode* rootNode = (ASTNode*)_compilerState->abstractSyntaxtTree;
		ASTNode* simulateNode = createASTNode(simulateBlock, NODE_TYPE_SIMULATE_BLOCK);
		
		// Add to end of AST
		if (rootNode->next == NULL) {
			rootNode->next = simulateNode;
		} else {
			ASTNode* current = rootNode;
			while (current->next != NULL) {
				current = current->next;
			}
			current->next = simulateNode;
		}
		
		logDebugging(_logger, "Added simulate block to AST with %d turns", turns);
		
		// Add all pending statements to the simulate block
		addPendingStatementsToSimulateBlock(simulateBlock);
		
		logDebugging(_logger, "Simulate block created with %d turns", turns);
	} else {
		logDebugging(_logger, "Failed to assign simulate block - compiler state or AST is NULL");
	}
	
	return simulateBlock;
}

Statement * PrintStatementSemanticAction(char* message) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logError(_logger, "PrintStatementSemanticAction called with message: %s", message ? message : "NULL");
	
	// Use actual message from parser
	Statement* statement = createStatement(STATEMENT_PRINT, message ? strdup(message) : strdup(""));
	
	storeStatementForLater(statement);
	
	return statement;
}

Statement * LogStatementSemanticAction(char* message) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logError(_logger, "LogStatementSemanticAction called with message: %s", message ? message : "NULL");
	
	// Use actual message from parser
	Statement* statement = createStatement(STATEMENT_LOG, message ? strdup(message) : strdup(""));
	
	storeStatementForLater(statement);
	
	return statement;
}

Statement * IntVariableSemanticAction(char* name, int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logError(_logger, "IntVariableSemanticAction called with name: %s, value: %d", name ? name : "NULL", value);
	
	// Use actual variable name from parser
	Statement* statement = createVariableStatement(VAR_TYPE_INT, name ? strdup(name) : strdup("var"), &value);
	
	storeStatementForLater(statement);
	
	return statement;
}

Statement * StringVariableSemanticAction(char* name, char* value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logError(_logger, "StringVariableSemanticAction called with name: %s, value: %s", name ? name : "NULL", value ? value : "NULL");
	
	// Use actual variable name and value from parser
	char* stringValue = value ? strdup(value) : strdup("");
	Statement* statement = createVariableStatement(VAR_TYPE_STRING, name ? strdup(name) : strdup("var"), stringValue);
	
	storeStatementForLater(statement);
	
	return statement;
}

Statement * BoolVariableSemanticAction(char* name, char* value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logError(_logger, "BoolVariableSemanticAction called with name: %s, value: %s", name ? name : "NULL", value ? value : "NULL");
	
	// Parse boolean value from string
	bool boolValue = (value != NULL && (strcmp(value, "true") == 0 || strcmp(value, "1") == 0));
	Statement* statement = createVariableStatement(VAR_TYPE_BOOL, name ? strdup(name) : strdup("var"), &boolValue);
	
	storeStatementForLater(statement);
	
	return statement;
}

Statement * IfStatementSemanticAction(Condition* condition, TokenLabel ifBody) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logError(_logger, "IfStatementSemanticAction called");
	
	// Get the last pending statement as the if body
	Statement* bodyStatement = NULL;
	if (g_pendingStatementCount > 0) {
		bodyStatement = g_pendingStatements[g_pendingStatementCount - 1];
		g_pendingStatementCount--;
	}
	
	// Use the semantic condition directly (no string conversion!)
	Statement* statement = createIfStatement(condition, bodyStatement);
	storeStatementForLater(statement);
	
	return statement;
}

Statement * IfElseStatementSemanticAction(Condition* condition, TokenLabel ifBody, TokenLabel elseBody) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logError(_logger, "IfElseStatementSemanticAction called");
	
	// Get the last two pending statements: first is elseBody, second is ifBody
	Statement* elseBodyStatement = NULL;
	Statement* ifBodyStatement = NULL;
	
	if (g_pendingStatementCount > 0) {
		elseBodyStatement = g_pendingStatements[g_pendingStatementCount - 1];
		g_pendingStatementCount--;
	}
	if (g_pendingStatementCount > 0) {
		ifBodyStatement = g_pendingStatements[g_pendingStatementCount - 1];
		g_pendingStatementCount--;
	}
	
	// Use the semantic condition directly (no string conversion!)
	Statement* statement = createIfElseStatement(condition, ifBodyStatement, elseBodyStatement);
	storeStatementForLater(statement);
	
	return statement;
}

Statement * ForStatementSemanticAction(char* var, int start, int end, TokenLabel body) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logError(_logger, "ForStatementSemanticAction called with var: %s, start: %d, end: %d", var ? var : "NULL", start, end);
	
	// Get the last pending statement as the body
	Statement* bodyStatement = NULL;
	if (g_pendingStatementCount > 0) {
		bodyStatement = g_pendingStatements[g_pendingStatementCount - 1];
		g_pendingStatementCount--;
	}
	
	// Store range semantically (no string conversion!)
	Statement* statement = createForStatement(var ? strdup(var) : strdup("i"), start, end, bodyStatement);
	storeStatementForLater(statement);
	
	return statement;
}

Statement * WhileStatementSemanticAction(Condition* condition, TokenLabel body) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logError(_logger, "WhileStatementSemanticAction called");
	
	// Get the last pending statement as the body
	Statement* bodyStatement = NULL;
	if (g_pendingStatementCount > 0) {
		bodyStatement = g_pendingStatements[g_pendingStatementCount - 1];
		g_pendingStatementCount--;
	}
	
	// Use the semantic condition directly (no string conversion!)
	Statement* statement = createWhileStatement(condition, bodyStatement);
	storeStatementForLater(statement);
	
	return statement;
}

// === CONDITION SEMANTIC ACTIONS (Semantic, no double-parsing!) ===

Condition* ComparisonExpressionSemanticAction(char* left, int right, TokenLabel operator) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	
	// Convert TokenLabel to ComparisonOperator
	ComparisonOperator op;
	switch (operator) {
		case GREATER_THAN: op = CMP_GREATER_THAN; break;
		case LESS_THAN: op = CMP_LESS_THAN; break;
		case GREATER_EQUAL: op = CMP_GREATER_EQUAL; break;
		case LESS_EQUAL: op = CMP_LESS_EQUAL; break;
		case EQUALITY: op = CMP_EQUAL; break;
		case NOT_EQUAL: op = CMP_NOT_EQUAL; break;
		default: op = CMP_GREATER_THAN; break;
	}
	
	// Create semantic comparison expression (no snprintf!)
	ComparisonExpression* expr = createComparisonExpression(left, right, op);
	Condition* cond = createComparisonCondition(expr);
	
	logError(_logger, "ComparisonExpressionSemanticAction: %s %d %d", left ? left : "var", op, right);
	return cond;
}

Condition* IdentifierConditionSemanticAction(char* identifier) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logError(_logger, "IdentifierConditionSemanticAction: %s", identifier ? identifier : "NULL");
	return createIdentifierCondition(identifier);
}

Condition* IntegerConditionSemanticAction(int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	// Integer as condition: non-zero is true
	ComparisonExpression* expr = createComparisonExpression("_literal", value, CMP_NOT_EQUAL);
	expr->rightOperand = 0;  // Compare to 0
	return createComparisonCondition(expr);
}

Condition* StringConditionSemanticAction(char* value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	// String as condition: create identifier condition
	return createIdentifierCondition(value);
}
