/*
 *    SPDX-FileCopyrightText: 2024 Luiz Antonio de Abreu Pereira <lap.junior@gmail.com>
 *    SPDX-FileCopyrightText: 2024 Tiago Oliva <tiago.oliva.costa@gmail.com>
 *
 *    SPDX-License-Identifier: GPL-3.0-or-later
 *
 *    This file is part of SYSeg, available at https://gitlab.com/monaco/syseg.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 1
#define MAX_LINE_LENGTH 100 /* assuming lines are less than 100 characters long */

/* Define a structure to hold instruction mappings */
typedef struct {
        char *instruction;
        unsigned int machine_code;
} InstructionMapping;

/* Define a structure to hold argument mappings */
typedef struct {
        char *argument;
        unsigned int machine_code;
} ArgumentMapping;

/* Define instruction mappings */
InstructionMapping opcodes[] = {
    {"mov%ah", 0xb4},  {"mov%al", 0x8a}, {"mov%bx", 0xbb}, {"cmp%al", 0x3c}, {"add%bx", 0x83c3},
    {"je", 0x74},      {"int", 0xcd},    {"jmp", 0xeb},    {"halt:", 0x14},  {"hlt", 0xf4},
    {".string", 0x48}, {".fill", 0x0},   {".word", 0x0},
};

/* Define register mappings */
ArgumentMapping arguments[] = {
    {"$0xe", 0xe}, {"$0x0", 0x0000}, {"$0x1", 0x1},      {"halt", 0x14},
    {"loop", 0x5}, {"$0x10", 0x10},  {"0xaa55", 0x55aa}, {"msg(%bx)", 0x177c},

};

/*Function to get opcode for a given instruction*/
unsigned int get_opcode(const char *instruction)
{
        int i;
        /*Iterate through the lookup table*/
        for (i = 0; i < sizeof(opcodes) / sizeof(opcodes[0]); ++i) {
                /*Compare the instruction*/
                if (strcmp(instruction, opcodes[i].instruction) == 0) {
                        /*Return the corresponding opcode*/
                        return opcodes[i].machine_code;
                }
        }
        /*If instruction not found, return NULL*/
        return -1;
}

/*Function to get opcode for a given instruction*/
unsigned int get_argument(const char *argument)
{
        int i;
        /*Iterate through the lookup table*/
        for (i = 0; i < sizeof(opcodes) / sizeof(opcodes[0]); ++i) {
                /*Compare the instruction*/
                if (strcmp(argument, arguments[i].argument) == 0) {
                        /*Return the corresponding opcode*/
                        return arguments[i].machine_code;
                }
        }
        /*If instruction not found, return NULL*/
        return -1;
}

char *trim_line(char *str)
{
        int i; /* index into input string */
        int j; /* index into output string */
        int len = strlen(str);

        for (i = 0, j = 0; i < len; i++) {
                if (str[i] == '#' && j == 0) {
                        /* Found comment in the beggining of the line, ignore the whole line */
                        break;
                } else if (str[i] == '#') {
                        /* Found comment in the end of the line, ignore the rest of the line */
                        str[j++] = 0;
                        break;
                } else if (str[i] == '\t' || str[i] == '\n') {
                        /* Discard tabs and newline characters */
                        continue;
                }

                /* Otherwise, just copy characters */
                str[j++] = str[i];
        }

        /* Null-terminate the string */
        str[j] = '\0';
        return str;
}

bool check_instruction(char *instruction, char *pattern)
{
        return strcmp(instruction, pattern) == 0;
}

/* Identify opcodes and replace them by corresponding values*/
const char *pattern_match(char *str)
{
        const char *beggining = str;

        char *instruction = strtok(strdup(str), " ");
        if (instruction == NULL) {
                return beggining;
                /************ PREPROCESSOR DIRECTIVES *************/
        } else if (strcmp(instruction, ".code16") == 0) {
                *str = 0;
        } else if (strcmp(instruction, ".global") == 0) {
                *str = 0;
        } else if (check_instruction(instruction, ".string")) {
                /* Hello World string */

                char *text = strtok(NULL, "\"");
                int len = strlen(text);

                /*Convert text to hex.*/
                int i; /* index into input string */
                int j; /* index into output string */
                for (i = 0, j = 0; i < len; ++i, j += 2) {
                        sprintf(str + j, "%02x", text[i] & 0xff);
                }

        } else if (check_instruction(instruction, ".fill")) {
                /* Pad with zeros */
        } else if (strcmp(instruction, ".word") == 0) {
                /* Boot signature */
                /*************************************************/
                /********************** LABELS *******************/
        } else if (strcmp(instruction, "begin:") == 0) {
                *str = 0;
        } else if (strcmp(instruction, "loop:") == 0) {
                *str = 0;
        } else if (strcmp(instruction, "halt:") == 0) {
                *str = 0;
        } else if (strcmp(instruction, "msg:") == 0) {
                *str = 0;
                /*************************************************/
                /********************* COMMANDS ******************/
        } else if (check_instruction(instruction, "mov") || check_instruction(instruction, "cmp") ||
                   check_instruction(instruction, "add")) {
                /* 2 part instruction*/
                char *argument = strdup(strtok(NULL, ","));
                char *reg = strtok(NULL, " ");
                strcat(instruction, reg);

                unsigned char opcode = get_opcode(instruction);

                unsigned char argument_code;
                if (*argument == '$') { /* check first character */
                        /* is a literal number */
                        argument_code = (int)strtol(++argument, NULL, 16); /* number base 16 */
                } else {
                        argument_code = get_argument(argument);
                }

                unsigned int instruction = (opcode << 8) | argument_code;
                sprintf(str, "%x", instruction);
        } else if (check_instruction(instruction, "je") || check_instruction(instruction, "jmp") ||
                   check_instruction(instruction, "int")) {
                /* Jump to halt */
                unsigned char opcode = get_opcode(instruction);

                char *argument = strtok(NULL, " ");
                unsigned char argument_code;
                if (*argument == '$') { /* check first character */
                        /* is a literal number */
                        argument_code = (int)strtol(++argument, NULL, 16); /* number base 16 */
                } else {
                        argument_code = get_argument(argument);
                }

                unsigned int instruction = (opcode << 8) | argument_code;
                sprintf(str, "%x", instruction);

                /* Loop while char is not 0x0 */
        } else if (check_instruction(instruction, "hlt")) {
                unsigned char opcode = get_opcode(instruction);
                sprintf(str, "%x", opcode);
        }
        /*************************************************/

        return str;
}

void write_hex_to_binary_file(const char *hex_array, size_t array_size, FILE *output_file)
{
        size_t i;
        for (i = 0; i < array_size; i += 2) {
                unsigned int value;
                sscanf(&hex_array[i], "%2x", &value);
                fwrite(&value, sizeof(unsigned char), 1, output_file);
        }
}

void parse_file(const char *filename)
{
        FILE *input_file = fopen(filename, "r");
        if (input_file == NULL) {
                printf("Error opening file %s.\n", filename);
                exit(1);
        }

        FILE *output_file = fopen("output.bin", "wb");
        char line[MAX_LINE_LENGTH];
        const char *buffer;

        while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
                trim_line(line);
                buffer = pattern_match(line);
#if DEBUG
                printf("%s", line);
#endif
                write_hex_to_binary_file(buffer, strlen(buffer), output_file);
        }

        fclose(input_file);
        fclose(output_file);
}

int main()
{
        parse_file("hw.S");
        return 0;
}
