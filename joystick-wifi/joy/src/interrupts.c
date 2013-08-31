#include <interrupts.h>
#include <msp430.h>

static usci_rx_callback_t usci0a_rx_cb;
static usci_rx_callback_t usci0b_rx_cb;
static usci_tx_callback_t usci0a_tx_cb;
static usci_tx_callback_t usci0b_tx_cb;

void usci0a_set_rx_cb(usci_rx_callback_t cb)
{
	usci0a_rx_cb = cb;
}

void usci0b_set_rx_cb(usci_rx_callback_t cb)
{
	usci0b_rx_cb = cb;
}

void usci0a_set_tx_cb(usci_tx_callback_t cb)
{
	usci0a_tx_cb = cb;
}

void usci0b_set_tx_cb(usci_tx_callback_t cb)
{
	usci0b_tx_cb = cb;
}

__attribute__((__interrupt__(USCIAB0RX_VECTOR)))
void usci0rx_isr(void)
{
	if(IFG2 & UCA0RXIFG)
	{
		if (usci0a_rx_cb)
			usci0a_rx_cb((char)UCA0RXBUF);
		IFG2 &= ~UCA0RXIFG;
	}
	else if(IFG2 & UCB0RXIFG)
	{
		if (usci0b_rx_cb)
			usci0b_rx_cb((char)UCB0RXBUF);
		IFG2 &= ~UCB0RXIFG;
	}
}

__attribute__((__interrupt__(USCIAB0TX_VECTOR)))
void usci0tx_isr(void)
{
	if(IFG2 & UCA0TXIFG)
	{
		if (usci0a_tx_cb)
			usci0a_tx_cb();
		IFG2 &= ~UCA0TXIFG;
	}
	else if(IFG2 & UCB0TXIFG)
	{
		if (usci0b_tx_cb)
			usci0b_tx_cb();
		IFG2 &= ~UCB0TXIFG;
	}
}
