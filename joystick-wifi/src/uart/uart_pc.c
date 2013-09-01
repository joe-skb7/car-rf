#include <msp430.h>
#include "uart/uart_pc.h"
#include "uart/uart.h"
#include "delay.h"

#define PC_QUIRK_COUNT		1	/* count of symbols to eat (from PC) */
#define PC_QUIRK_DELAY		2000	/* msec */

/* NOTE: bug on PC: do not send data to PC before getting some data from it */
static volatile int pc_ready;
static uart_pc_rx_callback_t pc_rx_cb;

static void uart_pc_process_uart_rx(char c)
{
	if (pc_ready < PC_QUIRK_COUNT)
		++pc_ready;

	if (pc_rx_cb)
		pc_rx_cb(c);
}

/* NOTE: uart_pc_init() must be first in main init
 *       (right after clock init)
 */
__attribute__((always_inline))
void uart_pc_init(void)
{
	uart_set_rx_callback(uart_pc_process_uart_rx);
	uart_init();

	/* Wait for symbol from PC to be eaten */
	mdelay(PC_QUIRK_DELAY);
}

__attribute__((always_inline))
void uart_pc_set_rx_callback(uart_pc_rx_callback_t cb)
{
	pc_rx_cb = cb;
}

__attribute__((always_inline))
int uart_pc_write_char_sync(char c)
{
	if (pc_ready < PC_QUIRK_COUNT)
		return -EPCNOTREADY;
	return uart_send_char_sync(c);
}

__attribute__((always_inline))
int uart_pc_write_str_sync(char *s)
{
	if (pc_ready < PC_QUIRK_COUNT)
		return -EPCNOTREADY;
	return uart_send_str_sync(s);
}
