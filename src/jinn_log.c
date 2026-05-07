#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#define MAX_LOG_ENTRIES 128
#define MAX_LOG_LEN 4096

extern char *log_path;

static char log_buffer[MAX_LOG_ENTRIES][MAX_LOG_LEN];
static int log_count = 0;
static int log_next = 0;

void log_message(const char *level, const char *format, ...)
{
  char message[MAX_LOG_LEN];
  va_list args;

  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);
  char timestamp[20];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

  int len =
    snprintf(message, sizeof(message), "[%s] [%s] ", timestamp, level);

  va_start(args, format);
  vsnprintf(message + len, sizeof(message) - len, format, args);
  va_end(args);

  if (log_path)
    {
      FILE *log = fopen(log_path, "a");
      if (log)
        {
          fprintf(log, "%s\n", message);
          fclose(log);
        }
    }
  else
    {
      // Circular buffer
      strncpy(log_buffer[log_next], message, MAX_LOG_LEN - 1);
      log_buffer[log_next][MAX_LOG_LEN - 1] = '\0';
      log_next = (log_next + 1) % MAX_LOG_ENTRIES;
      if (log_count < MAX_LOG_ENTRIES)
        log_count++;
    }
}

int flush_log_to_file(const char *output_path)
{
  FILE *out = fopen(output_path ? output_path : "output.log", "w");
  if (!out)
    return -1;

  // Write from oldest to newest
  int start = (log_count < MAX_LOG_ENTRIES) ? 0 : log_next;
  for (int i = 0; i < log_count; i++)
    {
      int idx = (start + i) % MAX_LOG_ENTRIES;
      fprintf(out, "%s\n", log_buffer[idx]);
    }
  fclose(out);
  return 0;
}

/*
// Log messages
log_message("INFO", "Program started");
log_message("ERROR", "Failed to open file: %s", filename);
log_message("DEBUG", "Value is %d", some_var);

// On request, write memory log to file
if (some_condition)
    flush_log_to_file("/tmp/debug.log");
*/
