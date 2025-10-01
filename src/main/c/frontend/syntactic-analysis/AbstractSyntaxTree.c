#include "AbstractSyntaxTree.h"
#include <string.h>

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: AbstractSyntaxTree...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntaxTree");
	return _shutdownAbstractSyntaxTreeModule;
}

/* PUBLIC FUNCTIONS */

void destroyConstant(Constant * constant) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (constant != NULL) {
		free(constant);
	}
}

void destroyExpression(Expression * expression) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (expression != NULL) {
		switch (expression->type) {
			case ADDITION:
			case DIVISION:
			case MULTIPLICATION:
			case SUBTRACTION:
				destroyExpression(expression->leftExpression);
				destroyExpression(expression->rightExpression);
				break;
			case FACTOR:
				destroyFactor(expression->factor);
				break;
		}
		free(expression);
	}
}

void destroyFactor(Factor * factor) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (factor != NULL) {
		switch (factor->type) {
			case CONSTANT:
				destroyConstant(factor->constant);
				break;
			case EXPRESSION:
				destroyExpression(factor->expression);
				break;
		}
		free(factor);
	}
}

void destroyProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		destroyExpression(program->expression);
		free(program);
	}
}

BoardDef* createBoardDef(char* id, char* type, int size) {
	logDebugging(_logger, "Creating BoardDef");
	BoardDef* boardDef = calloc(1, sizeof(BoardDef));
	boardDef->id = strdup(id);
	boardDef->type = strdup(type);
	boardDef->size = size;
	boardDef->cells = NULL;  // Will be populated during parsing
	return boardDef;
}

void destroyBoardDef(BoardDef* boardDef) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (boardDef != NULL) {
		free(boardDef->id);
		free(boardDef->type);
		// TODO: Destroy cells linked list
		free(boardDef);
	}
}

PieceDef* createPieceDef(char* id, char* owns, int armies) {
	logDebugging(_logger, "Creating PieceDef");
	PieceDef* pieceDef = calloc(1, sizeof(PieceDef));
	pieceDef->id = strdup(id);
	pieceDef->owns = owns ? strdup(owns) : NULL;
	pieceDef->armies = armies;
	pieceDef->properties = NULL;
	return pieceDef;
}

void destroyPieceDef(PieceDef* pieceDef) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (pieceDef != NULL) {
		free(pieceDef->id);
		if (pieceDef->owns) free(pieceDef->owns);
		// TODO: Destroy properties linked list
		free(pieceDef);
	}
}

SimulateBlock* createSimulateBlock(int turns, char* strategy) {
	logDebugging(_logger, "Creating SimulateBlock");
	SimulateBlock* simulateBlock = calloc(1, sizeof(SimulateBlock));
	simulateBlock->turns = turns;
	simulateBlock->strategy = strategy ? strdup(strategy) : NULL;
	simulateBlock->statements = NULL;
	return simulateBlock;
}

void destroySimulateBlock(SimulateBlock* simulateBlock) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (simulateBlock != NULL) {
		if (simulateBlock->strategy) free(simulateBlock->strategy);
		// TODO: Destroy statements linked list
		free(simulateBlock);
	}
}

ASTNode* createASTNode(void* data, int nodeType) {
	logDebugging(_logger, "Creating ASTNode");
	ASTNode* node = calloc(1, sizeof(ASTNode));
	node->data = data;
	node->nodeType = nodeType;
	node->next = NULL;
	return node;
}

void destroyASTNode(ASTNode* node) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (node != NULL) {
		// Note: data should be destroyed by the specific type destructor
		// This function only handles the ASTNode wrapper
		if (node->next) {
			destroyASTNode(node->next);
		}
		free(node);
	}
}
