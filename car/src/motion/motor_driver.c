#include <msp430.h>
#include <motion/motor_driver_cfg.h>
#include <motion/motor_driver.h>

#define IN_BITS (IN1_BIT | IN2_BIT | IN3_BIT | IN4_BIT)

void md_init(void)
{
	MD_DIR |= IN_BITS;
	MD_OUT &= ~IN_BITS;
}

void md_command(int cmd_flags)
{
	unsigned char out_new = MD_OUT & IN_BITS;

	if ((cmd_flags & M1_STOP) == M1_STOP) {
		out_new &= ~(IN1_BIT | IN2_BIT);
	} else if ((cmd_flags & M1_COROTATION) == M1_COROTATION) {
		out_new |= IN1_BIT;
		out_new &= ~IN2_BIT;
	} else if ((cmd_flags & M1_REVERSAL) == M1_REVERSAL) {
		out_new &= ~IN1_BIT;
		out_new |= IN2_BIT;
	}

	if ((cmd_flags & M2_STOP) == M2_STOP) {
		out_new &= ~(IN3_BIT | IN4_BIT);
	} else if ((cmd_flags & M2_COROTATION) == M2_COROTATION) {
		out_new |= IN3_BIT;
		out_new &= ~IN4_BIT;
	} else if ((cmd_flags & M2_REVERSAL) == M2_REVERSAL) {
		out_new &= ~IN3_BIT;
		out_new |= IN4_BIT;
	}

	/* Ruquirements:
	 * - disable interrupts, so that P2OUT 100% is not changing
	 * - there must be no temporary values in P2OUT (motors will react)
	 * - don't touch bits in P2OUT that are not related to motor driver
	 */
	__disable_interrupt();
	out_new |= MD_OUT & ~IN_BITS;
	MD_OUT = out_new;
	__enable_interrupt();
}
