/*============================================================================*/
/*** Copyright (C) 2009-2011, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  
 *  @file       <CanTp.h>
 *  @brief      <The CanTp used type defined and extern function declarations>
 *  
 *  
 *  @author     <Tommy Yu>
 *  @date       <2012-12-28>
 */
/*============================================================================*/
 
#ifndef CAN_TP_H
#define CAN_TP_H

/*=======[I N C L U D E S]====================================================*/

#include "Common_Types.h"
#include "CanTp_Cfg.h"

/*=======[M A C R O S]========================================================*/

/* followed item is for NotifResultType */
#define  NTFRSLT_OK                    (NotifResultType)(0x00u)
#define  NTFRSLT_E_NOT_OK              (NotifResultType)(0x01u)
#define  NTFRSLT_E_TIMEOUT_A           (NotifResultType)(0x02u)
#define  NTFRSLT_E_TIMEOUT_BS          (NotifResultType)(0x03u)
#define  NTFRSLT_E_TIMEOUT_CR          (NotifResultType)(0x04u)
#define  NTFRSLT_E_WRONG_SN            (NotifResultType)(0x05u)
#define  NTFRSLT_E_INVALID_FS          (NotifResultType)(0x06u)
#define  NTFRSLT_E_UNEXP_PDU           (NotifResultType)(0x07u)
#define  NTFRSLT_E_WFT_OVRN            (NotifResultType)(0x08u)
#define  NTFRSLT_E_NO_BUFFER           (NotifResultType)(0x09u)
#define  NTFRSLT_E_CANCELATION_OK      (NotifResultType)(0x0Au)
#define  NTFRSLT_E_CANCELATION_NOT_OK  (NotifResultType)(0x0Bu)

/*=======[T Y P E   D E F I N I T I O N S]====================================*/
typedef enum 
{
	CANTP_EXTENDED, 
	CANTP_STANDARD
} CanTp_Address_FormatType;

typedef enum
{
	TP_STATE_IDLE = 0u,
	
	/* wait rx data buffer */
	/* wait buffer for single frame */
	WAITING_RX_SF_BUFFER,
	
	/* wait buffer for SF or CF frame,send control frame */
	WAITING_RX_CF_BUFFER,								
	
	/* wait FC confirmation  */
	/* FC with CTS status confirmation */
	WAITING_FOR_FC_CTS_TX_CONFIRMATION,	
	
	/* FC with overflow status confirmation */
	WAITING_FOR_FC_OVFLW_TX_CONFIRMATION,				
	
	/* wait for cf frame */
	WAITING_FOR_CF_RX,									
	
	/* wait buffer for SF or FF, maybe is not usefull */
	WAITING_TX_BUFFER, 									
	
	/* single process */
	/* Single fram only */
	WAITING_FOR_SF_TX_CONFIRMATION,
	
	/* FF,CF process */
	WAITING_FOR_FF_TX_CONFIRMATION,	
	WAITING_FOR_FC_RX,
	
	/* transmit CF */
	WAITING_FOR_CF_TX,	
	WAITING_FOR_LAST_CF_TX_CONFIRMATION,	
	WAITING_FOR_CF_TX_CONFIRMATION,
	WAITING_FOR_CF_BLOCK_TX_CONFIRMATION
}CanTp_StateType;

/*=======[E X T E R N A L   D A T A]==========================================*/
extern uint8 Cantp_test;

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
extern Std_ReturnType CanTp_Transmit(const PduIdType CanTp_txSduId, PduInfoType *CanTp_txInfoPtr);
extern void CanTp_RxIndication(PduIdType RxPduId, const PduInfoType *PduInfoPtr);
extern void CanTp_TxConfirmation(PduIdType TxPduId);
extern void CanTp_MainFunction(void);
extern void CanTp_Init(const CanTp_ConfigType *CfgPtr);  

#endif


/***=======[R E V I S I O N   H I S T O R Y]====================================*/
/**** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121228     Tommy Yu     Initial version
 * 
 */
/***=======[E N D   O F   F I L E]==============================================*/


