// cfi helper definitions

#ifndef CFI_HPP_INCLUDED
#define CFI_HPP_INCLUDED

#include "include/assembly.hpp"
#include "include/global_var.hpp"

// define a type of function
typedef void (*pfunc_t)();

// define the type
typedef pfunc_t *pvtable_t;

#ifdef DLL_DEFINITION
//DLL_DEFINITION should be defined in all DLL's source code
#else
  #define DLL_DEFINITION DllImport
#endif

// create a fake virtual table on heap with `nfunc' number of `pfunc_t'
DLL_DEFINITION extern pvtable_t create_fake_vtable_on_heap(unsigned int nfunc);
DLL_DEFINITION extern void free_fake_vtable_on_heap(pvtable_t addr);


class DLL_DEFINITION Base
{
public:
  virtual void virtual_func();
  virtual ~Base() {}
};

class DLL_DEFINITION Base_1v
{
public:
  virtual void virtual_func(void*);
  virtual ~Base_1v() {}
};

class DLL_DEFINITION Ret_From_Helper : public Base_1v
{
public:
  Ret_From_Helper(){}
  Ret_From_Helper(const Ret_From_Helper&) = default; 
  virtual void virtual_func(void*);
  virtual ~Ret_From_Helper() {}
};

class DLL_DEFINITION  Ret_To_Helper : public Base
{
public:
  Ret_To_Helper(){}
  Ret_To_Helper(const Ret_To_Helper&) = default; 
  virtual void virtual_func();
  virtual ~Ret_To_Helper() {}
};

class DLL_DEFINITION  Helper : public Base
{
public:
  Helper(){}
  Helper(const Helper&) = default; 
  virtual void virtual_func();
  virtual ~Helper() {}
};

class Helper2 : public Base
{
  double lvar;
public:
  DLL_DEFINITION virtual void virtual_func(int arg);
  double get_lvar() { return lvar; }
  virtual ~Helper2() {}
};

class Helper3 : public Base
{
  double lvar;
public:
  DLL_DEFINITION virtual void virtual_func(double arg);
  double get_lvar() { return lvar; }
  virtual ~Helper3() {}
};

class BaseM
{
public:
  DLL_DEFINITION virtual int virtual_funcM0();
  DLL_DEFINITION virtual int virtual_funcM1();
  virtual ~BaseM() {}
};

class Helper1M : public BaseM
{
public:
  DLL_DEFINITION virtual int virtual_funcM0();
  DLL_DEFINITION virtual int virtual_funcM1();
  virtual ~Helper1M() {}
};

class Helper2M : public BaseM
{
public:
  DLL_DEFINITION virtual int virtual_funcM0();
  DLL_DEFINITION virtual int virtual_funcM1();
  virtual ~Helper2M() {}
};

// read the vtable pointer of an object
DLL_DEFINITION extern pvtable_t read_vtable_pointer(const Base *);
DLL_DEFINITION extern pvtable_t read_vtable_pointer(const BaseM *);

// write the vtable pointer of an object
DLL_DEFINITION extern void write_vtable_pointer(Base *, pvtable_t);
DLL_DEFINITION extern void write_vtable_pointer(BaseM *, pvtable_t);

#endif
