#ifndef CODE_WRITER_H
#define CODE_WRITER_H

/* 
 * code_writer.h
 * 
 * This module provides functionality for translating assembly instructions
 * into machine code words, as well as determining how many memory words 
 * each instruction consumes based on its operands.
 */

#include "data_strct.h"

/**
 * get_instruction_word_count
 * --------------------------
 * Calculates how many memory words are needed to encode an instruction,
 * based on its opcode and operand addressing types.
 * 
 * Parameters:
 *  - opcode: the mnemonic string of the instruction (not used in this function)
 *  - op1: first operand string (can be NULL or empty)
 *  - op2: second operand string (can be NULL or empty)
 * 
 * Returns:
 *  - Number of memory words required to encode the instruction,
 *    or -1 if an invalid addressing mode or syntax is detected.
 */
int get_instruction_word_count(char *opcode, char *op1, char *op2);

/**
 * encode_operand
 * Encodes a single operand into the memory_words array at the specified position.
 * 
 * Parameters:
 *  - operand: operand string (e.g., "#5", "LABEL", "r3", or matrix expression)
 *  - type: addressing type of the operand
 *  - memory_words: array where encoded CodeLine words are stored
 *  - pos: starting position in memory_words array to write the operand words
 *  - reg_index_out: pointer to int to store register index if operand is a register, else -1
 * 
 * Returns:
 *  - number of words written for this operand (usually 0, 1 or 2)
 */
int write_operand_code(const char *operand, AddressType type, CodeLine *memory_words, int pos, int *reg_index_out, int start_address);


/**
 * encode_register_operands
 * Encodes one or two register operands into memory_words, starting at position pos.
 * If both registers are present, they share one word.
 * 
 * Parameters:
 *  - src_reg: index of source register or -1 if none
 *  - dst_reg: index of destination register or -1 if none
 *  - memory_words: array to write encoded register word(s)
 *  - pos: start index to write
 * 
 * Returns:
 *  - number of words written (0, 1, or 2)
 */
int write_register_operands_code(int src_reg, int dst_reg, CodeLine *memory_words, int pos, int start_address);



/**
 * encode_instruction
 * Encodes the full instruction (opcode and operands) into the memory_words array.
 * Uses helper functions to encode operands and registers.
 * 
 * Parameters:
 *  - opcode: instruction mnemonic string
 *  - op1: first operand string (can be NULL)
 *  - op2: second operand string (can be NULL)
 *  - memory_words: array to store encoded CodeLine words
 *  - start_address: starting index in memory_words to write instruction
 * 
 * Returns:
 *  - total number of words written for the instruction (opcode + operands)
 */
int write_instruction_code(const char *opcode, const char *op1, const char *op2,
                       CodeLine *memory_words, int start_address);

/**
 * Encodes a single data word (from .data, .string, or .mat directives)
 * into the memory_words array at the specified position.
 * 
 * This function masks the value to 10 bits and does NOT set the ARE bits,
 * as per project specification for data words.
 * 
 * Parameters:
 *   - value: The integer value to encode (e.g., a data number or char code).
 *   - memory_words: Array of CodeLine structures representing memory.
 *   - pos: The position in the memory_words array to write to.
 * 
 * Returns:
 *   - Number of words written (always 1).
 */
int write_data_word(int value, CodeLine *memory_words, int pos);

/**
 * Writes encoded binary data for a .data, .string, or .mat directive line into memory.
 *
 * This function identifies the type of directive from the line and calls the appropriate
 * handler function to encode the data into memory.
 *
 * @param line         The full line containing the directive (may include label).
 * @param DC           The current data counter (starting address).
 * @param memory_words Array where encoded words will be stored.
 * @param error_flag   Pointer to a flag that will be set to 1 if an error occurs.
 * @return             The number of memory words written, or 0 if an error occurred.
 */
int write_data_words(const char *directive, const char *line, int IC, int DC, CodeLine *memory_words, int *error_flag);
#endif /* CODE_WRITER_H */

