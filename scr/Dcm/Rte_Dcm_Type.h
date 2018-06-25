/*============================================================================*/
/* Copyright (C) 2015-2016, Chengdu WeiTe Electronic Fuel CO.,LTD.
 *  
 *  All rights reserved. This software is WIT property. Duplication 
 *  or disclosure without WIT written authorization is prohibited.
 *  
 *  
 *  @file            :<Rte_Dcm_Type.h>
 *  @brief           :<Dcm for MPC5634> 
 *  @author          :<Mingdong.Yuan>
 *  @date            :<2016-7-15>
 *  @Current revision: $Revision: 1.0
 */
/*============================================================================*/

#ifndef RTE_DCM_TYPE_H_
#define RTE_DCM_TYPE_H_

/*************************************************************************
Packages inclusion
*************************************************************************/
#include "Rte_Type.h"
#include "can.h"

typedef enum
{
    DCM_BUFF_FREE,
     
    DCM_BUFF_FOR_TP,
	
    DCM_BUFF_FOR_SERVICE
	
} Dcm_BuffStatusType;

/** struct of DCM Buffer type*/
typedef struct
{
    /* status of this buffer */
    Dcm_BuffStatusType buffStatus;

    /* PduId of this buffer */
    PduIdType pduId;

    /* Pdu Data of this buffer */
    PduInfoType pduInfo;
} Dcm_BuffType;
/*************************************************************************
Declaration of constants
*************************************************************************/
/* AUTOSAR v4.2.2 SWS_Dcm_00980:Dcm_NegativeResponseCodeType */
typedef uint8 Dcm_NegativeResponseCodeType;

#define DCM_E_POSITIVERESPONSE								((Dcm_NegativeResponseCodeType)0x00)
#define DCM_E_GENERALREJECT									((Dcm_NegativeResponseCodeType)0x10)
#define DCM_E_SERVICENOTSUPPORTED							((Dcm_NegativeResponseCodeType)0x11)
#define DCM_E_SUBFUNCTIONNOTSUPPORTED						((Dcm_NegativeResponseCodeType)0x12)
#define DCM_E_INCORRECTMESSAGELENGTHORINVALIDFORMAT	((Dcm_NegativeResponseCodeType)0x13)

#define DCM_E_BUSYREPEATREQUEST								((Dcm_NegativeResponseCodeType)0x21)
#define DCM_E_CONDITIONSNOTCORRECT							((Dcm_NegativeResponseCodeType)0x22)
#define DCM_E_REQUESTSEQUENCEERROR							((Dcm_NegativeResponseCodeType)0x24)

#define DCM_E_REQUESTOUTOFRANGE								((Dcm_NegativeResponseCodeType)0x31)
#define DCM_E_SECUTITYACCESSDENIED							((Dcm_NegativeResponseCodeType)0x33)

#define DCM_E_GENERALPROGRAMMINGFAILURE					((Dcm_NegativeResponseCodeType)0x72)
#define DCM_E_RESPONSEPENDING									((Dcm_NegativeResponseCodeType)0x78)
#define DCM_E_SUBFUNCTIONNOTSUPPORTEDINACTIVESESSION	((Dcm_NegativeResponseCodeType)0x7E)
#define DCM_E_SERVICENOTSUPPORTEDINACTIVESESSION		((Dcm_NegativeResponseCodeType)0x7F)

#define DCM_E_RPMTOOHIGH										((Dcm_NegativeResponseCodeType)0x81)
#define DCM_E_RPMTOLOW											((Dcm_NegativeResponseCodeType)0x82)
#define DCM_E_ENGINEISRUNNING									((Dcm_NegativeResponseCodeType)0x83)
#define DCM_E_ENGINEISNOTRUNNING								((Dcm_NegativeResponseCodeType)0x84)
#define DCM_E_ENGINERUNTIMETOOLOW							((Dcm_NegativeResponseCodeType)0x85)
#define DCM_E_TEMPERATURETOOHIGH								((Dcm_NegativeResponseCodeType)0x86)
#define DCM_E_TEMPERATURETOOLOW								((Dcm_NegativeResponseCodeType)0x87)
#define DCM_E_VEHICLESPEEDTOOHIGH							((Dcm_NegativeResponseCodeType)0x88)
#define DCM_E_VEHICLESPEEDTOOLOW								((Dcm_NegativeResponseCodeType)0x89)
#define DCM_E_THROTTLE_PEDALTOOHIGH							((Dcm_NegativeResponseCodeType)0x8A)
#define DCM_E_THROTTLE_PEDALTOOLOW							((Dcm_NegativeResponseCodeType)0x8B)
#define DCM_E_TRANSMISSIONRANGENOTINNEUTRAL				((Dcm_NegativeResponseCodeType)0x8C)
#define DCM_E_TRANSMISSIONRANGENOTINGEAR					((Dcm_NegativeResponseCodeType)0x8D)
#define DCM_E_BRAKESWITCH_NOTCLOSED							((Dcm_NegativeResponseCodeType)0x8F)

#define DCM_E_SHIFTERLEVERNOTINPARK							((Dcm_NegativeResponseCodeType)0x90)
#define DCM_E_TORQUECONVERTERCLUTCHLOCKED					((Dcm_NegativeResponseCodeType)0x91)
#define DCM_E_VOLTAGETOOHIGH									((Dcm_NegativeResponseCodeType)0x92)
#define DCM_E_VOLTAGETOOLOW									((Dcm_NegativeResponseCodeType)0x93)

/*UDS service ID*/
#define M_U8_CMNSRV_START_SESSION_SID         ((uint8)0x10)
#define M_U8_CMNSRV_TESTER_PRESENT_SID        ((uint8)0x3E)
#define M_U8_CMNSRV_TRANSMIT_CONTROL_SID      ((uint8)0x28)
#define M_U8_CMNSRV_ECU_RESET_SID             ((uint8)0x11)
#define M_U8_CMNSRV_SECURITY_ACCESS_SID       ((uint8)0x27)
#define M_U8_CMNSRV_READ_DATA_SID             ((uint8)0x22)
#define M_U8_CMNSRV_WRITE_DATA_SID            ((uint8)0x2E)
#define M_U8_CMNSRV_ROUTINE_CONTROL_SID       ((uint8)0x31)
#define M_U8_CMNSRV_REQUEST_DOWNLOAD_SID      ((uint8)0x34)
#define M_U8_CMNSRV_TRANSFER_DATA_SID         ((uint8)0x36)
#define M_u8_CMNSRV_REQUEST_TRANSFER_EXIT     ((uint8)0x37)
#define M_U8_CMNSRV_CONTROL_DTC_SETTING       ((uint8)0x85)

/* Autosar DCM v4.2.2,Dcm_SesCtrlType */
typedef uint8 Dcm_SesCtrlType;
#define DCM_DEFAULT_SESSION						((Dcm_SesCtrlType)0x01)
#define DCM_PROGRAMMING_SESSION					((Dcm_SesCtrlType)0x02)
#define DCM_EXTENDED_DIAGNOSTIC_SESSION		((Dcm_SesCtrlType)0x03)
#define DCM_SAFTEY_SYSTEM_DIAGNOSTIC_SESSION	((Dcm_SesCtrlType)0x04)
#define DCM_ALL_SESSION_LEVEL					   ((Dcm_SesCtrlType)0xFF)

/* DCM session index */
#define DCM_SESSION_DEFAULT_INDEX			((uint8)0x00u)
#define DCM_SESSION_PROGRAMMING_INDEX		((uint8)0x01u)
#define DCM_SESSION_EXTENDED_INDEX			((uint8)0x02u)
#define DCM_SESSION_EOL_INDEX					((uint8)0x03u)

/** service 10 diagnosticSessionType */
typedef uint8 Dcm_SessionType;
#define DCM_SESSION_DEFAULT     0x01u
#define DCM_SESSION_PROGRAMMING 0x02u
#define DCM_SESSION_EXTEND      0x03u

/* Autosar DCM v4.2.2, Dcm_SecLevelType */
typedef uint8 Dcm_SecLevelType;
#define DCM_SEC_LEV_LOCKED			((Dcm_SecLevelType)0x00)
#define DCM_SEC_LEV_L1				((Dcm_SecLevelType)0x01)
#define DCM_SECURITY_EOL_INDEX	((Dcm_SecLevelType)0x02)
#define DCM_SEC_LEV_ALL				((Dcm_SecLevelType)0xFF)

/** service 27 security access lock type */
typedef uint8 Dcm_SecurityType;
#define DCM_SECURITY_LOCKED     0x00u
#define DCM_SECURITY_LEV1       0x01u
#define DCM_SECURITY_LEV2       0x02u

/** service 11 resetType */
typedef uint8 Dcm_ResetType;
#define DCM_HARD_RESET  0x01u

/** service 31 routine control type */
typedef uint8 Dcm_RoutineControlType;
#define DCM_START_ROUTINE   0x01u

/** service 28 communication control */
typedef uint8 Dcm_ComControlType;
#define DCM_ENABLE_RXANDTX      0x00u
#define DCM_ENABLE_RXANDDISTX   0x01u

typedef uint8 Dcm_CommunicationType;
#define DCM_NORMAL_COM_MESSAGES 0x01u

/** service 85 DTC setting type */
typedef uint8 Dcm_DTCSettingType;
#define DCM_DTC_SETTING_ON  0x01u
#define DCM_DTC_SETTING_OFF 0x02u

/*service 22  ReadData By Identifier DID define */
#define DCM_VIN_DATA_IDENTIFIER  ((uint16)0xF190u)
#define DCM_FINGER_PRINT_DATA    ((uint16)0xF15Au)


/* Autosar DCM v4.2.2, 8.8.1.5 Dcm_ProtocolType */
typedef uint8 Dcm_ProtocolType;
#define DCM_OBD_ON_CAN						((Dcm_ProtocolType)0x00)
#define DCM_UDS_ON_CAN						((Dcm_ProtocolType)0x01)
#define DCM_UDS_ON_FLEAXRAY					((Dcm_ProtocolType)0x02)
#define DCM_ROE_ON_CAN						((Dcm_ProtocolType)0x03)
#define DCM_ROE_ON_FLEXRAY					((Dcm_ProtocolType)0x04)
#define DCM_PERIODICTRANS_ON_CAN			((Dcm_ProtocolType)0x05)
#define DCM_PERIODICTRANS_ON_FLEXRAY		((Dcm_ProtocolType)0x06)

typedef uint8 Dcm_ProtocolAddrType;
#define DCM_PROTOCOL_FUNCTIONAL_ADDR_TYPE	   ((Dcm_ProtocolAddrType)0x01)
#define DCM_PROTOCOL_PHYSICAL_ADDR_TYPE		((Dcm_ProtocolAddrType)0x02)

/* PduR Rx NSdu Id */
#define PDUR_RX_vEcuC_Pdu_1   1

/* PduR Tx NSdu Id */
#define PDUR_TX_vEcuC_Pdu_1 	0

#endif /* RTE_DCM_TYPE_H_ */
