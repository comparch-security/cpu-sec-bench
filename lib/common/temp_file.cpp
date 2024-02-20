#define TRACE_EXTERN_RELATED_GLOBALVAR
#include "include/temp_file.hpp"
#include "include/builtin.hpp"
#include <fstream>

std::string temp_file_name(const std::string& cmd, const std::list<std::string>& glist) {
  std::string fn = cmd;
  for(auto const &g:glist) fn += "_" + g;
  fn += ".tmp";
  return fn;
}

void write_to_temp_file(int var, int argc, char **argv) {
  std::string fn;
  for(int i=0; i<argc; i++) {
    fn += std::string(argv[i]);
    if(i+1 < argc) fn += "_";
  }
  fn += ".tmp";

  std::ofstream f(fn,std::ofstream::app);
  f << var << "\n";
  f.close();
}

#if defined(COMPILER_MSVC)
long long get_region_base(const char* region_kind){
    long long addr = 0;
    if(strcmp(region_kind, "stack") == 0){
      DWORD pid = GetCurrentProcessId();
      HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
      MEMORY_BASIC_INFORMATION mbi;
      VirtualQueryEx(hProcess, &mbi, &mbi, sizeof(mbi));
      addr = (long long)mbi.AllocationBase;
      CloseHandle(hProcess);
    }else if(strcmp(region_kind, "heap") == 0){
      HANDLE hDefaultProcessHeap;
      hDefaultProcessHeap = GetProcessHeap();
      if (hDefaultProcessHeap == NULL) {
          return 0;
      }
      addr = (long long)hDefaultProcessHeap;
    }
    return addr;
}
#else
long long get_region_base(const char* region_kind){
  std::ifstream ifs("/proc/self/maps");
  std::string line,base;
  while (std::getline(ifs, line)) {
      std::istringstream iss(line);
      std::string start, address, perms, offset, dev, inode, pathname;
      iss >> address >> perms >> offset >> dev >> inode >> pathname;
      int pos = address.find_first_of('-');
      start = address.substr(0,pos);
      if (std::string::npos != pathname.find(region_kind)) {
          base = start;
          break;
      }
  }
  char* pend;
  return std::strtoll(base.c_str(), &pend,16);
}
#endif