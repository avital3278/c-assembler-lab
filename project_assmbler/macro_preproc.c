#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "macro_prepro.h"
#include "Errors.h"
#include "data_strct.h"
#include "instr_utils.h"

#define MAX_LINE_LENGTH 82



char *trim_whitespace(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
    return str;
}

char *extract_macro_body(FILE *source_file, fpos_t *macro_start, int *current_line) {
    char line_buffer[MAX_LINE_LENGTH];
    long total_length = 0;
    char *macro_text = NULL;
    char *clean_line;

    if (fsetpos(source_file, macro_start) != 0) {
        print_internal_error(ERROR_CODE_2);
        return NULL;
    }

    while (fgets(line_buffer, MAX_LINE_LENGTH, source_file)) {
        (*current_line)++;
        clean_line = trim_whitespace(line_buffer);
        if (strcmp(clean_line, "mcroend") == 0) break;
        total_length += strlen(line_buffer);
    }

    macro_text = (char *) malloc((total_length + 1) * sizeof(char));
    if (!macro_text) {
        print_internal_error(ERROR_CODE_5);
        return NULL;
    }
    macro_text[0] = '\0';

    if (fsetpos(source_file, macro_start) != 0) {
        free(macro_text);
        print_internal_error(ERROR_CODE_2);
        return NULL;
    }

    while (fgets(line_buffer, MAX_LINE_LENGTH, source_file)) {
        clean_line = trim_whitespace(line_buffer);
        if (strcmp(clean_line, "mcroend") == 0) break;
        strcat(macro_text, clean_line);
        strcat(macro_text, "\n");
    }

    return macro_text;
}

int add_macro_to_list(macro_t **head, const char *name, const char *content, int line_number) {
    macro_t *new_macro = (macro_t *) malloc(sizeof(macro_t));
    if (!new_macro) {
        print_internal_error(ERROR_CODE_5);
        return 0;
    }

    new_macro->name = my_strdup(name);
    new_macro->content = my_strdup(content);
    new_macro->line = line_number;
    new_macro->next = NULL;

    if (!new_macro->name || !new_macro->content) {
        free(new_macro->name);
        free(new_macro->content);
        free(new_macro);
        print_internal_error(ERROR_CODE_5);
        return 0;
    }

    if (!*head) {
        *head = new_macro;
    } else {
        macro_t *current = *head;
        while (current->next) current = current->next;
        current->next = new_macro;
    }
    return 1;
}

int remove_macro_definitions(FILE *src, FILE *dest, const char *file_name) {
    char line[MAX_LINE_LENGTH];
    int in_macro = 0;
    (void)file_name;

    while (fgets(line, MAX_LINE_LENGTH, src)) {
        char line_copy[MAX_LINE_LENGTH];
        char *token;
        strcpy(line_copy, line);
        token = strtok(line_copy, " \t\n");

        if (token && strcmp(token, "mcro") == 0) {
            in_macro = 1;
            continue;
        }
        if (token && strcmp(token, "mcroend") == 0) {
            in_macro = 0;
            continue;
        }
        if (!in_macro) fputs(line, dest);
    }
    return 1;
}

void free_macro_table(macro_t *head) {
    macro_t *current = head;
    while (current) {
        macro_t *next = current->next;
        free(current->name);
        free(current->content);
        free(current);
        current = next;
    }
}

void expand_macros_in_file(FILE *src, FILE *dest, macro_t *macro_list) {
    char line[MAX_LINE_LENGTH];
    char trimmed[MAX_LINE_LENGTH];
    char label[MAX_LABEL_LENGTH];
    int line_number;
    int matched;
    size_t len;
    char *colon;
    char *token;
    macro_t *m;
    char *macro_copy;
    char *line_part;
    char *after_colon;

    line_number = 0;

    while (fgets(line, MAX_LINE_LENGTH, src)) {
        line_number++;
        strcpy(trimmed, trim_whitespace(line));
        label[0] = '\0';
        matched = 0;

        if (strlen(trimmed) == 0 || trimmed[0] == ';') {
            continue;
        }

        colon = strchr(trimmed, ':');
        if (colon) {
            len = colon - trimmed + 1;
            if (len >= MAX_LABEL_LENGTH) len = MAX_LABEL_LENGTH - 1;
            sprintf(label, "%.*s", (int)len, trimmed);  
            label[len] = '\0';
            after_colon = trim_whitespace(colon + 1);
            token = after_colon;
        } else {
            token = strtok(trimmed, " \t\n");
        }

        if (!token) {
            continue;
        }

        for (m = macro_list; m; m = m->next) {
            if (strcmp(token, m->name) == 0) {
                macro_copy = my_strdup(m->content);
                if (!macro_copy) {
                    print_internal_error(ERROR_CODE_5);
                    return;
                }
                matched = 1;
                line_part = strtok(macro_copy, "\n");

                if (label[0] != '\0' && line_part) {
                    fprintf(dest, "%s\t%s\n", label, line_part);
                    line_part = strtok(NULL, "\n");
                }

                while (line_part) {
                    fprintf(dest, "\t%s\n", line_part);
                    line_part = strtok(NULL, "\n");
                }

                free(macro_copy);
                break;
            }
        }

        if (!matched) {
            if (label[0] != '\0') {
                fprintf(dest, "%s\t%s\n", label, after_colon);
            } else {
                fprintf(dest, "\t%s\n", trim_whitespace(line));
            }
        }
    }
}

int collect_macro_table(FILE *fp, macro_t **macro_head) {
    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    int error_found = 0;
    fpos_t macro_start;

    while (fgets(line, MAX_LINE_LENGTH, fp)) {
        char line_copy[MAX_LINE_LENGTH];
        char *name_token, *macro_name;
        char *macro_body;

        line_number++;
        if (!strstr(line, "mcro")) continue;

        if (fgetpos(fp, &macro_start) != 0) {
            print_internal_error(ERROR_CODE_11);
            return 0;
        }

        strcpy(line_copy, line);
        strtok(line_copy, " \t\n");
        name_token = strtok(NULL, " \t\n");
        macro_name = my_strdup(name_token);

        macro_body = extract_macro_body(fp, &macro_start, &line_number);
        if (!macro_name || !macro_body) {
            free(macro_name);
            free(macro_body);
            error_found = 1;
            continue;
        }

        if (!error_found && !add_macro_to_list(macro_head, macro_name, macro_body, line_number)) {
            free(macro_name);
            free(macro_body);
            return 0;
        }

        free(macro_name);
        free(macro_body);
    }

    return !error_found;
}

int preprocess_macros(const char *source_filename) {
    FILE *source_file = NULL, *temp_file = NULL, *output_file = NULL;
    char am_filename[FILENAME_MAX];
    char temp_filename[] = "temp_macro.am";
    macro_t *macro_list = NULL;

    source_file = fopen(source_filename, "r");
    if (!source_file) {
        print_error_with_file(ERROR_CODE_1, source_filename);
        return 0;
    }

    if (!collect_macro_table(source_file, &macro_list)) {
        fclose(source_file);
        free_macro_table(macro_list);
        return 0;
    }

    rewind(source_file);
    temp_file = fopen(temp_filename, "w+");
    if (!temp_file) {
        fclose(source_file);
        free_macro_table(macro_list);
        print_error("Failed to create temp file.");
        return 0;
    }

    remove_macro_definitions(source_file, temp_file, source_filename);
    rewind(temp_file);
    sprintf(am_filename, "%s.am", source_filename);
    output_file = fopen(am_filename, "w");

    if (!output_file) {
        fclose(source_file);
        fclose(temp_file);
        free_macro_table(macro_list);
        print_error("Failed to create output .am file.");
        return 0;
    }

    expand_macros_in_file(temp_file, output_file, macro_list);

    fclose(source_file);
    fclose(temp_file);
    fclose(output_file);
    free_macro_table(macro_list);
    return 1;
}
