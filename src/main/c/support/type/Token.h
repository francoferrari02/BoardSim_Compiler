#ifndef TOKEN_HEADER
#define TOKEN_HEADER

#include "FlexContext.h"
#include "SemanticValue.h"
#include "TokenLabel.h"

/**
 * The unit of information to transfer between a lexical-analyzer and a
 * syntactic-analyzer.
 */
typedef struct {
	char * lexeme;
	FlexContext context;
	SemanticValue * semanticValue;
	TokenLabel label;
	unsigned int length;
	unsigned int line;
} Token;

enum BoardSimTokens {
	// Keywords
	TOKEN_BOARD,
	TOKEN_PIECE,
	TOKEN_PLAYER,
	TOKEN_EVENT,
	TOKEN_DICE,
	TOKEN_RULE,
	TOKEN_SIMULATE,
	TOKEN_MOVE,
	TOKEN_APPLY,
	TOKEN_IF,
	TOKEN_ELSE,
	TOKEN_ELSEIF,
	TOKEN_FOR,
	TOKEN_WHILE,
	TOKEN_SWITCH,
	TOKEN_CASE,
	TOKEN_PRINT,
	TOKEN_LOG,
	TOKEN_EXPORT,
	TOKEN_CELL,
	TOKEN_NODE,
	TOKEN_LOOP,
	TOKEN_GRAPH,
	TOKEN_CONNECTED,
	TOKEN_OWNS,
	TOKEN_ARMIES,
	TOKEN_CONTINENT,
	TOKEN_STRATEGY,
	TOKEN_RANDOM,
	TOKEN_AGGRESSIVE,
	TOKEN_TURNS,
	TOKEN_SIDES,
	TOKEN_COST,
	TOKEN_RENT,
	TOKEN_MONEY,
	TOKEN_POSITION,
	TOKEN_TO,
	
	// Types
	TOKEN_INT_TYPE,
	TOKEN_STRING_TYPE,
	TOKEN_BOOL_TYPE,
	
	// Literals
	TOKEN_TRUE,
	TOKEN_FALSE,
	TOKEN_IDENTIFIER,
	TOKEN_STRING_LITERAL,
	
	// Operators
	TOKEN_MOD,
	TOKEN_EQ,
	TOKEN_NEQ,
	TOKEN_LEQ,
	TOKEN_GEQ,
	TOKEN_LT,
	TOKEN_GT,
	TOKEN_AND,
	TOKEN_OR,
	TOKEN_NOT,
	TOKEN_ASSIGN,
	TOKEN_ADD_ASSIGN,
	TOKEN_SUB_ASSIGN,
	
	// Delimiters
	TOKEN_OPEN_BRACKET,
	TOKEN_CLOSE_BRACKET,
	TOKEN_SEMICOLON,
	TOKEN_COMMA
};

#endif
