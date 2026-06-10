PS3DEV ?= /usr/local/ps3dev

TARGET = wm_restorer
ELF = $(TARGET).elf
SPRX = $(TARGET).sprx
SELF = $(TARGET).self

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

# Build everything (ELF, SPRX, SELF)
all: $(ELF) $(SPRX) $(SELF)

# Compile object
$(TARGET).o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link ELF
$(ELF): $(TARGET).o
	$(CC) $^ $(LDFLAGS) -o $@

# Generate SPRX from ELF. Prefer prxgen if available, fall back to sprxlinker.
$(SPRX): $(ELF)
	@if command -v prxgen >/dev/null 2>&1; then \
		prxgen $(ELF) $(SPRX); \
	elif command -v sprxlinker >/dev/null 2>&1; then \
		sprxlinker -o $(SPRX) $(ELF); \
	else \
		echo "Error: neither prxgen nor sprxlinker found in PATH"; exit 1; \
	fi

# Generate SELF from SPRX using make_fself (commonly provided by PS3 toolchain)
$(SELF): $(SPRX)
	@if command -v make_fself >/dev/null 2>&1; then \
		make_fself $(SPRX) $(SELF); \
	else \
		echo "Error: make_fself not found in PATH"; exit 1; \
	fi

clean:
	rm -f *.o *.elf *.sprx *.self
