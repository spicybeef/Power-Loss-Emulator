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
    Init_System();
    Init_Gpio();
    Init_Timer0();
    Init_Eusart1();
    
    // Wait a bit for things to stabilize
    Util_WaitMicroseconds(500);
    
    // Setup console interface
    Console_Init(&splashScreen, NUM_SPLASH_LINES, &mainMenu);
    // Erase screen
    Console_Print(ERASE_SCREEN);
    // Start console interface
    Console_Main(); // Does not return
    
    // Unreachable code.
    // SO STUPID: In order for vprintf to work, you need to have a printf with
    // the formats, otherwise the XC8 compiler doesn't build in support for it.
    printf("%c%s%d",0,0,0);
}

