#ifndef CSB_TEMP_FILE_HPP
#define CSB_TEMP_FILE_HPP

#include <cstring>
#include <string>
#include <list>
#include <sstream>

extern std::string temp_file_name(const std::string& cmd, const std::list<std::string>& glist);
extern void write_to_temp_file(int var, int argc, char **argv);
/*To read stack/heap region's base*/
extern long long get_region_base(const char* region_kind);

#endif
