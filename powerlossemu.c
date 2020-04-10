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
#include <math.h>

#include "console.h"
#include "utils.h"
#include "powerlossemu.h"

static uint16_t startPeriod;
static uint16_t endPeriod;
static uint16_t rampPeriod;
static uint16_t rampSteps;
static uint16_t rampStepSize;
static uint16_t workloadLength;
static workloadType_e workloadType;

static arrayOfStrings_t workloadStrings =
{
    ANSI_COLOR_CYAN"Sawtooth-Up"ANSI_COLOR_RESET,
    ANSI_COLOR_CYAN"Sawtooth-Down"ANSI_COLOR_RESET,
    ANSI_COLOR_CYAN"Sine"ANSI_COLOR_RESET,
    ANSI_COLOR_CYAN"Square"ANSI_COLOR_RESET,
};

void PowerLossEmu_Init(void)
{
    startPeriod = 10000;
    endPeriod = 5000;
    rampPeriod = 1000;
    rampSteps = 20;
    // Calculate step size
    if (startPeriod > endPeriod)
    {
        rampStepSize = (startPeriod - endPeriod)/rampSteps;
    }
    else
    {
        rampStepSize = (endPeriod - startPeriod)/rampSteps;
    }
    workloadLength = 60;
    workloadType = WORKLOAD_SAWTOOTH_DOWN;
}

functionResult_e PowerLossEmu_PulsePowerLossSignal(unsigned int numArgs, int args[])
{
    Util_GeneratePulseRB0();
    return SUCCESS;
}

functionResult_e PowerLossEmu_Setup(unsigned int numArgs, int args[])
{
    uint16_t tempPeriod;

    PowerLossEmu_CurrentSettings(0, 0);
    
    startPeriod = Console_PromptForInt("Enter starting period (us): ");
    tempPeriod = startPeriod;
    endPeriod = Console_PromptForInt("Enter ending period (us): ");
    rampPeriod = Console_PromptForInt("Enter ramp period (ms): ");
    rampSteps = Console_PromptForInt("Enter number of ramp steps: ");
    workloadLength = Console_PromptForInt("Enter length of workload (s): ");

    Console_Print("Choose a workload setting");
    Console_Print("[0]-sawtooth-up, [1]-sawtooth-down [2]-sine [3]-square");
    workloadType = (workloadType_e)((0x3)&Console_PromptForInt("Enter workload type: "));

    // Calculate step size
    if (startPeriod > endPeriod)
    {
        rampStepSize = (startPeriod - endPeriod)/rampSteps;
    }
    else
    {
        rampStepSize = (endPeriod - startPeriod)/rampSteps;
    }

    // For some workloads, swap periods if they don't make sense
    if (((workloadType == WORKLOAD_SAWTOOTH_UP) || (workloadType == WORKLOAD_SINE)) && (startPeriod > endPeriod))
    {
        Console_Print("Swapping periods");
        startPeriod = endPeriod;
        endPeriod = tempPeriod;
    }
    else if ((workloadType == WORKLOAD_SAWTOOTH_DOWN) && (startPeriod < endPeriod))
    {
        Console_Print("Swapping periods");
        startPeriod = endPeriod;
        endPeriod = tempPeriod;
    }

    PowerLossEmu_CurrentSettings(0, 0);
    
    return SUCCESS;
}

functionResult_e PowerLossEmu_CurrentSettings(unsigned int numArgs, int args[])
{
    Console_Print("Current power loss emulation settings:");
    Console_Print("Start period:    %6d us", startPeriod);
    Console_Print("End period:      %6d us", endPeriod);
    Console_Print("Ramp period:     %6d ms", rampPeriod);
    Console_Print("Ramp steps:      %6d", rampSteps, rampSteps);
    if ((workloadType == WORKLOAD_SAWTOOTH_UP) || (workloadType == WORKLOAD_SAWTOOTH_DOWN))
    {
        Console_Print("Ramp step size:  %6d", rampStepSize);
    }
    Console_Print("Workload length: %6d s", workloadLength);
    Console_Print("Workload type:   %s", workloadStrings[(uint8_t)workloadType]);

    return SUCCESS;
}

functionResult_e PowerLossEmu_RunWorkload(unsigned int numArgs, int args[])
{
    uint16_t currentPeriod = startPeriod;
    uint32_t workloadStartTime;
    uint32_t periodStartTime;
    uint32_t progressStartTime;
    uint32_t currentTime;
    uint8_t currentStep = 1;
    float sineStep;

    PowerLossEmu_CurrentSettings(0, 0);
    Console_Print("Workload will pulse RB0, running...");

    // Sine workload starts off differently.
    if (workloadType == WORKLOAD_SINE)
    {
        sineStep = ((float)currentStep)/((float)rampSteps);
        currentPeriod = startPeriod + (endPeriod - startPeriod)*(1.0 + (0.5*sinf(2.0*M_PI*sineStep)));
    }

    // Initialize the comparator
    Util_SetNewCompareValue(currentPeriod);
    workloadStartTime = Util_GetMicrosecondUptime();
    periodStartTime = workloadStartTime;
    progressStartTime = workloadStartTime;
    for(;;)
    {
        currentTime = Util_GetMicrosecondUptime();
        // Check if we have to move to a new period step
        if ((((currentTime - periodStartTime) / MICROSECONDS_IN_MILLISECONDS) >= rampPeriod) && rampPeriod != 0)
        {
            switch (workloadType)
            {
                case WORKLOAD_SAWTOOTH_UP:
                    // Increment our current period by the step size
                    currentPeriod += rampStepSize;
                    // Check if we're past max
                    if (currentPeriod > endPeriod)
                    {
                        // Go back to the beginning
                        currentPeriod = startPeriod;
                    }
                    break;
                case WORKLOAD_SAWTOOTH_DOWN:
                    // Increment our current period by the step size
                    currentPeriod -= rampStepSize;
                    // Check if we're past max
                    if (currentPeriod < endPeriod)
                    {
                        // Go back to the beginning
                        currentPeriod = startPeriod;
                    }
                    break;
                case WORKLOAD_SINE:
                    sineStep = ((float)currentStep)/((float)rampSteps);
                    currentPeriod = startPeriod + (endPeriod - startPeriod)*(1.0 + (0.5*sinf(2.0*M_PI*sineStep)));
                    break;
                case WORKLOAD_SQUARE:
                    if (currentPeriod == startPeriod)
                    {
                        currentPeriod = endPeriod;
                    }
                    else
                    {
                        currentPeriod = startPeriod;
                    }
                    break;
                default:
                    break;
            }
            // Reset the period
            periodStartTime = Util_GetMicrosecondUptime();
            // Update the comparator
            Util_SetNewCompareValue(currentPeriod);
            // Increment the step
            currentStep++;
            // Check if we rolled over
            if (currentStep > rampSteps)
            {
                currentStep = 1;
            }
        }
        
        // Print progress every seconds
        if (((currentTime - progressStartTime) / MICROSECONDS_IN_SECONDS) > 0)
        {
            Console_PrintNoEol(".");
            progressStartTime = Util_GetMicrosecondUptime();
        }
        
        // Check if we're done our workload
        if (((currentTime - workloadStartTime) / MICROSECONDS_IN_SECONDS) >= workloadLength)
        {
            break;
        }
        
        // Check if we're quitting early
        if (Console_CheckForKey() != 0)
        {
            
            break;
        }
    }
    Console_PrintNewLine();
    // Disable power-loss pulse
    Util_SetNewCompareValue(0);
    Console_Print("Workload exiting!");

    return SUCCESS;
}