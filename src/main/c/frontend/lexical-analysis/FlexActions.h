#ifndef FLEX_ACTIONS_HEADER
#define FLEX_ACTIONS_HEADER

#include "../../support/configuration/Environment.h"
#include "../../support/language/String.h"
#include "../../support/logging/Logger.h"
#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/LexicalAnalyzer.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/Token.h"
#include "../../support/type/TokenLabel.h"
#include "../Frontend.h"

/** Initialize module's internal state. */
ModuleDestructor initializeFlexActionsModule();

/**
 * BoardSim lexeme actions
 */
CompilationStatus BoardSimKeywordLexemeAction(TokenLabel label);
CompilationStatus BoardSimTypeLexemeAction(TokenLabel label);
CompilationStatus BoardSimLiteralLexemeAction(TokenLabel label);
CompilationStatus ComparisonOperatorLexemeAction(TokenLabel label);
CompilationStatus LogicalOperatorLexemeAction(TokenLabel label);
CompilationStatus AssignmentOperatorLexemeAction(TokenLabel label);
CompilationStatus DelimiterLexemeAction(TokenLabel label);
CompilationStatus IdentifierLexemeAction();
CompilationStatus StringLiteralLexemeAction();

#endif
