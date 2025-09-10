#ifndef MACRO_PROC_H
#define MACRO_PROC_H

#include <string.h>
#include <stdio.h>
#include "data_strct.h"  /* For macro_t definition */

/**
 * Parses the source file and builds a linked list of macros.
 * Validates macro names and prevents duplicates.
 *
 * @param fp           File pointer to the source file (opened for reading).
 * @param file_name    The name of the source file (for error reporting).
 * @param macro_head   Pointer to the head of the macro list to populate.
 * @return             1 if successful, 0 if errors occurred.
 */
int collect_macro_table(FILE *fp, macro_t **macro_head);

/**
 * Replaces all macro calls in the file with their corresponding content.
 * If a macro is preceded by a label, it is preserved and inserted
 * before the first line of the macro.
 *
 * @param src          Input file (after removing macro definitions).
 * @param dest         Output file to write the expanded code (.am file).
 * @param macro_list   The linked list of macros.
 * @param file_name    Name of the file (used for error reporting).
 */
void expand_macros_in_file(FILE *src, FILE *dest, macro_t *macro_list);

/**
 * Main preprocessing function that performs the macro handling:
 * - Reads macros from the source file
 * - Removes macro definitions
 * - Expands macros into a .am file
 *
 * @param source_filename  The name of the original .as source file.
 * @return                 1 if successful, 0 if errors occurred.
 */
int preprocess_macros(const char *source_filename);

/**
 * Frees the memory used by the macro linked list.
 *
 * @param head   The head of the macro list.
 */
void free_macro_table(macro_t *head);

/**
 * Checks if a macro is used before being defined.
 * (Used for optional validation and debugging.)
 *
 * @param filename     Name of the source file.
 * @param macro_list   Linked list of defined macros.
 * @return             1 if error found, 0 if OK.
 */
int check_macro_usage_before_definition(const char *filename, macro_t *macro_list);

/**
 * Removes leading and trailing whitespace characters from a string.
 * The trimming is done in-place (modifies the original string).
 *
 * @param str   The string to trim (must be null-terminated).
 * @return      A pointer to the trimmed string (may be moved forward).
 */
char *trim_whitespace(char *str);



#endif /* MACRO_PROC_H */
