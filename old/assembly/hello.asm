; PURPOSE:
;	Print string "hello"
; COMPILE:
;	nasm -f macho64 hello.asm && ld -macosx_version_min 10.8 -lSystem -o hello hello.o
; RUN:
;	./hello



	
	global _main
	section .text
_main:
	mov	rax, 0x2000004		; write
	mov	rdi, 1			; stdout
	mov	rsi, msg
	mov	rdx, len
	syscall

	mov	rax, 0x2000001		; exit
	mov	rdi, 0			; exit code 0
	syscall

	section .data
	msg:	db	"hello", 0xa
	len	equ	$ - msg


