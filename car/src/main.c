#include <msp430.h>

#define BTN_BIT			BIT3
#define LED1_BIT		BIT0
#define LED2_BIT		BIT6

#define IN1_BIT			BIT0
#define IN2_BIT			BIT1
#define IN3_BIT			BIT2
#define IN4_BIT			BIT3

#define M1_COROTATION		0x01
#define M1_REVERSAL		0x02
#define M1_STOP			0x03
#define M2_COROTATION		0x10
#define M2_REVERSAL		0x20
#define M2_STOP			0x30

#define MOTOR_DELAY		1000000UL /* in loops (500000 ~= 1sec) */

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

	P2DIR |= IN1_BIT | IN2_BIT | IN3_BIT | IN4_BIT;
	P2OUT &= ~(IN1_BIT | IN2_BIT | IN3_BIT | IN4_BIT);
}

static int init(void)
{
	int ret;

	ret = mcu_init();
	if (ret)
		return ret;

	gpio_init();

#if 0
	/* Enter LPM0, interrupts enabled */
	__bis_SR_register(LPM0_bits + GIE);
#endif

#if 0
	__enable_interrupt();
#endif

	return 0;
}

static void motor_action(int cmd_flags)
{
	register unsigned char p2 = P2OUT;

	if ((cmd_flags & M1_STOP) == M1_STOP) {
		p2 &= ~(IN1_BIT | IN2_BIT);
	} else if ((cmd_flags & M1_COROTATION) == M1_COROTATION) {
		p2 |= IN1_BIT;
		p2 &= ~IN2_BIT;
	} else if ((cmd_flags & M1_REVERSAL) == M1_REVERSAL) {
		p2 &= ~IN1_BIT;
		p2 |= IN2_BIT;
	}

	if ((cmd_flags & M2_STOP) == M2_STOP) {
		p2 &= ~(IN3_BIT | IN4_BIT);
	} else if ((cmd_flags & M2_COROTATION) == M2_COROTATION) {
		p2 |= IN3_BIT;
		p2 &= ~IN4_BIT;
	} else if ((cmd_flags & M2_REVERSAL) == M2_REVERSAL) {
		p2 &= ~IN3_BIT;
		p2 |= IN4_BIT;
	}

	P2OUT = p2;
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
		/* Motor M1 */
		P1OUT &= ~LED2_BIT;
		P1OUT |= LED1_BIT;
		motor_action(M1_COROTATION | M2_STOP);
		__delay_cycles(MOTOR_DELAY);
		motor_action(M1_STOP | M2_STOP);
		__delay_cycles(MOTOR_DELAY);
		motor_action(M1_REVERSAL | M2_STOP);
		__delay_cycles(MOTOR_DELAY);
		motor_action(M1_STOP | M2_STOP);
		__delay_cycles(MOTOR_DELAY);

		/* Motor M2 */
		P1OUT &= ~LED1_BIT;
		P1OUT |= LED2_BIT;
		motor_action(M1_STOP | M2_COROTATION);
		__delay_cycles(MOTOR_DELAY);
		motor_action(M1_STOP | M2_STOP);
		__delay_cycles(MOTOR_DELAY);
		motor_action(M1_STOP | M2_REVERSAL);
		__delay_cycles(MOTOR_DELAY);
		motor_action(M1_STOP | M2_STOP);
		__delay_cycles(MOTOR_DELAY);
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
