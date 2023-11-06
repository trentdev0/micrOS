.extern interrupt_handler

.macro SAVE_INTERRUPT_CONTEXT
	pushq %rax
	pushq %rcx
	pushq %rdx
	pushq %rbx
	pushq %rsi
	pushq %rdi
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15
.endm

.macro RESTORE_INTERRUPT_CONTEXT
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdi
	popq %rsi
	popq %rbx
	popq %rdx
	popq %rcx
	popq %rax
.endm

.macro INT interrupt_number
	.global interrupt\interrupt_number
	interrupt\interrupt_number:
		SAVE_INTERRUPT_CONTEXT

		movq $\interrupt_number, %rdi

		call interrupt_handler

		RESTORE_INTERRUPT_CONTEXT
		iretq
.endm

INT 0
INT 2
INT 4
INT 5
INT 6
INT 7
INT 8
INT 10
INT 11
INT 12
INT 13
INT 14
INT 15
INT 16
INT 18
INT 21
