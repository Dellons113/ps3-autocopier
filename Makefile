PS3DEV ?= /usr/local/ps3dev
PSL1GHT ?= /usr/local/ps3dev

CC = powerpc64-ps3-elf-gcc

CFLAGS = -O2 -Wall -mcpu=cell \
          -I$(PS3DEV)/ppu/include

LDFLAGS = -L$(PS3DEV)/ppu/lib \
           -lpsl1ght -lrt -lc -lm

TARGET = wm_restorer

all: $(TARGET).elf

$(TARGET).o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(TARGET).o
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	rm -f *.o *.elf
