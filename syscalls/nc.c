#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

static inline uint64_t now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

static const uint64_t ONE_SECOND = 1000000000ull;

void start_server(uint16_t port) {
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0); 
    if (socket_fd == -1) handle_error("socket");

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(port);

    if (inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr) == -1)
        handle_error("invalid addr");

    if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        handle_error("bind");

    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    memset(&client_addr, 0, sizeof(client_addr));
    
    uint32_t counter_n, counter;
    uint64_t expected = 0;
    uint64_t dropped = 0, received = 0;

    uint64_t last_refresh = now_ns();

    for (;;) {
        ssize_t buffer_read = recvfrom(
            socket_fd,
            &counter_n, 
            sizeof(counter_n),
            0,
            (struct sockaddr*)&client_addr,
            &addrlen
        );

        if (buffer_read == -1) handle_error("read");
        counter = ntohl(counter_n);
        if (counter != expected) dropped += counter - expected;
        expected = counter + 1;
        ++received;

        uint64_t now = now_ns();
        if (now - last_refresh >= ONE_SECOND) {
            printf("recv %lu, dropped %lu\n", received, dropped);
            last_refresh = now;
            received = 0;
            dropped = 0;
        }     
    }

    close(socket_fd);
}

void start_client(char* addr, uint16_t port) {
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0); 
    if (socket_fd == -1) handle_error("socket");

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(port);

    if (inet_pton(AF_INET, addr, &server_addr.sin_addr) == -1)
        handle_error("invalid address");

    for (uint32_t i = 0;; ++i) {
        uint32_t counter = htonl(i);
        if (sendto(
            socket_fd, 
            &counter, 
            sizeof(counter), 
            0, 
            (struct sockaddr*)&server_addr,
            sizeof(server_addr)
        ) == -1)
            handle_error("send");
    }

    close(socket_fd);
}

int main(int argsc, char** argsv) {
    if (argsc == 1) {
        fprintf(stderr, "./nc [IP] [PORT]\n");
        return 1;
    } else if (argsc == 2) {
        start_server((uint16_t)strtol(argsv[1], NULL, 10));
    } else if (argsc == 3) {
        start_client(argsv[1], (uint16_t)strtol(argsv[2], NULL, 10));
    }

    return 0;
}

