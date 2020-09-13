#ifndef UART_COMMANDS_H
#define UART_COMMANDS_H

#include <stdint.h>

typedef void (* commands_ready_parser_t)(char * command_str);

void uart_commands_init(uint32_t rx_pin_no, uint32_t tx_pin_no, commands_ready_parser_t handler);
void uart_commands_service();

#endif
