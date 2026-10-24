#define TRACE_EXTERN_RELATED_GLOBALVAR
#include "include/signal.hpp"

#include <cerrno>
#include <cstdint>
#include <unistd.h>

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
// index of the newest live registration; volatile and sig_atomic_t because the handler
// reads it, and every store is published with a signal fence before the caller faults
static volatile sig_atomic_t svp = -1;

// Linux reports the tag bits in si_addr only when the handler asks for them.  The macro
// is invisible through <csignal>, and FreeBSD/CheriBSD has no such flag, so it is
// defined here for Linux only and the install below retries without it on EINVAL.
#ifndef SA_EXPOSE_TAGBITS
  #if defined(__linux__)
    #define SA_EXPOSE_TAGBITS 0x00000800
  #endif
#endif

// The kernel reports the tagged si_addr while a registration holds the untagged one, so
// the address predicate ignores the tag byte on a tag-capable architecture.  The mask is
// a no-op elsewhere, and masking a user address is harmless where no tag is in use.
#if defined(__aarch64__)
  #define RT_ADDR_TAG_MASK 0xff00000000000000ULL
#else
  #define RT_ADDR_TAG_MASK 0ULL
#endif

// Delivery and diagnostics from a handler must be async-signal-safe: a corrupted stack
// is never returned into, and no stdio lock is taken.
static void xcpt_deliver(int code) {
  _exit(code);
}

static void xcpt_report(const char *msg) {
  size_t len = 0;
  while(msg[len] != '\0') len++;
  ssize_t rv = write(STDERR_FILENO, msg, len);
  (void)rv;
}

static bool xcpt_addr_match(const void *registered, const void *faulted) {
  uintptr_t a = reinterpret_cast<uintptr_t>(registered) & ~(uintptr_t)RT_ADDR_TAG_MASK;
  uintptr_t b = reinterpret_cast<uintptr_t>(faulted)    & ~(uintptr_t)RT_ADDR_TAG_MASK;
  return a == b;
}

void xcpt_handler(int signo, siginfo_t *sinfo, void *context) {
  // check the exception cause
  for(int i=svp; i>=0; i--) {
    if((sigact_stack[i].si_signo == sinfo->si_signo) &&
       (sigact_stack[i].si_code == 0 || sinfo->si_code == sigact_stack[i].si_code) &&
      (sigact_stack[i].daddr == NULL || xcpt_addr_match(sigact_stack[i].daddr, sinfo->si_addr))){
        xcpt_deliver(sigact_stack[i].rv_code);
      }
  }

  xcpt_report("xcpt_handler(): mismatched signal\n");
  xcpt_deliver(RT_CODE_MISMATCH);
}

// Install xcpt_handler for one signal.  If the kernel rejects SA_EXPOSE_TAGBITS with
// EINVAL the handler is installed without it -- the tag predicate then degrades to the
// untagged comparison -- rather than turning every caught-exception case into a
// configuration failure.
static void install_xcpt_handler(int signo, const char *fail_msg) {
  struct sigaction act;
  act.sa_sigaction = xcpt_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_SIGINFO;
  #if defined(SA_EXPOSE_TAGBITS) && defined(__linux__)
    act.sa_flags |= SA_EXPOSE_TAGBITS;
  #endif

  if(-1 == sigaction(signo, &act, NULL)) {
  #if defined(SA_EXPOSE_TAGBITS) && defined(__linux__)
    if(errno == EINVAL) {
      act.sa_flags &= ~SA_EXPOSE_TAGBITS;
      if(-1 == sigaction(signo, &act, NULL)) {
        perror(fail_msg);
        exit(RT_CODE_LIBFAIL);
      }
      return;
    }
  #endif
    perror(fail_msg);
    exit(RT_CODE_LIBFAIL);
  }
}

void FORCE_INLINE begin_catch_exception_common() {
  install_xcpt_handler(SIGSEGV, "begin_catch_nx_exception() fails to catch SIGSEGV!");
  install_xcpt_handler(SIGBUS,  "begin_catch_nx_exception() fails to catch SIGBUS!");
  install_xcpt_handler(SIGFPE,  "begin_catch_nx_exception() fails to catch SIGFPE!");
  install_xcpt_handler(SIGILL,  "begin_catch_nx_exception() fails to catch SIGILL!");
}

void begin_catch_exception_gcc(const void *expected_faulty_addr, int si_code, int rv_code, int si_signo) {
  if(svp < 0) begin_catch_exception_common();
  if(svp + 1 >= (sig_atomic_t)sigact_stack.size()) {
    xcpt_report("begin_catch_exception(): the registration pool is full\n");
    xcpt_deliver(RT_CODE_LIBFAIL);
  }
  sigact_stack[++svp] = {expected_faulty_addr, si_signo, si_code, rv_code};
  __atomic_signal_fence(__ATOMIC_SEQ_CST);
}

void begin_catch_exception_gcc(const void **expected_faulty_addr, int si_code, int rv_code, int si_signo) {
  if(svp < 0) begin_catch_exception_common();
  if(svp + 1 >= (sig_atomic_t)sigact_stack.size()) {
    xcpt_report("begin_catch_exception(): the registration pool is full\n");
    xcpt_deliver(RT_CODE_LIBFAIL);
  }
  sigact_stack[++svp] = {expected_faulty_addr, si_signo, si_code, rv_code};
  __atomic_signal_fence(__ATOMIC_SEQ_CST);
}

void end_catch_exception() {
  if(svp < 0) {
    xcpt_report("end_catch_exception(): no live registration\n");
    xcpt_deliver(RT_CODE_LIBFAIL);
  }
  --svp;
  __atomic_signal_fence(__ATOMIC_SEQ_CST);
  if(svp < 0) {
    // construct a default signal action
    struct sigaction act;
    act.sa_handler = SIG_DFL;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    // install the default signal action
    if(-1 == sigaction(SIGSEGV, &act, NULL)) {
      perror("end_catch_nx_exception() fails to reset SIGSEGV!");
      exit(RT_CODE_LIBFAIL);
    }
    if(-1 == sigaction(SIGBUS, &act, NULL)) {
      perror("end_catch_nx_exxception() fails to reset SIGBUS!");
      exit(RT_CODE_LIBFAIL);
    }
    if(-1 == sigaction(SIGFPE, &act, NULL)) {
      perror("end_catch_nx_exception() fails to reset SIGFPE");
      exit(RT_CODE_LIBFAIL);
    }
    if(-1 == sigaction(SIGILL, &act, NULL)) {
      perror("end_catch_nx_exception() fails to reset SIGILL");
      exit(RT_CODE_LIBFAIL);
    }
  }
}
