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

#include "kaux.h"  /* For ROWS and COLS. */
#include "bios2.h" /* For udelay().      */

/* Video RAM as 2D matrix: short vram[row][col]. */

short (*vram)[COLS] = (short (*)[COLS])0xb8000;

char character_color = 0x02; /* Default fore/background character color.*/

/* Write 'string' starting at the position given by 'row' and 'col'.
   Text is wrapped around both horizontally and vertically.

   The implementation manipulates the video-RAM rather than BIOS services.
*/

void writexy(unsigned char row, unsigned char col, const char *string)
{
    int k = 0;

    while (string[k]) {

        col = col % COLS;
        row = row % ROWS;

        vram[row][col] = color_char(string[k]);
        col++;
        k++;
    }
}

/* Clear the entire screen

   The implementation manipulates the video-RAM rather than BIOS services.

 */

void clearxy()
{
    int i, j;

    for (j = 0; j < ROWS; j++)
        for (i = 0; i < COLS; i++)
            vram[j][i] = color_char(' ');
}

/* A not-that-impressive splash screen that is entirely superfluous. */

extern const char logo[];
void splash(void)
{
    int i, j, k;

    clearxy();

    for (i = 0; i < COLS; i++) {
        for (j = 0; j < ROWS; j += 2) {
            vram[j][i] = color_char(logo[j * COLS + i]);
            vram[j + 1][COLS - i] = color_char(logo[(j + 1) * COLS + (COLS - i)]);
            udelay(1);
        }
    }

    udelay(500);
    clearxy();
}

/* Return 0 is string 's1' and 's2' are equal; return non-zero otherwise.*/

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (*s1 - *s2);
}

/* Function to convert unsigned integer types to string. */
void uint_to_string(unsigned int num, char *str)
{

    // Handle special case when num is 0
    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    // Start building the string from the end
    int index = 0;
    while (num != 0) {
        unsigned int digit = num % 10;
        str[index++] = '0' + digit; // convert digit to character
        num /= 10;
    }
    str[index] = '\0';

    // Reverse the string in place
    int start = 0;
    int end = index - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void load_disk(int sector_coordinate, int readSectors, void *target_addres)
{
    __asm__ volatile("pusha \n"
                     "mov boot_drive, %%dl \n"    /* Select the boot drive (from rt0.o). */
                     "mov $0x2, %%ah \n"          /* BIOS disk service: op. read sector. */
                     "mov %[sectToRead], %%al \n" /* How many sectors to read          */
                     "mov $0x0, %%ch \n"          /* Cylinder coordinate (starts at 0).  */
                     "mov %[sectCoord], %%cl \n"  /* Sector coordinate   (starts at 1).  */
                     "mov $0x0, %%dh \n"          /* Head coordinate     (starts at 0).      */
                     "mov %[targetAddr], %%bx \n" /* Where to load the file system (rt0.o).   */
                     "int $0x13 \n"               /* Call BIOS disk service 0x13.        */
                     "popa \n" ::[sectCoord] "g"(sector_coordinate),
                     [sectToRead] "g"(readSectors), [targetAddr] "g"(target_addres));
}
