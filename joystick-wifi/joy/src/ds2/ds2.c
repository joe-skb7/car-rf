#include <ds2/ds2.h>
#include <msp430.h>
#include <uart/uart_pc.h>
#include <stdio.h>

void ds2_send_receive_byte_sync(unsigned char send_data,
		unsigned char *receive_data)
{
	char buf[32] = {0};
	unsigned char received;

	while (!(IFG2 & UCB0TXIFG)); /* TODO: add timeout and return -1 */
	UCB0TXBUF = send_data;

	while (!(IFG2 & UCB0RXIFG));
	received = UCB0RXBUF;
	if (receive_data)
		*receive_data = received;

	snprintf(buf, 32, "%#x : %#x\r\n", (unsigned int)send_data,
			(unsigned int)received);
	uart_pc_write_str_sync(buf);
}

void ds2_send_receive_packet_sync(unsigned char *send_packet,
		unsigned char *receive_packet, size_t size)
{
	size_t i;

	if (!send_packet)
		return;

	/* Enable Joystick Chip Select: set "Attention" line to 0 */
	P1OUT &= ~BIT0;

	for (i = 0; i < size; ++i) {
		ds2_send_receive_byte_sync(send_packet[i],
				receive_packet ? receive_packet + i : NULL);
	}

	/* Release CS */
	P1OUT |= BIT0;
}

void ds2_poll_buttons_sync(unsigned int *buttons)
{
	unsigned char request[] = {0x01, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00};
	unsigned char response[9];

	ds2_send_receive_packet_sync(request, response, 9);

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

	ds2_send_receive_packet_sync(send_data, receive_data, 6);

	for (i = 0; i < 6; ++i) {
		if (0xff != receive_data[i])
			return 1;
	}

	return 0;
}
#endif
