#ifndef INSTR_UTILS_H
#define INSTR_UTILS_H

#include "data_strct.h"

/**
 * Checks if the given word is a valid assembly instruction.
 * @param word The string to check.
 * @return 1 if it is an instruction, 0 otherwise.
 */
int is_instr(const char *word);

/**
 * Returns the opcode number of the instruction.
 * @param word The instruction name.
 * @return The opcode index, or -1 if not valid.
 */
int what_opcode(const char *word);

/**
 * Checks if the given word is a valid register name (r0–r7).
 * @param word The string to check.
 * @return Register number 0–7, or -1 if not valid.
 */
int what_reg(const char *word);

/**
 * Duplicates a string (heap-allocated copy).
 * @param s Input string.
 * @return Newly allocated copy of the string.
 */
char *my_strdup(const char *s);

/**
 * Skips leading spaces and tabs in a string.
 * @param str Pointer to the input string.
 * @return Pointer to the first non-space, non-tab character.
 */
char *skip_spaces(char *str);

/**
 * Checks if a word is a reserved keyword in the assembler.
 * @param word String to check.
 * @return 1 if the word is reserved, 0 otherwise.
 */
int is_reserved_word(const char *word);

/**
 * Checks if a string represents a valid register name (r0–r7).
 * @param str String to check.
 * @return 1 if valid register, 0 otherwise.
 */
int is_register(const char *str);

/**
 * Checks if a token is a directive or macro keyword.
 * @param token The string to check.
 * @return 1 if directive/macro keyword, 0 otherwise.
 */
int is_directive(const char *token);

/**
 * Validates a label declaration (ends with ':') according to assembler rules.
 * @param text The label text to check.
 * @return 1 if valid, 0 otherwise.
 */
int is_label(const char *text);

/**
 * Validates a label name (not necessarily with ':' at end).
 * @param text The label name.
 * @return 1 if valid, 0 otherwise.
 */
int is_valid_label_name(const char *text);

/**
 * Checks if a line contains invalid comma usage.
 * @param line The line to check.
 * @return 1 if invalid commas exist, 0 otherwise.
 */
int has_invalid_commas(const char *line);

/**
 * Removes leading and trailing whitespace from a line (in-place).
 * @param str Line to clean.
 * @return Trimmed string.
 */
char *strip_whitespace(char *str);

/**
 * Validates whether a string is a numeric integer.
 * @param text String to validate.
 * @return 1 if numeric, 0 otherwise.
 */
int check_numeric(const char *text);

/**
 * Checks if operand is an immediate value (e.g., "#5").
 * @param operand Operand string.
 * @return 1 if immediate, 0 otherwise.
 */
int is_immediate_value(const char *operand);

/**
 * Determines addressing type for an operand.
 * @param operand Operand string.
 * @return AddressType enum.
 */
AddressType get_addressing_type(const char *operand);

/**
 * Maps opcode name to its numeric value.
 * @param opcode Instruction string.
 * @return Opcode number, or -1 if unknown.
 */
int get_opcode_code(const char *opcode);

/**
 * Parses register operand and returns its index.
 * @param str Register string (e.g., "r3").
 * @return Register index (0-7), or -1 if invalid.
 */
int get_register_index(const char *str);

/**
 * Checks if a string is a valid signed integer.
 * @param s The string to check.
 * @return 1 if valid integer, 0 otherwise.
 */
int is_valid_integer(const char *s);

/**
 * Checks if an opcode requires only destination operand.
 * @param opcode Instruction name.
 * @return 1 if only destination is required, 0 otherwise.
 */
int opcode_requires_only_dst(const char *opcode);

/**
 * Returns expected number of operands for a command.
 * @param cmd Instruction name.
 * @return Expected operand count (0–2).
 */
int expected_operand_count(const char *cmd);

#endif /* INSTR_UTILS_H */

