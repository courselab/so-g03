/*
 *    SPDX-FileCopyrightText: 2024 Luiz Antonio de Abreu Pereira <lap.junior@gmail.com>
 *    SPDX-FileCopyrightText: 2024 Tiago Oliva <tiago.oliva.costa@gmail.com>
 *    SPDX-FileCopyrightText: 2024 Monaco F. J. <monaco@usp.br>
 *   
 *    SPDX-License-Identifier: GPL-3.0-or-later
 *
 *  This file is a derivative work from SYSeg (https://gitlab.com/monaco/syseg)
 *  and contains modifications carried out by the following author(s):
 *  Luiz Antonio de Abreu Pereira <lap.junior@gmail.com>
 *  Tiago Oliva <tiago.oliva.costa@gmail.com>
 */

#ifndef BIOS1_H
#define BIOS1_H

void __attribute__((fastcall)) clear (void);
void __attribute__((fastcall)) kwrite(const char*);
void __attribute__((fastcall)) kwriteln(const char*);
/* void __attribute__((fastcall)) kread(char *); */
void __attribute__((fastcall)) fatal(const char*);
void __attribute__((fastcall)) load_kernel(void);
void __attribute__((fastcall)) set_cursor(char, char);

#endif
