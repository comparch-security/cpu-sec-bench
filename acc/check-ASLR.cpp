#include <string>
#include <cstdlib>
#include <cstdint>
#include "include/gcc_builtin.hpp"
#include <iostream>

int FORCE_NOINLINE helper(long long p0, long long p1) {
  std::cerr << p0 << " " << p1 << std::endl;
  return p0 == p1 ? 1 : 0;
}

int main(int argc, char* argv[]) {
  uintptr_t fp = reinterpret_cast<uintptr_t>(&helper);
  if(argc == 1) {
    std::string cmd = "test/acc-check-ASLR ";
    cmd += std::to_string(fp);
    return system(cmd.c_str());
  } else if(argc == 2) {
    std::string fp_prev(argv[1]);
    return helper(fp, std::stoll(fp_prev));
  }
  return 2;
}
