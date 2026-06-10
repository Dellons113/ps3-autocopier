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

# The project's main C file is at the repository root (main.c), not in a "source/" directory.
# Update the dependency to point to main.c so the build works as-is.
$(TARGET).o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(TARGET).o
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	rm -f *.o *.elf *.sprx *.self
