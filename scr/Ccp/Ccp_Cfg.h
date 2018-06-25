/*============================================================================*/
/* Copyright (C) 2017-2018, Chengdu WeiTe Electronic Fuel CO.,LTD..
 *  
 *  All rights reserved. This software is WIT property. Duplication 
 *  or disclosure without WIT written authorization is prohibited.
 *  
 *  
 *  @file            :<Ccp_Cfg.h>
 *  @brief           :<Ccp for MPC5634> 
 *  @author          :<Mingdong.Yuan>
 *  @date            :<2017-7-7>
 *  @Current revision: $Revision: 1.0
 */
/*============================================================================*/
/*************************************************************************
Declaration of constants
*************************************************************************/
#define CCP_DTO_ID        0x101       /* CAN identifier ECU -> Master */
#define CCP_CRO_ID        0x100       /* CAN identifier Master -> ECU */
#define CCP_DAQ_ID        0x105       /* CAN identifier ECU -> Master */

#define DAQ_10MS_COUNTER   ((uint8)0x02)   /*2*5ms=10ms*/
#define DAQ_100MS_COUNTER  ((uint8)0x14)   /*20*5ms=100ms*/

#define CCP_MAX_ODT                       0x14    /* Number of ODTs in each DAQ lists */
#define CCP_MAX_DAQ                       0x03    /* Number of DAQ lists */
#define CCP_MIN_DAQ						  0x01

/*************************************************************************
Declaration of types
*************************************************************************/
typedef struct	
{
	const uint8 					 u8CcpMaxOdt;/*  Maximum number of ODT's in CcpOdt array;
												  *   [USER]	: When static DAQ configuration
												  *   [INTERNAL]: Dynamic DAQ configuration.
												  * 0 .. 255 
												  */
} Ccp_DaqListType;

typedef struct 
{
	const Ccp_DaqListType			 *CcpDaqList;
	const uint16					 cu16CcpMaxDaq; 			/* 0 .. 65535, CCP_MAX_DAQ */
	const uint16					 cu16CcpMinDaq; 			/* 0 .. 255  , CCP_MIN_DAQ */
} Ccp_ConfigType;

/*************************************************************************
EXPORTED Functions declaration
*************************************************************************/
extern const Ccp_ConfigType Ccp_Config;

/*************************************************************************
Declaration of functions
*************************************************************************/

