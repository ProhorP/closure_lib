#include "../../closure.h"
#include <stdio.h>

intptr_t
fix_sum (intptr_t a, intptr_t b)
{
  return a + b;
}

int
main ()
{

  printf ("1: %ld \n", fix_sum (1, 2));

  typedef intptr_t (*closure_t) (intptr_t);
  closure_t func;

  CLOSURE_MAKE (func, fix_sum, 2, 1, CLOSURE_SCALAR (1));
  intptr_t res = func (2);

  printf ("2: %ld \n", res);

  CLOSURE_DELETE (func);

  return EXIT_SUCCESS;
}
