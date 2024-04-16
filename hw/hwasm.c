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
    {"mov%ah", "b4"},
    {"mov%ebx", "bb"},
};

/* Define register mappings */
ArgumentMapping arguments[] = {
    {"$0xe", "0e"},
    {"$0x878a0000", "00008a87"},
};

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

char *extract_instruction(char *str)
{
        char *instruction = strstr(str, "mov");
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

/* Identify opcodes and replace them by corresponding values &*/
const char *pattern_match(const char *str)
{

        const char *beggining = str;
        /* mov instruction */
        char *instruction = strstr(str, "mov ");
        if (instruction == NULL) {
                return beggining;
        } else {
                char *reg = strchr(instruction, '%');
                if (reg == NULL) {
                        return beggining;
                } else {
                        strcat(instruction+3, reg);
                }
                /* get register */
                *reg = '\0';
                puts(instruction);
        }
#if 0
        extract_instruction(str);
#endif
        return instruction;
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
                pattern_match(line);
                printf("%s", line);
        }

        fclose(file);
}

#if 0
/* Mapping OpCodes */
mov_ah = "b4"; /* 00 */
mov_ebx = "bb"; /* 00 */
jmp = "eb";
hlt = "f4";
add_eax = "04";


/*Mapping Arguments */
"e" = "0e";
"878a0000" = "00_00_8a_87";
#endif

int main()
{
        parse_file("hw.S");
        return 0;
}
