#include "include/gcc_builtin.hpp"
#include "include/ptt.hpp"

volatile charBuffer *t;
volatile unsigned long addr;

int helper()
{
    volatile charBuffer *buffer = new charBuffer;   // volatile to avoid compiler optimization
    char_buffer_init(buffer);
    t = buffer;
    addr=(unsigned long)t;
	delete buffer;

	if((unsigned long)t == addr)  //if pionter temporal protection exist ,it will throw exception here
		return 0;
    else
		return 1;

}


int main()
{
	return helper();
}