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
/*int use_stack = [rbp - 0x14]*/
/*argv_entry *local_argv_head = [rbp - 0x1c]*/
/*argv_entry *temp_argv_head = [rbp - 0x24]*/
/*size_t rax_save = [rbp - 0x2c]*/


/*выделяем место для локальных переменных*/
/*sub rsp, 0x2c*/
  src[offset++] = 0x48;
  src[offset++] = 0x83;
  src[offset++] = 0xec;
  src[offset++] = 0x2c;//смещение для переменных

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
  src[offset++] = 0xf8;//0x100-смещение_rbp

/*argc_all = closure_ptr->argc_all;*/
/*mov rax, qword [rbp - 8]*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x45;
  src[offset++] = 0xf8;//0x100-смещение_rbp
/*mov eax, dword [rax]*/
  src[offset++] = 0x8b;
  src[offset++] = 0x00;
/*mov dword [rbp - 0x10], eax*/
  src[offset++] = 0x89;
  src[offset++] = 0x45;
  src[offset++] = 0xf0;//0x100-смещение_rbp


/*temp_argv_head = local_argv_head;*/
/*mov rax, qword [rbp - 0x1c]*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;        
  src[offset++] = 0x45;
  src[offset++] = 0xe4;//0x100-смещение_rbp
/*mov qword [rbp - 0x24], rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x89;        
  src[offset++] = 0x45;
  src[offset++] = 0xdc;//0x100-смещение_rbp

/*Заполняем стек параметрами и регистры аргументами
в регистр попадаю первые 6 аргументов, остальные в стек
заполняем задом наперед, как компилятор, начиная с самого дальнего аргумента:
сначала стек, а потом регистры по убыванию аргументов
*/

/*Начало while (argc_all)*/
/*jmp смещение проверки*/
  src[offset++] = 0xeb;
  src[offset++] = 0x75;//относительное смещение(длина тела цикла)

/*Начало тела цикла*/

/*заполняем регистры в обратном порядке*/

/*if (!temp_argv_head)
     temp_argv_head = closure_ptr->head;*/
/*cmp qword [rbp - 0x24], 0*/
  src[offset++] = 0x48;
  src[offset++] = 0x83;
  src[offset++] = 0x7d;
  src[offset++] = 0xdc;//0x100-смещение_rbp
  src[offset++] = 0x00;
/*jne конец условия*/
  src[offset++] = 0x75;
  src[offset++] = 0x0c;
/*mov rax, qword [rbp - 0x8]*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x45;
  src[offset++] = 0xf8;//0x100-смещение_rbp
/*mov rax, qword [rax + 0x10](получение head)*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x40;
  src[offset++] = 0x10;
/*mov qword [rbp - 0x24], rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0x45;
  src[offset++] = 0xdc;//0x100-смещение_rbp

/*аргумент попадает в rax, а затем либо в стек, либо в регистры*/
/*mov rax, qword [rbp - 0x24]*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x45;
  src[offset++] = 0xdc;//0x100-смещение_rbp
/*mov rax, qword [rax + 0x8](получение value)*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x40;
  src[offset++] = 0x08;//0x100-смещение_rbp

/*начало_ветвления_по_аргументам*/
/*Начало стек*/
/*if (argc_all > 6)*/
/*cmp dword [rbp - 0x10], 6*/
  src[offset++] = 0x83;
  src[offset++] = 0x7d;
  src[offset++] = 0xf0;//0x100-смещение_rbp
  src[offset++] = 0x06;
/*jle else if*/
  src[offset++] = 0x7e;
  src[offset++] = 0x03; //относительное смещение
/*push rax*/
  src[offset++] = 0x50;
/*jmp конец_ветвления_по_аргументам*/
  src[offset++] = 0xeb;
  src[offset++] = 0x42;//относительное смещение
/*Конец стек*/
/*Начало r9*/
/*  if (argc_all == 6)*/
/*cmp dword [rbp - 0x10], 6*/
  src[offset++] = 0x83;
  src[offset++] = 0x7d;
  src[offset++] = 0xf0;//0x100-смещение_rbp
  src[offset++] = 0x06;
/*jne else if*/
  src[offset++] = 0x75;
  src[offset++] = 0x05; //относительное смещение
/*mov r9, rax*/
  src[offset++] = 0x49;
  src[offset++] = 0x89;
  src[offset++] = 0xc1;
/*jmp конец_ветвления_по_аргументам*/
  src[offset++] = 0xeb;
  src[offset++] = 0x37;//относительное смещение
/*Конец r9*/
/*Начало r8*/
/*  if (argc_all == 5)*/
/*cmp dword [rbp - 0x10], 5*/
  src[offset++] = 0x83;
  src[offset++] = 0x7d;
  src[offset++] = 0xf0;//0x100-смещение_rbp
  src[offset++] = 0x05;
/*jne else if*/
  src[offset++] = 0x75;
  src[offset++] = 0x05; //относительное смещение
/*mov r8, rax*/
  src[offset++] = 0x49;
  src[offset++] = 0x89;
  src[offset++] = 0xc0;
/*jmp конец_ветвления_по_аргументам*/
  src[offset++] = 0xeb;
  src[offset++] = 0x2c;//относительное смещение
/*Конец r8*/
/*Начало rcx*/
/*  if (argc_all == 4)*/
/*cmp dword [rbp - 0x10], 4*/
  src[offset++] = 0x83;
  src[offset++] = 0x7d;
  src[offset++] = 0xf0;//0x100-смещение_rbp
  src[offset++] = 0x04;
/*jne else if*/
  src[offset++] = 0x75;
  src[offset++] = 0x05; //относительное смещение
/*mov rcx, rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0xc1;
/*jmp конец_ветвления_по_аргументам*/
  src[offset++] = 0xeb;
  src[offset++] = 0x21;//относительное смещение
/*Конец rcx*/
/*Начало rdx*/
/*  if (argc_all == 3)*/
/*cmp dword [rbp - 0x10], 3*/
  src[offset++] = 0x83;
  src[offset++] = 0x7d;
  src[offset++] = 0xf0;//0x100-смещение_rbp
  src[offset++] = 0x03;
/*jne else if*/
  src[offset++] = 0x75;
  src[offset++] = 0x05; //относительное смещение
/*mov rdx, rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0xc2;
/*jmp конец_ветвления_по_аргументам*/
  src[offset++] = 0xeb;  
  src[offset++] = 0x16;//относительное смещение
/*Конец rdx*/
/*Начало rsi*/
/*  if (argc_all == 2)*/
/*cmp dword [rbp - 0x10], 2*/
  src[offset++] = 0x83;
  src[offset++] = 0x7d;
  src[offset++] = 0xf0;//0x100-смещение_rbp
  src[offset++] = 0x02;
/*jne else if*/
  src[offset++] = 0x75;
  src[offset++] = 0x05; //относительное смещение
/*mov rsi, rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0xc6;
/*jmp конец_ветвления_по_аргументам*/
  src[offset++] = 0xeb;    
  src[offset++] = 0x0b;//относительное смещение
/*Конец rsi*/
/*Начало rdi*/
/*  if (argc_all == 1)*/
/*cmp dword [rbp - 0x10], 1*/
  src[offset++] = 0x83;
  src[offset++] = 0x7d;
  src[offset++] = 0xf0;//0x100-смещение_rbp
  src[offset++] = 0x01;
/*jne else if*/
  src[offset++] = 0x75;
  src[offset++] = 0x05; //относительное смещение
/*mov rdi, rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0xc7;
/*jmp конец_ветвления_по_аргументам*/
  src[offset++] = 0xeb;     
  src[offset++] = 0x00;//относительное смещение
/*Конец rdi*/

/*конец_ветвления_по_аргументам*/

/*temp_argv_head = temp_argv_head->next;*/
/*mov rax, qword [rbp - 0x24]*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x45;
  src[offset++] = 0xdc;//0x100-смещение_rbp
/*mov rax, qword [rax](получение next по указателю)*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x00;
/*mov qword [rbp - 0x24], rax*/
  src[offset++] = 0x48;
  src[offset++] = 0x89;
  src[offset++] = 0x45;
  src[offset++] = 0xdc;//0x100-смещение_rbp

/*argc_all--;*/
/*sub dword [rbp - 0x10], 1*/
  src[offset++] = 0x83;
  src[offset++] = 0x6d;
  src[offset++] = 0xf0;//0x100-смещение_rbp
  src[offset++] = 0x01;

/*Конец тела цикла*/

/*cmp dword [rbp - 0x10], 0*/
  src[offset++] = 0x83;
  src[offset++] = 0x7d;
  src[offset++] = 0xf0;//0x100-смещение_rbp
  src[offset++] = 0x00;
/*jne конец_условия*/
  src[offset++] = 0x75;
  src[offset++] = 0x85; //относительное смещение(-6 - длина тела цикла)

/*Конец while (argc_all)*/

/*Делаем вызов функции*/
/*сохраняем в стеке IP первого байта после вызова функции*/
/*mov rax, addr*/
  src[offset] = 0x48;
  src[offset + 1] = 0xb8;
//+количество байт до первого байта после вызова функции
  *((size_t *) (src + offset + 2)) = (size_t) (src + offset+26);
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
  src[offset++] = 0xd4;//0x100-смещение_rbp

/*очищаем список локальных переменных
  free_argv (local_argv_head);*/
/*помещаем 1-й аргумент в регистр rdi*/
/*mov rax, qword [rbp - 0x1c]*/
  src[offset++] = 0x48;
  src[offset++] = 0x8b;
  src[offset++] = 0x45;
  src[offset++] = 0xe4;//0x100-смещение_rbp
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
  *((size_t *) (src + offset + 2)) = (size_t) (src + offset+26);
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
  src[offset++] = 0xd4;//0x100-смещение_rbp

/*leave*/
/*Превращается в следующие команды:*/
/*mov rsp, rbp*/
/*pop rbp*/
/*Команда LEAVE имеет действие, противоположное команде ENTER. Фактически команда LEAVE только копирует содержимое EBP в ESP, тем самым выбрасывая из стека весь кадр, созданный командой ENTER, и считывает из стека значение регистра EBP для предыдущей процедуры.*/
  src[offset++] = 0xc9;

/*ret*/
  src[offset++] = 0xc3;

