#include "backend/code-generation/Generator.h"
#include "backend/domain-specific/BoardSim.h"
#include "frontend/Frontend.h"
#include "frontend/lexical-analysis/FlexActions.h"
#include "frontend/syntactic-analysis/BisonActions.h"
#include "frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "frontend/semantic-analysis/SemanticAnalyzer.h"
#include "support/logging/Logger.h"
#include "support/type/CompilationStatus.h"
#include "support/type/CompilerState.h"
#include "support/type/ModuleDestructor.h"

// Forward declaration of global counters from BoardSim
extern int g_parsedPlayers;
extern int g_parsedDice;
extern int g_parsedBoards;

/**
 * The main entry-point of the entire application. If you use "strtok" to
 * parse anything inside this project instead of using Flex and Bison, I will
 * find you, and I will kill you (Bryan Mills; "Taken", 2008).
 */
const int main(const int length, const char ** arguments) {
	LexicalAnalyzer * lexicalAnalyzer = createLexicalAnalyzer();
	Logger * logger = createLogger("EntryPoint");
	for (int k = 0; k < length; ++k) {
		logDebugging(logger, "Argument %d: \"%s\"", k, arguments[k]);
	}
	CompilerState compilerState = {
		.abstractSyntaxtTree = NULL,
		.value = 0,
		.symbolTable = { .count = 0 }
	};
	ModuleDestructor moduleDestructors[] = {
		initializeAbstractSyntaxTreeModule(),
		initializeFlexActionsModule(lexicalAnalyzer),
		initializeBisonActionsModule(&compilerState),
		initializeFrontendModule(lexicalAnalyzer),
		initializeSemanticAnalyzer(),
		initializeBoardSimModule(),
		initializeGeneratorModule()
	};
	CompilationStatus compilationStatus = executeSyntacticAnalysis();
	if (compilationStatus == SUCCEEDED) {
		// ----------------------------------------------------------------------------------------
		// Beginning of the Backend... ------------------------------------------------------------
		logDebugging(logger, "Computing expression value...");

		logDebugging(logger, "Calling executeBoardSim...");
		ComputationResult computationResult = executeBoardSim(&compilerState);
		if (computationResult.succeeded) {
			compilerState.value = computationResult.value;
			executeGenerator(&compilerState);
		}
		else {
			logError(logger, "The computation phase rejects the input program.");
			compilationStatus = FAILED;
		}
		// ...end of the Backend. -----------------------------------------------------------------
		// ----------------------------------------------------------------------------------------
	}
	else {
		logError(logger, "The syntactic-analysis phase rejects the input program.");
		compilationStatus = FAILED;
	}
	logDebugging(logger, "Releasing AST resources...");
	// Clean up AST if it exists and compilation succeeded
	if (compilationStatus == SUCCEEDED && compilerState.abstractSyntaxtTree != NULL) {
		// Check if this is a BoardSim program or Calculator program
		if (g_parsedBoards > 0 || g_parsedPlayers > 0 || g_parsedDice > 0) {
			// BoardSim program - destroy as ASTNode
			ASTNode* rootNode = (ASTNode*)compilerState.abstractSyntaxtTree;
			if (rootNode != NULL) {
				destroyASTNode(rootNode);
			}
		} else {
			// Calculator program - destroy as Program
			Program* program = (Program*)compilerState.abstractSyntaxtTree;
			if (program != NULL) {
				destroyProgram(program);
			}
		}
	}
	for (int k = (sizeof(moduleDestructors)/sizeof(ModuleDestructor)) - 1; 0 <= k; --k) {
		moduleDestructors[k]();
	}
	logDebugging(logger, "Compilation is done.");
	destroyLogger(logger);
	destroyLexicalAnalyzer(lexicalAnalyzer);
	return compilationStatus;
}
