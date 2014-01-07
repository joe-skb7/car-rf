#include <msp430.h>
#include <stdio.h>
#include <motion/vehicle.h>
#include <wl/wl.h>
#include <uart/uart_pc.h>
#include <delay.h>

#define JS_UP		BIT4
#define JS_RIGHT	BIT5
#define JS_DOWN		BIT6
#define JS_LEFT		BIT7

static volatile unsigned char js_data;

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

static void wl_data_received(const unsigned char *data, size_t len)
{
	if (len < 2) {
#ifdef DEBUG
		char s[20];

		snprintf(s, 20, "wtf1\r\n");
		uart_pc_write_str_sync(s);
#endif
		return;
	}

	js_data = ~(data[0]);
}

static int init(void)
{
	int ret;

	ret = mcu_init();
	if (ret)
		return ret;

	uart_pc_init();
	gpio_init();
	vehicle_init();
	wl_set_rx_cb(wl_data_received);
	wl_init();
	wl_goto_rx();

	__enable_interrupt();

#if 0
	/* Enter LPM0, interrupts enabled */
	__bis_SR_register(LPM0_bits + GIE);
#endif

	return 0;
}

static void loop(void)
{
	int flags = 0;

	if (js_data & JS_UP)
		flags |= Y_UP;
	if (js_data & JS_RIGHT)
		flags |= X_RIGHT;
	if (js_data & JS_DOWN)
		flags |= Y_DOWN;
	if (js_data & JS_LEFT)
		flags |= X_LEFT;

	vehicle_move(flags);

	mdelay(100);
}

int main(void)
{
	if (init())
		return err_loop();

	for (;;)
		loop();

	return 0;
}
