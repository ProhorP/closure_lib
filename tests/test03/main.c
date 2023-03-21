#define CLOSURE_LIB
#include "../../closure.h"
#include <stdio.h>

intptr_t
fix_sum_char (char a, char b)
{
  return (intptr_t) (a + b);
}

int
main ()
{

  typedef intptr_t (*closure_t) (char);

  printf ("1: %ld \n", fix_sum_char (2, 2));

  closure_t func;
  CLOSURE_MAKE (func, fix_sum_char, 2, 1, CLOSURE_SCALAR (2));
  intptr_t res = func (2);

  printf ("2: %ld \n", res);

  CLOSURE_DELETE (func);

  return EXIT_SUCCESS;
}
