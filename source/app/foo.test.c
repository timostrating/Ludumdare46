#include "./main.h"
#include "minunit.h"

char * fooTest()
{
    int result = foo();
    mu_assert("foo - foo should always return 3", result == 3);
    return 0;
}
