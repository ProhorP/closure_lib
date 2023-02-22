#include <stdio.h>
#include <stdarg.h>

int
sum (int n, ...)
{
  int result = 0;
  int select = 0;
  va_list factor;		//указатель va_list
  va_start (factor, n);		// устанавливаем указатель
  for (int i = 0; i < n; i++)
    {
      select = va_arg (factor, int);	// получаем значение текущего параметра типа int
      result += select;
    }
  va_end (factor);		// завершаем обработку параметров
  return result;
}

int
translate ()
{
  return sum (8, 1, 2, 3, 4, 5, 6, 7, 8 );
}

int
main (void)
{
int a = translate();
  printf ("%d \n", sum (3, 1, 2, 3));
  printf ("%d \n", sum (7, 1, 2, 3, 4, 5, 6, 7));
  return 0;
}
