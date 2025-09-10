#include <stdio.h>
#include <string.h>
#include "Errors.h"

/* Prints internal errors (not tied to line number) */
void print_internal_error(int code) {
	switch (code) {
		case ERROR_CODE_1:
			fprintf(stderr, "Error 1: Failed to open source file.\n");
			break;
		case ERROR_CODE_2:
			fprintf(stderr, "Error 2: Failed to position file pointer at macro start.\n");
			break;
		case ERROR_CODE_3:
			fprintf(stderr, "Error 3: Macro body is empty.\n");
			break;
		case ERROR_CODE_4:
			fprintf(stderr, "Error 4: Extra text found after 'mcroend'.\n");
			break;
		case ERROR_CODE_5:
			fprintf(stderr, "Error 5: Memory allocation failed.\n");
			break;
		case ERROR_CODE_8:
			fprintf(stderr, "Error 8: Failed to open file for pre-check.\n");
			break;
		case ERROR_CODE_11:
			fprintf(stderr, "Error 11: Failed to get file position (fgetpos).\n");
			break;
		case ERROR_CODE_16:
			fprintf(stderr, "Error 16: Macro used before its declaration.\n");
			break;
		default:
			fprintf(stderr, "Unknown internal error code: %d\n", code);
			break;
	}
}

/* Prints external errors (with location context) */
void print_external_error(int code, location loc) {
	fprintf(stderr, "File: %s | Line: %d | ", loc.file_name, loc.line_number);
	switch (code) {
		case ERROR_CODE_9:
			fprintf(stderr, "Error 9: Missing macro name after 'mcro'.\n");
			break;
		case ERROR_CODE_10:
			fprintf(stderr, "Error 10: Extra text after macro name.\n");
			break;
		case ERROR_CODE_12:
			fprintf(stderr, "Error 12: Line too long (exceeds 80 characters).\n");
			break;
		case ERROR_CODE_17:
			fprintf(stderr, "Error 17: Invalid or reserved macro name.\n");
			break;
		case ERROR_CODE_18:
			fprintf(stderr, "Error 18: Duplicate macro name.\n");
			break;
		case ERR_CODE_19:
			fprintf(stderr, "Error 19: Failed to allocate memory for label list.\n");
			break;
		case ERR_CODE_20:
			fprintf(stderr, "Error 20: Failed to allocate memory for label name.\n");
			break;
		case ERR_CODE_21:
			fprintf(stderr, "Error 21: Duplicate label name.\n");
			break;
		case ERR_CODE_22:
			fprintf(stderr, "Error 22: Label used in instruction but not defined.\n");
			break;
		case ERR_CODE_23:
			fprintf(stderr, "Error 23: Illegal label name.\n");
			break;
		default:
			fprintf(stderr, "Unknown external error code: %d\n", code);
			break;
	}
}

/* Prints internal error with filename */
void print_error_with_file(int code, const char *filename) {
	fprintf(stderr, "File: %s | ", filename);
	print_internal_error(code);
}

/* Prints a general error message */
void print_error(const char *message) {
	fprintf(stderr, "Error: %s\n", message);
}

/* Reports custom error with filename and line number */
void report_error(const char *file_name, int line_number, const char *message) {
	fprintf(stderr, "%s:%d: Error: %s\n", file_name, line_number, message);
}

