
#include "irq.h"


void IRQ_Disable(uint32_t irq)
{
    volatile uint32_t *reg = (volatile uint32_t *)(IRQ_BASE_ADDR + IRQ_DISABLE_OFFSET);
    if (irq < 32)
        *reg = (1u << irq);
}

void IRQ_Enable(uint32_t irq)
{
    volatile uint32_t *reg = (volatile uint32_t *)(IRQ_BASE_ADDR + IRQ_ENABLE_OFFSET);
    if (irq < 32)
        *reg = (1u << irq);
}
void IRQ_ClearStatus(uint32_t irq)
{
    uint32_t mask = (1u << irq);
    volatile uint32_t *base = IRQ_BASE_ADDR;

    base[0x00] = mask;
    base[0x04] = mask;
}


void IRQ_Init(void)
{
    volatile uint32_t *base = (uint32_t *)IRQ_BASE_ADDR;
    volatile irq_handler_t *table = (irq_handler_t *)IRQ_HANDLER_TABLE;


    memset((void *)table, 0, 32 * sizeof(irq_handler_t));
    base[0x10] = 0xFFFFFFFF;
    base[0x00] = 0xFFFFFFFF;
    base[0x04] = 0xFFFFFFFF;
    base[0x18] = ((uint32_t)table) & ~0x3u;

    for (int i = 0; i < 8; i++)
    {
        base[(0x40) + i] = 0;
    }
}

void IRQ_ISR_Register(uint32_t irq,
                                        uint32_t config,
                                        irq_handler_t handler)
{
    // Guard conditions
    if (irq >= 32 || handler == 0)
        return;

    volatile uint32_t *base = (uint32_t *)IRQ_BASE_ADDR;
    volatile irq_handler_t *table = (irq_handler_t *)IRQ_HANDLER_TABLE;

    table[irq] = handler;

    uint32_t reg_index = irq / 4;
    volatile uint32_t *reg = (uint32_t *)((uint8_t *)base + (reg_index * 4) + 0x40);

    uint32_t val = *reg;

    uint32_t shift = (irq % 4) * 3;

    val &= ~(0x7u << shift);
    val |= ((config & 0x7u) << shift);

    *reg = val;
    base[0x14] = (1u << irq);
}
