#ifndef COMPILER_STATE_HEADER
#define COMPILER_STATE_HEADER

/**
 * Type enumeration for BoardSim domain (based on PDF sections 6-8)
 */
typedef enum Type { 
	TYPE_INT, 
	TYPE_STRING, 
	TYPE_BOOL, 
	TYPE_ARRAY, 
	TYPE_BOARD, 
	TYPE_PIECE, 
	TYPE_PLAYER, 
	TYPE_EVENT, 
	TYPE_DICE, 
	TYPE_RULE 
} Type;

/**
 * Variable information for symbol table
 */
typedef struct {
	char* name;
	Type type;
	void* value;
	int line;  // For error reporting
} VarInfo;

/**
 * Symbol table for tracking variables and their types (static typing)
 */
typedef struct {
	VarInfo* entries[100];  // Simple array, will expand to hash in Paso 4
	int count;
} SymbolTable;

/**
 * The global state of the compiler. Should transport every data structure
 * needed across the different phases of a compilation.
 */
typedef struct {
	/**
	 * The root node of the AST.
	 */
	void * abstractSyntaxtTree;

	/**
	 * The computed value of the entire program (only for the calculator). You
	 * should change or remove this field, or a random child will die, and it
	 * will be your fault.
	 */
	signed int value;

	/**
	 * Symbol table for static type checking (pointer for proper memory management)
	 */
	SymbolTable * symbolTable;
} CompilerState;

#endif
