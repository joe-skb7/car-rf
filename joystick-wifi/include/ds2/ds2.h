#ifndef DS2_H
#define DS2_H

#include <stdio.h>

void ds2_init_spi(void);
void ds2_init(void);
void ds2_send_receive_packet_sync(const unsigned char* send_packet,
		unsigned char* receive_packet, size_t size);
void ds2_poll_buttons_sync(unsigned int *buttons);

/* TODO: do smth w/ handshake routine */
#if 0
int ds2_handshake(void);
#endif

#endif /* DS2_H */
