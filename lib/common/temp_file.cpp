#include "include/temp_file.hpp"
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

