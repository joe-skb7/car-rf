#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

typedef void (*usci_rx_callback_t)(uint8_t data);
typedef void (*usci_tx_callback_t)(void);

void usci0a_set_rx_cb(usci_rx_callback_t cb);
void usci0b_set_rx_cb(usci_rx_callback_t cb);
void usci0a_set_tx_cb(usci_tx_callback_t cb);
void usci0b_set_tx_cb(usci_tx_callback_t cb);

#endif /* INTERRUPTS_H */
