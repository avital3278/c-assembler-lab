#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data_strct.h"
#include "label.h"
#include "macro_prepro.h"	
#include "step1.h"			
#include "pass2.h"			


static char *add_ext(const char *base, const char *ext) {
	size_t n = strlen(base) + strlen(ext) + 1;
	char *p = (char*)malloc(n);
	if (p) { strcpy(p, base); strcat(p, ext); }
	return p;
}


static void strip_last_ext(const char *src, char *dst) {
	const char *dot = strrchr(src, '.');
	if (dot) {
		size_t n = (size_t)(dot - src);
		memcpy(dst, src, n);
		dst[n] = '\0';
	} else {
		strcpy(dst, src);
	}
}

int main(int argc, char **argv) {
	int i, any_fail = 0;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s file1 [file2 ...]   (name without extension)\n", argv[0]);
		return 1;
	}

	for (i = 1; i < argc; ++i) {
		char base[FILENAME_MAX];
		char *as_path = add_ext(argv[i], ".as");    
		char *am_path = NULL;                        

		CodeLine *code = NULL;
		LabelEntry *labels = NULL;
		EntryExternRef *entries = NULL, *externs = NULL;
		int IC = 0, DC = 0, err = 0, label_count = 0;
		int entry_count = 0, extern_count = 0;
		int ok, code_size;

		if (!as_path) { fprintf(stderr, "alloc failed\n"); any_fail = 1; goto next_file; }

		
		if (!preprocess_macros(as_path)) {
			fprintf(stderr, "[%s] macro preprocess failed\n", argv[i]);
			any_fail = 1; goto next_file;
		}

		
		am_path = add_ext(as_path, ".am");
		if (!am_path) { fprintf(stderr, "alloc failed\n"); any_fail = 1; goto next_file; }

		
		ok = step1(am_path,
			&code, &labels,
			&IC, &DC,
			&err, &label_count,
			&entries, &entry_count,
			&externs, &extern_count);

		if (!ok || err) {
			fprintf(stderr, "[%s] step1 failed\n", argv[i]);
			any_fail = 1; goto next_file;
		}

		code_size = IC + DC - 100;
		if (code_size < 0) code_size = 0;

		
		strip_last_ext(argv[i], base);

		
		ok = pass2_resolve_and_write(
			base,
			code, code_size,
			labels, label_count,
			entries, entry_count,
			externs, extern_count,
			IC, DC, &err);

		if (!ok || err) {
			fprintf(stderr, "[%s] pass2 failed\n", argv[i]);
			any_fail = 1; goto next_file;
		}

		printf("[%s] OK → %s.ob%s%s\n",
			argv[i], base,
			(entry_count > 0 ? " + .ent" : ""),
			(extern_count > 0 ? " + .ext" : ""));

next_file:
		if (code) free(code);
		if (labels) free_name_list(labels, label_count, 1);
		if (entries) free_name_list(entries, entry_count, 0);
		if (externs) free_name_list(externs, extern_count, 0);
		if (as_path) free(as_path);
		if (am_path) free(am_path);
	}

	return any_fail ? 1 : 0;
}

