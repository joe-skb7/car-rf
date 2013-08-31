/* UART: USCI_A0
 * baudrate: 9600
 * mode:  8N1
 */

#include <msp430.h>
#include "uart/uart_cfg.h"
#include "uart/uart.h"

static uart_rx_callback_t rx_cb;

void uart_init(void)
{
	/* USCI A0 init sequence */
	UCA0CTL1 |= UCSWRST;		/* enable SW reset */
	UCA0CTL1 |= UCSSEL_2 + UCSWRST;	/* SMCLK */
	UCA0BR0 = UART_PRESCALER_0;	/* 1MHz 9600 */
	UCA0BR1 = UART_PRESCALER_1;	/* 1MHz 9600 */
	UCA0MCTL = UCBRS0;		/* modulation UCBRSx = 1 */
	UART_MUX_PINS;
	UCA0CTL1 &= ~UCSWRST;		/* initialize USCI state machine */

	IE2 |= UCA0RXIE;		/* enable USCI_A0 RX interrupt */
}

/*
 * NOTE: Be sure that callback as small as possible (it called for ISR)
 */
void uart_set_rx_callback(uart_rx_callback_t cb)
{
	rx_cb = cb;
}

/* TODO: replace _sync with interrupt driven send of data with callback
 * "transieved"
 * */
int uart_send_char_sync(char c)
{
	while (!(IFG2 & UCA0TXIFG)); /* TODO: add timeout and return -1 */
	UCA0TXBUF = (unsigned char)c;
	while (!(IFG2 & UCA0TXIFG)); /* TODO: add timeout and return -1 */
	return 0;
}

int uart_send_str_sync(char *s)
{
	int ret;

	while (*s) {
		ret = uart_send_char_sync(*s);
		if (ret)
			goto err;
		++s;
	}

err:
	return ret;
}

__attribute__((__interrupt__(USCIAB0RX_VECTOR)))
void usci0rx_isr(void)
{
	if (rx_cb)
		rx_cb((char)UCA0RXBUF);

	IFG2 &= ~UCA0RXIFG;
}
