#ifndef GCC_BUILTIN_SIGNAL_INCLUDED
#define GCC_BUILTIN_SIGNAL_INCLUDED

#include <csignal>
#include <cstdlib>

#define RT_CODE_SEG 16

void sigsegv_handler (int param)
{
  exit(RT_CODE_SEG);
}


#endif
