#include "SemanticAnalyzer.h"
#include "../../support/type/CompilerState.h"
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

CompilationStatus executeSemanticAnalysis(CompilerState * state) {
    // TODO: Traverse AST to populate symbolTable and check types
    logDebugging(_logger, "Performing semantic analysis...");
    // For now, dummy success
    return SUCCEEDED;
}