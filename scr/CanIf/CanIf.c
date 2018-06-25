/*============================================================================*/
/* Copyright (C) 2015-2016, Chengdu WeiTe Electronic Fuel CO.,LTD.
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
#include "CanIf.h"
#include "Common_Types.h"
#include "Can.h"
#include "CanTp.h"

/**=======[E X T E R N A L   D A T A]==========================================*/
/* Global configure */
EXPORTED const CanIf_ConfigType *CanIf_ConfigPtr;
/**=======[IN T E R N A L   D A T A]==========================================*/
LOCAL const CanIf_TxPduConfigType *CanIf_FindTxPduEntry(PduIdType id)
{
	uint8 u8PduIndex;
	CanIf_TxPduConfigType *pstReturnTxPdu; 
	
   for(u8PduIndex = 0; u8PduIndex < CanIf_ConfigPtr->InitConfig->CanIfNumberOfCanTXPduIds; u8PduIndex++)
   {
		if(CanIf_ConfigPtr->InitConfig->CanIfTxPduConfigPtr[u8PduIndex].CanIf_TxPduId == id)
	   	{
			pstReturnTxPdu = &CanIf_ConfigPtr->InitConfig->CanIfTxPduConfigPtr[u8PduIndex];
	   	}
   }
	return(pstReturnTxPdu);
}

EXPORTED void CanIf_Init( const CanIf_ConfigType *ConfigPtr )
{
	 CanIf_ConfigPtr = ConfigPtr;
}

EXPORTED Std_ReturnType CanIf_Transmit(PduIdType CanTxPduId,const PduInfoType *PduInfoPtr)
{
	Can_PduType CanPdu;
	Std_ReturnType CanIf_transmitRet;
	const CanIf_TxPduConfigType *TxEntry;
	Can_ReturnType rVal;

	CanIf_transmitRet = E_NOT_OK;

	/*Get the controller from L-PDU handle*/
	TxEntry = CanIf_FindTxPduEntry(CanTxPduId);

    CanPdu.id          = TxEntry->CanIf_TxPduCanId;
    CanPdu.length      = (uint8)PduInfoPtr->sduLength;
    CanPdu.sdu         = PduInfoPtr->sduDataPtr;
    CanPdu.swPduHandle = CanTxPduId;
	 
    rVal = Can_Write(TxEntry->CanIfCanTxPduHthRef->CanIfHthIdSymRef, &CanPdu);
    if (rVal == CAN_NOT_OK)
	{
       CanIf_transmitRet =  E_NOT_OK;
    }
    else if (rVal == CAN_BUSY)  
    {
       CanIf_transmitRet = E_NOT_OK;
    }
	else
	{ 
	    CanIf_transmitRet = E_OK;
	}
    return CanIf_transmitRet;
}

EXPORTED void CanIf_RxIndication(const Can_HwType* Mailbox,const PduInfoType* PduInfoPtr)
{
	const CanIf_RxPduConfigType *entry;
	const CanIf_HrhConfigType   *pstHrhConfig;
	uint8 u8PduIdIndex;

	entry = CanIf_ConfigPtr->InitConfig->CanIfRxPduConfigPtr;

	/* Find the CAN id in the RxPduList */
	for (u8PduIdIndex = 0; u8PduIdIndex < CanIf_ConfigPtr->InitConfig->CanIfNumberOfCanRxPduIds; u8PduIdIndex++)
	{
		pstHrhConfig = &entry->CanIfCanRxPduHrhRef[u8PduIdIndex];
		if (pstHrhConfig->CanIfHrhIdSymRef == Mailbox->Hoh)
		{
			/* Call Back */
			switch(entry->CanIf_RxUserType)
			{
				case CANIF_USER_TYPE_CAN_TP:
					CanTp_RxIndication(CanIf_RxPduConfigData[u8PduIdIndex].CanIf_RxPduId, PduInfoPtr);
					break;
				
				case CANIF_USER_TYPE_CCPTP:
					//CCP_CanRxIndication(CanIf_RxPduConfigData[u8PduIdIndex].CanIf_RxPduId, PduInfoPtr);
					break;
				
				default :
					break;
			}
		}
		else 
		{
			entry++;
		}
	}
}

