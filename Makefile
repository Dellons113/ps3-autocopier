# Deklarasi direktori SDK
PS3DEV  ?= /usr/local/ps3dev
PSL1GHT ?= /usr/local/ps3dev/psl1ght

# Deklarasi Tools yang digunakan
CC = powerpc64-ps3-elf-gcc
MAKE_FSELF = make_fself

# Konfigurasi Compiler (CFLAGS) & Linker (LDFLAGS)
CFLAGS = -O2 -Wall -mcpu=cell -fno-builtin -fno-exceptions \
         -I$(PS3DEV)/ppu/include \
         -I$(PSL1GHT)/ppu/include

LDFLAGS = -L$(PS3DEV)/ppu/lib -L$(PSL1GHT)/ppu/lib -lpsl1ght -lrt -lc -lm

# --- TARGET BUILD ---

# Target utama yang dipanggil oleh perintah 'make'
all: main.sprx

# Langkah 1: Compile file C menjadi Object (.o)
main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

# Langkah 2: Link Object menjadi ELF (.elf)
main.elf: main.o
	$(CC) main.o $(LDFLAGS) -o main.elf

# Langkah 3: Konversi ELF menjadi SPRX
main.sprx: main.elf
	$(MAKE_FSELF) main.elf main.sprx

# Perintah 'make clean' untuk menghapus file hasil compile
clean:
	rm -f main.o main.elf main.sprx
