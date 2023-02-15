#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <uchar.h>
#include <sys/mman.h>

#define src_len 100
typedef unsigned char byte;
/*
const byte src_pattern[] = { 0xf3, 0x0f, 0x1e, 0xfa,
  0x55, 0x48, 0x89, 0xe5, 0xbe,
  0x09, 0x00, 0x00, 0x00,
  0xbf, 0x00, 0x00, 0x00, 0x00,
  0xb8, 0x00, 0x00, 0x00, 0x00,
  0xe8, 0x00, 0x00, 0x00, 0x00,
  0x90,
  0x5d,
  0xc3
};
*/
const byte src_pattern[] = { 0xf3, 0x0f, 0x1e, 0xfa,
  0x55, 0x48, 0x89, 0xe5, 0x90,
  0x5d,
  0xc3
};


typedef int (*closure_t) (int);
typedef int (*ptr_func) (int);
typedef void (*test_print) (void);

closure_t closure_save_func;
int closure_save_val;

void
print_error (const char *format, ...)
{
  va_list argptr;
  va_start (argptr, format);
  vprintf (format, argptr);
  va_end (argptr);
  exit (EXIT_FAILURE);
}

int
test_func (int a)
{
  return a + closure_save_val;
}

test_print
create_func (int number)
{

  int a = sizeof (src_pattern);

  void *src;
  if ((src =
       mmap (0, sizeof (src_pattern), PROT_EXEC | PROT_WRITE,
	     MAP_PRIVATE | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED)
    print_error ("%s\n",
		 "Ошибка вызова функции mmap для входного файла");

  memcpy (src, src_pattern, sizeof (src_pattern));
//snprintf (src, src_len, src_pattern, number);

  return (test_print) src;
  //uint32_t a = crc32 ((const void *) src, statbuf.st_size);

  // munmap (src, statbuf.st_size);

}

int
closure_func (int val)
{
/*вот тут не могу понять как сопоставить эту функцию
с данными, которые передавались в closure_make
МОжно ли создавать не указатель на closure_func, а
указатель на данные и этот указатель передавать как
первый параметр в процедуру например EXEC_CLOSURE:
EXEC_CLOSURE(ptr_closure, ...)
Переменное число аргументов будут перенапрявляться
в нужную функцию
*/
  return closure_save_func (val);

}

closure_t
closure_make (ptr_func func, int val)
{

  closure_save_val = val;
  closure_save_func = func;

  return closure_func;

}

void
delete_closure (closure_t adder)
{
  closure_save_val = 0;
  closure_save_func = adder = NULL;

}

void
hello ()
{

  //printf ("%d", 9);

}

int
main ()
{

  hello ();

  int n = 3;

  test_print adder = create_func (n);
  adder ();
//  closure_t adder = closure_make(test_func, n);

  //printf ("%d\n", adder (2)); // ожидаем увидеть 5

  //delete_closure (adder);

  return 0;
}
