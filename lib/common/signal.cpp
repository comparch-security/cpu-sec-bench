#include "include/signal.hpp"
#include "include/gcc_builtin.hpp"
#include <csignal>
#include <cstdlib>
#include <cstdio>
#include <stack>

const int RT_CODE_MISMATCH = -1;      // ERROR: mismatched signal
const int RT_CODE_NX       = 16;      // non-execution on writable pages

std::stack<sigact_record_t *> sigact_stack;

void xcpt_nx_handler(int signo, siginfo_t *sinfo, void *context) {
  // check the exception cause
  sigact_record_t *record = sigact_stack.top();
  sigact_stack.pop();
  if((record->faulty_data_addr != NULL && record->faulty_data_addr != sinfo->si_addr) // faulty addr
     || sinfo->si_code != SEGV_ACCERR) {                                            // exception cause
    puts("xcpt_nx_handler(): mismatched SEGV signal.");
    exit(RT_CODE_MISMATCH);
  } else {
    delete record;
    exit(RT_CODE_NX);
  }
}

void FORCE_INLINE begin_catch_nx_exception_common(sigact_record_t *record) {
  // construct the signal action
  struct sigaction act;
  act.sa_sigaction = xcpt_nx_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_SIGINFO;

  // install the signal action
  if(-1 == sigaction(SIGSEGV, &act, NULL)) {
    perror("begin_catch_nx_exception()");
    exit(-1);
  } else {
    sigact_stack.push(record);
  }
}

void begin_catch_nx_exception(const void *expected_faulty_addr) {
  // construct the sigact environment
  sigact_record_t *record = new sigact_record_t;
  record->faulty_data_addr = expected_faulty_addr;

  begin_catch_nx_exception_common(record);
}

void begin_catch_nx_exception(const void **expected_faulty_addr) {
  // construct the sigact environment
  sigact_record_t *record = new sigact_record_t;
  record->faulty_func_addr = expected_faulty_addr;

  begin_catch_nx_exception_common(record);
}

void begin_catch_nx_exception() {
  // construct the sigact environment
  sigact_record_t *record = new sigact_record_t;
  record->faulty_data_addr = NULL;

  begin_catch_nx_exception_common(record);
}

void end_catch_nx_exception() {
  // construct a default signal action
  struct sigaction act;
  act.sa_handler = SIG_DFL;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;

  // install the default signal action
  if(-1 == sigaction(SIGSEGV, &act, NULL)) {
    perror("end_catch_nx_exception()");
    exit(-1);
  } else {
    delete sigact_stack.top();
    sigact_stack.pop();
  }   
}
