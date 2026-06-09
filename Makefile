OUTPUT		:= restorer
OBJS		:= main.o

# 1. Deklarasi Lokasi SDK Terlebih Dahulu
PS3DEV		?= /usr/local/ps3dev
PSL1GHT		?= $(PS3DEV)/psl1ght
export PATH	:= $(PATH):$(PS3DEV)/bin:$(PS3DEV)/ppu/bin:$(PS3DEV)/spu/bin

# 2. Beritahu Compiler Tempat Mencari File PSL1GHT (-I dan -L)
CFLAGS		:= -O2 -Wall -Wno-strict-aliasing -I$(PSL1GHT)/ppu/include -I$(PS3DEV)/portlibs/ppu/include
CXXFLAGS	:= $(CFLAGS) -fno-exceptions -fno-rtti
LDFLAGS		:= -mprx -L$(PSL1GHT)/ppu/lib -L$(PS3DEV)/portlibs/ppu/lib

PREFIX		:= powerpc64-ps3-elf-
CC			:= $(PREFIX)gcc
CXX			:= $(PREFIX)g++
OBJCOPY		:= $(PREFIX)objcopy

all: $(OUTPUT).prx

$(OUTPUT).prx: $(OUTPUT).elf
	$(OBJCOPY) -O binary $< $@
	@mv $(OUTPUT).prx $(OUTPUT).sprx

$(OUTPUT).elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) -lrt -llv2

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.elf *.prx *.sprx
