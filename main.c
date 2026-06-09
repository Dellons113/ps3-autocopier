#include <psl1ght/module.h>
#include <sys/thread.h>
#include <lv2/fs.h>
#include <fcntl.h>
#include <string.h>

// Definisi Modul Plugin
SYS_MODULE_INFO("restorer", 0, 1, 1);
SYS_MODULE_START(restorer_start);
SYS_MODULE_STOP(restorer_stop);

sys_ppu_thread_t thread_id;

// Fungsi Salin (Menggunakan standar pembacaan PSL1GHT)
void check_and_copy() {
    int fd;
    
    // 1. Cek apakah file sudah ada di HDD
    if (lv2FsOpen("/dev_hdd0/tmp/wm_lang/LANG_ID.TXT", O_RDONLY, &fd, 0, 0) == 0) {
        lv2FsClose(fd);
        return;
    }

    // 2. Jika tidak ada, siapkan penyalinan dari dev_flash
    int fd_src, fd_dst;
    uint64_t read_bytes, write_bytes;
    char buffer[1024];

    if (lv2FsOpen("/dev_flash/etc/brankas/LANG_ID.TXT", O_RDONLY, &fd_src, 0, 0) == 0) {
        
        lv2FsMkdir("/dev_hdd0/tmp/wm_lang", 0777);
        
        if (lv2FsOpen("/dev_hdd0/tmp/wm_lang/LANG_ID.TXT", O_WRONLY | O_CREAT | O_TRUNC, &fd_dst, 0, 0) == 0) {
            while (lv2FsRead(fd_src, buffer, sizeof(buffer), &read_bytes) == 0 && read_bytes > 0) {
                lv2FsWrite(fd_dst, buffer, read_bytes, &write_bytes);
            }
            lv2FsClose(fd_dst);
        }
        lv2FsClose(fd_src);
    }
}

// Background Thread
static void restorer_thread(void *arg) {
    check_and_copy();
    sys_ppu_thread_exit(0);
}

// Eksekutor Saat Mesin Menyala
int restorer_start(size_t args, void *argp) {
    sys_ppu_thread_create(&thread_id, restorer_thread, NULL, 1000, 0x1000, SYS_PPU_THREAD_CREATE_JOINABLE, "Restorer_Thread");
    return 0; 
}

int restorer_stop(void) {
    return 0;
}
