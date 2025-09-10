#ifndef LABEL_H
#define LABEL_H

#include "data_strct.h" 
#include "Errors.h"


#define LABEL_CODE 0
#define LABEL_DATA 1


/**
* Adds a new label to the label list.
*
* @param label_list Pointer to the label list (updated if necessary)
* @param size Current number of labels
* @param name Name of the new label
* @param address Memory address
* @param type 0 = code, 1 = data
* @param error_flag Set to 1 if there is an error
* @param origin Location of the source line (file + line)
* @return 1 if successful, 0 if memory allocation failed
*/

int add_new_label(LabelEntry **label_list, int size, const char *name, int address, int type, int *error_flag, location origin);


/**
 * @brief Checks if a label name already exists in the label list.
 *
 * This function iterates through the label array and compares each
 * label name to the given name. Returns 1 if a duplicate is found,
 * otherwise returns 0.
 *
 * @param list Pointer to the array of labels.
 * @param size Number of labels in the array.
 * @param name The label name to check for duplicates.
 * @return     1 if a duplicate label is found, 0 otherwise.
 */
int has_duplicate_label(const LabelEntry *list, int size, const char *name);


/**
 * @brief Adjusts the addresses of data labels after the code section is  finalized.
 *
 * After the first pass, data labels initially have addresses starting from 0.
 * This function adds the total number of code words (total_ic) to their address
 * so that the data section will be placed after the code in memory.
 *
 * @param label_list Pointer to the array of labels.
 * @param size       Number of labels in the array.
 * @param total_ic   Final instruction count (used as address offset).
 */
void adjust_data_labels(LabelEntry *label_list, int size, int total_ic);

/**
 * @brief Retrieves the memory address of a label by its name.
 *
 * This function searches the given label list for a label with the specified name,
 * and returns its address (location) if found. If not found, returns -1.
 *
 * @param labels       Array of defined labels.
 * @param label_count  Number of labels in the array.
 * @param name         The name of the label to search for.
 * @return             The label's address if found, -1 otherwise.
 */
int get_label_address(const LabelEntry *labels, int label_count, const char *name);

/**
 * @brief Resolves all pending label references in the code section.
 *
 * This function scans the list of machine words (code lines) and updates
 * all words that reference a label. If a label is not found in the label list,
 * an error is reported with its approximate location.
 *
 * @param code         Array of machine words (code section).
 * @param code_len     Number of code words.
 * @param label_list   Array of defined labels (from first pass).
 * @param label_count  Number of labels in the array.
 * @param filename     The name of the original file (used for error reporting).
 * @return             1 if all labels were resolved successfully, 0 if there were errors.
 */
int resolve_label_addresses(CodeLine *code, int code_len,
                            const LabelEntry *label_list, int label_count,
                            const char *filename);

/**
 * @brief Converts a number to special base-4 representation (e.g., "abdc").
 *
 * This function converts an integer (typically an address or value) into a string
 * consisting of 5 characters using the custom base-4 alphabet: a=0, b=1, c=2, d=3.
 *
 * @param num The non-negative number to convert.
 * @param buffer A buffer of at least 6 characters (5 digits + null terminator).
 */
void convert_to_special_base4(int num, char *buffer);


/** 
 * @brief Exports entry labels to a .ent file.
 *
 * Goes over the list of entry references (from .entry directives), finds them in the label list,
 * and writes their names and addresses (in special base-4 format) to a file named "<filename>.ent".
 *
 * @param filename     The base name of the original assembly file (without extension).
 * @param label_list   The array of all defined labels.
 * @param label_count  The number of labels in the label list.
 * @param entry_list   The array of entry declarations (symbol names + line numbers).
 * @param entry_count  The number of entry references.
 * @return             1 if the export succeeded, 0 if there was an error.
 */
int export_entries(const char *filename, const LabelEntry *label_list, int label_count,
		const EntryExternRef *entry_list, int entry_count);


/**
 * @brief Exports external label references to a .ext file.
 *
 * Goes over all references to external labels (from .extern declarations),
 * and writes their names and the addresses where they are used
 * (in special base-4 format) to a file named "<filename>.ext".
 *
 * @param filename      The base name of the original assembly file (without extension).
 * @param extern_list   The array of extern references.
 * @param extern_count  The number of extern references.
 * @return              1 if the export succeeded, 0 if there was an error.
 */
int export_externals(const char *filename, const EntryExternRef *extern_list, int extern_count);


/**
 * @brief Frees a list of labels or entry/extern references.
 *
 * Used to free memory for either LabelEntry[] or EntryExternRef[] arrays.
 *
 * @param names An array of structs containing a dynamically allocated name field.
 * @param size  Number of elements in the array.
 */
void free_name_list(void *names, int size, int is_label_list);

#endif /* LABEL_H */

