#include <psl1ght/lv2.h>
#include <psl1ght/lv2/modules.h>
#include <psl1ght/lv2/fs.h>
#include <psl1ght/lv2/thread.h>
#include <string.h>

// Definisi Modul
LV2_MODULE_INFO(restorer, 0, 1, 1);
LV2_MODULE_START(restorer_start);
LV2_MODULE_STOP(restorer_stop);

// Fungsi Salin (Menggunakan fungsi file system PSL1GHT)
void check_and_copy() {
    int fd;
    // Cek file
    if (lv2FsOpen("/dev_hdd0/tmp/wm_lang/LANG_ID.TXT", LV2_FS_O_RDONLY, &fd, NULL, 0) == 0) {
        lv2FsClose(fd);
        return;
    }

    int fd_src, fd_dst;
    uint64_t read_bytes, write_bytes;
    char buffer[1024];

    if (lv2FsOpen("/dev_flash/etc/brankas/LANG_ID.TXT", LV2_FS_O_RDONLY, &fd_src, NULL, 0) == 0) {
        lv2FsMkdir("/dev_hdd0/tmp/wm_lang", 0777);
        if (lv2FsOpen("/dev_hdd0/tmp/wm_lang/LANG_ID.TXT", LV2_FS_O_WRONLY | LV2_FS_O_CREAT | LV2_FS_O_TRUNC, &fd_dst, NULL, 0) == 0) {
            while (lv2FsRead(fd_src, buffer, sizeof(buffer), &read_bytes) == 0 && read_bytes > 0) {
                lv2FsWrite(fd_dst, buffer, read_bytes, &write_bytes);
            }
            lv2FsClose(fd_dst);
        }
        lv2FsClose(fd_src);
    }
}

static void restorer_thread(uint64_t arg) {
    check_and_copy();
    lv2ThreadExit(0);
}

int restorer_start(size_t args, void *argp) {
    sys_ppu_thread_t thread_id;
    lv2ThreadCreate(&thread_id, restorer_thread, 0, 1000, 0x1000, 0, "Restorer_Thread");
    return 0; // LV2_MODULE_RESIDENT;
}

int restorer_stop(void) {
    return 0;
}
