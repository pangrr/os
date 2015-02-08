; PURPOSE:
;	re-write cat filename in nasm for os x 10.9 x86_64

; COMPILE:
;	nasm -f macho64	cat64.asm
;	ld -macosx_version_min 10.8 -lSystem -o cat64 cat64.o
;
; RUN:
;	./cat64 filename



ByteBuff resb 256

section .data

section .text

global _main
_main:
	nop
	cmp qword [rsp], 2	; check argc at rsp
	jl Exit	; jump to exit if only 1 command argument, ie. no file specified
	
	; open file
	mov rax, 2	; sys_open
	mov rdi, [rsp+16]	; put argv[1] in rdi for syscall
	mov rsi, 0	; set flag to 0 for read only
	moc rdx, 0	;
	syscall
	cmp rax, 0	; check for errors on openning file
	jl Error	; if less than 0, error
	mov rbx, rax	; put file descriptor to rbx

read:
	mov rax, 0	; sys_read
	mov rdi, rbx	; put file descriptor to rdi
	mov rsi, ByteBuff	; 
	mov rdx, 256	; specify to read 256 bytes
	syscall
	cmp rax, 0	; check for EOF
	jl Error	; if less than 0, error
	jne print	; if not EOF, jump to print

	; close file and jump to exit
	mov rdx, rax	; put number of bytes read to rdx
	mov rax, 1	; sys_write
	mov rdi, 1	; stdout file descriptor
	mov rsi, ByteBuff ;
	syscall
	cmp rax, 0
	jl Error
	jmp read	; keep reading
Error:
	
