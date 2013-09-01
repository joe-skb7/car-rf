#include <stdio.h>
#include <msp430.h>
#include <spi/spi.h>
#include <uart/uart_pc.h> /* TODO: for debug (temporary); remove further */

static void spi_mux_pins(void)
{
	/* P1.5 - UCB0CLK
	 * P1.6 - UCB0SOMI
	 * P1.7 - UCB0SIMO
	 */
	P1SEL  |= BIT5 | BIT6 | BIT7;
	P1SEL2 |= BIT5 | BIT6 | BIT7;
}

void spi_init(void)
{
	/* Reset SPI */
	UCB0CTL1 = UCSWRST;

	/* UCCKPH   = 0: Data is changed on the first UCLK edge and captured
	 *               on the following edge
	 * UCMSB    = 0: Send lesat significant bit first
	 * UC7BIT   = 0: 8-bit character length
	 * UCMODE_0 = 0: 3-pin SPI
	 * UCCKPL   = 1: Inactive clock state is high
	 * UCMST    = 1: Master mode
	 * UCSYNC   = 1: Synchronous mode
	 */
	UCB0CTL0 = UCCKPL | UCMST | UCSYNC | UCMODE_0;

	/* Set clock source to SMCLK */
	UCB0CTL1 = UCSSEL_2 | UCSWRST;

	/* Set bitrate to 250kbps */
	UCB0BR0 = 4;

	spi_mux_pins();

	/* Enable SPI */
	UCB0CTL1 &= ~UCSWRST;

	/* Enable interrupts */
	/* TODO: uncomment further */
#if 0
	IE2 |= UCB0RXIE | UCB0TXIE;
#endif
}

void spi_send_receive_byte_sync(unsigned char send_data,
		unsigned char *receive_data)
{
	char buf[32] = {0};
	unsigned char received;

	while (!(IFG2 & UCB0TXIFG)); /* TODO: add timeout and return -1 */
	UCB0TXBUF = send_data;

	while (!(IFG2 & UCB0RXIFG));
	received = UCB0RXBUF;
	if (receive_data)
		*receive_data = received;

	/* TODO: for debug (temporary); remove further */
	snprintf(buf, 32, "%#x : %#x\r\n", (unsigned int)send_data,
			(unsigned int)received);
	uart_pc_write_str_sync(buf);
}

void spi_send_receive_packet_sync(unsigned char *send_packet,
		unsigned char *receive_packet, size_t size)
{
	size_t i;

	if (!send_packet)
		return;

	/* Enable Joystick Chip Select: set "Attention" line to 0 */
	P1OUT &= ~BIT0;

	for (i = 0; i < size; ++i) {
		spi_send_receive_byte_sync(send_packet[i],
				receive_packet ? receive_packet + i : NULL);
	}

	/* Release CS */
	P1OUT |= BIT0;
}
