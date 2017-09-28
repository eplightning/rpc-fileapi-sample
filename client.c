#include "nfs.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define EPL_READ_WRITE_BUFFER 1024

CLIENT *g_client;

int epl_open(const char *filename, int flags, int mode)
{
    open_params *params = malloc(sizeof(open_params));
    params->file = (char*) filename;
    params->flags = flags;
    params->mode = mode;
    
    int *res = epl_open_1(params, g_client);
    
    free(params);
    
    if (res == NULL) {
        errno = ETIMEDOUT;
        return -1;
    }

    if (*res < 0) {
        errno = (*res) * (-1);
        return -1;
    }
    
    return *res;
}

int epl_creat(const char *filename, int mode)
{
    creat_params *params = malloc(sizeof(creat_params));
    params->file = (char*) filename;
    params->mode = mode;

    int *res = epl_creat_1(params, g_client);
    
    free(params);
    
    if (res == NULL) {
        errno = ETIMEDOUT;
        return -1;
    }

    if (*res < 0) {
        errno = (*res) * (-1);
        return -1;
    }
    
    return *res;
}

off_t epl_lseek(int fd, off_t offset, int whence)
{
    lseek_params *params = malloc(sizeof(lseek_params));
    params->fd = fd;
    params->offset = (int) offset;
    params->whence = whence;

    int *res = epl_lseek_1(params, g_client);
    
    free(params);
    
    if (res == NULL) {
        errno = ETIMEDOUT;
        return -1;
    }

    if (*res < 0) {
        errno = (*res) * (-1);
        return -1;
    }
    
    return (off_t) *res;
}

int epl_close(int fd)
{
    int *res = epl_close_1(&fd, g_client);
    
    if (res == NULL) {
        errno = ETIMEDOUT;
        return -1;
    }

    if (*res < 0) {
        errno = (*res) * (-1);
        return -1;
    }
    
    return 0;
}

ssize_t epl_read2(int fd, void *buf, size_t count)
{
    read_params *params = malloc(sizeof(read_params));
    params->fd = fd;
    params->max = (int) count;

    read_retval *res = epl_read_1(params, g_client);
    
    free(params);
    
    if (res == NULL) {
        errno = ETIMEDOUT;
        return -1;
    }

    if (res->status < 0) {
        errno = res->status * (-1);
        return -1;
    } else if (res->status > 0 && res->bytes.bytes_len > 0) {
        memcpy(buf, res->bytes.bytes_val, res->bytes.bytes_len);
    }
    
    ssize_t read_bytes = (ssize_t) res->bytes.bytes_len;
    
    return read_bytes;
}

ssize_t epl_read(int fd, void *buf, size_t count)
{
    size_t index = 0;
    
    while (index < count) {
        size_t count2 = EPL_READ_WRITE_BUFFER;
        
        if (index + count2 > count) {
            count2 = count - index;
        }
        
        ssize_t result = epl_read2(fd, buf + index, count2);
        
        if (result == -1) {
            return -1;
        } else if (result < count2) {
            return index + result;
        }
        
        index = index + count2;
    }
    
    return index;
}

ssize_t epl_write2(int fd, char *buf, size_t count)
{
    write_params *params = malloc(sizeof(write_params));
    params->fd = fd;
    params->bytes.bytes_len = (u_int) count;
    params->bytes.bytes_val = buf;
    
    int *res = epl_write_1(params, g_client);

    free(params);
    
    if (res == NULL) {
        errno = ETIMEDOUT;
        return -1;
    }

    if (*res < 0) {
        errno = (*res) * (-1);
        return -1;
    }

    return (ssize_t) *res;
}

ssize_t epl_write(int fd, const void *buf, size_t count)
{
    char *data = malloc(count);
    memcpy(data, buf, count);
    
    size_t index = 0;
    
    while (count > 0) {
        size_t count2 = EPL_READ_WRITE_BUFFER;
        
        if (count2 > count) {
            count2 = count;
        }
        
        count -= count2;
        
        ssize_t result = epl_write2(fd, data + index, count2);
        
        if (result == -1) {
            free(data);
            return -1;
        } else if (result < count2) {
            free(data);
            return index + result;
        }
        
        index = index + count2;
    }
    
    free(data);
    return index;
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("wywołanie: client host plik\n");
        return 1;
    }
    
    g_client = clnt_create(argv[1], EPNFS, EPNFS_V1, "udp");
    if (g_client == NULL) {
        printf("error: could not connect to server.\n");
        return 1;
    }
    
    char *write_data = malloc(10 * 8096);
    char *read_data = malloc(10 * 8096);

    for (int i = 0; i < 10 * 8096; i++) {
        write_data[i] = 48 + (i % 43);
    }

    char *filename = argv[2];
    
    int fd = epl_open(filename, O_TRUNC | O_CREAT | O_RDWR, 0777);
    printf("Fd: %d\n", fd);
    
    ssize_t written = epl_write(fd, write_data, 10 * 8096);
    printf("Written: %ld\n", written);
    
    off_t offset = epl_lseek(fd, 0, SEEK_SET);
    printf("Seeked: %ld\n", offset);
    
    ssize_t read_bytes = epl_read(fd, read_data, 3 * 8096);
    printf("Read bytes: %ld\n", read_bytes);

    ssize_t read_bytes2 = epl_read(fd, read_data, 10 * 8096);
    printf("Read bytes: %ld\n", read_bytes2);
    
    int closed = epl_close(fd);
    printf("Close retval: %d\n", closed);
    
    return 0;
}
