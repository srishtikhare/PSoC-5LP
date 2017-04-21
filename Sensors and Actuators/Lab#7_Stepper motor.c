#include <device.h>
#include <lcd.h>
#include "stdio.h"

#define delay 2000

uint32 curPos, oldPos,curPos_deg,loop;
uint32 i;

void step1()
{
    P_0_5_in1_Write(0); //input 1 to motor
    P_0_3_in2_Write(1); //input 2 to motor
    P_3_1_in3_Write(1); //input 3 to motor
    P_3_3_in4_Write(0); //input 4 to motor
 }

void step2() 
{
    P_0_5_in1_Write(0); //input 1 to motor
    P_0_3_in2_Write(1); //input 2 to motor
    P_3_1_in3_Write(0); //input 3 to motor
    P_3_3_in4_Write(1); //input 4 to motor
}

void step3()
{
    P_0_5_in1_Write(1); //input 1 to motor
    P_0_3_in2_Write(0); //input 2 to motor
    P_3_1_in3_Write(0); //input 3 to motor
    P_3_3_in4_Write(1); //input 4 to motor  
}

void step4()
{
    P_0_5_in1_Write(1); //input 1 to motor
    P_0_3_in2_Write(0); //input 2 to motor
    P_3_1_in3_Write(1); //input 3 to motor
    P_3_3_in4_Write(0); //input 4 to motor   
}

void stopmotor()
{
    P_0_5_in1_Write(0); //input 1 to motor
    P_0_3_in2_Write(0); //input 2 to motor
    P_3_1_in3_Write(0); //input 3 to motor
    P_3_3_in4_Write(0); //input 4 to motor
}

int main()
{    
	/* Variable to store loop number */
	uint8 sensorScanNo = 0;
	
	/* Enable global interrupts */
    CyGlobalIntEnable;
	
	/* Initialize the LCD component */
    LCD_Start();

	/* Manually load the custom fonts for the bargraph */
	LCD_LoadCustomFonts(LCD_customFonts);
 	
	/* Set the LCD pointer to (0,0) */
	LCD_Position(ROW_0, COLUMN_0);
	
	/* Display CapSense initialization status */
    LCD_PrintString("Initializing");   
	
	/* Initialize the CapSense CSD component */
    CapSense_CSD_Start();
	
	/* Initialize baselines of CapSense sensors */ 
    CapSense_CSD_InitializeAllBaselines();
	
	/* Set the LCD pointer to (0,0) */
	LCD_Position(ROW_0, COLUMN_0);
	
	/* Display the character "CSD01" on the LCD */
    LCD_PrintString("CapSense CSD");	
	
	/* Perform >= 5 dummy sensor scans to initialize the 
	*  threshold parameters when using SmartSense
	*/
	for(sensorScanNo = 0; sensorScanNo < DUMMY_SCANS; sensorScanNo++)
	{
		/* Update baselines of all CapSense sensors */
        CapSense_CSD_UpdateEnabledBaselines();
		
   		/* Scan all enabled sensors */
    	CapSense_CSD_ScanEnabledWidgets();
    
        /* Wait until scanning is complete */
		while(CapSense_CSD_IsBusy() != 0);
	}
	
    while(1u)
    {	
		
        if(CapSense_CSD_IsBusy() == 0)
		{
			/* Update baselines of all CapSense sensors */
	        CapSense_CSD_UpdateEnabledBaselines();
			
	   		/* Scan all enabled sensors */
	    	CapSense_CSD_ScanEnabledWidgets();
		}
		
        /* While scanning is in progress, display the sensor status of previous scan*/
        
       
        CapSense_DisplayState();	
    }
}

void CapSense_DisplayState(void)
{
    /* Display BUTTON0 state */
	if (CapSense_CSD_CheckIsWidgetActive(CapSense_CSD_BUTTON0__BTN)) 
	{
		LED1_Write(1u);
	}
	else
	{
		LED1_Write(0u);
	}
    
	/* Display BUTTON1 state */
	if (CapSense_CSD_CheckIsWidgetActive(CapSense_CSD_BUTTON1__BTN))
	{
		LED2_Write(1u);
	}
	else
	{
		LED2_Write(0u);
	}

    /* Find finger position on the slider */
    curPos = CapSense_CSD_GetCentroidPos(CapSense_CSD_LINEARSLIDER0__LS);    
    
    /* Reset position if finger is not present on the slider */
    if(curPos == 0xFFFFu)
    {
        curPos = 0u;
    }
                
    /* If finger movement is detected, update the bargraph */
    if (curPos != oldPos)
    {
        oldPos = curPos;
    }
		
    /* Display finger position using bargraph */            
    LCD_DrawHorizontalBG(BARGRAPH_ROW, BARGRAPH_COLUMN, BARGRAPH_MAX_CHAR, curPos >> 2);
        
    /* Display finger position value */
    LCD_Position(ROW_1, 7);
    curPos_deg=(uint32)(1.85*curPos);
    LCD_PrintString("deg");
    LCD_Position(1,11);
    LCD_PrintU32Number(curPos_deg);
    LCD_Position(1,11);
    LCD_PrintString("      ");
    LCD_Position(1,11);
    LCD_PrintU32Number(curPos_deg);
    LCD_Position(0,0);
    LCD_PrintString("            ");
    LCD_Position(0,0);
    LCD_PrintString("cur  ");
    LCD_PrintU32Number(curPos);
    CyDelay(50);
    loop=(uint32)(curPos_deg/2.5);           
     
    for( i=0; i<loop; i++ )   //at lopp value=50, motor rotates 0-180
    {
        step1(); 
        CyDelayUs(delay);
        step2();
        CyDelayUs(delay);
        step3();
        CyDelayUs(delay);
        step4();
        CyDelayUs(delay);
    }        
}
/* [] END OF FILE */
