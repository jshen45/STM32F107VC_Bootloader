/*============================================================================*/
/* Copyright (C) 2016-2017, Chengdu WeiTe Electronic Fuel CO.,LTD..
 *  
 *  All rights reserved. This software is WIT property. Duplication 
 *  or disclosure without WIT written authorization is prohibited.
 *  
 *  
 *  @file            :<Dcm_Dsl.c>
 *  @brief           :<Dcm for MPC5634> 
 *  @author          :<Mingdong.Yuan>
 *  @date            :<2016-8-2>
 *  @Current revision: $Revision: 1.0
 */
/*============================================================================*/

/*************************************************************************
Packages inclusion
*************************************************************************/
#include "Dcm_Dsl.h"
#include "Dcm_Dsp.h"
#include "Dcm_Types.h"
#include "Appl.h"
#include "Dcm.h"

/**=======[E X T E R N A L   D A T A]==========================================*/


/**=======[IN T E R N A L   D A T A]==========================================*/
EXPORTED Dcm_DslRunTimeProtocolParametersType dcmRunTime;

static DcmDsl_RunTimeDataType DcmDslRunTimeData = 
{
	/* initRun = */ FALSE,
	/* stActiveProtocol = */ NULL
};

/**************************************************************************
Private Functions
**************************************************************************/

/******************************************************************************/
/**
 * @brief               <stop reset timer>
 * 
 * <stop reset timer> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
LOCAL void StopResetTimer(void)
{    
    /* clear reset timer value */
    dcmRunTime.resetTimer = 0x00u;
    
    return;
}

/******************************************************************************/
/**
 * @brief               <process P3C timer>
 * 
 * <process P3C timer> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
LOCAL void S3ServerTimerCheck(void)
{
    if (dcmRunTime.S3ServerTimeoutCount > 0x00u)
    {
        dcmRunTime.S3ServerTimeoutCount --;
        
        if (0x00u == dcmRunTime.S3ServerTimeoutCount)
        {
			ChangeDiagnosticSession(&dcmRunTime,DCM_DEFAULT_SESSION);
			/* if S3Server timeout, set ECU reset to default session */
			Appl_EcuReset();
        }
    } 
    return;
}

/******************************************************************************/
/**
 * @brief               <process security timer>
 * 
 * <process security timer> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
LOCAL void SecurityTimerCheck(void)
{
    if (dcmRunTime.securityTimer > 0x00u)
    {
        dcmRunTime.securityTimer --;
    }
    
    return;
}

/******************************************************************************/
/**
 * @brief               <process reset timer>
 * 
 * <process reset timer> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
LOCAL void ResetTimerCheck(void)
{
    if (dcmRunTime.resetTimer > 0x00u)
    {
        dcmRunTime.resetTimer --;
        
        /* check if reset timer is timeout */
        if (0x00u == dcmRunTime.resetTimer)
        {
			/* ECU reset */
			Appl_EcuReset();
        }
    }
    
    return;
}

/**************************************************************************
Object:This function start the P2CanServer Timer 
Parameters:u16ProtocolTimer :  P2CanServer Timer used
Return:none
**************************************************************************/
LOCAL void StartP2CanServerTimer(const uint16 u16ProtocolTimer)
{
	dcmRunTime.p2eTimer = u16ProtocolTimer / DCM_MAIN_TICK;
}

/**************************************************************************
Object:This function start the P2CanServer Timer 
Parameters:u16ProtocolTimer :  P2CanServer Timer used
Return:none
**************************************************************************/
LOCAL void StopP2CanServerTimer(void)
{
	dcmRunTime.p2eTimer = KUL_NULL;
}

/******************************************************************************/
/**
 * @brief               <process P2E timer>
 * 
 * <process P2E timer,and send pending> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
LOCAL void P2CanServerTimerCheck(Dcm_DslRunTimeProtocolParametersType *stRuntime, const Dcm_DslProtocolRowType *stProtocolRow)
{
	const Dcm_DslProtocolTimingRowType *stTimingParams;
	
	stTimingParams = stProtocolRow->DcmDslProtocolTimeLimit;
	if (stRuntime->p2eTimer > 0x00u)
   {
        stRuntime->p2eTimer --;
        if (0x00u == stRuntime->p2eTimer)
        {            
            /* start P2E timer to 5000ms timeout */
				StartP2CanServerTimer(stTimingParams->TimStrP2StarServerMax);
            
            if(DCM_BUFF_FREE == Dcm_ComStatus.Dcm_TxBuffer.buffStatus)
            {
                if(stRuntime->pendingCount < DCM_PENDING_ATTEMPT_NUM)
                {
                    Dsl_SendNcr(DCM_E_PENDING);
                }
                else
                {
                    Dsl_SendNcr(DCM_E_GENERAL_REJECT);
                }  
            }
        }	
    }
    
    return;
}

/******************************************************************************/
/**
 * @brief               <process session timer P2E,P3C,reset ,security timer>
 * 
 * <process session timer P2E,P3C,reset,security timer> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
LOCAL void TimingParameterCheck(void)
{
    /* process security timer */
    SecurityTimerCheck();

    /* process S3Server timer */
    S3ServerTimerCheck();

    /* process P2E timer */
	 P2CanServerTimerCheck(&dcmRunTime, &DcmDslProtocolRowList);

    /* process ECU reset timer */
    ResetTimerCheck();
    
    return;
}

/**************************************************************************
Object:This function implements the requirement SWS_Dcm_00139 when transition 
       from one session to another. 
Parameters:stRuntime : structure of timing Parameters
           u8NewSession  : new session
Return:none
**************************************************************************/
LOCAL void ChangeDiagnosticSession(Dcm_DslRunTimeProtocolParametersType *stRuntime, Dcm_SesCtrlType u8NewSession) 
{
	/* @req SWS_Dcm_00139 */
	switch (stRuntime->curSession) 
	{
		case DCM_DEFAULT_SESSION: // "default".
			/* to set the dsp buffer to default*/
			Dsp_Init();
			break;

		case DCM_PROGRAMMING_SESSION:
		case DCM_EXTENDED_DIAGNOSTIC_SESSION:
		case DCM_SAFTEY_SYSTEM_DIAGNOSTIC_SESSION:
		case DCM_ALL_SESSION_LEVEL:
			stRuntime->securityLevel = DCM_SEC_LEV_LOCKED; // "0x00".
			break;

		default:
			break;
	}

	switch (u8NewSession) 
	{
		case DCM_DEFAULT_SESSION: // "default".
		case DCM_PROGRAMMING_SESSION:
		case DCM_EXTENDED_DIAGNOSTIC_SESSION:
		case DCM_SAFTEY_SYSTEM_DIAGNOSTIC_SESSION:
		case DCM_ALL_SESSION_LEVEL:
			stRuntime->curSession = u8NewSession;
			break;

		default:
			break;
	}
}

/******************************************************************************/
/**
 * @brief               <transmit response>
 * 
 * <transmit response, but not include pending> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
LOCAL void StartTransmit( void )
{	 
	 Dcm_ComStatus.Dcm_TxBuffer.buffStatus = DCM_BUFF_FOR_SERVICE;
    Dcm_ComStatus.Dcm_TxBuffer.pduId = DCM_TX_PDU_ID;
	 
    CanTp_Transmit(Dcm_ComStatus.Dcm_TxBuffer.pduId, &(Dcm_ComStatus.Dcm_TxBuffer.pduInfo)); 
    return;
}

/******************************************************************************/
/**
 * @brief               <start service process when received message>
 * 
 * <start service process when received message> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
LOCAL void ServiceStart(void)
{
	const Dcm_DslProtocolTimingRowType *stTimingParams;

	stTimingParams = Dcm_Config.DcmConfigSet->DcmDsl->DcmDslProtocol->DcmDslProtocolRow->DcmDslProtocolTimeLimit;

	/* Rx buffer in service status */
	Dcm_ComStatus.Dcm_RxBuffer.buffStatus = DCM_BUFF_FOR_SERVICE;

	/* start P2E timer */
	StartP2CanServerTimer(stTimingParams->TimStrP2ServerMax);

	/* stop P3C timer */
	Dsl_StopS3ServerTimer();

	/* a new service request is received */
	Dcm_ComStatus.Dcm_ReqNew = TRUE;

	Dcm_ComStatus.Dcm_RespFinished = FALSE;

	return;
}

/**************************************************************************
Exported Functions
**************************************************************************/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	Implements 'void Dcm_Init(void)' for DSL.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EXPORTED void Dsl_Init(void) 
{
	//const Dcm_DslProtocolRowType *listEntry;
	
	/* initialize tx and rx buffer */
   Dcm_ComStatus.Dcm_RxBuffer.pduInfo.sduDataPtr = Dcm_MainRxBuff;
   Dcm_ComStatus.Dcm_TxBuffer.pduInfo.sduDataPtr = Dcm_MainTxBuff;

	Dcm_ComStatus.Dcm_CurServicePtr = NULLPTR;
	Dcm_ComStatus.Dcm_RxBuffer.buffStatus = DCM_BUFF_FREE;
   Dcm_ComStatus.Dcm_TxBuffer.buffStatus = DCM_BUFF_FREE;

	memset(Dcm_MainRxBuff,0x00,sizeof(Dcm_MainRxBuff));
	memset(Dcm_MainTxBuff,0x00,sizeof(Dcm_MainTxBuff));
	/* set session to default session */
   Dsl_SetSessionMode(DCM_SESSION_DEFAULT);
    
   /* reset to security access level */
   Dsl_SetSecurityLevel(DCM_SECURITY_LOCKED);

	/* start security access timer */
   Dsl_StartSecurityTimer(DCM_SECURITY_TIME_ON_BOOT);

   /* stop ECU reset timer */
   StopResetTimer();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	Implements 'void Dcm_MainFunction(void)' for DSL.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EXPORTED void Dsl_MainFunction(void) 
{
   /* process P2CanServer, S3Server, security timer */
	TimingParameterCheck();
}

/******************************************************************************/
/**
 * @brief               <DCM module program initialize>
 * 
 * <when program boot request is equal to FL_EXT_PROG_REQUEST_RECEIVED,
 *  this API will be called in Appl_FlStartup function, session is initialized 
 *  to programming session,and simulate an 10 03 session control service is 
 *  received> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
EXPORTED void Dsl_ProgramInit(void)
{
    /* set session to extended session */
    Dsl_SetSessionMode(DCM_SESSION_EXTEND);
    
    /* simulate receive service 10 */
    Dcm_ComStatus.Dcm_RxBuffer.buffStatus = DCM_BUFF_FOR_TP;
    Dcm_ComStatus.Dcm_RxBuffer.pduId = DCM_RX_PHY_PDU_ID;
    
	 /* set received data */
    Dcm_ComStatus.Dcm_RxBuffer.pduInfo.sduDataPtr[0] = 0x10u;
    Dcm_ComStatus.Dcm_RxBuffer.pduInfo.sduDataPtr[1] = DCM_SESSION_PROGRAMMING;
    Dcm_ComStatus.Dcm_RxBuffer.pduInfo.sduLength = 0x02u;
	
	Dsl_StartSecurityTimer(0x00u);
    
	 /* start process service 0x10 */
    ServiceStart();
    
    return;
}

/******************************************************************************/
/**
 * @brief               <DCM module program initialize>
 * 
 * <when program boot request is equal to FL_EXT_PROG_REQUEST_RECEIVED,
 *  this API will be called in Appl_FlStartup function, session is initialized 
 *  to programming session,and simulate an 10 03 session control service is 
 *  received> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
EXPORTED void Dsl_ResetInit(void)
{
    /* set session to extended session */
    Dsl_SetSessionMode(DCM_SESSION_EXTEND);
    
    /* simulate receive service 10 */
    Dcm_ComStatus.Dcm_RxBuffer.buffStatus = DCM_BUFF_FOR_TP;
    
    Dcm_ComStatus.Dcm_RxBuffer.pduId = DCM_RX_PHY_PDU_ID;
    
	 /* set received data */
    Dcm_ComStatus.Dcm_RxBuffer.pduInfo.sduDataPtr[0] = 0x11u;
    Dcm_ComStatus.Dcm_RxBuffer.pduInfo.sduDataPtr[1] = 0x01u;

    Dcm_ComStatus.Dcm_RxBuffer.pduInfo.sduLength = 0x02u;
	
	 /* start process service 0x11 */
    ServiceStart();
    
    return;
}

/**************************************************************************
Object:This function Dsl rx indication from CanTp,mean an request is received. 
Parameters:pduId : Identification of the received I-PDU. 
           result  : Result of the reception. 
Return:none
**************************************************************************/
EXPORTED void Dsl_RxIndicationFromPduR(PduIdType pduId,NotifResultType result)
{
	 /* check if Rx buffer is provide for CanTp */
    if (DCM_BUFF_FOR_TP == Dcm_ComStatus.Dcm_RxBuffer.buffStatus)
    {
        /* Indication for MainBuff */
	    if ((NTFRSLT_OK == result) && (pduId == Dcm_ComStatus.Dcm_RxBuffer.pduId))
	    {
		   if(Dcm_ComStatus.Dcm_CurServicePtr->SID == 0x36)
		   {
		   		ServiceStart();
		   }
		   /* receive Ok */
           ServiceStart();
	    }
	    else
	    {
	        /* receive failed */
           Dsl_ServiceFinish();
	    }
    }
    
    return;
}

/******************************************************************************/
/**
 * @brief               <set session mode>
 * 
 * <set session mode> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <sessMode (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
EXPORTED void Dsl_SetSessionMode(const Dcm_SessionType sessMode)
{
    /* set current session */
    dcmRunTime.curSession = sessMode;
    
    return;
}

/**************************************************************************
Object:This function provides the active session control type value.  
Parameters:SesCtrlType : Active Session Control Type value
Return:E_OK/E_NOT_OK
**************************************************************************/
EXPORTED Std_ReturnType Dsl_GetSesCtrlType(Dcm_SesCtrlType *sesCtrlType) 
{ 
	/** @req SWS_Dcm_00022 *//** @req SWS_Dcm_00339 */
	Std_ReturnType u8ReturnCode = E_NOT_OK;

	*sesCtrlType = dcmRunTime.curSession;
	u8ReturnCode = E_OK;

	return u8ReturnCode;
}

/******************************************************************************/
/**
 * @brief               <set security level>
 * 
 * <set security level> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <secLev (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
EXPORTED void Dsl_SetSecurityLevel(const Dcm_SecurityType secLev)
{
    /* set current security level */
    dcmRunTime.securityLevel = secLev;
    
    return;
}

/**************************************************************************
Object:This function provides the active security level value. 
Parameters:secLevel : Active Security Level value
Return:E_OK/E_NOT_OK
**************************************************************************/
EXPORTED Std_ReturnType Dsl_GetSecurityLevel(Dcm_SecLevelType *secLevel) 
{  
	/** @req SWS_Dcm_00020 *//** @req [SWS_Dcm_00338 */
	Std_ReturnType u8ReturnCode = E_NOT_OK;

	*secLevel = dcmRunTime.securityLevel;
	u8ReturnCode = E_OK;
	
	return u8ReturnCode;
}

/******************************************************************************/
/**
 * @brief               <start security timer>
 * 
 * <start security timer> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <timeOut (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
EXPORTED void  Dsl_StartSecurityTimer(uint32 timeOut)
{  
    /* set to config value */
    dcmRunTime.securityTimer = timeOut / DCM_MAIN_TICK;
    
    return;
}

/******************************************************************************/
/**
 * @brief               <check if security timer is expired>
 * 
 * <check if security timer is expired> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <boolean>    
 */
/******************************************************************************/
EXPORTED boolean Dsl_GetSecurityTimerExpired(void)
{
    boolean ret = TRUE;

    /* check if security timer expired */
    if (dcmRunTime.securityTimer > 0)
    {
        ret = FALSE;
    }
    
    return ret;
}

/**************************************************************************
Object:This function stars the session (S3Server) timer. 
Parameters:stRuntime : structure of timing Parameters
           stProtocolRow  : const of Protocol Parameters
Return:none
**************************************************************************/
EXPORTED void Dsl_StartS3ServerTimer(Dcm_DslRunTimeProtocolParametersType *stRuntime, const Dcm_DslProtocolRowType *stProtocolRow) 
{
	const Dcm_DslProtocolTimingRowType *stTimingParams;
	
	stTimingParams = stProtocolRow->DcmDslProtocolTimeLimit;
	stRuntime->S3ServerTimeoutCount = DCM_CONVERT_MS_TO_MAIN_CYCLES(stTimingParams->TimStrS3Server);
	stRuntime->S3ServerStarted = TRUE;
}

/**************************************************************************
Object:This function reset the session (S3Server) timer. 
Parameters:none
Return:none
**************************************************************************/
EXPORTED void Dsl_StopS3ServerTimer(void) 
{
	dcmRunTime.S3ServerStarted      = FALSE;
	dcmRunTime.S3ServerTimeoutCount = 0x00;
}

/******************************************************************************/
/**
 * @brief               <start reset timer>
 * 
 * <start reset timer> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <timeOut (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
EXPORTED void Dsl_StartResetTimer(uint16 u16TimeOut)
{    
    /* set reset timer value */
    dcmRunTime.resetTimer = u16TimeOut / DCM_MAIN_TICK;
    
    return;
}

/******************************************************************************/
/**
 * @brief               <send response>
 * 
 * <send response> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
EXPORTED void Dsl_SendRsp(void) 
{
	 /* service process is finished */
	 
    Dcm_ComStatus.Dcm_RespFinished = TRUE;
    
    /* transmit message */
    StartTransmit();
    
    return;
}
/******************************************************************************/
/**
 * @brief               <transmit negative response>
 * 
 * <transmit negative response> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <nrcCode (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
EXPORTED void Dsl_SendNcr( uint8 nrcCode ) 
{
    uint8* pduData = (uint8*)&(Dcm_ComStatus.Dcm_TxBuffer.pduInfo.sduDataPtr[0]);
	 
	 /* check if send pending */
    if (nrcCode != DCM_E_PENDING)
    {
    	  /* if send NRC, service process is finished */
        Dcm_ComStatus.Dcm_RespFinished = TRUE;
    }
    else
    {
    	  /* if send pending , increat count */
        dcmRunTime.pendingCount ++;
    }
    
    pduData[0] = DCM_RSP_SID_NRC;
    pduData[1] = Dcm_ComStatus.Dcm_RxBuffer.pduInfo.sduDataPtr[0];
    pduData[2] = nrcCode;
    Dcm_ComStatus.Dcm_TxBuffer.pduInfo.sduLength = 0x03u;
    
    StartTransmit();
    
    return;
}
/**************************************************************************
Object:This function returns the active protocol name from DSL layer. 
Parameters:ActiveProtocol:Active protocol type value
Return:Std_ReturnType: E_OK: this value is always returned. 
**************************************************************************/
EXPORTED Std_ReturnType Dsl_GetActiveProtocol(Dcm_ProtocolType *u8ProtocolId) 
{ 
	/* @req SWS_Dcm_00340 */
	Std_ReturnType u8ReturnValue = E_NOT_OK;
	const Dcm_DslProtocolRowType *stActiveProtocol;

	stActiveProtocol = DcmDslRunTimeData.stActiveProtocol;
	if (stActiveProtocol != NULL) 
	{
		*u8ProtocolId = stActiveProtocol->DcmDslProtocolID;
		u8ReturnValue = E_OK;
	}
	return u8ReturnValue;
}

/******************************************************************************/
/**
 * @brief               <set service process finish>
 * 
 * <set service process finish> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
EXPORTED void Dsl_ServiceFinish(void)
{	
	 /* set current service */
    Dcm_ComStatus.Dcm_CurServicePtr = NULLPTR;

    /* initialize tx and rx buffer */
    Dcm_ComStatus.Dcm_RxBuffer.pduInfo.sduDataPtr = Dcm_MainRxBuff;
    Dcm_ComStatus.Dcm_TxBuffer.pduInfo.sduDataPtr = Dcm_MainTxBuff;
	
    /* initialize tx and rx buffer status */
	 Dcm_ComStatus.Dcm_RxBuffer.buffStatus = DCM_BUFF_FREE;
    Dcm_ComStatus.Dcm_TxBuffer.buffStatus = DCM_BUFF_FREE;
    
    /* initialize no new service request */
    Dcm_ComStatus.Dcm_ReqNew = FALSE;
    Dcm_ComStatus.Dcm_RespFinished = TRUE;

    /* clear pending count num */
    dcmRunTime.pendingCount = 0x00u;
    
    /* stop P2E timer */
    StopP2CanServerTimer();

    /* process P3C timer in extended and programming session */
    if (DCM_SESSION_DEFAULT == dcmRunTime.curSession)
    {
		  Dsl_StopS3ServerTimer();
    }
    else
    {
        Dsl_StartS3ServerTimer(&dcmRunTime,&DcmDslProtocolRowList);
    }
			
    return;
}

/*reserve*/
EXPORTED Std_ReturnType vRequestService_1_Start (Dcm_ProtocolType protocolID)
{
	protocolID = 0x00;
	if(protocolID == 0x00)
	{;}
	return E_OK;
}

/*reserve*/
EXPORTED Std_ReturnType vRequestService_1_Stop (Dcm_ProtocolType protocolID)
{
	protocolID = 0x00;
	if(protocolID == 0x00)
	{;}
	return E_OK;
}
