#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>

#ifdef __x86_64__
#include <x86intrin.h>
#endif

void jinn_noop(){
#ifdef __x86_64__
    switch(rand() % 10) {
        case 0: __asm__ volatile("nop"); break;
        case 1: __asm__ volatile("xchg %ax, %ax"); break;
        case 2: __asm__ volatile("mov %rax, %rax"); break;
        case 3: __asm__ volatile("mov %rbx, %rbx"); break;
        case 4: __asm__ volatile("mov %rcx, %rcx"); break;
        case 5: __asm__ volatile("push %rax; pop %rax"); break;
        case 6: __asm__ volatile("add $0, %rsp"); break;
        case 7: __asm__ volatile("sub $0, %rsp"); break;
        case 8: __asm__ volatile("nopl 0x0(%rax)"); break;
        case 9: __asm__ volatile("lea 0x0(%rsi), %rsi"); break;
    }

#elif defined(__aarch64__)
    switch(rand() % 8) {
        case 0: __asm__ volatile("nop"); break;
        case 1: __asm__ volatile("mov x0, x0"); break;
        case 2: __asm__ volatile("mov x1, x1"); break;
        case 3: __asm__ volatile("mov x2, x2"); break;
        case 4: __asm__ volatile("add x0, x0, #0"); break;
        case 5: __asm__ volatile("sub x0, x0, #0"); break;
        case 6: __asm__ volatile("and x0, x0, x0"); break;
        case 7: __asm__ volatile("orr x0, x0, x0"); break;
    }
#endif

  return;
}

void jinn_junk_inst(){

}

// Method 1 - Simple loop
void jinn_delay_v1(int iterations) {
    for (volatile int i = 0; i < iterations; i++) {
        // do nothing
    }
}

// Method 2 - rdtsc timing loop
void jinn_delay_v2(int target_cycles) {
    unsigned long long start, current;
    start = __rdtsc();
    do {
        current = __rdtsc();
    } while ((current - start) < target_cycles);
}

// Method 3 - syscall based (nanosleep)
void jinn_delay_v3(int milliseconds) {
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = milliseconds * 1000000;
    nanosleep(&ts, NULL);
}

// Method 4 - pause instruction loop
void jinn_delay_v4(int iterations) {
    for (int i = 0; i < iterations; i++) {
        __asm__ volatile("pause");
    }
}

// Method 5 - busy wait with random operations
void jinn_delay_v5(int iterations) {
    volatile int x = 0;
    for (int i = 0; i < iterations; i++) {
        x += i;
        x -= i;
    }
}

void jinn_rand_delay(int duration) {
    static int last_method = 0;
    int method = rand() % 5;

    // Avoid same method twice in a row
    while (method == last_method) {
        method = rand() % 5;
    }
    last_method = method;

    switch(method) {
        case 0: jinn_delay_v1(duration); break;
        case 1: jinn_delay_v2(duration * 100000); break;  // rough conversion
        case 2: jinn_delay_v3(duration); break;
        case 3: jinn_delay_v4(duration); break;
        case 4: jinn_delay_v5(duration); break;
    }
}

void jinn_rand_nops(int max_nops) {
    int r = rand() % 100;

    if (r < 50) {
        // 50% - no nops
        return;
    } else if (r < 85) {
        // 35% - single nop
        jinn_noop();
    } else {
        // 15% - multiple nops
        int count = (rand() % max_nops) + 1;
        for (int i = 0; i < count; i++) {
            jinn_noop();
        }
    }
}

// 1. mprotect
/*int jinn_mprotect(void *addr, size_t len, int prot) {
    long ret;
#ifdef __x86_64__
    __asm__ volatile("mov %0, %%rdi" : : "r"(addr));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rsi" : : "r"(len));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rdx" : : "r"(prot));
    jinn_rand_nops(2);
    __asm__ volatile("mov $10, %%rax");
    jinn_rand_nops(2);
    __asm__ volatile("syscall");
    jinn_rand_nops(2);
    __asm__ volatile("mov %%rax, %0" : "=r"(ret));
#elif defined(__aarch64__)
    __asm__ volatile("mov %0, x0" : : "r"(addr));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x1" : : "r"(len));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x2" : : "r"(prot));
    jinn_rand_nops(2);
    __asm__ volatile("mov x8, #227");
    jinn_rand_nops(2);
    __asm__ volatile("svc #0");
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x0" : "=r"(ret));
#endif
    return (int)ret;
}*/

int jinn_mprotect(void *addr, size_t len, int prot) {
    long ret;
#ifdef __x86_64__
    __asm__ volatile (
        "mov %1, %%rdi\n"
        "mov %2, %%rsi\n"
        "mov %3, %%rdx\n"
        "mov $10, %%rax\n"
        "syscall\n"
        "mov %%rax, %0\n"
        : "=r"(ret)
        : "r"(addr), "r"(len), "r"(prot)
        : "%rax", "%rdi", "%rsi", "%rdx", "memory"
    );
#elif defined(__aarch64__)
    __asm__ volatile (
        "mov x0, %1\n"
        "mov x1, %2\n"
        "mov x2, %3\n"
        "mov x8, #227\n"
        "svc #0\n"
        "mov %0, x0\n"
        : "=r"(ret)
        : "r"(addr), "r"(len), "r"(prot)
        : "x0", "x1", "x2", "x8", "memory"
    );
#endif
    return (int)ret;
}

// 2. execve
int jinn_execve(const char *path, char *const argv[], char *const envp[]) {
    long ret;
#ifdef __x86_64__
    __asm__ volatile("mov %0, %%rdi" : : "r"(path));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rsi" : : "r"(argv));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rdx" : : "r"(envp));
    jinn_rand_nops(2);
    __asm__ volatile("mov $59, %%rax");
    jinn_rand_nops(2);
    __asm__ volatile("syscall");
    jinn_rand_nops(2);
    __asm__ volatile("mov %%rax, %0" : "=r"(ret));
#elif defined(__aarch64__)
    __asm__ volatile("mov %0, x0" : : "r"(path));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x1" : : "r"(argv));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x2" : : "r"(envp));
    jinn_rand_nops(2);
    __asm__ volatile("mov x8, #221");
    jinn_rand_nops(2);
    __asm__ volatile("svc #0");
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x0" : "=r"(ret));
#endif
    return (int)ret;
}

// 3. mmap
void *jinn_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    long ret;
#ifdef __x86_64__
    __asm__ volatile("mov %0, %%rdi" : : "r"(addr));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rsi" : : "r"(length));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rdx" : : "r"(prot));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%r10" : : "r"(flags));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%r8" : : "r"(fd));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%r9" : : "r"(offset));
    jinn_rand_nops(2);
    __asm__ volatile("mov $9, %%rax");
    jinn_rand_nops(2);
    __asm__ volatile("syscall");
    jinn_rand_nops(2);
    __asm__ volatile("mov %%rax, %0" : "=r"(ret));
#elif defined(__aarch64__)
    __asm__ volatile("mov %0, x0" : : "r"(addr));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x1" : : "r"(length));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x2" : : "r"(prot));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x3" : : "r"(flags));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x4" : : "r"(fd));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x5" : : "r"(offset));
    jinn_rand_nops(2);
    __asm__ volatile("mov x8, #222");
    jinn_rand_nops(2);
    __asm__ volatile("svc #0");
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x0" : "=r"(ret));
#endif
    return (void *)ret;
}

// 4. fork
int jinn_fork(void) {
    long ret;
#ifdef __x86_64__
    __asm__ volatile("mov $57, %%rax");
    jinn_rand_nops(2);
    __asm__ volatile("syscall");
    jinn_rand_nops(2);
    __asm__ volatile("mov %%rax, %0" : "=r"(ret));
#elif defined(__aarch64__)
    __asm__ volatile("mov x8, #220");
    jinn_rand_nops(2);
    __asm__ volatile("svc #0");
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x0" : "=r"(ret));
#endif
    return (int)ret;
}

// 5. ptrace
int jinn_ptrace(int request, pid_t pid, void *addr, void *data) {
    long ret;
#ifdef __x86_64__
    __asm__ volatile("mov %0, %%rdi" : : "r"(request));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rsi" : : "r"(pid));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rdx" : : "r"(addr));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%r10" : : "r"(data));
    jinn_rand_nops(2);
    __asm__ volatile("mov $101, %%rax");
    jinn_rand_nops(2);
    __asm__ volatile("syscall");
    jinn_rand_nops(2);
    __asm__ volatile("mov %%rax, %0" : "=r"(ret));
#elif defined(__aarch64__)
    __asm__ volatile("mov %0, x0" : : "r"(request));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x1" : : "r"(pid));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x2" : : "r"(addr));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x3" : : "r"(data));
    jinn_rand_nops(2);
    __asm__ volatile("mov x8, #117");
    jinn_rand_nops(2);
    __asm__ volatile("svc #0");
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x0" : "=r"(ret));
#endif
    return (int)ret;
}

// 6. socket
int jinn_socket(int domain, int type, int protocol) {
    long ret;
#ifdef __x86_64__
    __asm__ volatile("mov %0, %%rdi" : : "r"(domain));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rsi" : : "r"(type));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rdx" : : "r"(protocol));
    jinn_rand_nops(2);
    __asm__ volatile("mov $41, %%rax");
    jinn_rand_nops(2);
    __asm__ volatile("syscall");
    jinn_rand_nops(2);
    __asm__ volatile("mov %%rax, %0" : "=r"(ret));
#elif defined(__aarch64__)
    __asm__ volatile("mov %0, x0" : : "r"(domain));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x1" : : "r"(type));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x2" : : "r"(protocol));
    jinn_rand_nops(2);
    __asm__ volatile("mov x8, #198");
    jinn_rand_nops(2);
    __asm__ volatile("svc #0");
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x0" : "=r"(ret));
#endif
    return (int)ret;
}

// 7. bind
int jinn_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    long ret;
#ifdef __x86_64__
    __asm__ volatile("mov %0, %%rdi" : : "r"(sockfd));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rsi" : : "r"(addr));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rdx" : : "r"(addrlen));
    jinn_rand_nops(2);
    __asm__ volatile("mov $49, %%rax");
    jinn_rand_nops(2);
    __asm__ volatile("syscall");
    jinn_rand_nops(2);
    __asm__ volatile("mov %%rax, %0" : "=r"(ret));
#elif defined(__aarch64__)
    __asm__ volatile("mov %0, x0" : : "r"(sockfd));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x1" : : "r"(addr));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x2" : : "r"(addrlen));
    jinn_rand_nops(2);
    __asm__ volatile("mov x8, #202");
    jinn_rand_nops(2);
    __asm__ volatile("svc #0");
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x0" : "=r"(ret));
#endif
    return (int)ret;
}

// 8. connect
int jinn_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    long ret;
#ifdef __x86_64__
    __asm__ volatile("mov %0, %%rdi" : : "r"(sockfd));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rsi" : : "r"(addr));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rdx" : : "r"(addrlen));
    jinn_rand_nops(2);
    __asm__ volatile("mov $42, %%rax");
    jinn_rand_nops(2);
    __asm__ volatile("syscall");
    jinn_rand_nops(2);
    __asm__ volatile("mov %%rax, %0" : "=r"(ret));
#elif defined(__aarch64__)
    __asm__ volatile("mov %0, x0" : : "r"(sockfd));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x1" : : "r"(addr));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x2" : : "r"(addrlen));
    jinn_rand_nops(2);
    __asm__ volatile("mov x8, #203");
    jinn_rand_nops(2);
    __asm__ volatile("svc #0");
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x0" : "=r"(ret));
#endif
    return (int)ret;
}

// 9. open
int jinn_open(const char *path, int flags, mode_t mode) {
    long ret;
#ifdef __x86_64__
    __asm__ volatile("mov %0, %%rdi" : : "r"(path));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rsi" : : "r"(flags));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rdx" : : "r"(mode));
    jinn_rand_nops(2);
    __asm__ volatile("mov $2, %%rax");
    jinn_rand_nops(2);
    __asm__ volatile("syscall");
    jinn_rand_nops(2);
    __asm__ volatile("mov %%rax, %0" : "=r"(ret));
#elif defined(__aarch64__)
    __asm__ volatile("mov %0, x0" : : "r"(path));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x1" : : "r"(flags));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x2" : : "r"(mode));
    jinn_rand_nops(2);
    __asm__ volatile("mov x8, #56");
    jinn_rand_nops(2);
    __asm__ volatile("svc #0");
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x0" : "=r"(ret));
#endif
    return (int)ret;
}

// 10. read
ssize_t jinn_read(int fd, void *buf, size_t count) {
    long ret;
#ifdef __x86_64__
    __asm__ volatile("mov %0, %%rdi" : : "r"(fd));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rsi" : : "r"(buf));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rdx" : : "r"(count));
    jinn_rand_nops(2);
    __asm__ volatile("mov $0, %%rax");
    jinn_rand_nops(2);
    __asm__ volatile("syscall");
    jinn_rand_nops(2);
    __asm__ volatile("mov %%rax, %0" : "=r"(ret));
#elif defined(__aarch64__)
    __asm__ volatile("mov %0, x0" : : "r"(fd));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x1" : : "r"(buf));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x2" : : "r"(count));
    jinn_rand_nops(2);
    __asm__ volatile("mov x8, #63");
    jinn_rand_nops(2);
    __asm__ volatile("svc #0");
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x0" : "=r"(ret));
#endif
    return (ssize_t)ret;
}

// 11. write
ssize_t jinn_write(int fd, const void *buf, size_t count) {
    long ret;
#ifdef __x86_64__
    __asm__ volatile("mov %0, %%rdi" : : "r"(fd));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rsi" : : "r"(buf));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rdx" : : "r"(count));
    jinn_rand_nops(2);
    __asm__ volatile("mov $1, %%rax");
    jinn_rand_nops(2);
    __asm__ volatile("syscall");
    jinn_rand_nops(2);
    __asm__ volatile("mov %%rax, %0" : "=r"(ret));
#elif defined(__aarch64__)
    __asm__ volatile("mov %0, x0" : : "r"(fd));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x1" : : "r"(buf));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x2" : : "r"(count));
    jinn_rand_nops(2);
    __asm__ volatile("mov x8, #64");
    jinn_rand_nops(2);
    __asm__ volatile("svc #0");
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x0" : "=r"(ret));
#endif
    return (ssize_t)ret;
}

// 12. kill
int jinn_kill(pid_t pid, int sig) {
    long ret;
#ifdef __x86_64__
    __asm__ volatile("mov %0, %%rdi" : : "r"(pid));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rsi" : : "r"(sig));
    jinn_rand_nops(2);
    __asm__ volatile("mov $62, %%rax");
    jinn_rand_nops(2);
    __asm__ volatile("syscall");
    jinn_rand_nops(2);
    __asm__ volatile("mov %%rax, %0" : "=r"(ret));
#elif defined(__aarch64__)
    __asm__ volatile("mov %0, x0" : : "r"(pid));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x1" : : "r"(sig));
    jinn_rand_nops(2);
    __asm__ volatile("mov x8, #129");
    jinn_rand_nops(2);
    __asm__ volatile("svc #0");
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x0" : "=r"(ret));
#endif
    return (int)ret;
}

// 13. setuid
int jinn_setuid(uid_t uid) {
    long ret;
#ifdef __x86_64__
    __asm__ volatile("mov %0, %%rdi" : : "r"(uid));
    jinn_rand_nops(2);
    __asm__ volatile("mov $105, %%rax");
    jinn_rand_nops(2);
    __asm__ volatile("syscall");
    jinn_rand_nops(2);
    __asm__ volatile("mov %%rax, %0" : "=r"(ret));
#elif defined(__aarch64__)
    __asm__ volatile("mov %0, x0" : : "r"(uid));
    jinn_rand_nops(2);
    __asm__ volatile("mov x8, #146");
    jinn_rand_nops(2);
    __asm__ volatile("svc #0");
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x0" : "=r"(ret));
#endif
    return (int)ret;
}

// 14. setgid
int jinn_setgid(gid_t gid) {
    long ret;
#ifdef __x86_64__
    __asm__ volatile("mov %0, %%rdi" : : "r"(gid));
    jinn_rand_nops(2);
    __asm__ volatile("mov $106, %%rax");
    jinn_rand_nops(2);
    __asm__ volatile("syscall");
    jinn_rand_nops(2);
    __asm__ volatile("mov %%rax, %0" : "=r"(ret));
#elif defined(__aarch64__)
    __asm__ volatile("mov %0, x0" : : "r"(gid));
    jinn_rand_nops(2);
    __asm__ volatile("mov x8, #147");
    jinn_rand_nops(2);
    __asm__ volatile("svc #0");
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x0" : "=r"(ret));
#endif
    return (int)ret;
}

// 15. memfd_create
int jinn_memfd_create(const char *name, unsigned int flags) {
    long ret;
#ifdef __x86_64__
    __asm__ volatile("mov %0, %%rdi" : : "r"(name));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rsi" : : "r"(flags));
    jinn_rand_nops(2);
    __asm__ volatile("mov $319, %%rax");
    jinn_rand_nops(2);
    __asm__ volatile("syscall");
    jinn_rand_nops(2);
    __asm__ volatile("mov %%rax, %0" : "=r"(ret));
#elif defined(__aarch64__)
    __asm__ volatile("mov %0, x0" : : "r"(name));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x1" : : "r"(flags));
    jinn_rand_nops(2);
    __asm__ volatile("mov x8, #279");
    jinn_rand_nops(2);
    __asm__ volatile("svc #0");
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x0" : "=r"(ret));
#endif
    return (int)ret;
}

// 16. execveat
int jinn_execveat(int dirfd, const char *pathname, char *const argv[], char *const envp[], int flags) {
    long ret;
#ifdef __x86_64__
    __asm__ volatile("mov %0, %%rdi" : : "r"(dirfd));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rsi" : : "r"(pathname));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%rdx" : : "r"(argv));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%r10" : : "r"(envp));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, %%r8" : : "r"(flags));
    jinn_rand_nops(2);
    __asm__ volatile("mov $322, %%rax");
    jinn_rand_nops(2);
    __asm__ volatile("syscall");
    jinn_rand_nops(2);
    __asm__ volatile("mov %%rax, %0" : "=r"(ret));
#elif defined(__aarch64__)
    __asm__ volatile("mov %0, x0" : : "r"(dirfd));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x1" : : "r"(pathname));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x2" : : "r"(argv));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x3" : : "r"(envp));
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x4" : : "r"(flags));
    jinn_rand_nops(2);
    __asm__ volatile("mov x8, #281");
    jinn_rand_nops(2);
    __asm__ volatile("svc #0");
    jinn_rand_nops(2);
    __asm__ volatile("mov %0, x0" : "=r"(ret));
#endif
    return (int)ret;
}
