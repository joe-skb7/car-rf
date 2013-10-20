#ifndef UART_CFG_H
#define UART_CFG_H

/* UART pins numbers */
#define UART_RX_PIN		BIT1	/* P1.1 */
#define UART_TX_PIN		BIT2	/* P1.2 */

/* Mux pins for UART mode */
#define UART_MUX_PINS					\
	do {						\
		P1SEL  |= UART_RX_PIN + UART_TX_PIN;	\
		P1SEL2 |= UART_RX_PIN + UART_TX_PIN;	\
	} while (0)


/* Clock prescaler configuration: prescale = P0 + P1*256
 *
 * MCLK = SMCLK (DCO) = CALxxx_1MHZ = 1 MHz = 1000000 Hz
 * baudrate: 9600 symbols/sec
 * baudrate generator clock prescaler = SMCLK / baudrate = 104
 */
#define UART_PRESCALER_0	104
#define UART_PRESCALER_1	0

#endif /* UART_CFG_H */
