// cfi helper definitions

#ifndef CFI_HPP_INCLUDED
#define CFI_HPP_INCLUDED

#include "include/assembly.hpp"

// define a type of function
typedef void (*pfunc_t)();

// define the type
typedef pfunc_t *pvtable_t;

// create a fake virtual table on heap with `nfunc' number of `pfunc_t'
extern pvtable_t create_fake_vtable_on_heap(unsigned int nfunc);

class Base
{
public:
  virtual void virtual_func();
};

class Helper : public Base
{
public:
  virtual void virtual_func();
};

class Helper2 : public Base
{
	double lvar;
public:
	virtual void virtual_func(int arg);
};

#endif
