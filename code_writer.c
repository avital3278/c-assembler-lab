#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "instr_utils.h"
#include "data_strct.h"
#include "line_parser.h"
#include "data_directives.h"
#include "code_writer.h"

int get_instruction_word_count(char *opcode, char *op1, char *op2) {
	 
	int total_words = 1;
	int src_addr_type = ADDR_NONE, dst_addr_type = ADDR_NONE;
	int registers_share_word = 0;
	char base[MAX_LABEL_LENGTH], idx1[MAX_LABEL_LENGTH], idx2[MAX_LABEL_LENGTH];

	(void)opcode;

	if (op1 && op1[0] != '\0') {
		src_addr_type = get_addressing_type(op1);
		switch (src_addr_type) {
			case ADDR_MATRIX:
				if (!extract_matrix_parts(op1, base, idx1, idx2))
					return -1;
				total_words += 2;
				break;
			case ADDR_IMMEDIATE:
			case ADDR_REGISTER:
			case ADDR_DIRECT:
				total_words += 1;
				break;
			case ADDR_NONE:
				break;
			case ADDR_INVALID:
			default:
				return -1;
		}
	}

	if (op2 && op2[0] != '\0') {
		dst_addr_type = get_addressing_type(op2);
		switch (dst_addr_type) {
			case ADDR_MATRIX:
				if (!extract_matrix_parts(op2, base, idx1, idx2))
					return -1;
				total_words += 2;
				break;
			case ADDR_IMMEDIATE:
			case ADDR_REGISTER:
			case ADDR_DIRECT:
				total_words += 1;
				break;
			case ADDR_NONE:
				break;
			case ADDR_INVALID:
			default:
				return -1;
		}

		if (src_addr_type == ADDR_REGISTER && dst_addr_type == ADDR_REGISTER)
			registers_share_word = 1;
	}

	if (registers_share_word)
		total_words -= 1;

	return total_words;
}

int write_operand_code(const char *operand, AddressType type, CodeLine *memory_words, int pos, int *reg_index_out, int start_address) {
	char base[MAX_LABEL_LENGTH], i1[MAX_LABEL_LENGTH], i2[MAX_LABEL_LENGTH];
	*reg_index_out = -1;

	if (type == ADDR_NONE || operand == NULL || strlen(operand) == 0)
		return 0;

	if (type == ADDR_IMMEDIATE) {
		int value = atoi(operand + 1) & 0x3FF;

		memory_words[pos].address = start_address + pos;
		memory_words[pos].word.value = (value << 2);
		memory_words[pos].word.is_label = 0;
		memory_words[pos].word.label_name = NULL;
		return 1;

	} else if (type == ADDR_DIRECT) {

		memory_words[pos].address = start_address + pos;
		memory_words[pos].word.value = 0;
		memory_words[pos].word.is_label = 1;
		memory_words[pos].word.label_name = my_strdup(operand);
		return 1;

	} else if (type == ADDR_REGISTER) {
		*reg_index_out = get_register_index(operand);
		return 0;

	} else if (type == ADDR_MATRIX) {
		if (extract_matrix_parts(operand, base, i1, i2)) {
			int reg1 = get_register_index(i1);
			int reg2 = get_register_index(i2);

			memory_words[pos].address = start_address + pos;
			memory_words[pos].word.value = 0;
			memory_words[pos].word.is_label = 1;
			memory_words[pos].word.label_name = my_strdup(base);

			memory_words[pos + 1].address = start_address + pos + 1;
			memory_words[pos + 1].word.value = ((reg1 & 0xF) << 6) | ((reg2 & 0xF) << 2);
			memory_words[pos + 1].word.is_label = 0;
			memory_words[pos + 1].word.label_name = NULL;
			return 2;
		}
		return -1;
	}

	return -1;
}

int write_register_operands_code(int src_reg, int dst_reg, CodeLine *memory_words, int pos, int start_address) {
	
	if (src_reg == -1 && dst_reg == -1)
		return 0;

	memory_words[pos].address = start_address + pos;
	memory_words[pos].word.is_label = 0;
	memory_words[pos].word.label_name = NULL;
	memory_words[pos].word.value = 0;

	if (src_reg != -1)
		memory_words[pos].word.value |= ((src_reg & 0xF) << 6);
	if (dst_reg != -1)
		memory_words[pos].word.value |= ((dst_reg & 0xF) << 2);

	return 1;
}

int write_instruction_code(const char *opcode, const char *op1, const char *op2, CodeLine *memory_words, int start_address) {
	int word_count = 0;
	int opcode_code;
	AddressType src_type = ADDR_NONE, dst_type = ADDR_NONE;
	int src_reg = -1, dst_reg = -1;
	int op1_result = 0, op2_result = 0;
	int reg_pos = -1;
	int has_op1 = op1 != NULL && strlen(op1) > 0;
	int has_op2 = op2 != NULL && strlen(op2) > 0;
	const char *src_op = NULL;
	const char *dst_op = NULL;
	int src_encoded, dst_encoded;

	/* Opcode encoding */
	opcode_code = get_opcode_code(opcode);
	if (opcode_code == -1) {
		printf("ERROR: Unknown opcode '%s'\n", opcode);
		return 0;
	}

	/* Determine operand types and separate source/destination */
	if (opcode_requires_only_dst(opcode)) {
		src_type = ADDR_NONE;
		dst_type = has_op1 ? get_addressing_type(op1) : ADDR_NONE;
		dst_op = has_op1 ? op1 : NULL;
	} else {
		src_type = has_op1 ? get_addressing_type(op1) : ADDR_NONE;
		dst_type = has_op2 ? get_addressing_type(op2) : ADDR_NONE;
		src_op = has_op1 ? op1 : NULL;
		dst_op = has_op2 ? op2 : NULL;
	}

	/* Write the first word - opcode + operand types */
	memory_words[word_count].address = start_address + word_count;
	src_encoded = (src_type == ADDR_NONE) ? 0 : src_type;
	dst_encoded = (dst_type == ADDR_NONE) ? 0 : dst_type;

	memory_words[word_count].word.value = ((opcode_code & 0xF) << 6) |
										  ((src_encoded & 0x3) << 4) |
										  ((dst_encoded & 0x3) << 2);
	memory_words[word_count].word.value &= 0x3FF;
	memory_words[word_count].word.is_label = 0;
	memory_words[word_count].word.label_name = NULL;
	word_count++;

	/* If both operands are registers - share a word */
	if (src_type == ADDR_REGISTER && dst_type == ADDR_REGISTER) {
		src_reg = get_register_index(src_op);
		dst_reg = get_register_index(dst_op);
		reg_pos = word_count;
		word_count++;
	} else {
		/* Source operand */
		if (src_type == ADDR_REGISTER) {
			src_reg = get_register_index(src_op);
			reg_pos = word_count;
			word_count++;
		} else if (src_type != ADDR_NONE) {
			op1_result = write_operand_code(src_op, src_type, memory_words, word_count, &src_reg, start_address);
			if (op1_result == -1) return 0;
			word_count += op1_result;
		}

		/* Destination operand */
		if (dst_type == ADDR_REGISTER) {
			dst_reg = get_register_index(dst_op);
			reg_pos = word_count;
			word_count++;
		} else if (dst_type != ADDR_NONE) {
			op2_result = write_operand_code(dst_op, dst_type, memory_words, word_count, &dst_reg, start_address);
			if (op2_result == -1) return 0;
			word_count += op2_result;
		}
	}

	/* Write register word if needed */
	if (reg_pos != -1)
		write_register_operands_code(src_reg, dst_reg, memory_words, reg_pos, start_address);

	return word_count;
}

int write_data_word(int value, CodeLine *memory_words, int pos) {
	memory_words[pos].address = 100 + pos;
	memory_words[pos].word.value = (value & 0x3FF) << 2;

	memory_words[pos].word.is_label = 0;
	memory_words[pos].word.label_name = NULL;
	return 1;
}

int write_data_words(const char *directive, const char *line, int IC, int DC, CodeLine *memory_words, int *error_flag) {
	line = skip_spaces((char *)line);
	if (strcmp(directive, ".data") == 0)
		return handle_data_directive(line, IC, DC, memory_words, error_flag);
	else if (strcmp(directive, ".string") == 0)
		return handle_string_directive(line, IC, DC, memory_words, error_flag);
	else if (strcmp(directive, ".mat") == 0)
		return handle_mat_directive(line, IC, DC, memory_words, error_flag);

	*error_flag = 1;
	return 0;
}

