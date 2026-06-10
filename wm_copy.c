#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pthread.h>

static pthread_t worker_thread;
static int worker_running = 0;
static int worker_stop = 0;

#define BUF_SIZE 65536

static int ensure_dir(const char *path)
{
    struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) return 0;
        /* Exists but not a dir */
        return -1;
    }
    if (mkdir(path, 0755) == 0) return 0;
    return -1;
}

static int copy_file_internal(const char *src, const char *dst, int overwrite)
{
    int in, out;
    ssize_t nread;
    char *buf = malloc(BUF_SIZE);
    if (!buf) return -1;

    if (!overwrite) {
        struct stat st;
        if (stat(dst, &st) == 0) {
            free(buf);
            return 0; /* skip existing */
        }
    }

    in = open(src, O_RDONLY);
    if (in < 0) {
        printf("[wm_restorer] open src failed %s: %s\n", src, strerror(errno));
        free(buf);
        return -1;
    }

    out = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out < 0) {
        printf("[wm_restorer] open dst failed %s: %s\n", dst, strerror(errno));
        close(in);
        free(buf);
        return -1;
    }

    while ((nread = read(in, buf, BUF_SIZE)) > 0) {
        char *out_ptr = buf;
        ssize_t to_write = nread;
        while (to_write > 0) {
            ssize_t nw = write(out, out_ptr, to_write);
            if (nw < 0) {
                printf("[wm_restorer] write failed %s: %s\n", dst, strerror(errno));
                close(in); close(out); free(buf);
                return -1;
            }
            to_write -= nw;
            out_ptr += nw;
        }
    }

    if (nread < 0) {
        printf("[wm_restorer] read failed %s: %s\n", src, strerror(errno));
    }

    close(in);
    close(out);

    /* Try to copy permissions */
    struct stat st;
    if (stat(src, &st) == 0) {
        chmod(dst, st.st_mode & 0777);
    }

    free(buf);
    return 0;
}

static int path_join(char *out, size_t out_sz, const char *a, const char *b)
{
    if (snprintf(out, out_sz, "%s/%s", a, b) >= (int)out_sz) return -1;
    return 0;
}

static int copy_dir_recursive_internal(const char *src, const char *dst, int overwrite)
{
    DIR *d = opendir(src);
    if (!d) {
        printf("[wm_restorer] opendir failed %s: %s\n", src, strerror(errno));
        return -1;
    }

    if (ensure_dir(dst) != 0) {
        printf("[wm_restorer] ensure_dir failed %s\n", dst);
        closedir(d);
        return -1;
    }

    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        char src_path[1024];
        char dst_path[1024];
        if (path_join(src_path, sizeof(src_path), src, ent->d_name) != 0) continue;
        if (path_join(dst_path, sizeof(dst_path), dst, ent->d_name) != 0) continue;

        struct stat st;
        if (stat(src_path, &st) != 0) {
            printf("[wm_restorer] stat failed %s: %s\n", src_path, strerror(errno));
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            copy_dir_recursive_internal(src_path, dst_path, overwrite);
        } else if (S_ISREG(st.st_mode)) {
            copy_file_internal(src_path, dst_path, overwrite);
        } else {
            printf("[wm_restorer] skipping non-regular file %s\n", src_path);
        }

        if (worker_stop) break;
    }

    closedir(d);
    return 0;
}

static void *copy_worker(void *arg)
{
    (void)arg;
    printf("[wm_restorer] copy_worker: started\n");

    const char *folders_to_copy[][2] = {
        {"/dev_flash/wm_lang", "/dev_hdd0/wm_lang"},
        {"/dev_flash/wm_res",  "/dev_hdd0/wm_res"},
        {"/dev_flash/wm",      "/dev_hdd0/wm"},
    };

    size_t i;
    for (i = 0; i < sizeof(folders_to_copy)/sizeof(folders_to_copy[0]); ++i) {
        const char *src = folders_to_copy[i][0];
        const char *dst = folders_to_copy[i][1];
        printf("[wm_restorer] copying %s -> %s\n", src, dst);
        copy_dir_recursive_internal(src, dst, 1);
        if (worker_stop) break;
    }

    /* Copy mygames.xml specifically */
    if (!worker_stop) {
        printf("[wm_restorer] copying /dev_flash/mygames.xml -> /dev_hdd0/mygames.xml\n");
        copy_file_internal("/dev_flash/mygames.xml", "/dev_hdd0/mygames.xml", 1);
    }

    printf("[wm_restorer] copy_worker: finished\n");
    worker_running = 0;
    return NULL;
}

int start_copy_worker(void)
{
    if (worker_running) return 0;
    worker_stop = 0;
    int rc = pthread_create(&worker_thread, NULL, copy_worker, NULL);
    if (rc != 0) {
        printf("[wm_restorer] pthread_create failed: %s\n", strerror(rc));
        return -1;
    }
    pthread_detach(worker_thread);
    worker_running = 1;
    return 0;
}

int stop_copy_worker(void)
{
    if (!worker_running) return 0;
    worker_stop = 1;
    /* give worker some time to stop (simple) */
    sleep(1);
    return 0;
}
