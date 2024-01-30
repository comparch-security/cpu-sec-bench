#include <string>
#include <cstdlib>
#include <cstdint>
#include "include/builtin.hpp"
#include "include/temp_file.hpp"
#include <cstring>
#if !defined(TRACE_RUN)
  #include <fstream>
#endif
#if !defined(TRACE_RUN) && !defined(DEBUG_OUTPUT)
  #include <iostream>
#endif

int FORCE_NOINLINE helper(uintptr_t p0, uintptr_t p1) {
  // if debug code is really needed for future use, keep it simple and commented out
  //std::cerr << std::hex << p0 << " " << p1 << std::endl;
  return p0 == p1 ? 0 : 1;
}

std::string tmp_file;

int main(int argc, char* argv[]) {
  uintptr_t fp = 0;

  std::stringstream ss;
  int count_flag, region_flag;
  ss << std::string(argv[1]); ss >> count_flag;
  ss.clear();
  ss << std::string(argv[2]); ss >> region_flag;
  switch (region_flag)
  {
  case 0:
    tmp_file = "check-ASLR.tmp";
    fp = reinterpret_cast<uintptr_t>(&helper);
    break;
  case 1:
    tmp_file = "check-stack-region-ASLR.tmp";
    fp = static_cast<uintptr_t>(get_region_base("stack"));
    break;
  default:
    return -1; // should never run to here
  }

  if(count_flag == 1){
    // write out the location of helper() to tmp file
    std::ofstream tmpf(tmp_file);
    tmpf << (unsigned long long)fp;
    tmpf.close();
    return 0;
  }else if(count_flag == 0){
    char command[50] = "test" PATH_SEPARATOR "acc-check-ASLR 1 ";
    strncat(command, argv[2], 2);
    if(0 == system(command)) { // run the inner call
      std::ifstream tmpf(tmp_file);
      uintptr_t addr;
      unsigned long long addr_num;
      tmpf >> addr_num;
      addr = (uintptr_t)addr_num;
      tmpf.close();
      return helper(fp, addr); // check whether both calls put helper() on the same location
    } else
      return 2; // should never run to here
  }
}
