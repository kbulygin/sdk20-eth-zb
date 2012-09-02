CC=arm-elf-gcc
WARNINGS=-Wall -Wstrict-prototypes -Wno-trigraphs
INCL=-I. -I./include
CFLAGS= $(INCL) $(WARNINGS) -fpack-struct -O2 -mcpu=arm7tdmi 
LDFLAGS= -nostartfiles -N -Tlpc2292.ld -Xlinker -Map=Eth-ZB_SDK20.map

SRC_FILES = \
	src/main.c \
	src/init.c \
	src/interrupt.c \
	src/drv/stdio_uart0.c \
	src/drv/systimer.c \
	src/drv/serial.c \
	src/drv/eth.c \
	src/drv/eth_mii_phy.c \
	src/drv/zb.c \
	src/drv/spi.c \
	src/drv/wdt.c \
	src/drv/leds.c \
	src/drv/sw.c

OBJ_FILES = $(SRC_FILES:.c=.o)

all: Eth-ZB_SDK20
 
Eth-ZB_SDK20: src/startup.o $(OBJ_FILES)
	ctags -R
	$(CC) src/startup.o $(OBJ_FILES) $(LDFLAGS) -o Eth-ZB_SDK20
#	arm-elf-objdump -xS Eth-ZB_SDK20 > Eth-ZB_SDK20.d
	arm-elf-objcopy -O ihex Eth-ZB_SDK20 Eth-ZB_SDK20.hex
#	arm-elf-nm Eth-ZB_SDK20 > Eth-ZB_SDK20.n
#	rm -rf *.o
	 
src/startup.o: src/startup.s
	$(CC) -c $(CFLAGS) src/startup.s -o src/startup.o

$(OBJ_FILES): %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@


clean:
	find . -name "*.o" -delete
#	c:/cygwin/bin/find . -name "*.o" -delete
	rm -rf *.o *.elf *.gdb Eth-ZB_SDK20 *.r *.n *.d *.map
load:
	lpc21isp -PHILIPSARM -term -hex Eth-ZB_SDK20.hex /dev/ttyUSB0 38400 12000
term:
	lpc21isp -PHILIPSARM -termonly -hex Eth-ZB_SDK20.hex /dev/ttyUSB0 9600 12000

touch:	
	touch src/*.c
	touch src/drv/*.c
	touch include/*.h
