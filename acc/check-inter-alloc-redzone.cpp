#include "include/assembly.hpp"
#include <cstdlib>
#include <map>
#include "include/temp_file.hpp"

#define MIN_OBJ_TEST 16
#define MIN_DIST 4
#define MAX_OBJ_SIZE 256

int main(int argc, char* argv[])
{
  std::map<char *, int> obj_map;
  long long min_dist = -1, min_dist_prev = 0;

  while(min_dist_prev != min_dist) {
    min_dist_prev = min_dist;
    for(int i=0; i<MIN_OBJ_TEST; i++) {
      int obj_size = (rand() % 128) + 1;
      char *p = new char[obj_size];
      if(obj_map.size() > 1)
        for(auto m:obj_map) {
          auto pa = (unsigned long long)m.first;
          auto pp = (unsigned long long)p;
          auto s  = pa > pp ? obj_size : m.second;
          long long d  = llabs(pa - pp) - s;
          if(min_dist < 0 || d < min_dist) min_dist = d;
        }
      obj_map[p] = obj_size;
    }
  }

  for(auto m:obj_map) delete[] m.first;

  write_to_temp_file(min_dist, argc, argv);
  return 64;
}
