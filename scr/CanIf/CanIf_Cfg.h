/*============================================================================*/
/** Copyright (C) 2015-2016, Chengdu WeiTe Electronic Fuel CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  
 *  @file       <CanIf.c>
 *  @brief      <CAN driver for MPC5634>
 *              In this code, we don't use interrupt to handle the can receive 
 *              and transmit, but use polling the flag for the bootloader. 
 *  @author     <Mingdong.Yuan>
 *  @date       <2016-4-1>
 */
/*============================================================================*/
#include "CanIf_Types.h"

#ifndef CANIF_CFG_H_
#define CANIF_CFG_H_

/* Type of the upper layer interfacing this module */
#define 	CANIF_USER_TYPE_CAN_NM        0x00u
#define 	CANIF_USER_TYPE_CAN_TP        0x01u
#define 	CANIF_USER_TYPE_J1939TP       0x02u
#define 	CANIF_USER_TYPE_CCPTP         0x03u
#define 	CANIF_USER_TYPE_CAN_SPECIAL   0x04u
	
/* Info used by CanIF,index of configure in Can_ControllerCfgData[] */
#define INDEX_OF_CAN_CTRL_A     0u
#define INDEX_OF_CAN_CTRL_C     1u

#define CANIF_CHANNEL_A 0x00u
#define CANIF_CHANNEL_C 0x01u

/*************************************************************************
Declaration of types
*************************************************************************/
/*Top level config container*/
typedef struct
{
	const CanIf_ControllerConfigType 	*ControllerConfig;           /* Reference to the list of channel init configurations. */
	const CanIf_InitConfigType          *InitConfig;                 /* This container contains the init parameters of the CAN Interface. */
	//const uint8  	                  *Arc_ChannelToControllerMap; /* ArcCore: Contains the mapping from CanIf-specific Channels to Can Controllers */
                                                                    /*Can_ControllerIdType:CAN_CONTROLLER_A = 0 ;CAN_CONTROLLER_C = 1*/
}CanIf_ConfigType;

extern const Can_ConfigSetType Can_ConfigSetData;
extern const Can_ConfigType    Can_ConfigData;

extern const CanIf_ConfigType      CanIf_Config;
extern const CanIf_TxPduConfigType CanIf_TxPduConfigData[];
extern const CanIf_RxPduConfigType CanIf_RxPduConfigData[];
#endif /* CANIF_CFG_H_ */
