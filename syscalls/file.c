#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int resize(const char* filename, size_t bytes) {
    if (truncate(filename, (off_t)bytes) == -1) {
        fprintf(stderr, "Failed to truncate file: %s\n", strerror(errno));
        return 1;
    }
    return 0;
}

ssize_t read_from(const char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Failed to open file: %s\n", strerror(errno));
        return 1;
    }

    printf("Reading from '%s'\n", filename);

    const size_t READ_WINDOW_SIZE = 255;
    char buffer[READ_WINDOW_SIZE];
    ssize_t total_read_count = 0, read_count;

    do {
        read_count = read(fd, buffer, READ_WINDOW_SIZE);
        if (read_count == -1) {
            fprintf(stderr, "Failed to read file: %s\n", strerror(errno));
            total_read_count = -1;
            goto cleanup_read_from_fd;
        }
        total_read_count += read_count;
    } while (read_count == (ssize_t)READ_WINDOW_SIZE);

cleanup_read_from_fd:
    close(fd);
    printf("Closing file!\n");
    return total_read_count;
}

int main(int argsc, char** argsv) {
    size_t filesize = argsc == 1 
        ? 256ul 
        : (size_t)strtol(argsv[1], NULL, 10);

    printf("resizing to %zu bytes\n", filesize);

    if (resize("temp.txt", filesize) == -1) return 1;
    ssize_t read_bytes = read_from("temp.txt");
    printf("Read %zd bytes\n", read_bytes);
    return 0;
}
