#include "SemanticAnalyzer.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/SymbolTable.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/logging/Logger.h"
#include <string.h>

static Logger * _logger = NULL;

// Forward declarations for AST traversal
static CompilationStatus analyzeNode(ASTNode* node, CompilerState* compilerState);
static CompilationStatus analyzeStatement(Statement* stmt, CompilerState* compilerState);
static CompilationStatus analyzeCondition(Condition* cond, CompilerState* compilerState);

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
	
	if (compilerState == NULL || compilerState->abstractSyntaxtTree == NULL) {
		logError(_logger, "No AST available for semantic analysis");
		return FAILED;
	}
	
	// Clear symbol table for fresh analysis
	if (compilerState->symbolTable != NULL) {
		clearSymbolTable(compilerState->symbolTable);
	}
	
	// Traverse the AST and validate semantics
	ASTNode* root = (ASTNode*)compilerState->abstractSyntaxtTree;
	ASTNode* current = root;
	
	bool hasBoard = false;
	bool hasPlayer = false;
	int boardCount = 0;
	int playerCount = 0;
	
	while (current != NULL) {
		// First pass: count and validate declarations
		if (current->nodeType == NODE_TYPE_BOARD_DEF) {
			boardCount++;
			hasBoard = true;
		} else if (current->nodeType == NODE_TYPE_PLAYER_DEF) {
			playerCount++;
			hasPlayer = true;
		}
		current = current->next;
	}
	
	// Reset for second pass
	current = root;
	
	// Second pass: full semantic analysis
	while (current != NULL) {
		if (analyzeNode(current, compilerState) == FAILED) {
			return FAILED;
		}
		current = current->next;
	}
	
	// Validation rules
	
	// Rule 1: Must have at least one board
	if (!hasBoard) {
		logError(_logger, "Semantic Error: No board definition found. A BoardSim program requires at least one board.");
		return FAILED;
	}
	
	// Rule 2: Cannot have more than one board
	if (boardCount > 1) {
		logError(_logger, "Semantic Error: Multiple board definitions found (%d). Only one board is allowed.", boardCount);
		return FAILED;
	}
	
	// Rule 3: Must have at least one player for meaningful simulation
	if (!hasPlayer) {
		logError(_logger, "Semantic Error: No player definition found. A BoardSim program requires at least one player.");
		return FAILED;
	}
	
	// Rule 4: Maximum 8 players
	if (playerCount > 8) {
		logError(_logger, "Semantic Error: Too many players (%d). Maximum is 8 players.", playerCount);
		return FAILED;
	}
	
	logDebugging(_logger, "Semantic analysis completed successfully");
	logDebugging(_logger, "Found: %d board(s), %d player(s)", boardCount, playerCount);
	
	if (compilerState->symbolTable != NULL) {
		logDebugging(_logger, "Symbol table contains %d symbols", compilerState->symbolTable->count);
	}
	
	return SUCCEEDED;
}

static CompilationStatus analyzeNode(ASTNode* node, CompilerState* compilerState) {
	if (node == NULL || node->data == NULL) {
		return SUCCEEDED;
	}
	
	switch (node->nodeType) {
		case NODE_TYPE_BOARD_DEF: {
			BoardDef* board = (BoardDef*)node->data;
			
			// Rule: Board must have valid ID
			if (board->id == NULL || strlen(board->id) == 0) {
				logError(_logger, "Semantic Error: Board definition has empty or NULL identifier");
				return FAILED;
			}
			
			// Rule: No duplicate board IDs
			if (compilerState->symbolTable != NULL && symbolExists(compilerState->symbolTable, board->id)) {
				logError(_logger, "Semantic Error: Duplicate board definition '%s'", board->id);
				return FAILED;
			}
			
			// Rule: Board size must be non-negative
			if (board->size < 0) {
				logError(_logger, "Semantic Error: Board '%s' has invalid size %d", board->id, board->size);
				return FAILED;
			}
			
			// Add board to symbol table
			if (compilerState->symbolTable != NULL) {
				addSymbol(compilerState->symbolTable, board->id, TYPE_BOARD, board, 0);
			}
			
			logDebugging(_logger, "Validated board: %s (%s, %d cells)", board->id, board->type, board->size);
			break;
		}
		
		case NODE_TYPE_PLAYER_DEF: {
			PlayerDef* player = (PlayerDef*)node->data;
			
			// Rule: Player ID must be positive
			if (player->id <= 0) {
				logError(_logger, "Semantic Error: Player ID must be positive, got %d", player->id);
				return FAILED;
			}
			
			// Rule: Player money cannot be negative
			if (player->money < 0) {
				logError(_logger, "Semantic Error: Player %d has negative money %d", player->id, player->money);
				return FAILED;
			}
			
			// Rule: Player position cannot be negative
			if (player->position < 0) {
				logError(_logger, "Semantic Error: Player %d has negative position %d", player->id, player->position);
				return FAILED;
			}
			
			// Check for duplicate player IDs
			char playerName[32];
			snprintf(playerName, sizeof(playerName), "player_%d", player->id);
			if (compilerState->symbolTable != NULL && symbolExists(compilerState->symbolTable, playerName)) {
				logError(_logger, "Semantic Error: Duplicate player ID %d", player->id);
				return FAILED;
			}
			
			// Add player to symbol table
			if (compilerState->symbolTable != NULL) {
				addSymbol(compilerState->symbolTable, playerName, TYPE_PLAYER, player, 0);
			}
			
			logDebugging(_logger, "Validated player: %d (money=%d, position=%d)", player->id, player->money, player->position);
			break;
		}
		
		case NODE_TYPE_CELL_DEF: {
			CellDef* cell = (CellDef*)node->data;
			
			// Rule: Cell index must be non-negative
			if (cell->index < 0) {
				logError(_logger, "Semantic Error: Cell index must be non-negative, got %d", cell->index);
				return FAILED;
			}
			
			logDebugging(_logger, "Validated cell: %d (%s)", cell->index, cell->name ? cell->name : "unnamed");
			break;
		}
		
		case NODE_TYPE_DICE_DEF: {
			DiceDef* dice = (DiceDef*)node->data;
			
			// Rule: Dice must have at least 2 sides
			if (dice->sides < 2) {
				logError(_logger, "Semantic Error: Dice must have at least 2 sides, got %d", dice->sides);
				return FAILED;
			}
			
			logDebugging(_logger, "Validated dice: %d sides", dice->sides);
			break;
		}
		
		case NODE_TYPE_SIMULATE_BLOCK: {
			SimulateBlock* block = (SimulateBlock*)node->data;
			
			// Rule: Simulate block must have at least 1 turn
			if (block->turns < 1) {
				logError(_logger, "Semantic Error: Simulate block must have at least 1 turn, got %d", block->turns);
				return FAILED;
			}
			
			// Analyze statements within the simulate block
			ASTNode* stmtNode = block->statements;
			while (stmtNode != NULL) {
				if (stmtNode->nodeType == NODE_TYPE_STATEMENT && stmtNode->data != NULL) {
					if (analyzeStatement((Statement*)stmtNode->data, compilerState) == FAILED) {
						return FAILED;
					}
				}
				stmtNode = stmtNode->next;
			}
			
			logDebugging(_logger, "Validated simulate block: %d turns", block->turns);
			break;
		}
		
		default:
			// Unknown node types are ignored (forward compatibility)
			break;
	}
	
	return SUCCEEDED;
}

static CompilationStatus analyzeStatement(Statement* stmt, CompilerState* compilerState) {
	if (stmt == NULL) {
		return SUCCEEDED;
	}
	
	switch (stmt->type) {
		case STATEMENT_VARIABLE_DECL: {
			Variable* var = stmt->data.variable;
			if (var == NULL || var->name == NULL) {
				logError(_logger, "Semantic Error: Variable declaration has NULL name");
				return FAILED;
			}
			
			// Rule: No duplicate variable names
			if (compilerState->symbolTable != NULL && symbolExists(compilerState->symbolTable, var->name)) {
				logError(_logger, "Semantic Error: Variable '%s' already declared", var->name);
				return FAILED;
			}
			
			// Add variable to symbol table
			if (compilerState->symbolTable != NULL) {
				Type varType = TYPE_INT; // Simplified - could map VAR_TYPE_* to Type
				addSymbol(compilerState->symbolTable, var->name, varType, NULL, 0);
			}
			
			logDebugging(_logger, "Validated variable: %s", var->name);
			break;
		}
		
		case STATEMENT_IF:
		case STATEMENT_IF_ELSE: {
			if (stmt->data.conditional != NULL) {
				// Validate condition
				if (analyzeCondition(stmt->data.conditional->condition, compilerState) == FAILED) {
					return FAILED;
				}
				
				// Recursively analyze if body
				if (stmt->data.conditional->ifBody != NULL) {
					if (analyzeStatement(stmt->data.conditional->ifBody, compilerState) == FAILED) {
						return FAILED;
					}
				}
				
				// Recursively analyze else body (if present)
				if (stmt->data.conditional->elseBody != NULL) {
					if (analyzeStatement(stmt->data.conditional->elseBody, compilerState) == FAILED) {
						return FAILED;
					}
				}
			}
			break;
		}
		
		case STATEMENT_FOR: {
			if (stmt->data.loop != NULL) {
				LoopStatement* loop = stmt->data.loop;
				
				// Rule: For loop range must be valid (start <= end)
				if (loop->rangeStart > loop->rangeEnd) {
					logError(_logger, "Semantic Error: For loop has invalid range (%d to %d)", 
							 loop->rangeStart, loop->rangeEnd);
					return FAILED;
				}
				
				// Recursively analyze body
				if (loop->body != NULL) {
					if (analyzeStatement(loop->body, compilerState) == FAILED) {
						return FAILED;
					}
				}
			}
			break;
		}
		
		case STATEMENT_WHILE: {
			if (stmt->data.loop != NULL) {
				// Validate condition
				if (analyzeCondition(stmt->data.loop->condition, compilerState) == FAILED) {
					return FAILED;
				}
				
				// Recursively analyze body
				if (stmt->data.loop->body != NULL) {
					if (analyzeStatement(stmt->data.loop->body, compilerState) == FAILED) {
						return FAILED;
					}
				}
			}
			break;
		}
		
		case STATEMENT_PRINT:
		case STATEMENT_LOG:
			// Print/Log statements are always valid
			break;
		
		default:
			break;
	}
	
	return SUCCEEDED;
}

static CompilationStatus analyzeCondition(Condition* cond, CompilerState* compilerState) {
	if (cond == NULL) {
		return SUCCEEDED;
	}
	
	if (cond->type == CONDITION_COMPARISON && cond->comparison != NULL) {
		ComparisonExpression* expr = cond->comparison;
		char* varName = expr->leftOperand;
		
		if (varName == NULL) {
			logError(_logger, "Semantic Error: Condition has NULL left operand");
			return FAILED;
		}
		
		// Built-in variables that are always valid
		bool isBuiltIn = (
			strcmp(varName, "score") == 0 ||
			strcmp(varName, "turn") == 0 ||
			strcmp(varName, "money") == 0 ||
			strcmp(varName, "active") == 0 ||
			strcmp(varName, "players") == 0 ||
			strcmp(varName, "_literal") == 0  // Internal use for integer conditions
		);
		
		// Validate that variable exists (built-in or declared)
		if (!isBuiltIn && compilerState->symbolTable != NULL) {
			if (!symbolExists(compilerState->symbolTable, varName)) {
				logError(_logger, "Semantic Error: Undefined variable '%s' in condition", varName);
				return FAILED;
			}
		}
		
		logDebugging(_logger, "Validated condition: %s (operator=%d, value=%d)", 
					 varName, expr->op, expr->rightOperand);
	}
	else if (cond->type == CONDITION_IDENTIFIER && cond->identifier != NULL) {
		char* varName = cond->identifier;
		
		// Built-in booleans
		bool isBuiltIn = (
			strcmp(varName, "active") == 0 ||
			strcmp(varName, "score") == 0
		);
		
		// Validate that boolean variable exists
		if (!isBuiltIn && compilerState->symbolTable != NULL) {
			if (!symbolExists(compilerState->symbolTable, varName)) {
				logError(_logger, "Semantic Error: Undefined boolean variable '%s' in condition", varName);
				return FAILED;
			}
		}
		
		logDebugging(_logger, "Validated identifier condition: %s", varName);
	}
	
	return SUCCEEDED;
}

// Keep these for backwards compatibility (they now just delegate to main function)
CompilationStatus analyzeBoardSimProgram(CompilerState * compilerState) {
	return executeSemanticAnalysis(compilerState);
}

CompilationStatus validateBoardSimRules(CompilerState * compilerState) {
	// Now handled in main executeSemanticAnalysis
	return SUCCEEDED;
}

CompilationStatus validateBoardConfiguration(CompilerState * compilerState) {
	// Now handled in main executeSemanticAnalysis
	return SUCCEEDED;
}

CompilationStatus validatePlayerConfiguration(CompilerState * compilerState) {
	// Now handled in main executeSemanticAnalysis
	return SUCCEEDED;
}
