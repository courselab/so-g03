#
#    SPDX-FileCopyrightText: 2021 Monaco F. J. <monaco@usp.br>
#    SPDX-FileCopyrightText: 2024 Luiz Antonio de Abreu Pereira <lap.junior@gmail.com>
#    SPDX-FileCopyrightText: 2024 Tiago Oliva <tiago.oliva.costa@gmail.com>
#
#    SPDX-License-Identifier: GPL-3.0-or-later
#
#  This file is a derivative work from SYSeg (https://gitlab.com/monaco/syseg)
#  and contains modifications carried out by the following author(s):
#  Luiz Antonio de Abreu Pereira <lap.junior@gmail.com>
#  Tiago Oliva <tiago.oliva.costa@gmail.com>
#

	.code16
	.global clear, print, readln, println, strcmp, date, convert_to_ascii
	
	.section .text

	## void clear(void)
	##
	## Clear the screen.
	
clear:
#	pusha			/* Save all GP registers.              */
#	mov $0x0600, %ax	/* Video service: scroll up.           */
#	mov $0x07, %bh		/* Attribute (background/foreground).  */
#	mov $0x00, %cx		/* Upper-left corner:   (0,0).         */
#	mov $0x184f, %dx	/* Botton-right corner: (24,79).       */
#	int $0x10		/* Call BIOS video service.            */
#
#	mov $0x2, %ah		/* Video service: set cursor position. */
#	mov $0x0, %bh		/* Select page number 0.               */
#	mov $0x0, %dx		/* Set position (0,0).                 */
#	int $0x10		/* Call BIOS video service.            */
#	
#	popa			/* Restore all GP-registers.           */
	ret

	## void print(const char* msg)
	## 
	## Print 'msg' onto the screen.
	
print:
        pusha			/* Save all GP registers.              */
        mov %cx, %bx		/* Copy argument into base register.   */
	mov $0x0, %si		/* Initialize buffer index.            */
        mov $0x0e, %ah          /* BIOS video service: TTY mode.       */
print_loop:
        mov (%bx, %si), %al	/* Get each ASCII character.           */
        cmp $0x0, %al		/* End writing on 0.                   */
        je print_end
        int $0x10		/* Call BIOS video service.            */
        inc %si			/* Point to the next character.        */
        jmp print_loop
print_end:
        popa
        ret


	## void println(const char* msg)
	##
	## Print 'msg', followed by a CR-LF sequence, onto the screen.
	
println:
	pusha			/* Save all GP registers.              */
	call print		/* Print argument (in %cx).            */
	lea crlf, %cx		
	call print		/* Print a CR-LF sequence.             */
	popa			/* Restore all GP registers.           */
	ret


	## void readln(char *buffer)
	##
	## Reads a CR-terminated line from keyboard into buffer.

	
readln:
	pusha	                /* Save all GP registers.                     */

	mov %cx, %bx		/* Argument received in %cx.                  */
	mov $0x0, %si		/* Initialize buffer index.                   */
	
readln_loop:
	mov $0x0, %ax		/* BIOS keyboard service (blocking read).     */
	int $0x16		/* Call BIOS keyboard service.                */

	cmp $0xb, %si		/* End reading if more than 10 characters.    */
	je readln_trunc

	mov %al, %es:(%bx,%si)	/* Add ASCII to the buffer.                   */
	inc %si			/* Increment the buffer index.                */
	
readln_trunc:	
	mov $0xe, %ah		/* Echo the ascii onto the screen.            */
	int $0x10

	cmp $0xd, %al		/* End reading if character is CR.            */
	je readln_end		
	
	jmp readln_loop		/* Keep reading.                              */
	
readln_end:	
	mov $0xe, %ah		/* Echo a newline.                            */
	mov $0xa, %al
	int $0x10

	movb $0x0, -1(%bx,%si)  /* Remove trailing CR.                        */
	popa			/* Restore all GP registers.                  */
	ret

date:
    pusha             /* Save all GP registers. */
    mov $0x04, %ah    /* To get System Date */
    int $0x1A
	
	# do year
	movb    %cl, %al
    call    convert_to_ascii	
	mov %bx, date_buffer + 6
	
	# do day
	movb    %dl, %al
	call    convert_to_ascii
	mov %bx, date_buffer
    movb $'/', date_buffer + 2 /* Insert '/' character */	
	
	# do month
    movb    %dh, %al
    call    convert_to_ascii
    mov %bx, date_buffer + 3
    movb $'/', date_buffer + 5 /* Insert '/' character */
	

    mov $date_buffer, %cx

    call println
    popa              /* Restore all GP registers. */
    ret
	
	
# Converts a byte in AL to two ASCII characters in BX
convert_to_ascii:
    push    %ax              # Save original AX
    mov     %al, %ah         # Move the high nibble to AH
    and     $0x0F, %ah       # Mask the high nibble
    shr     $4, %al          # Shift right to get the high nibble in AL
    and     $0x0F, %al       # Mask the low nibble

    # Convert high nibble to ASCII
    cmp     $0x09, %al       # Compare with 9
    jbe     .high_is_digit   # If <= 9, it's a digit
    add     $0x07, %al       # Else, it's a letter (A-F)
.high_is_digit:
    add     $0x30, %al       # Convert to ASCII
    mov     %al, %bl         # Move high ASCII character to BL

    # Convert low nibble to ASCII
    cmp     $0x09, %ah       # Compare with 9
    jbe     .low_is_digit    # If <= 9, it's a digit
    add     $0x07, %ah       # Else, it's a letter (A-F)
.low_is_digit:
    add     $0x30, %ah       # Convert to ASCII
    mov     %ah, %bh         # Move low ASCII character to BH

    pop     %ax              # Restore original AX
    ret                      # Return	
	

/* Read-only data */
.section .rodata

date_buffer:
    .ascii "dd/mm/yy\0"   /* Placeholder for date string. */

crlf:
    .byte 0xd, 0x0a, 0x0  /* CR-FL sequence. */
