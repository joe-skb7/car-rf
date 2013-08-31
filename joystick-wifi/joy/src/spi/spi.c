#include <spi/spi.h>
#include <msp430.h>

void spi_mux_pins(void)
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
