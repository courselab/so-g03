/*
 *    SPDX-FileCopyrightText: 2024 Luiz Antonio de Abreu Pereira <lap.junior@gmail.com>
 *    SPDX-FileCopyrightText: 2024 Monaco F. J. <monaco@usp.br>
 *    SPDX-FileCopyrightText: 2024 Tiago Oliva <tiago.oliva.costa@gmail.com>
 *
 *    SPDX-License-Identifier: GPL-3.0-or-later
 *
 *    This file is part of SYSeg, available at https://gitlab.com/monaco/syseg.
 */

#ifndef UTILS_H
#define UTILS_H

int __attribute__((fastcall)) strcmp(const char *s1, const char *s2);
char *str_to_lower(char *str);
int to_lower(int chr);
int strlen(char *str);

#endif /* UTILS_H  */
