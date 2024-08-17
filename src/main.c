#include <stdio.h>
#include <stdbool.h>
#include "index.h"

void process_input(FILE *in_fp, FILE *out_fp, label_index* index) {
	char c;
	int linecount = 1;
	int instruction = 0;
	int line_len = 0;
	bool comment_flag = false;
	bool lw_flag = true;
	bool whitespace_flag = false;
	bool cw_flag = false;
	bool instr_flag = false;
	char label_buffer[128];

	while ((c = fgetc(in_fp)) != EOF) {

		switch (c) {
			case '#':
				comment_flag = true;
				break;

			case '\n':
				if (instr_flag) fputc('\n', out_fp);
				instruction += instr_flag?1:0;
				linecount += 1;
				line_len = 0;
				comment_flag = false;
				lw_flag = true;
				cw_flag = false;
				instr_flag = false;
				break;

			case ':':
				if (comment_flag) break;
				if (lw_flag) printf("WARN: Empty Label on line %d, Ignoring and moving on...", linecount);
				else {	
					label_buffer[line_len] = '\0';
					add_label(index, label_buffer, instruction);
					fseek(out_fp, -line_len, SEEK_CUR);
					instr_flag = false;
				}
				break;

			case ' ':
			case '\t':
				if (comment_flag) break;
				if (lw_flag || cw_flag) break;
				cw_flag = true;
				whitespace_flag = true;

			default:
				if (comment_flag) break;
				lw_flag = false;
				if (!whitespace_flag) cw_flag = false;
				instr_flag = true;
				label_buffer[line_len] = c;
				fputc(c, out_fp);
				line_len += 1;
		}
		whitespace_flag = false;
	}
}

int main(void) {
	
	FILE *in_fp = fopen("input.s", "r");
	FILE *out_fp = fopen("cleaned.s", "w");
	label_index *index;
	index = new_label_index();
	
	if (!in_fp) {
		printf("FATAL: Failed to read input.s!\nExiting...\n");
		return 1;
	}
	if (!out_fp) {
		printf("FATAL: Failed to open temp.s!\nExiting...\n");
		return 1;
	}

	process_input(in_fp, out_fp, index);
	
	fclose(in_fp);
	fclose(out_fp);
	FILE *code_fp = fopen("cleaned.s", "r");

	debug_print_label_index(index);

	free_label_index(index);
	return 0;
}
