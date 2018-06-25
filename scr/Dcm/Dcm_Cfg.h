/*============================================================================*/
/** Copyright (C) 2009-2011, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  @file       <Dcm_Cfg.h>
 *  @brief      <Dcm Configuration file>
 *              include Dcm used Timer and Macros defined.
 *  
 *  @author     <Gary Chen>
 *  @date       <2012-11-09>
 */
/*============================================================================*/

#ifndef DCM_CFG_H
#define DCM_CFG_H
 
/*=======[M A C R O S]========================================================*/
/** main receive buffer size */
/* @type:uint16 range:1~4095 note:NONE */
#define DCM_RX_BUF_SIZE    4095u

/** main send buffer size */
/* @type:uint16 range:1~4095 note:NONE */
#define DCM_TX_BUF_SIZE    80u

/** DCM pduId use for CanTp */
/* @type:define range:NONE note:reference to CanTpNSduConfigList.CanTpTxNSdu->txDcmId */
#define DCM_TX_PDU_ID       0xAAu

/* @type:define range:NONE note:reference to CanTpNSduConfigList.CanTpRxNSdu->rxDcmId */
#define DCM_RX_PHY_PDU_ID   0xCCu

/* @type:define range:NONE note:reference to CanTpNSduConfigList.CanTpRxNSdu->rxDcmId */
#define DCM_RX_FUNC_PDU_ID  0xFFu

/** DCM protocol time */
/* @type:define range:1~65535 note:unit ms ,P2CAN_Server*/
#define DCM_P2MAX_TIME  40u/* ms */
//#define DCM_P2MAX_TIME  60u/* ms */
/* @type:define range:1~65535 note:unit ms ,P2*CAN_Server*/
#define DCM_P2SMAX_TIME 4000u/* ms */

/* @type:define range:1~65535 note:unit ms ,S3Server*/
#define DCM_P3MAX_TIME  5000u/* ms */

/** DCM main task tick time */
/* @type:define range:NONE note:auto generate */
//#define DCM_MAIN_TICK   GPT_PERIOD_TIME
#define DCM_MAIN_TICK   5

/** pending send attemt num */
/* @type:define range:1~65535 note:NONE */
#define DCM_PENDING_ATTEMPT_NUM     100u

/** DCM security access time */
/* security access timeout on boot */
/* @type:define range:0~65535 note:unit ms */
#define DCM_SECURITY_TIME_ON_BOOT   0u

/* security access timeout when failed */
/* @type:define range:0~65535 note:unit ms */
#define DCM_SECURITY_TIME           3000u

/* security access attempt max num */
/* @type:define range:0~65535 note:NONE */
#define DCM_SECURITY_ATTEMPT_NUM    2u

/** session supportted num */
/* @type:define range:NONE note:auto generate */
#define DCM_SESSION_SUPPORT_NUM    3

/** security supportted num */
/* @type:define range:NONE note:auto generate */
#define DCM_SECURITY_SUPPORT_NUM    3

/** service table num */
/* @type:define range:1~255 note:reference to Dcm_ServiceTable */
#define DCM_SERVICE_NUM         12

/** session mode num */
/* @type:define range:1~3 note:reference to Dcm_SessionRow */
#define DCM_SESSION_NUM         3

/** reset mode num */
/* @type:define range:1~255 note:reference to Dcm_ResetRow */
#define DCM_RESET_NUM           1

/** read data ID num */
/* @type:define range:0~255 note:reference to Dcm_ReadDidRow */
#define DCM_READDID_NUM         1

/** security level num,not include locked level */
/* @type:define range:1~2 note:reference to Dcm_SecurityRow */
#define DCM_SECURITY_NUM        2

/** communication control num */
/* @type:define range:0~255 note:reference to Dcm_ComControlRow */
#define DCM_COM_CONTROL_NUM     2

/** write data ID num */
/* @type:define range:1~255 note:reference to Dcm_WriteDidRow */
#define DCM_WRITEDID_NUM        1

/** routine control ID num */
/** write data ID num */
/* @type:define range:1~255 note:reference to Dcm_RoutineControlRow */
#define DCM_ROUTINE_CONTROL_NUM 4

/** DTC settin mode num */
/* @type:define range:0~255 note:reference to Dcm_DTCSettingRow */
#define DCM_DTC_SET_NUM         2

/* SecurityAccess */
#define M_U8_CMNSRV_SECURITY_ACCESS_SID      ((uint8)0x27)
#define M_U8_CMNSRV_SECURITY_LEVEL1_SEED     ((uint8)0x11)
#define M_U8_CMNSRV_SECURITY_LEVEL2_SEED     ((uint8)0x01)
#define M_U8_CMNSRV_SECURITY_LEVEL1_KEY   	((uint8)0x12)
#define M_U8_CMNSRV_SECURITY_LEVEL2_KEY   	((uint8)0x02)


#endif/* endof DCM_CFG_H */

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121109    Gary       Initial version
 */
/*=======[E N D   O F   F I L E]==============================================*/



