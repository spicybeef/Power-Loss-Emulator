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
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#include "utils.h"

void putch(char c)
{
    while (!TXIF); // Wait until peripheral is free
    TXREG1 = c; // Load character into transmit shift register
}

char getch(void)
{
    char c;
    
    while (!RCIF); // Wait for data to be received
    c = (char)RCREG1;
    
    return c;
}

char getche(void)
{
    char c;
    
    c = getch();
    putch(c);
    
    return c;
}

int scanf(const char *format, ...)
{
    va_list vl;
    uint16_t i = 0;
    uint16_t j = 0;
    uint16_t ret = 0;
    char buff[128] = {0};
    char c;
    char *endPointer;

    // Read in line
    do
    {
        c = (char)getche();
        if (c == '\r')
        {
            buff[i] = 0;
        }
        else
        {
            buff[i] = c;
        }
        i++;
    }
    while(c != '\r');

    va_start( vl, format );
    i = 0;
    while (format && format[i])
    {
        if (format[i] == '%') 
        {  
            i++;
            switch (format[i]) 
            {
                case 'c': 
                    *(char *) va_arg(vl, char*) = buff[j];
                    j++;
                    ret++;
                    break;
                case 'd': 
                    *(int *) va_arg(vl, int*) = strtol(&buff[j], &endPointer, 10);
                    j += endPointer - &buff[j];
                    ret++;
                    break;
                case 'x': 
                    *(int *) va_arg(vl, int*) = strtol(&buff[j], &endPointer, 16);
                    j += endPointer - &buff[j];
                    ret++;
                    break;
                case 's':
                    strcpy((char *)va_arg(vl, char*), &buff[j]);
                    j += (strlen(&buff[j]) + 1);
                    ret += (strlen(&buff[j]) + 1);
                    break;
                
            }
        } 
        else 
        {
            j++;
        }
        i++;
    }
    va_end(vl);
    return ret;
}

void Util_GeneratePulse(void)
{
    // The following generates an approximately 1us pulse
    LATBbits.LATB0 = 1;
    NOP();
    NOP();
    NOP();
    LATBbits.LATB0 = 0;
}

void Util_WaitMicroseconds(uint16_t microseconds)
{
    uint16_t startTime;
    uint16_t current;
    
    startTime = TMR0;
    do
    {
        current = TMR0;
    }
    while ((current - startTime)<100);
}