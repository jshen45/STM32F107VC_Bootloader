/*============================================================================*/
/* Copyright (C) 2016-2017, Chengdu WeiTe Electronic Fuel CO.,LTD..
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  
 *  @file       <Dcm_Dsl.h>
 *  @brief      <Ccp for MPC5634> 
 *  @author     <Mingdong.Yuan>
 *  @date       <2016-8-2>
 */
/*============================================================================*/
#ifndef DCM_DSL_H_
#define DCM_DSL_H_

/*************************************************************************
Packages inclusion
*************************************************************************/
#include "Dcm_Types.h"
#include "Common_Types.h"
#include "Rte_Dcm_Type.h"
#include "CanTp.h"
/*=======[M A C R O S]========================================================*/
#define DCM_MAIN_FUNCTION_PERIOD_TIME_MS	((uint8)0x05)
#define DCM_CONVERT_MS_TO_MAIN_CYCLES(x)  ((x)/DCM_MAIN_FUNCTION_PERIOD_TIME_MS)

#define DCM_DSL_TX_PDU_ID_LIST_LENGTH		((uint8)0x01)
#define DCM_DSL_RX_PDU_ID_LIST_LENGTH		((uint8)0x01)

/*=======[T Y P E   D E F I N I T I O N S]====================================*/
typedef struct 
{
	PduIdType				u16DiagReqestRxPduId;         // Tester request PduId.
	uint32					u32StateTimeoutCounter;       // Counter for timeout.
   uint16               resetTimer;                   /* ECU reset Timer */
   uint32               securityTimer;                /* DCM security access timer */
	//Dcm_DslBufferUserType	externalRxBufferStatus;
	//PduInfoType				diagnosticRequestFromTester;
	//PduInfoType				diagnosticResponseFromDsd;
	//Dcm_DslBufferUserType	externalTxBufferStatus;
	boolean					bProtocolStarted;             // Has the protocol been started?
	//Dcm_DslLocalBufferType	localRxBuffer;
	//Dcm_DslLocalBufferType	localTxBuffer;
	//boolean					diagnosticActiveComM; 
	uint16					S3ServerTimeoutCount;
	boolean					S3ServerStarted;  
   uint16               p2eTimer;              /* DCM P2e timer */
	uint16               pendingCount;
	uint8					   responsePendingCount;
	Dcm_SecLevelType		   securityLevel;      //Dcm_SecurityLevel;
	Dcm_SesCtrlType			curSession;         //Dcm_SessionControl;
}Dcm_DslRunTimeProtocolParametersType;

typedef struct 
{
	boolean initRun;
	const Dcm_DslProtocolRowType *stActiveProtocol; // Points to the currently active protocol.
}DcmDsl_RunTimeDataType;

/*************************************************************************
Declaration of variables
*************************************************************************/
extern Dcm_DslRunTimeProtocolParametersType dcmRunTime;
/*************************************************************************
Declaration of functions
*************************************************************************/
/*=======[I N T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
static void StopResetTimer(void);
static void TimingParameterCheck(void);
static void S3ServerTimerCheck(void);
static void SecurityTimerCheck(void);
static void StartP2CanServerTimer(const uint16 u16ProtocolTimer);
static void StopP2CanServerTimer(void);
static void ChangeDiagnosticSession(Dcm_DslRunTimeProtocolParametersType *stRuntime, Dcm_SesCtrlType u8NewSession);
static void P2CanServerTimerCheck(Dcm_DslRunTimeProtocolParametersType *stRuntime, const Dcm_DslProtocolRowType *stProtocolRow);
static void TimingParameterCheck(void);
static void StartTransmit(void);
static void ServiceStart(void);
/*
static boolean FindRxPduIdParaConfiguration(PduIdType dcmRxPduId,
		                                     const Dcm_DslProtocolRowType **protocolRow,
														 const Dcm_DslMainConnectionType **mainConnection,
														 const Dcm_DslConnectionType **connection,
														 const Dcm_DslProtocolRxType **protocolRx,
														 Dcm_DslRunTimeProtocolParametersType **runtime);
*/														 
/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
extern void Dsl_Init(void);
extern void Dsl_MainFunction(void);
extern void Dsl_ProgramInit(void);
extern void Dsl_ResetInit(void);
extern void Dsl_RxIndicationFromPduR(PduIdType pduId,NotifResultType result);
extern void Dsl_SetSessionMode(const Dcm_SessionType sessMode);
extern Std_ReturnType Dsl_GetSesCtrlType(Dcm_SesCtrlType *sesCtrlType);
extern void Dsl_SetSecurityLevel(const Dcm_SecurityType secLev);
extern Std_ReturnType Dsl_GetSecurityLevel(Dcm_SecLevelType *secLevel); 
extern void Dsl_StartSecurityTimer(uint32 timeOut);
extern boolean Dsl_GetSecurityTimerExpired(void);
extern void Dsl_StartS3ServerTimer(Dcm_DslRunTimeProtocolParametersType *stRuntime, const Dcm_DslProtocolRowType *stProtocolRow);
extern void Dsl_StopS3ServerTimer(void);
extern void Dsl_StartResetTimer(uint16 u16TimeOut);
extern void	Dcm_StopP3cTimer(void);
extern void	Dcm_StartP3cTimer(void);
extern void Dsl_SendRsp(void); 
extern void Dsl_SendNcr( uint8 nrcCode );
extern Std_ReturnType Dsl_GetActiveProtocol(Dcm_ProtocolType *u8ProtocolId); 
extern void Dsl_ServiceFinish(void);
extern Std_ReturnType vRequestService_1_Start (Dcm_ProtocolType protocolID);
extern Std_ReturnType vRequestService_1_Stop (Dcm_ProtocolType protocolID);


#endif /* DCM_DSL_H_ */
