CC = gcc
CFLAGS = -std=c90 -Wall -Wextra -pedantic -I.
SRCS = assembler.c pass2.c step1.c macro_preproc.c code_writer.c data_directives.c line_parser.c label.c instr_utils.c Errors.c
TARGET = assembler

$(TARGET):
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

