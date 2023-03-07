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
closure_save_arg (argv_entry * head, size_t value, size_t size)
{
argv_entry *new = NULL;
  if (size)
    {
      new = (argv_entry *) malloc (sizeof (argv_entry) + size);
      new->value =(size_t) ((char*)new + sizeof (argv_entry));
      new->next = head;
      memcpy ((char*)new->value, (const char *)value, size);

    }
  else
    {
      new = (argv_entry *) malloc (sizeof (argv_entry));
      new->value = value;
      new->next = head;
    }
  return new;

}

closure_t
closure_make (closure_t func, int argc_all, int argc, ...)
{

  byte *src;
  int src_len = 1024;
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

  size_t value = 0, size = 0;
  va_list factor;		//указатель va_list
  va_start (factor, argc);	// устанавливаем указатель
  for (int i = 0; i < argc; i++)
    {
      value = va_arg (factor, size_t);	// получаем значение текущего параметра типа int
      size = va_arg (factor, size_t);	// получаем значение текущего параметра типа int
      closure_ptr->head = closure_save_arg (closure_ptr->head, value, size);
    }
  va_end (factor);		// завершаем обработку параметров

  size_t offset = sizeof (closure_data);

/*пролог функции компилятора gcc*/
  src[offset++] = 0xf3;
  src[offset++] = 0x0f;
  src[offset++] = 0x1e;
  src[offset++] = 0xfa;
/*push rbp*/
  src[offset++] = 0x55;
/*mov rbp, rsp*/
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0xe5;

/*closure_data *closure_ptr = [rbp - 0x8]*/
/*int argc = [rbp - 0xc]*/
/*int argc_all = [rbp - 0x10]*/
/*int argc_remains = [rbp - 0x14]*/
/*argv_entry *local_argv_head = [rbp - 0x1c]*/
/*argv_entry *temp_argv_head = [rbp - 0x24]*/
/*size_t rax_save = [rbp - 0x2c]*/
/*size_t r9 = [rbp - 0x34]*/
/*size_t r8 = [rbp - 0x3c]*/
/*size_t rcx = [rbp - 0x44]*/
/*size_t rdx = [rbp - 0x4c]*/
/*size_t rsi = [rbp - 0x54]*/
/*size_t rdi = [rbp - 0x5c]*/
/*size_t ptr_local_arg = [rbp - 0x64]*/

/*выделяем место для локальных переменных*/
/*sub rsp, 0x64*/
  src[offset++] = 0x48;
  src[offset++] = 0x83;
  src[offset++] = 0xec;
  src[offset++] = 0x64;		//смещение для переменных

/*Сохраняем аргументы в стек */
/*mov qword [rbp - 0x34], r9*/
  src[offset++] = 0x4c;
  src[offset++] = 0x89;
  src[offset++] = 0x8d;
  src[offset++] = 0xcc;		//смещение для переменных
  src[offset++] = 0xff;
  src[offset++] = 0xff;
  src[offset++] = 0xff;
/*mov qword [rbp - 0x3c], r8*/
  src[offset++] = 0x4c;
  src[offset++] = 0x89;
  src[offset++] = 0x85;
  src[offset++] = 0xc4;		//смещение для переменных
  src[offset++] = 0xff;
  src[offset++] = 0xff;
  src[offset++] = 0xff;
/*mov qword [rbp - 0x44], rcx*/
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0x8d;
  src[offset++] = 0xbc;		//смещение для переменных
  src[offset++] = 0xff;
  src[offset++] = 0xff;
  src[offset++] = 0xff;
  /*mov qword [rbp - 0x4c], rdx */
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0x95;
  src[offset++] = 0xb4;		//смещение для переменных
  src[offset++] = 0xff;
  src[offset++] = 0xff;
  src[offset++] = 0xff;
  /*mov qword [rbp - 0x54], rsi */
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0xb5;
  src[offset++] = 0xac;		//смещение для переменных
  src[offset++] = 0xff;
  src[offset++] = 0xff;
  src[offset++] = 0xff;
  /*mov qword [rbp - 0x5c], rdi */
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0xbd;
  src[offset++] = 0xa4;		//смещение для переменных
  src[offset++] = 0xff;
  src[offset++] = 0xff;
  src[offset++] = 0xff;

/*ptr_local_arg = addr(rdi[rbp - 0x5c])*/
/*mov rax, rbp*/
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0xe8;
/*sub rax, 0x5c*/
  src[offset++] = 0x48;
  src[offset++] = 0x83;
  src[offset++] = 0xe8;
  src[offset++] = 0x5c;
/* mov qword [rbp - 0x64], rax */
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0x45;
  src[offset++] = 0x9c;		//0x100-смещение_rbp

/*closure_ptr = 0x948349384983*/
/*сохраняем в стеке указатель на данные замыкания*/
/*mov rax, closure_ptr*/
  src[offset] = 0x48;
  src[offset + 1] = 0xb8;
  *((size_t *) (src + offset + 2)) = (size_t) closure_ptr;
  offset += 10;
/* mov qword [rbp - 8], rax */
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0x45;
  src[offset++] = 0xf8;		//0x100-смещение_rbp

/*argc = 0;*/
/*xor rax, rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x31;
  src[offset++] = 0xc0;
/*mov dword [rbp - 0xc], eax*/
  src[offset++] = 0x89;
  src[offset++] = 0x45;
  src[offset++] = 0xf4;		//0x100-смещение_rbp

/*local_argv_head = NULL*/
/*xor rax, rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x31;
  src[offset++] = 0xc0;
/* mov qword [rbp - 0x1c], rax */
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0x45;
  src[offset++] = 0xe4;		//0x100-смещение_rbp

/*int argc_remains = closure_ptr->argc_remains;*/
/*mov rax, qword [rbp - 8]*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x45;
  src[offset++] = 0xf8;		//0x100-смещение_rbp
/*mov eax, dword [rax + 0x20]*/
  src[offset++] = 0x8b;
  src[offset++] = 0x40;
  src[offset++] = 0x20;
/*mov dword [rbp - 0x14], eax*/
  src[offset++] = 0x89;
  src[offset++] = 0x45;
  src[offset++] = 0xec;		//0x100-смещение_rbp

/*Начало while (argc_remains)*/
/*jmp смещение проверки*/
  src[offset++] = 0xeb;
  src[offset++] = 0x56;		//относительное смещение(длина тела цикла)

/*Начало тела цикла*/

/*argc++;*/
/*add dword [rbp - 0xc], 1*/
  src[offset++] = 0x83;
  src[offset++] = 0x45;
  src[offset++] = 0xf4;		//0x100-смещение_rbp
  src[offset++] = 0x01;

/*if (argc == 7)
     ptr_local_arg = rbp+0x10;
адрес последнего аргумента переданного через стек = rbp+0x10
между ними адрес предыдущего rbp и адрес возврата
*/
/*cmp dword [rbp - 0xc, 7*/
  src[offset++] = 0x83;
  src[offset++] = 0x7d;
  src[offset++] = 0xf4;
  src[offset++] = 0x07;
/*jne количество байт до конца условия*/
  src[offset++] = 0x75;
  src[offset++] = 0x0b;		//относительное смещение
/*ptr_local_arg = rbp + 0x8*/
/*mov rax, rbp*/
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0xe8;
/*add rax, 0x10*/
  src[offset++] = 0x48;
  src[offset++] = 0x83;
  src[offset++] = 0xc0;
  src[offset++] = 0x10;
/* mov qword [rbp - 0x64], rax */
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0x45;
  src[offset++] = 0x9c;		//0x100-смещение_rbp

/*сохраняем локальные переменные в связный список*/
/*local_argv_head = closure_save_arg (local_argv_head, qword[ptr_local_arg], qword[ptr_local_arg+8])*/
/*помещаем 1-й аргумент в регистр rdi*/
/*mov rax, qword [rbp - 0x1c]*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x45;
  src[offset++] = 0xe4;		//0x100-смещение_rbp
/*mov rdi, rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0xc7;
/*помещаем 2-й аргумент в rsi*/
/*mov rax, qword [rbp - 0x64]*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x45;
  src[offset++] = 0x9c;		//0x100-смещение_rbp
/*mov rsi, qword [rax]*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x30;
/*помещаем 3-й аргумент в rdx*/
/*ptr_local_arg+=8;*/
/*add qword [rbp - 0x64], 8*/
  src[offset++] = 0x48;
  src[offset++] = 0x83;
  src[offset++] = 0x45;
  src[offset++] = 0x9c;
  src[offset++] = 0x08;
/*mov rax, qword [rbp - 0x64]*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x45;
  src[offset++] = 0x9c;		//0x100-смещение_rbp
/*mov rdx, qword [rax]*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x10;

/*Делаем вызов функции*/
/*сохраняем в стеке IP первого байта после вызова функции*/
/*mov rax, addr*/
  src[offset] = 0x48;
  src[offset + 1] = 0xb8;
//+количество байт до первого байта после вызова функции
  *((size_t *) (src + offset + 2)) = (size_t) (src + offset + 26);
  offset += 10;
/*push rax*/
  src[offset++] = 0x50;
/*mov rax, func*/
  src[offset] = 0x48;
  src[offset + 1] = 0xb8;
  *((size_t *) (src + offset + 2)) = (size_t) closure_save_arg;
  offset += 10;
/*push rax*/
  src[offset++] = 0x50;
/*xor rax, rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x31;
  src[offset++] = 0xc0;
/*прыгаем в ранее сохраненную функцию в стеке*/
/*ret*/
  src[offset++] = 0xc3;

/*сохраняем результат в local_argv_head*/
/*mov qword [rbp - 0x1с], rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0x45;
  src[offset++] = 0xe4;		//0x100-смещение_rbp

/*ptr_local_arg+=8;*/
/*add qword [rbp - 0x64], 8*/
  src[offset++] = 0x48;
  src[offset++] = 0x83;
  src[offset++] = 0x45;
  src[offset++] = 0x9c;
  src[offset++] = 0x08;

/*argc_remains--;*/
/*sub dword [rbp - 0x14], 1*/
  src[offset++] = 0x83;
  src[offset++] = 0x6d;
  src[offset++] = 0xec;		//0x100-смещение_rbp
  src[offset++] = 0x01;

/*Конец тела цикла*/

/*cmp dword [rbp - 0x14], 0*/
  src[offset++] = 0x83;
  src[offset++] = 0x7d;
  src[offset++] = 0xec;		//0x100-смещение_rbp
  src[offset++] = 0x00;
/*jne конец_условия*/
  src[offset++] = 0x75;
  src[offset++] = 0xa4;		//относительное смещение(-6 - длина тела цикла)

/*Конец while (argc_remains)*/


/*argc_all = closure_ptr->argc_all;*/
/*mov rax, qword [rbp - 8]*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x45;
  src[offset++] = 0xf8;		//0x100-смещение_rbp
/*mov eax, dword [rax]*/
  src[offset++] = 0x8b;
  src[offset++] = 0x00;
/*mov dword [rbp - 0x10], eax*/
  src[offset++] = 0x89;
  src[offset++] = 0x45;
  src[offset++] = 0xf0;		//0x100-смещение_rbp

/*temp_argv_head = local_argv_head;*/
/*mov rax, qword [rbp - 0x1c]*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x45;
  src[offset++] = 0xe4;		//0x100-смещение_rbp
/*mov qword [rbp - 0x24], rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0x45;
  src[offset++] = 0xdc;		//0x100-смещение_rbp

/*Заполняем стек параметрами и регистры аргументами
в регистр попадаю первые 6 аргументов, остальные в стек
заполняем задом наперед, как компилятор, начиная с самого дальнего аргумента:
сначала стек, а потом регистры по убыванию аргументов
*/

/*Начало while (argc_all)*/
/*jmp смещение проверки*/
  src[offset++] = 0xeb;
  src[offset++] = 0x75;		//относительное смещение(длина тела цикла)

/*Начало тела цикла*/

/*заполняем регистры в обратном порядке*/

/*if (!temp_argv_head)
     temp_argv_head = closure_ptr->head;*/
/*cmp qword [rbp - 0x24], 0*/
  src[offset++] = 0x48;
  src[offset++] = 0x83;
  src[offset++] = 0x7d;
  src[offset++] = 0xdc;		//0x100-смещение_rbp
  src[offset++] = 0x00;
/*jne конец условия*/
  src[offset++] = 0x75;
  src[offset++] = 0x0c;
/*mov rax, qword [rbp - 0x8]*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x45;
  src[offset++] = 0xf8;		//0x100-смещение_rbp
/*mov rax, qword [rax + 0x10](получение head)*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x40;
  src[offset++] = 0x10;
/*mov qword [rbp - 0x24], rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0x45;
  src[offset++] = 0xdc;		//0x100-смещение_rbp

/*аргумент попадает в rax, а затем либо в стек, либо в регистры*/
/*mov rax, qword [rbp - 0x24]*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x45;
  src[offset++] = 0xdc;		//0x100-смещение_rbp
/*mov rax, qword [rax + 0x8](получение value)*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x40;
  src[offset++] = 0x08;		//0x100-смещение_rbp

/*начало_ветвления_по_аргументам*/
/*Начало стек*/
/*if (argc_all > 6)*/
/*cmp dword [rbp - 0x10], 6*/
  src[offset++] = 0x83;
  src[offset++] = 0x7d;
  src[offset++] = 0xf0;		//0x100-смещение_rbp
  src[offset++] = 0x06;
/*jle else if*/
  src[offset++] = 0x7e;
  src[offset++] = 0x03;		//относительное смещение
/*push rax*/
  src[offset++] = 0x50;
/*jmp конец_ветвления_по_аргументам*/
  src[offset++] = 0xeb;
  src[offset++] = 0x42;		//относительное смещение
/*Конец стек*/
/*Начало r9*/
/*  if (argc_all == 6)*/
/*cmp dword [rbp - 0x10], 6*/
  src[offset++] = 0x83;
  src[offset++] = 0x7d;
  src[offset++] = 0xf0;		//0x100-смещение_rbp
  src[offset++] = 0x06;
/*jne else if*/
  src[offset++] = 0x75;
  src[offset++] = 0x05;		//относительное смещение
/*mov r9, rax*/
  src[offset++] = 0x49;
  src[offset++] = 0x89;
  src[offset++] = 0xc1;
/*jmp конец_ветвления_по_аргументам*/
  src[offset++] = 0xeb;
  src[offset++] = 0x37;		//относительное смещение
/*Конец r9*/
/*Начало r8*/
/*  if (argc_all == 5)*/
/*cmp dword [rbp - 0x10], 5*/
  src[offset++] = 0x83;
  src[offset++] = 0x7d;
  src[offset++] = 0xf0;		//0x100-смещение_rbp
  src[offset++] = 0x05;
/*jne else if*/
  src[offset++] = 0x75;
  src[offset++] = 0x05;		//относительное смещение
/*mov r8, rax*/
  src[offset++] = 0x49;
  src[offset++] = 0x89;
  src[offset++] = 0xc0;
/*jmp конец_ветвления_по_аргументам*/
  src[offset++] = 0xeb;
  src[offset++] = 0x2c;		//относительное смещение
/*Конец r8*/
/*Начало rcx*/
/*  if (argc_all == 4)*/
/*cmp dword [rbp - 0x10], 4*/
  src[offset++] = 0x83;
  src[offset++] = 0x7d;
  src[offset++] = 0xf0;		//0x100-смещение_rbp
  src[offset++] = 0x04;
/*jne else if*/
  src[offset++] = 0x75;
  src[offset++] = 0x05;		//относительное смещение
/*mov rcx, rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0xc1;
/*jmp конец_ветвления_по_аргументам*/
  src[offset++] = 0xeb;
  src[offset++] = 0x21;		//относительное смещение
/*Конец rcx*/
/*Начало rdx*/
/*  if (argc_all == 3)*/
/*cmp dword [rbp - 0x10], 3*/
  src[offset++] = 0x83;
  src[offset++] = 0x7d;
  src[offset++] = 0xf0;		//0x100-смещение_rbp
  src[offset++] = 0x03;
/*jne else if*/
  src[offset++] = 0x75;
  src[offset++] = 0x05;		//относительное смещение
/*mov rdx, rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0xc2;
/*jmp конец_ветвления_по_аргументам*/
  src[offset++] = 0xeb;
  src[offset++] = 0x16;		//относительное смещение
/*Конец rdx*/
/*Начало rsi*/
/*  if (argc_all == 2)*/
/*cmp dword [rbp - 0x10], 2*/
  src[offset++] = 0x83;
  src[offset++] = 0x7d;
  src[offset++] = 0xf0;		//0x100-смещение_rbp
  src[offset++] = 0x02;
/*jne else if*/
  src[offset++] = 0x75;
  src[offset++] = 0x05;		//относительное смещение
/*mov rsi, rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0xc6;
/*jmp конец_ветвления_по_аргументам*/
  src[offset++] = 0xeb;
  src[offset++] = 0x0b;		//относительное смещение
/*Конец rsi*/
/*Начало rdi*/
/*  if (argc_all == 1)*/
/*cmp dword [rbp - 0x10], 1*/
  src[offset++] = 0x83;
  src[offset++] = 0x7d;
  src[offset++] = 0xf0;		//0x100-смещение_rbp
  src[offset++] = 0x01;
/*jne else if*/
  src[offset++] = 0x75;
  src[offset++] = 0x05;		//относительное смещение
/*mov rdi, rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0xc7;
/*jmp конец_ветвления_по_аргументам*/
  src[offset++] = 0xeb;
  src[offset++] = 0x00;		//относительное смещение
/*Конец rdi*/

/*конец_ветвления_по_аргументам*/

/*temp_argv_head = temp_argv_head->next;*/
/*mov rax, qword [rbp - 0x24]*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x45;
  src[offset++] = 0xdc;		//0x100-смещение_rbp
/*mov rax, qword [rax](получение next по указателю)*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x00;
/*mov qword [rbp - 0x24], rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0x45;
  src[offset++] = 0xdc;		//0x100-смещение_rbp

/*argc_all--;*/
/*sub dword [rbp - 0x10], 1*/
  src[offset++] = 0x83;
  src[offset++] = 0x6d;
  src[offset++] = 0xf0;		//0x100-смещение_rbp
  src[offset++] = 0x01;

/*Конец тела цикла*/

/*cmp dword [rbp - 0x10], 0*/
  src[offset++] = 0x83;
  src[offset++] = 0x7d;
  src[offset++] = 0xf0;		//0x100-смещение_rbp
  src[offset++] = 0x00;
/*jne конец_условия*/
  src[offset++] = 0x75;
  src[offset++] = 0x85;		//относительное смещение(-6 - длина тела цикла)

/*Конец while (argc_all)*/

/*Делаем вызов функции*/
/*сохраняем в стеке IP первого байта после вызова функции*/
/*mov rax, addr*/
  src[offset] = 0x48;
  src[offset + 1] = 0xb8;
//+количество байт до первого байта после вызова функции
  *((size_t *) (src + offset + 2)) = (size_t) (src + offset + 26);
  offset += 10;
/*push rax*/
  src[offset++] = 0x50;
/*mov rax, func*/
  src[offset] = 0x48;
  src[offset + 1] = 0xb8;
  *((size_t *) (src + offset + 2)) = (size_t) closure_ptr->func;
  offset += 10;
/*push rax*/
  src[offset++] = 0x50;
/*xor rax, rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x31;
  src[offset++] = 0xc0;
/*прыгаем в ранее сохраненную функцию в стеке*/
/*ret*/
  src[offset++] = 0xc3;

/*сохраняем rax в переменную*/
/* mov qword [rbp - 2c], rax */
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0x45;
  src[offset++] = 0xd4;		//0x100-смещение_rbp

/*очищаем список локальных переменных
  free_argv (local_argv_head);*/
/*помещаем 1-й аргумент в регистр rdi*/
/*mov rax, qword [rbp - 0x1c]*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x45;
  src[offset++] = 0xe4;		//0x100-смещение_rbp
/*mov rdi, rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0xc7;
/*Делаем вызов функции*/
/*сохраняем в стеке IP первого байта после вызова функции*/
/*mov rax, addr*/
  src[offset] = 0x48;
  src[offset + 1] = 0xb8;
//+количество байт до первого байта после вызова функции
  *((size_t *) (src + offset + 2)) = (size_t) (src + offset + 26);
  offset += 10;
/*push rax*/
  src[offset++] = 0x50;
/*mov rax, func*/
  src[offset] = 0x48;
  src[offset + 1] = 0xb8;
  *((size_t *) (src + offset + 2)) = (size_t) free_argv;
  offset += 10;
/*push rax*/
  src[offset++] = 0x50;
/*xor rax, rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x31;
  src[offset++] = 0xc0;
/*прыгаем в ранее сохраненную функцию в стеке*/
/*ret*/
  src[offset++] = 0xc3;

/*восстановили rax*/
/*mov rax, qword [rbp - 0x2c]*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x45;
  src[offset++] = 0xd4;		//0x100-смещение_rbp

/*leave*/
/*Превращается в следующие команды:*/
/*mov rsp, rbp*/
/*pop rbp*/
/*Команда LEAVE имеет действие, противоположное команде ENTER. Фактически команда LEAVE только копирует содержимое EBP в ESP, тем самым выбрасывая из стека весь кадр, созданный командой ENTER, и считывает из стека значение регистра EBP для предыдущей процедуры.*/
  src[offset++] = 0xc9;

/*ret*/
  src[offset++] = 0xc3;

  return (closure_t) (src + sizeof (closure_data));

}


void
delete_closure (closure_t adder)
{

  closure_data *cd = (closure_data *) ((size_t)adder - sizeof (closure_data));

  free_argv (cd->head);

  munmap (cd, cd->src_len);

}

int
main ()
{

#if 0
  printf ("1: %ld \n", sum (9, 1, 2, 3, 4, 5, 6, 7, 8, 9));

  closure_t func = closure_make (sum, 10, 2, 9, 1);
  size_t res = func (2, 3, 4, 5, 6, 7, 8, 9);

  printf ("2: %ld \n", res);

  closure_t func_new = closure_make_new (sum, 10, 2, 9, 1);
  size_t res_new = func_new (2, 3, 4, 5, 6, 7, 8, 9);

  printf ("3: %ld \n", res_new);

  delete_closure (func);
  delete_closure (func_new);

#endif

#if 0
  printf ("1: %ld \n", fix_sum (1, 2));

  closure_t func = closure_make (fix_sum, 2, 1, 1);
  size_t res = func (2);

  printf ("2: %ld \n", res);

  closure_t func_new = closure_make_new (fix_sum, 2, 1, 1);
  size_t res_new = func_new (2);

  printf ("3: %ld \n", res_new);

  delete_closure (func);
  delete_closure (func_new);

#endif

#if 0
  printf ("1: %ld \n", fix_sum_char (2, 2));

  closure_t func = closure_make (fix_sum_char, 2, 1, 2);
  size_t res = func (2);

  printf ("2: %ld \n", res);

  closure_t func_new = closure_make_new (fix_sum_char, 2, 1, 2);
  size_t res_new = func_new (2);

  printf ("3: %ld \n", res_new);


  delete_closure (func);
  delete_closure (func_new);

#endif

#if 1
  char *pattern = "%s%d\n";
  printf (pattern, "Простая функция", 5);

  char *text = "Функция замыкания";
  closure_t func = closure_make (printf, 3, 1, pattern, strlen (pattern) + 1);
  size_t res = func (text, strlen (text) + 1, 5, 0);

  delete_closure (func);

#endif



  return 0;
}
