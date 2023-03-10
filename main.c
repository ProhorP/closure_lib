#include "closure.h"
#include <stdio.h>


intptr_t
fix_sum (intptr_t a, intptr_t b)
{
  return a + b;
}

intptr_t
fix_sum_char (char a, char b)
{
  return (intptr_t) (a + b);
}

intptr_t
sum (intptr_t n, ...)
{
  intptr_t result = 0;
  intptr_t select = 0;
  va_list factor;		//указатель va_list
  va_start (factor, n);		// устанавливаем указатель
  for (intptr_t i = 0; i < n; i++)
    {
      select = va_arg (factor, intptr_t);	// получаем значение текущего параметра типа int
      printf ("%ld\n", select);
      result += select;
    }
  va_end (factor);		// завершаем обработку параметров
  return result;
}

int
main ()
{

#if 1
  printf ("1: %ld \n", sum (9, 1, 2, 3, 4, 5, 6, 7, 8, 9));

  typedef intptr_t (*closure_t) (intptr_t, ...);

  closure_t func;
  CLOSURE_MAKE (func, sum, 10, 2, CLOSURE_SCALAR (9), CLOSURE_SCALAR (1));
  intptr_t res = func (2, 3, 4, 5, 6, 7, 8, 9);

  printf ("2: %ld \n", res);

  CLOSURE_DELETE (func);

#endif

#if 0
  printf ("1: %ld \n", fix_sum (1, 2));

  typedef intptr_t (*closure_t) (intptr_t);
  closure_t func;

  CLOSURE_MAKE (func, fix_sum, 2, 1, CLOSURE_SCALAR (1));
  intptr_t res = func (2);

  printf ("2: %ld \n", res);

  CLOSURE_DELETE (func);

#endif

#if 0

  typedef intptr_t (*closure_t) (char);

  printf ("1: %ld \n", fix_sum_char (2, 2));

  closure_t func;
  CLOSURE_MAKE (func, fix_sum_char, 2, 1, CLOSURE_SCALAR (2));
  intptr_t res = func (2);

  printf ("2: %ld \n", res);

  CLOSURE_DELETE (func);

#endif

#if 0

  typedef void (*closure_t) (char *, ...);

  char *pattern = malloc (100);
  strcpy (pattern, "%s%d\n");

  printf (pattern, "Простая функция", 5);

  closure_t func;
  CLOSURE_MAKE (func, printf, 3, 1, pattern, strlen (pattern) + 1);

  free (pattern);		//удаляем указатель переданный в замыкание
  func ("Функция замыкания", 5);

  CLOSURE_DELETE (func);
//  delete_closure ((void*)func);

#endif

  return EXIT_SUCCESS;
}
