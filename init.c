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
    // (48 MHz)/(4 FOSC)/(4 PRESCALER) = 3 MHz tick rate
    T0CONbits.T0CS = 0;     // Internal clock (FOSC /4)
    T0CONbits.PSA = 0;      // Use prescaler
    T0CONbits.T0PS = 0x1;   // Use a prescaler value of 4 (0b001) to get a timer frequency of 1 MHz
    T0CONbits.T08BIT = 0;   // 16-bit counter mode
    T0CONbits.TMR0ON = 1;   // Enable timer
}

void Init_Timer1(void)
{
    // (48 MHz)/(4 FOSC) = 12 MHz tick rate
    T1CONbits.TMR1CS = 0x0; // 0b00 = Timer1 clock source is the instruction clock (FOSC /4)
    T1CONbits.T1CKPS = 0x0; // 0b00 = 1:1 Prescale value
    T1CONbits.T1OSCEN = 0;  // 0b0 = Timer1 crystal driver is off
    T1CONbits.RD16 = 1;     // 0b1 = Enables register read/write of Timer1 in one 16-bit operation
    T1CONbits.TMR1ON = 1;   // 0b0 = Timer1 crystal driver is off
}

void Init_Timer2(void)
{
    // (48 MHz)/(4 FOSC) = 12 MHz tick rate
    T2CONbits.T2OUTPS = 0x0;// 0b0000 = 1:1 Postscale
    T2CONbits.T2CKPS = 0x0; // 0b00 = Prescaler is 1
    PR2 = 120;              // 12 ticks = 1 us compare rate
    T2CONbits.TMR2ON = 1;   // 0b1 = Timer2 is on
    PIE1bits.TMR2IE = 1;    // Timer2 match interrupt enable
}

void Init_Timer3(void)
{
    // (48 MHz)/(4 FOSC) = 12 MHz tick rate
    T3CONbits.TMR3CS = 0x0; // 0b00 = Timer1 clock source is the instruction clock (FOSC /4)
    T3CONbits.T3CKPS = 0x0; // 0b00 = 1:1 Prescale value
    T3CONbits.T3OSCEN = 0;  // 0b0 = Timer3 crystal driver is off
    T3CONbits.RD16 = 1;     // 0b1 = Enables register read/write of Timer1 in one 16-bit operation
    T3CONbits.TMR3ON = 1;   // 0b1 = Timer3 is on
}

void Init_Eccp1(void)
{
    // (48 MHz)/(4 FOSC) = 12 MHz tick rate
    TCLKCONbits.T3CCP2 = 1; // 0b10 = ECCP1 and ECCP2 both use Timer3 (capture/compare) and Timer4 (PWM)
    TCLKCONbits.T3CCP1 = 0; // 0b10 = ECCP1 and ECCP2 both use Timer3 (capture/compare) and Timer4 (PWM)
    PIE1bits.CCP1IE = 1;    // Enable ECCP1 interrupt
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
    // = ((48000000/115200)/4) - 1
    // = [103.167] = 103 (our value for the baud rate registers)
    // Calculated Baud Rate = 48000000/(4 (34 + 1))
    // = 114285.7
    // Error = (Calculated Baud Rate - Desired Baud Rate)/Desired Baud Rate
    // = (115384.6 - 115200)/115200 = 0.00160%
    SPBRGH1 = 0x00;
    SPBRG1  = 0x67;
}

void Init_Interrupts(void)
{
    INTCONbits.GIE = 1;     // 0b1 = Enables all unmasked interrupts
    INTCONbits.PEIE = 1;    // 0b1 = Enables all unmasked peripheral interrupts (when GIE is also set)
}