#include <cstdlib>
#include <cstring>
#define STACK  0
#define HEAP   1
#define DATA   2
#define RODATA 3

#define REGION_KIND_STR_0 stack
#define REGION_KIND_STR_1 heap
#define REGION_KIND_STR_2 data
#define REGION_KIND_STR_3 rodata

#define BARE_ARRAY      0
#define ENCLOSING_ARRAY 1
#define SYMM_ARRAY      2
#define LARGE_ARRAY     3 

#define BUFFER_KIND_STR_0 bare_array
#define BUFFER_KIND_STR_1 enclosing_array
#define BUFFER_KIND_STR_2 symm_array
#define BUFFER_KIND_STR_3 large_array

#define STR_(X) #X
#define STR(x) STR_(x)
#define STR2_(X,Y) X ## Y
#define STR2(X,Y) STR2_(X,Y)
#define STR2UL(X,Y) STR2(STR2(X,_),Y)
#define STR2BR_(X,Y) X[Y]
#define STR2BR(X,Y) STR2BR_(X,Y)
#define RSTR(x) STR2(REGION_KIND_STR_,x)
#define BSTR(x) STR2(BUFFER_KIND_STR_,x)
#define MEMBEROP_(X,Y) X.Y
#define MEMBEROP(X,Y) MEMBEROP_(X,Y)
#define ARROW_(X,Y) X->Y
#define ARROW(X,Y) ARROW_(X,Y)
#define DEREF_(X) X
#define DEREF(X) DEREF_(X)

#define HEAPTR_TO_ARRAYREF_(X) X
#define HEAPTR_TO_ARRAYREF(X) HEAPTR_TO_ARRAYREF_(X)

// 117 is ascii code of "u"
#define VAL_1U     117
#define VAL_2U     VAL_1U,  VAL_1U
#define VAL_4U     VAL_2U,  VAL_2U
#define VAL_8U     VAL_4U,  VAL_4U
#define VAL_16U    VAL_8U,  VAL_8U
#define VAL_32U    VAL_16U, VAL_16U
#define VAL_64U    VAL_32U, VAL_32U

// 100 is ascii code of "d"
#define VAL_1D     100
#define VAL_2D     VAL_1D,  VAL_1D
#define VAL_4D     VAL_2D,  VAL_2D
#define VAL_8D     VAL_4D,  VAL_4D
#define VAL_16D    VAL_8D,  VAL_8D
#define VAL_32D    VAL_16D, VAL_16D
#define VAL_64D    VAL_32D, VAL_32D

// 111 is ascii code of "o"
#define VAL_1O     111
#define VAL_2O     VAL_1O,  VAL_1O
#define VAL_4O     VAL_2O,  VAL_2O
#define VAL_8O     VAL_4O,  VAL_4O
#define VAL_16O    VAL_8O,  VAL_8O
#define VAL_32O    VAL_16O, VAL_16O
#define VAL_64O    VAL_32O, VAL_32O

#ifndef BUFFER_SIZE
    #define BUFFER_SIZE 8
#endif

#if  !defined(BUFFER_VAL_UNDERFLOW) && !defined(BUFFER_VAL_MID) && !defined(BUFFER_VAL_OVERFLOW)
#define BUFFER_VAL_UNDERFLOW  VAL_4U,VAL_2U,VAL_1U
#define BUFFER_VAL_MID        VAL_4D,VAL_2D,VAL_1D
#define BUFFER_VAL_OVERFLOW   VAL_4O,VAL_2O,VAL_1O
#endif

class charBuffer
{
public:
char underflow[BUFFER_SIZE];
char data[BUFFER_SIZE];
char overflow[BUFFER_SIZE];

charBuffer() = default;
charBuffer(const char uf, const char d, const char of);
void updateBuffer(const char uf, const char d, const char of);
};

charBuffer::charBuffer(const char uf, const char d, const char of){
for(unsigned int i=0; i!=BUFFER_SIZE-1; i++) {
    underflow[i] = uf;
    data[i] = d;
    overflow[i] = of;
}
underflow[BUFFER_SIZE-1] = 0;
data[BUFFER_SIZE-1]      = 0;
overflow[BUFFER_SIZE-1]  = 0;
}

void charBuffer::updateBuffer(const char uf, const char d, const char of){
for(unsigned int i=0; i!=BUFFER_SIZE-1; i++) {
    underflow[i] = uf;
    data[i] = d;
    overflow[i] = of;
}
underflow[BUFFER_SIZE-1] = 0;
data[BUFFER_SIZE-1]      = 0;
overflow[BUFFER_SIZE-1]  = 0;
}

class LargeMemberBufferBase
{
  public:
  virtual void updateBuffer(const char uf, const char d, const char of) = 0;
  virtual const char * get_const_underflow() const = 0;
  virtual const char * get_const_data() const = 0;
  virtual const char * get_const_overflow() const = 0;
  virtual char * get_underflow() = 0;
  virtual char * get_data() = 0;
  virtual char * get_overflow() = 0;
  virtual ~LargeMemberBufferBase() = default;
};

template<unsigned int SZ>
class LargeMemberBuffer : public LargeMemberBufferBase
{
public:
  char underflow[SZ];
  char data[SZ];
  char overflow[SZ];

  LargeMemberBuffer() = default;
  LargeMemberBuffer(const char uf, const char d, const char of) {
    for(unsigned int i=0; i!=SZ-1; i++) {
      underflow[i] = uf;
      data[i] = d;
      overflow[i] = of;
    }
    underflow[SZ-1] = 0;
    data[SZ-1]      = 0;
    overflow[SZ-1]  = 0;
  }

  virtual void updateBuffer(const char uf, const char d, const char of) {
    for(unsigned int i=0; i!=SZ-1; i++) {
      underflow[i] = uf;
      data[i] = d;
      overflow[i] = of;
    }
    underflow[SZ-1] = 0;
    data[SZ-1]      = 0;
    overflow[SZ-1]  = 0;
  }

  virtual const char * get_const_underflow() const {
    return underflow;
  }

  virtual const char * get_const_data() const {
    return data;
  }

  virtual const char * get_const_overflow() const {
    return overflow;
  }

  virtual char * get_underflow() {
    return underflow;
  }

  virtual char * get_data() {
    return data;
  }

  virtual char * get_overflow() {
    return overflow;
  }

};

template<typename tT, unsigned int CB_SIZE = BUFFER_SIZE>
class typeCastBuffer
{
public:
  tT target;
  char data[CB_SIZE];

  typeCastBuffer() = default;
  typeCastBuffer(tT t, char d)
    : target(t)
  {
    for(unsigned int i=0; i!=CB_SIZE-1; i++) data[i] = d;
    data[CB_SIZE-1] = 0;
  }
};

// We need these macros be defined: REGION_KIND,BUFFER_SIZE,BUFFER_KIND,BUFFER_VAL_UNDERFLOW,BUFFER_VAL,BUFFER_VAL_OVERFLOW

#if REGION_KIND == STACK
    #define UNINIT_BUFFER {}
    #if BUFFER_KIND == BARE_ARRAY

        #define INIT_BUFFER char stack_bare_array_underflow[] = {BUFFER_VAL_UNDERFLOW,0};   \
                            char stack_bare_array[]           = {BUFFER_VAL_MID,0};         \
                            char stack_bare_array_overflow[]  = {BUFFER_VAL_OVERFLOW,0}

    #elif BUFFER_KIND == ENCLOSING_ARRAY

        #define INIT_BUFFER charBuffer stack_enclosing_array_underflow = charBuffer('u','d','o');  \
                            charBuffer stack_enclosing_array           = charBuffer('u','d','o'); \
                            charBuffer stack_enclosing_array_overflow  = charBuffer('u','d','o')

    #elif BUFFER_KIND == SYMM_ARRAY
        #define INIT_BUFFER  char stack_p00[1],   stack_p01[1];                      \
                                char stack_p10[2],   stack_p11[2];                   \
                                char stack_p20[4],   stack_p21[4];                   \
                                char stack_p30[8],   stack_p31[8];                   \
                                char stack_p40[16],  stack_p41[16];                  \
                                char stack_p50[32],  stack_p51[32];                  \
                                char stack_p60[64],  stack_p61[64];                  \
                                char stack_p70[BUFFER_SIZE], stack_p71[BUFFER_SIZE]; \
                                char* stack_symm_array[] = {                         \
                                    stack_p00, stack_p01, stack_p10, stack_p11,      \
                                    stack_p20, stack_p21, stack_p30, stack_p31,      \
                                    stack_p40, stack_p41, stack_p50, stack_p51,      \
                                    stack_p60, stack_p61, stack_p70, stack_p71       \
                                }

    #elif BUFFER_KIND == LARGE_ARRAY

        #define INIT_BUFFER LargeMemberBuffer<0x101>  buffer_stack1('u','d','o');                          \
                            LargeMemberBuffer<0x201>  buffer_stack2('u','d','o');                          \
                            LargeMemberBuffer<0x401>  buffer_stack3('u','d','o');                          \
                            LargeMemberBuffer<0x801>  buffer_stack4('u','d','o');                          \
                            LargeMemberBuffer<0x1001> buffer_stack5('u','d','o');                          \
                            LargeMemberBuffer<0x2001> buffer_stack6('u','d','o');                          \
                            LargeMemberBuffer<0x4001> buffer_stack7('u','d','o');                          \
                            LargeMemberBuffer<0x8001> buffer_stack8('u','d','o');                          \
                            LargeMemberBuffer<BUFFER_SIZE> buffer_stack9('u','d','o');                     \
                            LargeMemberBufferBase * stack_large_array[] = {                                \
                            &buffer_stack1, &buffer_stack2, &buffer_stack3, &buffer_stack4,                \
                            &buffer_stack5, &buffer_stack6, &buffer_stack7, &buffer_stack8, &buffer_stack9 \
                            }

    #endif

#endif

#if REGION_KIND == HEAP
    #undef MEMBEROP_
    #undef MEMBEROP
    #undef DEREF_
    #undef DEREF
    #undef HEAPTR_TO_ARRAYREF_
    #undef HEAPTR_TO_ARRAYREF

    #define DEREF_(X) *X
    #define DEREF(X) DEREF_(X)
    #define MEMBEROP_(X,Y) X->Y
    #define MEMBEROP(X,Y) MEMBEROP_(X,Y)

    #define HEAPTR_TO_ARRAYREF_(X) (char(&)[BUFFER_SIZE])*X
    #define HEAPTR_TO_ARRAYREF(X) HEAPTR_TO_ARRAYREF_(X)

    #if BUFFER_KIND == BARE_ARRAY

        #define INIT_BUFFER char* heap_bare_array_underflow = (char*) malloc(sizeof(char) * BUFFER_SIZE);   \
                            char* heap_bare_array       = (char*) malloc(sizeof(char) * BUFFER_SIZE);       \
                            char* heap_bare_array_overflow  = (char*) malloc(sizeof(char) * BUFFER_SIZE);   \
                            memset(heap_bare_array_underflow, 'u', BUFFER_SIZE);                            \
                            memset(heap_bare_array, 'd' ,BUFFER_SIZE);                                      \
                            memset(heap_bare_array_overflow, 'o', BUFFER_SIZE);                             \
                            heap_bare_array_underflow[BUFFER_SIZE-1] = '\0';                                \
                            heap_bare_array[BUFFER_SIZE-1]      = '\0';                                     \
                            heap_bare_array_overflow[BUFFER_SIZE-1]  = '\0'
        #define UNINIT_BUFFER  free(heap_bare_array_underflow);  \
                               free(heap_bare_array);            \
                               free(heap_bare_array_overflow)

    #elif BUFFER_KIND == ENCLOSING_ARRAY

        #define INIT_BUFFER charBuffer *heap_enclosing_array_underflow = new charBuffer('u','d','o');  \
                            charBuffer *heap_enclosing_array           = new charBuffer('u','d','o');  \
                            charBuffer *heap_enclosing_array_overflow  = new charBuffer('u','d','o')
        #define UNINIT_BUFFER  delete heap_enclosing_array_underflow;   \
                               delete heap_enclosing_array;             \
                               delete heap_enclosing_array_overflow

    #elif BUFFER_KIND == SYMM_ARRAY
        #define INIT_BUFFER {}

    #elif BUFFER_KIND == LARGE_ARRAY

        #define INIT_BUFFER LargeMemberBufferBase * heap_large_array[] = {   \
                            new LargeMemberBuffer<0x101>('u','d','o'),       \
                            new LargeMemberBuffer<0x201>('u','d','o'),       \
                            new LargeMemberBuffer<0x401>('u','d','o'),       \
                            new LargeMemberBuffer<0x801>('u','d','o'),       \
                            new LargeMemberBuffer<0x1001>('u','d','o'),      \
                            new LargeMemberBuffer<0x2001>('u','d','o'),      \
                            new LargeMemberBuffer<0x4001>('u','d','o'),      \
                            new LargeMemberBuffer<0x8001>('u','d','o'),      \
                            new LargeMemberBuffer<BUFFER_SIZE>('u','d','o')       \
                            }
        #define UNINIT_BUFFER for(auto b:heap_large_array) delete b;

    #endif

#endif

#if REGION_KIND == DATA
    #define INIT_BUFFER {}
    #define UNINIT_BUFFER {}
    #if BUFFER_KIND == BARE_ARRAY

    char data_bare_array_underflow[] = {BUFFER_VAL_UNDERFLOW,0};
    char data_bare_array[]       = {BUFFER_VAL_MID,0};
    char data_bare_array_overflow[]  = {BUFFER_VAL_OVERFLOW,0};

    #elif BUFFER_KIND == ENCLOSING_ARRAY
        charBuffer data_enclosing_array_underflow('u','d','o');
        charBuffer data_enclosing_array('u','d','o');
        charBuffer data_enclosing_array_overflow('u','d','o');

    #elif BUFFER_KIND == SYMM_ARRAY
        char data_p00[1],   data_p01[1];
        char data_p10[2],   data_p11[2];
        char data_p20[4],   data_p21[4];
        char data_p30[8],   data_p31[8];
        char data_p40[16],  data_p41[16];
        char data_p50[32],  data_p51[32];
        char data_p60[64],  data_p61[64];
        char data_p70[BUFFER_SIZE], data_p71[BUFFER_SIZE];
        char* data_symm_array[] = {
        data_p00, data_p01, data_p10, data_p11,
        data_p20, data_p21, data_p30, data_p31,
        data_p40, data_p41, data_p50, data_p51,
        data_p60, data_p61, data_p70, data_p71};

    #elif BUFFER_KIND == LARGE_ARRAY

        LargeMemberBuffer<0x101>  buffer_data1('u','d','o');
        LargeMemberBuffer<0x201>  buffer_data2('u','d','o');
        LargeMemberBuffer<0x401>  buffer_data3('u','d','o');
        LargeMemberBuffer<0x801>  buffer_data4('u','d','o');
        LargeMemberBuffer<0x1001> buffer_data5('u','d','o');
        LargeMemberBuffer<0x2001> buffer_data6('u','d','o');
        LargeMemberBuffer<0x4001> buffer_data7('u','d','o');
        LargeMemberBuffer<0x8001> buffer_data8('u','d','o');
        LargeMemberBuffer<BUFFER_SIZE> buffer_data9('u','d','o');
        LargeMemberBufferBase * data_large_array[] = {
        &buffer_data1, &buffer_data2, &buffer_data3, &buffer_data4,
        &buffer_data5, &buffer_data6, &buffer_data7, &buffer_data8, &buffer_data9
        };

    #endif

#endif

#if REGION_KIND == RODATA
    #define INIT_BUFFER {}
    #define UNINIT_BUFFER {}
    #if BUFFER_KIND == BARE_ARRAY

    char rodata_bare_array_underflow[] = {BUFFER_VAL_UNDERFLOW,0};
    char rodata_bare_array[]       = {BUFFER_VAL_MID,0};
    char rodata_bare_array_overflow[]  = {BUFFER_VAL_OVERFLOW,0};

    #elif BUFFER_KIND == ENCLOSING_ARRAY

        const charBuffer rodata_enclosing_array_underflow('u','d','o');
        const charBuffer rodata_enclosing_array('u','d','o');
        const charBuffer rodata_enclosing_array_overflow('u','d','o');

    #elif BUFFER_KIND == SYMM_ARRAY 
        const char rodata_p00[1]   = {}, rodata_p01[1]   = {};
        const char rodata_p10[2]   = {}, rodata_p11[2]   = {};
        const char rodata_p20[4]   = {}, rodata_p21[4]   = {};
        const char rodata_p30[8]   = {}, rodata_p31[8]   = {};
        const char rodata_p40[16]  = {}, rodata_p41[16]  = {};
        const char rodata_p50[32]  = {}, rodata_p51[32]  = {};
        const char rodata_p60[64]  = {}, rodata_p61[64]  = {};
        const char rodata_p70[BUFFER_SIZE] = {}, rodata_p71[BUFFER_SIZE] = {};
        const char* rodata_symm_array[] = {
        rodata_p00, rodata_p01, rodata_p10, rodata_p11,
        rodata_p20, rodata_p21, rodata_p30, rodata_p31,
        rodata_p40, rodata_p41, rodata_p50, rodata_p51,
        rodata_p60, rodata_p61, rodata_p70, rodata_p71};

    #elif BUFFER_KIND == LARGE_ARRAY

        const LargeMemberBuffer<0x101>  buffer_rodata1('u','d','o');
        const LargeMemberBuffer<0x201>  buffer_rodata2('u','d','o');
        const LargeMemberBuffer<0x401>  buffer_rodata3('u','d','o');
        const LargeMemberBuffer<0x801>  buffer_rodata4('u','d','o');
        const LargeMemberBuffer<0x1001> buffer_rodata5('u','d','o');
        const LargeMemberBuffer<0x2001> buffer_rodata6('u','d','o');
        const LargeMemberBuffer<0x4001> buffer_rodata7('u','d','o');
        const LargeMemberBuffer<0x8001> buffer_rodata8('u','d','o');
        const LargeMemberBuffer<BUFFER_SIZE> buffer_rodata9('u','d','o');
        const LargeMemberBufferBase * rodata_large_array[] = {
        &buffer_rodata1, &buffer_rodata2, &buffer_rodata3, &buffer_rodata4,
        &buffer_rodata5, &buffer_rodata6, &buffer_rodata7, &buffer_rodata8, &buffer_rodata9
        };

    #endif

#endif
