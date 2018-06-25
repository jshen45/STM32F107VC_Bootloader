/*============================================================================*/
/** Copyright (C) 2016-2017, Chengdu WeiTe Electronic Fuel CO.,LTD..
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
#include "Common_Types.h"
#include "Can.h"

#ifndef CANIF_TYPES_H_
#define CANIF_TYPES_H_

/*CanIfInitHthConfig container*/
/*Definition of Hardware Transmit Handle */
typedef const struct 
{
  const uint8  CanIfHthType;                      /* Defines the HTH type i.e, whether its a BasicCan or FullCan. */
  	
  const uint8  CanIfCanControllerIdRef;           /* Reference to controller Id to which the HTH belongs to. A controller can contain one or more HTHs */

  const uint8  CanIfHthIdSymRef ;                 /* The parameter refers to a particular HTH object in the CAN Driver Module
					                                       *  configuration. The HTH id is unique in a given CAN Driver. The HTH Ids
					                                       *  are defined in the CAN Driver Module and hence it is derived from CAN
					                                       *  Driver Configuration. */
  
  boolean CanIf_Arc_EOL;                             /* End Of List. Set to TRUE if this is the last object in the list. */
} CanIf_HthConfigType;

/*CanIfInitHrhConfig container*/
/* Definition of Hardware Receive Handle */
typedef struct 
{
	const uint8 CanIfHrhType;							/* Defines the HRH type i.e, whether its a BasicCan or FullCan. If BasicCan is
									                        configured, software filtering is enabled. */
    
	const uint8 CanIfCanControllerHrhIdRef;      /* Reference to controller Id to which the HRH belongs to. A controller can
															      contain one or more HRHs. */
	
	const uint8 CanIfHrhIdSymRef ;           /* The parameter refers to a particular HRH object in the CAN Driver Module
																 *  configuration. The HRH id is unique in a given CAN Driver. The HRH Ids
																 *  are defined in the CAN Driver Module and hence it is derived from CAN
																 *  Driver Configuration. */
	
   boolean CanIf_Arc_EOL;                        /* End Of List. Set to TRUE if this is the last object in the list. */
  
} CanIf_HrhConfigType;

/*Definition of Rx PDU (Protocol Data Unit)*/
typedef struct 
{
   const PduIdType  CanIf_RxPduId;         /*ECU wide unique, symbolic handle for receive CAN L-PDU. The CanRxPduId is configurable at pre-compile and post-built*/ 
                                           /*time. It shall fulfill ANSI/AUTOSAR definitions for constant defines. Range: 0..max. number of defined CanRxPduIds       */
   const uint32 	  CanIf_RxPduCanId;      /*CAN Identifier of Receive CAN L-PDUs used by the CAN Interface. Exa:Software Filtering. Range: 11 Bit For Standard CAN*/ 
                                           /*Identifier ... 29 Bit For Extended CAN identifier                                                                        */
   const uint8		  CanIf_RxPduDlc;        /*Data Length code of received CAN L-PDUs used by the CAN Interface.Exa: DLC check. The data area size of a CAN L-PDU can */
                                            /*have a range from 0 to 8 bytes.                                                                                          */

   const boolean    CanIf_StandardFrame;     /* standardFrame or not */
	                                          /*CAN Identifier of receive CAN L-PDUs used by the CAN Driver for CAN L-PDU transmission.*/
                                             /*EXTENDED_CAN  The CANID is of type Extended (29 bits)                                  */
                                             /*STANDARD_CAN  The CANID is of type Standard (11 bits)                                  */
	const uint8      CanIf_RxUserType;        /* This parameter defines the type of the receive upper layer*/
   const CanIf_HrhConfigType *CanIfCanRxPduHrhRef; /* The HRH to which Rx L-PDU belongs to, is referred through this parameter. */						 
   const uint32     CanIf_RxMailBoxMask;    /* Used for Rx Mail Box select */
   const uint32     CanIf_FilterMaskRef;    /* Can Rx Individual Mask registers */
}CanIf_RxPduConfigType;

typedef const struct 
{
	const PduIdType        CanIf_TxPduId;        /*ECU wide unique, symbolic handle for transmit CAN L-PDU. The CanIfCanTxPduId is configurable at */
	                                             /* pre-compile and post-built time.Range: 0..max. number of CantTxPduIds PduIdType CanTxPduId;   */

	const uint32           CanIf_TxPduCanId;     /*CAN Identifier of transmit CAN L-PDUs used by the CAN Driver for CAN L-PDU transmission. */
                                                 /*Range: 11 Bit For Standard CAN Identifier ... 29 Bit For Extended CAN identifier              */

	const uint8 		     CanIf_TxPduDlc;       /*Data length code (in bytes) of transmit CAN L-PDUs used by the CAN Driver for CAN L-PDU transmission. */
	                                              /*The data area size of a CAN L-Pdu can have a range from 0 to 8 bytes.                         */
   const boolean          CanIf_StandardFrame;   /* standardFrame or not */
	                                              /*CAN Identifier of transmit CAN L-PDUs used by the CAN Driver for CAN L-PDU transmission.
                                                 //EXTENDED_CAN  The CANID is of type Extended (29 bits).
                                                 //STANDARD_CAN  The CANID is of type Standard (11 bits).   */
                                                 
	const CanIf_HthConfigType *CanIfCanTxPduHthRef;  /*Handle, that defines the hardware object or the pool of hardware objects*/
	                                                   /*configured for transmission. The parameter refers HTH Id, to which the L-PDU belongs to.  */
	
	const uint32           CanIf_TxMailBoxMask;  /* Used for Tx Mail Box select */
} CanIf_TxPduConfigType;

typedef struct 
{
	const uint8                      CanIfControllerIdRef;                /* CanIf-specific id of the controller */
	const Can_ControllerConfigType  *CanIfInitControllerRef;              /*Reference to */
}CanIf_ControllerConfigType;

typedef struct 
{
	const uint32 CanIfConfigSet;                   /*Not used.*/
	const uint32 CanIfNumberOfCanRxPduIds;         /*Size of Rx PDU list.*/
	const uint32 CanIfNumberOfCanTXPduIds;         /*Size of Tx PDU list.*/
	const uint32 CanIfNumberOfDynamicCanTXPduIds;  /*Not used*/
	const CanIf_RxPduConfigType *CanIfRxPduConfigPtr;    /*Rx PDU's list*/
	const CanIf_TxPduConfigType *CanIfTxPduConfigPtr;    /*Tx PDU's list*/
}CanIf_InitConfigType;
#endif /* CANIF_TYPES_H_ */
