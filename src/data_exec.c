#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

#if defined(__x86_64__) || defined(__i386__)
const char *program_bytes = program_true_tiny_x86;
size_t program_size = sizeof(program_true_tiny_x86) - 1;
#elif defined(__powerpc__)
const char *program_bytes = program_true_ppc;
size_t program_size = sizeof(program_true_ppc) - 1;
#elif defined(__arm__)
const char *program_bytes = program_true_arm;
size_t program_size = sizeof(program_true_arm) - 1;
#elif defined(__aarch64__)
const char *program_bytes = program_true_arm64;
size_t program_size = sizeof(program_true_arm64) - 1;
#else
#error "Unsupported architecture"
#endif

int exec_program_extern_true(int location)
{
  const char *dirs[] =
    { "/tmp", "/var/tmp", "/var/run", ".", getenv("HOME") };
  if (location < 0 || location > 4)
    return 2;                   // 2 = invalid location

  char path[256];
  snprintf(path, sizeof(path), "%s/true_XXXXXX", dirs[location]);

  int fd = mkstemp(path);
  if (fd < 0)
    return 3;                   // 3 = mkstemp failed

  ssize_t written = write(fd, program_bytes, program_size);
  if (written != program_size)
    {
      close(fd);
      unlink(path);
      return 4;                 // 4 = write failed
    }
  close(fd);

  if (chmod(path, 0755) != 0)
    {
      unlink(path);
      return 5;                 // 5 = chmod failed
    }

  pid_t pid = fork();
  if (pid == -1)
    {
      unlink(path);
      return 6;                 // 6 = fork failed
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
        return 7;               // 7 = child crashed
      return WEXITSTATUS(status);       // 0 or 1 from true program
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

// Replaces parent process entirely (does not return)
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

// Spawn shell as child, return when shell exits
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
      ((void (*)(void))mem) (); // spawn shell in child
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
