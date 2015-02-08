; ENVIRONMENT:
;	mac os x 64 bit
; PURPOSE:
;	print first 90  Fibonacci number
; COMPILE:
;	nasm -f macho64 fib.asm && gcc -o fib fib.o
; RUN:
;	./fib



	
	global _main
	extern _printf			; external function name must begin with "_"

	section .text
_main:
	push	rbx
	mov	ecx, 90
	xor	rax, rax		; reset rax, rax will hold the current number
	xor	rbx, rbx		; reset rbx, rbx will hold the next number
	inc	rbx			; set rb = 1
print:
	push	rax			; save rax and rcx for restoration, because printf will destroy rax and rcx
	push	rcx
	mov	rdi, format		; set 1st parameter format
	mov	rsi, rax		; set 2nd parameter current number
	xor	rax, rax

	call	_printf			; printf(format, current number)

	pop	rcx
	pop	rax

	mov	rdx, rax		; save current number
	mov	rax, rbx		; next number bacomes current
	add	rbx, rdx		; get new next number
	dec	ecx			; count down
	jnz	print			; if ZF != 0, loop print

	pop	rbx
	ret
format:
	db	"%20ld", 10, 0
