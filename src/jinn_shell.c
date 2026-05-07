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

  if (strcmp(input, "exit") == 0)
    {
      if (use_socket && socket_fd > 0)
        fclose(out);
      return -1;
    }
  else if (strncmp(input, "ls", 2) == 0)
    {
      char *arg = input + 2;
      while (*arg == ' ')
        arg++;
      if (*arg == '\0')
        handle_list(out);
      else
        handle_read(arg, out);
    }
  else if (strncmp(input, "remote ", 7) == 0)
    {
      char host[256];
      int port;
      if (sscanf(input + 7, "%s %d", host, &port) == 2)
        remote_shell_mode(host, port);
      else
        fprintf(out, "Usage: remote host port\n");
    }
  else if (strcmp(input, "genie") == 0)
    {
      genie_mode();
    }
  else if (strcmp(input, "shell") == 0)
    {
      SHELL_MODE();
    }
  else if (strcmp(input, "datapipe") == 0)
    {
      pid_t pid = fork();
      if (pid == 0)
        {
          datapipe_mode(NULL, 0, NULL, 0);
          exit(0);
        }
    }
  else if (strncmp(input, "exec ", 5) == 0)
    {
      pid_t pid = fork();
      if (pid == 0)
        {
          execlp("/bin/sh", "sh", "-c", input + 5, NULL);
          exit(0);
        }
    }
  else if (strncmp(input "log", 4) == 0)
    {
      char *cmd = input + 3;
      while (*cmd == ' ')
        cmd++;

      if (strncmp(cmd, "read", 4) == 0)
        {
          char *num_str = cmd + 4;
          while (*num_str == ' ')
            num_str++;
          int num = atoi(num_str);
          if (num <= 0)
            num = 10;

          int start = (log_count < MAX_LOG_ENTRIES) ? 0 : log_next;
          int count = 0;
          for (int i = log_count - 1; i >= 0 && count < num; i--, count++)
            {
              int idx = (start + i) % MAX_LOG_ENTRIES;
              fprintf(out, "%s\n", log_buffer[idx]);
            }
        }
      else if (strncmp(cmd, "delete", 6) == 0)
        {
          log_count = 0;
          log_next = 0;
          memset(log_buffer, 0, sizeof(log_buffer));
          fprintf(out, "Log buffer cleared\n");
        }
      else if (strncmp(cmd, "write", 5) == 0)
        {
          char *filename = cmd + 5;
          while (*filename == ' ')
            filename++;
          if (*filename == '\0')
            fprintf(out, "Usage: log write <filename>\n");
          else if (flush_log_to_file(filename) == 0)
            fprintf(out, "Log written to %s\n", filename);
          else
            fprintf(out, "Failed to write log to %s\n", filename);
        }
      else
        {
          fprintf(out,
                  "Usage: log read [num], log delete, log write <filename>\n");
        }
    }
  else
    {
      fprintf(out, "Unknown command\n");
    }

  return 0;
}
