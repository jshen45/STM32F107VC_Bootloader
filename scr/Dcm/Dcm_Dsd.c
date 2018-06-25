/*============================================================================*/
/* Copyright (C) 2016-2017, Chengdu WeiTe Electronic Fuel CO.,LTD..
 *  
 *  All rights reserved. This software is WIT property. Duplication 
 *  or disclosure without WIT written authorization is prohibited.
 *  
 *  
 *  @file            :<Dcm_Dsd.c>
 *  @brief           :<Dcm for MPC5634> 
 *  @author          :<Mingdong.Yuan>
 *  @date            :<2016-8-11>
 *  @Current revision: $Revision: 1.0
 */
/*============================================================================*/
/*************************************************************************
Packages inclusion
*************************************************************************/
#include "Dcm_Dsd.h"
#include "Dcm_Dsp.h"
#include "Dcm_Dsl.h"
#include "Dcm_Types.h"
#include "Dcm.h"
/**************************************************************************
Private Functions
**************************************************************************/

/******************************************************************************/
/**
 * @brief               <handle requested service>
 * 
 * <handle requested service> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
LOCAL void ServiceHandle(void)
{
    boolean SIDFind = FALSE;
    uint8 tableIndex = DCM_SERVICE_NUM;
    
    /* record current service table */
	 Dcm_ComStatus.Dcm_CurServicePtr = Dcm_Config.DcmConfigSet->DcmDsd->DsdServiceTable->DcmDsdService;
    
    /* find service table */
    while ((tableIndex > 0) && (FALSE == SIDFind))
    {
        tableIndex --;
        if ((Dcm_ComStatus.Dcm_CurServicePtr->SID == Dcm_ComStatus.Dcm_RxBuffer.pduInfo.sduDataPtr[0]) && 
            ((DCM_RX_PHY_PDU_ID == Dcm_ComStatus.Dcm_RxBuffer.pduId) ||
             ((DCM_RX_FUNC_PDU_ID == Dcm_ComStatus.Dcm_RxBuffer.pduId) && 
              (TRUE == Dcm_ComStatus.Dcm_CurServicePtr->funcAddrSupportted))))
        {
            SIDFind = TRUE;
        }
        else
        {
            Dcm_ComStatus.Dcm_CurServicePtr ++;
        }
    }

	/* check if service table is finded */
    if (TRUE == SIDFind)
    {
    	/* check if service is supportted in current session */
        if (TRUE == Dsp_CheckSessionLevel(Dcm_ComStatus.Dcm_CurServicePtr->sessionSupp))
        {
				/* execute service process */
            Dcm_ComStatus.Dcm_CurServicePtr->serviceFct(&Dcm_ComStatus.Dcm_RxBuffer,&Dcm_ComStatus.Dcm_TxBuffer);
        }
        else
        {
            /* service is not supportted in active session */
            Dsl_SendNcr(DCM_E_SERVICE_NOT_SUPPORTED_INACTIVE_SESSION);
        } 
    }
    else
    {
    	/* has not find service table */
        if (DCM_RX_FUNC_PDU_ID == Dcm_ComStatus.Dcm_RxBuffer.pduId)
        {
            /* if received PDU ID is function address ,reset service process */
            Dsl_ServiceFinish();            
        }
        else
        {
            /* if received PDU ID is phycial address ,service is not supportted */
            Dsl_SendNcr(DCM_E_SERVICE_NOT_SUPPORTED);
        }
    }
		
    return;
}

/**************************************************************************
Object:ServiceProcess() is the main process new requested service
Parameters:none
Return:none
**************************************************************************/
LOCAL void ServiceProcess(void)
{
	 /* check if tx buffer is free */
    if (DCM_BUFF_FREE == Dcm_ComStatus.Dcm_TxBuffer.buffStatus)
    {
    	/* check if there if a new request service */
        if (TRUE == Dcm_ComStatus.Dcm_ReqNew)
        {
            Dcm_ComStatus.Dcm_ReqNew = FALSE;
            ServiceHandle();
        }
        else
        {
        	   /* check service process for pending */
            if (FALSE == Dcm_ComStatus.Dcm_RespFinished)
            {
                FL_ResultType errorCode;
                boolean serviceFinished = FL_ServiceFinished(&errorCode);

                /* check if service busy is finished,then can send response. */
                if ((TRUE == serviceFinished) && (Dcm_ComStatus.Dcm_CurServicePtr->pendingFct != NULLPTR)&&((Dcm_ComStatus.Dcm_CurServicePtr->SID == 0x2E)||
					(Dcm_ComStatus.Dcm_CurServicePtr->SID == 0x36)||(Dcm_ComStatus.Dcm_CurServicePtr->SID == 0x31)))
				{
                    Dcm_ComStatus.Dcm_CurServicePtr->pendingFct(errorCode,&Dcm_ComStatus.Dcm_RxBuffer,&Dcm_ComStatus.Dcm_TxBuffer);
                }
            }
        }
    }

    return;
}
/**************************************************************************
Exported Functions
**************************************************************************/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	Implements 'void Dcm_Init(void)' for DSD.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EXPORTED void Dsd_Init(void) 
{
	//Dcm_ComStatus.Dcm_CurServicePtr = ((void *)0x000A0000);
	Dcm_ComStatus.Dcm_CurServicePtr = ((void *)0x00010000);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	Implements 'void Dcm_MainFunction(void)' for DSD.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EXPORTED void Dsd_MainFunction(void) 
{
   /* process P2CanServer, S3Server, security timer */
	ServiceProcess();
}
