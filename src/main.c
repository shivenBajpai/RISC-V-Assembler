#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "vec.h"
#include "index.h"
#include "translator.h"

// Reads in_fp and writes the same to out_fp while ignoring all whitespace, comments and labels (but makes a note of label positions)
int first_pass(FILE *in_fp, FILE *out_fp, label_index* index, vec* line_mapping) {
	char c;
	char label_buffer[128];
	int linecount = 1;
	int instruction_count = 0;
	int line_len = 0;
	bool comment_flag = false; // Are we reading a comment
	bool lw_flag = true; // leading whitespace flag
	bool cw_flag = false; // consecutive whitespace flag
	bool whitespace_flag = false; // is the current charachter whitespace (different rules are followed to decide if it should be written to output)
	bool instr_flag = false; // Was there an instruction on this line
	bool keep_reading = true;

	while (keep_reading) {
		c = fgetc(in_fp);
		switch (c) {
			case '#':
			case ';':
				comment_flag = true;
				break;

			case EOF:
				keep_reading = false;
			case '\n':
				if (instr_flag) {
					fputc('\n', out_fp);
					append(line_mapping, linecount);
					instruction_count += 1;
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
				if (lw_flag) { 
					printf("WARN: Empty Label on line %d, Stopping...\n", linecount);
					return -1;
				}
				if (line_len >= 128) {
					printf("ERROR: Label too long on line %d, Stopping...\n", linecount);
					return -2;
				}

				label_buffer[line_len] = '\0';
				int prev_line = label_to_position(index, label_buffer);
				if (prev_line != -1) {
					printf("ERROR: Pre-existing label defined on line %d repeated on %d, Stopping...\n", line_mapping->values[prev_line], linecount);
					return -3;
				}
				
				add_label(index, label_buffer, instruction_count);
				fseek(out_fp, -line_len, SEEK_CUR);
				instr_flag = false;
				lw_flag = true;
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
	return 0;
}


// Actually Encode all the instructions and write it to the int array.
int second_pass(FILE* clean_fp, int* hexcode, label_index* index, vec* line_mapping, bool debug) {
	
	char name[8]; // Sufficient for any valid instruction/pseudo instruction in Base class
	int instruction_count = 0;
	int i = 0;
	char c;

	while ((c = fgetc(clean_fp)) != EOF) {
		if (i<8) {
			if (c == ' ' || c == '\t' || c == '\n') {
				name[i] = '\0';

				// Attempt instruction translation
				
				// Checking name of instruction against known instructions
				const instruction_info* instruction = parse_instruction(name);

				if (!instruction) {
					printf("Error on line %d\nUnknown Instruction: %s\n                     ^^^^^^^^\n", line_mapping->values[instruction_count], name);
					return 1;
				}

				int addend = 0;
				bool fail_flag = false;

				// Parsing arguments of instruction
				switch (instruction->handler_type) {
					case R_TYPE:
						addend = R_type_parser(&clean_fp, index, &line_mapping->values[instruction_count], instruction_count, &fail_flag);
						break;

					case I1_TYPE:
						addend = I1_type_parser(&clean_fp, index, &line_mapping->values[instruction_count], instruction_count, &fail_flag);
						break; 

					case I1B_TYPE:
						addend = I1B_type_parser(&clean_fp, index, &line_mapping->values[instruction_count], instruction_count, &fail_flag);
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
						printf("Error on line %d\nUnclassified type, This should not have happened!\n", line_mapping->values[instruction_count]);
						return 1;
				}
				
				if (fail_flag) return 1;

				// Write the instruction to the output buffer
				hexcode[instruction_count] = instruction->constant + addend;
				if (debug) printf("Instruction %d: %08X\n", instruction_count, hexcode[instruction_count]);
				
				instruction_count++;
				i = 0;

			} else name[i++] = c;
		} else {
			// No valid instruction would be this long
			name[7] = '\0';
			printf("Error on line %d\nInvalid Instruction: %s\n                     ^^^^^^^^\n", line_mapping->values[instruction_count], name);
			return 1;
		}

	}
		
	return 0;
}

int main(int argc, char **argv) {

	// Initializing and Parsing command line switches

	bool debug = false;
	bool binary = false;

	char *input_file_name = "input.s";
	char *output_file_name = "output.hex";

	for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i],"-d")==0 || strcmp(argv[i],"--debug")==0) debug = true;

		if (strcmp(argv[i],"-b")==0 || strcmp(argv[i],"--binary")==0) binary = true;

		if (strcmp(argv[i],"-i")==0 || strcmp(argv[i],"--input")==0) {
			i++;
			if (i==argc) {
				printf("Missing input file name for option input\n");
				return 1;
			}

			input_file_name = argv[i];

		}

		if (strcmp(argv[i],"-o")==0 || strcmp(argv[i],"--output")==0) {
			i++;
			if (i==argc) {
				printf("Missing input file name for option output\n");
				return 1;
			}

			output_file_name = argv[i];
		}
    }
	
	// Prepare for the First pass
	FILE *in_fp = fopen(input_file_name, "r");
	FILE *clean_fp = fopen("cleaned.s", "w+");
	label_index *index;
	vec *line_mapping;
	int result;
	index = new_label_index();
	line_mapping = new_managed_array();

	if (!in_fp) {
		printf("FATAL: Failed to read %s!\nExiting...\n", input_file_name);
		return 1;
	}
	if (!clean_fp) {
		printf("FATAL: Failed to open cleaned.s!\nExiting...\n");
		return 1;
	}

	// Perform the first pass
	if ((result = first_pass(in_fp, clean_fp, index, line_mapping)) != 0) {
		printf("FATAL: Code Parsing failed with code %d\nExiting...\n", result);
		return 1;
	}

	// Prepare for the second pass	
	fclose(in_fp);
	fseek(clean_fp, 0, SEEK_SET);

	int hexcode[line_mapping->len];

	if (debug) {
		printf("Labels:\n");
		debug_print_label_index(index);
		printf("\n");
	}
	
	// Perform the second pass
	if ((result = second_pass(clean_fp, hexcode, index, line_mapping, debug)) != 0) {
		printf("FATAL: Code Compilation failed with code %d\nExiting...\n", result);
		return 1;
	}

	// Write to output.hex
	FILE *output_fp = fopen(output_file_name, "wb");
	if (!output_fp) {
		printf("FATAL: Failed to write to %s!\nExiting...\n", output_file_name);
		return 1;
	}

	if (binary) fwrite(hexcode, 4, (line_mapping->len), output_fp);
	else {
		for (int i=0; i<line_mapping->len; i++) {
			fprintf(output_fp, "%08x\n", hexcode[i]);
		}
	}
	fclose(output_fp);
	fclose(clean_fp);

	free_label_index(index);
	free_managed_array(line_mapping);
	return 0;
}
