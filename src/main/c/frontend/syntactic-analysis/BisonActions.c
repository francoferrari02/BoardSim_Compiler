#include "BisonActions.h"
#include "../../backend/domain-specific/BoardSim.h"

/* MODULE INTERNAL STATE */

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownBisonActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: BisonActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	_compilerState = NULL;
}

ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState) {
	_compilerState = compilerState;
	_logger = createLogger("BisonActions");
	return _shutdownBisonActionsModule;
}

/* IMPORTED FUNCTIONS */

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
	if (_compilerState != NULL && _compilerState->abstractSyntaxtTree != NULL) {
		ASTNode* rootNode = (ASTNode*)_compilerState->abstractSyntaxtTree;
		if (rootNode->data != NULL) {
			SimulateBlock* simulateBlock = (SimulateBlock*)rootNode->data;
			
			// Create AST node for statement
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
		}
	}
}

Program * BoardSimProgramSemanticAction(TokenLabel token) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	
	// Create a proper AST structure for BoardSim programs
	ASTNode* rootNode = createASTNode(NULL, NODE_TYPE_SIMULATE_BLOCK);
	
	// Store the AST root in compiler state
	_compilerState->abstractSyntaxtTree = rootNode;
	
	// Create a dummy program for compatibility
	Program * program = calloc(1, sizeof(Program));
	program->expression = NULL;
	
	return program;
}

BoardDef * BoardDefSemanticAction(TokenLabel identifier, TokenLabel boardType, int size) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	
	// Count the board for game detection
	g_parsedBoards++;
	
	// For now, we'll use placeholder strings. 
	// TODO: Extract actual strings from tokens
	char* id = strdup("BoardSim_Board");
	char* type = (boardType == LOOP) ? strdup("loop") : strdup("graph");
	return createBoardDef(id, type, size);
}

CellDef * CellDefSemanticAction(int index, char* name, int cost) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	// For now, basic cell creation
	return createCellDef(index, name, cost, 0, NULL);
}

PlayerDef * PlayerDefSemanticAction(int id, int money, int position) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	
	// Count the player for game detection
	g_parsedPlayers++;
	
	return createPlayerDef(id, money, position);
}

DiceDef * DiceDefSemanticAction(int sides) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	
	// Count the dice for game detection
	g_parsedDice++;
	
	return createDiceDef(sides);
}

SimulateBlock * SimulateBlockSemanticAction(int turns) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	g_simulateTurns = turns;  // Store the parsed simulate turns globally
	
	// Create the simulate block
	SimulateBlock* simulateBlock = createSimulateBlock(turns, NULL);
	
	// Update the AST root with the simulate block
	if (_compilerState != NULL && _compilerState->abstractSyntaxtTree != NULL) {
		ASTNode* rootNode = (ASTNode*)_compilerState->abstractSyntaxtTree;
		rootNode->data = simulateBlock;
	}
	
	return simulateBlock;
}

Statement * PrintStatementSemanticAction(TokenLabel token) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	// TODO: Extract actual string from token
	Statement* statement = createStatement(STATEMENT_PRINT, "TEST PRINT");
	
	// Add to AST
	addStatementToAST(statement);
	
	return statement;
}

Statement * LogStatementSemanticAction(TokenLabel token) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	// TODO: Extract actual string from token
	Statement* statement = createStatement(STATEMENT_LOG, "TEST LOG");
	
	// Add to AST
	addStatementToAST(statement);
	
	return statement;
}

Statement * IntVariableSemanticAction(TokenLabel name, int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	// TODO: Extract actual identifier name from token
	Statement* statement = createVariableStatement(VAR_TYPE_INT, "testVar", &value);
	
	// Add to AST
	addStatementToAST(statement);
	
	return statement;
}

Statement * StringVariableSemanticAction(TokenLabel name, TokenLabel value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	// TODO: Extract actual strings from tokens
	char* stringValue = "testString";
	Statement* statement = createVariableStatement(VAR_TYPE_STRING, "testStringVar", stringValue);
	
	// Add to AST
	addStatementToAST(statement);
	
	return statement;
}

Statement * BoolVariableSemanticAction(TokenLabel name, TokenLabel value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	// TODO: Extract actual values from tokens (true/false from identifier)
	bool boolValue = true;
	Statement* statement = createVariableStatement(VAR_TYPE_BOOL, "testBoolVar", &boolValue);
	
	// Add to AST
	addStatementToAST(statement);
	
	return statement;
}

Statement * IfStatementSemanticAction(TokenLabel condition, TokenLabel ifBody) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	// TODO: Extract actual condition and body from tokens
	Statement* ifBodyStatement = createStatement(STATEMENT_PRINT, "if body executed");
	Statement* statement = createIfStatement("testCondition", ifBodyStatement);
	
	// Add to AST
	addStatementToAST(statement);
	
	return statement;
}

Statement * IfElseStatementSemanticAction(TokenLabel condition, TokenLabel ifBody, TokenLabel elseBody) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	// TODO: Extract actual condition, ifBody, and elseBody from tokens
	Statement* ifBodyStatement = createStatement(STATEMENT_PRINT, "if body executed");
	Statement* elseBodyStatement = createStatement(STATEMENT_PRINT, "else body executed");
	Statement* statement = createIfElseStatement("testCondition", ifBodyStatement, elseBodyStatement);
	
	// Add to AST
	addStatementToAST(statement);
	
	return statement;
}

Statement * ForStatementSemanticAction(TokenLabel var, int start, int end, TokenLabel body) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	// TODO: Extract actual variable name and body from tokens
	char condition[100];
	snprintf(condition, sizeof(condition), "%s in %d..%d", "testVar", start, end);
	Statement* bodyStatement = createStatement(STATEMENT_PRINT, "for body executed");
	Statement* statement = createForStatement(condition, bodyStatement);
	
	// Add to AST
	addStatementToAST(statement);
	
	return statement;
}

Statement * WhileStatementSemanticAction(TokenLabel condition, TokenLabel body) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	// TODO: Extract actual condition and body from tokens
	Statement* bodyStatement = createStatement(STATEMENT_PRINT, "while body executed");
	Statement* statement = createWhileStatement("testCondition", bodyStatement);
	
	// Add to AST
	addStatementToAST(statement);
	
	return statement;
}
