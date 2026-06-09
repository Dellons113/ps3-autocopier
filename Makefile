TARGET   = restorer
OBJS     = main.o

CC       = powerpc64-ps3-elf-gcc
CP       = cp

CFLAGS   = -O2 -Wall -mcpu=cell -fno-builtin -fno-exceptions \
           -I$(PSL1GHT)/ppu/include

LDFLAGS  = -L$(PSL1GHT)/ppu/lib -lpsl1ght -lv2 -Wl,-q

all: $(TARGET).sprx

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(TARGET).sprx: $(TARGET).elf
	$(CP) $(TARGET).elf $(TARGET).sprx

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).sprx
