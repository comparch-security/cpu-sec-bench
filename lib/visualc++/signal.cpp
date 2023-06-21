#include "include/signal.hpp"

// information recorded for exception checking
struct exception_record_t{
  enum {PTR,PTRPTR} tok;
  union {
    // faulty memory location: SIGILL, SIGFPE, SIGSEGV, SIGBUS, and SIGTRAP
    const PVOID si_ExceptionAddress;
    const PVOID *si_IndExceptionAddress;
  };
  DWORD si_ExceptionCode;
  ULONG* si_ExceptionInformation;
  int rv_code;
  exception_record_t() = default;
  exception_record_t (const PVOID si_ExceptionAddress, DWORD si_ExceptionCode, ULONG* si_ExceptionInformation, int rv_code):
  tok(PTR), si_ExceptionAddress(si_ExceptionAddress), si_ExceptionCode(si_ExceptionCode), si_ExceptionInformation(si_ExceptionInformation),
  rv_code(rv_code){};
  exception_record_t (const PVOID *si_IndExceptionAddress, DWORD si_ExceptionCode, ULONG* si_ExceptionInformation, int rv_code):
  tok(PTRPTR), si_ExceptionAddress(si_ExceptionAddress), si_ExceptionCode(si_ExceptionCode), si_ExceptionInformation(si_ExceptionInformation),
  rv_code(rv_code){};
  exception_record_t operator=(const exception_record_t & e){
    if(e.tok == PTR)
      return exception_record_t(e.si_ExceptionAddress,e.si_ExceptionCode,e.si_ExceptionInformation,e.rv_code);
    else
      return exception_record_t(e.si_IndExceptionAddress,e.si_ExceptionCode,e.si_ExceptionInformation,e.rv_code);
  }
};

static std::vector<exception_record_t> sigact_stack(16);
PVOID installed_handler = NULL;
static int svp = -1;

LONG WINAPI TopLevelUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptioninfo){
    perror("begin_catch_nx_exception() fails to catch EXCEPTIONS!");
    return EXCEPTION_EXECUTE_HANDLER;
}

bool checkExceptInformation(ULONG* record_exceptionInfo, ULONG_PTR* exceptionInfo){

  for(int i = 0; i != EXCEPTION_MAXIMUM_PARAMETERS; i++){
    if(record_exceptionInfo[i] != exceptionInfo[i])
      return false;
  }
  return true;

}

LONG WINAPI ExceptionHandler(struct _EXCEPTION_POINTERS* pExceptionInfo){
  // check the exception cause
  for(int i=svp; i>=0; i--) {
    if((sigact_stack[i].si_ExceptionCode == pExceptionInfo->ExceptionRecord->ExceptionCode) &&
       (sigact_stack[i].si_ExceptionInformation == NULL ||
       checkExceptInformation(sigact_stack[i].si_ExceptionInformation,(pExceptionInfo->ExceptionRecord->ExceptionInformation))) &&
      (sigact_stack[i].si_ExceptionAddress == NULL ||
      sigact_stack[i].si_ExceptionAddress == pExceptionInfo->ExceptionRecord->ExceptionAddress))
      exit(sigact_stack[i].rv_code);
  }

  fprintf(stderr, "ExceptionHandler(): mismatched signal with ExceptionCode = %d, ExceptionAddress = 0x%p\n",
   pExceptionInfo->ExceptionRecord->ExceptionCode, pExceptionInfo->ExceptionRecord->ExceptionAddress);

  fprintf(stderr,"ExceptionInformation is:");
  for(int i = 0; i != EXCEPTION_MAXIMUM_PARAMETERS; i++){
    fprintf(stderr," %d",pExceptionInfo->ExceptionRecord->ExceptionInformation[i]);
  }
  //if(sinfo->si_addr != NULL) fprintf(stderr, "bad data = 0x%lx\n", *(unsigned int *)sinfo->si_addr);
  //for(int i=svp; i>=0; i--) {
  //  fprintf(stderr, "registered sigact: si_signo = %d, si_code = %d and fault-addr = 0x%p\n", sigact_stack[i].si_signo, sigact_stack[i].si_code, sigact_stack[i].daddr);
  //}
  exit(RT_CODE_MISMATCH);
  return EXCEPTION_CONTINUE_SEARCH;
}

void begin_catch_exception(const PVOID expected_faulty_addr, ULONG* si_code,
                      int rv_code, DWORD si_signo){
    if(svp < 0){
        if(SetUnhandledExceptionFilter(TopLevelUnhandledExceptionFilter) != NULL){
            perror("SetUnhandledExceptionFilter() already has set TopLevelUnhandledExceptionFilter!");
        }
        installed_handler = AddVectoredExceptionHandler(0,ExceptionHandler);
    }
    sigact_stack[++svp] = {expected_faulty_addr, si_signo, si_code, rv_code};
}

void begin_catch_exception(const PVOID* expected_faulty_addr, ULONG* si_code,
                      int rv_code, DWORD si_signo){
    if(svp < 0){
        if(SetUnhandledExceptionFilter(TopLevelUnhandledExceptionFilter) != NULL){
            perror("SetUnhandledExceptionFilter() already has set TopLevelUnhandledExceptionFilter!");
        }
        installed_handler = AddVectoredExceptionHandler(0,ExceptionHandler);
    }
    sigact_stack[++svp] = {expected_faulty_addr, si_signo, si_code, rv_code};
}

void end_catch_exception() {
  --svp;
  if(svp < 0) {
    if(SetUnhandledExceptionFilter(NULL) == NULL){
      perror("SetUnhandledExceptionFilter() fails to set TopLevelUnhandledExceptionFilter previously!");
      exit(-1);
    }
    if(!RemoveVectoredExceptionHandler(installed_handler)){
      perror("end_catch_nx_exception() fails to reset Vectored Exception!");
      exit(-1);
    }
  }
}