#include <set>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdint>
#include "include/mss.hpp"

/*As implemented by ptmalloc,
 *small size chunk was organized in fast bins
 *mid size chunk was organized in small bins
 *large size chunk was organied in large bins
*/

/*PASS: dlmalloc/ptmalloc FAIL: jemalloc/ntheap/mallocng
*PASS: I7 Risc-v FAIL: Mac-M1
*add access/write test by using allocator double free debug
*malloc_count and free_count is used to avoid code reorderind
*which is caused by mac llvm optimization
*split three test cases into three functions
*make tccache size more flexile
*/
std::vector<int> chunk_size = {
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

int length = 0;

int double_free_check(std::set<uintptr_t>* addr_buffer){
    for(int i = 0; i != RELOC_NUM; i++){
        auto ret = addr_buffer->insert((uintptr_t) malloc(length));
        if(!ret.second)
            return 0;
    }
    return 1;
}

int access_check(std::set<uintptr_t>* addr_buffer){
    for(int i = 0; i != RELOC_NUM; i++){
        char* tmp = (char*) malloc(length);
        auto ret = addr_buffer->insert((uintptr_t) tmp);
        if(!ret.second){
            return check(tmp,length,1,'c');
        }
        update_by_pointer(tmp, 0, length, 1, 'c'); // init the buffer
    }
    return 1;
}

int write_check(std::set<uintptr_t>* addr_buffer){
    for(int i = 0; i != RELOC_NUM; i++){
        char* tmp = (char*) malloc(length);
        auto ret = addr_buffer->insert((uintptr_t) tmp);
        if(!ret.second){
            update_by_pointer(tmp, 0, length, 1, 'o'); // override the buffer by duplicate pointer
            uintptr_t tmp_num = (uintptr_t) tmp;
            const char* orig = (const char*) (*(addr_buffer->find(tmp_num)));
            return check(orig,length,1,'o');

        }else{
            update_by_pointer(tmp, 0, length, 1, 'c'); // init the buffer
        }
    }
    return 1;
}

int main(int argc, char** argv){

    if(argc == 3){
        char* x = (char*)malloc(10 * sizeof(char));
        memset(x, std::stod(std::string(argv[2])), 10);
        int res = x[argc];

    for(int i = 0; i != 10; i++){
        free(x+i);
    }
        
    for(int i=0; i<RELOC_NUM; i++) {
        char* buffer = (char*)malloc(10 * sizeof(char));
        if(buffer == x) return res;
        delete buffer;
    }
        return res;
    }

    int ret = 0;
    // take user input as variables to avoid optimization.
    int malloc_count = atoi(argv[1]);
    int free_count   = atoi(argv[2]);
    int offset       = atoi(argv[3]);
    // tccache size 
    int tccache_size = atoi(argv[4]);
    int flag_option  = atoi(argv[5]);
    if(tccache_size < 7){
        // tccache size is too small to overflow free chunk to fastbin chunk 
        return -2;
    }

    //traverse the chunk_size array to try different size of free chunk 
    int pow_index = 0;
    for(auto cs : chunk_size){
        length = cs + offset * (2 << pow_index++);

        //int* tccache[TCCACHE_SIZE];
        std::vector<int*> tccache(tccache_size);

        // Firstly, allocate tcache_size chunks with a size of chunk_size[i]
        if(malloc_count == 0){
            //for(int i = 0; i != TCCACHE_SIZE; i++){
            for(int i = 0; i != tccache_size; i++){
                tccache[i] = (int*) malloc(length);
                if(tccache[i]) malloc_count++;
            }
        }

        int *free_target = NULL;
        int *by_pass_target = NULL;
        // allocate the double free target chunk
        // the conditional statement is used for squashing compiler optimization
        if(malloc_count == tccache_size && free_count == 0){
            free_target = (int*) malloc(length);
            malloc_count++;
        }
        // allocate a chunk to bypass ptmalloc fast chunk's fd check
        if(malloc_count == tccache_size+1 && free_count == 0){
            by_pass_target = (int*) malloc(length);
            malloc_count++;
        }
        // fill the tccache bin
        if(malloc_count == tccache_size+2 && free_count == 0){
            for(int i = 0; i != tccache_size; i++){
                if(tccache[i]){ 
                    free_count++;
                    free(tccache[i]);    //add conditional statement to avoid optimized out.
                }
            }
        }

        if(malloc_count == tccache_size+2 && free_count == tccache_size){
            if(free_target){
                free(free_target);
                free_count++;
            }
        }
        if(malloc_count == tccache_size+2 && free_count == tccache_size+1){
            if(by_pass_target){
                free_count++;
                free(by_pass_target);
            }
        }
        if(malloc_count == tccache_size+2 && free_count == tccache_size+2){
            if(free_target){
                free_count++;
                free(free_target);
            }
        }

        if(malloc_count == tccache_size+2 && free_count == tccache_size+3){
            std::set<uintptr_t> addr_buffer;

            switch(flag_option){
                case 0:
                    ret = double_free_check(&addr_buffer);
                    break;
                case 1:
                    ret = access_check(&addr_buffer);
                    break;
                case 2:
                    ret = write_check(&addr_buffer);
                    break;
                default:
                    break;
            }

            if(!ret) return 0;

            for(auto i = addr_buffer.begin(); i != addr_buffer.end(); i++){
                int* tmp = (int*)*i;
                if(tmp) free(tmp);
            }
        }

    }

    return 1;
}
