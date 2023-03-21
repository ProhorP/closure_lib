#define CLOSURE_LIB
#include "../../closure.h"
#include <stdio.h>

int
sum (int a, int b)
{
  return a + b;
}

int
main ()
{

  /*Independent work*/
  typedef int (*closure_t) (int);

  /*memory allocation for the closure function*/
  closure_t closure_func; 

  /*create closure function*/
/*2 - total number of arguments in finction sum */
/*1 - the number of arguments stored in the closure*/
  CLOSURE_MAKE (closure_func, sum, 2, 1, CLOSURE_SCALAR (1));

  /*call closure function*/
  int res = closure_func (2);

  printf ("%d\n", res);

  /*be sure to clear the memory*/
  CLOSURE_DELETE (closure_func);

  return EXIT_SUCCESS;
}

