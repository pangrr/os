; ENVIRONMENT:
;	mac os x 64 bit
; PURPOSE:
;	displays its command line arguments, one per line
; COMPILE:
;	nasm -f macho64 echo.asm && gcc -o echo echo.o
; RUN:
;	./echo [argument 1] [argument 2] ...

	global	_main
	extern	_puts
	section	.text
_main:
	push	rdi		; rdi contains argc
	push	rsi		; rsi contains argv
	sub	rsp,	8	; must align stack before call

	mov	rdi,	[rsi]	; argument string to display
	call	_puts

	add	rsp,	8	; restore rsp to pre-aligned value
	pop	rsi		; restore resgisters
	pop	rdi

	add	rsi,	8	; point to next argument
	dec	rdi		; count down
	jnz	_main		; if not all arguments are printed, continue

	ret

