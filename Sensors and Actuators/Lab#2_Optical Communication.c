/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t adc_output=0;
unsigned long duration = 0;

unsigned long pulseIn(void) { //fuction to calculate the output at photo transistor HIGH duration

    unsigned long pulseWidth = 0; //to count the duration for which P0_7 (photo transistor output) stays high
    unsigned long loopCount = 0;  //loop counter
    unsigned long loopMax = 500000;  //loopcounter upper limit

    // While the P0_7 pin is not HIGH, make sure the timeout hasn't been reached
    while ((P0_7_Phototransistor_Output_Read()) != 1) {
        if (loopCount++ == loopMax) {   //run loop counter and 'wait'
            return 0;
        }
    }

    // When the P0_7 pin is HIGH, increment the counter while still keeping track of the timeout
    while ((P0_7_Phototransistor_Output_Read()) == 1) {
        if (loopCount++ == loopMax) {   //run loop counter and 'wait'
            return 0;
        }
        pulseWidth++;   //increment counter to keep tab of the pulse 'high' duration
    }

    // Return the pulse time in microseconds
    return pulseWidth * 2.36; // Calculated the pulseWidth++ loop to be about 2.36uS in length
}


int main(void)
{
    ADC_Start();    //start ADC module
    PWM_Start();    //start PWM module
    DAC_Start();    //start DAC module        
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    for(;;)
    {
        ADC_StartConvert(); //start conversion by ADC
        ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);   //return a nonzero value when conversion is complete
        adc_output = ADC_GetResult8();  //return a signed 8-bit conversion result
        
        PWM_WriteCompare(adc_output);   //write the value obtained from ADC into PWM for comparison
        
        duration = pulseIn();   //calculate the duration for which the output at photo transistor stayed high 
        
        duration = duration/32; //scale the output for 8-bit input to DAC
        
        DAC_SetValue(duration); //set amplitude values into DAC to reproduce the original wave
    }
    return 0;
}

/* [] END OF FILE */