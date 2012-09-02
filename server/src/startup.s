.text
.arm
.global _start
.global main
.global irq_handler

/* linker script symbols */
.global _etext
.global _data
.global _edata
.global _bss
.global _ebss

_start:
    /* Определяем вектора исключений */
    b reset  /* сброс */
    b loop   /* неправильная команда */
    b loop   /* программное прерывание */
    b loop   /* prefetch abort */
    b loop   /* data abort */
    nop      /* checksum загрузчика */
    b irq_handler /* IRQ */
    b loop   /* FIQ */

    /* Наш код */
reset:             /*после сброса*/

    /* Use r0 for indirect addressing */
    ldr r0, PLLBASE

    /* PLLCFG = PLLCFG_VALUE */
    mov r3, #PLLCFG_VALUE
    str r3, [r0, #PLLCFG_OFFSET]

    /* PLLCON = PLLCON_PLLE */
    mov r3, #PLLCON_PLLE
    str r3, [r0, #PLLCON_OFFSET]

    /* PLLFEED = PLLFEED1, PLLFEED2 */
    mov r1, #PLLFEED1
    mov r2, #PLLFEED2
    str r1, [r0, #PLLFEED_OFFSET]
    str r2, [r0, #PLLFEED_OFFSET]

    /*  while ((PLLSTAT & PLLSTAT_PLOCK) == 0)*/
pll_loop:
    ldr r3, [r0, #PLLSTAT_OFFSET]
    tst r3, #PLLSTAT_PLOCK
    beq pll_loop

    /* PLLCON = PLLCON_PLLC|PLLCON_PLLE */
    mov r3, #PLLCON_PLLC|PLLCON_PLLE
    str r3, [r0, #PLLCON_OFFSET]

    /* PLLFEED = PLLFEED1, PLLFEED2 */
    str r1, [r0, #PLLFEED_OFFSET]
    str r2, [r0, #PLLFEED_OFFSET]

vpbdiv_init:
    /* Use r0 for indirect addressing */
    ldr r0, VPBDIVBASE
    mov r1, #VPBDIV_VALUE
    str r1, [r0, #VPBDIV_OFFSET]


/* LPC2292 EMC Setup */
    ldr r0, EMC_BASE

    ldr r1, =BCFG0_Val
    str r1, [r0, #BCFG0_OFS]

    ldr r1, =BCFG1_Val
    str r1, [r0, #BCFG1_OFS]

    ldr r1, =BCFG2_Val
    str r1, [r0, #BCFG2_OFS]

    ldr r1, =BCFG3_Val
    str r1, [r0, #BCFG3_OFS]


/* LPC2292 MAM Setup */
mam_init:
    /* Use r0 for indirect addressing */
    ldr r0, MAMBASE

    /* MAMCR = MAMCR_VALUE */
    mov r1, #MAMCR_VALUE
    str r1, [r0, #MAMCR_OFFSET]

    /* MAMTIM = MAMTIM_VALUE */
    mov r1, #MAMTIM_VALUE
    str r1, [r0, #MAMTIM_OFFSET]


/*  LPC2292 Stack Setup for all 6 CPU modes leaving CPU in system mode */
stacks_init:
    ldr r0, STACK_START

    /* FIQ mode stack */
    msr CPSR_c, #FIQ_MODE|IRQ_DISABLE|FIQ_DISABLE
    mov sp, r0
    sub r0, r0, #FIQ_STACK_SIZE

    /* IRQ mode stack */
    msr CPSR_c, #IRQ_MODE|IRQ_DISABLE|FIQ_DISABLE
    mov sp, r0
    sub r0, r0, #IRQ_STACK_SIZE

    /* Supervisor mode stack */
    msr CPSR_c, #SVC_MODE|IRQ_DISABLE|FIQ_DISABLE
    mov sp, r0
    sub r0, r0, #SVC_STACK_SIZE

    /* Undefined mode stack */
    msr CPSR_c, #UND_MODE|IRQ_DISABLE|FIQ_DISABLE
    mov sp, r0
    sub r0, r0, #UND_STACK_SIZE

    /* Abort mode stack */
    msr CPSR_c, #ABT_MODE|IRQ_DISABLE|FIQ_DISABLE
    mov sp, r0
    sub r0, r0, #ABT_STACK_SIZE

    /* System mode stack */
    msr CPSR_c, #SYS_MODE
    mov sp, r0

/* Setup C subroatine
#  - copy .data from flash to sram
#  - clear .bss
#  - go to main  */
runtime_init:

    /* Copy .data */
    ldr   r0, data_source
    ldr   r1, data_start
    ldr   r2, data_end
copy_data:
    cmp   r1, r2
    ldrne r3, [r0], #4
    strne r3, [r1], #4
    bne   copy_data

    /* Clear .bss */
    ldr   r0, =0
    ldr   r1, bss_start
    ldr   r2, bss_end
clear_bss:
    cmp   r1, r2
    strne r0, [r1], #4
    bne   clear_bss

    /* Jump to main */
    mov   r0, #0
    mov   r1, #0
    bl    main

loop:
    b     loop  /*бесконечный цикл*/


/* constants & registers */
STACK_START:     .word    0x40004000
PLLBASE:         .word    0xE01FC080 /* PLLCON */
MAMBASE:         .word    0xE01FC000 /* MAMCR */
VPBDIVBASE:      .word    0xE01FC100 /* VPBDIV Address */
EMC_BASE:        .word    0xFFE00000 /* EMC Base Address */


/* for linker */
data_source:     .word    _etext
data_start:      .word    _data
data_end:        .word    _edata
bss_start:       .word    _bss
bss_end:         .word    _ebss

/* External Memory Controller (EMC) */
    .equ BCFG0_OFS,       0x00     /* BCFG0 Offset */
    .equ BCFG1_OFS,       0x04     /* BCFG1 Offset */
    .equ BCFG2_OFS,       0x08     /* BCFG2 Offset */
    .equ BCFG3_OFS,       0x0C     /* BCFG3 Offset */

    .equ BCFG0_Val,       0x1000FFEF
    .equ BCFG1_Val,       0x1000FFEF
    .equ BCFG2_Val,       0x0000FFEF
    .equ BCFG3_Val,       0x0000FFEF

/* PLL configuration */
    .equ PLLCON_OFFSET,   0x0
    .equ PLLCFG_OFFSET,   0x4
    .equ PLLSTAT_OFFSET,  0x8
    .equ PLLFEED_OFFSET,  0xC

    .equ PLLCON_PLLE,     (1 << 0)
    .equ PLLCON_PLLC,     (1 << 1)
    .equ PLLSTAT_PLOCK,   (1 << 10)
    .equ PLLFEED1,        0xAA
    .equ PLLFEED2,        0x55

    .equ PLLCFG_VALUE,    0x23    /* cclk = 48MHz */

/* VPBDIV configuration */
    .equ VPBDIV_OFFSET,   0x0
    .equ VPBDIV_VALUE,    0x0

/* MAM configuration */
    .equ MAMCR_OFFSET,    0x0
    .equ MAMTIM_OFFSET,   0x4

    .equ MAMCR_VALUE,     0x2  /* fully enabled */
    .equ MAMTIM_VALUE,    0x4  /* fetch cycles for cclk = 60MHz  */

/* Stack configuration */
    /* Processor modes */
    .equ FIQ_MODE,        0x11
    .equ IRQ_MODE,        0x12
    .equ SVC_MODE,        0x13  /* reset mode */
    .equ ABT_MODE,        0x17
    .equ UND_MODE,        0x1B
    .equ SYS_MODE,        0x1F

    /* Stack sizes */
    .equ FIQ_STACK_SIZE,  0x00000080    /*  32x32-bit words */
    .equ IRQ_STACK_SIZE,  0x00000080
    .equ SVC_STACK_SIZE,  0x00000080
    .equ ABT_STACK_SIZE,  0x00000010    /*   4x32-bit words */
    .equ UND_STACK_SIZE,  0x00000010
    .equ SYS_STACK_SIZE,  0x00000400    /* 256x32-bit words */

    /* CPSR interrupt disable bits */
    .equ IRQ_DISABLE,     (1 << 7)
    .equ FIQ_DISABLE,     (1 << 6)

.end

