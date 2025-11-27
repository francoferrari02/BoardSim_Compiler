#include "FlexActions.h"

/* MODULE INTERNAL STATE */

static bool _logIgnoredLexemes = true;
static InputBuffer * _inputBuffer = NULL;
static LexicalAnalyzer * _lexicalAnalyzer = NULL;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownFlexActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: FlexActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	if (_inputBuffer != NULL) {
		destroyInputBuffer(_inputBuffer);
		_inputBuffer = NULL;
	}
	_lexicalAnalyzer = NULL;
}

ModuleDestructor initializeFlexActionsModule(LexicalAnalyzer * lexicalAnalyzer) {
	_inputBuffer = NULL;
	_lexicalAnalyzer = lexicalAnalyzer;
	_logger = createLogger("FlexActions");
	_logIgnoredLexemes = getBooleanOrDefault("LOG_IGNORED_LEXEMES", _logIgnoredLexemes);
	return _shutdownFlexActionsModule;
}

/* PRIVATE FUNCTIONS */

static void _logTokenAction(const char * actionName, Token * token);

/**
 * Logs a lexical-analyzer action over a token in DEBUGGING level.
 */
static void _logTokenAction(const char * actionName, Token * token) {
	char * _lexeme = escape(token->lexeme);
	logDebugging(_logger, WARNING_COLOR "%s" DEFAULT_COLOR ": Token(context=%d, label=%d, length=%d, lexeme=%s\"%s\"%s, line=%d, semanticValue=%p)",
		actionName,
		token->context,
		token->label,
		token->length,
		INFORMATION_COLOR, _lexeme, DEFAULT_COLOR,
		token->line,
		token->semanticValue);
	free(_lexeme);
	_lexeme = NULL;
}

/* PUBLIC FUNCTIONS */

CompilationStatus ArithmeticOperatorLexemeAction(TokenLabel label) {
	Token * token = createToken(_lexicalAnalyzer, label);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus EnterImportExpressionLexemeAction(FlexContext context) {
	if (_logIgnoredLexemes) {
		Token * token = createToken(_lexicalAnalyzer, OPEN_BRACE);
		_logTokenAction(__FUNCTION__, token);
		destroyToken(token);
	}
	enterLexicalAnalyzerContext(_lexicalAnalyzer, context);
	return IN_PROGRESS;
}

CompilationStatus EnterMultilineCommentLexemeAction(FlexContext context) {
	if (_logIgnoredLexemes) {
		Token * token = createToken(_lexicalAnalyzer, OPEN_COMMENT);
		_logTokenAction(__FUNCTION__, token);
		destroyToken(token);
	}
	enterLexicalAnalyzerContext(_lexicalAnalyzer, context);
	return IN_PROGRESS;
}

CompilationStatus EOFLexemeAction() {
	CompilationStatus status = IN_PROGRESS;
	Token * token = createToken(_lexicalAnalyzer, 0);
	_logTokenAction(__FUNCTION__, token);
	if (!popInputBuffer(_lexicalAnalyzer)) {
		status = pushToken(_lexicalAnalyzer, token);
		FlexContext context = currentLexicalAnalyzerContext(_lexicalAnalyzer);
		if (0 < context) {
			logError(_logger, "The final context is not closed (context=%d).", context);
			status = FAILED;
		}
	}
	destroyToken(token);
	return status;
}

CompilationStatus IgnoredLexemeAction() {
	if (_logIgnoredLexemes) {
		Token * token = createToken(_lexicalAnalyzer, IGNORED);
		_logTokenAction(__FUNCTION__, token);
		destroyToken(token);
	}
	return IN_PROGRESS;
}

CompilationStatus IntegerLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, INTEGER);
	token->semanticValue->integer = atoi(token->lexeme);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus LeaveImportExpressionLexemeAction() {
	pushInputBuffer(_inputBuffer);
	leaveLexicalAnalyzerContext(_lexicalAnalyzer);
	if (_logIgnoredLexemes) {
		Token * token = createToken(_lexicalAnalyzer, CLOSE_BRACE);
		_logTokenAction(__FUNCTION__, token);
		destroyToken(token);
	}
	return IN_PROGRESS;
}

CompilationStatus LeaveMultilineCommentLexemeAction() {
	leaveLexicalAnalyzerContext(_lexicalAnalyzer);
	if (_logIgnoredLexemes) {
		Token * token = createToken(_lexicalAnalyzer, CLOSE_COMMENT);
		_logTokenAction(__FUNCTION__, token);
		destroyToken(token);
	}
	return IN_PROGRESS;
}

CompilationStatus ParenthesisLexemeAction(TokenLabel label) {
	Token * token = createToken(_lexicalAnalyzer, label);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus SubexpressionLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, IGNORED);
	_inputBuffer = createInputBuffer(_lexicalAnalyzer, token->lexeme);
	if (_logIgnoredLexemes) {
		_logTokenAction(__FUNCTION__, token);
	}
	destroyToken(token);
	return IN_PROGRESS;
}

CompilationStatus UnknownLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, UNKNOWN);
	_logTokenAction(__FUNCTION__, token);
	destroyToken(token);
	return FAILED;
}

/**
 * Common token lexeme action - used by all simple token actions.
 */
static CompilationStatus _tokenLexemeAction(TokenLabel label) {
	Token * token = createToken(_lexicalAnalyzer, label);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

/**
 * BoardSim lexeme actions - all delegate to common implementation
 */

CompilationStatus BoardSimKeywordLexemeAction(TokenLabel label) {
	return _tokenLexemeAction(label);
}

CompilationStatus ComparisonOperatorLexemeAction(TokenLabel label) {
	return _tokenLexemeAction(label);
}

CompilationStatus DelimiterLexemeAction(TokenLabel label) {
	return _tokenLexemeAction(label);
}

CompilationStatus IdentifierLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, IDENTIFIER);
	// Store the identifier string in semantic value
	token->semanticValue->token = IDENTIFIER;
	
	// Store the actual identifier string from the token's lexeme
	token->semanticValue->string = strdup(token->lexeme);
	
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

// String buffer for accumulating string content (Flex context approach)
static char* _stringBuffer = NULL;
static int _stringBufferLen = 0;
static int _stringBufferCap = 0;

static void _initStringBuffer() {
	_stringBufferCap = 256;
	_stringBuffer = calloc(_stringBufferCap, sizeof(char));
	_stringBufferLen = 0;
}

static void _appendToStringBuffer(const char* text, int len) {
	if (_stringBuffer == NULL) _initStringBuffer();
	
	// Grow buffer if needed
	while (_stringBufferLen + len + 1 > _stringBufferCap) {
		_stringBufferCap *= 2;
		_stringBuffer = realloc(_stringBuffer, _stringBufferCap);
	}
	
	memcpy(_stringBuffer + _stringBufferLen, text, len);
	_stringBufferLen += len;
	_stringBuffer[_stringBufferLen] = '\0';
}

static char* _finalizeStringBuffer() {
	char* result = _stringBuffer ? strdup(_stringBuffer) : strdup("");
	if (_stringBuffer) {
		free(_stringBuffer);
		_stringBuffer = NULL;
	}
	_stringBufferLen = 0;
	_stringBufferCap = 0;
	return result;
}

CompilationStatus EnterStringLexemeAction(FlexContext context) {
	// Start collecting string content (opening quote already consumed by Flex)
	_initStringBuffer();
	enterLexicalAnalyzerContext(_lexicalAnalyzer, context);
	return IN_PROGRESS;
}

CompilationStatus AppendStringLexemeAction() {
	// Append matched text to string buffer (Flex handles the content, no manual parsing!)
	Token * token = createToken(_lexicalAnalyzer, IGNORED);
	_appendToStringBuffer(token->lexeme, token->length);
	destroyToken(token);
	return IN_PROGRESS;
}

CompilationStatus LeaveStringLexemeAction() {
	// Closing quote found - create token with accumulated content
	leaveLexicalAnalyzerContext(_lexicalAnalyzer);
	
	Token * token = createToken(_lexicalAnalyzer, STRING_LITERAL);
	token->semanticValue->token = STRING_LITERAL;
	
	// String content is already clean (no quotes!) thanks to Flex context
	token->semanticValue->string = _finalizeStringBuffer();
	
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}
