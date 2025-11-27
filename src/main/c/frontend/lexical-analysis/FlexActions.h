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
ModuleDestructor initializeFlexActionsModule(LexicalAnalyzer * lexicalAnalyzer);

/**
 * BoardSim lexeme actions
 */
CompilationStatus BoardSimKeywordLexemeAction(TokenLabel label);
CompilationStatus ComparisonOperatorLexemeAction(TokenLabel label);
CompilationStatus DelimiterLexemeAction(TokenLabel label);
CompilationStatus IdentifierLexemeAction();

/**
 * String literal handling with Flex context (proper solution - no manual quote stripping!)
 */
CompilationStatus EnterStringLexemeAction(FlexContext context);
CompilationStatus AppendStringLexemeAction();
CompilationStatus LeaveStringLexemeAction();

#endif
