#include <ds2/ds2.h>
#include <spi/spi.h>
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

static void gpio_init(void)
{
	/* P1.0 - OUT (Joystick's Chip Select, "Attention")
	 * P1.4 - IN  ("Acknowledge")
	 */
	P1DIR |= BIT0;
	P1OUT |= BIT0; /* up on idle */

	/* It's recommended in datasheet for this package to pulldown port 3 */
	P3REN = 0xff;
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

/* TODO: do smth w/ handshake routine */
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
