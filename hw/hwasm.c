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

#define MAX_LINE_LENGTH                                                        \
  100 /* assuming lines are less than 100 characters long */

char *trim_comment(char *str) {
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

char *trim_tabs(char *str) {
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

void parse_file(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("Error opening file %s.\n", filename);
    exit(1);
  }

  char line[MAX_LINE_LENGTH];
  while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
    trim_tabs(line);
    trim_comment(line);
    printf("%s", line);
  }

  fclose(file);
}

int main() {
  parse_file("hw.S");
  return 0;
}
