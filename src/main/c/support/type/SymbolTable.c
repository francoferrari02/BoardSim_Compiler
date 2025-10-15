#include "SymbolTable.h"
#include "../logging/Logger.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

static Logger * _logger = NULL;

CompilationStatus addSymbol(SymbolTable* table, const char* name, Type type, void* value, int line) {
    if (table == NULL || name == NULL) {
        return FAILED;
    }
    
    if (!_logger) {
        _logger = createLogger("SymbolTable");
    }
    
    // Check if symbol already exists
    if (symbolExists(table, name)) {
        logError(_logger, "Symbol '%s' already declared at line %d", name, line);
        return FAILED;
    }
    
    // Check if we have space
    if (table->count >= 100) {
        logError(_logger, "Symbol table overflow - too many symbols");
        return FAILED;
    }
    
    // Create new VarInfo
    VarInfo* varInfo = (VarInfo*)calloc(1, sizeof(VarInfo));
    varInfo->name = strdup(name);
    varInfo->type = type;
    varInfo->value = value;
    varInfo->line = line;
    
    // Add to table
    table->entries[table->count] = varInfo;
    table->count++;
    
    logDebugging(_logger, "Added symbol '%s' of type %d at line %d", name, type, line);
    return SUCCEEDED;
}

VarInfo* findSymbol(SymbolTable* table, const char* name) {
    if (table == NULL || name == NULL) {
        return NULL;
    }
    
    for (int i = 0; i < table->count; i++) {
        if (table->entries[i] && strcmp(table->entries[i]->name, name) == 0) {
            return table->entries[i];
        }
    }
    
    return NULL;
}

bool symbolExists(SymbolTable* table, const char* name) {
    return findSymbol(table, name) != NULL;
}

void clearSymbolTable(SymbolTable* table) {
    if (table == NULL) {
        return;
    }
    
    for (int i = 0; i < table->count; i++) {
        if (table->entries[i]) {
            if (table->entries[i]->name) {
                free(table->entries[i]->name);
            }
            free(table->entries[i]);
            table->entries[i] = NULL;
        }
    }
    
    table->count = 0;
}

void printSymbolTable(SymbolTable* table) {
    if (!_logger) {
        _logger = createLogger("SymbolTable");
    }
    
    if (table == NULL) {
        logDebugging(_logger, "Symbol table is NULL");
        return;
    }
    
    logDebugging(_logger, "Symbol Table (%d entries):", table->count);
    for (int i = 0; i < table->count; i++) {
        if (table->entries[i]) {
            logDebugging(_logger, "  [%d] %s (type: %d, line: %d)", 
                        i, table->entries[i]->name, table->entries[i]->type, table->entries[i]->line);
        }
    }
}
