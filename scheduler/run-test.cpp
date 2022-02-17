#include <iostream>
#include <fstream>
#include <string>
#include <list>

#include "lib/include/jason.hpp"

using json = nlohmann::json;
static json config_db, result_db;

// json related functions
bool read_json(json &db, const std::string& fn, bool notice = true);
bool dump_json(json &db, const std::string& fn, bool notice = true);

// test case related
typedef std::list<std::string> arg_list_t;
bool case_parser(const std::sting& cn, arg_list_t& arg_list, std::string& vn);
bool run_tests();

int main(int argc, char* argv[]) {
  // parse argument

  // read the configure file
  if(!read_json(config_db, "configure.json")) return 1;

  

  return 0;
}

bool read_json(json &db, const std::string& fn, bool notice) {
  std::ifstream db_file(fn);
  if(db_file.good()) {
    if(notice) std::cerr << "read json file " << fn << std::endl;
    db_file >> db;
    db_file.close();
  } else {
    std::cerr << "Fail to open json file `" << fn << "'" << std::endl;
    return false;
  }
}

bool config::dump(json &db, const std::string& fn, bool notice) {
  std::ofstream db_file(fn);
  if(!db_file.fail()) {
    if(notice) std::cerr << "dump json " << cf << std::endl;
    db_file << db.dump(4);
    db_file.close();
    return true;
  } else {
    std::cerr << "Fail to dump json file `" << fn << "'" << std::endl;
    return false;
  }
}

bool case_parser(const std::sting& cn, arg_list_t& arg_list, std::string& vn) {
  // check whether the case exist
  if(!config_db.count(cn)) {
    std::cerr << "Fail to parse test case " << cn << std::endl;
    return false;
  }

  auto tcase = config_db[cn];

  // check requirement
  typedef std::list<std::list<std::string> > require_list_t;
  if(tcase.count("require")) {
    auto require_list = tcase["require"].get<require_list_t>();
    for(auto and_conds : require_list) {
      bool to_test = false;
      for(auto or_cond : and_conds) {
        if(result_db.count(or_cond) && 0 == result_db[or_cond].get<int>())
          to_test = true;
      }
      if(!to_test) return false;
    }
  }

  // get the argument lists
  arg_list.clear();
  arg_list.push_back("");
  if(tcase.count("arguments")) {
    auto arguments = tcase["arguments"].get<std::list<std::string> >();
    for(auto arg : arguments) {
      if(arg.size() >= 3) {
        auto atype = arg.substr(0,2);
        if(atype == "-r") { // range
          auto range_name = arg.substr(2);
          if(tcase.count(range_name)) {
            auto range_value = tcase[range_name].get<std::vector<int> >();
            for(int i=0; i<arg_list.size(); i++) {
              auto ale = arg_list.front();
              arg_list.pop_front();
              for(int r=range_value[0]; r<range_value[1]; r+=range_value[2])
                arg_list.push_back(ale + std::to_string(r) + " ");
            }
          } else // fallback
            for(auto ale : arg_list) ale += arg + " ";
        } else if(atype == "-l") { // list
        } else if(atype == "-v") { // variable
        } else // other, fallback
          for(auto ale : arg_list) ale += arg + " ";
      } else
        for(auto ale : arg_list) ale += arg + " ";
    }
  }
  
}

template<typename T>
bool obtain_config(T& rv, const std::string &param) {
  if(db.count(param)) rv = db[param].get<T>();
  else {
    std::cerr << "Fail to get parameter " << param << std::endl;
    return false;
  }
  return true;
}


bool run_tests() {
  
}
