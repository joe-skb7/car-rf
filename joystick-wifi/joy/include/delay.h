#ifndef DELAY_H
#define DELAY_H

#include <intrinsics.h>

/* Module contains delay routines definitions.
 *
 * XTAL macro must be specified, e.g. via -D gcc option.
 * XTAL value must be MCU clock frequency, in Hz.
 */

/* Both udelay and mdelay require it's argument to be const */
#define udelay(us)						\
	do {							\
		__delay_cycles(us * (XTAL / 1000000UL));	\
	} while (0);
#define mdelay(ms)						\
	do {							\
		__delay_cycles(ms * (XTAL / 1000UL));		\
	} while (0);

#endif /* DELAY_H */
