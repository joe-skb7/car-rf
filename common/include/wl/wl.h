#ifndef WL_H
#define WL_H

#include <msp430.h>
#include <stdlib.h>

#define WL_MAX_PACKET_SIZE 32

/* nRF24L01 register addrs */
#define WL_REG_CONFIG 0x00
#define WL_REG_EN_AA 0x01
#define WL_REG_SETUP_RETR 0x04
#define WL_REG_STATUS 0x07
#define WL_REG_FEATURE 0x1d
#define WL_REG_RX_PW_P0 0x11

#define WL_PWR_UP BIT1
#define WL_EN_CRC BIT3
#define WL_PRIM_RX BIT0
#define WL_EN_DPL BIT2
#define WL_TX_DS BIT5
#define WL_RX_DR BIT6

typedef void (*wl_rx_cb)(const unsigned char* data, size_t len);
typedef void (*wl_tx_cb)(void);

void wl_init(void);
void wl_init_spi(void);
void wl_set_rx_cb(wl_rx_cb cb);
void wl_set_tx_cb(wl_tx_cb cb);
void wl_send_receive_cmd_sync(const unsigned char* req, unsigned char* resp,
		unsigned int size);
void wl_send_data_sync(const unsigned char* data, int size);
unsigned char wl_read_register(unsigned char addr);
void wl_write_register(unsigned char addr, unsigned char val);
void wl_set_reg_bits(unsigned char reg_addr, unsigned char bits);
void wl_unset_reg_bits(unsigned char reg_addr, unsigned char bits);
void wl_goto_rx(void);
void wl_goto_tx(void);

#endif /* WL_H */
