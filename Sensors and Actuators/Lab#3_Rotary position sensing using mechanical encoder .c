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

//#define DEMO_1    //Potentiometer
#define DEMO_2  //Rotary Encoder

#ifdef DEMO_1   //switch for Demo 1 code (R56 Pot)
    int32_t count=0;
    int32_t output_deg=0;
    int32_t input_dac=0;
    char deg[5];
    char temp[5];
    int flag=0;
#endif

#ifdef DEMO_2   //switch for Demo 2 code (EN11 Rotary Encoder)
    int8_t quadec_count_raw=0;
    uint32_t quadec_output_deg=0;
    char quadec_deg[5];
    char quadec_raw[5];    
    int8_t en11_count_raw=0;
    uint32_t en11_output_deg=0;
    char en11_deg[5];
    char temp[5];
    char temp1[5];
    int counter=-1;
    int counter_output=0;
#endif

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    #ifdef DEMO_1
        ADC_Start();    //start ADC
        DAC_Start();    //start DAC
        LCD_Start();    //start LCD
        LCD_DisplayOn();    //turn LCD display ON

        for(;;) //infinite loop
        {
            LCD_ClearDisplay(); //Clear LCD display
            CyDelay(10);    //10ms delay
            ADC_StartConvert(); //start conversion by ADC
            ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);   //return a nonzero value when conversion is complete
            count = ADC_Read32();   //return a signed 32-bit conversion result
        
            output_deg = ((count+14)*300)/66241;    //scale the ADC output to reach 300 degrees (max degrees turn)
            input_dac = ((count+14)*255)/66241;    //scale the output of ADC to give to an 8-bit DAC
        
            DAC_SetValue(input_dac);    //input the ADC output to an 8-bit DAC
        
            sprintf(deg,"%03d",output_deg); //convert the output in degrees to string to print on LCD
            sprintf(temp,"%04d",input_dac); //convert the input to DAC to string to print on LCD

            LCD_PrintString(deg);   //print degrees of rotation on LCD
            LCD_Position(0,4);
            LCD_PrintString("Degrees");
            LCD_Position(1,0);
            LCD_PrintString("DAC In: ");    //print input to DAC on LCD
            LCD_Position(1,8);
            LCD_PrintString(temp);
            CyDelay(500);   //500ms delay
        }
    #endif
    
    #ifdef DEMO_2   //switch for Demo 2
        QuadDec_Start();    //start Quadrature Decoder
        LCD_Start();    //start LCD
        LCD_DisplayOn();    //turn LCDdisplay ON
        for(;;) //infinite loop
        {   //HARDWARE USING QUADRATURE DECODER
            quadec_count_raw = QuadDec_GetCounter();    //current QuadDec counter value
        
            quadec_output_deg = quadec_count_raw * 18;  //convert QuadDec counter value to degrees
            if(quadec_output_deg == 360)    //when degree value is greater than 360
            {
                quadec_output_deg = 0;  //reset degree value to 0
            }
        
            sprintf(quadec_raw,"%03d",quadec_count_raw);//convert raw QuadDec count value to string to print on LCD
            sprintf(quadec_deg,"%03d",quadec_output_deg);//convert degree value to string to print on LCD
            
            //SOFTWARE
            if (Pin_0_2_Input_B_Read() == 0)    //if Input B is low
            {   
                CyDelayUs(120); //120us delay
                if (Pin_0_1_Input_A_Read() == 0)    //if Input A is low
                {  
                    counter = counter - 1;  //anticlockwise rotation, decrease counter value by 1       
                }
    
                if (Pin_0_1_Input_A_Read() == 1)    //if next, Input A is high
                {   
                    CyDelayUs(120); //120us delay
                    if (Pin_0_2_Input_B_Read() == 0)    //and Input B is low
                    {  
                        counter = counter + 1;  //clockwise rotation, increase counter value by 1  
                    } 
                }
                en11_output_deg = counter*18;   //convert counter value to degrees
                if(en11_output_deg == 360)  //rollover condition
                {
                    en11_output_deg = 0;
                }
                sprintf(en11_deg,"%03d",en11_output_deg);//convert to string to print on LCD
                LCD_ClearDisplay(); //clear LCD display
                if (counter < 0)    //if negative event has occured (anticlockwise rotation of EN11)
                {
                    counter_output = (-1) * counter;    //make the counter value sign negative
                    sprintf(temp,"-%03d",counter_output);//convert to string to print on LCD
                    
                    LCD_PrintString("Raw:");    //print raw couter value of QuadDec on Row 0
                    LCD_PrintString(quadec_raw);
                    LCD_PrintString(",Deg:");   //print degree of rotation value of QuadDec on Row 0
                    LCD_PrintString(quadec_deg);
                    
                    LCD_Position(1,0);  //print raw couter value of firmware calculations on Row 1
                    LCD_PrintString("Raw:");
                    LCD_Position(1,4);
                    LCD_PrintString(temp);
                    LCD_PrintString(",Deg:");   //print raw couter value of firmware calculations on Row 1
                    LCD_PrintString(en11_deg);
                }
                else    //if positive event has occured (clockwise rotation of EN11) 
                {
                    sprintf(temp,"%03d",counter);   //convert to string to print on LCD
                        
                    LCD_PrintString("Raw:");    //print raw couter value of QuadDec on Row 0
                    LCD_PrintString(quadec_raw);
                    LCD_PrintString(",Deg:");   //print degree of rotation value of QuadDec on Row 0
                    LCD_PrintString(quadec_deg);
                    
                    LCD_Position(1,0);  //print raw couter value of firmware calculations on Row 1
                    LCD_PrintString("Raw:");
                    LCD_Position(1,4);
                    LCD_PrintString(temp);
                    LCD_PrintString(",Deg:");   //print raw couter value of firmware calculations on Row 1
                    LCD_PrintString(en11_deg);
                }
                CyDelay(100);   //100ms delay
            }
        }        
    #endif    
}