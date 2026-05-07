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


struct exec_true_perm_result {
    int checked;
    int return_code;
};

struct exec_true_perm_result check_exec_true_cache(int location) {
    static struct {
        int loc;
        int return_code;
    } cache[10];
    static int cache_count = 0;
    
    for (int i = 0; i < cache_count; i++) {
        if (cache[i].loc == location) {
            struct exec_true_perm_result res = {1, cache[i].return_code};
            return res;
        }
    }
    
    int rc = exec_program_extern_true(location);
    
    if (cache_count < 10) {
        cache[cache_count].loc = location;
        cache[cache_count].return_code = rc;
        cache_count++;
    }
    
    struct exec_true_perm_result res = {1, rc};
    return res;
}

struct exec_true_perm_result result;
int selected_location = -1;
int final_return_code = -1;

for (int loc = 0; loc <= 4; loc++) {
    result = check_exec_true_cache(loc);
    
    if (result.return_code == 0) {
        selected_location = loc;
        final_return_code = 0;
        break;  // found working location
    }
    else if (result.return_code == 1) {
        // Program executed but returned false (not writable/executable)
        continue;
    }
    else {
        // Error codes 2-7: mkstemp failed, write failed, chmod failed, fork failed, etc.
        final_return_code = result.return_code;
        // Continue checking other locations
    }
}

if (selected_location != -1) {
    // Found working location with return_code == 0
    // Use selected_location
}
else if (final_return_code > 1) {
    // All locations had errors, final_return_code holds last error
}
// else: All locations returned 1 (not writable/executable)

int exec_true_get_working_location(void) {
    static int cached_location = -2;  // -2 = not checked yet
    
    if (cached_location == -2) {
        for (int loc = 0; loc <= 4; loc++) {
            struct exec_true_perm_result result = check_exec_true_cache(loc);
            if (result.return_code == 0) {
                cached_location = loc;
                break;
            }
        }
        if (cached_location == -2) cached_location = -1;  // none found
    }
    
    return cached_location;
}

/* Usage
int exec_true_location = exec_true_get_working_location();
if (exec_true_location >= 0) {
    // use loc (0=/tmp, 1=/var/tmp, etc.)
}
*/