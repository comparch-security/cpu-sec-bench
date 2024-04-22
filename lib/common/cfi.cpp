#include <cstdlib>
#include <cstring>

#define TRACE_EXTERN_RELATED_GLOBALVAR
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
