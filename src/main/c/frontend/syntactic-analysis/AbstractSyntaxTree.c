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
		// Free the data based on nodeType
		if (node->data != NULL) {
			switch (node->nodeType) {
				case NODE_TYPE_BOARD_DEF:
					destroyBoardDef((BoardDef*)node->data);
					break;
				case NODE_TYPE_PIECE_DEF:
					destroyPieceDef((PieceDef*)node->data);
					break;
				case NODE_TYPE_SIMULATE_BLOCK:
					destroySimulateBlock((SimulateBlock*)node->data);
					break;
				case NODE_TYPE_CELL_DEF:
					destroyCellDef((CellDef*)node->data);
					break;
				case NODE_TYPE_PLAYER_DEF:
					destroyPlayerDef((PlayerDef*)node->data);
					break;
				case NODE_TYPE_DICE_DEF:
					destroyDiceDef((DiceDef*)node->data);
					break;
				case NODE_TYPE_RULE_DEF:
					destroyRuleDef((RuleDef*)node->data);
					break;
				default:
					free(node->data);
					break;
			}
		}
		
		// Recursively destroy next node
		if (node->next) {
			destroyASTNode(node->next);
		}
		free(node);
	}
}

// Additional BoardSim node creators/destructors

CellDef* createCellDef(int index, char* name, int cost, int rent, char* event) {
	logDebugging(_logger, "Executing constructor: %s", __FUNCTION__);
	CellDef* cellDef = calloc(1, sizeof(CellDef));
	cellDef->index = index;
	cellDef->name = name ? strdup(name) : NULL;
	cellDef->cost = cost;
	cellDef->rent = rent;
	cellDef->event = event ? strdup(event) : NULL;
	return cellDef;
}

void destroyCellDef(CellDef* cellDef) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (cellDef == NULL) {
		return;
	}
	if (cellDef->name != NULL) {
		free(cellDef->name);
	}
	if (cellDef->event != NULL) {
		free(cellDef->event);
	}
	free(cellDef);
}

PlayerDef* createPlayerDef(int id, int money, int position) {
	logDebugging(_logger, "Executing constructor: %s", __FUNCTION__);
	PlayerDef* playerDef = calloc(1, sizeof(PlayerDef));
	playerDef->id = id;
	playerDef->money = money;
	playerDef->position = position;
	playerDef->strategy = NULL; // Default to NULL
	return playerDef;
}

void destroyPlayerDef(PlayerDef* playerDef) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (playerDef == NULL) {
		return;
	}
	if (playerDef->strategy != NULL) {
		free(playerDef->strategy);
	}
	free(playerDef);
}

DiceDef* createDiceDef(int sides) {
	logDebugging(_logger, "Executing constructor: %s", __FUNCTION__);
	DiceDef* diceDef = calloc(1, sizeof(DiceDef));
	diceDef->sides = sides;
	return diceDef;
}

void destroyDiceDef(DiceDef* diceDef) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (diceDef == NULL) {
		return;
	}
	free(diceDef);
}

RuleDef* createRuleDef(char* name) {
	logDebugging(_logger, "Executing constructor: %s", __FUNCTION__);
	RuleDef* ruleDef = calloc(1, sizeof(RuleDef));
	ruleDef->name = name ? strdup(name) : NULL;
	return ruleDef;
}

void destroyRuleDef(RuleDef* ruleDef) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (ruleDef == NULL) {
		return;
	}
	if (ruleDef->name != NULL) {
		free(ruleDef->name);
	}
	free(ruleDef);
}

Statement* createStatement(StatementType type, char* text) {
	logDebugging(_logger, "Executing constructor: %s", __FUNCTION__);
	Statement* statement = calloc(1, sizeof(Statement));
	statement->type = type;
	statement->data.text = (text != NULL) ? strdup(text) : NULL;
	return statement;
}

Statement* createVariableStatement(VariableType varType, char* name, void* value) {
	logDebugging(_logger, "Executing constructor: %s", __FUNCTION__);
	Statement* statement = calloc(1, sizeof(Statement));
	statement->type = STATEMENT_VARIABLE_DECL;
	statement->data.variable = createVariable(varType, name, value);
	return statement;
}

void destroyStatement(Statement* statement) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (statement == NULL) {
		return;
	}
	if (statement->type == STATEMENT_VARIABLE_DECL) {
		destroyVariable(statement->data.variable);
	} else if (statement->type == STATEMENT_IF || statement->type == STATEMENT_IF_ELSE) {
		destroyConditionalStatement(statement->data.conditional);
	} else if (statement->type == STATEMENT_FOR || statement->type == STATEMENT_WHILE) {
		destroyLoopStatement(statement->data.loop);
	} else if (statement->data.text != NULL) {
		free(statement->data.text);
	}
	free(statement);
}

Statement* createForStatement(char* condition, Statement* body) {
	logDebugging(_logger, "Executing constructor: %s", __FUNCTION__);
	Statement* statement = calloc(1, sizeof(Statement));
	statement->type = STATEMENT_FOR;
	statement->data.loop = createLoopStatement("for", condition, body);
	return statement;
}

Statement* createWhileStatement(char* condition, Statement* body) {
	logDebugging(_logger, "Executing constructor: %s", __FUNCTION__);
	Statement* statement = calloc(1, sizeof(Statement));
	statement->type = STATEMENT_WHILE;
	statement->data.loop = createLoopStatement("while", condition, body);
	return statement;
}

Statement* createIfStatement(char* condition, Statement* ifBody) {
	logDebugging(_logger, "Executing constructor: %s", __FUNCTION__);
	Statement* statement = calloc(1, sizeof(Statement));
	statement->type = STATEMENT_IF;
	statement->data.conditional = createConditionalStatement(condition, ifBody, NULL);
	return statement;
}

Statement* createIfElseStatement(char* condition, Statement* ifBody, Statement* elseBody) {
	logDebugging(_logger, "Executing constructor: %s", __FUNCTION__);
	Statement* statement = calloc(1, sizeof(Statement));
	statement->type = STATEMENT_IF_ELSE;
	statement->data.conditional = createConditionalStatement(condition, ifBody, elseBody);
	return statement;
}

Variable* createVariable(VariableType type, char* name, void* value) {
	logDebugging(_logger, "Executing constructor: %s", __FUNCTION__);
	Variable* variable = calloc(1, sizeof(Variable));
	variable->type = type;
	variable->name = (name != NULL) ? strdup(name) : NULL;
	
	if (value != NULL) {
		switch (type) {
			case VAR_TYPE_INT:
				variable->value.intValue = *(int*)value;
				break;
			case VAR_TYPE_STRING:
				variable->value.stringValue = strdup((char*)value);
				break;
			case VAR_TYPE_BOOL:
				variable->value.boolValue = *(bool*)value;
				break;
		}
	}
	return variable;
}

void destroyVariable(Variable* variable) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (variable == NULL) {
		return;
	}
	if (variable->name != NULL) {
		free(variable->name);
	}
	if (variable->type == VAR_TYPE_STRING && variable->value.stringValue != NULL) {
		free(variable->value.stringValue);
	}
	free(variable);
}

ConditionalStatement* createConditionalStatement(char* condition, Statement* ifBody, Statement* elseBody) {
	logDebugging(_logger, "Executing constructor: %s", __FUNCTION__);
	ConditionalStatement* conditional = calloc(1, sizeof(ConditionalStatement));
	conditional->condition = (condition != NULL) ? strdup(condition) : NULL;
	conditional->ifBody = ifBody;
	conditional->elseBody = elseBody;
	return conditional;
}

void destroyConditionalStatement(ConditionalStatement* conditional) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (conditional == NULL) {
		return;
	}
	if (conditional->condition != NULL) {
		free(conditional->condition);
	}
	if (conditional->ifBody != NULL) {
		destroyStatement(conditional->ifBody);
	}
	if (conditional->elseBody != NULL) {
		destroyStatement(conditional->elseBody);
	}
	free(conditional);
}

LoopStatement* createLoopStatement(char* type, char* condition, Statement* body) {
	logDebugging(_logger, "Executing constructor: %s", __FUNCTION__);
	LoopStatement* loop = calloc(1, sizeof(LoopStatement));
	loop->type = (type != NULL) ? strdup(type) : NULL;
	loop->condition = (condition != NULL) ? strdup(condition) : NULL;
	loop->body = body;
	return loop;
}

void destroyLoopStatement(LoopStatement* loop) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (loop == NULL) {
		return;
	}
	if (loop->type != NULL) {
		free(loop->type);
	}
	if (loop->condition != NULL) {
		free(loop->condition);
	}
	if (loop->body != NULL) {
		destroyStatement(loop->body);
	}
	free(loop);
}

BoardSimProgram* createBoardSimProgram() {
	logDebugging(_logger, "Executing constructor: %s", __FUNCTION__);
	BoardSimProgram* program = calloc(1, sizeof(BoardSimProgram));
	program->declarations = NULL;
	program->statements = NULL;
	return program;
}

void destroyBoardSimProgram(BoardSimProgram* program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program == NULL) {
		return;
	}
	if (program->declarations != NULL) {
		destroyASTNode(program->declarations);
	}
	if (program->statements != NULL) {
		destroyASTNode(program->statements);
	}
	free(program);
}
