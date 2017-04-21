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

#define resistorValue 68000.0F  //define variables

uint32 elapsedTime;
float microFarads;
float nanoFarads;
float picoFarads;

CY_ISR(PWM) //ISR1
{
    Counter_Start();    //start counter
}

CY_ISR(Comparator)  //ISR2
{
    Counter_Stop(); //stop counter

    elapsedTime = Counter_ReadCounter();    //read counter value
    Counter_WriteCounter(0);    //reset counter value to zero
    
    microFarads = ((float)elapsedTime / resistorValue); //calculate capacitance from the time
}

int main(void)
{
   
    PWM_Start();    //start modules
    VDAC8_Start();
    Comp_Start();
    LCD_Start();
    LCD_DisplayOn();
    VDAC8_1_Start();
    PGA_1_Start();
    Comp_1_Start();
    
    isr_1_StartEx(PWM);
    isr_2_StartEx(Comparator);
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    for(;;)
    {
        LCD_ClearDisplay();
        LCD_PrintU32Number(elapsedTime);
        LCD_Position(0,6);
        LCD_PrintString("us");
        
        if (microFarads > 1)
        {
            LCD_Position(1,0);
            LCD_PrintNumber((long)microFarads);       // print the value to serial port
            LCD_Position(1,6);
            LCD_PrintString("microFarads");         // print units and carriage return
        }
        else
        {
    // if value is smaller than one microFarad, convert to nanoFarads (10^-9 Farad)

            nanoFarads = microFarads * 1000.0;      // multiply by 1000 to convert to nanoFarads (10^-9 Farads)
            if (nanoFarads > 1)
            {
                LCD_Position(1,0);
                LCD_PrintNumber((long)nanoFarads);         // print the value to serial port
                LCD_Position(1,6);
                LCD_PrintString("nanoFarads");          // print units and carriage return
            }
            else
            {
                picoFarads = nanoFarads * 1000.0;
                LCD_Position(1,0);
                LCD_PrintNumber((long)picoFarads);         // print the value to serial port
                LCD_Position(1,6);
                LCD_PrintString("picoFarads");          // print units and carriage return
            }
        }        
        CyDelay(50);       
    }
}

/* [] END OF FILE */
