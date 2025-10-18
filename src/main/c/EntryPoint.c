#include "backend/code-generation/Generator.h"
#include "backend/domain-specific/Calculator.h"
#include "frontend/Frontend.h"
#include "frontend/lexical-analysis/FlexActions.h"
#include "frontend/syntactic-analysis/BisonActions.h"
#include "frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "frontend/semantic-analysis/SemanticAnalyzer.h"
#include "support/logging/Logger.h"
#include "support/type/CompilationStatus.h"
#include "support/type/CompilerState.h"
#include "support/type/ModuleDestructor.h"

// No global counters needed for Calculator

/**
 * The main entry-point of the entire application. If you use "strtok" to
 * parse anything inside this project instead of using Flex and Bison, I will
 * find you, and I will kill you (Bryan Mills; "Taken", 2008).
 */
const int main(const int length, const char ** arguments) {
	// Main function started
	
	// Check arguments
	if (length != 3) {
		printf("ERROR: Expected 3 arguments (program, input.bsim, output.txt), got %d\n", length);
		return 1;
	}
	
	// Input and output files configured
	
	// Open output file for writing
	FILE* outputFile = fopen(arguments[2], "w");
	if (outputFile == NULL) {
		printf("ERROR: Could not open output file: %s\n", arguments[2]);
		return 1;
	}
	
	// Redirect stdout to output file
	FILE* originalStdout = stdout;
	stdout = outputFile;
	
	LexicalAnalyzer * lexicalAnalyzer = createLexicalAnalyzer();
	if (lexicalAnalyzer == NULL) {
		printf("ERROR: Could not create lexical analyzer\n");
		stdout = originalStdout;
		fclose(outputFile);
		return 1;
	}
	// Lexical analyzer created
	
	// Configure input file
	InputBuffer * inputBuffer = createInputBuffer(lexicalAnalyzer, arguments[1]);
	if (inputBuffer == NULL || inputBuffer->file == NULL) {
		printf("ERROR: Could not open input file: %s\n", arguments[1]);
		stdout = originalStdout;
		fclose(outputFile);
		destroyLexicalAnalyzer(lexicalAnalyzer);
		return 1;
	}
	yy_switch_to_buffer(inputBuffer->buffer, lexicalAnalyzer->scanner);
	// Input buffer configured
	
	Logger * logger = createLogger("EntryPoint");
	// Logger created
	for (int k = 0; k < length; ++k) {
		logDebugging(logger, "Argument %d: \"%s\"", k, arguments[k]);
	}
	// Arguments logged
	CompilerState compilerState = {
		.abstractSyntaxtTree = NULL,
		.value = 0,
		.symbolTable = { .count = 0 }
	};
	// Compiler state initialized
	ModuleDestructor moduleDestructors[] = {
		initializeAbstractSyntaxTreeModule(),
		initializeFlexActionsModule(lexicalAnalyzer),
		initializeBisonActionsModule(&compilerState),
		initializeFrontendModule(lexicalAnalyzer),
		initializeSemanticAnalyzer(),
		initializeCalculatorModule(),
		initializeGeneratorModule()
	};
	// Modules initialized
	// Starting syntactic analysis
	CompilationStatus compilationStatus = executeSyntacticAnalysis();
	// Syntactic analysis completed
	if (compilationStatus == SUCCEEDED) {
		// Compilation succeeded, entering backend
		// ----------------------------------------------------------------------------------------
		// Beginning of the Backend... ------------------------------------------------------------
		logDebugging(logger, "Computing expression value...");
		ComputationResult computationResult = executeCalculator(&compilerState);
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
	// Note: For Stage II, we skip AST cleanup to avoid segfaults with BoardSim syntax
	// The OS will clean up memory when the process exits
	// if (compilationStatus == SUCCEEDED && compilerState.abstractSyntaxtTree != NULL) {
	// 	// Calculator program - destroy as Program
	// 	Program* program = (Program*)compilerState.abstractSyntaxtTree;
	// 	if (program != NULL) {
	// 		destroyProgram(program);
	// 	}
	// }
	for (int k = (sizeof(moduleDestructors)/sizeof(ModuleDestructor)) - 1; 0 <= k; --k) {
		moduleDestructors[k]();
	}
	logDebugging(logger, "Compilation is done.");
	destroyLogger(logger);
	destroyLexicalAnalyzer(lexicalAnalyzer);
	
	// Restore stdout and close output file
	stdout = originalStdout;
	fclose(outputFile);
	
	return compilationStatus;
}
