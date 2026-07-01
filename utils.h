#ifndef UTILS_H_
#define UTILS_H_

#include <stddef.h>
/* Unsigned tipler */
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

/* Signed tipler */
typedef signed char        int8_t;
typedef signed short       int16_t;
typedef signed int         int32_t;
typedef signed long long   int64_t;

#define TRACKER (*(volatile uint32_t *)0xF0400000)
#define TRACKER_SET (*(volatile uint32_t *)0xF04002F0)

#define TASK_STRUCT_ADDR 0x000341AC



void delay_cycles(unsigned int count);

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *a, const void *b, size_t n);

uint32_t internal_div_func(uint32_t param_1) ;
uint32_t get_mod_10(uint32_t param_1) ;


int run_sram_full_test() ;





#endif  /* UTILS_H_ */