PS3DEV ?= /usr/local/ps3dev

TARGET = wm_restorer

CC = powerpc64-ps3-elf-gcc

CFLAGS = \
-O2 \
-Wall \
-mcpu=cell \
-I$(PS3DEV)/ppu/include

LDFLAGS = \
-L$(PS3DEV)/ppu/lib \
-lpsl1ght \
-lrt \
-lc

all: $(TARGET).elf

$(TARGET).o: source/main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(TARGET).o
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	rm -f *.o *.elf *.sprx *.self
