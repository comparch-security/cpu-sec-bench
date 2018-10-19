#include <cstdlib>
#include <cstring>
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

// Fake Vtable in Heap
extern pvtable_t create_fake_vtable_on_heap(unsigned int nfunc) {
	pvtable_t addr = (pvtable_t)malloc(nfunc * sizeof(pvtable_t));
	return addr;
}

void memory_exchange(void * a, void * b, int byte)
{
	void* c = malloc(byte);
	memcpy(c, a, byte);
	memcpy(a, b, byte);
	memcpy(b, c, byte);

}
