#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <uchar.h>
#include <sys/mman.h>

typedef unsigned char byte;
typedef int (*closure_t) (int, ...);
typedef int (*closure_src_func_t) (void);
typedef struct argv_entry argv_entry;

struct argv_entry
{
  argv_entry *next;
  int value;
};

struct closure_data
{
  int argc;
  closure_t func;
  argv_entry *head;
  size_t src_len;
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

int fix_sum(int a, int b)
{
return a + b;
}

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
      printf ("%d\n", select);
      result += select;
    }
  va_end (factor);		// завершаем обработку параметров
  return result;
}

argv_entry *
free_argv (argv_entry * head)
{
  if (!head)
    return head;

  argv_entry *temp = head->next;
  free (head);
  return temp;
}

/*сохранение агрументов, добавляя новые в голову.
Нужно для передачи параметров в пользовательскую функцию в обратном порядке*/
argv_entry *
closure_save_arg (argv_entry * head, int value)
{

  argv_entry *new = (argv_entry *) malloc (sizeof (argv_entry));
  new->value = value;
  new->next = head;

  return new;

}


int
closure_pattern (int argc, ...)
{

  closure_data *closure_ptr = NULL;

asm (" movq %%rbx, %0;":"=r" (closure_ptr));

  int select = 0;
  argv_entry *local_argv_head = NULL, *temp_argv_head = NULL;
  va_list factor;		//указатель va_list
  va_start (factor, argc);	// устанавливаем указатель
  for (int i = 0; i < argc; i++)
    {
      select = va_arg (factor, int);	// получаем значение текущего параметра типа int
      local_argv_head = closure_save_arg (local_argv_head, select);
    }
  va_end (factor);		// завершаем обработку параметров

  byte *src;
  int src_len = 100;
  if ((src =
       mmap (0, src_len, PROT_EXEC | PROT_WRITE,
	     MAP_PRIVATE | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED)
    print_error ("%s\n",
		 "Ошибка вызова функции mmap для входного файла");


  unsigned int ptr_call_offset = 0;

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

  int argc_all = argc + closure_ptr->argc;
  int use_stack = 0;
  temp_argv_head = local_argv_head;

  while (argc_all > 6)
    {
/*если общее количество аргументов больше 6
значит сначала аргументы помещаем в стек, пока не
останется 6 агрументов для передачи через регистры*/

      if (!temp_argv_head)
	temp_argv_head = closure_ptr->head;

      src[ptr_call_offset] = 0x6a;
      src[ptr_call_offset + 1] = temp_argv_head->value;
      ptr_call_offset += 2;
      use_stack += 8;
      temp_argv_head = temp_argv_head->next;
      argc_all--;
    }

/*заполняем регистры в обратном порядке*/
/*mov r9d, value*/
  if (argc_all == 6)
    {
      if (!temp_argv_head)
	temp_argv_head = closure_ptr->head;

      src[ptr_call_offset] = 0x41;
      src[ptr_call_offset + 1] = 0xb9;
      *((unsigned int *) (src + ptr_call_offset + 2)) =
	(unsigned int) temp_argv_head->value;
      ptr_call_offset += 6;
      temp_argv_head = temp_argv_head->next;
      argc_all--;
    }

/*заполняем регистры в обратном порядке*/
/*mov r8d, value*/
  if (argc_all == 5)
    {
      if (!temp_argv_head)
	temp_argv_head = closure_ptr->head;

      src[ptr_call_offset] = 0x41;
      src[ptr_call_offset + 1] = 0xb8;
      *((unsigned int *) (src + ptr_call_offset + 2)) =
	(unsigned int) temp_argv_head->value;
      ptr_call_offset += 6;
      temp_argv_head = temp_argv_head->next;
      argc_all--;
    }

/*mov ecx, value*/
  if (argc_all == 4)
    {
      if (!temp_argv_head)
	temp_argv_head = closure_ptr->head;

      src[ptr_call_offset] = 0xb9;
      *((unsigned int *) (src + ptr_call_offset + 1)) =
	(unsigned int) temp_argv_head->value;
      ptr_call_offset += 5;
      temp_argv_head = temp_argv_head->next;
      argc_all--;
    }


/*mov edx, value*/
  if (argc_all == 3)
    {
      if (!temp_argv_head)
	temp_argv_head = closure_ptr->head;

      src[ptr_call_offset] = 0xba;
      *((unsigned int *) (src + ptr_call_offset + 1)) =
	(unsigned int) temp_argv_head->value;
      ptr_call_offset += 5;
      temp_argv_head = temp_argv_head->next;
      argc_all--;
    }


/*mov esi, value*/
  if (argc_all == 2)
    {
      if (!temp_argv_head)
	temp_argv_head = closure_ptr->head;

      src[ptr_call_offset] = 0xbe;
      *((unsigned int *) (src + ptr_call_offset + 1)) =
	(unsigned int) temp_argv_head->value;
      ptr_call_offset += 5;
      temp_argv_head = temp_argv_head->next;
      argc_all--;
    }


/*mov edi, value*/
  if (argc_all == 1)
    {
      if (!temp_argv_head)
	temp_argv_head = closure_ptr->head;

      src[ptr_call_offset] = 0xbf;
      *((unsigned int *) (src + ptr_call_offset + 1)) =
	(unsigned int) temp_argv_head->value;
      ptr_call_offset += 5;
      temp_argv_head = temp_argv_head->next;
      argc_all--;
    }


/*Очищаем eax(так надо для вызова функций с переменным числом параметров) и вызываем функцию через rbx*/

/*mov eax, 0*/
  src[ptr_call_offset] = 0xb8;
  *((unsigned int *) (src + ptr_call_offset + 1)) = (unsigned int) 0;
  ptr_call_offset += 5;

/*mov rbx, func*/
  src[ptr_call_offset] = 0x48;
  src[ptr_call_offset + 1] = 0xbb;	//rbx
  *((size_t *) (src + ptr_call_offset + 2)) = (size_t) closure_ptr->func;
  ptr_call_offset += 10;

/*call rbx*/
  src[ptr_call_offset] = 0xff;
  src[ptr_call_offset + 1] = 0xd3;	//rbx
  ptr_call_offset += 2;

/*сдвиг указателя стека на размер переданных ранее параметров*/

/*add rsp, use_stack*/
  if (use_stack)
    {

      src[ptr_call_offset] = 0x48;
      src[ptr_call_offset + 1] = 0x83;
      src[ptr_call_offset + 2] = 0xc4;
      src[ptr_call_offset + 3] = use_stack;
      ptr_call_offset += 4;
    }

/*leave*/
  src[ptr_call_offset++] = 0xc9;

/*ret*/
  src[ptr_call_offset++] = 0xc3;

  int ret = ((closure_src_func_t) src) ();

/*очищаем список локальных переменных*/
  while (local_argv_head)
    local_argv_head = free_argv (local_argv_head);

  munmap (src, src_len);

  return ret;

}

closure_t
closure_make (closure_t func, int argc, ...)
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

  ((closure_data *) src)->func = func;
  ((closure_data *) src)->argc = argc;
  ((closure_data *) src)->src_len = src_len;

  int select = 0;
  va_list factor;		//указатель va_list
  va_start (factor, argc);	// устанавливаем указатель
  for (int i = 0; i < argc; i++)
    {
      select = va_arg (factor, int);	// получаем значение текущего параметра типа int
      ((closure_data *) src)->head =
	closure_save_arg (((closure_data *) src)->head, select);
    }
  va_end (factor);		// завершаем обработку параметров

  unsigned int ptr_call_offset = sizeof (closure_data);

  /*mov eax, 0 */
  src[ptr_call_offset] = 0xb8;
  *((unsigned int *) (src + ptr_call_offset + 1)) = (unsigned int) 0;
  ptr_call_offset += 5;

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

  while (cd->head)
    cd->head = free_argv (cd->head);

  munmap (cd, cd->src_len);

}


int
main ()
{

#if 1
  printf ("1: %d \n", sum (9, 1, 2, 3, 4, 5, 6, 7, 8, 9));

  closure_t func = closure_make (sum, 2, 9, 1);
  int res = func (8, 2, 3, 4, 5, 6, 7, 8, 9);

  printf ("2: %d \n", res);

  delete_closure (func);

#endif

#if 0
  printf ("1: %d \n", fix_sum (1, 2));

  closure_t func = closure_make (fix_sum, 1, 1);
  int res = func (1, 2);

  printf ("2: %d \n", res);

  delete_closure (func);

#endif



  return 0;
}
