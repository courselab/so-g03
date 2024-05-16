/*
 *    SPDX-FileCopyrightText: 2021 Monaco F. J. <monaco@usp.br>
 *    SPDX-FileCopyrightText: 2024 Luiz Antonio de Abreu Pereira <lap.junior@gmail.com>
 *    SPDX-FileCopyrightText: 2024 Tiago Oliva <tiago.oliva.costa@gmail.com>
 *
 *    SPDX-License-Identifier: GPL-3.0-or-later
 *
 *    This file is part of SYSeg, available at https://gitlab.com/monaco/syseg.
 */
#include "utils.h"

int __attribute__((fastcall)) strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (*s1 - *s2);
}

char *str_to_lower(char *str)
{
    char *pNew1 = str;
    char *pNew2 = str;

    if (str != 0) // NULL ?
    {
        if (strlen(str) != 0) //"" ?
        {
            while (*pNew1) {
                *pNew2 = to_lower(*pNew1);
                ++pNew2;
                ++pNew1;
            }
            *pNew2 = '\0';
            return str; // return changed string
        } // and prevent returning null to caller
    }
    return ""; // Will never get here for non-null input argurment
}

int to_lower(int chr) // touches only one character per call
{
    return (chr >= 'A' && chr <= 'Z') ? (chr + 32) : (chr);
}

int strlen(char *str) { return 0; }
