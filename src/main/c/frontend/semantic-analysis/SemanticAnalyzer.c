#include "SemanticAnalyzer.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/SymbolTable.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/logging/Logger.h"

static Logger * _logger = NULL;

void _shutdownSemanticAnalyzer() {
    if (_logger) {
        logDebugging(_logger, "Destroying module: SemanticAnalyzer...");
        destroyLogger(_logger);
        _logger = NULL;
    }
}

ModuleDestructor initializeSemanticAnalyzer() {
    _logger = createLogger("SemanticAnalyzer");
    return _shutdownSemanticAnalyzer;
}

CompilationStatus executeSemanticAnalysis(CompilerState * compilerState) {
	logDebugging(_logger, "Executing semantic analysis...");
	
	Program * program = compilerState->abstractSyntaxtTree;
	if (program == NULL) {
		logError(_logger, "No AST available for semantic analysis");
		return FAILED;
	}
	
	// Check if this is a BoardSim program vs Calculator program
	if (program->expression == NULL) {
		// BoardSim program - perform BoardSim semantic analysis
		logDebugging(_logger, "Analyzing BoardSim program semantics...");
		return analyzeBoardSimProgram(compilerState);
	} else {
		// Calculator program - no semantic analysis needed for now
		logDebugging(_logger, "Calculator program - skipping semantic analysis");
		return SUCCEEDED;
	}
}

CompilationStatus analyzeBoardSimProgram(CompilerState * compilerState) {
	logDebugging(_logger, "Starting BoardSim semantic analysis...");
	
	// Clear the symbol table before analysis
	clearSymbolTable(&compilerState->symbolTable);
	
	// For demonstration purposes, we'll simulate basic semantic analysis
	// In a real implementation, we would traverse the AST and populate the symbol table
	// based on actual declarations found in the parsed program
	
	// Since we don't have full AST traversal yet, we'll simulate it
	// by checking what type of declaration we detected during parsing
	logDebugging(_logger, "Simulating semantic analysis based on parse results...");
	
	CompilationStatus result = SUCCEEDED;
	
	// Perform additional BoardSim-specific validations
	result = validateBoardSimRules(compilerState);
	if (result != SUCCEEDED) {
		logError(_logger, "BoardSim rules validation failed");
		return FAILED;
	}
	
	result = validateBoardConfiguration(compilerState);
	if (result != SUCCEEDED) {
		logError(_logger, "Board configuration validation failed");
		return FAILED;
	}
	
	result = validatePlayerConfiguration(compilerState);
	if (result != SUCCEEDED) {
		logError(_logger, "Player configuration validation failed");
		return FAILED;
	}
	
	logDebugging(_logger, "BoardSim semantic analysis completed successfully");
	logDebugging(_logger, "Symbol table contains %d symbols", compilerState->symbolTable.count);
	
	return SUCCEEDED;
}

CompilationStatus validateBoardSimRules(CompilerState * compilerState) {
	logDebugging(_logger, "Validating BoardSim rules...");
	
	// Rule 1: Must have at least one board
	VarInfo* board = NULL;
	for (int i = 0; i < compilerState->symbolTable.count; i++) {
		if (compilerState->symbolTable.entries[i] && 
			compilerState->symbolTable.entries[i]->type == TYPE_BOARD) {
			board = compilerState->symbolTable.entries[i];
			break;
		}
	}
	
	if (board == NULL) {
		logError(_logger, "BoardSim programs must declare at least one board");
		return FAILED;
	}
	
	// Rule 2: Must have at least one player for games
	bool hasPlayer = false;
	for (int i = 0; i < compilerState->symbolTable.count; i++) {
		if (compilerState->symbolTable.entries[i] && 
			compilerState->symbolTable.entries[i]->type == TYPE_PLAYER) {
			hasPlayer = true;
			break;
		}
	}
	
	if (!hasPlayer) {
		logDebugging(_logger, "Warning: No players declared (simulation may not be meaningful)");
	}
	
	logDebugging(_logger, "BoardSim rules validation passed");
	return SUCCEEDED;
}

CompilationStatus validateBoardConfiguration(CompilerState * compilerState) {
	logDebugging(_logger, "Validating board configuration...");
	
	// Validate that board declarations are consistent
	int boardCount = 0;
	for (int i = 0; i < compilerState->symbolTable.count; i++) {
		if (compilerState->symbolTable.entries[i] && 
			compilerState->symbolTable.entries[i]->type == TYPE_BOARD) {
			boardCount++;
		}
	}
	
	if (boardCount > 1) {
		logError(_logger, "Multiple board declarations not supported in this version");
		return FAILED;
	}
	
	logDebugging(_logger, "Board configuration validation passed");
	return SUCCEEDED;
}

CompilationStatus validatePlayerConfiguration(CompilerState * compilerState) {
	logDebugging(_logger, "Validating player configuration...");
	
	// Check player declarations for consistency
	int playerCount = 0;
	for (int i = 0; i < compilerState->symbolTable.count; i++) {
		if (compilerState->symbolTable.entries[i] && 
			compilerState->symbolTable.entries[i]->type == TYPE_PLAYER) {
			playerCount++;
		}
	}
	
	if (playerCount > 8) {
		logError(_logger, "Too many players declared (maximum 8 supported)");
		return FAILED;
	}
	
	logDebugging(_logger, "Player configuration validation passed (%d players)", playerCount);
	return SUCCEEDED;
}