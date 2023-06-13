#include <cstdlib>
#include <string>
#include "include/global_var.hpp"

#define RA_POS_TEST_ENTRY(off, rv)                     \
  READ_STACK_DAT_IMM(ra_stack, off);                   \
  if(ra_stack == ra_label) { gvar_init(rv); return; }  \

const int max_gv_num = 4;
volatile arch_int_t gv[max_gv_num];

// function type: void return (v), 1 pointer argument (p), 0 global variable accessed inside (g0)
void FORCE_NOINLINE helper_v_p_g0(void *ra_label) {
  volatile void *ra_stack = NULL;
  gvar_init(-2);
  RA_POS_TEST;
  gvar_init(-1);
  return;
}

// function type: void return (v), 1 pointer argument (p), 1 global variable accessed inside (g0)
void FORCE_NOINLINE helper_v_p_g1(void *ra_label) {
  volatile void *ra_stack = NULL;
  gvar_init(-2-gv[0]);
  /* HiFive Unmatched, GCC 11.2.0
   * Make sure the global variable is modified as otherwise
   * the stack is not expanded as expected.
   */
  gv[0] = rand();
  RA_POS_TEST;
  gvar_init(-1);
  return;
}

/* x86-64 GCC 7.5.0
 * Really odd problem!
 * If using if (v == 0), the compiler thinks it is likely to happen
 * and make the goto the following branch while adding a jump
 * after calling the helper function,
 * which make the RA pointing to the jump but not the RA_POS!
 * Making if (v == -1) disable the compiler guess?!
 */
#define TEST_FUNC(FT)              \
int test_##FT(int v) {             \
  void *ra_label = &&RA_POS;       \
  if(v == -1) goto *ra_label;      \
  helper_##FT(ra_label);           \
  COMPILER_BARRIER;                \
 RA_POS:                           \
  if(gvar() < 0) return 1;         \
  return gvar() + 32;              \
}                                  \

TEST_FUNC(v_p_g0)
TEST_FUNC(v_p_g1)

int main(int argc, char* argv[])
{
  for(int i=0; i<max_gv_num; i++) gv[i] = rand();
  int v = argv[1][0] - '0';
  std::string ftype(argv[2]);
  if(ftype == "v-p-g0") return test_v_p_g0(v);
  if(ftype == "v-p-g1") return test_v_p_g1(v);
}

