## About
Closure is a lightweight library written in C.

## Features
* Lightweight (only 1 files)
* Simple API
* c11 compatible
* Only for GCC compiler
* Only x86-64 platform
* Test suites

## Installation
```
git clone https://github.com/ProhorP/closure_lib.git
```
and copy closure.h to you source code tree.

## Test
Run:
```
 cd tests/test01
or
 cd tests/test02
or
 cd tests/test03
or
 cd tests/test04
or
 cd tests/test05
```
```make test``` to compile and run tests.
```make clean``` to clean bin files.

## Examples

```
#define CLOSURE_LIB
#include "closure.h"
#include <stdio.h>

int
main ()
{

  typedef void (*closure_t) (char *, ...);

  char *pattern = malloc (100);
  strcpy (pattern, "%s%d\n");

  printf (pattern, "Simple function", 5);

  closure_t func;
  CLOSURE_MAKE (func, printf, 3, 1,
                CLOSURE_POINTER (pattern, strlen (pattern) + 1));

  free (pattern);
  func ("Closure function", 5);

  CLOSURE_DELETE (func);

  return EXIT_SUCCESS;
}

```

## Contributing

I will always merge *working* bug fixes. However, if you want to add something new to the API, please create an "issue" on github for this first so we can discuss if it should end up in the library before you start implementing it.
Remember to follow closure's code style and write appropriate tests.

## License
[not](http://opensource.org/licenses/mit-license.php)
