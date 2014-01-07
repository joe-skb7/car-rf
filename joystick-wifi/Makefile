PROJ = ds2
OBJS =					\
	../common/src/uart/uart.o	\
	../common/src/uart/uart_pc.o	\
	../common/src/spi/spi.o		\
	../common/src/wl/wl.o		\
	../common/src/interrupts.o	\
	src/ds2/ds2.o			\
	src/main.o

OUT = $(PROJ).elf
CROSS_COMPILE ?= msp430-
CC = $(CROSS_COMPILE)gcc
MCU = msp430g2553
PROGRAMMER = rf2500
XTAL = 1000000
UART_BAUDRATE = 9600
UART_DEV = /dev/ttyACM0

INCLUDES = -Iinclude/ -I../common/include
DEFINES = -D'XTAL=$(XTAL)UL'
DEFINES += -D__STDC_VERSION__=199401L

CFLAGS = -mmcu=$(MCU)
CFLAGS += -Os -s -Wall -pedantic -ansi -Werror -Wextra
CFLAGS += $(INCLUDES)
CFLAGS += $(DEFINES)
LDFLAGS = -mmcu=$(MCU) -mdisable-watchdog
# for debug: add -g, remove -s

MSP430_INC = -I/usr/msp430/include -I/usr/lib/gcc/msp430/4.6.3/include
MSP430_DEF = -D__MSP430G2553__
MSP430_CFLAGS = $(MSP430_INC) $(MSP430_DEF)
SPARSE_FLAGS = -Wsparse-all $(MSP430_CFLAGS) $(CFLAGS)
CPPCHECK_FLAGS = --enable=all $(MSP430_CFLAGS) $(INCLUDES) $(DEFINES)
CPPCHECK_FLAGS += -D__STDC__
SPLINT_FLAGS = -weak $(MSP430_CFLAGS) $(INCLUDES) $(DEFINES)

FLASH_SIZE = "16 KB"

all: $(OBJS)
	@echo [LD] $(OUT)
	@$(CC) $(LDFLAGS) -o $(OUT) $(OBJS)

%.o: %.c
	@echo [CC] $<
	@$(CC) $(CFLAGS) -c $< -o $@

size: $(OUT)
	@echo -n  "Total: "
	@msp430-size $(OUT) | tail -1 | awk '{print $$4}' | xargs echo -n
	@echo " B / $(FLASH_SIZE)"

flash: $(OUT)
	sudo mspdebug $(PROGRAMMER) "prog $(OUT)"

clean:
	-rm -rf $(OUT) $(OBJS)

_check_sparse:
	@echo "---> Checking with sparse..."
	@find . -type f -name '*.[ch]' -exec sparse $(SPARSE_FLAGS) {} \;
	@echo

_check_cppcheck:
	@echo "---> Checking with cppcheck..."
	@cppcheck $(CPPCHECK_FLAGS) .
	@echo

__check_clang: $(OBJS)

_check_clang:
	@echo "---> Checking with clang..."
	@$(MAKE) clean >/dev/null 2>/dev/null
	@$(MAKE) CFLAGS="$(MSP430_CFLAGS) $(CFLAGS)" \
		CROSS_COMPILE="scan-build $(CROSS_COMPILE)" __check_clang
	@$(MAKE) clean >/dev/null 2>/dev/null
	@echo

_check_splint:
	@echo "---> Checking with splint..."
	@find . -type f -name '*.[ch]' -exec splint $(SPLINT_FLAGS) {} \;
	@echo

check: _check_sparse _check_cppcheck _check_clang _check_splint

debug:
	sudo mspdebug $(PROGRAMMER)

# Run serial console
serial:
	@echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	@echo "!!! Be sure to reconnect Launchpad before doing that !!!"
	@echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	@echo
	# Clear chakras
	sudo mspdebug $(PROGRAMMER) exit
	# Set up speed
	stty $(UART_BAUDRATE) -F $(UART_DEV)
	# Our firmware waits to eat one char
	echo > $(UART_DEV)
	minicom -D $(UART_DEV)

.PHONY: all flash clean debug serial
