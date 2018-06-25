#include "stm32f10x.h"
#include "sysinit.h"
#include "main.h"
#include "Can.h"
#include "CanIf.h"
#include "Dcm.h"
#include "CanTp.h"
#include "Appl.h"
#include "tools.h"

EXPORTED u32 count=0;
static uint32 u32SystemTimer;
static uint32 u32Timer;

int main(void)
{
	Appl_EcuInitialize();
	Appl_EcuStartup();
	
	while(1)
	{
		u32SystemTimer++;
		if(u32SystemTimer == 100)
		{
			/*500us timer flag*/
			APPL_b500usTimerFlag = B_TRUE;
		}
		else if(u32SystemTimer == 200)
		{
			u32SystemTimer = (uint32)0x00;
			/*1ms timer flag*/
			APPL_b1msTimerFlag = B_TRUE;
		}

		if(APPL_b1msTimerFlag != B_FALSE)
		{
			u32Timer++;
			APPL_b1msTimerFlag = B_FALSE;	

			Appl_BootStayTimer();
			CanTp_MainFunction();
			Dcm_MainFunction();
			/* task for flash driver and checksum process */
			FL_MainFunction();
			
			if(u32Timer == 10)
			{
				u32Timer = (uint32)0x00;
				/*10ms timer flag*/
				APPL_b10msTimerFlag = B_TRUE;
			}			
		}
		Can_MainFunction();
	} 	  
}
																 

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
