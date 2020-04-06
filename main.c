#include <xc.h>

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

void Init_System(void)
{
    // Enable PLL
    OSCTUNEbits.PLLEN = 1;
    // PLL outputs 48 MHz and CPU clock source is 48 MHz / 3 = 16 MHz (as per configuration bits)
    // System clock from primary clock source (0b00)
    OSCCONbits.SCS = 0x0;
    // Output reference clock on REFO (RB2) configuration
    REFOCONbits.ROON = 1; // Enable output on REFO
    REFOCONbits.ROSEL = 0; // Clock using FOSC
    REFOCONbits.RODIV = 0x0; // No divisor
}

void Init_Gpio(void)
{
    // Set RB0 as an output and set it low
    TRISBbits.TRISB0 = 0;
    LATBbits.LATB0 = 0;
    // Set RB0/REFO as an output for FOSC
    TRISBbits.TRISB2 = 0;
    // Setup RC7 and RC6 for UART RX/TX
    TRISCbits.TRISC7 = 1; // RX - input
    TRISCbits.TRISC6 = 0; // TX - output
}

void Init_Timer0(void)
{
    T0CONbits.T0CS = 0;     // Internal clock (FOSC /4)
    T0CONbits.PSA = 0;      // Use prescaler
    T0CONbits.T0PS = 0x1;   // Use a prescaler value of 4 (0b001) to get a timer frequency of 1 MHz
    T0CONbits.T08BIT = 0;   // 16-bit counter mode
    T0CONbits.TMR0ON = 1;   // Enable timer
}

void Init_Eusart1(void)
{
    RCSTA1bits.SPEN = 1;    // Serial port enable
    TXSTA1bits.SYNC = 0;    // Asynchronous operation
    RCSTA1bits.CREN = 1;    // Enable receive
    TXSTA1bits.TXEN = 1;    // Enable transmit
    TXSTA1bits.BRGH = 1;    // High baud rate select bit
    BAUDCON1bits.BRG16 = 1; // 16 bit baud rate generator
    // For a device with Fosc of 16 MHz, desired baud rate of 115200,
    // Asynchronous mode, and 16-bit BRG:
    // Desired Baud Rate = Fosc/(4 ([SPBRGHx:SPBRGx] + 1))
    // Solving for SPBRGHx:SPBRGx:
    // X = ((Fosc/Desired Baud Rate)/4) - 1
    // = ((16000000/115200)/4) - 1
    // = [33.72] = 34 (our value for the baud rate registers)
    // Calculated Baud Rate = 16000000/(4 (34 + 1))
    // = 114285.7
    // Error = (Calculated Baud Rate - Desired Baud Rate)/Desired Baud Rate
    // = (114285.7 - 115200)/115200 = -0.794%
    SPBRGH1 = 0x00;
    SPBRG1  = 0x22;
}

void putch(char c)
{
    while (!TXIF); // Wait until peripheral is free
    TXREG1 = c; // Load character into transmit shift register
}

int getch(void)
{
    int c;
    
    while (!RCIF); // Wait for data to be received
    c = (char)RCREG1;
    
    return c;
}

int getche(void)
{
    int c;
    
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

void generatePulse(void)
{
    // The following generates an approximately 1us pulse
    LATBbits.LATB0 = 1;
    NOP();
    NOP();
    NOP();
    LATBbits.LATB0 = 0;
}

void waitMicroseconds(uint16_t microseconds)
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

void main(void)
{   
    char ioBuff[128] = {0};
    
    Init_System();
    Init_Gpio();
    Init_Timer0();
    Init_Eusart1();
    
    // Wait a bit for things to stabilize
    waitMicroseconds(1000);
    
    printf("\r\nHello there! What's the string?\r\n");
    scanf("%s", ioBuff);
    printf("I think %s is a pretty cool string too.\r\n", ioBuff);
    
    for (;;)
    {
        // Generate a pulse
        generatePulse();
        // Wait 100us
        waitMicroseconds(100);
    }
}

