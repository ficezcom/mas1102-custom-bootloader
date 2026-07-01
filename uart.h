#ifndef UART_H_
#define UART_H_
#include "utils.h"


#define UART_BASE 0xF0001000
#define UART_RBR_THR   (*(volatile uint8_t*)(UART_BASE + 0x00)) // DLL if DLAB=1
#define UART_IER_DLM   (*(volatile uint8_t*)(UART_BASE + 0x04)) // DLM if DLAB=1
#define UART_LCR       (*(volatile uint8_t*)(UART_BASE + 0x0C))
#define UART_LSR   (*(volatile uint8_t*)(UART_BASE + 0x14)) // Durum

void uart_init(uint32_t divisor);

void uart_put_char(char c);

void c_irq_handler();

void uart_puts(char* str);


void uart_put_byte(unsigned char val);
void uart_put_halfword(uint16_t val);
void uart_put_word(uint32_t val);

uint32_t uart_getu24(void);
uint32_t uart_getu32(void);


#endif  /* UART_H_ */