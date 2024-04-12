#    SPDX-FileCopyrightText: 2001 Monaco F. J. <monaco@usp.br>
#    SPDX-FileCopyrightText: 2024 Luiz Antonio de Abreu Pereira <lap.junior@gmail.com>
#    SPDX-FileCopyrightText: 2024 Tiago F Oliva Costa <tiago.oliva.costa@gmail.com>
#
#    SPDX-License-Identifier: GPL-3.0-or-later
#
#    This file is part of SYSeg, available at https://gitlab.com/monaco/syseg.

	#;; Boot, say hello, and halt
	#;; NASM assembly, neater version wit loop

	.code16			# Set 16-bit mode
	
	.global begin
	
begin:	

	mov $0xe, %ah		# BIOS tty mode

	mov $0x0, %bx		# Offsets now handled via org directive
	
loop:				
	mov msg(%bx), %al	# Ofsset to the message
	cmp $0x0, %al		# Loop while char is not 0x0 
	je halt			# Jump to halt
	int $0x10		# Call BIOS video interrupt
	add $0x1, %bx		# Point to the next character
	jmp loop		# Repeat until we find a 0x0

halt:
	hlt			
	jmp halt		# Safeguard

msg:				# C-like NULL terminated string

	.string "Hello World"
	
	.fill 510 - (. - begin), 1, 0		# Pad with zeros
	.word 0xaa55		    		# Boot signature


	#;; Notes
	#;;
	#;;  The org directive tells nasm where we intende to load the program.
	#;;  The offset is now automatically applied wherever needed.
