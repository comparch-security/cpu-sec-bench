#ifdef WINDOWS
#include <windows.h>
extern "C" IMAGE_DOS_HEADER __ImageBase;
#else
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#endif
#include <cstring>
#include <iostream>

extern char etext,end;

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

#ifdef WINDOWS
    PIMAGE_NT_HEADERS pinth = (PIMAGE_NT_HEADERS)((PBYTE)&__ImageBase 
                               + reinterpret_cast<PIMAGE_DOS_HEADER>(&__ImageBase)->e_lfanew);
    ULONG NumberOfSections = pinth->FileHeader.NumberOfSections
    PIMAGE_SECTION_HEADER pish = IMAGE_FIRST_SECTION(pinth);

    {
        if(!strcmp(pish->Name, ".text"))
            break;
        printf("%p %08x %.8s\n", (PBYTE)&__ImageBase + pish->VirtualAddress,
               pish->Misc.VirtualSize, pish->Name);
    } while (pish++, --NumberOfSections);  
    size_t text_start = (size_t) pish->VirtualAddress;
    size_t text_end   = (size_t) (pish->VirtualAddress + pish->Misc.VirtualSize);

#else
    size_t text_start = 0x400000;
    size_t text_end   = (size_t) &etext;
#endif


    int i = 0;
    for (size_t heap_address = heap_start; 
         heap_address != heap_end;i++,heap_address += sizeof(size_t)) {
        size_t heap_data;

#ifndef WINDOWS
        if(!heap_address || 
        write(open("/dev/random", O_WRONLY),(void*)heap_address,sizeof(size_t)) < 0){
            continue;
        }
#endif
        memcpy(&heap_data, (void*)heap_address, sizeof(size_t));

        if ((heap_data & ~0xffffLL) == stack_address) {
            unsigned long long* target_addr = (unsigned long long*)heap_data;
            if(*target_addr >= text_start && *target_addr <= text_end)
                return 0;
            else
                return 1;
        }
    }
    return 1;
}