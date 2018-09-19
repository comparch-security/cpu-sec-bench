
class HelperOrig
{
public:
  virtual int set_var() {
    return 1;
  }
};

class HelperReplace
{
  int lvar;
public:
    virtual int set_var(int new_var) {
      lvar = new_var;
      return 0;
    }
};

int main() {
  HelperOrig orig, *pOrig;
  HelperReplace replace;
  pOrig = &orig;
 
  // replace the vtable pointer
  *(unsigned long long *)&orig = *(unsigned long long *)&replace; 
  
  // call the original virtual function
  return pOrig->set_var();
 }


