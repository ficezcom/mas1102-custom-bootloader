.section .vectors, "ax"
.global _start

_start:
    ldr pc, =reset_entry        /* 0x00: Reset */
    b reset_entry                  /* 0x04: Undefined */
    b reset_entry                  /* 0x08: SWI */
    b reset_entry                  /* 0x0C: Prefetch Abort */
    b reset_entry                  /* 0x10: Data Abort */
    b reset_entry                  /* 0x14: Reserved */
    ldr pc, =irq_ptr      /* 0x18: IRQ */
    b reset_entry                  /* 0x1C: FIQ */

irq_ptr: .word irq_handler_wrapper

reset_entry:
    ldr pc, =reset_handler

.section .text

reset_handler:
    ldr        r0,=0x00040000
    msr        cpsr_c,#0xd1
    sub        r1,r0,#0x0
    cpy        sp,r1
    msr        cpsr_c,#0xd2
    sub        r1,r0,#0x400
    cpy        sp,r1
    msr        cpsr_c,#0xd7
    sub        r1,r0,#0x800
    cpy        sp,r1
    msr        cpsr_c,#0xdb
    sub        r1,r0,#0x800
    cpy        sp,r1
    msr        cpsr_c,#0xd3
    sub        r1,r0,#0x800
    cpy        sp,r1
    msr        cpsr_c,#0xdf
    ldr        r0,=0x00040000
    sub        r1,r0,#0x800
    cpy        sp,r1
    
    mov        r0,#0x0
    mov        r1,#0x0
    mov        r2,#0x0
    mov        r3,#0x0
    mov        r4,#0x0
    mov        r5,#0x0
    mov        r6,#0x0
    mov        r7,#0x0
    mov        r8,#0x0
    mov        r9,#0x0
    mov        r10,#0x0
    mov        r11,#0x0
    mov        r12,#0x0

    mrc        p15,0x0,r0,cr1,cr0,0x0
    bic        r0,r0,#0x2000
    mcr        p15,0x0,r0,cr1,cr0,0x0


    /* C koduna dallan */
    bl kernel_main


irq_handler_wrapper:
    sub lr, lr, #4
    stmdb sp!, {r0-r12, lr}
    bl c_irq_handler
    ldmia sp!, {r0-r12, pc}^
    