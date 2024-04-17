#include <cstdlib>
#include <cstdint>
#include "include/assembly.hpp"

#if defined(COMPILER_MSVC)
#include <string>
int main(int argc, char* argv[]) {
    HMODULE module = GetModuleHandle(NULL); // Get current module

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)module;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)module + dosHeader->e_lfanew);

    PIMAGE_IMPORT_DESCRIPTOR importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)module + ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    for (; importDescriptor->Name != 0; importDescriptor++) {
        #if defined(DEBUG_OUTPUT)
        char* moduleName = (char*)((BYTE*)module + importDescriptor->Name);
        std::cout << "Module: " << moduleName << std::endl;
        #endif

        PIMAGE_THUNK_DATA thunk = (PIMAGE_THUNK_DATA)((BYTE*)module + importDescriptor->OriginalFirstThunk);

        for (; thunk->u1.AddressOfData != 0; thunk++) {
            PIMAGE_IMPORT_BY_NAME importByName = (PIMAGE_IMPORT_BY_NAME)((BYTE*)module + thunk->u1.AddressOfData);
            #if defined(DEBUG_OUTPUT)
            std::cout << "\tFunction: " << importByName->Name << std::endl;
            #endif
            std::string tmp(importByName->Name);
            //necessary function in kernel32.dll
            if (tmp.find("GetStartupInfoW") != std::string::npos)
              return 0;
        }
    }

    return 1;
}
#else

int main(int argc, char* argv[])
{
  int cet_enabled = argv[1][0] - '0';

  void *rand_label = (void*)&main;
  GET_LABEL_ADDRESS(rand_label,TARGET_LABEL);

  // In LLVM, goto is not allow to jump over declaration of local variables.
  void *got = NULL;

  if(cet_enabled == -1) { GOTO_SAVED_LABEL(rand_label);}   // impossible to happen

  get_got_func(&got, rand_label, cet_enabled);
  COMPILER_BARRIER;
TARGET_LABEL(argc)
  rand();
  COMPILER_BARRIER;
  return 0 != *(uintptr_t *)(got) ? 0 : 1;
}

#endif
