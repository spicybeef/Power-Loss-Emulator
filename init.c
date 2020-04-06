#include <xc.h>

#include "init.h"

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