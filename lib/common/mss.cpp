#define TRACE_EXTERN_RELATED_GLOBALVAR
#include "include/mss.hpp"

void FORCE_NOINLINE update_by_pointer(char *buf, long long offset, long long size, int step, char c) {
  buf += offset;
  for(long long i=0; i != size; i += step, buf += step)
    *buf = c;
}