#include "hw_init.h"
#include "uart.h"
#include "utils.h"
#include "irq.h"

__attribute__((packed))
ControlBlock *global_queue = (ControlBlock *)TASK_STRUCT_ADDR;

uint64_t g_system_uptime = 0;
SoftwareTimer g_timer_table[256];
volatile DeviceBlock *my_device_config = (volatile DeviceBlock *)0x00034114;

void* hw_init(){

        TRACKER_SET= (uint8_t)0xE0;
    __asm__ volatile (
        "mrs r0, cpsr\n"
        "bic r0, r0, #0xc0\n"
        "msr cpsr_c, r0\n"
    );
    uart_put_char('1');

        TRACKER_SET = (uint8_t)0xAA;
    init_control_block(global_queue);
    uart_put_char('2');

        TRACKER_SET = (uint8_t)0x02;
    init_subsystem_step(my_device_config);
    uart_put_char('3');

    IRQ_Init();
    uart_put_char('4');

    TRACKER_SET = (uint8_t)0x03;
    System_Timer_Init();


}


//F0000100: 06 00 01 00 00 00 00 00 = 08 40 00 00 00 00 00 00 
//F0000110: F0 FF 00 00 00 00 00 00 = 00 00 00 80 XX 40 A5 80               
//F0000120: 00 00 00 00 00 00 00 00 = 41 54 41 53 00 00 00 00
            
   
 
void* init_subsystem_step(DeviceHandle *dev) {
    if (HW_CONFIG_LOCK & 0x80000000) {
        HW_CONFIG_LOCK &= ~0x80000000;
    }

    TRACKER_SET = 0x1032; 

    HW_TIMER_INIT = 0x0;

    dev->dev_status_id = 0x15E; 
  
}

void* System_Timer_Init() {
    volatile uint32_t *timer = (volatile uint32_t *)TIMER_BASE;


    IRQ_Disable(1);
    timer[0x00/4] = 1;
    timer[0x04/4] = 1;
    timer[0x08/4] = 1;
    timer[0x0C/4] = 1;

    timer[0x10/4] = 0; 
    timer[0x14/4] = 0;
    timer[0x18/4] = 0;
    timer[0x1C/4] = 0;

    timer[0x30/4] = 0xFF; // Prescaler / Mask 

    IRQ_ISR_Register(1, 2, (void *)SysTick_Handler);

    IRQ_Enable(1);

    timer[0x10/4] = TIMER_RELOAD; // Period
    timer[0x00/4] = 0x16;         // Start (Enable + Mode Configuration)
}




void* SysTick_Handler() {
    g_system_uptime++;

    *(volatile uint32_t *)0xF0002030 = 1;
    IRQ_ClearStatus(1);

    for (int i = 0; i < 256; i++) {
        SoftwareTimer *t = &g_timer_table[i];

        if (t->enabled != 0) {
            if (t->counter > 0) {
                t->counter--;
                
                if (t->counter == 0) {
                    if (t->callback) {
                        t->callback();
                    }
                    t->counter = t->period;
                }
            }
        }
    }
}

void* dummy_handler(void) {
    return;
}

void* init_control_block(ControlBlock *cb) {
    cb->handler = dummy_handler;
    cb->status = 0;
    cb->prev = cb;
    cb->next = cb;
}