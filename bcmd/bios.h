/*
 *    SPDX-FileCopyrightText: 2021 Monaco F. J. <monaco@usp.br>
 *    SPDX-FileCopyrightText: 2024 Luiz Antonio de Abreu Pereira <lap.junior@gmail.com>
 *    SPDX-FileCopyrightText: 2024 Tiago Oliva <tiago.oliva.costa@gmail.com>
 *   
 *    SPDX-License-Identifier: GPL-3.0-or-later
 *
 *  This file is a derivative work from SYSeg (https://gitlab.com/monaco/syseg)
 *  and contains modifications carried out by the following author(s):
 *  Luiz Antonio de Abreu Pereira <lap.junior@gmail.com>
 *  Tiago Oliva <tiago.oliva.costa@gmail.com>
 */

#ifndef BIOS_H
#define BIOS_H

void __attribute__((fastcall)) clear (void);
void __attribute__((fastcall)) print();
void __attribute__((fastcall)) println();
void __attribute__((fastcall)) readln(char *);
void __attribute__((fastcall)) date();


#endif
