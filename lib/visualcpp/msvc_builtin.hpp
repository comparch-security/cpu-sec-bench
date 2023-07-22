#ifndef MSVC_BUILTIN_HPP_INCLUDED
#define MSVC_BUILTIN_HPP_INCLUDED

// make a function incline/non-inline
#define FORCE_INLINE __forceinline
#define FORCE_NOINLINE __declspec(noinline)

// a barrier to stop compiler from reorder memory operations
// For hardware access, use the /volatile:iso compiler option together with the volatile keyword.
#define COMPILER_BARRIER __MACHINEX64(void __faststorefence(void))

// code/data alignment
#define ALIGN(NByte) __declspec ((aligned (NByte)))

GLOBAR_VAR_PRE uintptr_t target_offsets[10];
GLOBAR_VAR_PRE int fake_use_arg;

// It is needed add the option "get-code-offset" in configuration
// before using this inline func
inline void get_label(void* &ptr){
  static int index = 0;
  uintptr_t offset = -1;
  //read temp file once
  if(index == 0){
    std::ifstream offset_tmp("offset.tmp");
    if(offset_tmp.good()){
      int i = 0;
      while(offset_tmp >> offset){
        #ifdef DEBUG_OUTPUT
        if(offset == -1){
          std::cerr << "offset tmp file is empty" << std::endl;
        }
        #endif
        target_offsets[i++] = offset;
      }
    }
    #ifdef DEBUG_OUTPUT
    else{
      std::cerr << "offset tmp file is not exist" << std::endl;
    }
    #endif
    offset_tmp.close();
  }
  //get one offset from arry
  ptr = (void*)((uintptr_t)ptr + target_offsets[index++]);
}

#endif