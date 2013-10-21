#ifndef VEHICLE_H
#define VEHICLE_H

#define X_LEFT		0x01
#define X_RIGHT		0x02
#define Y_UP		0x10
#define Y_DOWN		0x20

void vehicle_init(void);

void vehicle_move(int flags);

void vehicle_forward(void);
void vehicle_forward_left(void);
void vehicle_forward_right(void);

void vehicle_backward(void);
void vehicle_backward_left(void);
void vehicle_backward_right(void);

void vehicle_turn_left(void);
void vehicle_turn_right(void);

void vehicle_stop(void);

#endif /* VEHICLE_H */
