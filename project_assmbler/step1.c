#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "data_strct.h"
#include "label.h"
#include "instr_utils.h"
#include "line_parser.h"
#include "code_writer.h"
#include "Errors.h"

#define MAX_LINE_LENGTH 80
#define MAX_CODE_LINES 500

int step1(const char *file_name, CodeLine **code_lines_out, LabelEntry **labels_out,
		int *IC_out, int *DC_out, int *error_flag_out, int *label_count_out,
		EntryExternRef **entries_out, int *entries_count_out,
		EntryExternRef **externs_out, int *externs_count_out) {
	FILE *fp;
	char line[MAX_LINE_LENGTH + 2];
	char label[MAX_LABEL_LENGTH];
	char cmd_token[MAX_LABEL_LENGTH];
	char op1[MAX_OPERAND_LENGTH], op2[MAX_OPERAND_LENGTH];
	int IC = 100, DC = 0, error_flag = 0;
	int code_size = 0;
	int line_number = 0;
	LabelEntry *labels = NULL;
	int i;
	int label_count = 0;
	EntryExternRef *entries = NULL, *externs = NULL;
	int entries_count = 0, externs_count = 0;

	/* Used to track the offset adjustments for data symbols after IC */
	typedef struct {
		int start_index;
		int count;
		int ic_at_creation;
	} DataChunk;
	DataChunk data_chunks[128];
	int data_chunks_count = 0;

	/* Open source file */
	fp = fopen(file_name, "r");
	if (!fp) {
		report_error(file_name, 0, "Cannot open file.");
		*error_flag_out = 1;
		return 0;
	}

	/* Allocate memory for code lines */
	*code_lines_out = (CodeLine *)malloc(MAX_CODE_LINES * sizeof(CodeLine));
	if (!*code_lines_out) {
		report_error(file_name, 0, "Memory allocation failed for code lines.");
		*error_flag_out = 1;
		fclose(fp);
		return 0;
	}

	/* First pass: read source line by line */
	while (fgets(line, sizeof(line), fp)) {
		char *line_copy;
		int has_label;
		char *cursor;
		char *cmd;

		line_number++;
		line_copy = my_strdup(line);

		/* Check for label at start of line */
		has_label = extract_label(line_copy, label);
		if (has_label)
			cursor = get_after_label(line_copy);
		else
			cursor = line_copy;

		/* Skip empty lines and comments */
		cursor = skip_spaces(cursor);
		if (*cursor == '\0' || *cursor == ';') {
			free(line_copy);
			continue;
		}

		/* Extract command or directive token */
		cmd = next_token(&cursor);
		if (!cmd) {
			free(line_copy);
			continue;
		}

		strncpy(cmd_token, cmd, MAX_LABEL_LENGTH);
		cmd_token[MAX_LABEL_LENGTH - 1] = '\0';

		

		/* Check label validity */
		if (has_label && has_duplicate_label(labels, label_count, label)) {
			report_error(file_name, line_number, "Duplicate label.");
			error_flag = 1;
			free(line_copy);
			continue;
		}
		if (has_label && !is_valid_label_name(label)) {
			location loc;
			loc.file_name = file_name;
			loc.line_number = line_number;
			print_external_error(ERR_CODE_23, loc);
			error_flag = 1;
			free(line_copy);
			continue;
		}

		/* Handle instruction */
		if (is_instr(cmd_token)) {
			int count;
			int num_operands = parse_instruction_operands(cursor, op1, op2);
			char *real_op1 = (num_operands >= 1) ? op1 : NULL;
			char *real_op2 = (num_operands == 2) ? op2 : NULL;
			int expected = expected_operand_count(cmd_token);

			if (expected != -1 && num_operands != expected) {
				char err_msg[100];
				sprintf(err_msg, "Instruction '%s' expects %d operand%s, but got %d.",
						cmd_token, expected, expected == 1 ? "" : "s", num_operands);
				report_error(file_name, line_number, err_msg);
				error_flag = 1;
				free(line_copy);
				continue;
			}

			

			/* Add label if present */
			if (has_label) {
				location origin;
				origin.file_name = file_name;
				origin.line_number = line_number;

				if (add_new_label(&labels, label_count, label, IC, LABEL_CODE, &error_flag, origin)) {
					label_count++;
				}
			}

			/* Encode instruction */
			count = write_instruction_code(cmd_token, real_op1, real_op2, *code_lines_out + code_size, IC);

			if (count == 0) {
				
				error_flag = 1;
			} else {
				IC += count;
				code_size += count;
			}
		}
		else if (is_directive(cmd_token)) {

			/* Handle .data, .string, .mat directives */
			if (strcmp(cmd_token, ".data") == 0 || strcmp(cmd_token, ".string") == 0 || strcmp(cmd_token, ".mat") == 0) {
				int words;
				char *directive_data = skip_spaces(cursor);

				if (has_label) {
					location origin;
					origin.file_name = file_name;
					origin.line_number = line_number;

					if (add_new_label(&labels, label_count, label, DC, LABEL_DATA, &error_flag, origin)) {
						label_count++;
					}
				}

				

				{
					int start_idx = code_size;
					int ic_snapshot = IC;

					words = write_data_words(cmd_token, directive_data, IC, DC, *code_lines_out + code_size, &error_flag);


					
					if (words == 0 && error_flag) {
						
					}
					DC += words;
					code_size += words;

					if (words > 0 && data_chunks_count < 128) {
						data_chunks[data_chunks_count].start_index    = start_idx;
						data_chunks[data_chunks_count].count          = words;
						data_chunks[data_chunks_count].ic_at_creation = ic_snapshot;
						data_chunks_count++;
					}
				}
			}

			/* Handle .entry directive */
			else if (strcmp(cmd_token, ".entry") == 0) {
				char *sym;

				if (has_label) {
					report_error(file_name, line_number, "Label before .entry is not allowed");
					error_flag = 1;
				}

				sym = next_token(&cursor);
				if (sym && is_valid_label_name(sym)) {
					EntryExternRef *tmp = (EntryExternRef*)realloc(entries, (entries_count + 1) * sizeof(EntryExternRef));
					if (!tmp) {
						report_error(file_name, line_number, "Memory allocation failed for .entry");
						error_flag = 1;
					} else {
						entries = tmp;
						entries[entries_count].name = my_strdup(sym);
						entries[entries_count].line_index = line_number;
						entries_count++;
					}
				} else {
					report_error(file_name, line_number, "Invalid .entry name");
					error_flag = 1;
				}
			}

			/* Handle .extern directive */
			else if (strcmp(cmd_token, ".extern") == 0) {
				char *sym;

				if (has_label) {
					report_error(file_name, line_number, "Label before .extern is not allowed");
					error_flag = 1;
				}

				sym = next_token(&cursor);
				if (sym && is_valid_label_name(sym)) {
					EntryExternRef *tmp = (EntryExternRef*)realloc(externs, (externs_count + 1) * sizeof(EntryExternRef));
					if (!tmp) {
						report_error(file_name, line_number, "Memory allocation failed for .extern");
						error_flag = 1;
					} else {
						externs = tmp;
						externs[externs_count].name = my_strdup(sym);
						externs[externs_count].line_index = line_number;
						externs_count++;
					}
				} else {
					report_error(file_name, line_number, "Invalid .extern name");
					error_flag = 1;
				}
			}

			else {
				report_error(file_name, line_number, "Unrecognized directive.");
				error_flag = 1;
			}
		}
		else {
			report_error(file_name, line_number, "Unrecognized command.");
			error_flag = 1;
		}

		free(line_copy);
	}

	fclose(fp);

	/* Adjust addresses of data words written before final IC */
	{
		int ch, k, delta;
		for (ch = 0; ch < data_chunks_count; ++ch) {
			delta = IC - data_chunks[ch].ic_at_creation;
			if (delta != 0) {
				for (k = 0; k < data_chunks[ch].count; ++k) {
					(*code_lines_out)[ data_chunks[ch].start_index + k ].address += delta;
				}
			}
		}
	}

	/* Update label addresses for data labels */
	for (i = 0; i < label_count; i++) {
		if (labels[i].type == LABEL_DATA) {
			labels[i].location += IC;
		}
	}

	*labels_out = labels;
	*IC_out = IC;
	*DC_out = DC;
	*error_flag_out = error_flag;
	*label_count_out = label_count;

	if (entries_out) {
		*entries_out = entries;
		if (entries_count_out) *entries_count_out = entries_count;
	}
	if (externs_out) {
		*externs_out = externs;
		if (externs_count_out) *externs_count_out = externs_count;
	}

	return !error_flag;
}

