#include "include/signal.hpp"
#include "include/gcc_builtin.hpp"
#include <cstdlib>
#include <cstdio>
#include <vector>

// information recorded for exception checking
struct sigact_record_t {
  union {
    // faulty memory location: SIGILL, SIGFPE, SIGSEGV, SIGBUS, and SIGTRAP
    const void *daddr;
    const void **faddr;
  };
  int err_code;
  int rv_code;
};

// use local pool to avoid affecting the heap
static std::vector<sigact_record_t> sigact_stack(16);
static int svp = -1;

void xcpt_handler(int signo, siginfo_t *sinfo, void *context) {
  // check the exception cause
  for(int i=svp; i>=0; i--) {
    if((sigact_stack[i].err_code == 0 || sinfo->si_code == sigact_stack[i].err_code) &&
      (sigact_stack[i].daddr == sinfo->si_addr))
      exit(sigact_stack[i].rv_code);
  }

  fprintf(stderr, "xcpt_handler(): mismatched SEGV signal with si_code = %d and fault-addr = 0x%p\n", sinfo->si_code, sinfo->si_addr);
  exit(RT_CODE_MISMATCH);
}

void FORCE_INLINE begin_catch_exception_common() {
  // construct the signal action
  struct sigaction act;
  act.sa_sigaction = xcpt_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_SIGINFO;

  // install the signal action
  if(-1 == sigaction(SIGSEGV, &act, NULL)) {
    perror("begin_catch_nx_exception()");
    exit(-1);
  }
}

void begin_catch_exception(const void *expected_faulty_addr, int err_code, int rv_code) {
  if(svp < 0) begin_catch_exception_common();
  sigact_stack[++svp] = {expected_faulty_addr, err_code, rv_code};
}

void begin_catch_exception(const void **expected_faulty_addr, int err_code, int rv_code) {
  if(svp < 0) begin_catch_exception_common();
  sigact_stack[++svp] = {expected_faulty_addr, err_code, rv_code};
}

void end_catch_exception() {
  --svp;
  if(svp < 0) {
    // construct a default signal action
    struct sigaction act;
    act.sa_handler = SIG_DFL;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    // install the default signal action
    if(-1 == sigaction(SIGSEGV, &act, NULL)) {
      perror("end_catch_nx_exception()");
      exit(-1);
    }
  }
}
