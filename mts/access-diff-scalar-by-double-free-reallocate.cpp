#include <memory>
#include <cstdint>
#include "include/mss.hpp"

#define RELOC_NUM 64

int main(int argc, char** argv){

    int length = atoi(argv[1]);

    allocator<int> alloc;

    int *a = alloc.allocate(length);
    int *b = alloc.allocate(length);
    int *c = alloc.allocate(length);

    alloc.deallocate(a);
    alloc.deallocate(b);
    alloc.deallocate(a);

    int *d = alloc.allocate(length);
    alloc.construct(d,10,1);
    for(int i = 0; i != RELOC_NUM; i++){
        uintptr_t address = (uintptr_t) alloc.allocate(length);
        if(address == (uintptr_t)d){
            if(check((int*) address, length, 1, 2)){
                return 0;
            }else{
                return 1;
            }
        }
    }

    return 1;
}