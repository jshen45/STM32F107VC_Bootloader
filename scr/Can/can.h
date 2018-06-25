/**
  ******************************************************************************
  * @file    can.h
  * $Author: wdluo $
  * $Revision: 17 $
  * $Date:: 2012-07-06 11:16:48 +0800 #$
  * @brief   CAN总线收发函数.
  ******************************************************************************
  * @attention
  *
  *<h3><center>&copy; Copyright 2009-2012, ViewTool</center>
  *<center><a href="http:\\www.viewtool.com">http://www.viewtool.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN_H
#define __CAN_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_conf.h"
#include "Common_Types.h"
#include "Can_GeneralTypes.h"
#include "Can_Cfg.h"

/*=======[M A C R O S]========================================================*/

#define CAN_CONTROLLER_A   0x00u
#define CAN_CONTROLLER_B   0x01u
#define CAN_CONTROLLER_C   0x02u

/** @defgroup CAN_identifier_type*/ 
//#define CAN_Id_Standard             ((uint32_t)0x00000000)  /*!< Standard Id */
//#define CAN_Id_Extended             ((uint32_t)0x00000004)  /*!< Extended Id */



/** @defgroup CAN_remote_transmission_request */
//#define CAN_RTR_DATA                ((uint32_t)0x00000000)  /*!< Data frame */
//#define CAN_RTR_REMOTE              ((uint32_t)0x00000002)  /*!< Remote frame */

#define CAN_RECEIVE_FLAG			((uint8_t)0x01) 
#define CAN_RECEIVE_CLEARED			((uint8_t)0x00)

#define CAN_GET_CONTROLLER_CONFIG(controller)	&Can_Global.Config->CanConfigSet->CanController[controller]

/*=======[T Y P E   D E F I N I T I O N S]====================================*/
typedef uint16	Can_HwHandleType;
typedef uint32	Can_IdType;

typedef enum
{
	CAN_OK,
	CAN_NOT_OK,
	CAN_BUSY
}Can_ReturnType;




typedef struct
{
	Can_IdType         Can_Id;       /*CAN Message ID*/
	Can_HwHandleType   Hoh;
	uint8              ControllerId; /*CAN controller ID*/
}Can_HwType;

typedef struct 
{
	PduIdType    swPduHandle;
	uint8        length;
	Can_IdType   id;
	uint8*       sdu;
}Can_PduType;

typedef struct Can_HardwareObjectStruct 
{
	const uint8  CanHandleType;      /* Specifies the type (Full-CAN or Basic-CAN) of a hardware object.*/

	const uint8  CanIdType;          /* Specifies whether the IdValue is of type - standard identifier - extended identifier - 
	                                    mixed mode ImplementationType: Can_IdType*/

	const uint32 CanIdValue;           /*	Specifies (together with the filter mask) the identifiers range that passes the hardware filter.*/

	const uint16 CanObjectId;          /*	Holds the handle ID of HRH or HTH. The value of this parameter is unique
													in a given CAN Driver, and it should start with 0 and continue without any
													gaps. The HRH and HTH Ids are defined under two different name-spaces.
													Example: HRH0-0, HRH1-1, HTH0-2, HTH1-3.*/
	
	const uint8  CanObjectType;      /* Specifies if the HardwareObject is used as Transmit or as Receive object*/

	const uint32 Can_MbMask;			/* This mask tells the driver that which Message Buffer should be occupied by this Hoh.*/							

	boolean Can_EOL;                    /* End Of List. Set to TRUE is this is the last object in the list.*/
} Can_HardwareObjectType;

typedef struct
{
	const uint8           CanControllerId;    /*This parameter provides the controller ID which is unique in a given CAN Driver. 
	                                            The value for this parameter starts with 0 and continue without any gaps. */
	
	const uint16          CanControllerBaudRate;   /*Specifies the baudrate of the controller in kbps. */
	
	const uint16          CanControllerPropSeg;    /*Specifies propagation delay in time quantas(1..8).*/
	
	const uint16          CanControllerSeg1;       /*Specifies phase segment 1 in time quantas(1..16). */
	
	const uint16          CanControllerSeg2;       /* Specifies phase segment 2 in time quantas(1..8). */
	
   const Can_HardwareObjectType  *Can_Hoh;       /* List of Hoh id's that belong to this controller  */
}Can_ControllerConfigType;

typedef struct 
{
	const Can_ControllerConfigType *CanController;
} Can_ConfigSetType;

/*
	This is  the type of the external data structure containing the overall initialization
	data for the CAN driver and SFR settings affecting all controllers. Furthermore it
	contains pointers to controller configuration structures. The contents of the
	initialization data structure are CAN hardware specific.
*/
typedef struct
{
	/*This is the multiple configuration set container for CAN Driver Multiplicity 1*/
	const Can_ConfigSetType	 *CanConfigSet;
}Can_ConfigType;

typedef struct
{
	const Can_ConfigType* Config;
	
	uint8  configured;    /* One bit for each channel that is configured.Used to determine if validity of a channel
							       1 - configured
							       0 - NOT configured */
}Can_GlobalType;

/*=======[E X T E R N A L   D A T A]==========================================*/
extern Can_GlobalType Can_Global;
extern volatile uint8_t CanRxMsgFlag;//接收到CAN数据后的标志

/**=======[I N T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
uint8_t CAN_WriteData(CanTxMsg *TxMessage);
void CAN_ConfigFilter(uint8_t can_addr);
extern CanRxMsg RxMessage;

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
extern void CAN_Configuration(const Can_ConfigType* Can_Config);
extern void CAN_ReceiveData(uint8 u8ControllerID);
extern Can_ReturnType Can_Write( Can_HwHandleType Hth, const Can_PduType* PduInfo ); 
extern void Can_MainFunction(void);
extern void CAN_RxMainFunction(void);


#endif /*__CAN_H */

/***********************************文件结束***********************************/
