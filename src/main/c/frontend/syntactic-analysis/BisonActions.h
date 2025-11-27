#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonParser.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState);

/**
 * Bison semantic actions.
 */

Constant * IntegerConstantSemanticAction(const int value);
Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type);
Expression * FactorExpressionSemanticAction(Factor * factor);
Factor * ConstantFactorSemanticAction(Constant * constant);
Factor * ExpressionFactorSemanticAction(Expression * expression);
Program * ExpressionProgramSemanticAction(Expression * expression);

/**
 * BoardSim semantic actions.
 * Functions use char* for string values passed from Bison grammar (STRING_LITERAL, IDENTIFIER).
 */
Program * BoardSimProgramSemanticAction(TokenLabel token);
BoardDef * BoardDefSemanticAction(char* identifier, TokenLabel boardType, int size);
CellDef * CellDefSemanticAction(int index, char* name, int cost);
PlayerDef * PlayerDefSemanticAction(int id, int money, int position, char* strategy);
DiceDef * DiceDefSemanticAction(int sides);
SimulateBlock * SimulateBlockSemanticAction(int turns);
Statement * PrintStatementSemanticAction(char* message);
Statement * LogStatementSemanticAction(char* message);
Statement * IntVariableSemanticAction(char* name, int value);
Statement * StringVariableSemanticAction(char* name, char* value);
Statement * BoolVariableSemanticAction(char* name, char* value);
Statement * IfStatementSemanticAction(Condition* condition, TokenLabel ifBody);
Statement * IfElseStatementSemanticAction(Condition* condition, TokenLabel ifBody, TokenLabel elseBody);
Statement * ForStatementSemanticAction(char* var, int start, int end, TokenLabel body);
Statement * WhileStatementSemanticAction(Condition* condition, TokenLabel body);

/* Condition semantic actions - returns semantic Condition* (no double-parsing!) */
Condition* ComparisonExpressionSemanticAction(char* left, int right, TokenLabel operator);
Condition* IdentifierConditionSemanticAction(char* identifier);
Condition* IntegerConditionSemanticAction(int value);
Condition* StringConditionSemanticAction(char* value);

#endif
