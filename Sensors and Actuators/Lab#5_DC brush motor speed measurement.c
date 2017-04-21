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

volatile uint32_t count = 0;    //variables defined
volatile uint32_t counter_main = 0;
int output = 0;

CY_ISR(DISCONTINUITY_COUNT_RESET)   //callback function for ISR to reset the count value
{
    counter_main = count;
    count=0;    
}

CY_ISR(Pin_High_Check)  //callback function for ISR to check if pin P_3_1_Comp_out is high
{
    count++;  //increment count value when pin is HIGH  
}

int main(void)
{
    Timer_Start();  //start Timer module
    PGA_Start();    //start PGA module
    VDAC8_Start();  //start VADC module
    Comp_Start();   //start Comparator module
    LCD_Start();    //start LCD module
    LCD_DisplayOn();    //turn LCD display ON
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    isr_StartEx(DISCONTINUITY_COUNT_RESET); //define ISRs
    isr_1_StartEx(Pin_High_Check);
    
    VDAC8_SetValue(25); //Set value to VDAC

    for(;;)
    {
        CyDelay(50);
        LCD_ClearDisplay();
        
        output = (counter_main*60)/6;   //convertcount value obtained to RPM
        
        LCD_PrintU32Number(output); //print RPM value on LCD
    }
}