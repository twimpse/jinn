#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 4096

int forward_tcp(const char *listen_host, int listen_port,
                const char *target_host, int target_port) {
    int listen_fd, target_fd;
    struct sockaddr_in listen_addr, target_addr;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Create listening socket
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket creation failed");
        return -1;
    }

    // Setup listening address
    memset(&listen_addr, 0, sizeof(listen_addr));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(listen_port);

    if (inet_pton(AF_INET, listen_host, &listen_addr.sin_addr) <= 0) {
        perror("invalid listen address");
        close(listen_fd);
        return -1;
    }

    // Bind and listen
    if (bind(listen_fd, (struct sockaddr*)&listen_addr, sizeof(listen_addr)) < 0) {
        perror("bind failed");
        close(listen_fd);
        return -1;
    }

    if (listen(listen_fd, 5) < 0) {
        perror("listen failed");
        close(listen_fd);
        return -1;
    }

    // Accept one connection
    socklen_t addr_len = sizeof(listen_addr);
    int client_fd = accept(listen_fd, (struct sockaddr*)&listen_addr, &addr_len);
    if (client_fd < 0) {
        perror("accept failed");
        close(listen_fd);
        return -1;
    }

    // Create target socket
    target_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (target_fd < 0) {
        perror("target socket creation failed");
        close(client_fd);
        close(listen_fd);
        return -1;
    }

    // Setup target address
    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(target_port);

    if (inet_pton(AF_INET, target_host, &target_addr.sin_addr) <= 0) {
        perror("invalid target address");
        close(client_fd);
        close(target_fd);
        close(listen_fd);
        return -1;
    }

    // Connect to target
    if (connect(target_fd, (struct sockaddr*)&target_addr, sizeof(target_addr)) < 0) {
        perror("connect to target failed");
        close(client_fd);
        close(target_fd);
        close(listen_fd);
        return -1;
    }

    // Forward data from client to target
    while ((bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0)) > 0) {
        if (send(target_fd, buffer, bytes_read, 0) < 0) {
            perror("send to target failed");
            break;
        }
    }

    // Cleanup
    close(client_fd);
    close(target_fd);
    close(listen_fd);

    return 0;
}
