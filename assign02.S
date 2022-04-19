#include "hardware/regs/addressmap.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/timer.h"
#include "hardware/regs/m0plus.h"

.syntax unified                     @ Specify unified assembly syntax 
.cpu    cortex-m0plus               @ Specify CPU type is Cortex M0+
.thumb                              @ Specify thumb assembly for RP2040
.global main_asm                    @ Provide program starting address to the linker 
.align  4                           @ Specify code alignment 

.equ    DFLT_ALARM_TIME, 2000000      @ Specify the default alarm timeout

.equ    DFLT,0
.equ    FIRED,1

.equ    GPIO_BTN_EN_FALL_MSK, 0x400000   @ Bit-18 for falling-edge event on GP20
.equ    GPIO_BTN_EN_RISE_MSK, 0x800000   @ Bit-22 for falling-edge event on GP21

.equ    time_dash,500000    
.equ    GPIO_BTN_EN,  21              @ Specify pin for the "enter" button

.equ    dot,1                       @ Dot selection value - 1
.equ    dash,2                      @ Dash selection value - 2
.equ    done,3                      @ Done selection value - 3( Inserts /0-EOL)

.equ    GPIO_ISR_OFFSET, 0x74         @ GPIO is int #13 (vector table entry 29)
.equ    ALRM_ISR_OFFSET, 0x40         @ ALARM0 is int #0 (vector table entry 16)

@ Entry point to the ASM portion of the program
@
@ Main entry point into the ASM portion of the code
@
main_asm:
    push {lr}
    ldr   r1,=DFLT              @ Loads R1 with DFLT value(0)
    ldr   r0,=ind               @ Loads R0 with DFLT value(0) as the variable ind
    str   r1,[r0]               @ Stores address of R0 in R1
    bl    init_btns           @ Subroutine to initialize the buttons 
    bl    install_gpio_isr    @ subroutine to install the GPIO interrupt
    wfi                       @ Waits for Interrrupt 
    bl    asm_watch_update    @ Watchdog timer
    bl    install_alrm_isr    @ subroutine to install the alarm interrupt 

loop:
    bl    set_alarm                     @ Set a new alarm
    wfi                                 @ Waits for Interrupt
    bl  asm_watch_update                @ Watchdog timer update
    ldr r0,=alarm_fired                 @ Loads R0 with alarm fired DFLT value 0
    ldr r1,[r0]                         @ Loads R1 with address of R0
    cmp r1,#1                           @ Compares R1 with 1-1sec
    beq loop_done                       @ Branches to loop done if equal, else skip 
    b     loop                          @ Always branch back to loop
loop_done:
    ldr r0, =DFLT                       @ Clears loop and reset alarm_fired 
    ldr r2, =alarm_fired                
    str r0, [r2]                        
    pop {pc}

init_btns:
    push    {lr}
    movs    r0, #GPIO_BTN_EN            @ Initialised R0 with GPIO_BTN_EN value
    bl      asm_gpio_init               @ Use Wrapper Function from C file, calls asm_gpio_init to initialise 
    movs    r0, #GPIO_BTN_EN            @ Initialised R0 with GPIO_BTN_EN value
    bl      asm_gpio_set_irq_f          @ Call the subroutine to set the GPIO pin direction for falling edge
    movs    r0, #GPIO_BTN_EN            @ Initialised R0 with GPIO_BTN_EN value
    bl      asm_gpio_set_irq_r          @ Call the subroutine to set the GPIO pin direction for rising edge
    pop     {pc} 


install_gpio_isr:
    ldr     r2, =(PPB_BASE + M0PLUS_VTOR_OFFSET)     @ Adds the base address of the CPU in RP2040 to vector offset address to find out where vector table is
    ldr     r1, [r2]                                 @ Hexadecimal address of where the vector table(value stored in r2) should be loaded in r1. r1 now has actual vector table base address
    movs    r2, #GPIO_ISR_OFFSET                     @ Move offset address to r2
    add     r2, r1                                   @ Adds offset address to the base address, stores the whole value in r2. r2 now has the address to the service routine
    ldr     r0, =gpio_isr                            @ Address label gpio_isr is stored in r0
    str     r0, [r2]                                 @ Pushes the address back to r0 and stores where we have the offset address of the table
    
    @ Enable the ALARM 0 IRQ(disabling(clear) and the anbling(set))
    @clear
    ldr     r0,=8192                                 @ Setting the 14th bit to 0 as that is the position of the GPIO interrput 
    ldr     r1,=(PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET) @ Disabling the GPIO IRQ by writing the correct value to (PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET)
    str     r0,[r1]                                  @ storing address of r1 in r0
    @set
    ldr     r1,=(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET) @Enabling the GPIO IRQ by writing the correct value to (PPB_BASE + M0PLUS_NVIC_ISER_OFFSET)
    str     r0,[r1]                                  @storing address of r1 in r0
    bx      lr 

install_alrm_isr:
    @Set the IRQ handler to our subroutine
    ldr     r2, =(PPB_BASE + M0PLUS_VTOR_OFFSET)     @ address of the RAM vector table using the (PPB_BASE + M0PLUS_VTOR_OFFSET) register 
    ldr     r1, [r2]                                 @ Hexadecimal address of where the vector table(value stored in r2) should be loaded in r1. r1 now has actual vector table base address
    movs    r2, #ALRM_ISR_OFFSET                     @ Move offset address to r2
    add     r2, r1                                   @ Adds offset address to the base address, stores the whole value in r2. r2 now has the address to the alarm interrupt 
    ldr     r0, =alrm_isr                            @ Address label alrm_isr is stored in r0
    str     r0, [r2]                                 @ Pushes the address back to r0 and stores where we have the offset address of the table
    @ Enable the ALARM 0 IRQ(disabling(clear) and the enbling(set))
    @clear
    movs    r0,#1                                       @ Setting the 0th bit to 1 as that is the position of the alarm 0 interrupt
    ldr     r1,=(PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET)    @ Disabling the ALARM0 IRQ by writing the correct value to (PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET)
    str     r0,[r1]                                     @ storing address of r1 in r0
    @set
    ldr     r1,=(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET)    @ Enabling the ALARM0 IRQ by writing the correct value to (PPB_BASE + M0PLUS_NVIC_ISER_OFFSET)
    str     r0,[r1]                                     @ storing address of r1 in r0
    bx      lr 

set_alarm:
    ldr  r2,=TIMER_BASE                 
    movs r1, #1                          @ for alarm 0 interrupt
    str  r1, [r2, #TIMER_INTE_OFFSET]    @ Enable alarm timer interrupts using the (TIMER_BASE + TIMER_INTE_OFFSET)
    ldr  r1, [r2, #TIMER_TIMELR_OFFSET]  @ the current timer count from (TIMER_BASE + TIMER_TIMELR_OFFSET)
    ldr  r3,=ltimer                      @ loads the address of the value of the timer 
    ldr  r4,[r3]                         @ loads the value of the timer in r4
    add  r1, r4                          @ adds r4 to the timer to set the alarm time
    str  r1, [r2, #TIMER_ALARM0_OFFSET]  @ Updated value is pushed to (TIMER_BASE + TIMER_ALARM0_OFFSET)
    bx lr                                @ exits the subroutine

.thumb_func                     @ Required for all interrupt service routines
alrm_isr:
    push {lr}                   @ Push LR to stack
    ldr r2, =TIMER_BASE         @ Set the base address of the TIMER module in R2
    movs r1, #1                 @ Target Alarm0 stored in r1
    str r1, [r2, #TIMER_INTR_OFFSET]       @ Write to the TIMER_INTR Register to clean the pending interrupt 
    ldr r0,=ind                 @ Stores index value in R0
    ldr r1,[r0]                 @ Loads Address of r0 in r1
    ldr r0,=done                @ /0 end of Line
    bl asm_arr                  @ Input into array 
    ldr r0,=ind                 @ Loads index 
    ldr r1,[r0]                 
    adds r1,#1                  @ Updates index +1
    ldr r0,=ind                 @ Loads Index
    str r1,[r0]                 
    ldr r0,=alarm_fired         @ DFLT
    ldr r1,=FIRED               @ Value 1, acknowledged alarm fired 
    str r1,[r0]
    pop  {pc}                   @ Pop the LR back from the stack to the PC to exit the ISR

.thumb_func
gpio_isr:
    push {lr}
    ldr r2, =(IO_BANK0_BASE + IO_BANK0_PROC0_INTS2_OFFSET)  @Reads the interrupt status 
    @Interrupt acknowledgement
    ldr r1, [r2]        @
    ldr r0, =8192
    str r0, [r2]
    @implementing the button functionality
    ldr r0, =GPIO_BTN_EN_FALL_MSK             @loads in the GP20 button mask value to compare 
    cmp r1, r0                           @and detect if this is the button that has been pressed
    beq btn_dn                           @branches to GP20 button functionality
    ldr r0, =GPIO_BTN_EN_RISE_MSK             @loads in the GP21 button mask value to compare and 
    cmp r1, r0                           @and detect if this is the button that has been pressed
    beq btn_up                          @branches to GP21 button functionality

btn_dn:
    ldr r0,=TIMER_BASE                              @Get the current time
    ldr r1,[r0,#TIMER_TIMELR_OFFSET]      @Gets the lower time register offset value
    ldr r0,=fall_time                       @time stamp 
    str r1,[r0]
    ldr r2, =(IO_BANK0_BASE + IO_BANK0_INTR2_OFFSET)    @Used to reset the interrupt
    ldr r1, =GPIO_BTN_EN_FALL_MSK                          @Used to reset the GP20 interrupt
    str r1, [r2]                                        @Used to reset the GP20 interrupt
    pop {pc}

btn_up:
    ldr r2,=TIMER_BASE                              @Get the current time
    ldr r1,[r2,#TIMER_TIMELR_OFFSET]                @ Lower time register offset value
    ldr r0,=fall_time                               @ time stamp
    ldr r2,[r0]
    subs r0,r1,r2                           @ Computes time, >0.5 - dash <0.5 dot
    ldr r1,=time_dash
    cmp r0,r1
    blt dot_func
    b   dash_func
dash_func:
    ldr r0,=ind                             @ Loads index
    ldr r1,[r0]                             
    ldr r0,=dash                            @ Loads r0 with variable value dash
    bl asm_arr                              @ Inputs into array
    ldr r0,=ind                             @ Loads index
    ldr r1,[r0]
    adds r1,#1                              @ Updates index with +1 
    ldr r0,=ind                             @ Loads index 
    str r1,[r0]
    b don                                   @ Clears for new input
dot_func:
    ldr r0,=ind                @ Loads index          
    ldr r1,[r0]                 
    ldr r0,=dot                  @ Loads r0 with variable value dot
    bl asm_arr                  @ Inputs into array with ind and value of dot
    ldr r0,=ind                  @ Loads index
    ldr r1,[r0]             
    adds r1,#1                  @ Updates index with +1 
    ldr r0,=ind                  @ Loads index 
    str r1,[r0]
    b don                       @ Clears for new input

don:
    ldr r2, =(IO_BANK0_BASE + IO_BANK0_INTR2_OFFSET)    @Used to reset the interrupt
    ldr r1, =GPIO_BTN_EN_RISE_MSK                           @Used to reset the GP20 interrupt
    str r1, [r2]                                        @Used to reset the GP20 interrupt
    pop {pc}
.align 4
@ Data segment for shared variables that get updated in the asm.
.data
fall_time: .word DFLT                      
ltimer: .word   DFLT_ALARM_TIME
ind:    .word DFLT
alarm_fired: .word DFLT
