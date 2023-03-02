#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <uchar.h>
#include <sys/mman.h>

size_t abc(size_t a, size_t b, size_t c, size_t d, size_t e, size_t f){

return a+b+c+d+e+f;

}

size_t xyz(size_t a, size_t b, size_t c, size_t d, size_t e, size_t f){

return abc(a,b,c,d,e,f);

}

int main(){

size_t res = xyz(0x1111111111111111,0x1111111111111111,0x1111111111111111,0x1111111111111111,0x1111111111111111,0x1111111111111111);


}
