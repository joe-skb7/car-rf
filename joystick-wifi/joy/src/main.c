#include <msp430.h>
#include <stdint.h>
#include <stdio.h>
#include "uart/uart_pc.h"
#include "delay.h"

static void process_uart_rx(char c);

/* Forever loop for exceptional states */
static int err_loop(void)
{
	for (;;);
	return 1;
}

static int mcu_init(void)
{
	/* If calibration constant erased */
	if (CALBC1_1MHZ == 0xff)
		return -1;

	DCOCTL = 0;		/* Select lowest DCOx and MODx settings */
	BCSCTL1 = CALBC1_1MHZ;	/* Set DCO */
	DCOCTL = CALDCO_1MHZ;

	return 0;
}

static void spi_mux_pins(void)
{
	/* P1.5 - UCB0CLK
	 * P1.6 - UCB0SOMI
	 * P1.7 - UCB0SIMO
	 */
	P1SEL  |= BIT5 | BIT6 | BIT7;
	P1SEL2 |= BIT5 | BIT6 | BIT7;
}

static void gpio_init(void)
{
	/* P1.0 - OUT (Joystick's Chip Select, "Attention")
	 * P1.4 - IN  ("Acknowledge")
	 */
	P1DIR |= BIT0;
	P1OUT |= BIT0; /* up on idle */
}

static void spi_init(void)
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

static int init(void)
{
	int ret;

	ret = mcu_init();
	if (ret)
		return ret;

	uart_pc_set_rx_callback(process_uart_rx);
	uart_pc_init();
	gpio_init();
	spi_init();

#if 0
	/* Enter LPM0, interrupts enabled */
	__bis_SR_register(LPM0_bits + GIE);
#endif

	__enable_interrupt();

	return 0;
}

static void ds2_send_receive_byte_sync(unsigned char send_data,
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

	snprintf(buf, 32, "%#x : %#x\r\n", (unsigned int)send_data,
			(unsigned int)received);
	uart_pc_write_str_sync(buf);
}

static void ds2_send_receive_packet_sync(unsigned char *send_packet,
		unsigned char *receive_packet, size_t size)
{
	size_t i;

	if (!send_packet)
		return;

	/* Enable Joystick Chip Select: set "Attention" line to 0 */
	P1OUT &= ~BIT0;

	for (i = 0; i < size; ++i) {
		ds2_send_receive_byte_sync(send_packet[i],
				receive_packet ? receive_packet + i : NULL);
	}

	/* Release CS */
	P1OUT |= BIT0;
}

static void ds2_poll_buttons_sync(unsigned int *buttons)
{
	unsigned char request[] = {0x01, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00};
	unsigned char response[9];

	ds2_send_receive_packet_sync(request, response, 9);

	if (buttons) {
		((uint8_t*)buttons)[0] = response[3];
		((uint8_t*)buttons)[1] = response[4];
	}
}

/* TODO */
#if 0
int ds2_handshake(void)
{
	uint8_t send_data[] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
	uint8_t receive_data[6];
	size_t i;

	ds2_send_receive_packet_sync(send_data, receive_data, 6);

	for (i = 0; i < 6; ++i) {
		if (0xff != receive_data[i])
			return 1;
	}

	return 0;
}
#endif

static void loop(void)
{
	char buf[20] = {0};
	unsigned int buttons;

	ds2_poll_buttons_sync(&buttons);
	snprintf(buf, 20, "buttons: %#x\r\n", buttons);
	uart_pc_write_str_sync(buf);

	mdelay(500);
}

int main(void)
{
	if (init())
		return err_loop();

#if 0
	if (ds2_handshake()) {
		uart_pc_write_str_sync("handshake failed\r\n");
		/* return err_loop(); */
	} else {
		uart_pc_write_str_sync("handshake successfull\r\n");
	}
#endif
	for (;;)
		loop();

	return 0;
}

/* NOTE: Must be short and fast since is done in ISR */
static void process_uart_rx(char c)
{
	uart_pc_write_char_sync(c); /* echo */
}
