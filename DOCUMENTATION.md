## Intro
There are only 2 macro in the library:
* CLOSURE_MAKE
* CLOSURE_DELETE

## CLOSURE_MAKE

CLOSURE_MAKE(void *closure_func, void *func, int argc_all, int argc, ...)

* closure_func - closure function, which can be called
* func - the function that is stored in the closure
* argc_all - total number of arguments
* argc - the number of arguments stored in the closure
* ... - arguments stored in the closure

if the argument is of scalar type, then you need to pass it through a macro
```CLOSURE_SCALAR(value)```
if the argument is of pointer type, then you need to pass it through a macro
```CLOSURE_POINTER(pointer, length)```

## CLOSURE_DELETE
CLOSURE_DELETE(void *closure_func)

Be sure to call the macro ```CLOSURE_DELETE(closure_func)``` when the closure is no longer used

## Independent work
it is necessary to declare the prototype of the closure function yourself, for the correct call

## Examples

```
#define CLOSURE_LIB
#include "closure.h"
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
  /*equal int sum (int b)*/
  /*because the argument A is stored in the closure*/
  typedef int (*closure_t) (int);

  /*memory allocation for the closure function*/
  closure_t closure_func;

  /*create closure function*/
  /*closure_func - closure function, which can be called*/
  /*func - the function that is stored in the closure*/
  /*2 - total number of arguments in function sum */
  /*1 - the number of arguments stored in the closure*/
  /*CLOSURE_SCALAR (1) - scalar artument equal to 1 stored in the closure*/
  CLOSURE_MAKE (closure_func, sum, 2, 1, CLOSURE_SCALAR (1));

  /*call closure function*/
  int res = closure_func (2);

  printf ("%d\n", res);

  /*be sure to clear the memory*/
  CLOSURE_DELETE (closure_func);

  return EXIT_SUCCESS;
}

```

