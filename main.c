#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <uchar.h>
#include <sys/mman.h>

typedef unsigned char byte;
typedef size_t (*closure_t) (size_t, ...);
typedef size_t (*closure_src_func_t) (void);
typedef struct argv_entry argv_entry;

struct argv_entry
{
  argv_entry *next;
  size_t value;
};

struct closure_data
{
  int argc_all;
  closure_t func;
  argv_entry *head;
  size_t src_len;
  int argc_remains;
};

typedef struct closure_data closure_data;


void
print_error (const char *format, ...)
{
  va_list argptr;
  va_start (argptr, format);
  vprintf (format, argptr);
  va_end (argptr);
  exit (EXIT_FAILURE);
}

size_t
fix_sum (size_t a, size_t b)
{
  return a + b;
}

size_t
fix_sum_char (char a, char b)
{
  return (size_t) (a + b);
}

size_t
sum (size_t n, ...)
{
  size_t result = 0;
  size_t select = 0;
  va_list factor;		//указатель va_list
  va_start (factor, n);		// устанавливаем указатель
  for (size_t i = 0; i < n; i++)
    {
      select = va_arg (factor, size_t);	// получаем значение текущего параметра типа int
      printf ("%ld\n", select);
      result += select;
    }
  va_end (factor);		// завершаем обработку параметров
  return result;
}

void
free_argv (argv_entry * head)
{

  if (!head)
    return;

  argv_entry *temp = head->next;
  free (head);

  free_argv (temp);

}

/*сохранение агрументов, добавляя новые в голову.
Нужно для передачи параметров в пользовательскую функцию в обратном порядке*/
argv_entry *
closure_save_arg (argv_entry * head, size_t value)
{

  argv_entry *new = (argv_entry *) malloc (sizeof (argv_entry));
  new->value = value;
  new->next = head;

  return new;

}

size_t
closure_pattern (size_t dummy, ...)
{

  closure_data *closure_ptr = NULL;

asm (" movq %%rbx, %0;":"=r" (closure_ptr));

  size_t select = 0;
  argv_entry *local_argv_head = NULL, *temp_argv_head = NULL;
  va_list factor;		//указатель va_list

  local_argv_head = closure_save_arg (local_argv_head, dummy);

  va_start (factor, dummy);	// устанавливаем указатель
  for (int i = 0; i < closure_ptr->argc_remains-1; i++)
    {
      select = va_arg (factor, size_t);	// получаем значение текущего параметра типа int
      local_argv_head = closure_save_arg (local_argv_head, select);
    }
  va_end (factor);		// завершаем обработку параметров

  byte *src;
  size_t src_len = 100;
  if ((src =
       mmap (0, src_len, PROT_EXEC | PROT_WRITE,
	     MAP_PRIVATE | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED)
    print_error ("%s\n",
		 "Ошибка вызова функции mmap для входного файла");


  size_t ptr_call_offset = 0;

/*пролог функции мпилятора gcc*/
  src[ptr_call_offset++] = 0xf3;
  src[ptr_call_offset++] = 0x0f;
  src[ptr_call_offset++] = 0x1e;
  src[ptr_call_offset++] = 0xfa;

/*push rbp*/
  src[ptr_call_offset++] = 0x55;

/*mov rbp, rsp*/
  src[ptr_call_offset] = 0x48;
  src[ptr_call_offset + 1] = 0x89;
  src[ptr_call_offset + 2] = 0xe5;
  ptr_call_offset += 3;

  int argc_all = closure_ptr->argc_all;
  temp_argv_head = local_argv_head;

  while (argc_all)
    {
/*если общее количество аргументов больше 6
значит сначала аргументы помещаем в стек, пока не
останется 6 агрументов для передачи через регистры*/

      if (!temp_argv_head)
	temp_argv_head = closure_ptr->head;

/*mov rax, value*/
      src[ptr_call_offset] = 0x48;
      src[ptr_call_offset + 1] = 0xb8;
      *((size_t *) (src + ptr_call_offset + 2)) =
	(size_t) temp_argv_head->value;
      ptr_call_offset += 10;

      if (argc_all > 6)
	{
/*в стек*/
/*push rax*/
	  src[ptr_call_offset++] = 0x50;

	}
      else if (argc_all == 6)
	{
	  /*заполняем регистры в обратном порядке */
/*mov r9, rax*/
	  src[ptr_call_offset++] = 0x49;
	  src[ptr_call_offset++] = 0x89;
	  src[ptr_call_offset++] = 0xc1;

	}
      else if (argc_all == 5)
	{
/*mov r8, rax*/
	  src[ptr_call_offset++] = 0x49;
	  src[ptr_call_offset++] = 0x89;
	  src[ptr_call_offset++] = 0xc0;
	}
      else if (argc_all == 4)
	{
/*mov rcx, rax*/
	  src[ptr_call_offset++] = 0x48;
	  src[ptr_call_offset++] = 0x89;
	  src[ptr_call_offset++] = 0xc1;

	}
      else if (argc_all == 3)
	{
/*mov rdx, rax*/
	  src[ptr_call_offset++] = 0x48;
	  src[ptr_call_offset++] = 0x89;
	  src[ptr_call_offset++] = 0xc2;
	}
      else if (argc_all == 2)
	{
/*mov rsi, rax*/
	  src[ptr_call_offset++] = 0x48;
	  src[ptr_call_offset++] = 0x89;
	  src[ptr_call_offset++] = 0xc6;
	}
      else if (argc_all == 1)
	{
/*mov rdi, rax*/
	  src[ptr_call_offset++] = 0x48;
	  src[ptr_call_offset++] = 0x89;
	  src[ptr_call_offset++] = 0xc7;
	}
      temp_argv_head = temp_argv_head->next;
      argc_all--;
    }

/*Очищаем eax(так надо для вызова функций с переменным числом параметров) и вызываем функцию через rbx*/
/*xor rax, rax*/
  src[ptr_call_offset++] = 0x48;
  src[ptr_call_offset++] = 0x31;
  src[ptr_call_offset++] = 0xc0;

/*mov rbx, func*/
  src[ptr_call_offset] = 0x48;
  src[ptr_call_offset + 1] = 0xbb;	//rbx
  *((size_t *) (src + ptr_call_offset + 2)) = (size_t) closure_ptr->func;
  ptr_call_offset += 10;

/*call rbx*/
  src[ptr_call_offset] = 0xff;
  src[ptr_call_offset + 1] = 0xd3;	//rbx
  ptr_call_offset += 2;

/*leave*/
  src[ptr_call_offset++] = 0xc9;

/*ret*/
  src[ptr_call_offset++] = 0xc3;

  size_t ret = ((closure_src_func_t) src) ();

/*очищаем список локальных переменных*/
  free_argv (local_argv_head);

  munmap (src, src_len);

  return ret;

}

closure_t
closure_make (closure_t func, int argc_all, int argc, ...)
{

/*Создаем функцию переходник, которая в своем коде имеет ссылку на данные замыкания
и прыгает в closure_pattern
Функция должна вызываться как прототип функции closure_pattern,
чтобы все параметры передались в функцию closure_pattern как есть
*/

  byte *src;
  int src_len = 100;
  if ((src =
       mmap (0, src_len, PROT_EXEC | PROT_WRITE,
	     MAP_PRIVATE | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED)
    print_error ("%s\n",
		 "Ошибка вызова функции mmap для входного файла");

  closure_data *closure_ptr = (closure_data *) src;

  closure_ptr->func = func;
  closure_ptr->argc_all = argc_all;
  closure_ptr->argc_remains = argc_all - argc;
  closure_ptr->src_len = src_len;

  size_t select = 0;
  va_list factor;		//указатель va_list
  va_start (factor, argc);	// устанавливаем указатель
  for (int i = 0; i < argc; i++)
    {
      select = va_arg (factor, size_t);	// получаем значение текущего параметра типа int
      closure_ptr->head = closure_save_arg (closure_ptr->head, select);
    }
  va_end (factor);		// завершаем обработку параметров

  size_t ptr_call_offset = sizeof (closure_data);

/*xor rax, rax*/
  src[ptr_call_offset++] = 0x48;
  src[ptr_call_offset++] = 0x31;
  src[ptr_call_offset++] = 0xc0;

/*mov rbx, func*/
  src[ptr_call_offset] = 0x48;
  src[ptr_call_offset + 1] = 0xbb;
  *((size_t *) (src + ptr_call_offset + 2)) = (size_t) closure_pattern;
  ptr_call_offset += 10;

/*сохранили в стеке адрес вызываемой функции*/
/*push rbx*/
  src[ptr_call_offset++] = 0x53;

/*этот же регистр кладем ссылку на данные замыкания*/
/*mov rbx, data_ptr*/
  src[ptr_call_offset] = 0x48;
  src[ptr_call_offset + 1] = 0xbb;	//rbx
  *((size_t *) (src + ptr_call_offset + 2)) = (size_t) src;
  ptr_call_offset += 10;

/*прыгаем в ранее сохраненную функцию в стеке*/
  src[ptr_call_offset++] = 0xc3;

  return (closure_t) (src + sizeof (closure_data));

}

void
delete_closure (closure_t adder)
{

  closure_data *cd = (closure_data *) (adder - sizeof (closure_data));

  free_argv (cd->head);

  munmap (cd, cd->src_len);

}


int
main ()
{

#if 0
  printf ("1: %ld \n", sum (9, 1, 2, 3, 4, 5, 6, 7, 8, 9));

  closure_t func = closure_make (sum, 10, 2, 9, 1);
  size_t res = func (0, 2, 3, 4, 5, 6, 7, 8, 9);

  printf ("2: %ld \n", res);

  delete_closure (func);

#endif

#if 0
  printf ("1: %ld \n", fix_sum (1, 2));

  closure_t func = closure_make (fix_sum, 2, 1, 1);
  size_t res = func (0, 2);

  printf ("2: %ld \n", res);

  delete_closure (func);

#endif

#if 1
  printf ("1: %ld \n", fix_sum_char (2, 2));

  closure_t func = closure_make (fix_sum_char, 2, 1, 2);
  size_t res = func (2);

  printf ("2: %ld \n", res);

  delete_closure (func);

#endif



  return 0;
}
