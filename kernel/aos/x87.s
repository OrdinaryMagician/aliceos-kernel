; x87.s : FPU functions.
; (C)2012-2013 Marisa Kirisame, UnSX Team.
; Part of AliceOS, the Alice Operating System.
; Released under the MIT License.
[BITS 32]
[ALIGN 4]
; enable the x87 coprocessor (no checking for availability)
[GLOBAL fpu_enable]
fpu_enable:
	mov eax, cr4
	or eax, 0x200
	mov cr4, eax
	ret
; common code
rnd_common:
	fstcw [esp+4]
	mov ah, [esp+5]
	mov [esp+5], al
	fldcw [esp+4]
	frndint
	mov [esp+5], ah
	fldcw [esp+4]
	ret
; rest of functions
[GLOBAL fpu_trunc]
fpu_trunc:
	fld long [esp+4]
	mov al, 0x0F
	jmp rnd_common
[GLOBAL fpu_ceil]
fpu_ceil:
	fld long [esp+4]
	mov al, 0x0B
	jmp rnd_common
[GLOBAL fpu_floor]
fpu_floor:
	fld long [esp+4]
	mov al, 0x07
	jmp rnd_common
[GLOBAL fpu_sqrt]
fpu_sqrt:
	fld long [esp+4]
	fsqrt
	ret
[GLOBAL fpu_log]
fpu_log:
	fldln2
	fld long [esp+4]
	fyl2x
	ret
[GLOBAL fpu_sin]
fpu_sin:
	fld long [esp+4]
	fsin
	ret
[GLOBAL fpu_cos]
fpu_cos:
	fld long [esp+4]
	fcos
	ret
[GLOBAL fpu_tan]
fpu_tan:
	fld long [esp+4]
	fld1
	fptan
	ret
[GLOBAL fpu_asin]
fpu_asin:
	fld long [esp+4]
	fld st0
	fld1
	fsub st1, st0
	fadd st2
	fmulp
	fsqrt
	fpatan
	ret
[GLOBAL fpu_acos]
fpu_acos:
	fld long [esp+4]
	fld st0
	fld1
	fsub st1, st0
	fadd st2
	fmulp
	fsqrt
	fabs
	fxch st1
	fpatan
	ret
[GLOBAL fpu_atan]
fpu_atan:
	fld long [esp+4]
	fld1
	fpatan
	ret
