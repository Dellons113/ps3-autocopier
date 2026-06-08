#include <sys/prx.h>
#include <sys/ppu_thread.h>
#include <lv2/sysfs.h>
#include <string.h>

// Definisi Modul Plugin
SYS_MODULE_INFO(restorer, 0, 1, 1);
SYS_MODULE_START(restorer_start);
SYS_MODULE_STOP(restorer_stop);

sys_ppu_thread_t thread_id;

// Fungsi Salin File
void check_and_copy() {
    int fd;
    
    // 1. Cek apakah file sudah ada di HDD (contoh: file bahasa webMAN)
    if (sysFsOpen("/dev_hdd0/tmp/wm_lang/LANG_ID.TXT", SYS_O_RDONLY, &fd, NULL, 0) == 0) {
        sysFsClose(fd);
        return; // File ada, hentikan proses
    }

    // 2. Jika tidak ada, siapkan penyalinan dari dev_flash
    int fd_src, fd_dst;
    uint64_t read_bytes, write_bytes;
    char buffer[1024];

    if (sysFsOpen("/dev_flash/etc/brankas/LANG_ID.TXT", SYS_O_RDONLY, &fd_src, NULL, 0) == 0) {
        
        // Buat folder jika belum ada (Abaikan error jika folder sudah terbentuk)
        sysFsMkdir("/dev_hdd0/tmp", 0777);
        sysFsMkdir("/dev_hdd0/tmp/wm_lang", 0777);

        // Buat dan tulis file target
        if (sysFsOpen("/dev_hdd0/tmp/wm_lang/LANG_ID.TXT", SYS_O_WRONLY | SYS_O_CREAT | SYS_O_TRUNC, &fd_dst, NULL, 0) == 0) {
            while (sysFsRead(fd_src, buffer, sizeof(buffer), &read_bytes) == 0 && read_bytes > 0) {
                sysFsWrite(fd_dst, buffer, read_bytes, &write_bytes);
            }
            sysFsClose(fd_dst);
        }
        sysFsClose(fd_src);
    }
}

// Background Thread
static void restorer_thread(uint64_t arg) {
    check_and_copy();
    sys_ppu_thread_exit(0);
}

// Eksekutor Saat Mesin Menyala
int restorer_start(size_t args, void *argp) {
    sys_ppu_thread_create(&thread_id, restorer_thread, 0, 1000, 0x1000, SYS_PPU_THREAD_CREATE_JOINABLE, "Restorer_Auto_Copier");
    return SYS_PRX_RESIDENT;
}

int restorer_stop(void) {
    return SYS_PRX_STOP_OK;
}
