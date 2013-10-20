PROJ = libspi
OUT = $(PROJ).a

CROSS_COMPILE = msp430-
CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
MCU = msp430g2553
XTAL = 1000000

OBJS = spi.o

INCLUDES = -Iinclude/
DEFINES = -D'XTAL=$(XTAL)UL'

CFLAGS = -mmcu=$(MCU)
CFLAGS += -Os -s -Wall -pedantic -ansi -Werror -Wextra
CFLAGS += $(INCLUDES)
CFLAGS += $(DEFINES)
LDFLAGS = -mmcu=$(MCU) -mdisable-watchdog

all: $(OBJS)
	@echo [AR] $(OUT)
	@$(AR) rcs $(OUT) $(OBJS)

%.o: %.c
	@echo [CC] $<
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -rf $(OUT) $(OBJS)

.PHONY: all clean
