/*
 *    SPDX-FileCopyrightText: 2021 Monaco F. J. <monaco@usp.br>
 *    SPDX-FileCopyrightText: 2024 Luiz Antonio de Abreu Pereira <lap.junior@gmail.com>
 *    SPDX-FileCopyrightText: 2024 Tiago Oliva <tiago.oliva.costa@gmail.com>
 *
 *    SPDX-License-Identifier: GPL-3.0-or-later
 *
 *    This file is part of SYSeg, available at https://gitlab.com/monaco/syseg.
 */

#include "bios.h"
#include "utils.h"

#define PROMPT "$ " /* Prompt sign.      */
#define SIZE 20     /* Read buffer size. */

char buffer[SIZE]; /* Read buffer.      */

/* TODO
 * Implement a new built-in command.
 *
 * Think of a new command, such as to return the current date, print the
 * available RAM memory, draw some cool graphics using video memory...
 * or anything else you feel like.
 *
 */
int main()
{
    clear();

    println("Boot Command 1.0");

    while (1) {
        print(PROMPT);  /* Show prompt.               */
        readln(buffer); /* Read use input.            */

        char * p = buffer;
        for (; *p; ++p){
            *p = to_lower(*p);
        }

        if (buffer[0]) /* Execute built-in command.  */
        {
            if (!strcmp(buffer, "help")) {
                println("A Beattles's song.");
            } else if (!strcmp(buffer, "ram")) {
                println("Available RAM is 42 MB");
            } else {
                println("Unkown command.");
            }
        }
    }

    return 0;
}
