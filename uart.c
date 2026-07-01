#include "uart.h"
#include "utils.h"
#include "irq.h"

int8_t *lsr = (volatile uint8_t *)0xF0001014;
uint8_t *thr = (volatile uint8_t *)0xF0001000;

static const char hex_table[] = "0123456789ABCDEF";
void uart_put_char(char c) {
    while (!(*lsr & 0x20)); 
    *thr = (uint8_t)c;
}

uint8_t uart_get_c() {
    while (!(UART_LSR & 0x01)); 
    return UART_RBR_THR;
}

void uart_irq_handler(void)
{
    volatile uint8_t *UART = (uint8_t *)0xF0001000;
    uint8_t status = UART[0x8];
    // RX interrupt?
    if (!(status & 0x04))
        goto exit;

    uint8_t *buffer = (uint8_t *)0x00034924;
    uint32_t *idx   = (uint32_t *)0x0003FBC0;

    while (status & 0x04)
    {
        uint8_t c = UART[0x0];

        uart_put_char(c);
        if (c != 0x09)
        {
            buffer[*idx] = c;
            (*idx)++;
        }
        status = UART[0x8];
    }

exit:
    IRQ_ClearStatus(6);
}


//F0001000: XX 00 00 00 01 00 00 00 = C1 00 00 00 03 00 00 00                
//F0001010: 00 00 00 00 00 00 00 00 = F0 00 00 00 00 00 00 00

void uart_init(uint32_t divisor) {
    UART_LCR |= 0x80;
    UART_RBR_THR = 1;  
    UART_IER_DLM = 0; 
    UART_LCR = 0x03;   

    uint8_t ier = UART_IER_DLM;
    ier |= 0x00;  // RX interrupt
    ier &= 0xFD;  // TX interrupt off
    UART_IER_DLM = ier;

    IRQ_Disable(6);
    IRQ_ISR_Register(6, 2, (void*)uart_irq_handler);
    IRQ_Enable(6);
}

void c_irq_handler() {
    volatile char c = *(volatile char*)0xF0001000; 

    uart_put_char('[');
    uart_put_char(c);
    uart_put_char(']');
}

void uart_puts(char* str) {
    while (*str) {
        uart_put_char(*str++);
    }
}

void uart_put_byte(uint8_t val) {
    uart_put_char(hex_table[(val >> 4) & 0x0F]); 
    uart_put_char(hex_table[val & 0x0F]);      
}

void uart_put_halfword(uint16_t val) {
    uart_put_byte((uint8_t)(val >> 8));   
    uart_put_byte((uint8_t)(val & 0xFF)); 
}

void uart_put_word(uint32_t val) {
    uart_put_byte((uint8_t)(val >> 24));
    uart_put_byte((uint8_t)(val >> 16));
    uart_put_byte((uint8_t)(val >> 8));
    uart_put_byte((uint8_t)(val & 0xFF));
}








uint32_t uart_getu24(void) {
  uint32_t val = 0;

  val  = (uint32_t)uart_get_c() << 0;
  val |= (uint32_t)uart_get_c() << 8;
  val |= (uint32_t)uart_get_c() << 16;

  return val;
}


uint32_t uart_getu32(void) {
  uint32_t val = 0;

  val  = (uint32_t)uart_get_c() << 0;
  val |= (uint32_t)uart_get_c() << 8;
  val |= (uint32_t)uart_get_c() << 16;
  val |= (uint32_t)uart_get_c() << 24;

  return val;
}