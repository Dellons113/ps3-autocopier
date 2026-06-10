# Makefile modern untuk PS3: deteksi toolchain dan build ELF -> SPRX -> SELF
# Sesuaikan PS3DEV atau set CROSS_COMPILE jika diperlukan.

PS3DEV ?= /usr/local/ps3dev
CROSS_COMPILE ?= ppu-lv2-
CC := $(CROSS_COMPILE)gcc
OBJCOPY := $(CROSS_COMPILE)objcopy
STRIP := $(CROSS_COMPILE)strip

TARGET ?= wm_restorer
ELF := $(TARGET).elf
SPRX := $(TARGET).sprx
SELF := $(TARGET).self

CFLAGS := -O2 -fPIC -Wall -Wextra -fvisibility=hidden -mcpu=cell -I$(PS3DEV)/ppu/include
LDFLAGS := -L$(PS3DEV)/ppu/lib -lpsl1ght -lrt -lc

SRCS := sprx_plugin.c wm_copy.c
OBJS := $(SRCS:.c=.o)

.PHONY: all clean check_toolchain install

all: check_toolchain $(ELF) $(SPRX) $(SELF)

check_toolchain:
	@echo "Checking toolchain..."
	@if command -v $(CC) >/dev/null 2>&1; then \
		echo "Using $(CC)"; \
	else \
		echo "ERROR: $(CC) not found. Install ps3toolchain or set CROSS_COMPILE/PS3DEV."; exit 1; \
	fi

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(ELF): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

# Generate SPRX from ELF. Prefer prxgen if available, fall back ke sprxlinker.
$(SPRX): $(ELF)
	@if command -v prxgen >/dev/null 2>&1; then \
		prxgen $(ELF) $(SPRX); \
	elif command -v sprxlinker >/dev/null 2>&1; then \
		sprxlinker -o $(SPRX) $(ELF); \
	else \
		echo "Error: neither prxgen nor sprxlinker found in PATH"; exit 1; \
	fi
	@$(STRIP) --strip-unneeded $(SPRX) >/dev/null 2>&1 || true

# Generate SELF from SPRX using make_fself (toolchain)
$(SELF): $(SPRX)
	@if command -v make_fself >/dev/null 2>&1; then \
		make_fself $(SPRX) $(SELF); \
	else \
		echo "Error: make_fself not found in PATH"; exit 1; \
	fi

clean:
	rm -f $(OBJS) $(ELF) $(SPRX) $(SELF)
