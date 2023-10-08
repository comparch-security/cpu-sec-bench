#ifdef _MSC_VER
// Windows MSVC
#define _CRT_SECURE_NO_WARNINGS
#include <process.h>
#else
// POSIX APIs (linux variant)
#include <spawn.h>
#include <sys/wait.h>
#endif

// standard cpp library
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <list>
#include <cstring>
#include <vector>

#include <chrono>
#include <iomanip>
// 3rd party library
#include "scheduler/json.hpp"

// internal library
#include "lib/include/temp_file.hpp"

using std::left;
using std::right;
using std::setw;
using json = nlohmann::basic_json<nlohmann::ordered_map>;
static json config_db, result_db, var_db;
char arg_pool[32][64];   // the maximal is 32 64-byte long arguments
char * gargv[33];

// global configure parameters
bool cond_run      = false;
bool debug_run     = false;
bool make_run      = true;
bool test_run      = true;
bool report_run    = false;
bool exhausted_run = false;
bool trace_run     = false;

// json related functions
bool file_exist(const std::string& fn);
bool read_json(json &db, const std::string& fn, bool notice);
bool dump_json(json &db, const std::string& fn, bool notice);
void report_gen();

long long make_time_count = 0, run_time_count = 0;

// test case related
char **org_env;
char **run_env;
void add_extra_run_prefix(char **);
#ifdef RUN_PREFIX
  static char run_prefix[] = RUN_PREFIX;
#endif
std::string make_config_macro;
std::list<char *> extra_run_prefix;
std::list<std::string> collect_case_list();
typedef std::list<std::string> str_list_t;
typedef std::list<str_list_t>  str_llist_t;
int case_parser(const std::string& cn, std::string& pn, str_llist_t& arg_list, str_list_t& vn,
                str_list_t& dbf, std::set<int> &expect_results, std::set<int> &retry_results);
void add_arguments(std::string arg, nlohmann::ordered_json tcase, str_llist_t &arg_list);
char ** argv_conv(const std::string &cmd, const str_list_t &args);
int run_cmd(char* argv[], char** runv, long long& time_count);
bool run_tests(std::list<std::string> cases);
long long get_file_size(const std::string& filename);

int main(int argc, char* argv[], char* envp[]) {
  // parse argument
  for(int i=1; i<argc; i++) {
    std::string param(argv[i]);

    if(param == "help") {
      std::cout << "Usage: ./run-test [OPTION]" << std::endl;
      std::cout << "Run Script for the Security Test Benchmark." << std::endl;
      std::cout << std::endl;
      std::cout << "Possible parameters:" << std::endl;
      std::cout << "  help          Show this help information." << std::endl;
      std::cout << "  continue      Continue a previous test by reading the results.json file first." << std::endl;
      std::cout << "  debug         Stop testing on the first unexpected exit status." << std::endl;
      std::cout << "  make-only     Make the test cases without running them." << std::endl;
      std::cout << "  no-make       Due to make the test cases as they are made aleady." << std::endl;
      std::cout << "  fast-run      Only run the test case that their requirement runs successfully, and then generate a report." << std::endl;
      std::cout << "  exhausted-run Run all tests until the total test case is exhausted, and then generate a report." << std::endl;
      std::cout << "  print-trace   Print the trace log of the attacked target." << std::endl;
      return 0;
    }

    else if(param == "continue")  cond_run   = true;
    else if(param == "debug")     debug_run  = true;
    else if(param == "make-only") test_run   = false;
    else if(param == "no-make")   make_run   = false;
    else if(param == "fast-run")  report_run = true;
    else if(param == "exhausted-run")   { exhausted_run    = true; report_run = true;}
    else if(param == "print-trace") {trace_run = true; }
    else {std::cout << "The scheduler has no "<< param << param << " option" << std::endl;}
  }

  // read the configure file
  if(!read_json(config_db, "configure.json", false)) return 1;

  // potentially read the results.json
  if(cond_run) {
    if(file_exist("results.json") && !read_json(result_db, "results.json", false)) return 1;
    if(file_exist("variables.json") && !read_json(result_db, "variables.json", false)) return 1;
  }

  org_env = envp;
  add_extra_run_prefix(envp);

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

long long get_file_size(const std::string& filename)
{
    std::ifstream in(filename, std::ios::binary);
    return in.rdbuf() -> pubseekoff(0, std::ios::end, std::ios::in);
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

str_list_t& append_str_list(str_list_t &l, const char *str_const) {
  char str[256];
  strcpy(str, str_const);
  char *pch = strtok(str, " ");
  while(pch != NULL) {
    l.push_back(std::string(pch));
    pch = strtok(NULL, " ");
  }
  return l;
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
  for(auto record: result_db.get<std::map<std::string, json> >()){
    report_file << setw(70) << left << record.first << " result: ";
    report_file << setw(15) << left << std::to_string((int)record.second["result"]);
    report_file << " make time: " << record.second["make-time"] << " run-time: " << record.second["run-time"] << " microseconds";
    report_file << "file_size:" << record.second["file-size"] << " bytes" << std::endl;
  }

  // record test time
  report_file << "Compilation time: " << make_time_count << " microseconds" << std::endl;
  report_file << "Run time: " << run_time_count << " microseconds" << std::endl;
  report_file.close();  
}

std::list<std::string> collect_case_list() {
  std::list<std::string> rv;
  for(auto record: config_db.get<std::map<std::string, json> >())
    if(!result_db.count(record.first)) rv.push_back(record.first);
  return rv;
}

int case_parser(const std::string& cn, std::string& pn, str_llist_t& arg_list, str_list_t& vn,
                str_list_t &dbf, std::set<int> &expect_results, std::set<int> &retry_results) {
  // check whether the case exist
  if(!config_db.count(cn)) {
    std::cerr << "Fail to parse test case " << cn << std::endl;
    return -1;
  }

  auto tcase = config_db[cn];

  // get the program name
  if(tcase.count("program"))
    pn = tcase["program"].get<std::string>();
  else
    pn = cn;

  // check requirement
  bool use_default_option = false;
  int req_case = 0;
  std::string req_case_str = "0";
  if(tcase.count("require")) {
    bool req_case_all_tested = true;
    bool req_case_tested = false;
    bool req_case_ok = false;
    do {
      req_case_str = std::to_string(req_case++);
      if(!tcase["require"].count(req_case_str)){
        req_case = std::stoi(req_case_str) - 1;
        req_case_str = std::to_string(req_case);
        break;
      }
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
    if(!req_case_ok){
      if(!exhausted_run){
        return 1024;
      }else{
        use_default_option = true;
      }
    }// no need to test as all prerequisit tested and failed
  }

  // get the argument lists
  arg_list.clear();
  arg_list.push_back(str_list_t());
  if(tcase.count("arguments") && !use_default_option) {
    auto arguments = tcase["arguments"][req_case_str].get<str_list_t>();
    for(auto arg : arguments) {
      add_arguments(arg,tcase,arg_list);
    }
  }else if(tcase.count("default_arguments") && use_default_option) {
    auto arguments = tcase["default_arguments"].get<str_list_t>();
    for(auto arg : arguments) {
      add_arguments(arg,tcase,arg_list);
    }
  }

  // check whether the case store a global variable
  if(tcase.count("set-var") && tcase["set-var"].count(req_case_str)) {
    vn = tcase["set-var"][req_case_str].get<str_list_t>();
  } else
    vn.clear();

  //dbf's size should be 2,
  //one is for current searched func name, the other is for target code name
  if(tcase.count("get-code-offset") && tcase["get-code-offset"].count(req_case_str)){
    dbf = tcase["get-code-offset"][req_case_str].get<str_list_t>();
  }else{
    dbf.clear();
  }

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

  make_config_macro = "";
  if(tcase.count("conf-macro")){
    std::map<std::string, std::string> conf_args;
    if(use_default_option){
      conf_args = tcase["conf-macro"]["0"].get<std::map<std::string,std::string>>();
    }else{
      conf_args = tcase["conf-macro"][req_case_str].get<std::map<std::string,std::string>>();
    }
    for(auto ca:conf_args){
      make_config_macro = make_config_macro + " " + ca.first + "=" + ca.second;
    }
  }

  //std::cout << "\nrun " << cn << " with requirements case " << req_case_str << std::endl;
  return 0;
}

void add_arguments(std::string arg, nlohmann::ordered_json tcase, str_llist_t &arg_list){
      if(arg.size() >= 3) {
        auto atype = arg.substr(0,2);
        if(atype == "-r") { // range
          auto range_name = arg.substr(2);
          if(tcase.count(range_name)) {
            auto range_value = tcase[range_name].get<std::vector<int> >();
            auto arg_list_size_org = arg_list.size();
            for(int i=0; i<arg_list_size_org; i++) {
              auto ale = arg_list.front();
              arg_list.pop_front();
              for(int r=range_value[0]; r<range_value[1]; r+=range_value[2]) {
                auto new_ale = str_list_t(ale);
                new_ale.push_back(std::to_string(r));
                arg_list.push_back(new_ale);
              }
            }
          } else // fallback
            for(auto &ale : arg_list) ale.push_back(arg);
        } else if(atype == "-l") { // list
          auto list_name = arg.substr(2);
          if(tcase.count(list_name)) {
            auto list_value = tcase[list_name].get<std::vector<int> >();
            auto arg_list_size_org = arg_list.size();
            for(int i=0; i<arg_list_size_org; i++) {
              auto ale = arg_list.front();
              arg_list.pop_front();
              for(auto v: list_value) {
                auto new_ale = str_list_t(ale);
                new_ale.push_back(std::to_string(v));
                arg_list.push_back(new_ale);
              }
            }
          } else // fallback
            for(auto &ale : arg_list) ale.push_back(arg);
        } else if(atype == "-v") { // variable
          auto var_name = arg.substr(2);
          if(var_db.count(var_name))
            for(auto &ale : arg_list) ale.push_back(std::to_string(var_db[var_name].get<int>()));
          else // fallback
            for(auto &ale : arg_list) ale.push_back(arg);
        }
        else // other, fallback
          for(auto &ale : arg_list) ale.push_back(arg);
      } else
        for(auto &ale : arg_list) ale.push_back(arg);
}

#ifdef _MSC_VER
// Windows MSVC
int run_cmd(char* argv[], char** runv, long long& time_count) {

  auto start = std::chrono::high_resolution_clock::now();
  auto pid = _spawnvp(_P_NOWAIT, argv[0], argv);
  auto stop = std::chrono::high_resolution_clock::now();
  time_count = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
  std::cout << "The time consumed by current process is " << time_count << " microseconds" << std::endl;

  if (pid == -1) {
    std::cerr << "Fail to spawn the executable!" << std::endl;
    exit(1);
  }

  int status;
  if (_cwait(&status, pid, _WAIT_CHILD) == -1) {
    #include <errno.h>
    errno_t err;
    _get_errno(&err);
    std::cerr << "errno = " << err;
    std::cerr << "ECHILD =" << ECHILD << std::endl;
    std::cerr << "EINVAL =" << EINVAL << std::endl;
    std::cerr << "Cannot wait for the executable!" << std::endl;
    exit(1);
  }

  if (status != 0)
    std::cout << "Terminated with code " << status << std::endl;

  return status;
}

#else
// POSIX version (Linux variant)
int run_cmd(char *argv[], char **runv, long long& time_count) {
  pid_t pid;
  if(runv == NULL) runv = org_env;
  /* debug env
  std::cout << "environment:";
  int envi = 0;
  while(runv[envi] != NULL) std::cout << std::string(runv[envi++]) << std::endl;
  std::cout << std::endl;
  */

  /*
  int argi = 0;
  while(argv[argi] != NULL) std::cout << std::string(argv[argi++]) << std::endl;
  */
  auto start = std::chrono::high_resolution_clock::now();
  int rv = posix_spawnp(&pid, argv[0], NULL, NULL, argv, runv);
  auto stop = std::chrono::high_resolution_clock::now();
  time_count = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
  std::cout << "The time consumed by current process is " << time_count << " microseconds" << std::endl;

  if(rv) {
    if(rv == ENOSYS) {
      std::cerr << "posix_spawn() is NOT supported in this system!" << std::endl;
      exit(1);
    } else {
      return rv;
    }
  }

  int s, status;
  do {
     s = waitpid(pid, &status, WUNTRACED | WCONTINUED);
     if(s == -1) {
       std::cerr << "waitpid() is NOT supported in this system!" << std::endl;
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

#endif

char ** argv_conv(const std::string &cmd, const str_list_t &args) {
  int i = 0;
  strcpy(arg_pool[i++], cmd.c_str());
  for(const auto a:args) strcpy(arg_pool[i++],a.c_str());
  for(int j=0; j<i; j++) gargv[j]=arg_pool[j];
  gargv[i] = NULL;
  return gargv;
}

bool run_tests(std::list<std::string> cases) {
  //check current test dependency and avoid endless loop
  int current_test_checkdep_count = 0;
  size_t total_cases = cases.size();
  std::cout << "The num of all cases is: " << total_cases << std::endl;
  std::string prog, cmd;
  str_llist_t alist;
  str_list_t gvar;
  std::set<int> expect_results, retry_results, rvs;
  str_list_t dbvar;
  while(!cases.empty()) {
    auto cn = cases.front();
    cases.pop_front();
    //std::cerr << "case: " << cn << std::endl; // keep in case needed in debug
      std::cout << "\n========== " << cn << " =========" << std::endl;
    int test_cond = case_parser(cn, prog, alist, gvar, dbvar, expect_results, retry_results);
    if(!test_run || test_cond == 0) {
      std::cout << "\n========== " << cn << " =========" << std::endl;
      int rv = 0;
      rvs.clear();
      if(0 == rv && make_run) {
        long long curr_time, curr_size;
        if(!make_config_macro.empty()){
          if(!trace_run){
            std::cout << "make -B test/" << prog << make_config_macro << std::endl;
            rv = run_cmd(argv_conv("make", str_list_t(1, "-B test/" + prog + make_config_macro)), NULL, curr_time);
          }else{
            std::cout << "make -B test/" << prog << make_config_macro << " TRACE_RUN=1 " << std::endl;
            rv = run_cmd(argv_conv("make", str_list_t(1, "-B test/" + prog + make_config_macro + " TRACE_RUN=1")), NULL, curr_time);
          }
        }else{
          if(!trace_run){
            std::cout << "make test/" << prog << std::endl;
            rv = run_cmd(argv_conv("make", str_list_t(1, "test/" + prog)), NULL, curr_time);
          }else{
            std::cout << "make test/" << prog << "TRACE_RUN=1" << std::endl;
            rv = run_cmd(argv_conv("make", str_list_t(1, "test/" + prog + " TRACE_RUN=1")), NULL, curr_time); 
          }

        }
        make_time_count += curr_time;
        result_db[cn]["make-time"] = curr_time;
        if(rv){
          std::cout << "fail to make " << prog << " with error status " << rv << std::endl;
          rv = -1;
          curr_size = 0;
        }else{
          #ifdef _MSC_VER
          curr_size = get_file_size("test/" + prog + ".exe");
          if(curr_size == -1) curr_size = get_file_size("test/" + prog);
          #else
          curr_size = get_file_size("test/" + prog);
          #endif
        }
        result_db[cn]["file-size"] = curr_size;
      }
      #ifdef _MSC_VER
      long long curr_script_time = 0;
      if(0 == rv && !dbvar.empty()){
        if(dbvar.size() != 2){
          std::cerr << "dbvar size is " << dbvar.size() << std::endl;
          std::cerr << "the parameter number is wrong (exactly is 2)" << std::endl;
        }
        std::cout << "dump bin: " << "script\\msvc_get_addroffset_of_currfunc.bat" << "test/" << prog << ".exe " <<
                     " " << dbvar.front() << " " << dbvar.back() << std::endl;
        rv = run_cmd(argv_conv("script\\msvc_get_addroffset_of_currfunc.bat", str_list_t{
                              "test/" + prog +".exe", dbvar.front(), dbvar.back()}), NULL, curr_script_time);
      }
      #endif

      if(0 == rv && test_run) { // run the test case
        for(auto arg:alist) {
          long long curr_time;
          cmd = "test/" + prog;
          std::cout << "\n";
          if(!extra_run_prefix.empty()) for(auto a:extra_run_prefix) std::cout << std::string(a) << " ";
          std::cout << cmd; for(auto a:arg) std::cout << " " << a; std::cout << std::endl;
          rv = run_cmd(argv_conv(cmd, arg), run_env, curr_time);
          #ifdef _MSC_VER
          curr_time += curr_script_time;
          #endif
          run_time_count += curr_time;
          result_db[cn]["run-time"] = curr_time;

          // record run-time parameter
          if(gvar.size() == 1 && rv >= 32 && rv < 64) { // successfully find a run-time parameter
            std::cerr << "set runtime variable " << gvar.front() << " to " << rv - 32 << std::endl;
            var_db[gvar.front()] = rv-32; dump_json(var_db, "variables.json", false);
            rv = 0;
          }

          if(!gvar.empty() && rv == 64) { // a run-time parameter recorded in a tmp file
            std::ifstream tmpf(temp_file_name(cmd, arg));
            if(tmpf.good()) {
              for(auto i =  gvar.begin(); i != gvar.end(); i++){
                int value;tmpf >> value;
                var_db[*i] = value; dump_json(var_db, "variables.json", false);
                std::cerr << "set runtime variable " << *i << " to " << value << " by reading " << temp_file_name(cmd, arg) << std::endl;
              }
              rv = 0;
              tmpf.close();
            }
          }

          if(0 == rv) break;
          else rvs.insert(rv);

          if(!expect_results.count(rv) && !retry_results.count(rv)) {
            std::cerr << "Test abnormality: " << cn << " failed with unexpected exit value " << rv << std::endl;
            if(debug_run) exit(1);
          }
        }

        if(rv != 0) {
          for(auto v:rvs) if(retry_results.count(v)) rv = v;
          for(auto v:rvs) if(expect_results.count(v)) rv = v;
          for(auto v:rvs) if(!expect_results.count(v) && !retry_results.count(v)) rv = v;
        }
      }

      result_db[cn]["result"] = rv; dump_json(result_db, "results.json", false);
      if(rv != 0 && !expect_results.count(rv)) {
        std::cerr << "Test abnormality: " << cn << " failed with unexpected exit value " << rv << std::endl;
        if(debug_run) exit(1);
      }
      current_test_checkdep_count = 0;
    } else if(test_run && test_cond == 1)
      if(current_test_checkdep_count < total_cases){
        cases.push_back(cn);
        current_test_checkdep_count++;
      }else{
        std::cerr << "Test abnormality: impossible to resolve the dependencies for the following test cases:" << std::endl;
        for(auto tcase: cases) std::cerr << tcase << std::endl;
        if(debug_run) exit(1);
      }
    else if(test_run && test_cond == -1) {
        std::cerr << "Test abnormality: " << cn << " does not exist in the configure.json file." << std::endl;
        if(debug_run) exit(1);
    } else if(test_run) {
      result_db[cn]["result"] = test_cond; dump_json(result_db, "results.json", false);
      current_test_checkdep_count = 0;
    }
  }
  return true;
}

void add_extra_run_prefix(char **envp) {
  // find the end of the environment
  int envi = 0;
  while(envp[envi] != NULL) envi++;

  // copy it locally
  static std::vector<char *> run_env_vect(envi + 8);
  for(int i=0; i<envi; i++) run_env_vect[i] = envp[i];

#ifdef RUN_PREFIX
  std::cout << "Need to add extra evironment variables" << std::endl;

  // assuming directly adding the environ array would not overflow
  char *pch = strtok(run_prefix, " ");
  while(pch != NULL) {
    if(run_env_vect.size() == envi) run_env_vect.resize(envi+8);
    run_env_vect[envi++] = pch;
    extra_run_prefix.push_back(pch);
    std::cout << "add " << std::string(pch) << " to env" << std::endl;
    pch = strtok(NULL, " ");
  }
  if(run_env_vect.size() == envi) run_env_vect.resize(envi+8);
  run_env_vect[envi] = NULL;
#endif

  run_env = &(run_env_vect[0]);
}
