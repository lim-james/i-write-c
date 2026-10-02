#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

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

    const size_t BUFFER_SIZE = 255;
    char buffer[BUFFER_SIZE];

    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    memset(&client_addr, 0, sizeof(client_addr));
    
    for (;;) {
        ssize_t buffer_read = recvfrom(
            socket_fd,
            buffer, 
            BUFFER_SIZE - 1,
            0,
            (struct sockaddr*)&client_addr,
            &addrlen
        );

        if (buffer_read == -1) handle_error("read");
        printf(
            "Recieve from [%s:%d]: %*s\n",
            inet_ntoa(client_addr.sin_addr),
            ntohs(client_addr.sin_port),
            (int)buffer_read, 
            buffer
        );
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

    const char msg[] = "hello";
    if (sendto(
        socket_fd, 
        msg, 
        sizeof(msg), 
        0, 
        (struct sockaddr*)&server_addr,
        sizeof(server_addr)
    ) == -1)
        handle_error("send");

    close(socket_fd);
}

int main(int argsc, char** argsv) {
    if (argsc == 1) {
        fprintf(stderr, "./nc [IP] [PORT]");
        return 1;
    } else if (argsc == 2) {
        start_server((uint16_t)strtol(argsv[1], NULL, 10));
    } else if (argsc == 3) {
        start_client(argsv[1], (uint16_t)strtol(argsv[2], NULL, 10));
    }

    return 0;
}

