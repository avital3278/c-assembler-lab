#include <string.h>
#include <ctype.h>
#include "line_parser.h"
#include "instr_utils.h"

char *get_line_label(char *line) {
	static char label[MAX_LABEL_LENGTH + 1];
	int i = 0;

	line = skip_spaces(line);

	if (*line == '\0' || *line == ';') {
		return NULL;
	}

	while (line[i] != '\0' && line[i] != ':' && !isspace(line[i]) && i < MAX_LABEL_LENGTH) {
		label[i] = line[i];
		i++;
	}

	if (line[i] == ':') {
		label[i] = '\0';
		return label;
	}

	return NULL;
}

int extract_label(char *line, char *label_out) {
	char *found = get_line_label(line);

	if (found) {
		strcpy(label_out, found);
		return 1;
	}
	return 0;
}

char *get_after_label(char *line) {
	line = skip_spaces(line);

	/* Find colon */
	while (*line != '\0' && *line != ':' && !isspace((unsigned char)*line)) {
		line++;
	}

	if (*line == ':') {
		line++;	/* Move past the ':' */
		line = skip_spaces(line);
		return line;
	}

	/* No label found → return original line */
	return line;
}

char *parse_command_token(char *line) {
	static char command[MAX_LABEL_LENGTH + 1];
	int i = 0;

	line = skip_spaces(line);

	/* If empty or comment line */
	if (*line == '\0' || *line == ';') {
		return NULL;
	}

	/* Read command name */
	while (line[i] != '\0' && !isspace(line[i]) && i < MAX_LABEL_LENGTH) {
		command[i] = line[i];
		i++;
	}
	command[i] = '\0';

	/* If nothing was copied, return NULL */
	if (i == 0) {
		return NULL;
	}

	return command;
}

int parse_instruction_operands(char *line, char *op1, char *op2) {
	char *ptr;
	int i;

	op1[0] = '\0';
	op2[0] = '\0';

	ptr = skip_spaces(line);

	if (*ptr == '\0' || *ptr == ';')
		return 0;

	/* Extract first operand */
	i = 0;
	while (*ptr != '\0' && *ptr != ',' && *ptr != '\n' && i < MAX_OPERAND_LENGTH - 1) {
		if (!isspace((unsigned char)*ptr)) {
			op1[i++] = *ptr;
		}
		ptr++;
	}
	op1[i] = '\0';

	ptr = skip_spaces(ptr);

	/* No comma → only one operand */
	if (*ptr != ',') {
		return (strlen(op1) > 0) ? 1 : 0;
	}

	ptr++; /* skip comma */
	ptr = skip_spaces(ptr);

	/* Extract second operand */
	i = 0;
	while (*ptr != '\0' && *ptr != ',' && *ptr != '\n' && i < MAX_OPERAND_LENGTH - 1) {
		if (!isspace((unsigned char)*ptr)) {
			op2[i++] = *ptr;
		}
		ptr++;
	}
	op2[i] = '\0';

	if (strlen(op2) > 0)
		return 2;
	if (strlen(op1) > 0)
		return 1;

	return 0;
}

int extract_matrix_parts(const char *expr, char *base, char *i1, char *i2) {
	int i = 0, j = 0;
	const char *p;

	/* Check for NULL pointers */
	if (!expr || !base || !i1 || !i2) {
		return 0;
	}

	p = expr;

	/* Extract base name until first '[' */
	while (*p != '\0' && *p != '[' && i < MAX_LABEL_LENGTH) {
		base[i++] = *p++;
	}
	base[i] = '\0';

	if (*p != '[') {
		return 0;
	}
	p++; /* Skip first '[' */

	/* Extract first index */
	j = 0;
	while (*p != '\0' && *p != ']' && j < MAX_LABEL_LENGTH) {
		if (!isspace((unsigned char)*p)) {
			i1[j++] = *p;
		}
		p++;
	}
	i1[j] = '\0';

	if (*p != ']') {
		return 0;
	}
	p++; /* Skip first ']' */

	/* Check for second '[' */
	if (*p != '[') {
		return 0;
	}
	p++; /* Skip second '[' */

	/* Extract second index */
	j = 0;
	while (*p != '\0' && *p != ']' && j < MAX_LABEL_LENGTH) {
		if (!isspace((unsigned char)*p)) {
			i2[j++] = *p;
		}
		p++;
	}
	i2[j] = '\0';

	if (*p != ']') {
		return 0;
	}

	p++; /* Skip second ']' */

	/* Skip trailing whitespace if any */
	while (isspace((unsigned char)*p)) {
		p++;
	}

	/* Skip trailing whitespace */
	while (isspace((unsigned char)*p)) {
		p++;
	}

	/* Allow ',' or end of string */
	if (*p != '\0' && *p != ',') {
    		return 0;
	}
	return 1;

}

char *remove_line_break(char *line) {
	char *p = line;

	while (*p != '\0') {
		if (*p == '\n' || *p == '\r') {
			*p = '\0';
			break;
		}
		p++;
	}

	return line;
}

char *next_token(char **line_ptr) {
	static char token[MAX_TOKEN_LENGTH];
	int i = 0;
	char *ptr;

	if (line_ptr == NULL || *line_ptr == NULL) {
		return NULL;
	}

	ptr = *line_ptr;

	/* Skip spaces, tabs, and commas */
	while (*ptr == ' ' || *ptr == '\t' || *ptr == ',') {
		ptr++;
	}

	/* End of line */
	if (*ptr == '\0' || *ptr == '\n') {
		*line_ptr = ptr;
		return NULL;
	}

	/* Collect token into static string */
	while (*ptr != '\0' && *ptr != '\n' && *ptr != ' ' && *ptr != '\t' && *ptr != ',' && i < MAX_TOKEN_LENGTH - 1) {
		token[i++] = *ptr++;
	}

	token[i] = '\0';

	*line_ptr = ptr; /* Update the external pointer */

	return token;
}

