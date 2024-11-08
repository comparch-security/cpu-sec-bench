#include <cstdlib>
#include <cstring>

#define DLL_DEFINITION DllExport

#include "include/cfi.hpp"

void Base::virtual_func() {
  exit(1);
}

void Helper::virtual_func() {
  exit(2);
}

void Helper2::virtual_func(int arg) {
  lvar = arg;
  exit(3);
}

void Helper3::virtual_func(double arg) {
  lvar = arg;
}

void Base_1v::virtual_func(void*) {
  exit(1);
}

volatile arch_int_t cfi_offset;

void Ret_From_Helper::virtual_func(void *label) {
  COMPILER_BARRIER;
  GET_RAA_SP_OFFSET(cfi_offset);
  #ifdef TRACE_RUN
    GET_SP_BASE(ra_addr);
    WRITE_TRACE("RA address: 0x", ra_addr);
    WRITE_TRACE("RA before modified: 0x", *(long long*)ra_addr);
  #endif
  MOD_STACK_DAT(label, cfi_offset);
  /* HiFive Unmatched, GCC 11.2.0
   * Make sure cfi_offset is modified as otherwise
   * the stack is not expanded similarily with
   * the return-to-parent-same-call-site test.
   */
  cfi_offset = rand();
}

int BaseM::virtual_funcM0() {
  return 1;
}

int BaseM::virtual_funcM1() {
  return 2;
}

int Helper1M::virtual_funcM0() {
  return 3;
}

int Helper1M::virtual_funcM1() {
  return 4;
}

int Helper2M::virtual_funcM0() {
  return 5;
}

int Helper2M::virtual_funcM1() {
  return 6;
}

// Fake Vtable in Heap
extern pvtable_t create_fake_vtable_on_heap(unsigned int nfunc) {
  pvtable_t addr = (pvtable_t)malloc(nfunc * sizeof(pvtable_t));
  return addr;
}

extern void free_fake_vtable_on_heap(pvtable_t addr) {
  free(addr);
}

void memory_exchange(void * a, void * b, int byte) {
  void* c = malloc(byte);
  memcpy(c, a, byte);
  memcpy(a, b, byte);
  memcpy(b, c, byte);
}

// read write vtable pointer

pvtable_t read_vtable_pointer(const Base * p) {
  pvtable_t *vtp = (pvtable_t *)p;
  return *vtp;
}

void write_vtable_pointer(Base * pobj, pvtable_t vtp) {
  pvtable_t *vtp_orig = (pvtable_t *)pobj;
  *vtp_orig = vtp;
}

pvtable_t read_vtable_pointer(const BaseM * p) {
  pvtable_t *vtp = (pvtable_t *)p;
  return *vtp;
}

void write_vtable_pointer(BaseM * pobj, pvtable_t vtp) {
  pvtable_t *vtp_orig = (pvtable_t *)pobj;
  *vtp_orig = vtp;
}
