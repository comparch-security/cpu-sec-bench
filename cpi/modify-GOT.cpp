#include <cstdlib>
#include "include/global_var.hpp"
#include "include/signal.hpp"

#if defined(COMPILER_MSVC)
#include "include/cfi.hpp"
typedef void(*func_t)();

int modifyIAT(const char* moduleName, const char* funcName, func_t newFunc) {
    HMODULE module = GetModuleHandleA(moduleName);
    #if defined(DEBUG_OUTPUT)
    if (module == NULL) {
        std::cout << "Failed to get module handle." << std::endl;
        return;
    }
    #endif

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)module;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)module + dosHeader->e_lfanew);

    DWORD importDescriptorRVA = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    #if defined(DEBUG_OUTPUT)
    if (importDescriptorRVA == 0) {
        std::cout << "No import table found." << std::endl;
        return;
    }
    #endif

    PIMAGE_IMPORT_DESCRIPTOR importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)module + importDescriptorRVA);
    for (; importDescriptor->Name != 0; importDescriptor++) {
        PIMAGE_THUNK_DATA thunk = (PIMAGE_THUNK_DATA)((BYTE*)module + importDescriptor->FirstThunk);
        for (; thunk->u1.AddressOfData != 0; thunk++) {
            if (strcmp((char*)((BYTE*)module + thunk->u1.AddressOfData + 2), funcName) == 0) {
                DWORD oldProtect;
                VirtualProtect(&thunk->u1.Function, sizeof(func_t), PAGE_EXECUTE_READWRITE, &oldProtect);
                thunk->u1.Function = (ULONG_PTR)newFunc;
                VirtualProtect(&thunk->u1.Function, sizeof(FARPROC), oldProtect, &oldProtect);
                return 0;
            }
        }
    }
    return 1;
}

void newfunc(){
    gvar_decr();
}

int main() {
    //func_t newFunc =  [](void) { std::cout << "New function called!" << std::endl; };
    gvar_init(1);
    Helper *orig = new Helper();
    if(modifyIAT("kernel32.dll", "create_fake_vtable_on_heap", newfunc)) return 2;
    create_fake_vtable_on_heap(8);
    return gvar();
}


#else

int FORCE_NOINLINE helper() {
  return 2;
}

int main(int argc, char* argv[])
{
  int cet_enabled = argv[1][0] - '0';
  void *rand_label = (void*)&main;
  GET_LABEL_ADDRESS(rand_label,TARGET_LABEL);
  // In LLVM, goto is not allow to jump over declaration of local variables.
  void *got = NULL;

  gvar_init(helper());

  if(cet_enabled == -1) { GOTO_SAVED_LABEL(rand_label);}   // impossible to happen

  get_got_func(&got, rand_label, cet_enabled);
  COMPILER_BARRIER;
TARGET_LABEL(argc)
  rand();
  COMPILER_BARRIER;

  begin_catch_exception(got, SEGV_ACCERR);
  replace_got_func((void **)helper, got);
  end_catch_exception();

  gvar_init(gvar() - rand());
  return gvar();
}

#endif