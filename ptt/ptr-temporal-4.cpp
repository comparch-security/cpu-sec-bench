#include "include/gcc_builtin.hpp"
#include "include/ptt.hpp"

int FORCE_NOINLINE helper()
{
	unsigned int i = 0;
	volatile charBuffer * t;
	volatile charBuffer *a = new charBuffer;
    char_buffer_init(a);
	t = a;
	delete a;

	volatile charBuffer *b = new charBuffer;
    char_buffer_init(b);

	for(;i<16;i++)                     //if pionter temporal protection exist ,it will throw exception here
	{
		if(t->data[i] == 'd') {
			delete b;
			return 0;
		}
	}
	if(i == 15){
		delete b;
		return 1;
	}
	delete b;
  	return 1;


}


int main()
{
	return helper();
}