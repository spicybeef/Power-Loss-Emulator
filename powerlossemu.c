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

#include "utils.h"
#include "powerlossemu.h"

static uint16_t startPeriod;
static uint16_t endPeriod;
static uint16_t rampPeriod;
static uint16_t rampSteps;
static uint16_t workloadLength;

void PowerLossEmu_Init(void)
{
    startPeriod = 100;
    endPeriod = 500;
    rampPeriod = 1000;
    rampSteps = 10;
    workloadLength = 30;
}

functionResult_e PowerLossEmu_TrimPll(unsigned int numArgs, int args[])
{
    Console_Print("Current setting: %6d (0x%02x)", OSCTUNEbits.TUN, OSCTUNEbits.TUN);
    // OSCTUNEbits.TUN is 6 bits
    // bit 5-0 TUN<5:0>: Frequency Tuning bits
    // 0b011111 = Maximum frequency
    // 0b011110
    // .
    // .
    // .
    // 0b000001
    // 0b000000 = Center frequency; oscillator module is running at the calibrated frequency
    // 0b111111
    // .
    // .
    // .
    // 0b100000 = Minimum frequency
    OSCTUNEbits.TUN = (0x3F)&Console_PromptForInt("Enter new trim: ");
    
    return SUCCESS;
}

functionResult_e PowerLossEmu_Setup(unsigned int numArgs, int args[])
{
    
    PowerLossEmu_CurrentSettings(0, 0);
    
    startPeriod = Console_PromptForInt("Enter starting period (us): ");
    endPeriod = Console_PromptForInt("Enter ending period (us): ");
    rampPeriod = Console_PromptForInt("Enter ramp period (ms): ");
    rampSteps = Console_PromptForInt("Enter number of ramp steps: ");
    workloadLength = Console_PromptForInt("Enter length of workload (s): ");

    PowerLossEmu_CurrentSettings(0, 0);
    
    return SUCCESS;
}

functionResult_e PowerLossEmu_CurrentSettings(unsigned int numArgs, int args[])
{
    Console_Print("Current power loss emulation settings:");
    Console_Print("Start period:    %6d us (0x%04x)", startPeriod, startPeriod);
    Console_Print("End period:      %6d us (0x%04x)", endPeriod, endPeriod);
    Console_Print("Ramp period:     %6d ms (0x%04x)", rampPeriod, rampPeriod);
    Console_Print("Ramp steps:      %6d", rampSteps, rampSteps);
    Console_Print("Ramp step size:  %6d", (endPeriod - startPeriod)/rampSteps);
    Console_Print("Workload length: %6d s", workloadLength);
    
    return SUCCESS;
}

functionResult_e PowerLossEmu_RunWorkload(unsigned int numArgs, int args[])
{
    uint16_t currentPeriod = startPeriod;
    uint16_t rampStepSize = (endPeriod - startPeriod)/rampSteps;
    uint32_t periodElapsedTime = 0;
    uint32_t totalElapsedTime = 0;
    uint32_t progressElapsedTime = 0;

    Console_Print("Workload will pulse RB0, running...");
    Util_SetNewCompareValue(startPeriod);
    for(;;)
    {
        Util_WaitMicroseconds(currentPeriod);

        // Increment our time trackers
        periodElapsedTime += currentPeriod;
        totalElapsedTime += currentPeriod;
        progressElapsedTime += currentPeriod;
        
        // Check if we have to move to a new period step
        if (((periodElapsedTime / MICROSECONDS_IN_MILLISECONDS) >= rampPeriod) && rampPeriod != 0)
        {
            // Increment our current period by the step size
            currentPeriod += rampStepSize;
            // Check if we're past max
            if (currentPeriod > endPeriod)
            {
                // Go back to the beginning
                currentPeriod = startPeriod;
            }
            // Update the comparator
            Util_SetNewCompareValue(currentPeriod);
            periodElapsedTime = 0;
        }
        
        // Print progress every seconds
        if ((progressElapsedTime / MICROSECONDS_IN_SECONDS) > 0)
        {
            progressElapsedTime = 0;
            Console_PrintNoEol(".");
        }
        
        // // Check if we're done our workload
        if ((totalElapsedTime / MICROSECONDS_IN_SECONDS) >= workloadLength)
        {
            Console_PrintNewLine();
            break;
        }
        
    }
    // Disable power-loss pulse
    Util_SetNewCompareValue(0);
    Console_Print("Workload exiting!");

    return SUCCESS;
}