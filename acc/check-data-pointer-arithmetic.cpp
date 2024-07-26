#include "include/assembly.hpp"
#include "include/conf.hpp"
#include <cstdlib>
#include <cstring>
#include <cstdint>
int main(){

    long long assem_dist;

    INIT_BUFFER;

    long long src = (long long)STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND));
    long long dst = (long long)STR2UL(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)) ,overflow);
    GET_DISTANCE_IMM(assem_dist, dst, src);

    if(dst == src + assem_dist){
        UNINIT_BUFFER;
        return 0;
    }else{
        UNINIT_BUFFER;
        return 1;
    }
}

