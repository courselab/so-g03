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

#include "bios1.h"		/* Function load_kernel . */
#include "kernel.h"		/* Function kmain.        */

int boot()
{
  
  load_kernel();		/* Load the kernel from disk image.  */
  
  kmain();		        /* Call the kernel's entry function. */
  
  return 0;

}

