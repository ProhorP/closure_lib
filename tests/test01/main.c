#define CLOSURE_LIB
#include "../../closure.h"
#include <stdio.h>

intptr_t
sum (intptr_t n, ...)
{
  intptr_t result = 0;
  intptr_t select = 0;
  va_list factor;		
  va_start (factor, n);		
  for (intptr_t i = 0; i < n; i++)
    {
      select = va_arg (factor, intptr_t);	
      printf ("%ld\n", select);
      result += select;
    }
  va_end (factor);		
  return result;
}

int
main ()
{

  printf ("1: %ld \n", sum (9, 1, 2, 3, 4, 5, 6, 7, 8, 9));

  typedef intptr_t (*closure_t) (intptr_t, ...);

  closure_t func;
  CLOSURE_MAKE (func, sum, 10, 2, CLOSURE_SCALAR (9), CLOSURE_SCALAR (1));
  intptr_t res = func (2, 3, 4, 5, 6, 7, 8, 9);

  printf ("2: %ld \n", res);

  CLOSURE_DELETE (func);
}
