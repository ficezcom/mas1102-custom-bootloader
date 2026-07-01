#include "uart.h"
#include "utils.h"
#include "irq.h"
#include "hw_init.h"
volatile uint64_t *counter = (volatile uint64_t *)0x0003390;

typedef struct __attribute__((packed)) {
    uint32_t addr;
    uint32_t size;
    uint8_t data;
    uint8_t  cmd;
} CommandPacket;

void handle_commands() {
    CommandPacket pkt;
    uint8_t *pkt_ptr = (uint8_t *)&pkt;

    while(1) {
        for (int i = 0; i < 10; i++) {
            pkt_ptr[i] = uart_get_c();
        }
        switch(pkt.cmd) {
            
            case 0x03: { // read command
                uart_put_char(0x06); // ACK
                for (uint32_t i = 0; i < pkt.size; i++) {
                    uart_put_char(*(volatile uint8_t*)(pkt.addr + i));
                }
                break;
            }

            case 0x02: { // write command
                // pkt.data 32-bit
                *(volatile uint8_t*)(pkt.addr) = pkt.data;
                uart_put_char(0x06); // ACK
                break;
            }
            
            case 0x04: { // BULK WRITE 
            uint8_t *dest = (uint8_t*)pkt.addr;
            
            for (uint32_t i = 0; i < pkt.size; i++) {
                dest[i] = uart_get_c(); 
            }
            uart_put_char(0x06); // ACK
            break;
        }
            case 0x01: { // PING / TEST
                uart_put_char(0x06); // ACK
                break;
            }

            case 0x05: { // CALL / JUMP 
                uart_puts("[DEBUG] Jumping to: ");
                uart_put_word(pkt.addr);
                uart_puts("\n");
                uart_put_char(0x06);  // ACK

                void (*run_code)(void);
                run_code = (void (*)(void))(pkt.addr);
                run_code();

                uart_puts("[DEBUG] Returned from RAM code.\n");
                break;
            }

            default: {
                uart_put_char(0x15); // NAK: unknown
                break;
            }
        }
    }
}

void kernel_main() {
    TRACKER_SET = (uint8_t)0xA4;
    TRACKER_SET= (uint8_t)0x0;
   

//hw_init();
//uart_init(0x684EE180); //BRom already init done. 
uart_put_char('Start');

/*
run_stack_check();
final_check();
*/

handle_commands();


while (1) {}
}
