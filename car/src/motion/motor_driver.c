#include <msp430.h>
#include <motion/motor_driver_cfg.h>
#include <motion/motor_driver.h>

void md_init(void)
{
	MD_DIR |= IN1_BIT | IN2_BIT | IN3_BIT | IN4_BIT;
	MD_OUT &= ~(IN1_BIT | IN2_BIT | IN3_BIT | IN4_BIT);
}

void md_command(int cmd_flags)
{
	register unsigned char out_new = MD_OUT;

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

	MD_OUT = out_new;
}
