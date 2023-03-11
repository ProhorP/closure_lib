#include "../../closure.h"
#include <stdio.h>

int
main ()
{

  typedef void (*closure_t) (char *, ...);

  char *pattern = malloc (100);
  strcpy (pattern, "%s%d\n");

  printf (pattern, "Simple function", 5);

  closure_t func;
  CLOSURE_MAKE (func, printf, 3, 1,
		CLOSURE_POINTER (pattern, strlen (pattern) + 1));

  free (pattern);	
  func ("Closure function", 5);

  CLOSURE_DELETE (func);

  return EXIT_SUCCESS;
}
