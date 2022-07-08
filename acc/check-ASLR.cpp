#include <string>
#include <cstdlib>
#include <cstdint>
#include "include/gcc_builtin.hpp"
#include <iostream>
#include <fstream>

int FORCE_NOINLINE helper(long long p0, long long p1) {
  std::cerr << p0 << " " << p1 << std::endl;
  return p0 == p1 ? 1 : 0;
}

long long readin_address(std::string tmp_string){
  std::ifstream tmpf(tmp_string,std::fstream::in);
  std::string tmp;
  tmpf >> tmp;
  tmpf.close();
  remove(tmp_string.c_str());
  return std::stoll(tmp);
}

void writeout_address(std::string tmp_string){
  std::ofstream tmpf(tmp_string,std::fstream::in | std::fstream::out | std::fstream::app);
  if(!tmpf) throw std::runtime_error("File create failed");
  tmpf << reinterpret_cast<uintptr_t>(&helper) << std::endl;
  tmpf.close();
}

int main(int argc, char* argv[]) {
  uintptr_t fp = reinterpret_cast<uintptr_t>(&helper);
  std::string tmp_string = "./cpu-sec-bench-tmpfile";
  if(argc == 1) {
    std::string cmd = "test/acc-check-ASLR ";
    cmd += std::to_string(fp);
    if(-1 == system(cmd.c_str())) std::cerr << "shell system libfunc failed" << std::endl;
    long long addr2 = readin_address(tmp_string);
    return helper(fp, addr2);
  } else if(argc == 2) {
    writeout_address(tmp_string);
  }
  return 2;
}