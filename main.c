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
typedef int (*test_print) (int);

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

int
hello1 (int a, int b)
{

  return a + b;

}

int
closure_pattern (int val)
{
  return hello1 (6, val);
}

int
closure_pattern_1 (int val)
{
  return hello1 (7, val);
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

  int func_len = 0;
  byte *ptr_pattern = (byte *) closure_pattern;
  for (func_len = 0; ptr_pattern[func_len] != 0xc3;)
    func_len++;


  func_len++;


  memcpy (src, ptr_pattern, func_len);

  return (test_print) src;
  //uint32_t a = crc32 ((const void *) src, statbuf.st_size);

  // munmap (src, statbuf.st_size);

}


closure_t
closure_make (ptr_func func, int val)
{

  closure_save_val = val;
  closure_save_func = func;

  return closure_pattern;

}

void
delete_closure (closure_t adder)
{
  closure_save_val = 0;
  closure_save_func = adder = NULL;

}


int
main ()
{

  int n = 0;

  n = closure_pattern (1);
  printf ("%d\n", n);

  test_print adder = create_func (n);

  n = adder (3);
  printf ("%d\n", n);

//  closure_t adder = closure_make(test_func, n);

  //printf ("%d\n", adder (2)); // ожидаем увидеть 5

  //delete_closure (adder);

  return 0;
}
