%top{

#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/TokenLabel.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"

}

%{

/**
 * @see https://westes.github.io/flex/manual/Definitions-Section.html#Definitions-Section
 */

extern CompilationStatus ArithmeticOperatorLexemeAction(TokenLabel label);
extern CompilationStatus EnterImportExpressionLexemeAction(FlexContext context);
extern CompilationStatus EnterMultilineCommentLexemeAction(FlexContext context);
extern CompilationStatus EOFLexemeAction();
extern CompilationStatus IgnoredLexemeAction();
extern CompilationStatus IntegerLexemeAction();
extern CompilationStatus LeaveImportExpressionLexemeAction();
extern CompilationStatus LeaveMultilineCommentLexemeAction();
extern CompilationStatus ParenthesisLexemeAction(TokenLabel label);
extern CompilationStatus SubexpressionLexemeAction();
extern CompilationStatus UnknownLexemeAction();

extern CompilationStatus BoardSimKeywordLexemeAction(TokenLabel label);
extern CompilationStatus BoardSimTypeLexemeAction(TokenLabel label);
extern CompilationStatus BoardSimLiteralLexemeAction(TokenLabel label);
extern CompilationStatus ComparisonOperatorLexemeAction(TokenLabel label);
extern CompilationStatus LogicalOperatorLexemeAction(TokenLabel label);
extern CompilationStatus AssignmentOperatorLexemeAction(TokenLabel label);
extern CompilationStatus DelimiterLexemeAction(TokenLabel label);
extern CompilationStatus IdentifierLexemeAction();
extern CompilationStatus StringLiteralLexemeAction();

%}

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */
%x IMPORT_EXPRESSION
%x MULTILINE_COMMENT

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */
whitespace			[[:space:]]+
identifier			[a-zA-Z_][a-zA-Z0-9_]*
integer				[0-9]+
string_literal		\"[^\"]*\"
comment_single		//.*
comment_multi_start	\/\*
comment_multi_end	\*\/

%%

%{

/**
 * @see https://westes.github.io/flex/manual/Rules-Section.html#Rules-Section
 */

%}

"{"									{ return EnterImportExpressionLexemeAction(IMPORT_EXPRESSION); }
<IMPORT_EXPRESSION>[^{}]+			{ return SubexpressionLexemeAction(); }
<IMPORT_EXPRESSION>"}"				{ return LeaveImportExpressionLexemeAction(); }
<IMPORT_EXPRESSION>.				{ return UnknownLexemeAction(); }

"/*"								{ return EnterMultilineCommentLexemeAction(MULTILINE_COMMENT); }
<MULTILINE_COMMENT>"*/"				{ return LeaveMultilineCommentLexemeAction(); }
<MULTILINE_COMMENT>[^*]+			{ return IgnoredLexemeAction(); }
<MULTILINE_COMMENT>.				{ return IgnoredLexemeAction(); }

"board"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"cell"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"loop"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"graph"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"player"							{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"piece"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"event"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"dice"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"rule"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"simulate"							{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"move"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"apply"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"if"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"else"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"for"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"while"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"print"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"log"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"export"							{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"turns"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"sides"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"cost"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"rent"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"money"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"position"							{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"to"								{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"strategy"							{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"random"							{ return BoardSimKeywordLexemeAction(UNKNOWN); }
"aggressive"						{ return BoardSimKeywordLexemeAction(UNKNOWN); }

"-"									{ return ArithmeticOperatorLexemeAction(SUB); }
"*"									{ return ArithmeticOperatorLexemeAction(MUL); }
"/"									{ return ArithmeticOperatorLexemeAction(DIV); }
"+"									{ return ArithmeticOperatorLexemeAction(ADD); }

"("									{ return ParenthesisLexemeAction(OPEN_PARENTHESIS); }
")"									{ return ParenthesisLexemeAction(CLOSE_PARENTHESIS); }
"["									{ return ParenthesisLexemeAction(OPEN_BRACE); }
"]"									{ return ParenthesisLexemeAction(CLOSE_BRACE); }
"{"									{ return EnterImportExpressionLexemeAction(IMPORT_EXPRESSION); }
"}"									{ return ParenthesisLexemeAction(CLOSE_BRACE); }
";"									{ return DelimiterLexemeAction(UNKNOWN); }
","									{ return DelimiterLexemeAction(UNKNOWN); }


[a-zA-Z_][a-zA-Z0-9_]*				{ return UnknownLexemeAction(); }
[[:digit:]]+						{ return IntegerLexemeAction(); }
\"[^\"]*\"							{ return UnknownLexemeAction(); }
\/\/.*								{ return IgnoredLexemeAction(); }
{whitespace}						{ return IgnoredLexemeAction(); }
.									{ return UnknownLexemeAction(); }

<<EOF>>								{ return EOFLexemeAction(); }

%%

/**
 * @see https://westes.github.io/flex/manual/User-Code-Section.html#User-Code-Section
 */

#include "FlexActions.h"
#include "FlexExport.h"
