#include <ds2/ds2.h>
#include <wl/wl.h>
#include <spi/spi.h>
#include <msp430.h>
#include <stdint.h>
#include <stdio.h>
#include <uart/uart_pc.h>
#include <delay.h>
#include <string.h>

#define DEBUG

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

	ds2_init();

	wl_init();
	wl_goto_tx();

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
	static unsigned int old_buttons = 0;
	unsigned int buttons;
	unsigned int t;

	__disable_interrupt();

	ds2_init_spi();
	ds2_poll_buttons_sync(&buttons);
	wl_init_spi();

	snprintf(buf, 20, "buttons: %#x\r\n", buttons);
	uart_pc_write_str_sync(buf);

#ifdef DEBUG
	t = wl_read_register(WL_REG_STATUS);
	snprintf(buf, 20, "%x\r\n", t);
	uart_pc_write_str_sync(buf);
#endif

	if(old_buttons != buttons)
	{
		wl_send_data_sync((unsigned char*)&buttons, sizeof(buttons));
		old_buttons = buttons;
	}
/*
	wl_send_data_sync((unsigned char*)buf, 20);
*/
	__enable_interrupt();

	mdelay(100);
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

#if 0

void wl_do_shit(void)
{
	char buf[30] = {0};
	static int c = 0;
	int conf = wl_read_register(WL_REG_CONFIG);

	++c;
	snprintf(buf, 30, "Hello! %d %d\r\n", c, conf);

	wl_send_data_sync((unsigned char*)buf, strlen(buf));

	uart_pc_write_str_sync(buf);
}

int main()
{
	int ret;

	ret = mcu_init();
	if (ret)
		return ret;

	uart_pc_set_rx_callback(process_uart_rx);
	uart_pc_init();

	wl_init();
	wl_goto_tx();

	__enable_interrupt();

	for(;;)
	{
		wl_do_shit();
		mdelay(100);
	}
}
#endif

/* NOTE: Must be short and fast since is done in ISR */
static void process_uart_rx(char c)
{
	uart_pc_write_char_sync(c); /* echo */
}
