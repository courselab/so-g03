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

#include "bios.h"
#include "utils.h"

#define PROMPT "$ "		/* Prompt sign.      */
#define SIZE 20			/* Read buffer size. */

char buffer[SIZE];		/* Read buffer.      */

int main()
{
  clear();
  
  println  ("Boot Command 1.0");

  while (1)
	{
		print(PROMPT);		/* Show prompt.               */
		readln(buffer);		/* Read use input.            */

		if (buffer[0])		/* Execute built-in command.  */
		{
		  if (!strcmp(buffer,"help"))
			println("A Beattles's song.");
		
		  else if (!strcmp(buffer,"clear"))
			clear();
		
		  else if (!strcmp(buffer,"date"))
			println("Get Date.");
		
		  else if (!strcmp(buffer,"exit"))
			return 0;
		
		  else 
			println("Unkown command.");
		}
	}
  
  return 0;

}

