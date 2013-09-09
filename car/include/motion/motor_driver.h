#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

/* Motor 1 commands */
#define M1_COROTATION		0x01
#define M1_REVERSAL		0x02
#define M1_STOP			0x03

/* Motor 2 commands */
#define M2_COROTATION		0x10
#define M2_REVERSAL		0x20
#define M2_STOP			0x30

void md_init(void);
void md_command(int cmd_flags);

#endif /* MOTOR_DRIVER_H */
