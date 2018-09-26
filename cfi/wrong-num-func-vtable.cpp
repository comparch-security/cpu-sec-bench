#include <iostream>
#include "include/assembly.hpp"

using namespace std;

class Base1
{
public:
    virtual void f1() {cout<<"Base1::f1"<<endl;}
    virtual void g1() {cout<<"Base1::g1"<<endl;}	
};

class Base2
{
public:
    virtual void f2() {cout<<"Base2::f2()"<<endl;}
};

int main() {
  Base1 b1, *pb1;
  Base2 b2;

  *(int *)&b1 = *(int *)&b2;
  //asm volatile("movq ")
  b1.f1();
  pb1 = &b1;
  pb1->f1();

  return 0;
}

