#ifndef STEP1_H
#define STEP1_H

#include "data_strct.h"
#include "label.h"

/**
 * @brief Performs the first pass over an .am file.
 *
 * This function processes an assembly source file:
 *  - Parses labels, instructions, and data.
 *  - Builds the symbol table (label list).
 *  - Collects .entry / .extern declarations.
 *  - Constructs the memory image (code and data words).
 *  - Detects syntax/semantic errors in the source.
 *
 * Outputs:
 *  - code_lines_out:	Allocated array of CodeLine (memory image). Caller must free().
 *  - labels_out:		Dynamically allocated labels table. Caller must free names and the array.
 *  - IC_out / DC_out:	Final IC and DC after pass 1.
 *  - error_flag_out:	Set to 1 if any error occurred, otherwise 0.
 *  - label_count_out:	Number of labels in labels_out.
 *  - entries_out / entries_count_out: collected .entry symbols.
 *  - externs_out / externs_count_out: collected .extern symbols.
 *
 * @return 1 if pass completed successfully (no errors), 0 if errors were found.
 */
int step1(const char *file_name,
		  CodeLine **code_lines_out,
		  LabelEntry **labels_out,
		  int *IC_out,
		  int *DC_out,
		  int *error_flag_out,
		  int *label_count_out,
		  EntryExternRef **entries_out,
		  int *entries_count_out,
		  EntryExternRef **externs_out,
		  int *externs_count_out);

#endif /* STEP1_H */

