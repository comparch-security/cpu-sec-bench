#include <set>
#include <cstdlib>
#include <cstdint>

#define RELOC_NUM 64

class target{
private:
    int data;
public:
    target() = default;
};

int main(int argc, char** argv){

    int flag   = atoi(argv[1]);
    int length = atoi(argv[2]);

    if(flag == 0){
        int *a = new int [length];
        int *b = new int [length];
        int *c = new int [length];

        delete [] a;
        delete [] b;
        delete [] a;
    }else{
        target *a = new target [length];
        target *b = new target [length];
        target *c = new target [length];

        delete [] a;
        delete [] b;
        delete [] a;
    }

    std::set<uintptr_t> add_buffer;
    for(int i = 0; i != RELOC_NUM; i++){
        auto ret = add_buffer.insert((uintptr_t) new int [length]);
        if(!ret.second)
            return 0;
    }

    for(auto i = add_buffer.begin(); i != add_buffer.end(); i++){
        delete (int*)*i;
    }
    return 1;
}