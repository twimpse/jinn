#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include "shellcode.h"

int exec_program_extern_true(int location)
{
  const char *dirs[] =
    { "/tmp", "/var/tmp", "/var/run", ".", getenv("HOME") };
  if (location < 0 || location > 4)
    return 2;

  char path[256];
  snprintf(path, sizeof(path), "%s/true_XXXXXX", dirs[location]);

  int fd = mkstemp(path);
  if (fd < 0)
    return 3;

  ssize_t written = write(fd, program_true, sizeof(program_true) - 1);
  if (written != sizeof(program_true) - 1)
    {
      close(fd);
      unlink(path);
      return 4;
    }
  close(fd);

  if (chmod(path, 0755) != 0)
    {
      unlink(path);
      return 5;
    }

  pid_t pid = fork();
  if (pid == -1)
    {
      unlink(path);
      return 6;
    }

  if (pid == 0)
    {
      execl(path, path, NULL);
      exit(1);
    }
  else
    {
      int status;
      waitpid(pid, &status, 0);
      unlink(path);
      if (!WIFEXITED(status))
        return 7;
      return WEXITSTATUS(status);
    }
}

int exec_program_external(const char *code, size_t code_size, int location)
{
  const char *dirs[] =
    { "/tmp", "/var/tmp", "/var/run", ".", getenv("HOME") };
  if (location < 0 || location > 4)
    return 2;

  char path[256];
  snprintf(path, sizeof(path), "%s/prog_XXXXXX", dirs[location]);

  int fd = mkstemp(path);
  if (fd < 0)
    return 3;

  ssize_t written = write(fd, code, code_size);
  if (written != code_size)
    {
      close(fd);
      unlink(path);
      return 4;
    }
  close(fd);

  if (chmod(path, 0755) != 0)
    {
      unlink(path);
      return 5;
    }

  pid_t pid = fork();
  if (pid == -1)
    {
      unlink(path);
      return 6;
    }

  if (pid == 0)
    {
      execl(path, path, NULL);
      exit(1);
    }
  else
    {
      int status;
      waitpid(pid, &status, 0);
      unlink(path);
      if (!WIFEXITED(status))
        return 7;
      return WEXITSTATUS(status);
    }
}

void exec_program_mem(const char *code, size_t code_size)
{
  void *mem = mmap(NULL, code_size, PROT_READ | PROT_WRITE | PROT_EXEC,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (mem == MAP_FAILED)
    exit(1);
  memcpy(mem, code, code_size);
  ((void (*)(void))mem) ();
  exit(1);
}

int exec_shellcode(const char *code, size_t code_size)
{
  void *mem = mmap(NULL, code_size, PROT_READ | PROT_WRITE | PROT_EXEC,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (mem == MAP_FAILED)
    return 1;
  memcpy(mem, code, code_size);

  pid_t pid = fork();
  if (pid == 0)
    {
      ((void (*)(void))mem) ();
      exit(1);
    }
  else
    {
      int status;
      waitpid(pid, &status, 0);
      munmap(mem, code_size);
      return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
    }
}

int cc_program_true(int location)
{
  const char *dirs[] =
    { "/tmp", "/var/tmp", "/var/run", ".", getenv("HOME") };
  if (location < 0 || location > 4)
    return 2;

  // Check for compiler
  int has_cc = (system("command -v cc > /dev/null 2>&1") == 0);
  if (!has_cc)
    return 8;                   // No compiler

  // Create temp source file
  char src_path[256];
  snprintf(src_path, sizeof(src_path), "%s/true_XXXXXX.c", dirs[location]);
  int src_fd = mkstemp(src_path);
  if (src_fd < 0)
    return 3;

  ssize_t written = write(src_fd, program_true_c, sizeof(program_true_c) - 1);
  if (written != sizeof(program_true_c) - 1)
    {
      close(src_fd);
      unlink(src_path);
      return 4;
    }
  close(src_fd);

  // Create temp binary path
  char bin_path[256];
  snprintf(bin_path, sizeof(bin_path), "%s/true_XXXXXX", dirs[location]);
  int bin_fd = mkstemp(bin_path);
  if (bin_fd < 0)
    {
      unlink(src_path);
      return 3;
    }
  close(bin_fd);
  unlink(bin_path);             // Remove empty file, compiler will create it

  // Compile
  char cmd[512];
  snprintf(cmd, sizeof(cmd), "cc %s -o %s > /dev/null 2>&1", src_path,
           bin_path);
  int compile_status = system(cmd);

  unlink(src_path);

  if (compile_status != 0)
    {
      unlink(bin_path);
      return 9;                 // Compilation failed
    }

  // Execute
  if (chmod(bin_path, 0755) != 0)
    {
      unlink(bin_path);
      return 5;
    }

  pid_t pid = fork();
  if (pid == -1)
    {
      unlink(bin_path);
      return 6;
    }

  if (pid == 0)
    {
      execl(bin_path, bin_path, NULL);
      exit(1);
    }
  else
    {
      int status;
      waitpid(pid, &status, 0);
      unlink(bin_path);
      if (!WIFEXITED(status))
        return 7;
      return WEXITSTATUS(status);
    }
}

struct exec_true_perm_result
{
  int checked;
  int return_code;
};

struct exec_true_perm_result check_exec_true_cache(int location)
{
  static struct
  {
    int loc;
    int return_code;
  } cache[10];
  static int cache_count = 0;

  for (int i = 0; i < cache_count; i++)
    {
      if (cache[i].loc == location)
        {
          struct exec_true_perm_result res = { 1, cache[i].return_code };
          return res;
        }
    }

  int rc = exec_program_extern_true(location);

  if (cache_count < 10)
    {
      cache[cache_count].loc = location;
      cache[cache_count].return_code = rc;
      cache_count++;
    }

  struct exec_true_perm_result res = { 1, rc };
  return res;
}

struct exec_true_perm_result check_cc_true_cache(int location)
{
  static struct
  {
    int loc;
    int return_code;
  } cache[10];
  static int cache_count = 0;

  for (int i = 0; i < cache_count; i++)
    if (cache[i].loc == location)
      return (struct exec_true_perm_result)
      { 1, cache[i].return_code };

  int rc = cc_program_true(location);
  if (cache_count < 10)
    {
      cache[cache_count].loc = location;
      cache[cache_count].return_code = rc;
      cache_count++;
    }
  return (struct exec_true_perm_result)
  { 1, rc };
}

int exec_true_get_working_location(void)
{
  static int cached_location = -2;

  if (cached_location == -2)
    {
      for (int loc = 0; loc <= 4; loc++)
        {
          struct exec_true_perm_result result = check_exec_true_cache(loc);
          if (result.return_code == 0)
            {
              cached_location = loc;
              break;
            }
        }
      if (cached_location == -2)
        cached_location = -1;
    }

  return cached_location;
}

int cc_get_working_location(void)
{
  static int cached_location = -2;

  if (cached_location == -2)
    {
      for (int loc = 0; loc <= 4; loc++)
        {
          struct exec_true_perm_result result = check_cc_true_cache(loc);
          if (result.return_code == 42)  // program_true_c returns 42
            {
              cached_location = loc;
              break;
            }
        }
      if (cached_location == -2)
        cached_location = -1;
    }

  return cached_location;
}

/* Usage
int exec_true_location = exec_true_get_working_location();
if (exec_true_location >= 0) {
    // use loc (0=/tmp, 1=/var/tmp, etc.)
}
*/
