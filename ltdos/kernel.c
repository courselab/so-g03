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

/* This source file implements the kernel entry function 'kmain' called
   by the bootloader, and the command-line interpreter. Other kernel functions
   were implemented separately in another source file for legibility. */

#include "kernel.h" /* Essential kernel functions.  */
#include "bios1.h"  /* For kwrite() etc.            */
#include "bios2.h"  /* For kread() etc.             */
#include "kaux.h"   /* Auxiliary kernel functions.  */

#define DIR_ENTRY_LEN 32 /* Max file name length in bytes.           */
#define SECTOR_SIZE 512
#define BOOT_START 0x7c00
#define PROGRAM_ADDRESS 0xFE00
#define FS_SIGLEN 4 /* Signature length.                        */

/* The file header. */
struct fs_header_t {
    unsigned char signature[FS_SIGLEN];     /* The file system signature.              */
    unsigned short total_number_of_sectors; /* Number of 512-byte disk blocks.         */
    unsigned short number_of_boot_sectors;  /* Sectors reserved for boot code.         */
    unsigned short number_of_file_entries;  /* Maximum number of files in the disk.    */
    unsigned short max_file_size;           /* Maximum size of a file in blocks.       */
    unsigned int unused_space;              /* Remaining space less than max_file_size.*/
} __attribute__((packed));                  /* Disable alignment to preserve offsets.  */

/* Kernel's entry function. */

void kmain(void)
{
    int i, j;

    register_syscall_handler(); /* Register syscall handler at int 0x21.*/

    splash(); /* Uncessary spash screen.              */

    shell(); /* Invoke the command-line interpreter. */

    halt(); /* On exit, halt.                       */
}

/* Tiny Shell (command-line interpreter). */

char buffer[BUFF_SIZE];
int go_on = 1;

void shell()
{
    int i;
    clear();
    kwrite("TinyDOS 1.0\n");

    while (go_on) {

        /* Read the user input.
         Commands are single-word ASCII tokens with no blanks. */
        do {
            kwrite(PROMPT);
            kread(buffer);
        } while (!buffer[0]);

        /* Check for matching built-in commands */

        i = 0;
        while (cmds[i].funct) {
            if (!strcmp(buffer, cmds[i].name)) {
                cmds[i].funct();
                break;
            }
            i++;
        }

        /* If the user input does not match any built-in command name, just
           ignore and read the next command. If we were to execute external
           programs, on the other hand, this is where we would search for a
           corresponding file with a matching name in the storage device,
           load it and transfer it the execution. Left as exercise. */

        if (!cmds[i].funct)
            kwrite("Command not found\n");
    }
}

/* Array with built-in command names and respective function pointers.
   Function prototypes are in kernel.h. */

struct cmd_t cmds[] = {{"help", f_help}, /* Print a help message.       */
                       {"quit", f_quit}, /* Exit TyDOS.                 */
                       {"exec", f_exec}, /* Execute an example program. */
                       {"list", f_list}, /* List files */
                       {0, 0}};

/* Build-in shell command: help. */

void f_help()
{
    kwrite("...me, Obi-Wan, you're my only hope!\n\n");
    kwrite("   But we can try also some commands:\n");
    kwrite("      exec    (to execute an user program example\n");
    kwrite("      list    (to list all files present in disk\n");
    kwrite("      quit    (to exit TyDOS)\n");
}

void f_quit()
{
    kwrite("Program halted. Bye.");
    go_on = 0;
}

/* Built-in shell command: example.

   Execute an example user program which invokes a syscall.

   The example program (built from the source 'prog.c') is statically linked
   to the kernel by the linker script (tydos.ld). In order to extend the
   example, and load and external C program, edit 'f_exec' and 'prog.c' choosing
   a different name for the entry function, such that it does not conflict with
   the 'main' function of the external program.  Even better: remove 'f_exec'
   entirely, and suppress the 'example_program' section from the tydos.ld, and
   edit the Makefile not to include 'prog.o' and 'libtydos.o' from 'tydos.bin'.

  */

/* Helper function to get header address.
 * Arguments: (none)
 */
struct fs_header_t *get_fs_header() { return (struct fs_header_t *)BOOT_START; }

/* List files in the volume.
 * Arguments: (none)
 */
void f_list()
{
    int i;
    char name[DIR_ENTRY_LEN];
    char buffer[6];

    struct fs_header_t *header = get_fs_header();

    /* Position at the begining of the directory region. */
    unsigned int start_sector = header->number_of_boot_sectors + 1;
    unsigned int n_sectors = header->number_of_file_entries * DIR_ENTRY_LEN / SECTOR_SIZE;

    // void *section_pointer = (void *)(start_sector * SECTOR_SIZE);
    extern int _MEM_POOL;
    void *section_pointer = (void *)&_MEM_POOL;

    load_disk(start_sector, n_sectors, section_pointer);
    /* Read all entries. */
    char *files = (char *)((char *)header + header->number_of_boot_sectors * SECTOR_SIZE);
    for (i = 0; i < 4; i++) {
        // char *name = &files[i * header->max_file_size];
        char *name = section_pointer + i * DIR_ENTRY_LEN;
        if (name[0]) {
            kwrite(name);
            kwrite("\n");
        }
    }
}

void f_exec()
{
    struct fs_header_t *header = get_fs_header();

    /* Position at the begining of the directory region. */
    unsigned int start_sector = header->number_of_boot_sectors + 1;
    unsigned int n_sectors = header->number_of_file_entries * DIR_ENTRY_LEN / SECTOR_SIZE;

    int memoryOffset = header->number_of_file_entries * 32 - (n_sectors - 1) * 512;

    void *section_pointer = (void *)(start_sector * SECTOR_SIZE);

    load_disk(start_sector, n_sectors, section_pointer);

    int bin_sector_coord = -1;
    for (int i = 0; i < header->number_of_file_entries; i++) {
        char *file_name = section_pointer + i * DIR_ENTRY_LEN;
        if (!strcmp(file_name, "hello.bin")) {
            bin_sector_coord = start_sector + n_sectors + header->max_file_size * i - 1;
            break;
        }
    }
    if (bin_sector_coord == -1) {
        kwrite("Program not found.\n");
        return;
    }

    void *program = (void *)(0xFE00);
    void *program_sector_start = program - memoryOffset;

    load_disk(bin_sector_coord, header->max_file_size, program_sector_start);

    exec();
}
