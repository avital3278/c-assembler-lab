#ifndef line_parser_H
#define line_parser_H

#define MAX_TOKEN_LENGTH 100


/**
 * Extracts a label from the beginning of a line, if it exists.
 * A label is assumed to be the first token ending with a colon (:).
 * 
 * @param line The input line (modified in-place).
 * @return A pointer to a static buffer containing the label name, or NULL if no label found.
 */
char *get_line_label(char *line);

/**
 * Extracts the label from a line if it exists.
 * @param line The input line to parse (will not be modified).
 * @param label_out Buffer to store the extracted label.
 * @return 1 if a label was found and copied, 0 otherwise.
 */
int extract_label(char *line, char *label_out);


/**
 * Returns the part of the line that comes after a label (if exists), skipping spaces.
 * If no label is found, returns the original line.
 *
 * @param line The input line.
 * @return A pointer to the position after the label, or the original line if no label.
 */


char *get_after_label(char *line);

/**
 * Extracts the command token (e.g., instruction name) from a line.
 * Skips leading spaces and comments.
 *
 * @param line The input line (modified in-place).
 * @return A pointer to a static buffer containing the command token, or NULL if invalid.
 */
char *parse_command_token(char *line);

/**
 * Parses up to two operands from an instruction line (C90 compliant).
 *
 * @param line   The input line (after label and command).
 * @param op1    Buffer to store the first operand.
 * @param op2    Buffer to store the second operand.
 * @return       Number of operands found (0, 1, or 2).
 */
int parse_instruction_operands(char *line, char *op1, char *op2);

/**
 * Parses a matrix expression of the form BASE[INDEX1][INDEX2].
 *
 * @param expr The expression to parse.
 * @param base Buffer to store the base name (e.g., matrix name).
 * @param i1   Buffer to store the first index.
 * @param i2   Buffer to store the second index.
 * @return     1 if successful, 0 if parsing fails.
 */
int extract_matrix_parts(const char *expr, char *base, char *i1, char *i2);

/**
 * Removes a newline ('\n') or carriage return ('\r') character from the input line.
 *
 * @param line The input string (modified in-place).
 * @return     The same pointer to the cleaned line.
 */
char *remove_line_break(char *line);

/**
 * Extracts the next token from a line of text.
 *
 * Skips leading whitespace and commas, then copies the next contiguous sequence
 * of non-whitespace, non-comma characters into a static buffer and returns a pointer to it.
 * The input pointer is updated to point to the position after the token.
 *
 * @param line_ptr A pointer to a char pointer pointing to the current position in the line.
 * @return A pointer to the next token, or NULL if no token is found.
 */
char *next_token(char **line_ptr);

#endif

