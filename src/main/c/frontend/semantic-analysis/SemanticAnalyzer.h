#ifndef SEMANTIC_ANALYZER_HEADER
#define SEMANTIC_ANALYZER_HEADER

#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/CompilationStatus.h"
#include "../../support/type/CompilerState.h"

ModuleDestructor initializeSemanticAnalyzer();

CompilationStatus executeSemanticAnalysis(CompilerState * compilerState);

/**
 * BoardSim specific semantic analysis.
 */
CompilationStatus analyzeBoardSimProgram(CompilerState * compilerState);

/**
 * BoardSim specific validation functions.
 */
CompilationStatus validateBoardSimRules(CompilerState * compilerState);
CompilationStatus validateBoardConfiguration(CompilerState * compilerState);
CompilationStatus validatePlayerConfiguration(CompilerState * compilerState);

#endif