#include <motion/vehicle.h>
#include <motion/motor_driver.h>

/* TODO: use PWM outputs to specify speed for each action */

void vehicle_init(void)
{
	md_init();
}

void vehicle_stop(void)
{
	md_command(M1_STOP | M2_STOP);
}

void vehicle_forward(void)
{
	md_command(M1_COROTATION | M2_COROTATION);
}

void vehicle_backward(void)
{
	md_command(M1_REVERSAL | M2_REVERSAL);
}

void vehicle_forward_left(void)
{
	md_command(M1_STOP | M2_COROTATION);
}

void vehicle_forward_right(void)
{
	md_command(M1_COROTATION | M2_STOP);
}

void vehicle_turn_left(void)
{
	md_command(M1_REVERSAL | M2_COROTATION);
}

void vehicle_turn_right(void)
{
	md_command(M1_COROTATION | M2_REVERSAL);
}

void vehicle_backward_left(void)
{
	md_command(M1_STOP | M2_REVERSAL);
}

void vehicle_backward_right(void)
{
	md_command(M1_REVERSAL | M2_STOP);
}
