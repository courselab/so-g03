/*
 *    SPDX-FileCopyrightText: 2024 Luiz Antonio de Abreu Pereira <lap.junior@gmail.com>
 *    SPDX-FileCopyrightText: 2024 Tiago Oliva <tiago.oliva.costa@gmail.com>
 *
 *    SPDX-License-Identifier: GPL-3.0-or-later
 *
 *    This file is part of SYSeg, available at https://gitlab.com/monaco/syseg.
 */

#include <limits.h>  /* for CHAR_BIT */
#include <stdbool.h> /* for TRUE and FALSE */
#include <stdio.h>   /* for printf and file handling */
#include <stdlib.h>
#include <string.h> /* for multiple string operations */

#define DEBUG 1
#define MAX_LINE_LENGTH 100 /* assuming lines are less than 100 characters long */
#define BOOT_SECTOR_SIZE 512
#define INPUT_FILE "hw.S"

unsigned int writer_count;

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
    {"mov%ah", 0xb4},  {"mov%al", 0x8a87}, {"mov%bx", 0xbb}, {"cmp%al", 0x3c}, {"add%bx", 0x83c3},
    {"je", 0x74},      {"int", 0xcd},      {"jmp", 0xeb},    {"halt:", 0x14},  {"hlt", 0xf4},
    {".string", 0x48}, {".fill", 0x0},     {".word", 0x0},
};

/* Define register mappings */
ArgumentMapping arguments[] = {
    {"$0xe", 0xe}, {"$0x0", 0x0000}, {"$0x1", 0x1},      {"halt", 0x07},
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

/* Jump instructions are calculated in a relative basis to the label.
 * Halt label is address 0x14. The argument is calculated as
 *      argument = label - current_pointer (1)
 * For instance
 *      0b: 74 07
 * is je 07 bytes ahead. That is 0b = 11, after 74 07 the instruction
 * pointer (IP) is at location 13, jump +7 from 13 means jump to
 * location 20, i.e. 0x14 in hex. From Eq.(1):
 *      0x07 = 0x14 - 0x13.
 *
 * */
int process_jmp(char *instruction, char *argument)
{
        /*  b:  74 07   je  0x14
         *  12: eb f1   jmp 0x5
         *  15: eb fd   jmp 0x14
         * */
        return -1;
}

unsigned int process_instruction(char *instruction, char *argument)
{
        unsigned int opcode = get_opcode(instruction);

        unsigned int argument_code;

        /* check first character */
        if (*argument == '$') {
                /* is a literal number */
                argument_code = (int)strtol(++argument, NULL, 16); /* number base 16 */
        } else {
                argument_code = get_argument(argument);
        }

        unsigned int argument_code_size = sizeof(argument_code) * CHAR_BIT / 4; /* default */
        if (opcode == get_opcode("mov%bx") || opcode == get_opcode("mov%al")) {
                argument_code_size = 16;
        }

        return (opcode << argument_code_size) | argument_code;
}

/* Identify opcodes and replace them by corresponding values*/
void pattern_match(char *str, char *out)
{
        char *instruction = strtok(strdup(str), " ");
        if (instruction == NULL) {
                return;
                /************ PREPROCESSOR DIRECTIVES *************/
        } else if (strcmp(instruction, ".code16") == 0) {
                *out = 0;
        } else if (strcmp(instruction, ".global") == 0) {
                *out = 0;
        } else if (check_instruction(instruction, ".string")) {
                /* Hello World string */
                char *text = strtok(NULL, "\"");
                int len = strlen(text);

                /*Convert text to hex.*/
                int i; /* index into input string */
                int j; /* index into output string */
                for (i = 0, j = 0; i < len; ++i, j += 2) {
                        sprintf(out + j, "%02x", text[i] & 0xff);
                }

        } else if (check_instruction(instruction, ".fill")) {
                /* Pad with zeros */
                int i; /* index into input string */
                for (i = 0; i < 510 - writer_count; ++i) {
                        sprintf(out + i, "%x", 0);
                }
        } else if (check_instruction(instruction, ".word")) {
                /* Boot signature */
                /*************************************************/
                /********************** LABELS *******************/
                sprintf(out, "%x", 0x55aa);
        } else if (strcmp(instruction, "begin:") == 0) {
                *out = 0;
        } else if (strcmp(instruction, "loop:") == 0) {
                *out = 0;
        } else if (strcmp(instruction, "halt:") == 0) {
                *out = 0;
        } else if (strcmp(instruction, "msg:") == 0) {
                *out = 0;
                /*************************************************/
                /********************* COMMANDS ******************/
        } else if (check_instruction(instruction, "mov") || check_instruction(instruction, "cmp") ||
                   check_instruction(instruction, "add")) {
                /* 2 part instruction*/
                char *argument = strdup(strtok(NULL, ","));
                char *reg = strtok(NULL, " ");
                strcat(instruction, reg);

                unsigned int hex_instruction = process_instruction(instruction, argument);
                sprintf(out, "%x", hex_instruction);
        } else if (check_instruction(instruction, "je") || check_instruction(instruction, "jmp") ||
                   check_instruction(instruction, "int")) {
                char *argument = strtok(NULL, " ");

                unsigned int hex_instruction = process_instruction(instruction, argument);
                sprintf(out, "%x", hex_instruction);
        } else if (check_instruction(instruction, "hlt")) {
                unsigned char opcode = get_opcode(instruction);
                sprintf(out, "%x", opcode);
        }
        /*************************************************/
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

void parse_line(char *line, char *buffer)
{
        trim_line(line);

        char out[BOOT_SECTOR_SIZE] = {0};
        pattern_match(line, out);
        if (!*out) {
                return;
        }

        strcat(buffer, out);
#if DEBUG
        printf("%s", out);
#endif
}

int main()
{
        FILE *input_file = fopen(INPUT_FILE, "r");
        if (input_file == NULL) {
                printf("Error opening file %s.\n", INPUT_FILE);
                exit(1);
        }

        FILE *output_file = fopen("output.bin", "wb");
        char line[MAX_LINE_LENGTH] = {0};
        char buffer[BOOT_SECTOR_SIZE] = {0};

        while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
                parse_line(line, buffer);

                writer_count = strlen(buffer);
        }

        write_hex_to_binary_file(buffer, writer_count, output_file);

        fclose(input_file);
        fclose(output_file);
        return 0;
}
