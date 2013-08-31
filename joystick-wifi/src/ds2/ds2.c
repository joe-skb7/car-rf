#include <spi/spi.h>
#include <ds2/ds2.h>

void ds2_poll_buttons_sync(unsigned int *buttons)
{
	unsigned char request[] = {0x01, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00};
	unsigned char response[9];

	spi_send_receive_packet_sync(request, response, 9);

	if (buttons) {
		((uint8_t*)buttons)[0] = response[3];
		((uint8_t*)buttons)[1] = response[4];
	}
}

/* TODO */
#if 0
int ds2_handshake(void)
{
	uint8_t send_data[] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
	uint8_t receive_data[6];
	size_t i;

	spi_send_receive_packet_sync(send_data, receive_data, 6);

	for (i = 0; i < 6; ++i) {
		if (0xff != receive_data[i])
			return 1;
	}

	return 0;
}
#endif
