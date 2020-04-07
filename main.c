/*
 * MIT License
 *
 * Copyright (c) 2020 Michel Kakulphimp
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

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

