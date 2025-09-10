#ifndef ERRORS_H
#define ERRORS_H

#include "data_strct.h"  /* For location struct */

/* Error codes */
#define ERROR_CODE_1		1	/* Failed to open source file */
#define ERROR_CODE_2		2	/* Failed to seek to macro start */
#define ERROR_CODE_3		3	/* Empty macro body */
#define ERROR_CODE_4		4	/* Extra text after 'mcroend' */
#define ERROR_CODE_5		5	/* Memory allocation failed */
#define ERROR_CODE_8		8	/* Failed to open file (general) */
#define ERROR_CODE_9		9	/* Missing macro name */
#define ERROR_CODE_10		10	/* Extra text after macro name */
#define ERROR_CODE_11		11	/* Failed to get file position */
#define ERROR_CODE_12		12	/* Line too long (exceeds 80 chars) */
#define ERROR_CODE_16		16	/* Macro used before declaration */
#define ERROR_CODE_17		17	/* Invalid or reserved macro name */
#define ERROR_CODE_18		18	/* Duplicate macro name */

#define ERR_CODE_19		19	/* Failed to allocate memory for label list */
#define ERR_CODE_20		20	/* Failed to allocate memory for label name */
#define ERR_CODE_21		21	/* Duplicate label name */
#define ERR_CODE_22		22	/* Label used in instruction but not defined */
#define ERR_CODE_23		23	/* Illegal label name */

/* Function Declarations */

/**
 * @brief Prints an internal system-level error message based on the error code.
 * @param code The internal error code.
 */
void print_internal_error(int code);

/**
 * @brief Prints an external error message with file and line context.
 * @param code The external error code.
 * @param loc  The location (filename and line) of the error.
 */
void print_external_error(int code, location loc);

/**
 * @brief Prints an internal error with a filename context.
 * @param code The internal error code.
 * @param file_name The file where the error occurred.
 */
void print_error_with_file(int code, const char *file_name);

/**
 * @brief Prints a general error message (no location info).
 * @param message The error message to print.
 */
void print_error(const char *message);

/**
 * @brief Reports an error with specific filename and line number.
 * @param file_name The name of the source file.
 * @param line_number The line where the error occurred.
 * @param message The error message to print.
 */
void report_error(const char *file_name, int line_number, const char *message);

#endif /* ERRORS_H */

