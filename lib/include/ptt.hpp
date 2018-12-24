#ifndef PTT_HPP_INCLUDED
#define PTT_HPP_INCLUDED

struct charBuffer
{
	char data[16];
};

# if __WORDSIZE == 64
typedef uint64_t pointer_t;
#endif
# if __WORDSIZE == 32
typedef uint32_t pointer_t;
#endif


extern void char_buffer_init(volatile charBuffer *);

#endif