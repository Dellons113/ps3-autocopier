OUTPUT		:= restorer
OBJS		:= main.o
CFLAGS		:= -O2 -Wall -Wno-strict-aliasing
CXXFLAGS	:= $(CFLAGS) -fno-exceptions -fno-rtti
LDFLAGS		:= -mprx

# Path ke SDK di dalam kontainer Docker GitHub
PS3DEV		?= /usr/local/ps3dev
PSL1GHT		?= $(PS3DEV)/psl1ght
export PATH	:= $(PATH):$(PS3DEV)/bin:$(PS3DEV)/ppu/bin:$(PS3DEV)/spu/bin

PREFIX		:= ppu-
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
