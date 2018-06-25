/*============================================================================*/
/** Copyright (C) 2009-2011, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  
 *  @file       <Can_cfg.h>
 *  @brief      <can configuration macros>
 *  
 *  
 *  @author     <Tommy Yu>
 *  @date       <2012-12-28>
 */
/*============================================================================*/
 
#ifndef CAN_CFG_H
#define CAN_CFG_H


/*=======[I N C L U D E S]====================================================*/
#include "Can.h"

/*=======[M A C R O S]========================================================*/

/** select controller  */
/* @type:define range:CAN_CONTROLLER_A~CAN_CONTROLLER_C note:NONE */
#define CAN_CONTROLLER_USED  CAN_CONTROLLER_A

/* define CAN TX and RX PDU number */
/* @type:uint8 range:1~255 note:reference to num of CanIf_TxPduConfigData */
#define CAN_TX_PDU_NUM	    3u

/* @type:uint8 range:1~255 note:reference to num of CanIf_RxPduConfigData */
#define CAN_RX_PDU_NUM	    2u

//set number of used msgboxes, max is 64
#define CAN_NUM_MSGBOXES	 64u

/*define the number of CAN controller*/
#define CAN_NUM_CONTROLLERS 1u


#define CAN_OBJECT_TYPE_RECEIVE   0x00u
#define CAN_OBJECT_TYPE_TRANSMIT  0x01u

typedef struct 
{
	const uint16  pduId;	
	const uint32  canId;
	const uint8   controller;            /* controller idx that this hoh is connected to*/
	const boolean standardFrame;
   const uint32  Can_TxMailBoxMask;     /* Used for Tx Mail Box select */

} CanTxPduType;


extern const CanTxPduType Can_TxPdu[CAN_TX_PDU_NUM];


#endif /* endof CAN_CFG_H */


/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121228   Tommy Yu     Initial version
 * 
 */
/*=======[E N D   O F   F I L E]==============================================*/

