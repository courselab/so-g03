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
        char *machine_code;
} InstructionMapping;

/* Define a structure to hold argument mappings */
typedef struct {
        char *argument;
        char *machine_code;
} ArgumentMapping;

/* Define instruction mappings */
InstructionMapping opcodes[] = {
    {"mov%ah", "b4"}, {"mov%al", "b4"}, {"mov%bx", "bb"}, {"cmp", ""},
    {"je", ""},       {"int", ""},      {"add", ""},      {"jmp", ""},
    {"hlt", ""},      {".string", ""},  {".fill", ""},    {".word", ""},
};

/* Define register mappings */
ArgumentMapping arguments[] = {
    {"$0xe", "0e"},  {"$0x0", "00008a87"}, {"$0x878a0000", "00008a87"}, {"halt", "00"},
    {"$0x10", "10"}, {"$0x1", "1"},        {"0xaa55", "55aa"},          {"msg(%bx)", ""},

};

/*Function to get opcode for a given instruction*/
char *get_opcode(const char *instruction)
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
        return NULL;
}

/*Function to get opcode for a given instruction*/
char *get_argument(const char *argument)
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
        return NULL;
}

char *trim_line(char *str)
{
        int i, j;
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

/* Identify opcodes and replace them by corresponding values*/
const char *pattern_match(char *str)
{
        const char *beggining = str;

        char *instruction = strtok(strdup(str), " ");
        if (instruction == NULL) {
                return beggining;
        } else if (strcmp(instruction, "mov") == 0) {
                /* mov instruction*/
                char *argument = strdup(strtok(NULL, ","));
                char *reg = strtok(NULL, " ");
                strcat(instruction, reg);

                char opcode[255];
                strcpy(opcode, get_opcode(instruction));
                char *argument_code = get_argument(argument);
                strcat(opcode, argument_code);
                strcpy(str, opcode);
        } else if (strcmp(instruction, ".code16") == 0) {
                *str = 0;
        } else if (strcmp(instruction, ".global") == 0) {
                *str = 0;
        } else if (strcmp(instruction, "cmp") == 0) {
                /* cmp instruction */
                /* Loop while char is not 0x0 */
        } else if (strcmp(instruction, "int") == 0) {
                /* Call BIOS video interrupt */
        } else if (strcmp(instruction, ".fill") == 0) {
                /* Pad with zeros */
        } else if (strcmp(instruction, ".word") == 0) {
                /* Boot signature */
        }

        return str;
}

void parse_file(const char *filename)
{
        FILE *input_file = fopen(filename, "r");
        if (input_file == NULL) {
                printf("Error opening file %s.\n", filename);
                exit(1);
        }

        FILE *output_file = fopen("output.bin", "w");
        char line[MAX_LINE_LENGTH];

        while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
                trim_line(line);
                pattern_match(line);
#if DEBUG
                printf("%s", line);
#endif
                fprintf(output_file, "%s", line);
        }

        fclose(input_file);
        fclose(output_file);
}

int main()
{
        parse_file("hw.S");
        return 0;
}
