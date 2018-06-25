/*============================================================================*/
/** Copyright (C) 2009-2011, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  @file       <Dcm.c>
 *  @brief      <UDS Service - ISO14229>
 *  
 *  <This Diagnostic Communication Manager file contained UDS services
 *   which used for bootloader project>
 *  @author     <Gary Chen>
 *  @date       <2012-11-09>
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "Dcm.h"
#include "Dcm_Types.h"
#include "Dcm_Internel.h"
#include "Rte_Dcm.h"
#include "Dcm_Dsl.h"
#include "Dcm_Dsd.h"
#include "Dcm_Dsp.h"

/*=======[T Y P E   D E F I N I T I O N S]====================================*/

/** Dcm communicate status */
typedef struct
{
    /* DCM main rx buffer */
    Dcm_BuffType	rxBuff;
    
    /* DCM main tx buffer */
    Dcm_BuffType	txBuff;

    /* DCM current service */
    const Dcm_ServiceTableType * curServicePtr;

    /* if there is a new service request is not processed */
    boolean reqNew;

    /* if there is response is finished */
    boolean respFinished;

    uint16 pendingCount;
        
    /* DCM P3C timer */
    uint16 p3cTimer;

    /* DCM P2e timer */
    uint16 p2eTimer;
    
} Dcm_ComType;

/*=======[E X T E R N A L   D A T A]==========================================*/
/** use static memory for service */
EXPORTED uint8	Dcm_MainRxBuff[DCM_RX_BUF_SIZE];
EXPORTED uint8	Dcm_MainTxBuff[DCM_TX_BUF_SIZE];

EXPORTED Dcm_ComStatusType Dcm_ComStatus;
/*=======[I N T E R N A L   D A T A]==========================================*/

/*=======[I N T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
STATIC void	Dcm_StartP3cTimer(void);
STATIC void	Dcm_StopP3cTimer(void);

/*=======[F U N C T I O N   I M P L E M E N T A T I O N S]====================*/
EXPORTED void Dcm_Init(const Dcm_ConfigType *ConfigPtr)
{
	if((ConfigPtr->DcmConfigSet->DcmDsl != NULL) && (ConfigPtr->DcmConfigSet->DcmDsd != NULL) && (ConfigPtr->DcmConfigSet->DcmDsp != NULL))
	{
		Dsl_Init();
		Dsd_Init();
		Dsp_Init();
	}
}

/******************************************************************************/
/**
 * @brief               <DCM main task function>
 * 
 * <DCM main task function> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
void Dcm_MainFunction(void)
{
    /* process P2CanServer, S3Server, security timer */
    Dsl_MainFunction();
    /* process service */
    Dsd_MainFunction();

    return;
}

/******************************************************************************/
/**
 * @brief               <DCM provide rx buffer for CanTp>
 * 
 * <DCM provide rx buffer for CanTp,mean an request is receiving> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <pduId (IN), pduLen (IN)>
 * @param[out]          <sduInfo (OUT)>
 * @param[in/out]       <NONE>
 * @return              <BufReq_RetType>    
 */
/******************************************************************************/
BufReq_RetType Dcm_ProvideRxBuffer(PduIdType pduId,PduLengthType pduLen,PduInfoType **sduInfo )
{
    BufReq_RetType ret = BUFREQ_OK;
    
    /* Rx buffer is free state and pduid is correct */
    if ((DCM_BUFF_FREE == Dcm_ComStatus.Dcm_RxBuffer.buffStatus) &&
        ((DCM_RX_PHY_PDU_ID == pduId) ||
         (DCM_RX_FUNC_PDU_ID == pduId)))
    {
	    /* Main Buffer should be free */
	    if (pduLen <= DCM_RX_BUF_SIZE)
	    {
	        /* allocate MainRxBuff */
		    Dcm_ComStatus.Dcm_RxBuffer.buffStatus = DCM_BUFF_FOR_TP;
          Dcm_ComStatus.Dcm_RxBuffer.pduId = pduId;
		    Dcm_ComStatus.Dcm_RxBuffer.pduInfo.sduLength = pduLen;
	      	*sduInfo = &Dcm_ComStatus.Dcm_RxBuffer.pduInfo;
	    }
	    else
	    {
	        /* main buffer provided fail */
		    ret = BUFREQ_E_OVFL;
	    }
    }
    else
    {
	      /* No buffer available */
		  ret = BUFREQ_E_NOT_OK; 
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <DCM rx indication from CanTp>
 * <Called after an I-PDU has been received via the TP API, the result indicates 
    whether the transmission was successful or not. >
 * <DCM rx indication from CanTp,mean an request is received> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <pduId (IN), result (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
EXPORTED void Dcm_TpRxIndication( PduIdType id, Std_ReturnType result ) 
{
   Dsl_RxIndicationFromPduR(id,result);
}
/******************************************************************************/
/**
 * @brief               <DCM provide tx buffer for CanTp>
 * 
 * <DCM provide rx buffer for CanTp,mean a response is sending> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <pduId (IN)>
 * @param[out]          <sduInfo (OUT)>
 * @param[in/out]       <NONE>
 * @return              <BufReq_RetType>    
 */
/******************************************************************************/
BufReq_RetType Dcm_ProvideTxBuffer(PduIdType pduId,PduInfoType **sduInfo)
{
    BufReq_RetType ret = BUFREQ_OK;  
	
	/* check if Tx Buffer is provide for service process */
	if ((DCM_BUFF_FOR_SERVICE == Dcm_ComStatus.Dcm_TxBuffer.buffStatus) &&
        (pduId == Dcm_ComStatus.Dcm_TxBuffer.pduId))
	{
		/* send MainTXBuff */
		*sduInfo = &Dcm_ComStatus.Dcm_TxBuffer.pduInfo;
		Dcm_ComStatus.Dcm_TxBuffer.buffStatus = DCM_BUFF_FOR_TP;
   }
   else
   {
       Dsl_ServiceFinish();
       ret = BUFREQ_E_NOT_OK;
   }

   return ret;
}

/******************************************************************************/
/**
 * @brief               <DCM tx confirmation from CanTp>
 * 
 * <DCM rx indication from CanTp,mean a response is sended> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <pduId (IN), result (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
void Dcm_TxConfirmation(PduIdType pduId,NotifResultType result)
{
    /* service response finished */
    if((NTFRSLT_OK == result) && 
        (pduId == Dcm_ComStatus.Dcm_TxBuffer.pduId) &&
        (FALSE == Dcm_ComStatus.Dcm_RespFinished))
    {
    	/* 
    	** this case is only appear when send pending message and service is 
    	** not finished 
    	*/
        Dcm_ComStatus.Dcm_TxBuffer.buffStatus = DCM_BUFF_FREE;
    }
    else
    {
        Dsl_ServiceFinish();
    }
 
    return;
}

EXPORTED Std_ReturnType Dcm_GetSesCtrlType(Dcm_SesCtrlType *SesCtrlType)
{
	Std_ReturnType returnCode;

	returnCode = Dsl_GetSesCtrlType(SesCtrlType);

	return returnCode;
}

EXPORTED Std_ReturnType Dcm_GetSecurityLevel(Dcm_SecLevelType *SecLevel)
{
	Std_ReturnType returnCode;

	returnCode = Dsl_GetSecurityLevel(SecLevel);

	return returnCode;
}

/**************************************************************************
Object: Manage dcm periodically (called in TASK(Task_5ms)).
Parameters: None.
Return: None.
**************************************************************************/
EXPORTED void DCM_ManageEvery5ms(void)
{
	Dcm_MainFunction();
}
/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121109    Gary       Initial version
 */
/*=======[E N D   O F   F I L E]==============================================*/


