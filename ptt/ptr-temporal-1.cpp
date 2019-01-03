#include <cstdlib>

int main() {
  char *buffer = (char *)malloc(16);
  for(int i=0; i<16; i++) buffer[i] = 'd';
  free(buffer);

  for(int i=0; i<16; i++) // except if pionter temporal protection exists
    if(buffer[i] == 'd') return 0;
  return 1;
}
