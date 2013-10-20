#ifndef UART_H
#define UART_H

typedef void (*uart_rx_callback_t)(char c);

void uart_init(void);
void uart_set_rx_callback(uart_rx_callback_t cb);
int uart_send_char_sync(char c);
int uart_send_str_sync(char *s);

#endif /* UART_H */
