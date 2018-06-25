/*============================================================================*/
/* Copyright (C) 2015-2016, Chengdu WeiTe Electronic Fuel CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  
 *  @file       <CanIf_Cfg.c>
 *  @brief      <CAN driver for MPC5634>
 *              In this code, we don't use interrupt to handle the can receive 
 *              and transmit, but use polling the flag for the bootloader. 
 *  @author     <Mingdong.Yuan>
 *  @date       <2016-4-1>
 */
/*============================================================================*/
#include "Can.h"
#include "CanIf_Cfg.h"
#include "CanIf_Types.h"
#include "Ccp_Cfg.h"

LOCAL const Can_HardwareObjectType CAN_CTRL_A_HOHCfgData[]=
{
	{
		CAN_TYPE_BASIC,
		CAN_ID_TYPE_29,
		CAN_RXID,
		CAN_CTRL_A_CANHRH,
		CAN_OBJECT_TYPE_RECEIVE,
		0x00000100uL,
		FALSE
	},
	{
		CAN_TYPE_BASIC,
		CAN_ID_TYPE_29,
		CAN_TXID,
		CAN_CTRL_A_CANHTH,
		CAN_OBJECT_TYPE_TRANSMIT,
		0x00000200uL,
		FALSE
	},

	{
		CAN_TYPE_BASIC,
		CAN_ID_TYPE_11,
		CCP_CRO_ID,
		CAN_CTRL_A_CCPHRH,
		CAN_OBJECT_TYPE_RECEIVE,
		0x00000400uL,
		FALSE
	},
	{
		CAN_TYPE_BASIC,
		CAN_ID_TYPE_11,
		CCP_DTO_ID,
		CAN_CTRL_A_CCPHTH,
		CAN_OBJECT_TYPE_TRANSMIT,
		0x00000800uL,
		FALSE
	},

	{
		CAN_TYPE_BASIC,
		CAN_ID_TYPE_11,
		CCP_DAQ_ID,
		CAN_CTRL_A_DAQHTH,
		CAN_OBJECT_TYPE_TRANSMIT,
		0x00000800uL,
		TRUE
	},
};

LOCAL const Can_HardwareObjectType CAN_CTRL_C_HOHCfgData[]=
{
	{
		CAN_TYPE_BASIC,
		CAN_ID_TYPE_29,
		CAN_RXID,
		CAN_CTRL_C_CANHRH,
		CAN_OBJECT_TYPE_RECEIVE,
		FALSE
	},
	{
		CAN_TYPE_BASIC,
		CAN_ID_TYPE_29,
		CAN_TXID,
		CAN_CTRL_C_CANHTH,
		CAN_OBJECT_TYPE_TRANSMIT,
		TRUE
	},
};

const Can_ControllerConfigType  Can_ControllerCfgData[]=
{
	{
		CAN_CONTROLLER_A,
		500u, /* baudrate(kbs) */
		CAN_SJW_2tq, /* propseg(SJW) = 0 */
		CAN_BS1_2tq, /* seg1 = 4 */
		CAN_BS2_1tq, /* seg2 = 2 */
		CAN_CTRL_A_HOHCfgData
	},
	{
		CAN_CONTROLLER_C,
		250u, /* baudrate(kbs) */
		CAN_SJW_1tq, /* propseg(SJW) = 0*/
		CAN_BS1_3tq, /* seg1 = 2*/
		CAN_BS2_2tq, /* seg2 = 1*/
		CAN_CTRL_C_HOHCfgData
	},
};

const CanIf_HthConfigType CanIfHthConfigData_vCanIf_Channel_A[]=
{
	{
		CAN_TYPE_BASIC,
		CANIF_CHANNEL_A,         
		CAN_CTRL_A_CANHTH,       
		FALSE                      
	},

	{
		CAN_TYPE_BASIC,
		CANIF_CHANNEL_A,         
		CAN_CTRL_A_CCPHTH,       
		FALSE                      
	},

	{
		CAN_TYPE_BASIC,
		CANIF_CHANNEL_A,         
		CAN_CTRL_A_DAQHTH,       
		TRUE                      
	},
};

const CanIf_HrhConfigType CanIfHrhConfigData_vCanIf_Channel_A[]=
{
	{
		CAN_TYPE_BASIC,
		CANIF_CHANNEL_A,                            
		CAN_CTRL_A_CANHRH,       
		FALSE                      
	},

	{
		CAN_TYPE_BASIC,
		CANIF_CHANNEL_A,                            
		CAN_CTRL_A_CCPHRH,       
		TRUE                      
	},
};

const CanIf_HthConfigType CanIfHthConfigData_vCanIf_Channel_C[]=
{
	{
		CAN_TYPE_BASIC,
		CANIF_CHANNEL_C,         
		CAN_CTRL_C_CANHTH,       
		TRUE                      
	},
};

const CanIf_HrhConfigType CanIfHrhConfigData_vCanIf_Channel_C[]=
{
	{
		CAN_TYPE_BASIC,
		CANIF_CHANNEL_C,                            
		CAN_CTRL_C_CANHRH,       
		TRUE                      
	},
};

/* Container that gets slamed into CanIf_InitController()
   Inits ALL controllers Multiplicity 1*/
const CanIf_ControllerConfigType CanIfControllerConfig[] = 
{
	/*This is the ConfigurationIndex in CanIf_InitController()*/
	{
		CANIF_CHANNEL_A,                              /*CanIfControllerIdRef   */ 
		&Can_ControllerCfgData[INDEX_OF_CAN_CTRL_A]   /*CanIfInitControllerRef */ 
	},
	
	{
		CANIF_CHANNEL_C,                              /*CanIfControllerIdRef   */ 
		&Can_ControllerCfgData[INDEX_OF_CAN_CTRL_C]   /*CanIfInitControllerRef */ 
	},
};

/** Can receive Pdu configuration*/
EXPORTED const CanIf_RxPduConfigType CanIf_RxPduConfigData[]= 
{
	{
      0x11u,                    /* CanIf_RxPduId @type:uint16 range:0x0000~0xFFFF note:auto generate */ 
      CAN_RXID,                 /* CanIf_RxPduCanId @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE */
	   0x08u,                    /* CanIf_RxPduDlc*/
	   B_FALSE,                  /* CanIf_TxPduCanIdType */ 
	   CANIF_USER_TYPE_CAN_TP,   /*CanIfRxUserType*/
	   &CanIfHrhConfigData_vCanIf_Channel_A[0],   /*CanIfCanRxPduHrhRef */ 
      0x00000100uL,             /* CanIf_RxMailBoxMask,buffer 1-31 can be selected ,@type:uint32 range:0x00000000~0xFFFFFFFF note:NONE*/
      0xFFFFFFFFuL             /* CanIf_FilterMaskRef, @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE*/
	},

	{
      0x12u,                    /* CanIf_RxPduId @type:uint16 range:0x0000~0xFFFF note:auto generate */ 
      CCP_CRO_ID,                 /* CanIf_RxPduCanId @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE */
		0x08u,                    /* CanIf_RxPduDlc*/
	   B_TRUE,                  /* CanIf_TxPduCanIdType */ 
	   CANIF_USER_TYPE_CCPTP,   /*CanIfRxUserType*/
	   &CanIfHrhConfigData_vCanIf_Channel_A[0],   /*CanIfCanRxPduHrhRef */ 
      0x00000400uL,             /* CanIf_RxMailBoxMask,buffer 1-31 can be selected ,@type:uint32 range:0x00000000~0xFFFFFFFF note:NONE*/
      0xFFFFFFFFuL             /* CanIf_FilterMaskRef, @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE*/
	},

};

/* Can transmit Pdu configuration*/
EXPORTED const CanIf_TxPduConfigType CanIf_TxPduConfigData[]= 
{
	{
      0x01u,                    /*CanIf_TxPduId @type:uint16 range:0x0000~0xFFFF note:auto generate*/
      CAN_TXID,                 /*CanIf_TxPduCanId @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE*/
	   0x08u,                    /*CanIf_TxPduDlc*/
	   B_FALSE,                  /*CanIf_TxPduCanIdType*/ 
	   &CanIfHthConfigData_vCanIf_Channel_A[0],    /*CanIfCanTxPduHthRef*/ 
      0x00000200uL              /*Can_TxMailBoxMask,buffer 1-31 can be selected,  @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE*/
	},

	{
	   0x02u,                    /*CanIf_TxPduId @type:uint16 range:0x0000~0xFFFF note:auto generate*/
	   CCP_DTO_ID,                 /*CanIf_TxPduCanId @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE*/
	   0x08u,                    /*CanIf_TxPduDlc*/
	   B_TRUE,                  /*CanIf_TxPduCanIdType*/ 
	   &CanIfHthConfigData_vCanIf_Channel_A[0],    /*CanIfCanTxPduHthRef*/ 
	   0x00000800uL              /*Can_TxMailBoxMask,buffer 1-31 can be selected,  @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE*/
	},

	{
	   0x03u,                    /*CanIf_TxPduId @type:uint16 range:0x0000~0xFFFF note:auto generate*/
	   CCP_DAQ_ID,                 /*CanIf_TxPduCanId @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE*/
	   0x08u,                    /*CanIf_TxPduDlc*/
	   B_TRUE,                  /*CanIf_TxPduCanIdType*/ 
	   &CanIfHthConfigData_vCanIf_Channel_A[0],    /*CanIfCanTxPduHthRef*/ 
	   0x00000800uL              /*Can_TxMailBoxMask,buffer 1-31 can be selected,  @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE*/
	},
};


/*This container contains the init parameters of the CAN Multiplicity 1*/
const CanIf_InitConfigType CanIfInitConfig = 
{
	(uint32)0u,        /*CanIfConfigSet*/   // Not used 
	(uint32)2u,        /*CanIfNumberOfCanRxPduIds */ 
	(uint32)3u,        /*CanIfNumberOfCanTXPduIds */ 
	(uint32)0u,        /*CanIfNumberOfDynamicCanTXPduIds */
	CanIf_RxPduConfigData,
	CanIf_TxPduConfigData,
};

const CanIf_ConfigType CanIf_Config =
{
	CanIfControllerConfig,               
	&CanIfInitConfig,                     
};

extern const Can_ConfigSetType Can_ConfigSetData = {Can_ControllerCfgData};
extern const Can_ConfigType    Can_ConfigData    = {&Can_ConfigSetData};
extern const CanIf_ConfigType CanIf_Config;
