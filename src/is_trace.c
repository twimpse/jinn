#include <sys/ptrace.h>
#include <unistd.h>
#include <sys/prctl.h>

int is_debugged(void) {
  return ptrace(PTRACE_TRACEME, 0, 1, 0) == -1;
}

void prevent_trace(void) {
// Prevent ptrace attachment (Linux)
  prctl(PR_SET_DUMPABLE, 0);  // Disable core dumps, often blocks debugging
  prctl(PR_SET_PTRACER, 0);   // Only allow PID 0 (no one) to attach
}
