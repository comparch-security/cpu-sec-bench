#include <string>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include "include/gcc_builtin.hpp"

int FORCE_NOINLINE helper(long long p0, long long p1) {
  std::cout << "p0 = " << p0 << ", p1 = " << p1 << std::endl;
  return p0 == p1 ? 0 : 1;
}

int main(int argc, char* argv[]) {
  uintptr_t fp = reinterpret_cast<uintptr_t>(&helper);
  std::cout << std::hex << "fp = " << fp << std::endl;
  if(argc == 1) {
    // original call
    std::string cmd = "./acc-check-ASLR ";
    cmd += std::to_string(fp);
    std::cout << "cmd = " <<cmd << std::endl;
    int rv = system(cmd.c_str());
    std::cout << "rv = " << rv << std::endl;
    return rv == 0 ? 0 : 1;
  } else if(argc == 2) {
    std::string fp_prev(argv[1]);
    std::cout << "fp_pre = " << std::stoll(fp_prev) << std::endl;
    return helper(fp, std::stoll(fp_prev));
  }
  return 2;
}
