#include <memory>
#include <cstdlib>
#include <cstdint>

#define RELOC_NUM 64

std::allocator<target> alloc_t;
std::allocator<attacker> alloc_a;
std::allocator<char> alloc_c;

class target{
private:
    long long data1;
    long long data2;
public:
    target() {
        this->data1 = 0x1111111111111111;
        this->data2 = 0x2222222222222222;
    }
    long long getData1() {
        return this->data1;}
    long long getData2() {
        return this->data2; }
    void setData1(long long data) {
        this->data1 = data; }
    void setData2(long long data) {
        this->data2 = data; }
};

class attacker{
private:
    long long data1 = 0x3333333333333333;
    long long data2 = 0x4444444444444444;
    char* pData;
public:
    attacker() = default;
    ~attacker(){
        delete this->pData;
    }
    char* getData() { return this->pData; }
    long long getData1() {
        return this->data1;}
    long long getData2() {
        return this->data2; }
    void setData(size_t length) {
        this->pData = alloc_c.allocate(length);
    }
};

int main(int argc, char** argv){

    int length = atoi(argv[1]);

    target   *buffer1 = alloc_t.allocate(length);
    attacker *buffer2 = alloc_a.allocate(length);
    alloc_a.construct(buffer2);
    buffer2->setData(length);
    alloc_c.deallocate(buffer2->getData(),length);
    alloc_a.destroy(buffer2);
    alloc_a.deallocate(buffer2, length);

    target *buffer3 = alloc_t.allocate(length);
    alloc_t.construct(buffer3);
    for(int i = 0; i != RELOC_NUM; i++){
        uintptr_t address = (uintptr_t) alloc_a.allocate(length);
        if(address == (uintptr_t)buffer3){
            if(((attacker*) address)->getData1() == 0x1111111111111111)
                return 0;
            else if(((attacker*) address)->getData2() == 0x2222222222222222){
                return 0;
            }else{
                return 1;
            }
        }
    }

    return 1;
}