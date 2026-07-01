#include "utils.h"



void delay_cycles(unsigned int count) {
    // volatile, derleyicinin bu değişkeni ve döngüyü silmesini engeller
    for (volatile unsigned int i = 0; i < count; i++) {
       __asm__ volatile ("nop");
    }
}

void *memcpy(void *dest, const void *src, size_t n)
{
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;

    while (n--)
    {
        *d++ = *s++;
    }

    return dest;
}

void *memset(void *s, int c, size_t n)
{
    uint8_t *p = (uint8_t *)s;

    while (n--)
    {
        *p++ = (uint8_t)c;
    }

    return s;
}

void *memmove(void *dest, const void *src, size_t n)
{
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;

    if (d < s)
    {
        while (n--)
        {
            *d++ = *s++;
        }
    }
    else
    {
        d += n;
        s += n;

        while (n--)
        {
            *--d = *--s;
        }
    }

    return dest;
}

int memcmp(const void *a, const void *b, size_t n)
{
    const uint8_t *p1 = (const uint8_t *)a;
    const uint8_t *p2 = (const uint8_t *)b;

    while (n--)
    {
        if (*p1 != *p2)
        {
            return *p1 - *p2;
        }

        p1++;
        p2++;
    }

    return 0;
}

uint32_t internal_div_func(uint32_t param_1) {
    // Register değerlerini tanımlayalım
    uint32_t r3_val1 = 0x91A2B3C5;
    uint32_t r3_val2 = 0xFFE3E000;

    // UMULL işlemi (64 bit çarpım)
    // r1 alt 32 bit, r2 üst 32 bit
    uint64_t full_mul = (uint64_t)param_1 * r3_val1;
    uint32_t r2 = (uint32_t)(full_mul >> 32); 

    // mov r1, r2, lsr #0x14
    uint32_t r1 = r2 >> 20;

    // mla param_1, r3, r1, param_1
    // Bu işlem: param_1 = (r3_val2 * r1) + param_1
    param_1 = (r3_val2 * r1) + param_1;

    return param_1;
}
uint32_t get_mod_10(uint32_t param_1) {
    // 10'a bölme (Derleyici bunu senin Ghidra'da gördüğün gibi optimize eder)
    uint32_t bolum = param_1 / 10;
    
    // Kalanı bulma (param_1 - (bolum * 10))
    uint32_t kalan = param_1 % 10; 
    
    return kalan;
}

__attribute__((target("arm")))
int sram_clear() {
    volatile uint32_t *start_addr = (uint32_t *)0xE0000000;
    uint32_t size_in_words = (1024 * 1024) / 4; // 1 MB alan
    uint32_t i;

    uart_puts("\r\n[SRAM] Test baslatiliyor (0xE0000000 - 1MB)...");

    // Adım 1: Sıfırlama ve Kontrol
    uart_puts("\r\n[SRAM]  Sifirlaniyor...");
    memset((void*)start_addr, 0, 1024 * 1024);
    
    for (i = 0; i < size_in_words; i++) {
        if (start_addr[i] != 0) {
            uart_puts("\r\n!!! HATA: Sifirlama basarisiz! Adres: ");
            uart_put_byte((uint8_t)(start_addr[i] >> 24));
            uart_put_byte((uint8_t)(start_addr[i] >> 16));
            uart_put_byte((uint8_t)(start_addr[i] >> 8));
            uart_put_byte((uint8_t)(start_addr[i] & 0xFF));
 
            return -1;
        }
    }
    uart_puts("\r\n[SRAM] Test PASSED.");
    return 0;
}



