#ifndef IRQ_H_
#define IRQ_H_
#include "utils.h"


#define IRQ_BASE_ADDR      0xF0003000u
#define IRQ_DISABLE_OFFSET 0x10u
#define IRQ_ENABLE_OFFSET   0x14u

#define IRQ_HANDLER_TABLE  0x00034834u

typedef void (*irq_handler_t)(void);


void IRQ_Disable(uint32_t irq);

void IRQ_Enable(uint32_t irq);

void IRQ_ClearStatus(uint32_t irq);

void IRQ_Init(void);

void IRQ_ISR_Register(uint32_t irq, uint32_t config,irq_handler_t handler);

#endif  /* IRQ_H_ */