#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <uchar.h>
#include <sys/mman.h>

size_t abc_n(size_t dummy, ...)
{
size_t a = 1;
return a + 1;
}

int main(){

size_t res1 = abc_n(1, 2, 3, 4, 5, 6, 7, 8);

}
