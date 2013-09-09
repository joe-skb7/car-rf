#include <msp430.h>
#include <motion/vehicle.h>

#define BTN_BIT			BIT3
#define LED1_BIT		BIT0
#define LED2_BIT		BIT6

/* in loops (500000 ~= 1sec) */
#define FWD_DELAY		1000000UL
#define TURN_DELAY		1300000UL
#define STOP_DELAY		2000000UL

static int ignition_on;

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
	/* Enable internal pull-up for button */
	P1OUT |= BTN_BIT;
	P1REN |= BTN_BIT;

	P1DIR |= LED1_BIT | LED2_BIT;
	P1OUT &= ~(LED1_BIT | LED2_BIT);
}

static int init(void)
{
	int ret;

	ret = mcu_init();
	if (ret)
		return ret;

	gpio_init();
	vehicle_init();

#if 0
	/* Enter LPM0, interrupts enabled */
	__bis_SR_register(LPM0_bits + GIE);
#endif

#if 0
	__enable_interrupt();
#endif

	return 0;
}

static void loop(void)
{
	if (!ignition_on) {
		if (!(P1IN & BTN_BIT)) { /* button pressed */
			ignition_on = 1;
			P1OUT |= LED1_BIT | LED2_BIT;
		} else {
			return;
		}
	} else {
		int i;

		for (i = 0; i < 4; ++i) {
			/* Forward */
			vehicle_forward();
			__delay_cycles(FWD_DELAY);

			/* Forward + Left */
			vehicle_forward_left();
			__delay_cycles(TURN_DELAY);
		}

		/* Stop */
		vehicle_stop();
		__delay_cycles(STOP_DELAY);
	}
}

int main(void)
{
	if (init())
		return err_loop();

	for (;;)
		loop();

	return 0;
}
