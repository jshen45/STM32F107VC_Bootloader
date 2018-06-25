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
#include "CanIf_Cfg.h"

#ifndef CANIF_H_
#define CANIF_H_

/**=======[E X T E R N A L   D A T A]==========================================*/
/* Global configure */
extern const CanIf_ConfigType *CanIf_ConfigPtr;
/*=======[F U N C T I O N   I M P L E M E N T A T I O N S]====================*/
static const CanIf_TxPduConfigType *CanIf_FindTxPduEntry(PduIdType id);

extern void CanIf_Init(const CanIf_ConfigType *ConfigPtr);
extern Std_ReturnType CanIf_Transmit(PduIdType CanTxPduId,const PduInfoType *PduInfoPtr);
extern void CanIf_RxIndication(const Can_HwType* Mailbox,const PduInfoType* PduInfoPtr);
#endif /* CANIF_H_ */
