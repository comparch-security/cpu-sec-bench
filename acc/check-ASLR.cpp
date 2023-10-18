#include <string>
#include <cstdlib>
#include <cstdint>
#include "include/builtin.hpp"
#if !defined(TRACE_RUN)
  #include <fstream>
#endif
#if !defined(TRACE_RUN) && !defined(DEBUG_OUTPUT)
  #include <iostream>
#endif

int FORCE_NOINLINE helper(uintptr_t p0, uintptr_t p1) {
  // if debug code is really needed for future use, keep it simple and commented out
  //std::cerr << std::hex << p0 << " " << p1 << std::endl;
  return p0 == p1 ? 1 : 0;
}

std::string tmp_file = "check-ASLR.tmp";

int main(int argc, char* argv[]) {
  uintptr_t fp = reinterpret_cast<uintptr_t>(&helper);
  if(argc == 1) {
    if(0 == system("test" PATH_SEPARATOR "acc-check-ASLR 1")) { // run the inner call
      std::ifstream tmpf(tmp_file);
      uintptr_t addr;
      unsigned long long addr_num;
      tmpf >> addr_num;
      addr = (uintptr_t)addr_num;
      tmpf.close();
      return helper(fp, addr); // check whether both calls put helper() on the same location
    } else
      return 2; // should never run to here
  } else { // write out the location of helper() to tmp file
    std::ofstream tmpf(tmp_file);
    tmpf << (unsigned long long)fp;
    tmpf.close();
    return 0;
  }
}
