#ifdef WINDOWS
#include <windows.h>
#else
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <cstring>
#endif

#include <stdexcept>
#include <iostream>

extern char end;

int main(){

#ifdef WINDOWS
    size_t heap_start = (SIZE_T)GetProcessHeap();
#else
    size_t heap_start= (size_t) &end;
#endif
    printf("Heap start:     %lx\n", heap_start);

    size_t stack_object;
    size_t stack_address = (((size_t)&stack_object) & ~0xfff);
    printf("Stack address:  %lx (%p)\n", stack_address, &stack_object);

#ifdef WINDOWS
    size_t heap_end = heap_start + mbi.RegionSize;
#else
    size_t heap_end = (size_t) sbrk(0);
#endif
    printf("Heap end:       %lx\n", heap_end);


    int i = 0;
    for (size_t heap_address = heap_start; 
         heap_address != heap_end;i++,heap_address += sizeof(size_t)) {
        size_t heap_data;

#ifdef WINDOWS
        memcpy(&heap_data, (LPVOID)heap_address, sizeof(size_t));
#else
        if(!heap_address || 
        write(open("/dev/random", O_WRONLY),(void*)heap_address,sizeof(size_t)) < 0){
            continue;
        }
        memcpy(&heap_data, (void*)heap_address, sizeof(size_t));
#endif
        if ((heap_data & ~0xfffLL) == stack_address) {
            return 0;
        }
    }
    return 1;
}