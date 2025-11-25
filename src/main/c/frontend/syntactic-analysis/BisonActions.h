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
ModuleDestructor initializeBisonActionsModule();

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
 */
Program * BoardSimProgramSemanticAction(TokenLabel token);
BoardDef * BoardDefSemanticAction(TokenLabel identifier, TokenLabel boardType, int size);
CellDef * CellDefSemanticAction(int index, char* name, int cost);
PlayerDef * PlayerDefSemanticAction(int id, int money, int position);
DiceDef * DiceDefSemanticAction(int sides);
SimulateBlock * SimulateBlockSemanticAction(int turns);
Statement * PrintStatementSemanticAction(TokenLabel token);
Statement * LogStatementSemanticAction(TokenLabel token);
Statement * IntVariableSemanticAction(TokenLabel name, int value);
Statement * StringVariableSemanticAction(TokenLabel name, TokenLabel value);
Statement * BoolVariableSemanticAction(TokenLabel name, TokenLabel value);
Statement * IfStatementSemanticAction(TokenLabel condition, TokenLabel ifBody);
Statement * IfElseStatementSemanticAction(TokenLabel condition, TokenLabel ifBody, TokenLabel elseBody);
Statement * ForStatementSemanticAction(TokenLabel var, int start, int end, TokenLabel body);
Statement * WhileStatementSemanticAction(TokenLabel condition, TokenLabel body);

#endif
