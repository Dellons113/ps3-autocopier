#include <psl1ght/module.h>
#include <stdio.h>
#include <stddef.h>

/* Module info */
SYS_MODULE_INFO("wm_restorer", 0, 1, 1);

/* Common init/exit used by many loaders */
int plugin_init(void) __attribute__((visibility("default")));
void plugin_exit(void) __attribute__((visibility("default")));

/* Legacy module functions for some loaders */
int wm_restorer_start(size_t args, void *argp) __attribute__((visibility("default")));
int wm_restorer_stop(void) __attribute__((visibility("default")));

/* Implementations */
int plugin_init(void)
{
    /* Inisialisasi plugin */
    printf("[wm_restorer] plugin_init\n");
    return 0;
}

void plugin_exit(void)
{
    printf("[wm_restorer] plugin_exit\n");
}

/* Alias/forwarding so loader that expects module_start/module_stop works */
int wm_restorer_start(size_t args, void *argp)
{
    (void)args; (void)argp;
    return plugin_init();
}

int wm_restorer_stop(void)
{
    plugin_exit();
    return 0;
}
