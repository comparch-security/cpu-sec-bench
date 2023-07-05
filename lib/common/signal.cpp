#include "include/signal.hpp"
#include "include/builtin.hpp"
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
  int si_signo;
  int si_code;
  int rv_code;
};

// use local pool to avoid affecting the heap
static std::vector<sigact_record_t> sigact_stack(16);
static int svp = -1;

void xcpt_handler(int signo, siginfo_t *sinfo, void *context) {
  // check the exception cause
  for(int i=svp; i>=0; i--) {
    if((sigact_stack[i].si_signo == sinfo->si_signo) &&
       (sigact_stack[i].si_code == 0 || sinfo->si_code == sigact_stack[i].si_code) &&
      (sigact_stack[i].daddr == NULL || sigact_stack[i].daddr == sinfo->si_addr))
      exit(sigact_stack[i].rv_code);
  }

  fprintf(stderr, "xcpt_handler(): mismatched signal with si_signo = %d, si_code = %d and fault-addr = 0x%p\n", sinfo->si_signo, sinfo->si_code, sinfo->si_addr);
  //if(sinfo->si_addr != NULL) fprintf(stderr, "bad data = 0x%lx\n", *(unsigned int *)sinfo->si_addr);
  //for(int i=svp; i>=0; i--) {
  //  fprintf(stderr, "registered sigact: si_signo = %d, si_code = %d and fault-addr = 0x%p\n", sigact_stack[i].si_signo, sigact_stack[i].si_code, sigact_stack[i].daddr);
  //}
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
    perror("begin_catch_nx_exception() fails to catch SIGSEGV!");
    exit(-1);
  }

  if(-1 == sigaction(SIGBUS, &act, NULL)) {
    perror("begin_catch_nx_exception() fails to catch SIGBUS!");
    exit(-1);
  }

  if(-1 == sigaction(SIGFPE, &act, NULL)) {
    perror("begin_catch_nx_exception() fails to catch SIGFPE!");
    exit(-1);
  }
  if(-1 == sigaction(SIGILL, &act, NULL)) {
    perror("begin_catch_nx_exception() fails to catch SIGFPE!");
    exit(-1);
  }
}

void begin_catch_exception(const void *expected_faulty_addr, int si_code, int rv_code, int si_signo) {
  if(svp < 0) begin_catch_exception_common();
  sigact_stack[++svp] = {expected_faulty_addr, si_signo, si_code, rv_code};
}

void begin_catch_exception(const void **expected_faulty_addr, int si_code, int rv_code, int si_signo) {
  if(svp < 0) begin_catch_exception_common();
  sigact_stack[++svp] = {expected_faulty_addr, si_signo, si_code, rv_code};
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
      perror("end_catch_nx_exception() fails to reset SIGSEGV!");
      exit(-1);
    }
    if(-1 == sigaction(SIGBUS, &act, NULL)) {
      perror("end_catch_nx_exxception() fails to reset SIGBUS!");
      exit(-1);
    }
    if(-1 == sigaction(SIGFPE, &act, NULL)) {
      perror("end_catch_nx_exception() fails to reset SIGFPE");
      exit(-1);
    }
    if(-1 == sigaction(SIGILL, &act, NULL)) {
      perror("end_catch_nx_exception() fails to reset SIGILL");
      exit(-1);
    }
  }
}
