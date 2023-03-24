#include <set>
#include <cstdlib>
#include <cstdint>
#include "include/mss.hpp"

/*As implemented by ptmalloc,
 *small size chunk was organized in fast bins
 *mid size chunk was organized in small bins
 *large size chunk was organied in large bins
*/
int chunk_size[] = {
          8,  //ALLOC_SMALL_SIZE1
         48,  //ALLOC_SMALL_SIZE2
         80,  //ALLOC_SMALL_SIZE3
         88,  //ALLOC_MID_SIZE1  
        256,  //ALLOC_MID_SIZE2  
        496,  //ALLOC_MID_SIZE3  
        512,  //ALLOC_LARGE_SIZE1
      32760,  //ALLOC_LARGE_SIZE2
     262144  //ALLOC_LARGE_SIZE3
};

#define TCCACHE_SIZE 7

int main(int argc, char** argv){

    int malloc_count = atoi(argv[1]);
    int free_count   = atoi(argv[2]);
    int offset       = atoi(argv[3]);

    for(int i = 0; i != sizeof(chunk_size)/sizeof(int); i++){

        int length = chunk_size[i] + offset * (2 << i);
        int* tccache[TCCACHE_SIZE];

        if(malloc_count == 0){
            for(int i = 0; i != TCCACHE_SIZE; i++){
                tccache[i] = (int*) malloc(length);
                if(tccache[i]) malloc_count++;
            }
        }

        int *free_target = NULL;
        int *by_pass_target = NULL;
        if(malloc_count == 7 && free_count == 0){
            free_target = (int*) malloc(length);
            malloc_count++;
        }
        if(malloc_count == 8 && free_count == 0){
            by_pass_target = (int*) malloc(length); //bypass ptmalloc fast chunk's fd check
            malloc_count++;
        }
        if(malloc_count == 9 && free_count == 0){
            for(int i = 0; i != TCCACHE_SIZE; i++){ //fill the tccache bin
                if(tccache[i]){ 
                    free_count++;
                    free(tccache[i]);    //add conditional statement to avoid optimized out.
                }
            }
        }

        if(malloc_count == 9 && free_count == 7){
            if(free_target){
                free(free_target);
                free_count++;
            }
        }
        if(malloc_count == 9 && free_count == 8){
            if(by_pass_target){
                free_count++;
                free(by_pass_target);
            }
        }
        if(malloc_count == 9 && free_count == 9){
            if(free_target){
                free_count++;
                free(free_target);
            }
        }

        if(malloc_count == 9 && free_count == 10){
            std::set<uintptr_t> add_buffer;
            for(int i = 0; i != RELOC_NUM; i++){
                auto ret = add_buffer.insert((uintptr_t) malloc(length));
                if(!ret.second)
                    exit(0);
            }
            for(auto i = add_buffer.begin(); i != add_buffer.end(); i++){
                int* tmp = (int*)*i;
                if(tmp) free(tmp);
            }
        }

    }

    return 1;
}