#ifndef DATA_STRCT_H
#define DATA_STRCT_H

#define MAX_LABEL_LENGTH 30
#define MAX_OPERAND_LENGTH 31

/**
 * Structure representing a single macro definition.
 */
typedef struct macr {
	char *name;           /* Macro name */
	char *content;        /* Macro body (lines of code) */
	int line;             /* Line number where macro was defined */
	struct macr *next;    /* Pointer to the next macro in the list */
} macro_t;

/* Represents the location of an error in the file */
typedef struct {
	const char *file_name;
	int line_number;
} location;

/**
 * Represents a label defined in the assembly file (either code or data).
 */
typedef struct {
	int location;       /* The memory address of the label */
	char *name;         /* The name of the label */
	int line_index;     /* The line number in the assembly file */
	int type;           /* 0 = code, 1 = data */
} LabelEntry;

/**
 * Represents a label reference from .entry or .extern declarations.
 */
typedef struct {
	char *name;
	int line_index;
} EntryExternRef;

/**
 * Represents a single word in memory (instruction or operand).
 */
typedef struct {
	int value;          /* The encoded binary value */
	int is_label;       /* 1 if it contains a label reference */
	char *label_name;   /* Label name to be resolved later (if needed) */
} AsmWord;

/**
 * Represents a full line of code in memory (includes address and word).
 */
typedef struct {
	int address;        /* Instruction counter (IC) */
	AsmWord word;       /* The encoded machine word */
} CodeLine;

typedef enum {
	ADDR_IMMEDIATE = 0,  /* e.g., #5 */
	ADDR_DIRECT    = 1,  /* e.g., LABEL */
	ADDR_MATRIX    = 2,  /* e.g., MAT[r1][r2] */
	ADDR_REGISTER  = 3,  /* e.g., r3 */
	ADDR_NONE      = -1, /* No operand */
	ADDR_INVALID   = -2  /* Invalid operand */
} AddressType;

#endif /* DATA_STRCT_H */

