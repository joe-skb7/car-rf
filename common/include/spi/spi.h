#ifndef SPI_H
#define SPI_H

#include <stdlib.h>

void spi_init(int phase, int polarity, int is_msb);
void spi_send_receive_byte_sync(unsigned char send_data,
		unsigned char *receive_data);
void spi_send_receive_packet_sync(const unsigned char *send_packet,
		unsigned char *receive_packet, size_t size);

#endif /* SPI_H */
