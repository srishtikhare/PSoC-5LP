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

#define delay 2150 
#define loop 128

#define THERMISTOR_REFERENCE_CHANNEL (1u)
#define THERMISTOR_SIGNAL_CHANNEL (0u)
#define SAMPLES (256u)  //powers of 2 make math easier for the CPU
#define DEGREE (0xDFu)  //Degree sign (positive or negative)

void step1()
{
    P_0_5_in1_Write(0);
    P_0_3_in2_Write(1);
    P_3_1_in3_Write(1);
    P_3_3_in4_Write(0);
    CyDelayUs(delay);
}

void step2()
{
    P_0_5_in1_Write(0);
    P_0_3_in2_Write(1);
    P_3_1_in3_Write(0);
    P_3_3_in4_Write(1);
    CyDelayUs(delay);
}

void step3()
{
    P_0_5_in1_Write(1);
    P_0_3_in2_Write(0);
    P_3_1_in3_Write(0);
    P_3_3_in4_Write(1);
    CyDelayUs(delay);
}

void step4()
{
    P_0_5_in1_Write(1);
    P_0_3_in2_Write(0);
    P_3_1_in3_Write(1);
    P_3_3_in4_Write(0);
    CyDelayUs(delay);
}

void stopmotor()
{
    P_0_5_in1_Write(0);
    P_0_3_in2_Write(0);
    P_3_1_in3_Write(0);
    P_3_3_in4_Write(0);
    //CyDelayUs(delay);
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    int i;
    
    int32 temperature = 0;  //set inital value of temperature to zero
    int32 absolute; //variable for absolute value
    int16 reference_voltage, thermistor_voltage;    //variable for reference and thermistor voltage values    
    uint8 initialize = 0u;    //defines the first sampling time
    
	ADC_Start();    //start ADc
	AMux_Start();   //start AMux
	Opamp_Start();  //start Opamp
    
    LCD_Start();    //start LCD
	LCD_PrintString("T=     .   C");    //set text format on LCD
    LCD_Position(0u, 10u);  //set text placement on LCD
    LCD_PutChar(DEGREE);    //set text placement on LCD
    
    for(;;)
    {
        for(i=0;i<=loop;i++)
        {
            step1();
            step2();
            step3();
            step4();
        }
        
        stopmotor();
        
        for(i=0;i<=loop;i++)
        {
            step4();
            step3();
            step2();
            step1();
        }
        
        stopmotor();
        
        AMux_FastSelect(THERMISTOR_SIGNAL_CHANNEL); //read thermistor voltage
        ADC_StartConvert();
        ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
        thermistor_voltage = ADC_GetResult32();
    	
        AMux_FastSelect(THERMISTOR_REFERENCE_CHANNEL);  //read reference resistor voltage
        ADC_StartConvert();
        ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
        reference_voltage = ADC_GetResult32();
        
        //find temperature in Celsius degree
        temperature += Thermistor_GetTemperature(Thermistor_GetResistance(reference_voltage, thermistor_voltage));

        if(initialize)    //if initialized
        {
            temperature -= (temperature/SAMPLES);   //average over a number of samples
        }
        else
        {
            temperature *= SAMPLES; //replicate the first value as no data is available
        }
        initialize = 1; //set initialize to 1
        
        absolute = temperature; //obtain the value of absolute temperature
        
        LCD_Position(0u, 7u);   //displaying vaues on LCD
        LCD_PutChar('.');
        
        LCD_Position(0u, 10u);
        LCD_PutChar(DEGREE);
        
        absolute /= SAMPLES;
        LCD_PutChar('C');

        LCD_PrintString("        ");
        
        if (temperature < 0)
        {        
            LCD_Position(0u, 3u);
            LCD_PrintString("-");
            absolute *= -1;
        }
        else
        {
            LCD_Position(0u, 3u);
            LCD_PrintString("+");
        }
        LCD_Position(0u, 4u);
        
        if(absolute < 10000)    //if absolute value is less than 10000, put a space
            LCD_PutChar(' ');
            
        if(absolute <= 1000)    //if absolute value is less than 1000, put a space
            LCD_PutChar(' ');
        
        LCD_PrintDecUint16(absolute / 100u);

        LCD_Position(0u, 8u);
        if(absolute % 100u < 10)
        {
            LCD_PrintDecUint16(0);  //if absolute%100 value is less than 10, put a 0 after '.'
        }
        LCD_PrintDecUint16(absolute % 100u);
    }  
}

/* [] END OF FILE */
