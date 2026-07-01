#ifndef HW_INIT_H_
#define HW_INIT_H_

#include <stddef.h>
#include "utils.h"


#define REGS_BASE_F000    0xF0000000
#define REGS_BASE_F040    0xF0400000

#define REG_CONTROL_118   (*(volatile uint32_t *)(REGS_BASE_F000 + 0x118))
#define REG_TIMER_100     (*(volatile uint32_t *)(REGS_BASE_F000 + 0x100))
#define REG_PLL_CFG       (*(volatile uint32_t *)(REGS_BASE_F040 + 0x2F0))
#define REG_ENABLE_8      (*(volatile uint8_t  *)(REGS_BASE_F000 + 0x4008))

#define HW_CONFIG_LOCK    (*(volatile uint32_t *)0xF0000118)
#define HW_TIMER_INIT     (*(volatile uint32_t *)0xF0000102)
#define HW_TRIGGER_REG    (*(volatile uint8_t  *)0xF0004008)

#define TIMER_BASE      0xF0002000
#define TIMER_RELOAD    0x0006ACFC


typedef struct {
    uint32_t unused[4];
    uint32_t dev_status_id; 
} DeviceHandle;






void* hw_init();

typedef struct ControlBlock {
    void (*handler)(void);      
    uint32_t status;            
    uint32_t reserved;          
    struct ControlBlock *prev;  
    struct ControlBlock *next;  
} ControlBlock;

typedef struct DeviceBlock {
    uint32_t padding[4];   
    uint32_t device_id;    
} DeviceBlock;

typedef struct {
    void (*callback)(void); 
    uint32_t period;        
    uint32_t counter;       
    uint8_t  enabled;       
    uint8_t  padding[3];    
} SoftwareTimer;



extern uint64_t g_system_uptime;
extern SoftwareTimer g_timer_table[256];
extern volatile DeviceBlock* my_device_config;

void* init_control_block(ControlBlock *cb);
void* init_subsystem_step(DeviceHandle *dev);
void* System_Timer_Init();
void* SysTick_Handler();

#endif  /* HW_INIT_H_ */