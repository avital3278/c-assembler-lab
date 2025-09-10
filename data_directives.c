#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "data_directives.h"
#include "data_strct.h"
#include "Errors.h"
#include "line_parser.h"
#include "instr_utils.h"
#include "code_writer.h"
#include "macro_prepro.h"

int handle_data_directive(const char *line, int IC, int DC, CodeLine *memory_words, int *error_flag) {
	char buffer[1024];
	char *token, *newline, *end;
	int word_count = 0;

	strncpy(buffer, line, sizeof(buffer));
	buffer[sizeof(buffer) - 1] = '\0';

	newline = strchr(buffer, '\n');
	if (newline) *newline = '\0';

	token = strtok(buffer, ",");
	while (token != NULL) {
		token = skip_spaces(token);
		end = token + strlen(token) - 1;
		while (end > token && isspace((unsigned char)*end)) {
			*end = '\0';
			end--;
		}

		if (*token == '\0' || !is_valid_integer(token)) {
			report_error(NULL, 0, "Invalid number in .data directive.");
			*error_flag = 1;
			return 0;
		}

		memory_words[word_count].address = IC + DC + word_count;
		memory_words[word_count].word.value = (atoi(token) & 0x3FF) << 2;

		memory_words[word_count].word.is_label = 0;
		memory_words[word_count].word.label_name = NULL;

		word_count++;
		token = strtok(NULL, ",");
	}

	return word_count;
}

int handle_string_directive(const char *line, int IC, int DC, CodeLine *memory_words, int *error_flag) {
	const char *start, *end, *after_end;
	int word_count = 0;

	line = remove_line_break((char *)line);
	start = skip_spaces((char *)line);

	if (*start != '"') {
		report_error(NULL, 0, "Missing opening quote in .string directive.");
		*error_flag = 1;
		return 0;
	}

	start++;
	end = strchr(start, '"');
	if (!end) {
		report_error(NULL, 0, "Missing closing quote in .string directive.");
		*error_flag = 1;
		return 0;
	}

	after_end = skip_spaces((char *)(end + 1));
	if (*after_end != '\0') {
		report_error(NULL, 0, "Unexpected characters after closing quote in .string.");
		*error_flag = 1;
		return 0;
	}

	while (start < end) {
		memory_words[word_count].address = IC + DC + word_count;
		memory_words[word_count].word.value = (((int)(*start) & 0x3FF) << 2);
		memory_words[word_count].word.is_label = 0;
		memory_words[word_count].word.label_name = NULL;

		

		word_count++;
		start++;
	}

	memory_words[word_count].address = IC + DC + word_count;
	memory_words[word_count].word.value = (0 << 2);
	memory_words[word_count].word.is_label = 0;
	memory_words[word_count].word.label_name = NULL;
	word_count++;

	return word_count;
}

int handle_mat_directive(const char *line, int IC, int DC, CodeLine *memory_words, int *error_flag) {
	char buffer[1024];
	int rows = -1, cols = -1, i = 0;
	char *token, *init_data;
	int total_values;

	line = remove_line_break((char *)line);
	strncpy(buffer, line, sizeof(buffer));
	buffer[sizeof(buffer) - 1] = '\0';

	if (sscanf(buffer, "[%d][%d]", &rows, &cols) != 2 || rows <= 0 || cols <= 0) {
		report_error(NULL, 0, "Invalid matrix dimensions in .mat directive.");
		*error_flag = 1;
		return 0;
	}

	init_data = strchr(buffer, ']');
	if (init_data) init_data = strchr(init_data + 1, ']');
	if (!init_data) {
		report_error(NULL, 0, "Missing ']' in .mat directive.");
		*error_flag = 1;
		return 0;
	}

	init_data++;
	init_data = skip_spaces(init_data);

	{
		char *sc = strchr(init_data, ';');
		if (sc) *sc = '\0';
	}

	total_values = rows * cols;

	if (*init_data == '\0') {
		int j;
		for (j = 0; j < total_values; j++) {
			memory_words[j].address = IC + DC + j;
			memory_words[j].word.value = (0 << 2);
			memory_words[j].word.is_label = 0;
			memory_words[j].word.label_name = NULL;
		}
		return total_values;
	}

	token = strtok(init_data, ",");


	while (token && i < total_values) {
		token = skip_spaces(token);

		if (!is_valid_integer(token)) {
			report_error(NULL, 0, "Invalid matrix initializer.");
			*error_flag = 1;
			return 0;
		}

		memory_words[i].address = IC + DC + i;
		memory_words[i].word.value = (atoi(token) & 0x3FF) << 2;
		memory_words[i].word.is_label = 0;
		memory_words[i].word.label_name = NULL;
		i++;

		token = strtok(NULL, ",");
	}

	if (token && i == total_values) {
		report_error(NULL, 0, "Too many initializers in .mat directive.");
		*error_flag = 1;
		return 0;
	}

	while (i < total_values) {
		memory_words[i].address = IC + DC + i;
		memory_words[i].word.value = (0 << 2);
		memory_words[i].word.is_label = 0;
		memory_words[i].word.label_name = NULL;
		i++;
	}

	return total_values;
}

