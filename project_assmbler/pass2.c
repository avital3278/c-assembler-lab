#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pass2.h"
#include "data_strct.h"
#include "label.h"
#include "Errors.h"

static void to_base4_10(unsigned int v10, char out[6]) {
	int k; v10 &= 0x3FF;
	for (k = 4; k >= 0; --k)
		out[4 - k] = (char)('a' + ((v10 >> (k*2)) & 3));
	out[5] = '\0';
}

static void to_base4_12(unsigned int v12, char out[6]) {
	int k; v12 &= 0xFFF;
	for (k = 4; k >= 0; --k)
		out[4 - k] = (char)('a' + ((v12 >> (k*2)) & 3));
	out[5] = '\0';
}

static int find_label_addr(const LabelEntry *labels, int n, const char *name) {
	int i;
	if (!name) return -1;
	for (i = 0; i < n; i++) {
		if (labels[i].name && strcmp(labels[i].name, name) == 0)
			return labels[i].location;
	}
	return -1;
}

static int is_in_externs(const EntryExternRef *exts, int n, const char *name) {
	int i;
	for (i = 0; i < n; i++) {
		if (exts[i].name && strcmp(exts[i].name, name) == 0)
			return 1;
	}
	return 0;
}

static unsigned int set_are_12(unsigned int value12, int are) {
	value12 &= 0xFFF;
	value12 = (value12 & ~0x3) | (are & 0x3);
	return value12;
}

static int write_ob_file(
	const char *base_name,
	const CodeLine *code,
	int code_size,
	int IC_final,
	int DC_final
) {
	char path[FILENAME_MAX];
	FILE *fp;
	int total_words, i;
	int *image = NULL;
	char word6[7], addr5[6], ic5[6], dc5[6];
	char *trimmed_ic;
	char *trimmed_dc;
	char word5[6];
	char *trimmed_addr;	

	total_words = (IC_final - 100) + DC_final;
	if (total_words < 0) total_words = 0;

	image = (int*)malloc((size_t)total_words * sizeof(int));
	if (!image && total_words > 0) return 0;

	for (i = 0; i < total_words; i++) image[i] = 0;

	for (i = 0; i < code_size; i++) {
		int idx = code[i].address - 100;
		if (idx >= 0 && idx < total_words) {
			image[idx] = (code[i].word.value & 0xFFF);
		}
	}

	sprintf(path, "%s.ob", base_name);
	fp = fopen(path, "w");
	if (!fp) {
		free(image);
		return 0;
	}

	to_base4_10((unsigned)(IC_final - 100), ic5);
	to_base4_10((unsigned)DC_final,        dc5);

	trimmed_ic = ic5; while (*trimmed_ic == 'a' && *(trimmed_ic + 1)) trimmed_ic++;
	trimmed_dc = dc5; while (*trimmed_dc == 'a' && *(trimmed_dc + 1)) trimmed_dc++;

	fprintf(fp, "%s %s\n", trimmed_ic, trimmed_dc);

	for (i = 0; i < total_words; i++) {
		int addr_abs = 100 + i;

		to_base4_10((unsigned)(addr_abs & 0x3FF), addr5);

		
		trimmed_addr = addr5;
		while (*trimmed_addr == 'a' && *(trimmed_addr + 1)) trimmed_addr++;

	
		to_base4_12((unsigned)image[i], word6);
		

		to_base4_10((unsigned)((image[i] >> 2) & 0x3FF), word5);

		if (addr_abs < IC_final) {
			fprintf(fp, "%s %s\n", trimmed_addr, word6);
		} else {
			fprintf(fp, "%s %s\n", trimmed_addr, word5);
		}
	}

	fclose(fp);
	free(image);
	return 1;
}

typedef struct {
	char *name;
	int   use_address;
} ExtUse;

static int write_ext_file(
	const char *base_name,
	ExtUse *uses,
	int uses_count
) {
	char path[FILENAME_MAX];
	FILE *fp;
	int i;
	char addr4[6];
	char *trimmed_addr;	

	if (!uses || uses_count <= 0) return 1;

	sprintf(path, "%s.ext", base_name);
	fp = fopen(path, "w");
	if (!fp) return 0;

	for (i = 0; i < uses_count; i++) {
		convert_to_special_base4(uses[i].use_address & 0x3FF, addr4);

		
		trimmed_addr = addr4;
		while (*trimmed_addr == 'a' && *(trimmed_addr + 1)) trimmed_addr++;

		fprintf(fp, "%s %s\n", uses[i].name, trimmed_addr);
	}

	fclose(fp);
	return 1;
}

int pass2_resolve_and_write(
	const char *base_name,
	CodeLine *code, int code_size,
	const LabelEntry *labels, int label_count,
	const EntryExternRef *entries, int entry_count,
	const EntryExternRef *externs, int extern_count,
	int IC_final, int DC_final,
	int *error_flag
) {
	int i, had_error = 0;
	ExtUse *ext_uses = NULL;
	int ext_sz = 0;

	for (i = 0; i < code_size; i++) {
		if (code[i].word.is_label && code[i].word.label_name) {
			const char *name = code[i].word.label_name;

			if (is_in_externs(externs, extern_count, name)) {
				ExtUse *tmp = (ExtUse*)realloc(ext_uses, (size_t)(ext_sz + 1) * sizeof(ExtUse));
				if (!tmp) {
					had_error = 1;
					break;
				}
				ext_uses = tmp;

				code[i].word.value = set_are_12(0, ARE_EXTERNAL);

				ext_uses[ext_sz].name = (name ? (char*)malloc(strlen(name) + 1) : NULL);
				if (ext_uses[ext_sz].name) strcpy(ext_uses[ext_sz].name, name);
				ext_uses[ext_sz].use_address = code[i].address;
				ext_sz++;
			} else {
				int addr = find_label_addr(labels, label_count, name);
				if (addr < 0) {
					had_error = 1;
					if (error_flag) *error_flag = 1;
					fprintf(stderr, "Error: undefined label '%s' (second pass) at address %d\n",
						name ? name : "(null)", code[i].address);
				} else {
					code[i].word.value = set_are_12((addr & 0x3FF) << 2, ARE_RELOCATABLE);
				}
			}

			code[i].word.is_label = 0;
			free(code[i].word.label_name);
			code[i].word.label_name = NULL;
		} else {
			code[i].word.value = (code[i].word.value & ~0x3) | ARE_ABSOLUTE;
		}
	}

	if (had_error) {
		for (i = 0; i < ext_sz; i++) free(ext_uses[i].name);
		free(ext_uses);
		return 0;
	}

	if (!write_ob_file(base_name, code, code_size, IC_final, DC_final)) {
		if (error_flag) *error_flag = 1;
		for (i = 0; i < ext_sz; i++) free(ext_uses[i].name);
		free(ext_uses);
		return 0;
	}

	if (entry_count > 0) {
		if (!export_entries(base_name, labels, label_count, entries, entry_count)) {
			if (error_flag) *error_flag = 1;
			for (i = 0; i < ext_sz; i++) free(ext_uses[i].name);
			free(ext_uses);
			return 0;
		}
	}

	if (!write_ext_file(base_name, ext_uses, ext_sz)) {
		if (error_flag) *error_flag = 1;
		for (i = 0; i < ext_sz; i++) free(ext_uses[i].name);
		free(ext_uses);
		return 0;
	}

	for (i = 0; i < ext_sz; i++) free(ext_uses[i].name);
	free(ext_uses);

	return 1;
}

