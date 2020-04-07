#include <xc.h>

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "init.h"
#include "utils.h"
#include "console.h"
#include "menus.h"

void main(void)
{   
    char ioBuff[128] = {0};
    
    Init_System();
    Init_Gpio();
    Init_Timer0();
    Init_Eusart1();
    
    // Wait a bit for things to stabilize
    Util_WaitMicroseconds(1000);
    
    printf("\r\nHello there! What's the string?\r\n");
    scanf("%s", ioBuff);
    printf("I think %s is a pretty cool string too.\r\n", ioBuff);

    Console_PrintMenu(&mainMenu);
    
    for (;;)
    {
        // Generate a pulse
        Util_GeneratePulse();
        // Wait 100us
        Util_WaitMicroseconds(100);
    }
}

