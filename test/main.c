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
  int argc_all;
  closure_t func;
  argv_entry *head;
  size_t src_len;
int argc_remains;
};

typedef struct closure_data closure_data;

void
free_argv (argv_entry * head)
{

  if (!head)
    return;

  argv_entry *temp = head->next;
  free (head);

  free_argv (temp);

}

int main(){
int argc;
closure_data *closure_ptr;
  int argc_all;

argv_entry *temp_argv_head;
free_argv(temp_argv_head);

//argc_all = closure_ptr->argc_all;

/*
argv_entry *temp_argv_head;
argv_entry *local_argv_head;
int use_stack;

use_stack = 0;
temp_argv_head = local_argv_head;

while (argc_all)
{
  if (argc_all > 6)
    {
      if (!temp_argv_head)
        temp_argv_head = closure_ptr->head;

      temp_argv_head = temp_argv_head->next;
      argc_all--;

    }
}
*/

}
