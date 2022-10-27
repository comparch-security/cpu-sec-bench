// standard cpp library
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <list>

// POSIX APIs (linux variant)
#include <spawn.h>
#include <sys/wait.h>

// 3rd party library
#include "scheduler/json.hpp"

// internal library
#include "lib/include/temp_file.hpp"

using json = nlohmann::basic_json<nlohmann::ordered_map>;
static json config_db, result_db, var_db;
char arg_pool[32][64];   // the maximal is 32 64-byte long arguments
char * gargv[33];
char env_pool[32][64];
char * genvv[33];

// global configure parameters
bool cond_run = false;
bool debug_run = false;
bool make_run = true;
bool test_run = true;
bool report_run = false;

// json related functions
bool file_exist(const std::string& fn);
bool read_json(json &db, const std::string& fn, bool notice);
bool dump_json(json &db, const std::string& fn, bool notice);
void report_gen();

// test case related
extern char **environ; // especially required by spawn
std::list<std::string> collect_case_list();
typedef std::list<std::string> str_list_t;
typedef std::list<str_list_t>  str_llist_t;
int case_parser(const std::string& cn, str_list_t& arg_list, str_list_t& vn, std::set<int> &results,
                std::set<int> &retry_results, str_llist_t &bf_list, str_llist_t &rf_list);
char ** argv_conv(const std::string &cmd, const str_list_t &args);
int run_cmd(char *argv[], char *envv[]);
bool run_tests(std::list<std::string> cases);

int main(int argc, char* argv[]) {
  // parse argument
  for(int i=1; i<argc; i++) {
    std::string param(argv[i]);

    if(param == "help") {
      std::cout << "Usage: ./run-test [OPTION]" << std::endl;
      std::cout << "Run Script for the Security Test Benchmark." << std::endl;
      std::cout << std::endl;
      std::cout << "Possible parameters:" << std::endl;
      std::cout << "  help        Show this help information." << std::endl;
      std::cout << "  continue    Continue a previous test by reading the results.json file first." << std::endl;
      std::cout << "  debug       Stop testing on the first unexpected exit status." << std::endl;
      std::cout << "  make-only   Make the test cases without running them." << std::endl;
      std::cout << "  no-make     Due to make the test cases as they are made aleady." << std::endl;
      std::cout << "  report      Generate a report after finishing all test cases." << std::endl;
      return 0;
    }

    if(param == "continue")  cond_run   = true;
    if(param == "debug")     debug_run  = true;
    if(param == "make-only") test_run   = false;
    if(param == "no-make")   make_run   = false;
    if(param == "report")    report_run = true;
  }

  // read the configure file
  if(!read_json(config_db, "configure.json", false)) return 1;

  // potentially read the results.json
  if(cond_run) {
    if(file_exist("results.json") && !read_json(result_db, "results.json", false)) return 1;
    if(file_exist("variables.json") && !read_json(result_db, "variables.json", false)) return 1;
  }

  run_tests(collect_case_list());

  if(report_run) report_gen();

  return 0;
}

bool file_exist(const std::string& fn) {
  std::ifstream file(fn);
  if(file.is_open()) {
    file.close();
    return true;
  } else
    return false;
}

bool read_json(json &db, const std::string& fn, bool notice) {
  std::ifstream db_file(fn);
  if(db_file.good()) {
    if(notice) std::cout << "read json file " << fn << std::endl;
    db_file >> db;
    db_file.close();
    return true;
  } else {
    std::cerr << "Fail to open json file `" << fn << "'" << std::endl;
    return false;
  }
}

bool dump_json(json &db, const std::string& fn, bool notice) {
  std::ofstream db_file(fn);
  if(!db_file.fail()) {
    if(notice) std::cout << "dump json " << fn << std::endl;
    db_file << db.dump(4);
    db_file.close();
    return true;
  } else {
    std::cerr << "Fail to dump json file `" << fn << "'" << std::endl;
    return false;
  }
}

void report_gen() {
  std::ofstream report_file("results.dat");

  // insert sys infomation to the result file
  std::ifstream sys_info_file("test/sys_info.txt");
  char buf[256];
  while(!sys_info_file.eof()) {
    sys_info_file.getline(buf, 256);
    report_file << "# " << buf << std::endl;
  }
  sys_info_file.close();

  // record test result
  for(auto record: result_db.get<std::map<std::string, json> >())
    report_file << record.first << " " << record.second["result"] << std::endl;
  report_file.close();  
}

std::list<std::string> collect_case_list() {
  std::list<std::string> rv;
  for(auto record: config_db.get<std::map<std::string, json> >())
    if(!result_db.count(record.first)) rv.push_back(record.first);
  return rv;
}

void proc_arg_parser(const json& tcase, const str_list_t& input_arg, str_llist_t& output_arg, bool IsEnv){
  for(auto ia:input_arg){
    if(ia.size() >= 3) {
      auto type = ia.substr(0,2);
      if(type == "-r") { // range
        auto range_name = ia.substr(2);
        if(tcase.count(range_name)) {
          auto range_value = tcase[range_name].get<std::vector<int> >();
          auto arg_list_size_org = output_arg.size();
          for(int i=0; i<arg_list_size_org; i++) {
            auto ale = output_arg.front();
            output_arg.pop_front();
            for(int r=range_value[0]; r<range_value[1]; r+=range_value[2]) {
              auto new_ale = str_list_t(ale);
              if(!IsEnv) new_ale.push_back(std::to_string(r));
              output_arg.push_back(new_ale);
            }
          }
        } else // fallback
          for(auto &ale : output_arg) ale.push_back(ia);
      } else if(type == "-l") { // list
        auto list_name = ia.substr(2);
        if(tcase.count(list_name)) {
          auto list_value = tcase[list_name].get<std::vector<int> >();
          auto arg_list_size_org = output_arg.size();
          for(int i=0; i<arg_list_size_org; i++) {
            auto ale = output_arg.front();
            output_arg.pop_front();
            for(auto v: list_value) {
              auto new_ale = str_list_t(ale);
              if(!IsEnv) new_ale.push_back(std::to_string(v));
              output_arg.push_back(new_ale);
            }
          }
        } else // fallback
          for(auto &ale : output_arg) ale.push_back(ia);
      } else if(type == "-v") { // variable
        auto var_name = ia.substr(2);
        if(var_db.count(var_name))
          for(auto &ale : output_arg) ale.push_back(std::to_string(var_db[var_name].get<int>()));
        else // fallback
          for(auto &ale : output_arg) ale.push_back(ia);
      }
      else // other, fallback
        for(auto &ale : output_arg) ale.push_back(ia);
    }else
      for(auto &ale : output_arg) ale.push_back(ia);
  }
}

int case_parser(const std::string& cn, std::string& pn, str_llist_t& arg_list, str_list_t& vn,
                 std::set<int> &expect_results, std::set<int> &retry_results, str_llist_t &bf_list, str_llist_t &rf_list) {
  // check whether the case exist
  if(!config_db.count(cn)) {
    std::cerr << "Fail to parse test case " << cn << std::endl;
    return -1;
  }

  auto tcase = config_db[cn];

  // check requirement
  int req_case = 0;
  std::string req_case_str = "0";
  if(tcase.count("require")) {
    bool req_case_all_tested = true;
    bool req_case_tested = false;
    bool req_case_ok = false;
    do {
      req_case_str = std::to_string(req_case++);
      if(!tcase["require"].count(req_case_str)) break;
      req_case_tested = true;

      // get the prerequisit list
      auto require_list = tcase["require"][req_case_str].get<str_llist_t>();

      // check whether the required test cases are tested
      for(auto and_conds : require_list) {
        if(!req_case_tested) break;
        for(auto or_cond : and_conds) {
          if(!result_db.count(or_cond)) {
            req_case_tested = false;
            req_case_all_tested = false;
            break;
          }
        }
      }

      // if tested, check the test condition is met
      if(req_case_tested) {
        req_case_ok = true;
        for(auto and_conds : require_list) {
          bool or_cond_ok = false;
          for(auto or_cond : and_conds)
            if(result_db.count(or_cond) && 0 == result_db[or_cond]["result"].get<int>())
              or_cond_ok = true;
          if(!or_cond_ok) {
            req_case_ok = false;
            break;
          }
        }
      }
    } while(!req_case_tested || !req_case_ok);

    if(!req_case_all_tested) return 1; // prerequisit not tested yet
    if(!req_case_ok) return 1024; // no need to test as all prerequisit tested and failed
  }

  // get the program name
  if(tcase.count("program"))
    pn = tcase["program"].get<std::string>();
  else
    pn = cn;

  // get the argument lists
  arg_list.clear();
  arg_list.push_back(str_list_t());
  if(tcase.count("arguments")) {
    auto arguments = tcase["arguments"][req_case_str].get<str_list_t>();
    proc_arg_parser(tcase, arguments, arg_list, false);
  }

  bf_list.clear();
  bf_list.push_back(str_list_t());
  if(tcase.count("build-flag") && tcase["build-flag"].count(req_case_str)){
    auto build_flag = tcase["build-flag"][req_case_str].get<str_list_t>();
    proc_arg_parser(tcase, build_flag, bf_list, true);
  }

  rf_list.clear();
  rf_list.push_back(str_list_t());
  if(tcase.count("run-flag") && tcase["run-flag"].count(req_case_str)){
    auto run_flag = tcase["run-flag"][req_case_str].get<str_list_t>();
    proc_arg_parser(tcase, run_flag, rf_list, true);
  }

  // check whether the case store a global variable
  if(tcase.count("set-var") && tcase["set-var"].count(req_case_str)) {
    vn = tcase["set-var"][req_case_str].get<str_list_t>();
  } else
    vn.clear();

  // collect the known exit code other than 0
  expect_results.clear();
  if(tcase.count("expect-results")) {
    for(auto r: tcase["expect-results"].get<std::map<std::string, json> >()) {
      expect_results.insert(std::stoi(r.first));
    }
  }
  retry_results.clear();
  if(tcase.count("retry-results")) {
    for(auto r: tcase["retry-results"].get<std::map<std::string, json> >()) {
      retry_results.insert(std::stoi(r.first));
    }
  }

  //std::cout << "\nrun " << cn << " with requirements case " << req_case_str << std::endl;
  return 0;
}

int run_cmd(char *argv[], char *envv[]) {
  pid_t pid;int rv = 0;
  if(envv && envv[0])
    rv = posix_spawnp(&pid, argv[0], NULL, NULL, argv, envv);
  else
    rv = posix_spawnp(&pid, argv[0], NULL, NULL, argv, environ);
  if(rv) {
    if(rv == ENOSYS) {
      std::cerr << "posix_spawn() is support in this system!" << std::endl;
      exit(1);
    } else {
      return rv;
    }
  }

  int s, status;
  do {
     s = waitpid(pid, &status, WUNTRACED | WCONTINUED);
     if(s == -1) {
       std::cerr << "waitpid() is support in this system!" << std::endl;
       exit(1);
     }
  } while (!WIFEXITED(status) && !WIFSIGNALED(status));

  if(WIFSIGNALED(status)) {
    std::cout << argv[0] << " terminated with signal " << WTERMSIG(status) << std::endl;
    return 256+WTERMSIG(status);
  }

  if(WIFEXITED(status))
    return WEXITSTATUS(status);

  return -2;  // should not run here!
}

char ** argv_conv(const std::string &cmd, const str_list_t &args) {
  int i = 0;
  strcpy(arg_pool[i++], cmd.c_str());
  for(const auto a:args) strcpy(arg_pool[i++],a.c_str());
  for(int j=0; j<i; j++) gargv[j]=arg_pool[j];
  gargv[i] = NULL;
  return gargv;
}

char ** env_conv(const str_list_t& args){
  int i = 0;
  for(const auto a:args) strcpy(env_pool[i++],a.c_str());
  for(int j=0; j<i; j++) genvv[j]=env_pool[j];
  genvv[i] = NULL;
  return genvv;
}

bool run_tests(std::list<std::string> cases) {
  std::string prog, cmd;
  str_llist_t alist;
  str_llist_t bflag;
  str_llist_t rflag;
  str_list_t gvar;
  std::set<int> expect_results, retry_results, rvs;
  while(!cases.empty()) {
    auto cn = cases.front();
    cases.pop_front();
    //std::cerr << "case: " << cn << std::endl; // keep in case needed in debug
    int test_cond = case_parser(cn, prog, alist, gvar, expect_results,
                                retry_results, bflag, rflag);
    if(!test_run || test_cond == 0) {
      std::cout << "\n========== " << cn << " =========" << std::endl;
      std::cout << "make test/" << prog << std::endl;
      int rv = 0;
      rvs.clear();
      
      cmd = "test/" + prog;
      int run_nums = alist.size() > bflag.size() ?
                     (alist.size() > rflag.size() ? alist.size() : rflag.size())
                    :(bflag.size() > rflag.size() ? bflag.size() : rflag.size());
      auto aiter = alist.begin();
      auto biter = bflag.begin();
      auto riter = rflag.begin();

      for(int i = 0; i != run_nums; i++){
        if(make_run) {
          biter->push_front(cmd);
          rv = run_cmd(argv_conv("make", *biter),NULL);
          if(rv){
            std::cout << "fail to make " << prog << " with error status " << rv << std::endl;
            rv = -1;
          }
          if(std::next(biter) != bflag.cend()) biter++;
        }

        if(0 == rv && test_run) { // run the test case
          std::cout << "\n"; for(auto r:(*riter)) std::cout << r << " ";std::cout << cmd;
          for(auto a:(*aiter)) std::cout << " " << a; std::cout << std::endl;

          rv = run_cmd(argv_conv(cmd, *aiter), env_conv(*riter));

          // record run-time parameter
          if(gvar.size() == 1 && rv >= 32 && rv < 64) { // successfully find a run-time parameter
            std::cerr << "set runtime variable " << gvar.front() << " to " << rv - 32 << std::endl;
            var_db[gvar.front()] = rv-32; dump_json(var_db, "variables.json", false);
            rv = 0;
          }

          if(!gvar.empty() && rv == 64) { // a run-time parameter recorded in atmp file
            std::ifstream tmpf(temp_file_name(cmd, (*aiter)));
            if(tmpf.good()) {
              for(auto i =  gvar.begin(); i != gvar.end(); i++){
                int value;tmpf >> value;
                var_db[*i] = value; dump_json(var_db, "variables.json", false);
                std::cerr << "set runtime variable " << *i << " to " << value << " by reading " << temp_file_name(cmd, (*aiter)) << std::endl;
              }
              rv = 0;
              tmpf.close();
            }else
              std::cerr << "can't open tmp file, set gvar failed" << std::endl;
          }

          if(0 == rv) break;
          else rvs.insert(rv);

          if(!expect_results.count(rv) && !retry_results.count(rv)) {
            std::cerr << "Test abnormality: " << cn << " failed with unexpected exit value " << rv << std::endl;
            if(debug_run) exit(1);
          }
        }
      }

      if(rv != 0) {
        for(auto v:rvs) if(retry_results.count(v)) rv = v;
        for(auto v:rvs) if(expect_results.count(v)) rv = v;
        for(auto v:rvs) if(!expect_results.count(v) && !retry_results.count(v)) rv = v;
      }

      result_db[cn]["result"] = rv; dump_json(result_db, "results.json", false);
      if(rv != 0 && !expect_results.count(rv)) {
        std::cerr << "Test abnormality: " << cn << " failed with unexpected exit value " << rv << std::endl;
        if(debug_run) exit(1);
      }
    } else if(test_run && test_cond == 1)
      cases.push_back(cn);
    else if(test_run && test_cond == -1) {
        std::cerr << "Test abnormality: " << cn << " does not exist in the configure.json file." << std::endl;
        if(debug_run) exit(1);      
    } else if(test_run) {
      result_db[cn]["result"] = test_cond; dump_json(result_db, "results.json", false);
    }
  }
  return true;
}
