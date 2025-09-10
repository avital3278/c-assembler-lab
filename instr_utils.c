#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "instr_utils.h"
#include "line_parser.h"

const char *instruction_names[] = {
	"mov", "cmp", "add", "sub",
	"lea", "clr", "not", "inc",
	"dec", "jmp", "bne", "jsr",
	"red", "prn", "rts", "stop",
	NULL
};

/* Checks if a word is a valid instruction */
int is_instr(const char *word) {
	int i;
	for (i = 0; instruction_names[i] != NULL; i++) {
		if (strcmp(word, instruction_names[i]) == 0)
			return 1;
	}
	return 0;
}

/* Returns opcode number for a valid instruction */
int what_opcode(const char *word) {
	int i;
	for (i = 0; instruction_names[i] != NULL; i++) {
		if (strcmp(word, instruction_names[i]) == 0)
			return i;
	}
	return -1;
}

/* Returns register number (0–7) or -1 if invalid */
int what_reg(const char *word) {
	if (!word) return -1;
	if (strlen(word) == 2 && word[0] == 'r' && word[1] >= '0' && word[1] <= '7')
		return word[1] - '0';
	return -1;
}

/* Returns expected operand count for given instruction */
int expected_operand_count(const char *cmd) {
	if (!cmd) return -1;

	if (strcmp(cmd, "mov") == 0 || strcmp(cmd, "cmp") == 0 ||
		strcmp(cmd, "add") == 0 || strcmp(cmd, "sub") == 0 ||
		strcmp(cmd, "lea") == 0)
		return 2;

	if (strcmp(cmd, "clr") == 0 || strcmp(cmd, "not") == 0 ||
		strcmp(cmd, "inc") == 0 || strcmp(cmd, "dec") == 0 ||
		strcmp(cmd, "jmp") == 0 || strcmp(cmd, "bne") == 0 ||
		strcmp(cmd, "jsr") == 0 || strcmp(cmd, "red") == 0 ||
		strcmp(cmd, "prn") == 0)
		return 1;

	if (strcmp(cmd, "rts") == 0 || strcmp(cmd, "stop") == 0)
		return 0;

	return -1;
}

/* Duplicates a string */
char *my_strdup(const char *s) {
	char *copy;

	if (!s) return NULL;

	copy = (char *)malloc(strlen(s) + 1);
	if (copy) strcpy(copy, s);

	return copy;
}

/* Skips leading spaces/tabs */
char *skip_spaces(char *str) {
	while (*str == ' ' || *str == '\t') {
		str++;
	}
	return str;
}

/* Checks if word is reserved (instruction, directive, register) */
int is_reserved_word(const char *word) {
	int i;

	for (i = 0; instruction_names[i] != NULL; i++) {
		if (strcmp(word, instruction_names[i]) == 0)
			return 1;
	}

	if (is_directive(word)) return 1;
	if (is_register(word)) return 1;

	return 0;
}

/* Checks if string is valid register (r0–r7) */
int is_register(const char *str) {
	if (!str) return 0;
	if (str[0] != 'r' || strlen(str) != 2) return 0;
	return (str[1] >= '0' && str[1] <= '7');
}

/* Returns register index (0–7) or -1 */
int get_register_index(const char *str) {
	if (!is_register(str)) return -1;
	return str[1] - '0';
}

/* Checks if a token is directive or macro keyword */
int is_directive(const char *token) {
	const char *dir[] = {
		".data", ".string", ".entry", ".extern", ".mat", "mcro", "mcroend"
	};
	int i, count = sizeof(dir) / sizeof(dir[0]);

	if (!token) return 0;

	for (i = 0; i < count; i++) {
		if (strcmp(token, dir[i]) == 0)
			return 1;
	}
	return 0;
}

/* Validates a full label (must end with ':') */
int is_label(const char *text) {
	int i, len;
	char name[MAX_LABEL_LENGTH + 1];

	if (!text) return 0;

	len = strlen(text);
	if (len < 2 || text[len - 1] != ':') return 0;
	if (len - 1 > MAX_LABEL_LENGTH) return 0;

	for (i = 0; i < len - 1; i++) {
		if ((i == 0 && !isalpha(text[i])) || !isalnum(text[i]))
			return 0;
	}

	strncpy(name, text, len - 1);
	name[len - 1] = '\0';

	return !is_reserved_word(name);
}

/* Checks if label name is valid (without colon) */
int is_valid_label_name(const char *text) {
	int i;

	if (!text || !isalpha(text[0])) return 0;

	for (i = 1; text[i] != '\0'; i++) {
		if (!isalnum(text[i])) return 0;
	}

	return !is_reserved_word(text);
}

/* Detects invalid comma usage */
int has_invalid_commas(const char *line) {
	const char *ptr = line;
	int prev_was_comma = 0;

	if (!line) return 1;

	while (*ptr == ' ' || *ptr == '\t') ptr++;
	if (*ptr == ',') return 1;

	while (*ptr) {
		if (*ptr == ',') {
			if (prev_was_comma) return 1;
			prev_was_comma = 1;
		} else if (*ptr != ' ' && *ptr != '\t') {
			prev_was_comma = 0;
		}
		ptr++;
	}

	ptr--;
	while (ptr > line && (*ptr == ' ' || *ptr == '\t')) ptr--;
	return *ptr == ',';
}

/* Trims whitespace from both ends */
char *strip_whitespace(char *str) {
	char *left, *right;

	if (!str) return NULL;

	left = str;
	while (*left && isspace((unsigned char)*left)) left++;

	if (*left == '\0') {
		*str = '\0';
		return str;
	}

	right = left + strlen(left) - 1;
	while (right > left && isspace((unsigned char)*right)) right--;

	*(right + 1) = '\0';

	if (left != str)
		memmove(str, left, strlen(left) + 1);

	return str;
}

/* Validates if string is numeric (+/- optional) */
int check_numeric(const char *text) {
	if (!text || *text == '\0') return 0;

	if (*text == '+' || *text == '-') text++;
	if (!isdigit((unsigned char)*text)) return 0;

	while (*text) {
		if (!isdigit((unsigned char)*text)) return 0;
		text++;
	}

	return 1;
}

/* Checks if operand is immediate value (e.g. "#5") */
int is_immediate_value(const char *operand) {
	return operand && operand[0] == '#' && check_numeric(operand + 1);
}

/* Returns AddressType enum based on operand form */
AddressType get_addressing_type(const char *operand) {
	char base[MAX_LABEL_LENGTH], i1[MAX_LABEL_LENGTH], i2[MAX_LABEL_LENGTH];

	if (!operand || strlen(operand) == 0)
		return ADDR_NONE;

	if (is_immediate_value(operand))
		return ADDR_IMMEDIATE;

	if (is_register(operand))
		return ADDR_REGISTER;

	if (extract_matrix_parts(operand, base, i1, i2))
		return ADDR_MATRIX;

	if (is_valid_label_name(operand))
		return ADDR_DIRECT;

	return ADDR_INVALID;
}

/* Maps opcode names to their numeric values */
int get_opcode_code(const char *opcode) {
	if (strcmp(opcode, "mov") == 0) return 0;
	if (strcmp(opcode, "cmp") == 0) return 1;
	if (strcmp(opcode, "add") == 0) return 2;
	if (strcmp(opcode, "sub") == 0) return 3;
	if (strcmp(opcode, "lea") == 0) return 4;
	if (strcmp(opcode, "clr") == 0) return 5;
	if (strcmp(opcode, "not") == 0) return 6;
	if (strcmp(opcode, "inc") == 0) return 7;
	if (strcmp(opcode, "dec") == 0) return 8;
	if (strcmp(opcode, "jmp") == 0) return 9;
	if (strcmp(opcode, "bne") == 0) return 10;
	if (strcmp(opcode, "jsr") == 0) return 11;
	if (strcmp(opcode, "red") == 0) return 12;
	if (strcmp(opcode, "prn") == 0) return 13;
	if (strcmp(opcode, "rts") == 0) return 14;
	if (strcmp(opcode, "stop") == 0) return 15;

	return -1;
}

/* Checks if a string is a valid signed integer */
int is_valid_integer(const char *s) {
	if (!s || *s == '\0') return 0;

	if (*s == '+' || *s == '-') s++;
	if (!isdigit((unsigned char)*s)) return 0;

	while (*s) {
		if (!isdigit((unsigned char)*s)) return 0;
		s++;
	}

	return 1;
}

/* Returns 1 if instruction uses only destination operand */
int opcode_requires_only_dst(const char *opcode) {
	return strcmp(opcode, "jmp") == 0 ||
		strcmp(opcode, "bne") == 0 ||
		strcmp(opcode, "jsr") == 0 ||
		strcmp(opcode, "red") == 0 ||
		strcmp(opcode, "prn") == 0 ||
		strcmp(opcode, "inc") == 0 ||
		strcmp(opcode, "dec") == 0 ||
		strcmp(opcode, "clr") == 0 ||
		strcmp(opcode, "not") == 0;
}

