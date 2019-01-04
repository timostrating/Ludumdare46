#include "./main.h"
#include "minunit.h"

char * barTest()
{
    int result = bar (3, 2);
    mu_assert("bar - 3 + 2 must be 5.", result == 5);
    return 0;
}
