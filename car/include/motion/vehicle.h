#ifndef VEHICLE_H
#define VEHICLE_H

void vehicle_init(void);

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
