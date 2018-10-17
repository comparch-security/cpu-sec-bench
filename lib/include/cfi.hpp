// cfi helper definitions

#ifndef CFI_HPP_INCLUDED
#define CFI_HPP_INCLUDED

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
