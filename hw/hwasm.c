/*
 *    SPDX-FileCopyrightText: 2024 Luiz Antonio de Abreu Pereira
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
    {"$0xe", "0e"}, {"$0x0", "00008a87"}, {"$0x878a0000", "00008a87"},
    {"halt", "00"}, {"$0x10", "10"},      {"$0x1", "1"},
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

char *trim_comment(char *str)
{
        int i, j;
        int len = strlen(str);

        for (i = 0, j = 0; i < len; i++) {
                if (str[i] == '#' && j == 0) {
                        /* Found comment in the beggining of the line, ignore the whole line */
                        break;
                } else if (str[i] == '#') {
                        /* Found comment in the end of the line, ignore the rest of the line */
                        str[j++] = '\n';
                        break;
                }

                /* Copy characters if not in a comment*/
                str[j++] = str[i];
        }

        /* Null-terminate the string */
        str[j] = '\0';
        return str;
}

char *trim_tabs(char *str)
{
        int i, j;
        int len = strlen(str);

        for (i = 0, j = 0; i < len; i++) {
                if (str[i] != '\t') {
                        str[j++] = str[i];
                }
        }
        str[j] = '\0'; /* Null-terminate the string */
        return str;
}

char *trim_newline(char *str)
{
        int i, j;
        int len = strlen(str);

        for (i = 0, j = 0; i < len; i++) {
                if (str[i] != '\n') {
                        str[j++] = str[i];
                }
        }
        str[j] = '\0'; /* Null-terminate the string */
        return str;
}

char *extract_instruction(char *str)
{
        char *instruction = strtok(str, " ");
        if (instruction == NULL)
                return NULL;

        char *comma = strchr(instruction, ',');
        printf("%s\n", comma);
        if (comma == NULL) {
                return NULL;
        } else {
                strcat(instruction, comma);
        }

        *comma = '\0'; /* Replace comma with null terminator to separate the instruction*/

        printf("%s", instruction);
        return instruction;
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
        } else if (strcmp(instruction, "cmp") == 0) {
                /* cmp instruction*/
                /* Loop while char is not 0x0 */
        } else if (strcmp(instruction, "int") == 0) {
                /* cmp instruction*/
        }

        return str;
}

void parse_file(const char *filename)
{
        FILE *file = fopen(filename, "r");
        if (file == NULL) {
                printf("Error opening file %s.\n", filename);
                exit(1);
        }

        char line[MAX_LINE_LENGTH];

        while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
                trim_tabs(line);
                trim_comment(line);
                trim_newline(line);
                pattern_match(line);
                printf("%s", line);
        }

        fclose(file);
}

int main()
{
        parse_file("hw.S");
        return 0;
}
