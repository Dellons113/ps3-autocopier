#include <psl1ght/module.h>
#include <sys/thread.h>
#include <sys/timer.h>
#include <lv2/fs.h>
#include <fcntl.h>
#include <string.h>

SYS_MODULE_INFO("wm_restorer", 0, 1, 1);
SYS_MODULE_START(restorer_start);
SYS_MODULE_STOP(restorer_stop);

static sys_ppu_thread_t thread_id;

typedef struct
{
    const char *src;
    const char *dst;
} restore_file_t;

/*
 * Tambahkan semua file wm_lang dan wm_res yang ingin direstore.
 */
static restore_file_t restore_list[] =
{
    {
        "/dev_flash/etc/brankas/wm_lang/LANG_ID.TXT",
        "/dev_hdd0/tmp/wm_lang/LANG_ID.TXT"
    },

    {
        "/dev_flash/etc/brankas/mygames.xml",
        "/dev_hdd0/xmlhost/game_plugin/mygames.xml"
    },

    /* Contoh wm_res */
    {
        "/dev_flash/etc/brankas/wm_res/icon_wm.png",
        "/dev_hdd0/tmp/wm_res/icon_wm.png"
    },

    {
        "/dev_flash/etc/brankas/wm_res/setup.png",
        "/dev_hdd0/tmp/wm_res/setup.png"
    }
};

static int file_exists(const char *path)
{
    int fd;

    if(lv2FsOpen(path, O_RDONLY, &fd, 0, 0) == 0)
    {
        lv2FsClose(fd);
        return 1;
    }

    return 0;
}

static void copy_file(const char *src, const char *dst)
{
    int fd_src;
    int fd_dst;

    uint64_t read_bytes;
    uint64_t write_bytes;

    char buffer[4096];

    if(lv2FsOpen(src, O_RDONLY, &fd_src, 0, 0) != 0)
        return;

    if(lv2FsOpen(dst,
                 O_WRONLY | O_CREAT | O_TRUNC,
                 &fd_dst,
                 0,
                 0) != 0)
    {
        lv2FsClose(fd_src);
        return;
    }

    while(lv2FsRead(fd_src,
                    buffer,
                    sizeof(buffer),
                    &read_bytes) == 0 &&
          read_bytes > 0)
    {
        if(lv2FsWrite(fd_dst,
                      buffer,
                      read_bytes,
                      &write_bytes) != 0)
        {
            break;
        }
    }

    lv2FsClose(fd_dst);
    lv2FsClose(fd_src);
}

static void create_dirs(void)
{
    lv2FsMkdir("/dev_hdd0/tmp", 0777);
    lv2FsMkdir("/dev_hdd0/tmp/wm_lang", 0777);
    lv2FsMkdir("/dev_hdd0/tmp/wm_res", 0777);

    lv2FsMkdir("/dev_hdd0/xmlhost", 0777);
    lv2FsMkdir("/dev_hdd0/xmlhost/game_plugin", 0777);
}

static void restore_all(void)
{
    unsigned int i;

    create_dirs();

    for(i = 0;
        i < (sizeof(restore_list) / sizeof(restore_list[0]));
        i++)
    {
        if(!file_exists(restore_list[i].dst))
        {
            copy_file(
                restore_list[i].src,
                restore_list[i].dst
            );
        }
    }
}

static void restorer_thread(uint64_t arg)
{
    int i;

    /* tunggu HDD siap */
    for(i = 0; i < 10; i++)
    {
        int fd;

        if(lv2FsOpen("/dev_hdd0", O_RDONLY, &fd, 0, 0) == 0)
        {
            lv2FsClose(fd);
            break;
        }

        sys_timer_sleep(1);
    }

    restore_all();

    sys_ppu_thread_exit(0);
}

int restorer_start(size_t args, void *argp)
{
    sys_ppu_thread_create(
        &thread_id,
        restorer_thread,
        0,
        1000,
        0x4000,
        SYS_PPU_THREAD_CREATE_DETACHED,
        "WM_Restorer"
    );

    return SYS_PRX_RESIDENT;
}

int restorer_stop(void)
{
    return SYS_PRX_STOP_OK;
}
