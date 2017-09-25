#include "nfs.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int g_fd;
read_retval g_readret;

int *epl_open_1_svc(open_params *params, struct svc_req *svc)
{
    int result = open(params->file, params->flags, params->mode);
    
    if (result == -1) {
        g_fd = errno * (-1);
    } else {
        g_fd = result;
    }

    return &g_fd;
}

int *epl_creat_1_svc(creat_params *params, struct svc_req *svc)
{
    int result = creat(params->file, params->mode);
    
    if (result == -1) {
        g_fd = errno * (-1);
    } else {
        g_fd = result;
    }

    return &g_fd;
}

int *epl_lseek_1_svc(lseek_params *params, struct svc_req *svc)
{
    off_t result = lseek(params->fd, params->offset, params->whence);
    
    if (result == -1) {
        g_fd = errno * (-1);
    } else {
        g_fd = result;
    }

    return &g_fd;
}

int *epl_close_1_svc(int *params, struct svc_req *svc)
{
    int result = close(*params);
    
    if (result == -1) {
        g_fd = errno * (-1);
    } else {
        g_fd = result;
    }
    
    return &g_fd;
}

int *epl_write_1_svc(write_params *params, struct svc_req *svc)
{
    int result = write(params->fd, params->bytes.bytes_val, params->bytes.bytes_len);

    if (result == -1) {
        g_fd = errno * (-1);
    } else {
        g_fd = result;
    }
    
    return &g_fd;
}

read_retval *epl_read_1_svc(read_params *params, struct svc_req *svc)
{
    if (g_readret.bytes.bytes_val != NULL) {
        free(g_readret.bytes.bytes_val);
    }

    char *bytes = malloc(params->max);
    int result = read(params->fd, bytes, params->max);

    g_readret.bytes.bytes_val = bytes;
    
    if (result == -1) {
        g_readret.status = errno * (-1);
        g_readret.bytes.bytes_len = 0;
    } else {
        g_readret.status = result;
        g_readret.bytes.bytes_len = result;
    }

    return &g_readret;
}
