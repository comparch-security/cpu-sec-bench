#ifndef CSB_TEMP_FILE_HPP
#define CSB_TEMP_FILE_HPP

#include<string>
#include<list>

extern std::string temp_file_name(const std::string& cmd, const std::list<std::string>& glist);
extern void write_to_temp_file(int var, int argc, char **argv);

#endif
