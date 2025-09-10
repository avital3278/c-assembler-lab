#ifndef DATA_DIRECTIVES_H
#define DATA_DIRECTIVES_H

#include "data_strct.h"

/**
 * Parses and encodes values from a `.data` directive.
 * Handles comma-separated integers with optional whitespace.
 * Reports errors for invalid numbers, empty entries, and trailing commas.
 *
 * @param line The full .data directive line (may include label).
 * @param IC The instruction counter (used to offset address).
 * @param DC The data counter (relative offset from IC).
 * @param memory_words The memory array where encoded words will be written.
 * @param error_flag Pointer to an integer set to 1 if an error occurs.
 * @return The number of words written, or 0 if an error occurred.
 */
int handle_data_directive(const char *line, int IC, int DC, CodeLine *memory_words, int *error_flag);

/**
 * Handles the .string directive.
 * Converts a quoted string into ASCII words and stores them in the memory array.
 *
 * @param line         The full source line containing the .string directive.
 * @param IC           Instruction counter (used as base address).
 * @param DC           Current Data Counter value.
 * @param memory_words Output memory array to store encoded data.
 * @param error_flag   Pointer to error flag, set to 1 in case of a syntax error.
 * @return             Number of memory words written (including null terminator).
 */
int handle_string_directive(const char *line, int IC, int DC, CodeLine *memory_words, int *error_flag);

/**
 * Handles the .mat directive (matrix of integers).
 * Parses dimensions and initial values, writes to memory.
 *
 * @param line         The full source line.
 * @param IC           Instruction counter.
 * @param DC           Data counter.
 * @param memory_words Memory array for writing output.
 * @param error_flag   Pointer to error flag.
 * @return             Number of memory words written.
 */
int handle_mat_directive(const char *line, int IC, int DC, CodeLine *memory_words, int *error_flag);

#endif

