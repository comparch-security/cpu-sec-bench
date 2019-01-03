#include <cstdlib>

char *buffer;

int main() {
  char *tmp = (char *)malloc(16);
  buffer = tmp;
  for(int i=0; i<16; i++) tmp[i] = 'd';
  free(tmp);

  for(int i=0; i<16; i++) // except if pionter temporal protection exists
    if(buffer[i] == 'd') return 0;
  return 1;
}

