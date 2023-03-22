#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <omp.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        return 1;
    }

    char *src_path = argv[1];
    char *dst_path = argv[2];

    int src_fd = open(src_path, O_RDONLY);
    if (src_fd == -1) {
        perror("open source");
        return 1;
    }

    struct stat src_stat;
    if (fstat(src_fd, &src_stat) == -1) {
        perror("stat source");
        return 1;
    }

    int dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, src_stat.st_mode);
    if (dst_fd == -1) {
        perror("open destination");
        return 1;
    }

    char buf[BUF_SIZE];
    ssize_t bytes_read;
    off_t offset = 0;

    #pragma omp parallel private(bytes_read, buf)
    {
        int tid = omp_get_thread_num();
        while ((bytes_read = pread(src_fd, buf, BUF_SIZE, offset)) > 0) {
            ssize_t bytes_written = pwrite(dst_fd, buf, bytes_read, offset);
            if (bytes_written != bytes_read) {
                fprintf(stderr, "Error: could not write all bytes in
thread %d\n", tid);
                break;
            }
            offset += bytes_read;
        }
    }

    close(src_fd);
    close(dst_fd);

    return 0;
}


