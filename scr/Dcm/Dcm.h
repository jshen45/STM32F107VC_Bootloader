/*============================================================================*/
/** Copyright (C) 2009-2011, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  @file       <Dcm.h>
 *  @brief      <Macro and function decalrations for Dcm Module>
 *  
 *  
 *  @author     <Gary Chen>
 *  @date       <2012-11-09>
 */
/*============================================================================*/

#ifndef DCM_H
#define DCM_H

/*=======[I N C L U D E S]====================================================*/
#include "Common_Types.h"
#include "Dcm_Types.h"

/*=======[M A C R O S]========================================================*/

/** Negtive response code */
typedef uint8 Dcm_NrcType;
#define DCM_E_GENERAL_REJECT                                0x10u
#define DCM_E_SERVICE_NOT_SUPPORTED                         0x11u
#define DCM_E_SUBFUNC_NOT_SUPPORTED                         0x12u
#define DCM_E_INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT    0x13u
#define DCM_E_BUSY_REPEAT_REQUEST                           0x21u
#define DCM_E_CONDITION_NOT_CORRECT                         0x22u
#define DCM_E_REQUEST_SEQUENCE_ERROR                        0x24u
#define DCM_E_REQUEST_OUT_OF_RANGE                          0x31u
#define DCM_E_SECURITY_ACCESS_DENIED                        0x33u
#define DCM_E_INVALID_KEY                                   0x35u
#define DCM_E_EXCEEDED_NUMBER_OF_ATTEMPTS                   0x36u
#define DCM_E_REQUIRED_TIME_DELAY_NOT_EXPIRED               0x37u
#define DCM_E_UPLOAD_DOWNLOAD_NOT_ACCEPTED                  0x70u
#define DCM_E_TRANSFER_DATA_SUSPENDED                       0x71u
#define DCM_E_GENERAL_PROGRAMMING_FAILURE                   0x72u
#define DCM_E_WRONG_BLOCK_SEQUENCE_COUNTER                  0x73u
#define DCM_E_PENDING                                       0x78u
#define DCM_E_SUBFUNC_NOT_SUPPORTED_INACTIVE_SESSION        0x7Eu
#define DCM_E_SERVICE_NOT_SUPPORTED_INACTIVE_SESSION        0x7Fu
#define DCM_E_VOLTAGE_TOO_HIGH                              0x92u
#define DCM_E_VOLTAGE_TOO_LOW                               0x93u

/** SID negative response */
#define DCM_RSP_SID_NRC 0x7Fu

/** if response needed */
#define DCM_RSP_REQUIRED        0x00u
#define DCM_RSP_NOT_REQUIRED    0x80u
#define DCM_RSP_CLEAR_REQUIRED  0x7Fu

/*=======[T Y P E   D E F I N I T I O N S]====================================*/
/** Dcm communicate status */
typedef struct
{
    Dcm_BuffType	Dcm_RxBuffer;  /* DCM main rx buffer */
    Dcm_BuffType	Dcm_TxBuffer;  /* DCM main tx buffer */
    const Dcm_ServiceTableType * Dcm_CurServicePtr;   /* DCM current service */
    boolean Dcm_ReqNew;            /* if there is a new service request is not processed */
    boolean Dcm_RespFinished;      /* if there is response is finished */  
}Dcm_ComStatusType;
/*=======[E X T E R N A L   D A T A]==========================================*/
extern uint8	Dcm_MainRxBuff[DCM_RX_BUF_SIZE];
extern uint8	Dcm_MainTxBuff[DCM_TX_BUF_SIZE];
extern Dcm_ComStatusType Dcm_ComStatus;
/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
extern void Dcm_MainFunction( void );
extern void Dcm_TpRxIndication( PduIdType id, Std_ReturnType result );
BufReq_RetType Dcm_ProvideRxBuffer(PduIdType pduId,PduLengthType pduLen,PduInfoType **sduInfo );

BufReq_RetType Dcm_ProvideTxBuffer(PduIdType pduId,PduInfoType **sduInfo);

void Dcm_TxConfirmation(PduIdType pduId,NotifResultType result);

extern Std_ReturnType Dcm_GetSesCtrlType(Dcm_SesCtrlType *SesCtrlType);
extern Std_ReturnType Dcm_GetSecurityLevel(Dcm_SecLevelType* SecLevel);
extern void DCM_ManageEvery5ms(void);
#endif/* endof DCM_H */

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121109    Gary       Initial version
 */
/*=======[E N D   O F   F I L E]==============================================*/


