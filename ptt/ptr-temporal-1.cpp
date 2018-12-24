#include "include/gcc_builtin.hpp"
#include "include/ptt.hpp"

volatile unsigned long addr;
int helper()
{
    volatile charBuffer *buffer = new charBuffer;   // volatile to avoid compiler optimization
    char_buffer_init(buffer);
    addr=(unsigned long)buffer;
	delete buffer;
	
	if((unsigned long)buffer == addr)
		return 0;
	else 
		return 1;

}


int main()
{
	return helper();
}