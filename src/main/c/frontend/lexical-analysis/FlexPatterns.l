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

"/*"								{ return EnterMultilineCommentLexemeAction(MULTILINE_COMMENT); }
<MULTILINE_COMMENT>"*/"				{ return LeaveMultilineCommentLexemeAction(); }
<MULTILINE_COMMENT>[^*]+			{ return IgnoredLexemeAction(); }
<MULTILINE_COMMENT>.				{ return IgnoredLexemeAction(); }

"board"								{ return BoardSimKeywordLexemeAction(BOARD); }
"cell"								{ return BoardSimKeywordLexemeAction(CELL); }

"loop"								{ return BoardSimKeywordLexemeAction(LOOP); }
"graph"								{ return BoardSimKeywordLexemeAction(GRAPH); }
"player"							{ return BoardSimKeywordLexemeAction(PLAYER); }

"dice"								{ return BoardSimKeywordLexemeAction(DICE); }
"simulate"							{ return BoardSimKeywordLexemeAction(SIMULATE); }
"print"								{ return BoardSimKeywordLexemeAction(PRINT); }
"log"								{ return BoardSimKeywordLexemeAction(LOG); }
"int"								{ return BoardSimKeywordLexemeAction(INT); }
"string"							{ return BoardSimKeywordLexemeAction(STRING); }
"bool"								{ return BoardSimKeywordLexemeAction(BOOL); }
"if"								{ return BoardSimKeywordLexemeAction(IF); }
"else"								{ return BoardSimKeywordLexemeAction(ELSE); }
"then"								{ return BoardSimKeywordLexemeAction(THEN); }
"for"								{ return BoardSimKeywordLexemeAction(FOR); }
"while"								{ return BoardSimKeywordLexemeAction(WHILE); }
"in"								{ return BoardSimKeywordLexemeAction(IN); }
"turns"								{ return BoardSimKeywordLexemeAction(TURNS); }
"sides"								{ return BoardSimKeywordLexemeAction(SIDES); }
"cost"								{ return BoardSimKeywordLexemeAction(COST); }
"rent"								{ return BoardSimKeywordLexemeAction(RENT); }
"to"								{ return BoardSimKeywordLexemeAction(TO); }
"strategy"							{ return BoardSimKeywordLexemeAction(STRATEGY); }
"random"							{ return BoardSimKeywordLexemeAction(RANDOM); }
"aggressive"						{ return BoardSimKeywordLexemeAction(AGGRESSIVE); }
"money"								{ return BoardSimKeywordLexemeAction(MONEY); }
"position"							{ return BoardSimKeywordLexemeAction(POSITION); }

"-"									{ return ArithmeticOperatorLexemeAction(SUB); }
"*"									{ return ArithmeticOperatorLexemeAction(MUL); }
"/"									{ return ArithmeticOperatorLexemeAction(DIV); }
"+"									{ return ArithmeticOperatorLexemeAction(ADD); }

"<="									{ return ComparisonOperatorLexemeAction(LESS_EQUAL); }
">="									{ return ComparisonOperatorLexemeAction(GREATER_EQUAL); }
"!="									{ return ComparisonOperatorLexemeAction(NOT_EQUAL); }
"=="									{ return ComparisonOperatorLexemeAction(EQUALITY); }
"<"									{ return ComparisonOperatorLexemeAction(LESS_THAN); }
">"									{ return ComparisonOperatorLexemeAction(GREATER_THAN); }

"("									{ return ParenthesisLexemeAction(OPEN_PARENTHESIS); }
")"									{ return ParenthesisLexemeAction(CLOSE_PARENTHESIS); }
"["									{ return DelimiterLexemeAction(OPEN_BRACKET); }
"]"									{ return DelimiterLexemeAction(CLOSE_BRACKET); }
"{"									{ return ParenthesisLexemeAction(OPEN_BRACE); }
"}"									{ return ParenthesisLexemeAction(CLOSE_BRACE); }
";"									{ return DelimiterLexemeAction(SEMICOLON); }
","									{ return DelimiterLexemeAction(COMMA); }
"="									{ return DelimiterLexemeAction(EQUALS); }


[a-zA-Z_][a-zA-Z0-9_]*				{ return IdentifierLexemeAction(); }
[[:digit:]]+						{ return IntegerLexemeAction(); }
\"[^\"]*\"							{ return StringLiteralLexemeAction(); }
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
