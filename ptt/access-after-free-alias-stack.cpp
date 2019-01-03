volatile char *buffer;

void helper() {
  volatile char tmp[16];
  for(int i=0; i<16; i++) tmp[i] = 'd';
  buffer = tmp;
}

int main() {
  helper();
  for(int i=0; i<16; i++) // except if pionter temporal protection exists
    if(buffer[i] == 'd') return 0;
  return 1;
}
