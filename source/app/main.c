#include "./main.h"
#include <stdio.h>
#include "tonc.h"

int main()
{
    REG_DISPCNT= DCNT_MODE0 | DCNT_BG0;
    tte_init_se_default(0, BG_CBB(0)|BG_SBB(31));
    tte_init_con();

    tte_printf("Hello, World!\n\n");

    int result = foo();
    result += bar(3, 2);
    tte_printf("The result = %d.\n\n", result);
    
    tte_printf("Kind regards,\nLuc van den Brand.");

    while(1);
}
