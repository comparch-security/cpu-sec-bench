#include <cstdlib>
#include <cstring>
#include "include/cfi.hpp"

#define VPOINTER void**

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
VPOINTER creat_fake_vtable(int func_num)
{
	VPOINTER addr = (VPOINTER)malloc(func_num * sizeof(void*));
	return addr;
}

void memory_exchange(void * a, void * b, int byte)
{
	void* c = malloc(byte);
	memcpy(c, a, byte);
	memcpy(a, b, byte);
	memcpy(b, c, byte);

}
