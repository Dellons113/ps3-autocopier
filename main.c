#include <psl1ght/module.h>

SYS_MODULE_INFO("wm_restorer", 0, 1, 1);

int wm_restorer_start(size_t args, void *argp)
{
    return SYS_PRX_RESIDENT;
}

int wm_restorer_stop(void)
{
    return SYS_PRX_STOP_OK;
}
