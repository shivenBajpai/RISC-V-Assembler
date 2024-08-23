#include <stdio.h>
#include <stdbool.h>
#include "vec.h"
#include "index.h"
#include "translator.h"

int first_pass(FILE *in_fp, FILE *out_fp, label_index* index, Vec* line_mapping) {
	char c;
	char label_buffer[128];
	int linecount = 1;
	int instruction = 0;
	int line_len = 0;
	bool comment_flag = false;
	bool lw_flag = true;
	bool whitespace_flag = false;
	bool cw_flag = false;
	bool instr_flag = false;

	while ((c = fgetc(in_fp)) != EOF) {

		switch (c) {
			case '#':
			case ';':
				comment_flag = true;
				break;

			case '\n':
				if (instr_flag) {
					fputc('\n', out_fp);
					append(line_mapping, linecount);
					instruction += 1;
				} 
				linecount += 1;
				line_len = 0;
				comment_flag = false;
				lw_flag = true;
				cw_flag = false;
				instr_flag = false;
				break;

			case ':':
				if (comment_flag) break;
				if (lw_flag) printf("WARN: Empty Label on line %d, Ignoring and moving on...\n", linecount);
				if (line_len >= 128) printf("ERROR: Label too long on line %d, Stopping...\n", linecount);
				else {	
					label_buffer[line_len] = '\0';
					add_label(index, label_buffer, instruction);
					fseek(out_fp, -line_len, SEEK_CUR);
					instr_flag = false;
					lw_flag = true;
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
				if (line_len<128) label_buffer[line_len] = c;
				fputc(c, out_fp);
				line_len += 1;
		}
		whitespace_flag = false;
	}
	fputc('\n', out_fp);
	return 0;
}

int second_pass(FILE* clean_fp, int* hexcode, label_index* index, Vec* line_mapping) {
	
	char name[8]; // Sufficient for any valid instruction/pseudo instruction in Base class
	int instruction_count = 0;
	int i = 0;
	char c;

	while ((c = fgetc(clean_fp)) != EOF) {
		if (i<8) {
			if (c == ' ' || c == '\t') {
				name[i] = '\0';

				// Attempt instruction translation
				
				const instruction_info* instruction = search_instruction(name);

				if (!instruction) {
					printf("Error on line %d\nUnknown Instruction: %s\n                     ^^^^^^^^\n", line_mapping->values[instruction_count], name);
					return 1;
				}

				int addend;
				bool fail_flag = false;

				switch (instruction->handler_type) {
					case R_TYPE:
						addend = R_type_parser(&clean_fp, index, &line_mapping->values[instruction_count], instruction_count, &fail_flag);
						break;

					case I1_TYPE:
						addend = I1_type_parser(&clean_fp, index, &line_mapping->values[instruction_count], instruction_count, &fail_flag);
						break; 

					case I2_TYPE:
						addend = I2_type_parser(&clean_fp, index, &line_mapping->values[instruction_count], instruction_count, &fail_flag);
						break; 

					case S_TYPE:
						addend = S_type_parser(&clean_fp, index, &line_mapping->values[instruction_count], instruction_count, &fail_flag);
						break; 

					case B_TYPE:
						addend = B_type_parser(&clean_fp, index, &line_mapping->values[instruction_count], instruction_count, &fail_flag);
						break; 

					case U_TYPE:
						addend = U_type_parser(&clean_fp, index, &line_mapping->values[instruction_count], instruction_count, &fail_flag);
						break; 

					case J_TYPE:
						addend = J_type_parser(&clean_fp, index, &line_mapping->values[instruction_count], instruction_count, &fail_flag);
						break; 

					case I3_TYPE:
						addend = I3_type_parser(&clean_fp, index, &line_mapping->values[instruction_count], instruction_count, &fail_flag);
						break; 

					case I4_TYPE:
						break;
						
					default:
						printf("Error on line %d\nUnclassified type, did you forget to write a case?\n", line_mapping->values[instruction_count]);
						return 1;
				}
				
				if (fail_flag) return 1;
				hexcode[instruction_count] = instruction->constant + (unsigned int) addend;
				printf("Wrote instruction %d: %d\n", instruction_count, hexcode[instruction_count]);
				instruction_count++;
				i = 0;
			} else name[i++] = c;
		} else {
			name[7] = '\0';
			printf("Error on line %d\nInvalid Instruction: %s\n                     ^^^^^^^^\n", line_mapping->values[instruction_count], name);
			return 1;
		}

	}
		
	return 0;
}

int main(void) {
	
	FILE *in_fp = fopen("input.s", "r");
	FILE *clean_fp = fopen("cleaned.s", "w+");
	label_index *index;
	Vec *line_mapping;
	int result;
	index = new_label_index();
	line_mapping = new_managed_array();

	if (!in_fp) {
		printf("FATAL: Failed to read input.s!\nExiting...\n");
		return 1;
	}
	if (!clean_fp) {
		printf("FATAL: Failed to open cleaned.s!\nExiting...\n");
		return 1;
	}

	if ((result = first_pass(in_fp, clean_fp, index, line_mapping)) != 0) {
		printf("FATAL: Code Parsing failed with code %d\nExiting...\n", result);
		return 1;
	}
	
	fclose(in_fp);
	fseek(clean_fp, 0, SEEK_SET);

	int hexcode[line_mapping->len];

	debug_print_label_index(index); // Uncomment This line to get a list of all labels and corresponding instruction numbers in output
	
	if ((result = second_pass(clean_fp, hexcode, index, line_mapping)) != 0) {
		printf("FATAL: Code Compilation failed with code %d\nExiting...\n", result);
		return 1;
	}

	FILE *output_fp = fopen("output.hex", "wb");
	if (!output_fp) {
		printf("FATAL: Failed to write to output.hex!\nExiting...\n");
		return 1;
	}

	fwrite(hexcode, 4, (line_mapping->len), output_fp);
	fclose(output_fp);
	fclose(clean_fp);

	free_label_index(index);
	free_managed_array(line_mapping);
	return 0;
}
