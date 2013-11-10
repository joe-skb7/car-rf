#include <wl/wl.h>
#include <delay.h>
#include <spi/spi.h>
#include <uart/uart_pc.h>
#include <stdio.h>
#include <string.h>

static volatile int packets_received;
static wl_rx_cb rx_cb = 0;
static wl_tx_cb tx_cb = 0;

void wl_send_receive_cmd_sync(const unsigned char* req, unsigned char* resp,
				unsigned int size)
{
	P2OUT &= ~BIT5;
	spi_send_receive_packet_sync(req, resp, size);
	P2OUT |= BIT5;
}

unsigned char wl_read_register(unsigned char addr)
{
	unsigned char cmd = 0x1f & addr;
	unsigned char req[2] = {0};
	unsigned char resp[2] = {0};

	req[0] = cmd;

	wl_send_receive_cmd_sync(req, resp, sizeof(req));

	return resp[1];
}

void wl_write_register(unsigned char addr, unsigned char val)
{
	unsigned char cmd = 0x20 | (0x1f & addr);
	unsigned char req[2] = {0};
	unsigned char resp[2] = {0};

	req[0] = cmd;
	req[1] = val;

	wl_send_receive_cmd_sync(req, resp, sizeof(req));
}

void wl_set_reg_bits(unsigned char reg_addr, unsigned char bits)
{
	unsigned char val = wl_read_register(reg_addr);

	val |= bits;
	wl_write_register(reg_addr, val);
}

void wl_unset_reg_bits(unsigned char reg_addr, unsigned char bits)
{
	unsigned char val = wl_read_register(reg_addr);

	val &= ~bits;
	wl_write_register(reg_addr, val);
}

void wl_goto_rx(void)
{
	wl_set_reg_bits(WL_REG_CONFIG, WL_PRIM_RX);

	/* set CE high to start receiving data */
	P2OUT |= BIT4;
}

void wl_goto_tx(void)
{
	wl_unset_reg_bits(WL_REG_CONFIG, WL_PRIM_RX);
}

void wl_send_data_sync(const unsigned char* data, int size)
{
	unsigned char req[33] = {0};
	unsigned char resp[33];

	req[0] = 0xa0; /* W_TX_PAYLOAD */
	memcpy(req + 1, data, size);

	/* TODO: enable DPL */
	/* always sending 32 bytes (for now) */
	wl_send_receive_cmd_sync(req, resp, 33/*size + 1*/);

	/* start transmission by short pulse on CE */
	P2OUT |= BIT4;
	udelay(20);
	P2OUT &= ~BIT4;
}

void wl_read_data_sync(unsigned char* buf, int size)
{
	unsigned char cmd = 0x61; /* R_RX_PAYLOAD */
	unsigned char req[WL_MAX_PACKET_SIZE + 1] = {0};
	unsigned char resp[WL_MAX_PACKET_SIZE + 1];

	req[0] = cmd;

	if(size > WL_MAX_PACKET_SIZE)
		size = WL_MAX_PACKET_SIZE;

	wl_send_receive_cmd_sync(req, resp, size + 1);

	if(buf)
		memcpy(buf, resp + 1, size);
}

void wl_read_packet(unsigned char* buf, int size, int* read_size)
{
	int len = wl_read_register(WL_REG_RX_PW_P0);
	int copy_size = (len < size) ? len : size;
	unsigned char data[WL_MAX_PACKET_SIZE];

	char pbuf[20] = {0};
	snprintf(pbuf, 20, "len = %d\r\n", len);
	uart_pc_write_str_sync(pbuf);

	wl_read_data_sync(data, len);

	if(buf)
		memcpy(buf, data, copy_size);

	if(read_size)
		*read_size = copy_size;
}

void wl_init_spi(void)
{
	spi_init(1, 0, 1);
#if 0
	/* Disable USCI */

	UCB0CTL1 |= UCSWRST;
	/*
	 * Control Register 0
	 *
	 * UCCKPH -- Data is captured on the first UCLK edge and changed
	 *           on the following edge
	 * ~UCCKPL -- Inactive state is low
	 * UCMSB -- MSB first
	 * ~UC7BIT -- 8-bit
	 * ~UCMST -- Slave mode
	 * UCMODE_0 -- 3-Pin SPI
	 * UCSYNC -- Synchronous Mode
	 *
	 * Note: ~<BIT> indicates that <BIT> has value zero
	 */
	UCB0CTL0 = UCCKPH | UCMSB | UCMODE_0 | UCSYNC | UCMST;

	/*
	 * Control Register 1
	 *
	 * UCSSEL_2 -- SMCLK
	 * UCSWRST -- Enabled. USCI logic held in reset state
	 */
	UCB0CTL1 = UCSSEL_2 | UCSWRST;

	/* P1.5 - UCB0CLK
	 * P1.6 - UCB0SOMI
	 * P1.7 - UCB0SIMO
	 */
	P1SEL  |= BIT5 | BIT6 | BIT7;
	P1SEL2 |= BIT5 | BIT6 | BIT7;

	/* Bit Rate Control Register 0 */
	UCB0BR0 = 4;

	/* Enable USCI */
	UCB0CTL1 &= ~UCSWRST;
#endif
}

void wl_init(void)
{
	mdelay(20);

	wl_init_spi();

	/* enable interrupts from wireless module */
	P1DIR &= ~BIT3;
	P1IE  |= BIT3;
	P1IES |= BIT3;

	P2DIR |= BIT4 | BIT5;

	P2OUT |= BIT5;

	/* power up */
	wl_set_reg_bits(WL_REG_CONFIG, WL_PWR_UP);

	mdelay(5);

	/* disable auto acknowledgment on all pipes */
	wl_write_register(WL_REG_EN_AA, 0x00);

	/* enable CRC */
	wl_set_reg_bits(WL_REG_CONFIG, WL_EN_CRC);

	/* enable dynamic payload length */
/*	wl_set_reg_bits(WL_REG_FEATURE, WL_EN_DPL);*/

	/* set rx payload size */
	wl_write_register(WL_REG_RX_PW_P0, 32);
}

void wl_set_rx_cb(wl_rx_cb cb)
{
	rx_cb = cb;
}

void wl_set_tx_cb(wl_tx_cb cb)
{
	tx_cb = cb;
}

__attribute__((__interrupt__(PORT1_VECTOR)))
void on_wl_interrupt(void)
{
	if(P1IFG & BIT3)
	{
		unsigned char status;

		P1IFG &= ~BIT3;

		status = wl_read_register(WL_REG_STATUS);

		if(status & WL_TX_DS) /* transmit successfull */
		{
			uart_pc_write_str_sync("transmit successfull\r\n");
			wl_set_reg_bits(WL_REG_STATUS, WL_TX_DS);

			if(tx_cb)
				tx_cb();
		}

		if(status & WL_RX_DR) /* data ready */
		{
			unsigned char msg[WL_MAX_PACKET_SIZE + 1] = {0};
			int msg_len;

			++packets_received;

			uart_pc_write_str_sync("!!!!!!!!");
			wl_read_packet(msg, WL_MAX_PACKET_SIZE, &msg_len);
			wl_set_reg_bits(WL_REG_STATUS, WL_RX_DR);

			if(rx_cb)
				rx_cb(msg, msg_len);
		}
	}
}
