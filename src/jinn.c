#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include "jinn.h"


int main(int argc, char *argv[])
{
  int opt;
  int genie_flag = 0, datapipe_flag = 0, shell_flag = 0, command_flag = 0;
  int remote_flag = 0, daemon_flag = 0, log_flag = 0, server_flag = 0, client_flag = 0;
  char *log_path = NULL;
  char *host = NULL;
  int port = 0;
  char *program_name;

  // Initialize random
  srand(time(NULL));
  jinn_noop();

  // Check program name for implicit mode
  program_name = strrchr(argv[0], '/');
  program_name = (program_name) ? program_name + 1 : argv[0];

  if (strcmp(program_name, "genie") == 0)
    {
      genie_flag = 1;
    }
  else if (strcmp(program_name, "datapipe") == 0)
    {
      datapipe_flag = 1;
    }
  else if (strcmp(program_name, "shell") == 0)
    {
      shell_flag = 1;
    }
  else if (strcmp(program_name, "command") == 0)
    {
      command_flag = 1;
    }
  else if (strncmp(program_name, "remote", 6) == 0)
    {
      remote_flag = 1;

      // Parse format: remote_A_B_C_D_Y
      int a, b, c, d, y;

      if (sscanf(program_name, "remote_%d_%d_%d_%d_%d", &a, &b, &c, &d, &y) ==
          5)
        {
          char ip_address[16];
          snprintf(ip_address, sizeof(ip_address), "%d.%d.%d.%d", a, b, c, d);
          host = strdup(ip_address);
          port = y;
        }
      else
        {
          fprintf(stderr,
                  "Error: Invalid remote program name format. Expected: remote_IP_IP_IP_IP_PORT\n");
          exit(1);
        }
    }

  static struct option long_options[] = {
    {"daemon", no_argument, 0, 'd'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
  };

  while ((opt =
          getopt_long(argc, argv, "GDSCH:rp:Ld", long_options, NULL)) != -1)
    {
      switch (opt)
        {
        case 'G':
          genie_flag = 1;
          break;
        case 'D':
          datapipe_flag = 1;
          break;
        case 'S':
          shell_flag = 1;
          break;
        case 's':
          server_flag = 1;
          break;
        case 'c':
          client_flag = 1;
          break;
        case 'C':
          command_flag = 1;
          break;
        case 'H':
          host = optarg;
          break;
        case 'r':
          remote_flag = 1;
          break;
        case 'p':
          port = atoi(optarg);
          break;
        case 'L':
          log_flag = 1;
          log_path = (optarg) ? optarg : ".";
          break;
        case 'd':
          daemon_flag = 1;
          break;
        case 'h':
          printf
            ("Usage: %s [-s] [-c] [-G] [-D] [-S] [-C] [-r] [-H host] [-p port] [-L logpath] [-d|--daemon]\n",
             argv[0]);
          exit(0);
        default:
          fprintf(stderr, "Unknown option. Use -h for usage.\n");
          exit(1);
        }
    }

  // Validate mutually exclusive modes
  int mode_count =
    genie_flag + datapipe_flag + shell_flag + command_flag + remote_flag;
  if (mode_count != 1)
    {
      fprintf(stderr,
              "Error: Must specify exactly one of -G, -D, -S, -C, or -r\n");
      exit(1);
    }

  // Validate remote-specific arguments
  if (remote_flag && (!host || port <= 0))
    {
      fprintf(stderr, "Error: -r requires -H and -p arguments\n");
      exit(1);
    }

  // Initialize logging if requested
  if (log_flag)
    {
      init_logging(log_path);
    }

  // Daemonize if requested
  if (daemon_flag)
    {
      daemonize();
    }

  // Execute requested mode
  if (genie_flag)
    {
      genie_mode();
    }
  else if (datapipe_flag)
    {
      datapipe_mode();
    }
  else if (shell_flag)
    {
      shell_mode();
    }
  else if (command_flag)
    {
      command_mode();
    }
  else if (remote_flag)
    {
      remote_shell_mode(host, port);
    }

  return 0;
}
