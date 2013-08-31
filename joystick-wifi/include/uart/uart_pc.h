#ifndef UART_PC_H
#define UART_PC_H

#define EPCNOTREADY	1

typedef void (*uart_pc_rx_callback_t)(char c);

void uart_pc_init(void);
void uart_pc_set_rx_callback(uart_pc_rx_callback_t cb);
int uart_pc_write_char_sync(char c);
int uart_pc_write_str_sync(char *s);

#endif /* UART_PC_H */
