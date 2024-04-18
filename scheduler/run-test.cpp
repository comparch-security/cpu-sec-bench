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

long long total_make_time = 0, total_run_time = 0;

// test case related
char **org_env;
char **run_env;
void add_extra_run_prefix(char **);
#ifdef RUN_PREFIX
  static char run_prefix[] = RUN_PREFIX;
#endif
std::list<char *> extra_run_prefix;
std::list<std::string> collect_case_list();
typedef std::list<std::string> str_list_t;
typedef std::list<str_list_t>  str_llist_t;
typedef std::vector<str_llist_t> str_vllist_t;// this strcture is used for various arguments cases
str_list_t make_config_macro;
bool requires_check(nlohmann::ordered_json tcase);
int case_parser(const std::string& cn, nlohmann::ordered_json tcase, int ind, std::string& pn, str_list_t& vn,
                str_list_t& dbf, std::set<int> &expect_results);
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
    else {std::cout << "The scheduler has no "<< param << " option" << std::endl; exit(1);}
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

  // insert sys infomation to the result file
  std::ifstream sys_info_file("test/sys_info.txt");

  std::string resfile_name = "results.dat";
  std::string str_buf;
  
  std::getline(sys_info_file, str_buf);
  size_t pos = str_buf.find("OVERVIEW:");

  if (pos != std::string::npos) {
      resfile_name = str_buf.substr(pos + sizeof("OVERVIEW:")-1) + ".dat"; // Extract the string after "CPU:"
  }

  std::ofstream report_file(resfile_name);
  //read the rest lines of sys_info.txt
  while(getline(sys_info_file,str_buf)) {
    // change below statement to use string parameter
    report_file << "# " << str_buf << std::endl;
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
  report_file << "Compilation time: " << total_make_time << " microseconds" << std::endl;
  report_file << "Run time: " << total_run_time << " microseconds" << std::endl;
  report_file.close();  
}

std::list<std::string> collect_case_list() {
  std::list<std::string> rv;
  for(auto record: config_db.get<std::map<std::string, json> >())
    if(!result_db.count(record.first)) rv.push_back(record.first);
  return rv;
}

void arg_parser(nlohmann::ordered_json tcase, str_vllist_t& arg_lists){
  // get the argument lists
  arg_lists.clear();
  std::string case_kind;
  if(!exhausted_run) {
    if(tcase.count("arguments")){
      case_kind = "arguments";
    }
  }else if(exhausted_run) {
    if(tcase.count("default_args"))
      case_kind = "default_args";
    else if(tcase.count("arguments")){
      case_kind = "arguments";
    }
  }
  int ind = 0;
  if(debug_run)std::cout << "----case kind is: " << case_kind << "----"<< std::endl;
  if(tcase[case_kind].empty()){
    if(debug_run)std::cout << "argument is empty" << std::endl;
    arg_lists.emplace_back(str_llist_t());
    arg_lists[0].emplace_back(str_list_t());
  }else{
    for(auto curr_case = tcase[case_kind].begin(); curr_case != tcase[case_kind].end(); curr_case++){
      if(debug_run)std::cout << "curr case is: " << (*curr_case) << std::endl;
      arg_lists.emplace_back(str_llist_t());
      arg_lists[ind].emplace_back(str_list_t());
      auto arguments = (*curr_case).get<str_list_t>();
      for(auto arg : arguments) {
        add_arguments(arg,tcase,arg_lists[ind]);
      }
      ind++;
    }
  }

}

bool requires_check(nlohmann::ordered_json tcase){
  //check whether all ind's requirement cases are tested.
  if(exhausted_run)
    return true;
  if(tcase.count("require")) {
    auto requires_list = tcase["require"];
    for(auto require:requires_list){
      if(require.empty()) break;
      auto require_list = require.get<str_llist_t>();
      for(auto and_conds : require_list) {
        for(auto or_cond : and_conds) {
          if(!result_db.count(or_cond)) {
            if(debug_run) std::cout << "has untested require tests: " << or_cond << std::endl;
            return false;
          }
        }
      }
    }
  }
  return true;
}

int case_parser(const std::string& cn, nlohmann::ordered_json tcase, int ind, std::string& pn, str_list_t& vn,
                str_list_t &dbf, std::set<int> &expect_results) {

  // get the program name
  if(tcase.count("program"))
    pn = tcase["program"].get<std::string>();
  else
    pn = cn;

  // check whether the ind's corresponding case's result is ok

  if(tcase.count("require")  && !exhausted_run && !tcase["require"][std::to_string(ind)].empty()) {

    if(debug_run)std::cout << "reuire case ind is: " << ind << std::endl;
    auto require_list = tcase["require"][std::to_string(ind)].get<str_llist_t>();

    for(auto and_conds : require_list) {
      bool has_passed_case = false;
      for(auto or_cond : and_conds) {
        if(debug_run)std::cout << "or_cond is: " << or_cond << std::endl;
        if(debug_run)std::cout << "result_db[or_cond] is: " << result_db[or_cond] << std::endl;
        if(result_db[or_cond]["result"] == 0) {
          has_passed_case = true;
        }else{
          if(config_db.count(or_cond) && config_db[or_cond].count("expect-results")) {
            for(auto r: config_db[or_cond]["expect-results"].get<std::map<std::string, json> >()) {
              if(result_db[or_cond]["result"] ==  std::stoi(r.first)){
                has_passed_case = true;
              }
            }
          }
        }
      }
      if(!has_passed_case) return 1024;
    }

    if(debug_run)std::cout << "\nrun " << cn << " with requirements case " << ind << std::endl;
  }

  // check whether the case store a global variable
  if(tcase.count("set-var") && tcase["set-var"].count(std::to_string(ind))) {
    vn = tcase["set-var"][std::to_string(ind)].get<str_list_t>();
  } else
    vn.clear();

  //dbf's size should be 2,
  //one is for current searched func name, the other is for target code name
  if(tcase.count("get-code-offset") && tcase["get-code-offset"].count(std::to_string(ind))){
    dbf = tcase["get-code-offset"][std::to_string(ind)].get<str_list_t>();
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

  make_config_macro.clear();
  if(tcase.count("conf-macro")){
    std::map<std::string, std::string> conf_args;
    //Currently, only the case of a single conf macro is being considered
    conf_args = tcase["conf-macro"][std::to_string(ind)].get<std::map<std::string,std::string>>();
    for(auto ca:conf_args){
      make_config_macro.push_back(ca.first + "=" + ca.second);
    }
  }

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
  }else{
    auto stop = std::chrono::high_resolution_clock::now();
    time_count = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
    std::cout << "The time consumed by current process is " << time_count << " microseconds" << std::endl;
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
  if(debug_run){
    std::cout << "environment:";
    int envi = 0;
    while(runv[envi] != NULL) std::cout << std::string(runv[envi++]) << std::endl;
    std::cout << std::endl;
  }

  /*
  int argi = 0;
  while(argv[argi] != NULL) std::cout << std::string(argv[argi++]) << std::endl;
  */
  auto start = std::chrono::high_resolution_clock::now();
  int rv = posix_spawnp(&pid, argv[0], NULL, NULL, argv, runv);

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

  auto stop = std::chrono::high_resolution_clock::now();
  time_count = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
  if(debug_run) std::cout << "The time consumed by current process is " << time_count << " microseconds" << std::endl;

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
  str_vllist_t alists;
  str_list_t gvar;
  str_list_t dbvar;
  while(!cases.empty()) {

    auto cn = cases.front();
    cases.pop_front();

    // check whether the case exist
    if(debug_run && !config_db.count(cn)) {
      std::cerr << "Fail to parse test case " << cn << std::endl;
      std::cerr << "Test abnormality: " << cn << " does not exist in the configure.json file." << std::endl;
      exit(1);
    }

    bool has_make = false;
    int test_cond = 0;
    auto tcase = config_db[cn];
  
    std::set<int> expect_results, make_results, test_results;
    arg_parser(tcase,alists);

    if(debug_run)std::cout << "alists.size() is: " << alists.size() << std:: endl;

    if (requires_check(tcase) == true){
      std::cerr << "\n========== start: " << cn << " =========" << std::endl;
      long long case_exec_time_sum = 0;
      for(int ind = 0; ind != alists.size(); ind++){
        if(debug_run) std::cout << "\"arguments\" ind is: " << ind << std::endl;
        auto alist = alists[ind];
        test_cond = case_parser(cn, tcase, ind, prog, gvar, dbvar, expect_results);
        if(!test_run || test_cond == 0) {
          if(debug_run)std::cout << "\n------ " << cn << " ------" << std::endl;
          int make_result = 0;
          if(make_run && !has_make) {
            long long one_make_time, curr_size;
            if(!make_config_macro.empty()){
              make_config_macro.push_front("test/" + prog);
              make_config_macro.push_front("-B");
              if(trace_run){
                make_config_macro.push_back("TRACE_RUN=1");
              }
              std::cout << "make";
              for(auto str:make_config_macro){
                std::cout << " " << str;
              }
              std::cout << std::endl;
              make_result = run_cmd(argv_conv("make", make_config_macro), NULL, one_make_time);
            }else{
              if(!trace_run){
                std::cout << "make test/" << prog << std::endl;
                make_result = run_cmd(argv_conv("make", str_list_t(1, "test/" + prog)), NULL, one_make_time);
              }else{
                std::cout << "make test/" << prog << "TRACE_RUN=1" << std::endl;
                str_list_t no_make_config_macro = {"test/" + prog, "TRACE_RUN=1"};
                make_result = run_cmd(argv_conv("make", no_make_config_macro), NULL, one_make_time); 
              }

            }
            total_make_time += one_make_time;
            result_db[cn]["make-time"] = one_make_time;
            if(make_result){
              std::cout << "fail to make " << prog << " with error status " << make_result << std::endl;
              make_results.insert(make_result);
              curr_size = 0;
              continue;
            }else{
              #ifdef _MSC_VER
              curr_size = get_file_size("test/" + prog + ".exe");
              if(curr_size == -1) curr_size = get_file_size("test/" + prog);
              if(!dbvar.empty()){
                if(dbvar.size() != 2){
                  std::cerr << "dbvar size is " << dbvar.size() << std::endl;
                  std::cerr << "the parameter number is wrong (exactly is 2)" << std::endl;
                }
                  std::cout << "dump bin: " << "script\\msvc_get_addroffset_of_currfunc.bat" << "test/" << prog << ".exe " <<
                        " " << dbvar.front() << " " << dbvar.back() << std::endl;
                  make_result = run_cmd(argv_conv("script\\msvc_get_addroffset_of_currfunc.bat", str_list_t{
                                  "test/" + prog +".exe", dbvar.front(), dbvar.back()}), NULL, curr_script_time);
                  make_results.insert(make_result);
              }
              #else
              curr_size = get_file_size("test/" + prog);
              #endif
            }
            result_db[cn]["file-size"] = curr_size;
            has_make = true;
          }

          if(0 == make_result && test_run) { // run the test case
          
            for(auto arg:alist) {
              long long one_run_time;
              cmd = "test/" + prog;
              std::cout << "\n";
              if(!extra_run_prefix.empty()) for(auto a:extra_run_prefix) std::cout << std::string(a) << " ";
              std::cout << cmd; for(auto a:arg) std::cout << " " << a; std::cout << std::endl;
              int one_run_result = run_cmd(argv_conv(cmd, arg), run_env, one_run_time);
              #ifdef _MSC_VER
              one_run_time += curr_script_time;
              #endif
              total_run_time += one_run_time;
              case_exec_time_sum += one_run_time;

              // record run-time parameter
              if(gvar.size() == 1 && one_run_result >= 32 && one_run_result < 64) { // successfully find a run-time parameter
                std::cerr << "set runtime variable " << gvar.front() << " to " << one_run_result - 32 << std::endl;
                var_db[gvar.front()] = one_run_result-32; dump_json(var_db, "variables.json", debug_run);
                one_run_result = 0;
              }

              if(!gvar.empty() && (one_run_result == 64 || one_run_result ==65)) { // a run-time parameter recorded in a tmp file
                std::ifstream tmpf(temp_file_name(cmd, arg));
                if(tmpf.good()) {
                  for(auto i =  gvar.begin(); i != gvar.end(); i++){
                    int value;tmpf >> value;
                    var_db[*i] = value; dump_json(var_db, "variables.json", debug_run);
                    std::cerr << "set runtime variable " << *i << " to " << value << " by reading " << temp_file_name(cmd, arg) << std::endl;
                  }
                  if(one_run_result==64)
                    one_run_result = 0;
                  else{
                    one_run_result = 2;
                  }
                  tmpf.close();
                }
              }

              test_results.insert(one_run_result);
              
              if(one_run_result == 0){
                goto FINISH_CURRENT_CASE;
              }else if(!expect_results.count(one_run_result)) {
                std::cerr << "Try test failed: " << cn;
                std::cerr << " failed with unexpected exit value " << one_run_result << std::endl;
              }
            } // one program repeated("r" option) loop
          }
        }else if(test_run && test_cond == 1024) {
          std::cerr << "\n------ " << cn << " ------" << std::endl;
          std::cerr << "Required case failed: " << cn << ", so curr case failed with unexpected exit value " << test_cond << std::endl;
          test_results.insert(1024);
        }

      }// one case diff arg/macro program(arg_list) loop
FINISH_CURRENT_CASE:
      int test_result = 0;
      if(!make_results.empty()){ // not succcessfully build, select first failed make result
        test_result = *make_results.cbegin();
        std::cerr << "Built all failed, the first try error code:: " << test_result << std::endl;
      }else{// successfully build
        if(!test_results.count(0)){ // hasn't successfull result, check expected test result
          for(auto v:test_results){
            if(expect_results.count(v)){
              test_result = v;
              break;
            }
          }
          if(!expect_results.count(test_result)){// hasn't expected result, select first failed test result
            test_result = *test_results.cbegin();
            std::cerr << "Run abnormality: " << cn << " failed with unexpected exit value " << test_result << std::endl;
          }else{
            std::cerr << "Run expected result: " << cn << " failed with expected exit value " << test_result << std::endl;
          }
        }
      }

      current_test_checkdep_count = 0;
      result_db[cn]["result"] = test_result;
      result_db[cn]["run-time"] = case_exec_time_sum;
      dump_json(result_db, "results.json", debug_run);
      std::cerr << "\n========== end: " << cn << " =========" << std::endl;
    }else{ // dependencies have not test yet
      if(current_test_checkdep_count < total_cases){
        current_test_checkdep_count++;
        std::cerr << "push the curr case, and go to check the next argument's corresponding requirement" << std::endl;
        cases.push_back(cn);
      }else{
            std::cerr << "Parse abnormality: impossible to resolve the dependencies(may exists rings) for the following test cases:" << std::endl;
            std::cerr << "**************************************" << std::endl;
            while(!cases.empty()){
              auto tcase = cases.front();
              cases.pop_front();
              std::cerr << tcase << std::endl;
            }
            exit(1);
      }
    }
  }// each case loop
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
