/*============================================================================*/
/* Copyright (C) 2016-2017, Chengdu WeiTe Electronic Fuel CO.,LTD..
 *  
 *  All rights reserved. This software is WIT property. Duplication 
 *  or disclosure without WIT written authorization is prohibited.
 *  
 *  @file       <Appl.c>
 *  @brief      <App Loader>
 *  
 *  <provide the environment of the Bootloader peoject>
 *  
 *  @author     <Mingdong.Yuan>
 *  @date       <2018-1-24>
 */
/*============================================================================*/
/*=======[I N C L U D E S]====================================================*/
#include "Appl.h"
#include "main.h"
#include "Dcm_Dsl.h"
#include "CanIf.h"
#include "Dcm.h"
#include "FL.h"

/*************************************************************************
EXPORTED variables declaration
*************************************************************************/
EXPORTED boolean APPL_b500usTimerFlag;
EXPORTED boolean APPL_b1msTimerFlag;
EXPORTED boolean APPL_b5msTimerFlag;
EXPORTED boolean APPL_b10msTimerFlag;
EXPORTED uint32 APPL_u32TimerCounter;
EXPORTED uint32 ecuBootStayTimer;

/* Private typedef -----------------------------------------------------------*/
typedef  void (*pFunction)(void);

/*=======[F U N C T I O N   I M P L E M E N T A T I O N S]====================*/
/**
  * @brief  控制程序跳转到指定位置开始执行 。
  * @param  Addr 程序执行地址。
  * @retval 程序跳转状态。
  */
LOCAL void Appl_GotoAppSW(uint32 Addr)
{
	pFunction Jump_To_Application;
	__IO uint32 JumpAddress; 

	CAN_DeInit(CAN1);
	/* Jump to user application */
	JumpAddress = *(__IO uint32*)(Addr + 0x04uL);
	Jump_To_Application = (pFunction) JumpAddress;
	//__set_PRIMASK(1);//关闭所有中断
	CAN_ITConfig(CAN1,CAN_IT_FMP0, DISABLE);
	/* Initialize user application's Stack Pointer */
	__set_MSP(*(__IO uint32*) Addr);
	Jump_To_Application();
}

/******************************************************************************/
/**
 * @brief               <get nvm infomation>
 * 
 * <get infomation from EEPROM about bootloader infomation(e.g.fingerprint)> .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
LOCAL void Appl_InitNvmInfo(void)
{
        uint8 blockIndex = 0;
        
        /* if there is no infomation in NVM,then initialize the infomation */
        for (blockIndex = 0; blockIndex < FL_NUM_LOGICAL_BLOCKS; blockIndex ++)
        {
            FL_NvmInfo.blockInfo[blockIndex].blkChecksum = 0x00uL;
            FL_NvmInfo.blockInfo[blockIndex].blkProgAttempt = 0x00u;
            FL_NvmInfo.blockInfo[blockIndex].blkValid = FALSE;
        }
        
        FL_NvmInfo.infoChecksum = 0x00u;
}

/******************************************************************************/
/**
 * @brief               <ECU Startup>
 * 
 * <This routine shall be called by the bootcode contained in the flashloader 
 *  to initialize common hardware.> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Non Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <void>    
 */
/******************************************************************************/
EXPORTED void Appl_EcuStartup (void)
{   
	Appl_InitNvmInfo();
	
	/* check if bootloader program is requested in app */ 
    if (FL_EXT_PROG_REQUEST_RECEIVED == (*(uint8 *)FL_BOOT_MODE))
    {	
        /* clear bootloader request flash and app update flag */
        *(uint8 *)FL_BOOT_MODE = 0x00u;
		  
        /* DCM go programming mode and response programming session service */
        Dsl_ProgramInit();
    }
    else
    {
		if(FL_APPL_UPDATED == *(uint8 *)FL_APPL_UPDATE)
		{
			ecuBootStayTimer = FL_MODE_STAY_TIME / 5;
		}
		/* check if application software is consistency */
		if(FL_OK == Appl_CheckConsistency())
		{
			ecuBootStayTimer = FL_MODE_STAY_TIME / 5;
		}
		
    }
}

/******************************************************************************/
/**
 * @brief               <ECU initialize>
 * 
 * <This routine shall be initialize the ECU each module
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Non Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <void>    
 */
/******************************************************************************/
EXPORTED void Appl_EcuInitialize(void)
{
	SystemInit(); 
	CAN_Configuration(&Can_ConfigData);
	CanIf_Init(&CanIf_Config);
	CanTp_Init(&CanTpConfig); 
	Dcm_Init(&Dcm_Config);

	ecuBootStayTimer = KUC_NULL;
}

/******************************************************************************/
/**
 * @brief               <check application software consistency>
 * 
 * <This routine shall be called by the flashloader to check whether the 
 *  individual parts (logical blocks) that make up the application software of 
 *  an ECU are consistent with each other.> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Non Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>    
 */
/******************************************************************************/
EXPORTED Std_ReturnType Appl_CheckConsistency (void)
{
    FL_ResultType ret = FL_OK;
    uint8 blockIndex = 0;
    
    for (blockIndex = 0; (blockIndex < FL_NUM_LOGICAL_BLOCKS) && (ret == FL_OK);blockIndex ++)
    {
        /* check if all block is valid */
        if (FALSE == FL_NvmInfo.blockInfo[blockIndex].blkValid)
        {
            ret = FL_FAILED;
        }
    }
    
    return ret;
}

/******************************************************************************/
/**
 * @brief               <time count for ECU stay in boot>
 * 
 * <time count for ECU stay in boot> .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
EXPORTED void Appl_BootStayTimer(void)
{
	Dcm_SecLevelType  u8SecLevel;
	Dcm_SesCtrlType   u8SesCtrlType;
	Std_ReturnType    u8ResultCode;
	
	 /* 
    ** if diagnostic leave default session mode or security unlocked, stop 
    ** boot stay timer count.
    */
	 u8ResultCode = Dcm_GetSecurityLevel(&u8SecLevel);
	 u8ResultCode = Dcm_GetSesCtrlType(&u8SesCtrlType);
	 if(u8ResultCode == E_OK)
	 {
		 if ((ecuBootStayTimer > 0) &&
	        (u8SecLevel == DCM_SECURITY_LOCKED) && 
	        (u8SesCtrlType == DCM_SESSION_DEFAULT))
	    {
	        ecuBootStayTimer --;
	        if (0x00u == ecuBootStayTimer)
	        {
	            Appl_GotoAppSW(FL_APP_RCHW_ADDR);
	        }
	    }
	    else
	    {
	        ecuBootStayTimer = 0x00u;
	    }
	}
}

/*******************************************************************************
* Function Name  : Appl_EcuReset
* Description    : This routine shall be called by the flashloader to reset the ECU upon.
                   request from the external programming device.> .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
EXPORTED void Appl_EcuReset(void)
{
	__set_FAULTMASK(1);
	NVIC_SystemReset();
}

EXPORTED void Appl_LedTest(void)
{
	APPL_u32TimerCounter++;
	if(APPL_u32TimerCounter>15)APPL_u32TimerCounter=1;

	if(APPL_u32TimerCounter < 5) 
	{
		GPIO_ResetBits(GPIOD,GPIO_Pin_15);   //'0'红色亮
	}
	else 	 
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_15);
	}

	if(APPL_u32TimerCounter < 10) 
	{
		GPIO_ResetBits(GPIOD,GPIO_Pin_13);   //'0'绿色亮
	}
	else 	 
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_13);
	}

	if(APPL_u32TimerCounter < 150) 
	{
		GPIO_ResetBits(GPIOD,GPIO_Pin_14);   //'0'蓝色亮
	}
	else 	 
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_14);
	}
}

