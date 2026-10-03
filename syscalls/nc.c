#define _GNU_SOURCE 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define MAX_MESSAGES 256

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

    uint32_t counter_n[MAX_MESSAGES];
    uint64_t expected = 0;
    uint64_t dropped = 0, received = 0;

    uint64_t last_refresh = now_ns();

    struct iovec msg_iovecs[MAX_MESSAGES];
    struct mmsghdr mmsg_headers[MAX_MESSAGES];

    for (unsigned int i = 0; i < MAX_MESSAGES; ++i) {
        msg_iovecs[i] = (struct iovec){
            .iov_base = counter_n + i,
            .iov_len  = sizeof *counter_n,
        };

        struct msghdr msg_header = {
            .msg_name    = &client_addr,
            .msg_namelen = addrlen,
            .msg_iov     = msg_iovecs + i,
            .msg_iovlen  = 1,
        };

        mmsg_headers[i] = (struct mmsghdr){.msg_hdr = msg_header};
    }

    for (;;) {
        int messages_recv  = recvmmsg(
            /* sockfd  = */ socket_fd,
            /* msgvec  = */ mmsg_headers,
            /* vlen    = */ MAX_MESSAGES,
            /* flags   = */ 0,
            /* timeout = */ NULL
        );

        if (messages_recv == -1) handle_error("read");

        for (int i = 0; i < messages_recv; ++i) {
            uint32_t counter = ntohl(counter_n[i]);
            if (counter > expected) dropped += counter - expected;
            expected = counter + 1;
            ++received;
        }

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

    uint32_t       counters[MAX_MESSAGES];
    struct iovec   vectors[MAX_MESSAGES];
    struct mmsghdr messages[MAX_MESSAGES];

    for (size_t i = 0; i < MAX_MESSAGES; ++i) {
        vectors[i] = (struct iovec) {
            .iov_base = counters + i,
            .iov_len  = sizeof *counters,
        };

        messages[i] = (struct mmsghdr){
            .msg_hdr = (struct msghdr){
                .msg_name    = &server_addr,
                .msg_namelen = sizeof server_addr,
                .msg_iov     = vectors + i,
                .msg_iovlen  = 1
            },
        };
    }

    uint32_t seq = 0;

    for (;;) {
        for (uint32_t j = 0; j < MAX_MESSAGES; ++j)
            counters[j]  = htonl(seq + j);

        int sent = sendmmsg(socket_fd, messages, MAX_MESSAGES, 0);
        if (sent == -1) handle_error("send");
        seq += (uint32_t)sent;
    }

    close(socket_fd);
}

int main(int argsc, char** argsv) {
    const struct option options[] = {
        { .name = "server", .has_arg = no_argument, .flag = NULL, .val = 's' }, 
        { .name = "addr", .has_arg = required_argument, .flag = NULL, .val = 'a' }, 
        { .name = "port", .has_arg = required_argument, .flag = NULL, .val = 'p' }, 
        { 0 },
    };
     
    int opt;

    bool is_server = false;
    uint16_t port;
    char* addr;

    while ((opt = getopt_long(argsc, argsv, "sca:p:", options, NULL)) != -1) {
        switch (opt) {
            case 's': is_server = true; break;
            case 'a': addr = optarg; break;
            case 'p': port = (uint16_t)strtol(optarg, NULL, 10); break;
            default: fprintf(stderr, "./nc --addr [IP] --port [PORT]\n"); return 1;
        }
    }

    if (is_server) {
        start_server(port);
    } else {
        start_client(addr, port);
    }

    return 0;
}

