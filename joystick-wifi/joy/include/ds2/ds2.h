#ifndef DS2_H
#define DS2_H

#include <stdlib.h>

void ds2_send_receive_byte_sync(unsigned char send_data,
		unsigned char *receive_data);

void ds2_send_receive_packet_sync(unsigned char *send_packet,
		unsigned char *receive_packet, size_t size);

void ds2_poll_buttons_sync(unsigned int *buttons);
int ds2_handshake(void);

#endif /* DS2_H */
