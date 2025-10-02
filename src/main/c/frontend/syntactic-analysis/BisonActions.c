#include "BisonActions.h"

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

Program * BoardSimProgramSemanticAction(TokenLabel token) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	// Mark as BoardSim program by setting expression to NULL
	program->expression = NULL;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}

BoardDef * BoardDefSemanticAction(TokenLabel identifier, TokenLabel boardType, int size) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
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
	return createPlayerDef(id, money, position);
}

DiceDef * DiceDefSemanticAction(int sides) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return createDiceDef(sides);
}

SimulateBlock * SimulateBlockSemanticAction(int turns) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return createSimulateBlock(turns, NULL);
}
