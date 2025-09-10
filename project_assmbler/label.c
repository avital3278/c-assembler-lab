#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "label.h"
#include "instr_utils.h"
#include "Errors.h"

int add_new_label(LabelEntry **label_list, int size, const char *name, int address, int type, int *error_flag, location origin) {
	LabelEntry *temp;

	/* Expand the label list */
	temp = (LabelEntry *)realloc(*label_list, (size + 1) * sizeof(LabelEntry));
	if (!temp) {
		print_external_error(ERR_CODE_19, origin);
		*error_flag = 1;
		return 0;
	}
	*label_list = temp;

	/* Duplicate the label name */
	(*label_list)[size].name = my_strdup(name);
	if (!(*label_list)[size].name) {
		print_external_error(ERR_CODE_20, origin);
		*error_flag = 1;
		return 0;
	}

	/* Store remaining label details */
	(*label_list)[size].location = address;
	(*label_list)[size].line_index = origin.line_number;
	(*label_list)[size].type = type;

	return 1;
}

int has_duplicate_label(const LabelEntry *list, int size, const char *name) {
	int i;

	/* Validate input */
	if (!list || !name)
		return 0;

	/* Search for duplicate label name */
	for (i = 0; i < size; i++) {
		if (strcmp(list[i].name, name) == 0)
			return 1;
	}
	return 0;
}

void adjust_data_labels(LabelEntry *label_list, int size, int total_ic) {
	int i;

	if (!label_list || size <= 0) {
		return;
	}

	for (i = 0; i < size; i++) {
		if (label_list[i].type == LABEL_DATA) {
			label_list[i].location += total_ic;
		}
	}
}

int get_label_address(const LabelEntry *labels, int label_count, const char *name) {
	int i;

	if (!labels || !name || label_count <= 0) {
		return -1;
	}

	for (i = 0; i < label_count; i++) {
		if (strcmp(labels[i].name, name) == 0) {
			return labels[i].location;
		}
	}

	return -1; /* Not found */
}

int resolve_label_addresses(CodeLine *code, int code_len,
								const LabelEntry *label_list, int label_count,
								const char *filename) {
	int i, address;
	int error_flag = 0;
	location loc;

	if (!code || !label_list || code_len <= 0 || label_count <= 0) {
		return 1;
	}

	loc.file_name = filename;

	for (i = 0; i < code_len; i++) {
		if (code[i].word.is_label) {
			address = get_label_address(label_list, label_count, code[i].word.label_name);

			if (address == -1) {
				loc.line_number = code[i].address;
				print_external_error(ERR_CODE_22, loc);
				error_flag = 1;
			} else {
				code[i].word.value = address;
			}
		}
	}

	return !error_flag;
}

void convert_to_special_base4(int num, char *buffer) {
	int i;
	const char base4_chars[] = { 'a', 'b', 'c', 'd' };

	for (i = 4; i >= 0; i--) {
		buffer[i] = base4_chars[num % 4];
		num /= 4;
	}

	buffer[5] = '\0'; /* null terminate */
}

int export_entries(const char *filename, const LabelEntry *label_list, int label_count,
					const EntryExternRef *entry_list, int entry_count) {
	FILE *ent_file;
	char ent_filename[256];
	int i, j;
	int found;
	char base4_address[6]; /* 5 base-4 digits + null terminator */
	char *trimmed_addr;   

	sprintf(ent_filename, "%s.ent", filename);

	ent_file = fopen(ent_filename, "w");
	if (!ent_file) {
		print_error_with_file(ERROR_CODE_8, filename);
		return 0;
	}

	for (i = 0; i < entry_count; i++) {
		found = 0;
		for (j = 0; j < label_count; j++) {
			if (strcmp(entry_list[i].name, label_list[j].name) == 0) {
				convert_to_special_base4(label_list[j].location, base4_address);

			
				trimmed_addr = base4_address;
				while (*trimmed_addr == 'a' && *(trimmed_addr + 1)) {
					trimmed_addr++;
				}

				fprintf(ent_file, "%s %s\n", label_list[j].name, trimmed_addr);
				found = 1;
				break;
			}
		}

		if (!found) {
			location loc;
			loc.file_name = filename;
			loc.line_number = entry_list[i].line_index;
			print_external_error(ERR_CODE_22, loc);
		}
	}

	fclose(ent_file);
	return 1;
}

int export_externals(const char *filename, const EntryExternRef *extern_list, int extern_count) {
	FILE *ext_file;
	char ext_filename[256];
	char base4_address[6]; /* 5 base-4 digits + null terminator */
	int i;

	/* If no externals, don't create a file */
	if (!extern_list || extern_count <= 0) {
		return 1;
	}

	sprintf(ext_filename, "%s.ext", filename);

	ext_file = fopen(ext_filename, "w");
	if (!ext_file) {
		print_error_with_file(ERROR_CODE_8, filename);
		return 0;
	}

	for (i = 0; i < extern_count; i++) {
		convert_to_special_base4(extern_list[i].line_index, base4_address);
		fprintf(ext_file, "%s %s\n", extern_list[i].name, base4_address);
	}

	fclose(ext_file);
	return 1;
}

void free_name_list(void *names, int size, int is_label_list) {
	int i;

	if (!names || size <= 0) {
		return;
	}

	if (is_label_list) {
		LabelEntry *label_list = (LabelEntry *)names;
		for (i = 0; i < size; i++) {
			free(label_list[i].name);
		}
	} else {
		EntryExternRef *ref_list = (EntryExternRef *)names;
		for (i = 0; i < size; i++) {
			free(ref_list[i].name);
		}
	}

	free(names);
}

