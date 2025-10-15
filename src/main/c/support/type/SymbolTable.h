#ifndef SYMBOL_TABLE_HEADER
#define SYMBOL_TABLE_HEADER

#include <stdbool.h>
#include "CompilerState.h"
#include "CompilationStatus.h"

/**
 * Symbol table management functions for BoardSim semantic analysis.
 */

/**
 * Add a symbol to the symbol table.
 */
CompilationStatus addSymbol(SymbolTable* table, const char* name, Type type, void* value, int line);

/**
 * Look up a symbol in the symbol table.
 */
VarInfo* findSymbol(SymbolTable* table, const char* name);

/**
 * Check if a symbol already exists in the symbol table.
 */
bool symbolExists(SymbolTable* table, const char* name);

/**
 * Clear all symbols from the symbol table.
 */
void clearSymbolTable(SymbolTable* table);

/**
 * Print the symbol table for debugging.
 */
void printSymbolTable(SymbolTable* table);

#endif
