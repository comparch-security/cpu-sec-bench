#include "include/gcc_builtin.hpp"
#include "include/ptt.hpp"

int FORCE_NOINLINE helper()
{
	unsigned int i = 0;
    volatile charBuffer *buffer = new charBuffer;   // volatile to avoid compiler optimization
    char_buffer_init(buffer);
	delete buffer;
	for(;i<16;i++)                     //if pionter temporal protection exist ,it will throw exception here
	{
		if(buffer->data[i] == 'd') 
			return 0;
	}
	if(i == 15)
		return 1;
	return -1;
}


int main()
{
	return helper();
}