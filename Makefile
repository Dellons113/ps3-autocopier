# Pastikan variabel ini otomatis terbaca jika kosong
PS3DEV  ?= /usr/local/ps3dev
PSL1GHT ?= /usr/local/ps3dev/psl1ght

CC = powerpc64-ps3-elf-gcc

# FLAG INI ADALAH KUNCI UNTUK MEMPERBAIKI ERROR KAMU
# Tambahkan -I$(PSL1GHT)/ppu/include agar compiler menemukan <psl1ght/module.h>
CFLAGS = -O2 -Wall -mcpu=cell -fno-builtin -fno-exceptions \
         -I$(PS3DEV)/ppu/include \
         -I$(PSL1GHT)/ppu/include

LDFLAGS = -L$(PS3DEV)/ppu/lib -L$(PSL1GHT)/ppu/lib

# Target utama SPRX kamu
all: main.sprx

# Aturan kompilasi C ke Object (.o)
main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

# --- CATATAN UNTUK SPRX ---
# Dari main.o, kamu perlu proses linker ke .prx, lalu gunakan make_fself untuk membuat .sprx
# Jika kamu menggunakan prx.mak atau tools tambahan, definisikan di sini.
# Contoh umum (sesuaikan dengan alur toolchain kamu):
# main.elf: main.o
# 	$(CC) main.o $(LDFLAGS) -o main.elf
# main.sprx: main.elf
# 	make_fself main.elf main.sprx
