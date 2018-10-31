/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>



###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */
	TRAPHANDLER_NOEC(th0, 0)
	TRAPHANDLER_NOEC(th1, 1)
	TRAPHANDLER_NOEC(th3, 3)
	TRAPHANDLER_NOEC(th4, 4)
	TRAPHANDLER_NOEC(th5, 5)
	TRAPHANDLER_NOEC(th6, 6)
	TRAPHANDLER_NOEC(th7, 7)
	TRAPHANDLER(th8, 8)
	TRAPHANDLER_NOEC(th9, 9)
	TRAPHANDLER(th10, 10)
	TRAPHANDLER(th11, 11)
	TRAPHANDLER(th12, 12)
	TRAPHANDLER(th13, 13)
	TRAPHANDLER(th14, 14)
	TRAPHANDLER_NOEC(th16, 16)

	TRAPHANDLER_NOEC(th32, IRQ_OFFSET)
	TRAPHANDLER_NOEC(th33, IRQ_OFFSET + 1)
	TRAPHANDLER_NOEC(th34, IRQ_OFFSET + 2)
	TRAPHANDLER_NOEC(th35, IRQ_OFFSET + 3)
	TRAPHANDLER_NOEC(th36, IRQ_OFFSET + 4)
	TRAPHANDLER_NOEC(th37, IRQ_OFFSET + 5)
	TRAPHANDLER_NOEC(th38, IRQ_OFFSET + 6)
	TRAPHANDLER_NOEC(th39, IRQ_OFFSET + 7)
	TRAPHANDLER_NOEC(th40, IRQ_OFFSET + 8)
	TRAPHANDLER_NOEC(th41, IRQ_OFFSET + 9)
	TRAPHANDLER_NOEC(th42, IRQ_OFFSET + 10)
	TRAPHANDLER_NOEC(th43, IRQ_OFFSET + 11)
	TRAPHANDLER_NOEC(th44, IRQ_OFFSET + 12)
	TRAPHANDLER_NOEC(th45, IRQ_OFFSET + 13)
	TRAPHANDLER_NOEC(th46, IRQ_OFFSET + 14)
	TRAPHANDLER_NOEC(th47, IRQ_OFFSET + 15)

	TRAPHANDLER_NOEC(th_syscall, T_SYSCALL)

/*
 * Lab 3: Your code here for _alltraps
 */
	//参考inc/trap.h中的Trapframe结构。tf_ss，tf_esp，tf_eflags，tf_cs，tf_eip，tf_err由处理器压入，所以现在只需要压入剩下寄存器（%ds,%es,通用寄存器）
	//切换到内核数据段
	_alltraps:
	pushl %ds
	pushl %es
	pushal
	pushl $GD_KD
	popl %ds
	pushl $GD_KD
	popl %es
	pushl %esp				//压入trap()的参数tf，%esp指向Trapframe结构的起始地址
	call trap
