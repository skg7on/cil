#include "../small1/testharness.h"

#define NUMERRORS 3

/* A test case with tagged unions */
#ifndef __TAGGED
 #define __TAGGED
#endif

union u {
  int  f1;
  int* f2;
  struct {
    int *f3_1;
    int *f3_2;
  }    f3;
  int* (*f4)(int*);
} __TAGGED;

int* foo(int *x) { return x;}

int main() {
  int i, i1;
  union u x;
  union u *px = &x;
#if ERROR == 0  
  x.f1 = 5;
  if(x.f1 != 5) E(1);

  x.f2 = &i;
  if(px->f2 != &i) E(2);

  x.f3.f3_1 = &i1;
  x.f3.f3_2 = &i;
  if(x.f3.f3_1 != &i1) E(4);

  // And some trick with the thing appearing both on left and right-side
  x.f2 = x.f3.f3_1;
  if(px->f2 != &i1) E(5);

  // Same for a function call
  x.f3.f3_1 = &i;
  x.f2 = foo(x.f3.f3_1); // Make sure we check the read before the write
  
  x.f4 = foo;
  x.f2 = x.f4(&i); // make sure that we check the read before the write
#endif

  // Try to provoke some errors
#if ERROR == 1
  x.f1 = 5;
  i = * x.f2; // ERROR(1): wrong union field
#endif  

#if ERROR == 2
  // We should not be allowed to take the address of a tagged field
  i1 = & px->f1; // ERROR(2): cannot take the address
#endif

#if ERROR == 3
  {
    struct s {
      union u u1;
    } x;
    i = & x.u1.f1; // ERROR(3): cannot take the address
  }
#endif
  
  if(! HAS_KIND(px, SAFE_KIND)) E(10);
  
  SUCCESS;
}
