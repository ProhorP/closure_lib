#include <stdio.h>

typedef int (*closure_t) (int);
typedef int (*ptr_func) (int);

closure_t closure_save_func;
int closure_save_val;


int
test_func (int a)
{
  return a + closure_save_val;
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

void hello(){

printf("%s", "Hello world!");

}

int
main ()
{
  int n = 3;

  closure_t adder = closure_make(test_func, n);

  printf ("%d\n", adder (2));	// ожидаем увидеть 5

  delete_closure (adder);

  return 0;
}
