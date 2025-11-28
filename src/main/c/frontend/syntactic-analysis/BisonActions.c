#include "BisonActions.h"
#include "../../backend/domain-specific/BoardSim.h"

/* MODULE INTERNAL STATE */

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

// Global variable to store pending statements
static Statement* g_pendingStatements[100];
static int g_pendingStatementCount = 0;

// Tail pointer for O(1) AST node insertion
static ASTNode* g_astTail = NULL;

// Forward declarations for helper functions
static void storeStatementForLater(Statement* statement);
static void addPendingStatementsToSimulateBlock(SimulateBlock* simulateBlock);
static void appendNodeToAST(ASTNode* newNode);

/** Shutdown module's internal state. */
void _shutdownBisonActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: BisonActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	_compilerState = NULL;
	g_astTail = NULL;  // Reset tail pointer for next compilation
	g_pendingStatementCount = 0;
}

// Helper function to store statements for later addition
static void storeStatementForLater(Statement* statement) {
	if (g_pendingStatementCount < 100) {
		g_pendingStatements[g_pendingStatementCount++] = statement;
		logDebugging(_logger, "Stored statement %d for later addition", g_pendingStatementCount);
	} else {
		logError(_logger, "CRITICAL: Too many statements in simulate block (max 100). Statement ignored.");
	}
}

// Helper function to add all pending statements to simulate block (O(1) insertion)
static void addPendingStatementsToSimulateBlock(SimulateBlock* simulateBlock) {
	logDebugging(_logger, "Adding %d pending statements to simulate block", g_pendingStatementCount);
	
	ASTNode* stmtTail = NULL;  // Local tail for statements list
	int addedCount = 0;
	
	for (int i = 0; i < g_pendingStatementCount; i++) {
		Statement* statement = g_pendingStatements[i];
		if (statement != NULL) {
			ASTNode* statementNode = createASTNode(statement, NODE_TYPE_STATEMENT);
			
			// O(1) insertion using tail pointer
			if (simulateBlock->statements == NULL) {
				simulateBlock->statements = statementNode;
				stmtTail = statementNode;
			} else {
				stmtTail->next = statementNode;
				stmtTail = statementNode;
			}
			addedCount++;
			logDebugging(_logger, "Added pending statement %d to simulate block", i + 1);
		}
	}
	
	// Clear pending statements
	g_pendingStatementCount = 0;
	
	logDebugging(_logger, "Successfully added %d statements to simulate block", addedCount);
}

// Helper function to append a node to AST in O(1) time using tail pointer
static void appendNodeToAST(ASTNode* newNode) {
	if (_compilerState == NULL || newNode == NULL) return;
	
	// Initialize AST root if needed
	if (_compilerState->abstractSyntaxtTree == NULL) {
		ASTNode* rootNode = createASTNode(NULL, NODE_TYPE_SIMULATE_BLOCK);
		_compilerState->abstractSyntaxtTree = rootNode;
		g_astTail = rootNode;
		logDebugging(_logger, "Initialized AST root node");
	}
	
	// O(1) insertion at tail
	if (g_astTail != NULL) {
		g_astTail->next = newNode;
		g_astTail = newNode;
	} else {
		// Edge case: find tail (should not happen normally)
		ASTNode* current = (ASTNode*)_compilerState->abstractSyntaxtTree;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = newNode;
		g_astTail = newNode;
	}
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
		
		// Store the AST root in compiler state and initialize tail pointer
		_compilerState->abstractSyntaxtTree = rootNode;
		g_astTail = rootNode;
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
	logDebugging(_logger, "BoardDefSemanticAction called with identifier: %s, boardType: %d, size: %d", identifier ? identifier : "NULL", boardType, size);
	
	// Determine board type string based on token
	char* boardTypeStr = (size == 0) ? "graph" : "loop";
	
	// Create board with actual data from parser
	BoardDef* boardDef = createBoardDef(identifier ? strdup(identifier) : strdup("DefaultBoard"), boardTypeStr, size);
	
	// Add to AST using O(1) insertion
	ASTNode* boardNode = createASTNode(boardDef, NODE_TYPE_BOARD_DEF);
	appendNodeToAST(boardNode);
	
	logDebugging(_logger, "Added board definition to AST: %s type %s size %d", identifier, boardTypeStr, size);
	
	// Increment global counter for game detection
	g_parsedBoards++;
	
	return boardDef;
}

CellDef * CellDefSemanticAction(int index, char* name, int cost) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logDebugging(_logger, "CellDefSemanticAction called with index: %d, name: %s, cost: %d", index, name ? name : "NULL", cost);
	
	// Create cell with actual name from parser
	CellDef* cellDef = createCellDef(index, name ? strdup(name) : strdup("Cell"), cost, 0, NULL);
	
	// Add to AST using O(1) insertion
	ASTNode* cellNode = createASTNode(cellDef, NODE_TYPE_CELL_DEF);
	appendNodeToAST(cellNode);
	
	logDebugging(_logger, "Added cell definition to AST: %d - %s (cost=%d)", index, name, cost);
	
	return cellDef;
}

PlayerDef * PlayerDefSemanticAction(int id, int money, int position, char* strategy) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logDebugging(_logger, "PlayerDefSemanticAction called with id: %d, strategy: %s", id, strategy ? strategy : "NULL");
	
	// Count the player for game detection
	g_parsedPlayers++;
	
	// Create player with actual data from parser
	PlayerDef* playerDef = createPlayerDef(id, money, position);
	
	// Set strategy if provided (directly from parser)
	if (strategy != NULL) {
		playerDef->strategy = strdup(strategy);
		logDebugging(_logger, "PlayerDefSemanticAction: Set strategy to %s for player %d", strategy, id);
	}
	
	// Add to AST using O(1) insertion
	ASTNode* playerNode = createASTNode(playerDef, NODE_TYPE_PLAYER_DEF);
	appendNodeToAST(playerNode);
	
	logDebugging(_logger, "Added player definition to AST: %d (money=%d, position=%d, strategy=%s)", id, money, position, playerDef->strategy ? playerDef->strategy : "none");
	
	return playerDef;
}

DiceDef * DiceDefSemanticAction(int sides) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	
	// Count the dice for game detection
	g_parsedDice++;
	
	// Create the dice definition
	DiceDef* diceDef = createDiceDef(sides);
	
	// Add to AST using O(1) insertion
	ASTNode* diceNode = createASTNode(diceDef, NODE_TYPE_DICE_DEF);
	appendNodeToAST(diceNode);
	
	logDebugging(_logger, "Added dice definition to AST: %d sides", sides);
	
	return diceDef;
}

SimulateBlock * SimulateBlockSemanticAction(int turns) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	g_simulateTurns = turns;  // Store the parsed simulate turns globally
	
	// Create the simulate block
	SimulateBlock* simulateBlock = createSimulateBlock(turns, NULL);
	
	// Add all pending statements to the simulate block (O(1) insertion)
	addPendingStatementsToSimulateBlock(simulateBlock);
	
	// Add to AST using O(1) insertion
	ASTNode* simulateNode = createASTNode(simulateBlock, NODE_TYPE_SIMULATE_BLOCK);
	appendNodeToAST(simulateNode);
	
	logDebugging(_logger, "Added simulate block to AST with %d turns", turns);
	
	return simulateBlock;
}

Statement * PrintStatementSemanticAction(char* message) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logDebugging(_logger, "PrintStatementSemanticAction called with message: %s", message ? message : "NULL");
	
	// Use actual message from parser
	Statement* statement = createStatement(STATEMENT_PRINT, message ? strdup(message) : strdup(""));
	
	storeStatementForLater(statement);
	
	return statement;
}

Statement * LogStatementSemanticAction(char* message) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logDebugging(_logger, "LogStatementSemanticAction called with message: %s", message ? message : "NULL");
	
	// Use actual message from parser
	Statement* statement = createStatement(STATEMENT_LOG, message ? strdup(message) : strdup(""));
	
	storeStatementForLater(statement);
	
	return statement;
}

Statement * IntVariableSemanticAction(char* name, int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logDebugging(_logger, "IntVariableSemanticAction called with name: %s, value: %d", name ? name : "NULL", value);
	
	// Use actual variable name from parser
	Statement* statement = createVariableStatement(VAR_TYPE_INT, name ? strdup(name) : strdup("var"), &value);
	
	storeStatementForLater(statement);
	
	return statement;
}

Statement * StringVariableSemanticAction(char* name, char* value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logDebugging(_logger, "StringVariableSemanticAction called with name: %s, value: %s", name ? name : "NULL", value ? value : "NULL");
	
	// Use actual variable name and value from parser
	char* stringValue = value ? strdup(value) : strdup("");
	Statement* statement = createVariableStatement(VAR_TYPE_STRING, name ? strdup(name) : strdup("var"), stringValue);
	
	storeStatementForLater(statement);
	
	return statement;
}

Statement * BoolVariableSemanticAction(char* name, char* value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logDebugging(_logger, "BoolVariableSemanticAction called with name: %s, value: %s", name ? name : "NULL", value ? value : "NULL");
	
	// Parse boolean value from string
	bool boolValue = (value != NULL && (strcmp(value, "true") == 0 || strcmp(value, "1") == 0));
	Statement* statement = createVariableStatement(VAR_TYPE_BOOL, name ? strdup(name) : strdup("var"), &boolValue);
	
	storeStatementForLater(statement);
	
	return statement;
}

Statement * IfStatementSemanticAction(Condition* condition, TokenLabel ifBody) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logDebugging(_logger, "IfStatementSemanticAction called");
	
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
	logDebugging(_logger, "IfElseStatementSemanticAction called");
	
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
	logDebugging(_logger, "ForStatementSemanticAction called with var: %s, start: %d, end: %d", var ? var : "NULL", start, end);
	
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
	logDebugging(_logger, "WhileStatementSemanticAction called");
	
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
	
	logDebugging(_logger, "ComparisonExpressionSemanticAction: %s %d %d", left ? left : "var", op, right);
	return cond;
}

Condition* IdentifierConditionSemanticAction(char* identifier) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	logDebugging(_logger, "IdentifierConditionSemanticAction: %s", identifier ? identifier : "NULL");
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
