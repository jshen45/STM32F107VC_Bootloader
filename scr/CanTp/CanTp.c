/*============================================================================*/
/*** Copyright (C) 2009-2011, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  
 *  @file       <CanTp.c>
 *  @brief      <TP (ISO 15765) source component>
 *  
 *  
 *  @author     <Tommy Yu>
 *  @date       <2012-12-28>
 */
/*============================================================================*/
 

/*=======[I N C L U D E S]====================================================*/
#include "CanTp.h"
#include "CanTp_Cfg.h"
#include "Dcm.h"
#include "CanIf.h"
#include <string.h>

/*=======[M A C R O S]========================================================*/

#define ISO15765_FLOW_CONTROL_STATUS_CTS        (0u)
#define ISO15765_FLOW_CONTROL_STATUS_WAIT       (1u)
#define ISO15765_FLOW_CONTROL_STATUS_OVFLW      (2u)

/** PCI mask */
#define ISO15765_TPCI_MASK      (0xF0u) 

/** Single Frame */			
#define ISO15765_TPCI_SF        (0x00u)  

/** First Frame */			
#define ISO15765_TPCI_FF        (0x10u)

/** Consecutive Frame */			
#define ISO15765_TPCI_CF        (0x20u)

/** Flow Control */		
#define ISO15765_TPCI_FC        (0x30u)

/** Single frame data length mask */			
#define ISO15765_TPCI_DL        (0x0Fu)

/** Flowcontrol status mask */			
#define ISO15765_TPCI_FS_MASK   (0x0Fu)  			


/** max data length of SF with standard address */
#define STD_ADDR_MAX_SF_LEGNTH	(7u)

/** max data length of SF with extended address */				
#define EXT_ADDR_MAX_SF_LEGNTH	(6u)

#define STD_ADDR_MAX_FF_LENGTH	(6u)

/** max data length of CF with standard address */		
#define STD_ADDR_MAX_CF_LENGTH	(7u)					

/** data offset of standard address */
#define STD_ADDR_SF_DATA_OFFSET	(1u)

/** data offset of extended address */				
#define EXT_ADDR_SF_DATA_OFFSET	(2u)

/** data offset of standard address */				
#define STD_ADDR_FF_DATA_OFFSET	(2u)

#define STD_ADDR_CF_DATA_OFFSET (1u)
#define STD_ADDR_FC_BS_OFFSET   (1u)
#define STD_ADDR_FC_STMIN_OFFSET (2u)

#define SEGMENT_NUMBER_MASK     (0x0fu)
#define CANDRV_PDU_MAX_LENGTH   (8u)

#define CANTP_TOTAL_CHANNEL_NUM	(uint8)(CANTP_TX_CHANNEL_NUM + CANTP_RX_CHANNEL_NUM)

#define INDEX_NOT_FIND  ((uint8)(0xFFu))

EXPORTED uint8 Cantp_test;
/**=======[T Y P E   D E F I N I T I O N S]====================================*/

typedef enum 
{
	/* Not specified by ISO15765 - used as error return type when decoding frame. */
	INVALID_FRAME, 										
	SINGLE_FRAME, 
	FIRST_FRAME, 
	CONSECUTIVE_FRAME, 
	/* FC with CTS:Continus to send */
	FLOW_CONTROL_CTS_FRAME 							
}CanTp_Iso15765_FrameType;

typedef struct 
{
	uint8 data[8u];
	uint8 byteCount;
}CanTp_PduType;



typedef struct 
{
	/* Counter keeping track total frames handled. */
	uint8 framesHandledCount; 	
	
	/* In case we are transmitters the remote node can configure this value (only valid for TX). */
	uint8 stmin;	
	
	/* Blocksize (only valid for TX).if FC with BS=0, the runtime BS will be 0xFFFF */
	uint16 bs;
	
	/* timeout counter */
	//uint8 timeoutCounterValue;
	uint16 timeoutCounterValue;
	/* Transfer state machine.*/
	CanTp_StateType tpState;
	
	/* The PDUR make an instance of this.*/
	PduInfoType *pdurBuffer;
	
	/* Number of bytes in PDUR buffer,which have been writed to dsm buffer */
	uint16 pdurBufferCount;	
	
	/* Total length of the PDU.*/
	uint16 transferTotal;		
	
	/* Counter ongoing transfer.*/
	uint16 transferCount;	
	
	/* Temp storage of SDU data.*/
	CanTp_PduType canFrameBuffer;		    
}CanTp_RunTime_DataType;

/**=======[I N T E R N A L   D A T A]==========================================*/

STATIC CanTp_RunTime_DataType CanTp_RuntimeData[CANTP_TOTAL_CHANNEL_NUM];

/**=======[I N T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/

STATIC uint16 CanTp_ConvertMsToMainCycles(const uint16 CanTp_time);
#ifdef PADDING_CHECK
STATIC boolean CanTp_FramePaddingCheck(const PduInfoType *const CanTp_objRxPtr, 
                                       const uint8 CanTp_checkDataOffset);
#endif

STATIC void CanTp_FrameAddPadding(PduInfoType *const CanTp_pduInfo);

STATIC void CanTp_ReceiveSingleFrame(CanTp_RunTime_DataType *CanTp_rxRuntime, 
                                     const CanTp_RxNSduType *CanTp_rxConfig,
							   	     const PduInfoType *const CanTp_objRxPtr, 
							   	     const uint8 CanTp_length);
							   	     
STATIC void CanTp_ReceiveFirstFrame (CanTp_RunTime_DataType *CanTp_rxRuntime, 
                                     const CanTp_RxNSduType *CanTp_rxConfig,
							         const PduInfoType *const CanTp_objRxPtr, 
							         const PduLengthType pduLength);
							         
STATIC void CanTp_ReceiveConsecutiveFrame(CanTp_RunTime_DataType *CanTp_rxRuntime, 
                                          const CanTp_RxNSduType *CanTp_rxConfig,
									      const PduInfoType *CanTp_objRxPtr);

STATIC void CanTp_ReceiveFlowControlFrame(CanTp_RunTime_DataType *CanTp_txRuntime, 
									      const CanTp_TxNSduType *CanTp_txConfig,
									      const PduInfoType *const CanTp_objRxPtr, 
									      const CanTp_Iso15765_FrameType frameType);

STATIC void CanTp_Init15765RuntimeData(CanTp_RunTime_DataType *CanTp_runtime);

STATIC BufReq_RetType CanTp_CopySegmentToRxBuffer(const CanTp_RxNSduType *CanTp_rxConfig,
													    CanTp_RunTime_DataType *CanTp_rxRuntime);													    
STATIC uint8 CanTp_FindTpRxIndicationIndexFromRxChannel(const PduIdType CanTp_pduId,
												        const PduInfoType *const CanTp_objRxPtr, 
												        uint8 *const CanTp_tpci, 
												        uint8 *const CanTp_tpciOffset);
												        
STATIC uint8 CanTp_FindTpRxIndexFromTxChannel(const PduIdType CanTp_pduId);

STATIC uint8 CanTp_FindTpListIndexFromRxPdu(const PduIdType CanTp_pduId,
                                            const PduInfoType *const CanTp_objRxPtr,
									        CanTp_Iso15765_FrameType *CanTp_format,
									        PduLengthType *pduLength); 
									        
STATIC uint8 CanTp_FindTpTxConfirmIndexFromTxChannel(PduIdType CanTp_txPduId);

STATIC uint8 CanTp_FindTpTxConfirmIndexFromRxChannel(PduIdType CanTp_txPduId);

STATIC uint8 CanTp_FindTpTxListFromSduId(const PduIdType CanTp_SduId);

STATIC CanTp_Iso15765_FrameType CanTp_calcTxFrameType(const CanTp_RunTime_DataType * const CanTp_txRuntime);
STATIC void CanTp_SendFlowControlFrame(const CanTp_RxNSduType *CanTp_rxConfig, 
                                       CanTp_RunTime_DataType *CanTp_rxRuntime, 
								       BufReq_RetType CanTp_flowStatus);
								       
STATIC boolean CanTp_CopyDataToMsgFromTxBuf(const CanTp_TxNSduType *CanTp_txConfig,
									        CanTp_RunTime_DataType *CanTp_txRuntime,
									        PduInfoType *const CanTp_pduInfo,
									        const uint8 t_u1_actualPayload);
									        								 
STATIC void CanTp_SendSingleFrame(const CanTp_TxNSduType *CanTp_txConfig, 
					    	      CanTp_RunTime_DataType *CanTp_txRuntime,
					    	      PduInfoType *const CanTp_pduInfo);
					    	      
STATIC void CanTp_SendFirstFrame(const CanTp_TxNSduType *CanTp_txConfig, 
					    	     CanTp_RunTime_DataType *CanTp_txRuntime,
					             PduInfoType *const CanTp_pduInfo);
					             
STATIC void CanTp_SendConsecutiveFrame(const CanTp_TxNSduType *CanTp_txConfig, 
				         		       CanTp_RunTime_DataType *CanTp_txRuntime,
								       PduInfoType *const CanTp_pduInfo);
								       					    	 					    	 
STATIC void CanTp_SendTxFrame(const CanTp_TxNSduType *CanTp_txConfig, 
					          CanTp_RunTime_DataType *const CanTp_txRuntime,
					          const CanTp_Iso15765_FrameType CanTp_framType);

STATIC void CanTp_WaitSForLastCFBufProcess(const CanTp_RxNSduType *CanTp_rxConfig,
								           CanTp_RunTime_DataType *CanTp_runtimeItem);

STATIC void CanTp_WaitCFBufProcess(const CanTp_RxNSduType *CanTp_rxConfig,
						           CanTp_RunTime_DataType *CanTp_runtimeItem);						           


/**=======[F U N C T I O N   I M P L E M E N T A T I O N S]====================*/

/********************************************************************************/
/****
 * @brief			:CanTp_Init  
 * 
 * Service ID		: <CanTp_Init>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
EXPORTED void CanTp_Init(const CanTp_ConfigType *CfgPtr) 
{
	uint8 u8NsduIndex;
	
	for(u8NsduIndex = 0;u8NsduIndex < CANTP_TOTAL_CHANNEL_NUM;u8NsduIndex ++)
	{
      if (( CfgPtr->CanTpNSduList[u8NsduIndex].CanTpDirection == ISO15765_TRANSMIT )
	     ||( CfgPtr->CanTpNSduList[u8NsduIndex].CanTpDirection == ISO15765_RECEIVE ))
      {
			/* initial runtime item */
			CanTp_Init15765RuntimeData(&CanTp_RuntimeData[u8NsduIndex]);
      }
	} 
}

/********************************************************************************/
/****
 * @brief			:CanTp_RxIndication  
 * 
 * Service ID		: <CanTp_RxIndication>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
EXPORTED void CanTp_RxIndication(PduIdType RxPduId, const PduInfoType *PduInfoPtr)
{	
   /* SF,FF,CF,FC,*/
	CanTp_Iso15765_FrameType frameType;
	PduLengthType pduLength ;
	uint8 CanTp_listIndex;
	const CanTp_RxNSduType *CanTp_rxConfig;
	const CanTp_TxNSduType *CanTp_txConfig;
	
	/* find */
	CanTp_listIndex = CanTp_FindTpListIndexFromRxPdu(RxPduId,PduInfoPtr,&frameType,&pduLength);
	
	if (CanTpConfig.CanTpNSduList[CanTp_listIndex].CanTpDirection == ISO15765_TRANSMIT)
	{
		CanTp_txConfig = (CanTp_TxNSduType*)&CanTpConfig.CanTpNSduList[CanTp_listIndex].ConfigData;
	}
	else
	{
		CanTp_rxConfig = (CanTp_RxNSduType*)&CanTpConfig.CanTpNSduList[CanTp_listIndex].ConfigData;
	}
	
	if (INDEX_NOT_FIND != CanTp_listIndex)
	{
		switch (frameType) 
		{
			case INVALID_FRAME:
				break;

			case SINGLE_FRAME:
				/* deal with  single frame */
				CanTp_ReceiveSingleFrame(&CanTp_RuntimeData[CanTp_listIndex],CanTp_rxConfig,PduInfoPtr,(uint8)pduLength);		
				break;		

			case FIRST_FRAME: 
				/* physics address FF */
				CanTp_ReceiveFirstFrame(&CanTp_RuntimeData[CanTp_listIndex],CanTp_rxConfig,PduInfoPtr,pduLength);
				break;

			case CONSECUTIVE_FRAME:	
				/** deal with consecutive frame */
				CanTp_ReceiveConsecutiveFrame(&CanTp_RuntimeData[CanTp_listIndex],CanTp_rxConfig,PduInfoPtr);
				break;
				
			case FLOW_CONTROL_CTS_FRAME:
				/* RX,FC; tx channel  */	
				/* deal with a flow control frame */
				CanTp_ReceiveFlowControlFrame(&CanTp_RuntimeData[CanTp_listIndex],CanTp_txConfig,PduInfoPtr, frameType);
				break;

			default:
				break;
		}
	}
	return;
}

/********************************************************************************/
/****
 * @brief			:CanTp_TxConfirmation  
 * 
 * Service ID		: <CanTp_TxConfirmation>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
EXPORTED void CanTp_TxConfirmation(PduIdType TxPduId)
{
	const CanTp_TxNSduType *CanTp_txConfig;
	const CanTp_RxNSduType *CanTp_rxConfig;

	/* find tp index from tx channel */
	uint8 CanTp_listIndex = CanTp_FindTpTxConfirmIndexFromTxChannel(TxPduId);

	if (INDEX_NOT_FIND != CanTp_listIndex)
	{	
		/* tx:SF,CF or FF confirmation ,tx channel */
		CanTp_RunTime_DataType *CanTp_runtimeItem;
		
		CanTp_runtimeItem = &CanTp_RuntimeData[CanTp_listIndex];
		CanTp_txConfig = (CanTp_TxNSduType*)&CanTpConfig.CanTpNSduList[CanTp_listIndex].ConfigData;
		
		switch (CanTp_runtimeItem->tpState)
		{
			case WAITING_FOR_SF_TX_CONFIRMATION:
			case WAITING_FOR_LAST_CF_TX_CONFIRMATION:
				/* transmit ok */
				Dcm_TxConfirmation(CanTp_txConfig->txDcmId, NTFRSLT_OK); 
				CanTp_Init15765RuntimeData(CanTp_runtimeItem);
				Cantp_test = 1;
				break;
				
			case WAITING_FOR_FF_TX_CONFIRMATION:
			case WAITING_FOR_CF_BLOCK_TX_CONFIRMATION:
				/* wait flow control frame */
				CanTp_runtimeItem->tpState = WAITING_FOR_FC_RX;
				CanTp_runtimeItem->timeoutCounterValue	= CanTp_ConvertMsToMainCycles(ISO15765_N_Bs);
				break;
				
			case WAITING_FOR_CF_TX_CONFIRMATION:
				CanTp_runtimeItem->tpState = WAITING_FOR_CF_TX;
				break;	
									
			default:
				/* wrong state */
				Dcm_TxConfirmation(CanTp_txConfig->txDcmId, NTFRSLT_E_NOT_OK); 
				CanTp_Init15765RuntimeData (CanTp_runtimeItem);
				Cantp_test = 2;
				break;
		}
	}
	else
	{ 	
		/* TX : FC,rx channel  */
		/* find tp index from rx channel ,the FC confirmation */
		CanTp_listIndex = CanTp_FindTpTxConfirmIndexFromRxChannel(TxPduId);
		if(INDEX_NOT_FIND != CanTp_listIndex)
		{ 	
		   /* FC tx confirmation */
			CanTp_RunTime_DataType *CanTp_rxRuntime = &CanTp_RuntimeData[CanTp_listIndex];
			CanTp_rxConfig = (CanTp_RxNSduType*)&CanTpConfig.CanTpNSduList[CanTp_listIndex].ConfigData;
			
			//CanTp_rxRuntime->tpState = WAITING_FOR_FC_CTS_TX_CONFIRMATION;
			if (WAITING_FOR_FC_CTS_TX_CONFIRMATION == CanTp_rxRuntime->tpState)
			{	
			   /* FC confirmation */
				CanTp_rxRuntime->tpState = WAITING_FOR_CF_RX;
				CanTp_rxRuntime->timeoutCounterValue = CanTp_ConvertMsToMainCycles(CanTp_rxConfig->CanTpNcr);//2018-5-11
			}
			else
			{ 	
			   /* wrong state */
				//Dcm_TpRxIndication(CanTp_rxConfig->rxDcmId, NTFRSLT_E_NOT_OK);//2018-5-11
				//CanTp_Init15765RuntimeData(CanTp_rxRuntime);//2018-5-11
			}
		}
	}
	
	return;
}

/********************************************************************************/
/****
 * @brief			:CanTp_Transmit  
 * 
 * Service ID		: <CanTp_Transmit>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
EXPORTED Std_ReturnType CanTp_Transmit(const PduIdType CanTpTxSduId, PduInfoType *CanTpTxInfoPtr) 
{
	CanTp_RunTime_DataType *CanTp_txRuntime;
	/* find tp list index from SduId which should be send */
	uint8 CanTp_listIndex = CanTp_FindTpTxListFromSduId(CanTpTxSduId);

	if (INDEX_NOT_FIND != CanTp_listIndex)
	{
		CanTp_txRuntime = &CanTp_RuntimeData[CanTp_listIndex];
		if (TP_STATE_IDLE == CanTp_txRuntime->tpState)
		{	
		   /* the TP state is IDLE state,and then change the trans state to prepare sent */
			CanTp_txRuntime->transferTotal = CanTpTxInfoPtr->sduLength;

			CanTp_txRuntime->pdurBuffer = CanTpTxInfoPtr;
			/* change state to wait tx buffer */
			CanTp_txRuntime->tpState = WAITING_TX_BUFFER;
		}
	}
	return E_OK;
}

/********************************************************************************/
/****
 * @brief			:CanTp_MainFunc  
 * 
 * Service ID		: <CanTp_MainFunc>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
EXPORTED void CanTp_MainFunction(void)
{
	uint8 u8MainLoop;
	
	const CanTp_TxNSduType *pstTxConfigListItem  = NULLPTR;
	const CanTp_RxNSduType *pstRxConfigListItem  = NULLPTR;

	CanTp_RunTime_DataType *pstTxRuntimeListItem = NULLPTR;
	CanTp_RunTime_DataType *pstRxRuntimeListItem = NULLPTR;
	
	/* for calc frame type ,SF or FF */
	CanTp_Iso15765_FrameType frameType ;	

	for (u8MainLoop = 0u; u8MainLoop < CANTP_TOTAL_CHANNEL_NUM; u8MainLoop ++)
	{	
		if (CanTpConfig.CanTpNSduList[u8MainLoop].CanTpDirection == ISO15765_TRANSMIT) 
		{
			pstTxConfigListItem = (CanTp_TxNSduType*)&CanTpConfig.CanTpNSduList[u8MainLoop].ConfigData;
			pstTxRuntimeListItem = &CanTp_RuntimeData[pstTxConfigListItem->CanTpTxChannel];

			if(pstTxRuntimeListItem->tpState != TP_STATE_IDLE)
			{
				if(pstTxRuntimeListItem->tpState == WAITING_TX_BUFFER)
				{
					/* calc the frame type which should be sent */
					frameType = CanTp_calcTxFrameType(pstTxRuntimeListItem);
					/* send fram SF or FF */
					CanTp_SendTxFrame(pstTxConfigListItem,pstTxRuntimeListItem,frameType);					
				}
				else
				{
					/* wait tx confirmation */
					/* Decrement timeout counter, if not already zero. */
					if (pstTxRuntimeListItem->timeoutCounterValue > 0u)
					{
						pstTxRuntimeListItem->timeoutCounterValue--;
					}

					if (pstTxRuntimeListItem->timeoutCounterValue == 0u)
					{	
					    /* time out */
						switch (pstTxRuntimeListItem->tpState)
						{
							/* tx channel */
							/* signal frame tx confirmation */	
							case WAITING_FOR_SF_TX_CONFIRMATION:	
							/* first frame tx confirmation */
							case WAITING_FOR_FF_TX_CONFIRMATION:		
							/* last CF of this message */
							case WAITING_FOR_LAST_CF_TX_CONFIRMATION:	
							/* wait for CF tx confirmation */	
					//case WAITING_FOR_CF_TX_CONFIRMATION:	
							/* CF tx confirmation ,if confirmation then wait FC frame */
							case WAITING_FOR_CF_BLOCK_TX_CONFIRMATION:	
							/* wait for flow control fram */
							case WAITING_FOR_FC_RX:	
								/* transmit data timeout */
								Dcm_TxConfirmation(pstTxConfigListItem->txDcmId,NTFRSLT_E_NOT_OK);                                             
								CanTp_Init15765RuntimeData(pstTxRuntimeListItem);
								Cantp_test =4;
								break;	

							case WAITING_FOR_CF_TX:
								/* send CF */
								CanTp_SendTxFrame(pstTxConfigListItem, pstTxRuntimeListItem,CONSECUTIVE_FRAME);
								break;
							default:
								break;
						}
					}
				}
			}
		}
		else
		{
			pstRxConfigListItem = (CanTp_RxNSduType*)&CanTpConfig.CanTpNSduList[u8MainLoop].ConfigData;
			pstRxRuntimeListItem = &CanTp_RuntimeData[pstRxConfigListItem->CanTpRxChannel];

			if(pstRxRuntimeListItem->tpState != TP_STATE_IDLE)
			{
				if( ( pstRxRuntimeListItem->tpState == WAITING_RX_CF_BUFFER )
				  ||(pstRxRuntimeListItem->tpState == WAITING_RX_SF_BUFFER))
				{
					switch(pstRxRuntimeListItem->tpState)
					{
						case WAITING_RX_CF_BUFFER:
							if( u8MainLoop < CANTP_RX_CHANNEL_NUM )
							{	
							    /* wait CF buffer */
								 CanTp_WaitCFBufProcess(pstRxConfigListItem, pstRxRuntimeListItem);
							}
							break;
							
						case WAITING_RX_SF_BUFFER:
							if (u8MainLoop < CANTP_RX_CHANNEL_NUM)
							{
							   /* wait single buffer of last CF buffer */
								CanTp_WaitSForLastCFBufProcess(pstRxConfigListItem, pstRxRuntimeListItem);
							}  
							break;
							
						default:
							break;
					}
				}			
				else
				{
					/* wait  rx indication  */
					/* Decrement timeout counter, if not already zero. */
					if (pstRxRuntimeListItem->timeoutCounterValue > 0u)
					{
						pstRxRuntimeListItem->timeoutCounterValue--;
					}

					if (pstRxRuntimeListItem->timeoutCounterValue == 0u)
					{	
						/* time out */
						switch (pstRxRuntimeListItem->tpState)
						{
							/* rx channel */
							case WAITING_FOR_FC_CTS_TX_CONFIRMATION:
							case WAITING_FOR_FC_OVFLW_TX_CONFIRMATION:
							case WAITING_FOR_CF_RX:
								/* receive data time */
								if (u8MainLoop < CANTP_RX_CHANNEL_NUM)
								{
									Dcm_TpRxIndication(pstRxConfigListItem->rxDcmId, NTFRSLT_E_NOT_OK);
								}
								CanTp_Init15765RuntimeData(pstRxRuntimeListItem);
								Cantp_test =6;
								break;
								
							default:
								break;
						}
					}
			  	}
		 	}
		}
	}
}

/********************************************************************************/
/****
 * @brief			:CanTp_ConvertMsToMainCycles  
 * 
 * Service ID		: <CanTp_ConvertMsToMainCycles>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC uint16 CanTp_ConvertMsToMainCycles(const uint16 CanTp_time)
{
	return (uint16)(CanTp_time / CANTP_MAIN_TICK);
}

/********************************************************************************/
/****
 * @brief			:CanTp_FramePaddingCheck  
 * 
 * Service ID		: <CanTp_FramePaddingCheck>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/

#ifdef PADDING_CHECK
STATIC boolean CanTp_FramePaddingCheck(const PDU_INFO *const CanTp_objRxPtr, 
								          const uint8 CanTp_checkDataOffset)
{
	uint8 loop;
	boolean checkOk = TRUE;
	
	if (CANDRV_PDU_MAX_LENGTH  == CanTp_objRxPtr->sduLength)
	{
        for (loop = CanTp_checkDataOffset; (loop < CANDRV_PDU_MAX_LENGTH) && (checkOk); loop++)
        {
            if (CANTP_PADDING_VALUE  != CanTp_objRxPtr->sduDataPtr[loop])
            {	
                /* check error */
                checkOk = FALSE;
            }
            else
            {
                /*do nothing*/
            }
        }
	}
	else
	{
		checkOk = FALSE;
	}
	return checkOk;	
}
#endif




/********************************************************************************/
/****
 * @brief			:CanTp_FrameAddPadding  
 * 
 * Service ID		: <CanTp_FrameAddPadding>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC void CanTp_FrameAddPadding(PduInfoType *const CanTp_pduInfo)
{
	#ifdef PADDING_ON
	for (; CanTp_pduInfo->sduLength < CANDRV_PDU_MAX_LENGTH; CanTp_pduInfo->sduLength++)
	{	/* add padding */
		CanTp_pduInfo->sduDataPtr[CanTp_pduInfo->sduLength] = CANTP_PADDING_VALUE;
	}
	#endif
}

/********************************************************************************/
/****
 * @brief			:CanTp_ReceiveSingleFrame  
 * 
 * Service ID		: <CanTp_ReceiveSingleFrame>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC void CanTp_ReceiveSingleFrame (CanTp_RunTime_DataType *CanTp_rxRuntime, 
                                      const CanTp_RxNSduType *CanTp_rxConfig,
							          const PduInfoType *const CanTp_objRxPtr, 
							          const uint8 CanTp_length)
{
	uint8 loop;
	uint8 dataOffset;
	#ifdef PADDING_CHECK
	boolean	paddingCheckOk = FALSE;
	#endif

	if (CANTP_STANDARD  == CanTp_rxConfig->CanTpAddressingFormant)
	{	
	   /* standard address frame */
		dataOffset = STD_ADDR_SF_DATA_OFFSET;
	}
	else
	{	
	   /* extended address frame */
		dataOffset = EXT_ADDR_SF_DATA_OFFSET;
	}	
	
	#ifdef PADDING_CHECK
	/* padding check */
	paddingCheckOk = CanTp_FramePaddingCheck(CanTp_objRxPtr, (CanTp_length+dataOffset));
	if (TRUE == paddingCheckOk)
	#endif
	{
		if (TP_STATE_IDLE  != CanTp_rxRuntime->tpState)
		{	
		   /* not idle state,reset state */
			Dcm_TpRxIndication(CanTp_rxConfig->rxDcmId, NTFRSLT_E_NOT_OK);
			CanTp_Init15765RuntimeData(CanTp_rxRuntime); 
			Cantp_test =7;
		} 

		for (loop = 0u; loop < CanTp_length; loop++)
		{	
		   /* copy data to local buffer */
			CanTp_rxRuntime->canFrameBuffer.data[loop] = CanTp_objRxPtr->sduDataPtr[dataOffset + loop];
		}
		CanTp_rxRuntime->canFrameBuffer.byteCount = CanTp_length;
		CanTp_rxRuntime->transferTotal = (uint16)CanTp_length;
		/* change state */
		CanTp_rxRuntime->tpState = WAITING_RX_SF_BUFFER;
	}
	
	return;
}




/********************************************************************************/
/****
 * @brief			:CanTp_ReceiveFirstFrame  
 * 
 * Service ID		: <CanTp_ReceiveFirstFrame>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC void CanTp_ReceiveFirstFrame(CanTp_RunTime_DataType *CanTp_rxRuntime, 
                                    const CanTp_RxNSduType *CanTp_rxConfig,
							        const PduInfoType *const CanTp_objRxPtr, 
							        const PduLengthType pduLength)
{
	uint8 loop;
	
	if(CANDRV_PDU_MAX_LENGTH  == CanTp_objRxPtr->sduLength) 
	{	
#if 1
		/* first frame length must be 8 */
		if (TP_STATE_IDLE  != CanTp_rxRuntime->tpState)
		{	
			/* check if is a correct state  */
			Dcm_TpRxIndication(CanTp_rxConfig->rxDcmId, NTFRSLT_E_NOT_OK);
			CanTp_Init15765RuntimeData(CanTp_rxRuntime); 
			Cantp_test =8;
		}
#endif		
		for (loop = 0u; loop < STD_ADDR_MAX_FF_LENGTH; loop++ )
		{	
			/* copy data to local buffer */
			CanTp_rxRuntime->canFrameBuffer.data[loop] = CanTp_objRxPtr->sduDataPtr[STD_ADDR_FF_DATA_OFFSET + loop];
		}	
		
		CanTp_rxRuntime->canFrameBuffer.byteCount = STD_ADDR_MAX_FF_LENGTH;
		CanTp_rxRuntime->transferTotal = pduLength;

		CanTp_rxRuntime->tpState = WAITING_RX_CF_BUFFER;
		CanTp_rxRuntime->framesHandledCount++;  
	}
	
	return;
}

/********************************************************************************/
/****
 * @brief			:CanTp_ReceiveConsecutiveFrame  
 * 
 * Service ID		: <CanTp_ReceiveConsecutiveFrame>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC void CanTp_ReceiveConsecutiveFrame(CanTp_RunTime_DataType *CanTp_rxRuntime, 
         											const CanTp_RxNSduType *CanTp_rxConfig,
														const PduInfoType * CanTp_objRxPtr)
{
	uint8 loop;	
	boolean lastCF = FALSE;
	uint8 dataCount;
	uint16 bytesLeftToCopy = CanTp_rxRuntime->transferTotal - CanTp_rxRuntime->transferCount;	
	uint8 CanTp_sn = CanTp_objRxPtr->sduDataPtr[0u] & SEGMENT_NUMBER_MASK; 
	#ifdef PADDING_CHECK
	boolean paddingCheckOk = FALSE;
	#endif

	if (WAITING_FOR_CF_RX != CanTp_rxRuntime->tpState)
	{	
		/* error state rx cf frame */
		Dcm_TpRxIndication(CanTp_rxConfig->rxDcmId, NTFRSLT_E_NOT_OK); 
		CanTp_Init15765RuntimeData(CanTp_rxRuntime);
		Cantp_test = 9;
	}
	else
	{
		if (CanTp_sn != CanTp_rxRuntime->framesHandledCount) 
		{
			/*	"unit too fast? Increase STmin (cofig) to slow it down!\n");*/
			Dcm_TpRxIndication(CanTp_rxConfig->rxDcmId, NTFRSLT_E_WRONG_SN);
			CanTp_Init15765RuntimeData(CanTp_rxRuntime);
			Cantp_test = 10;
		}
		else
		{
			CanTp_rxRuntime->framesHandledCount ++;
			/* increase for sn */
			CanTp_rxRuntime->framesHandledCount &= SEGMENT_NUMBER_MASK; 

			if (bytesLeftToCopy <= (uint16)STD_ADDR_MAX_CF_LENGTH) 
			{ 
				/* last cf of this message */
				dataCount = (uint8)bytesLeftToCopy; /** 1-7 */
				lastCF = TRUE;
			} 
			else 
			{
				/* depends on addressing CanTp_format used.*/
				dataCount 	= STD_ADDR_MAX_CF_LENGTH;
			}

			#ifdef PADDING_CHECK
			/* padding check */
			paddingCheckOk = CanTp_FramePaddingCheck(CanTp_objRxPtr, (dataCount + STD_ADDR_SF_DATA_OFFSET));
			if (TRUE == paddingCheckOk )
			#endif
			{ 	/* copy data */
				for ( loop = 0; loop < dataCount; loop++ )
				{	
					/* copy to buffer */
					CanTp_rxRuntime->pdurBuffer->sduDataPtr[CanTp_rxRuntime->pdurBufferCount] 
						= CanTp_objRxPtr->sduDataPtr[loop + STD_ADDR_CF_DATA_OFFSET];
					
					CanTp_rxRuntime->pdurBufferCount++;
				}
                /* calc transfer counter */
				CanTp_rxRuntime->transferCount += (uint16)dataCount;

				if (TRUE == lastCF)
				{	
					Dcm_TpRxIndication(CanTp_rxConfig->rxDcmId, NTFRSLT_OK);
					CanTp_Init15765RuntimeData(CanTp_rxRuntime);	
					Cantp_test = 11;
				}
				else
				{	
					CanTp_rxRuntime->tpState = WAITING_FOR_CF_RX;
					/* reset time counter */
					CanTp_rxRuntime->timeoutCounterValue = CanTp_ConvertMsToMainCycles(CanTp_rxConfig->CanTpNcr);
				}
			}
			#ifdef PADDING_CHECK
			else
			{	
				/* padding check not ok */
				Dcm_TpRxIndication(CanTp_rxConfig->rxDcmId, NTFRSLT_E_NOT_OK);
				CanTp_Init15765RuntimeData(CanTp_rxRuntime);
				Cantp_test = 12;
			}
			#endif
		} 
	}
	return;
}



/********************************************************************************/
/****
 * @brief			:CanTp_ReceiveConsecutiveFrame  
 * 
 * Service ID		: <CanTp_ReceiveConsecutiveFrame>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC void CanTp_ReceiveFlowControlFrame(CanTp_RunTime_DataType *CanTp_txRuntime, 
                                          const CanTp_TxNSduType *CanTp_txConfig,
								          const PduInfoType *const CanTp_objRxPtr, 
								          const CanTp_Iso15765_FrameType frameType)
{
	#ifdef PADDING_CHECK
	boolean	paddingCheckOk = FALSE;
	#endif
	if (WAITING_FOR_FC_RX  == CanTp_txRuntime->tpState)
	{ 	
	    /* state ok */
		switch (frameType)
		{
		case FLOW_CONTROL_CTS_FRAME:
			#ifdef PADDING_CHECK
			/* padding check */
			paddingCheckOk = CanTp_FramePaddingCheck(CanTp_objRxPtr, (STD_ADDR_FC_STMIN_OFFSET + 1));
			if (TRUE == paddingCheckOk)
			#endif
			{
				CanTp_txRuntime->bs = (uint16)CanTp_objRxPtr->sduDataPtr[STD_ADDR_FC_BS_OFFSET];
				if (0u == CanTp_txRuntime->bs)
				{
					CanTp_txRuntime->bs = 0xFFFFu;
				}
				
				if (((CanTp_objRxPtr->sduDataPtr[STD_ADDR_FC_STMIN_OFFSET] > 0x7Fu) 
				   &&(CanTp_objRxPtr->sduDataPtr[STD_ADDR_FC_STMIN_OFFSET] < 0xF0u))
				   ||(CanTp_objRxPtr->sduDataPtr[STD_ADDR_FC_STMIN_OFFSET] > 0xF9u))
				{ 	
				    /* if STmin is reserved value */
					Dcm_TxConfirmation(CanTp_txConfig->txDcmId, NTFRSLT_E_NOT_OK); 
					CanTp_Init15765RuntimeData(CanTp_txRuntime);
					Cantp_test = 13;
				}
				else
				{	
				    /* get STmin value */
					if(CanTp_objRxPtr->sduDataPtr[STD_ADDR_FC_STMIN_OFFSET] > 0x7Fu)
					{
						CanTp_txRuntime->stmin = 0u;
					} 
					else
					{
						CanTp_txRuntime->stmin = (uint8)CanTp_ConvertMsToMainCycles(CanTp_objRxPtr->sduDataPtr[STD_ADDR_FC_STMIN_OFFSET]);
					}
					CanTp_txRuntime->tpState = WAITING_FOR_CF_TX;
					/* clear time , send data immediately */
					CanTp_txRuntime->timeoutCounterValue = 0u;	
				}
			}
			#ifdef PADDING_CHECK
			else
			{	/* padding check not ok */
				Dcm_TxConfirmation(CanTp_txConfig->txDcmId, NTFRSLT_E_NOT_OK); 
				CanTp_Init15765RuntimeData(CanTp_txRuntime);
				Cantp_test = 14;
			}
			#endif
			break;
				
		default:
			Dcm_TxConfirmation(CanTp_txConfig->txDcmId, NTFRSLT_E_NOT_OK); 
			CanTp_Init15765RuntimeData(CanTp_txRuntime);
			Cantp_test = 15;
			break;
		}
	}
	else
	{
		/* Dcm_TxConfirmation ( CanTp_txConfig->txDcmId, NTFRSLT_E_NOT_OK ); 
		   CanTp_Init15765RuntimeData ( CanTp_txRuntime );*/	
	}
	return;
}



/********************************************************************************/
/****
 * @brief			:CanTp_Init15765RuntimeData  
 * 
 * Service ID		: <CanTp_Init15765RuntimeData>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC void CanTp_Init15765RuntimeData(CanTp_RunTime_DataType *CanTp_runtime) 
{
	CanTp_runtime->framesHandledCount = 0u;
	CanTp_runtime->stmin = 0u;
	CanTp_runtime->bs = 0u;
	CanTp_runtime->timeoutCounterValue = 0u;
	CanTp_runtime->tpState = TP_STATE_IDLE;
	CanTp_runtime->pdurBuffer = NULLPTR;
	CanTp_runtime->pdurBufferCount 	= 0u;
	CanTp_runtime->transferTotal = 0u;
	CanTp_runtime->transferCount = 0u;
	CanTp_runtime->canFrameBuffer.byteCount = 0u;	

	return;	
}



/********************************************************************************/
/***
 * @brief			:CanTp_CopySegmentToRxBuffer  
 * 
 * Service ID		: <CanTp_CopySegmentToRxBuffer>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC  BufReq_RetType CanTp_CopySegmentToRxBuffer(const CanTp_RxNSduType *CanTp_rxConfig,
													     CanTp_RunTime_DataType *CanTp_rxRuntime) 
{
	BufReq_RetType Can_ret = BUFREQ_E_NOT_OK;
	uint8 loop;

	if (NULLPTR == CanTp_rxRuntime->pdurBuffer)
	{	
	   /* buffer should be enough for message */
		Can_ret = Dcm_ProvideRxBuffer(CanTp_rxConfig->rxDcmId, CanTp_rxRuntime->transferTotal, &CanTp_rxRuntime->pdurBuffer); 
	}
	else
   {
      /* do nothing */
   }

	if ((NULLPTR != CanTp_rxRuntime->pdurBuffer) && (CanTp_rxRuntime->pdurBuffer->sduLength > CanTp_rxRuntime->pdurBufferCount))
	{
		Can_ret = BUFREQ_OK;
		for (loop = 0u; loop < CanTp_rxRuntime->canFrameBuffer.byteCount;loop ++)
		{	
			/* copy local buffer value to provided buffer from DCM */
			CanTp_rxRuntime->pdurBuffer->sduDataPtr[CanTp_rxRuntime->pdurBufferCount] = CanTp_rxRuntime->canFrameBuffer.data[loop];
			CanTp_rxRuntime->transferCount++;
			CanTp_rxRuntime->pdurBufferCount++;
		}
		
	}
	return Can_ret;
}

/********************************************************************************/
/***
 * @brief			:CanTp_FindTpRxIndicationIndexFromRxChannel  
 * 
 * Service ID		: <CanTp_FindTpRxIndicationIndexFromRxChannel>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC uint8 CanTp_FindTpRxIndicationIndexFromRxChannel(const PduIdType RxPduId,
										                const PduInfoType *PduInfoPtr, 
							                            uint8 *const CanTp_tpci, 
							                            uint8 *const CanTp_tpciOffset)
{
	uint8 loop;
	uint8 index = INDEX_NOT_FIND;

	const CanTp_RxNSduType *CanTp_rxConfig;
	
	for (loop = 0u; (loop < CANTP_NSDU_CONFIG_LIST_SIZE) && (index == INDEX_NOT_FIND); loop++)
	{
		if (CanTpConfig.CanTpNSduList[loop].CanTpDirection == ISO15765_RECEIVE) 
		{
			CanTp_rxConfig = (CanTp_RxNSduType*)&CanTpConfig.CanTpNSduList[loop].ConfigData;
			/* check is this channel */
			if ((CanTp_rxConfig->CanIf_FcPduId == RxPduId)
				&&(((CANTP_STANDARD == CanTp_rxConfig->CanTpAddressingFormant)
				 &&((PduInfoPtr->sduDataPtr[0u] & ISO15765_TPCI_MASK) < ISO15765_TPCI_FC))
				 ||((CANTP_EXTENDED ==  CanTp_rxConfig->CanTpAddressingFormant)
				 && (CanTp_rxConfig->CanTpNSa->CanTpNSa == PduInfoPtr->sduDataPtr[0u])
				 &&((PduInfoPtr->sduDataPtr[1u]&ISO15765_TPCI_MASK) < ISO15765_TPCI_FC))))
			{
				if(CANTP_STANDARD == CanTp_rxConfig->CanTpAddressingFormant)
				{
					*CanTp_tpci = PduInfoPtr->sduDataPtr[0u];
					*CanTp_tpciOffset = 0u;
				}
				else
				{
					*CanTp_tpci = PduInfoPtr->sduDataPtr[1u];
					*CanTp_tpciOffset = 1u;
				}
				index = loop;	
			}
		}
	}
	return index;
}

/********************************************************************************/
/***
 * @brief			:CanTp_FindTpRxIndicationIndexFromTxChannel  
 * 
 * Service ID		: <CanTp_FindTpRxIndicationIndexFromTxChannel>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC uint8 CanTp_FindTpRxIndexFromTxChannel(const PduIdType CanTp_pduId)
{
	uint8 index = INDEX_NOT_FIND;
   uint8 loop;
	const CanTp_TxNSduType *CanTp_txConfig;
	 
   for (loop = 0u; (loop < CANTP_NSDU_CONFIG_LIST_SIZE) && (INDEX_NOT_FIND == index); loop ++)  
	{
		if (CanTpConfig.CanTpNSduList[loop].CanTpDirection == ISO15765_TRANSMIT) 
		{
			CanTp_txConfig = (CanTp_TxNSduType*)&CanTpConfig.CanTpNSduList[loop].ConfigData;
			/* check it is the receiver channel pduid */
		   if (CanTp_pduId == CanTp_txConfig->CanTp_FcPduId)
		   {	
			   index = loop;
		   }
		   else
		   { 
		   	/*Do nothing*/
	      }
		}
   }
   return index;
}

/********************************************************************************/
/***
 * @brief			:CanTp_FindTpListIndexFromRxPdu  
 * 
 * Service ID		: <CanTp_FindTpListIndexFromRxPdu>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC uint8 CanTp_FindTpListIndexFromRxPdu( const PduIdType RxPduId, 
                                             const PduInfoType * PduInfoPtr,
									         CanTp_Iso15765_FrameType *CanTp_format, 
									         PduLengthType *pduLength)
{
	uint8 index;
	uint8 CanTp_tpci = 0u;
	uint8 CanTp_tpciOffset	= 0u;

	const CanTp_RxNSduType *CanTp_rxConfig;

	/* find index from received message */
	index = CanTp_FindTpRxIndicationIndexFromRxChannel(RxPduId, PduInfoPtr, &CanTp_tpci, &CanTp_tpciOffset);
	
	if (INDEX_NOT_FIND == index )
	{	
	   /* find index from received message */
		index = CanTp_FindTpRxIndexFromTxChannel(RxPduId); 
		CanTp_tpci = PduInfoPtr->sduDataPtr[0u];
	}
	else
	{
	}
	
	if(INDEX_NOT_FIND != index)
	{
		CanTp_rxConfig = (CanTp_RxNSduType*)&CanTpConfig.CanTpNSduList[index].ConfigData;
		
		switch (CanTp_tpci & ISO15765_TPCI_MASK) 
		{
			case ISO15765_TPCI_SF:
				*CanTp_format = SINGLE_FRAME;
				*pduLength = (PduLengthType)(PduInfoPtr->sduDataPtr[CanTp_tpciOffset] & ISO15765_TPCI_DL);
				if(((*pduLength > STD_ADDR_MAX_SF_LEGNTH) && (CANTP_STANDARD == CanTp_rxConfig->CanTpAddressingFormant))
				  ||((*pduLength > EXT_ADDR_MAX_SF_LEGNTH) && (CANTP_EXTENDED == CanTp_rxConfig->CanTpAddressingFormant))
				  ||(0U == *pduLength))		
				{ 	
				   /* error frame, if single frame lenght is 0 of the length is less than 6 or 7 */
					*CanTp_format = INVALID_FRAME;
				}
				break;
			
			case ISO15765_TPCI_FF:
				*CanTp_format = FIRST_FRAME;
				*pduLength = ((PduLengthType)(PduInfoPtr->sduDataPtr[0u] & 0xfu) << 8u) 
								             +(PduLengthType)PduInfoPtr->sduDataPtr[1u];
				if (*pduLength < (STD_ADDR_MAX_SF_LEGNTH + 1u))
				{ 	
				   /* error frame, for this tp we only support standard frame */
					*CanTp_format = INVALID_FRAME;
				}
				break;
			case ISO15765_TPCI_CF:
				*CanTp_format = CONSECUTIVE_FRAME;
				break;
			
			case ISO15765_TPCI_FC: 
				/* we only receive FC with CTS */
				if (ISO15765_FLOW_CONTROL_STATUS_CTS == (CanTp_tpci & ISO15765_TPCI_FS_MASK))
				{
					*CanTp_format = FLOW_CONTROL_CTS_FRAME;
				}
				else if ((ISO15765_FLOW_CONTROL_STATUS_WAIT == (CanTp_tpci & ISO15765_TPCI_FS_MASK))
						||(ISO15765_FLOW_CONTROL_STATUS_OVFLW == (CanTp_tpci & ISO15765_TPCI_FS_MASK)))
				{	
				   /* if WAIT or OVFLW not ok */
					*CanTp_format = INVALID_FRAME;
					Dcm_TpRxIndication(CanTp_rxConfig->rxDcmId, NTFRSLT_E_NOT_OK);
					CanTp_Init15765RuntimeData(&CanTp_RuntimeData[index+CANTP_RX_CHANNEL_NUM]);
					Cantp_test = 16;
				}
				else
				{
					*CanTp_format = INVALID_FRAME;
				}
				break;
				
			default:
				*CanTp_format = INVALID_FRAME;
				break;
			}
	}
	return index;
}

/********************************************************************************/
/***
 * @brief			:CanTp_FindTpTxConfirmIndexFromTxChannel  
 * 
 * Service ID		: <CanTp_FindTpTxConfirmIndexFromTxChannel>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC uint8 CanTp_FindTpTxConfirmIndexFromTxChannel(PduIdType CanTp_txPduId)
{
	uint8 loop;
	uint8 index 	= INDEX_NOT_FIND;
	CanTp_RunTime_DataType *CanTp_txRuntime;
	const CanTp_TxNSduType *CanTp_txConfig;

	for (loop = 0u; (loop < CANTP_NSDU_CONFIG_LIST_SIZE) && (index == INDEX_NOT_FIND); loop++)
	{
		if (CanTpConfig.CanTpNSduList[loop].CanTpDirection == ISO15765_TRANSMIT) 
		{
			/* search tp index */
			CanTp_txRuntime	= &CanTp_RuntimeData[loop];
			CanTp_txConfig = (CanTp_TxNSduType*)&CanTpConfig.CanTpNSduList[loop].ConfigData;
	
		   if ((CanTp_txConfig->CanIf_PduId == CanTp_txPduId)
				&&((WAITING_FOR_SF_TX_CONFIRMATION == CanTp_txRuntime->tpState)
				 ||(WAITING_FOR_FF_TX_CONFIRMATION == CanTp_txRuntime->tpState)
				 ||(WAITING_FOR_CF_TX_CONFIRMATION == CanTp_txRuntime->tpState)
				 ||(WAITING_FOR_LAST_CF_TX_CONFIRMATION == CanTp_txRuntime->tpState)
				 ||(WAITING_FOR_CF_BLOCK_TX_CONFIRMATION == CanTp_txRuntime->tpState))) 
			{
				index = loop;	
			}
		}
	}
	return index;
}

/********************************************************************************/
/***
 * @brief			:CanTp_FindTpTxConfirmIndexFromRxChannel  
 * 
 * Service ID		: <CanTp_FindTpTxConfirmIndexFromRxChannel>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC uint8 CanTp_FindTpTxConfirmIndexFromRxChannel(PduIdType CanTp_txPduId)
{

	uint8 rx_Index = INDEX_NOT_FIND;
   uint8 tx_Index = INDEX_NOT_FIND;
   uint8 loop;

   //CanTp_RunTime_DataType *CanTp_rxRuntime;
	const CanTp_RxNSduType *CanTp_rxConfig;

	//CanTp_RunTime_DataType *CanTp_txRuntime;
	const CanTp_TxNSduType *CanTp_txConfig;

	for (loop = 0u; (loop < CANTP_NSDU_CONFIG_LIST_SIZE) && (tx_Index == INDEX_NOT_FIND); loop++)
	{
		if (CanTpConfig.CanTpNSduList[loop].CanTpDirection == ISO15765_TRANSMIT) 
		{
			/* search tp index */
			//CanTp_txRuntime	= &CanTp_RuntimeData[loop];
			CanTp_txConfig = (CanTp_TxNSduType*)&CanTpConfig.CanTpNSduList[loop].ConfigData;

			if (CanTp_txConfig->CanIf_PduId == CanTp_txPduId)
			{
    			tx_Index = loop;
	    	}
	    	else
	    	{
	    	   /* do nothing */
	    	}	
		}
	}

	for (loop = 0u; (loop < CANTP_NSDU_CONFIG_LIST_SIZE) && (rx_Index == INDEX_NOT_FIND); loop++)
	{
		if (CanTpConfig.CanTpNSduList[loop].CanTpDirection == ISO15765_RECEIVE) 
		{
			/* search tp index */
			/*CanTp_rxRuntime	= &CanTp_RuntimeData[loop];*/
			CanTp_rxConfig = (CanTp_RxNSduType*)&CanTpConfig.CanTpNSduList[loop].ConfigData;

			if ((CanTp_rxConfig->CanTp_FcPduId == CanTp_txPduId) && 
				 (CanTp_rxConfig->CanIf_FcPduId == CanTp_txConfig->CanTp_FcPduId))
			{
    			rx_Index = loop;	
	    	}
	    	else
	    	{
	    	   /* do nothing */
	    	}	
		}
	}
	return rx_Index;
}

/********************************************************************************/
/***
 * @brief			:CanTp_FindTpTxListFromSduId  
 * 
 * Service ID		: <CanTp_FindTpTxListFromSduId>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC uint8 CanTp_FindTpTxListFromSduId(const PduIdType CanTp_SduId)
{
	uint8 index = INDEX_NOT_FIND;
   uint8 loop;
	const CanTp_TxNSduType *CanTp_txConfig;

	 for (loop = 0u; (loop < CANTP_NSDU_CONFIG_LIST_SIZE) && (index == INDEX_NOT_FIND); loop++)
	{
		if (CanTpConfig.CanTpNSduList[loop].CanTpDirection == ISO15765_TRANSMIT) 
		{
			CanTp_txConfig = (CanTp_TxNSduType*)&CanTpConfig.CanTpNSduList[loop].ConfigData;

			if(CanTp_txConfig->txDcmId == CanTp_SduId)
			{
				index = loop;
			}
			else
	    	{
	    	   /* do nothing */
	    	}
		}
	}
	return index;
}

/********************************************************************************/
/***
 * @brief			:CanTp_calcTxFrameType  
 * 
 * Service ID		: <CanTp_calcTxFrameType>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC  CanTp_Iso15765_FrameType CanTp_calcTxFrameType( const CanTp_RunTime_DataType * const CanTp_txRuntime ) 
{
	CanTp_Iso15765_FrameType Can_ret = INVALID_FRAME;
	/* CANTP_STANDARD */
	if (CanTp_txRuntime->transferTotal > STD_ADDR_MAX_SF_LEGNTH) 
	{	
	    /* first frame */
		Can_ret = FIRST_FRAME;	
	} 
	else
	{	
	    /* single frame */
		Can_ret = SINGLE_FRAME;
	}

	return Can_ret;
}

/********************************************************************************/
/***
 * @brief			:CanTp_SendFlowControlFrame  
 * 
 * Service ID		: <CanTp_SendFlowControlFrame>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC  void CanTp_SendFlowControlFrame(const CanTp_RxNSduType *CanTp_rxConfig, 
                                        CanTp_RunTime_DataType *CanTp_rxRuntime, 
								        BufReq_RetType CanTp_flowStatus) 								        
{
	PduInfoType CanTp_pduInfo;
	uint8 sduData[8u]; 					

	CanTp_pduInfo.sduDataPtr = sduData;
	CanTp_pduInfo.sduLength  = 0u;
	
	if (BUFREQ_OK == CanTp_flowStatus)
	{	/* add PCI,and STmin */
		sduData[CanTp_pduInfo.sduLength] = ISO15765_TPCI_FC | ISO15765_FLOW_CONTROL_STATUS_CTS;
		CanTp_pduInfo.sduLength++;
		sduData[CanTp_pduInfo.sduLength] = (uint8)0u;
		CanTp_pduInfo.sduLength++;
		sduData[CanTp_pduInfo.sduLength] = (uint8)ISO15765_ST_MIN;
		CanTp_pduInfo.sduLength++;
	}
	else
	{
		sduData[CanTp_pduInfo.sduLength] = ISO15765_TPCI_FC | ISO15765_FLOW_CONTROL_STATUS_OVFLW;
		CanTp_pduInfo.sduLength++;
	}
	
	/* add padding */
	CanTp_FrameAddPadding(&CanTp_pduInfo);		
    /* maybe should for a  tolerant value */
	CanTp_rxRuntime->timeoutCounterValue = CanTp_ConvertMsToMainCycles(ISO15765_N_Ar);
	/* send frame */
	(void)CanIf_Transmit((uint16)CanTp_rxConfig->CanTp_FcPduId,&CanTp_pduInfo);

	return;
}

/********************************************************************************/
/***
 * @brief			:CanTp_CopyDataToMsgFromTxBuf  
 * 
 * Service ID		: <CanTp_CopyDataToMsgFromTxBuf>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC boolean CanTp_CopyDataToMsgFromTxBuf(const CanTp_TxNSduType *CanTp_txConfig,
									        CanTp_RunTime_DataType * CanTp_txRuntime,
									        PduInfoType *const CanTp_pduInfo,
									        const uint8 t_u1_actualPayload)
{
	boolean framPrepare = FALSE;
	uint8 loop;

	if (NULLPTR == CanTp_txRuntime->pdurBuffer)
	{	
	    /* first get buffer */
		(void)Dcm_ProvideTxBuffer( CanTp_txConfig->txDcmId,&CanTp_txRuntime->pdurBuffer);
	}

	if (NULLPTR != CanTp_txRuntime->pdurBuffer)
	{	
	    /* buffer ok ,copy data */
		for ( loop = 0; loop < t_u1_actualPayload; loop++ )
		{	
		    /* copy local buffer to provided buffer by DCM */
			CanTp_pduInfo->sduDataPtr[CanTp_pduInfo->sduLength] 
				= CanTp_txRuntime->pdurBuffer->sduDataPtr[CanTp_txRuntime->pdurBufferCount];
			CanTp_txRuntime->transferCount++;
			CanTp_pduInfo->sduLength++;
			CanTp_txRuntime->pdurBufferCount++;
		}
		framPrepare = TRUE;	
	}
	CanTp_FrameAddPadding(CanTp_pduInfo);

	return framPrepare;
}

/********************************************************************************/
/***
 * @brief			:CanTp_SendSingleFrame  
 * 
 * Service ID		: <CanTp_SendSingleFrame>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC void CanTp_SendSingleFrame(const CanTp_TxNSduType *CanTp_txConfig, 
					    	      CanTp_RunTime_DataType *CanTp_txRuntime,
					    	      PduInfoType *const CanTp_pduInfo)
{
	boolean framPrepare ;
	
	CanTp_pduInfo->sduDataPtr[CanTp_pduInfo->sduLength] = ISO15765_TPCI_SF + (uint8)CanTp_txRuntime->transferTotal;
	CanTp_pduInfo->sduLength++;
	
	framPrepare = CanTp_CopyDataToMsgFromTxBuf(CanTp_txConfig, CanTp_txRuntime, 
						   CanTp_pduInfo,(uint8)CanTp_txRuntime->transferTotal);
	if(TRUE == framPrepare)	
	{
		CanTp_txRuntime->tpState = WAITING_FOR_SF_TX_CONFIRMATION;
		/* load time of N_As */
		CanTp_txRuntime->timeoutCounterValue = CanTp_ConvertMsToMainCycles(CanTp_txConfig->CanTpNas);	
		(void)CanIf_Transmit((uint16)CanTp_txConfig->CanIf_PduId, CanTp_pduInfo);
	}
	else
	{
		Dcm_TxConfirmation(CanTp_txConfig->txDcmId, NTFRSLT_E_NO_BUFFER);
		CanTp_Init15765RuntimeData(CanTp_txRuntime);	
		Cantp_test = 17;
	}
}

/********************************************************************************/
/***
 * @brief			:CanTp_SendFirstFrame  
 * 
 * Service ID		: <CanTp_SendFirstFrame>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC void CanTp_SendFirstFrame(const CanTp_TxNSduType *CanTp_txConfig, 
					    	     CanTp_RunTime_DataType *CanTp_txRuntime,
					    	     PduInfoType *const CanTp_pduInfo)
{
	uint8 sfActualPayload;
	boolean framPrepare;
	
	CanTp_pduInfo->sduDataPtr[CanTp_pduInfo->sduLength] 
			= ISO15765_TPCI_FF | (uint8)((CanTp_txRuntime->transferTotal & 0x0f00u) >> 8u);
	CanTp_pduInfo->sduLength++;
	CanTp_pduInfo->sduDataPtr[CanTp_pduInfo->sduLength] 
			= (uint8) (CanTp_txRuntime->transferTotal & 0xffu);
	CanTp_pduInfo->sduLength++;

	sfActualPayload = STD_ADDR_MAX_FF_LENGTH;
	
	framPrepare = CanTp_CopyDataToMsgFromTxBuf(CanTp_txConfig, CanTp_txRuntime, CanTp_pduInfo,sfActualPayload);
	if (TRUE == framPrepare)
	{
		/* + 1 is because the consecutive frame numbering begins with 1 and not */
		CanTp_txRuntime->framesHandledCount  = 1u; 	
		CanTp_txRuntime->tpState = WAITING_FOR_FF_TX_CONFIRMATION;
		CanTp_txRuntime->timeoutCounterValue = CanTp_ConvertMsToMainCycles(CanTp_txConfig->CanTpNas);
		(void)CanIf_Transmit((uint16)CanTp_txConfig->CanIf_PduId, CanTp_pduInfo);
	}
	else
	{ 	
		/* frame error */
		Dcm_TxConfirmation(CanTp_txConfig->txDcmId,NTFRSLT_E_NO_BUFFER);
		CanTp_Init15765RuntimeData(CanTp_txRuntime);		
		Cantp_test = 18;
	}
}

/********************************************************************************/
/***
 * @brief			:CanTp_SendConsecutiveFrame  
 * 
 * Service ID		: <CanTp_SendConsecutiveFrame>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC void  CanTp_SendConsecutiveFrame(const CanTp_TxNSduType *CanTp_txConfig, 
					    		        CanTp_RunTime_DataType *CanTp_txRuntime,
								        PduInfoType *const CanTp_pduInfo)
{
	uint8 cfActualPayload;
	boolean lastFrame = FALSE;
	uint16 txDataRemain = CanTp_txRuntime->transferTotal - CanTp_txRuntime->transferCount;
	
	CanTp_pduInfo->sduDataPtr[CanTp_pduInfo->sduLength] = ISO15765_TPCI_CF + CanTp_txRuntime->framesHandledCount;
	CanTp_pduInfo->sduLength++;
	/* Calculate number of valid bytes that reside in this CF */
	if (txDataRemain <= (uint16)(STD_ADDR_MAX_CF_LENGTH)) 
	{	
	   /* Last frame. */
		cfActualPayload = (uint8)(txDataRemain); 
		lastFrame = TRUE;
	} 
	else 
	{
		cfActualPayload = (uint8)(STD_ADDR_MAX_CF_LENGTH);
	}
	
	/* copy data to can message */
	(void)CanTp_CopyDataToMsgFromTxBuf(CanTp_txConfig,CanTp_txRuntime, CanTp_pduInfo,cfActualPayload);							  						  

	/* change tp state */
	if (TRUE == lastFrame)
	{	
	    /* this is the last frame of this message */
		CanTp_txRuntime->tpState = WAITING_FOR_LAST_CF_TX_CONFIRMATION;
		CanTp_txRuntime->timeoutCounterValue = CanTp_ConvertMsToMainCycles(CanTp_txConfig->CanTpNas);
	}
	else
	{
		if (CanTp_txRuntime->bs > 0u)
		{
			CanTp_txRuntime->bs-- ;
		}

		if (CanTp_txRuntime->bs > 0u)
		{	
		    /* for recevier side the block size is zero */
			CanTp_txRuntime->tpState = WAITING_FOR_CF_TX_CONFIRMATION;
			CanTp_txRuntime->timeoutCounterValue = CanTp_txRuntime->stmin;	
		}
		else
		{	
		    /* bs=0 */
			CanTp_txRuntime->tpState = WAITING_FOR_CF_BLOCK_TX_CONFIRMATION;
		   CanTp_txRuntime->timeoutCounterValue = CanTp_ConvertMsToMainCycles(CanTp_txConfig->CanTpNas);		
		}
	}

	CanTp_txRuntime->framesHandledCount = (CanTp_txRuntime->framesHandledCount + 1u) & SEGMENT_NUMBER_MASK;
	/* send frame */
	(void)CanIf_Transmit((uint16)CanTp_txConfig->CanIf_PduId, CanTp_pduInfo);
}

/********************************************************************************/
/***
 * @brief			:CanTp_SendTxFrame  
 * 
 * Service ID		: <CanTp_SendTxFrame>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC void CanTp_SendTxFrame(const CanTp_TxNSduType *CanTp_txConfig, 
					          CanTp_RunTime_DataType *CanTp_txRuntime,
					          const CanTp_Iso15765_FrameType CanTp_framType)

{
	uint8 sduData[CANDRV_PDU_MAX_LENGTH];
	PduInfoType	 CanTp_pduInfo;

	CanTp_pduInfo.sduDataPtr = sduData;
	CanTp_pduInfo.sduLength 	= 0;

	switch (CanTp_framType)
	{
	case SINGLE_FRAME:
		/* send single frame */
		CanTp_SendSingleFrame(CanTp_txConfig, CanTp_txRuntime, &CanTp_pduInfo);
		break;
		

	case FIRST_FRAME:
		/* send first frame */
		CanTp_SendFirstFrame(CanTp_txConfig, CanTp_txRuntime, &CanTp_pduInfo);
		break;
		

	case CONSECUTIVE_FRAME:	
		/* send consecutive frame */
		CanTp_SendConsecutiveFrame(CanTp_txConfig, CanTp_txRuntime, &CanTp_pduInfo);
		break;
		

	default:
		Dcm_TxConfirmation(CanTp_txConfig->txDcmId, NTFRSLT_E_NOT_OK);
		CanTp_Init15765RuntimeData(CanTp_txRuntime);	
		Cantp_test = 19;
		break;
	}
	
	return;
}

/********************************************************************************/
/***
 * @brief			:CanTp_WaitSForLastCFBufProcess  
 * 
 * Service ID		: <CanTp_WaitSForLastCFBufProcess>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC void CanTp_WaitSForLastCFBufProcess(const CanTp_RxNSduType *CanTp_rxConfig,
								           CanTp_RunTime_DataType *CanTp_runtimeItem)								           
{
	BufReq_RetType Can_ret;
	
	Can_ret = CanTp_CopySegmentToRxBuffer(CanTp_rxConfig,CanTp_runtimeItem);
	if (BUFREQ_OK == Can_ret) 
	{	
		/* buffer ok */
		Dcm_TpRxIndication(CanTp_rxConfig->rxDcmId, NTFRSLT_OK);								
		CanTp_Init15765RuntimeData(CanTp_runtimeItem); /* ok */
		Cantp_test = 20;
	} 
	else 
	{	
		/* buffer not ok */
		Dcm_TpRxIndication(CanTp_rxConfig->rxDcmId, NTFRSLT_E_NO_BUFFER);
		CanTp_Init15765RuntimeData(CanTp_runtimeItem);	
		Cantp_test = 21;
	}
}

/********************************************************************************/
/***
 * @brief			:CanTp_WaitCFBufProcess  
 * 
 * Service ID		: <CanTp_WaitCFBufProcess>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/********************************************************************************/
STATIC void CanTp_WaitCFBufProcess(const CanTp_RxNSduType *CanTp_rxConfig,CanTp_RunTime_DataType *CanTp_runtimeItem)						           
{
	BufReq_RetType Can_ret = CanTp_CopySegmentToRxBuffer(CanTp_rxConfig, CanTp_runtimeItem);
	if (BUFREQ_OK == Can_ret) 
	{	/* sent CTS */	
		CanTp_runtimeItem->tpState = WAITING_FOR_FC_CTS_TX_CONFIRMATION;
		CanTp_SendFlowControlFrame(CanTp_rxConfig, CanTp_runtimeItem, Can_ret);
	} 
	else 
	{
		/* send overflow status control */
		CanTp_runtimeItem->tpState = WAITING_FOR_FC_OVFLW_TX_CONFIRMATION;
		CanTp_SendFlowControlFrame(CanTp_rxConfig, CanTp_runtimeItem, BUFREQ_E_OVFL);			
	}
}

/***=======[R E V I S I O N   H I S T O R Y]====================================*/
/**** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121228    Tommy Yu     Initial version
 *  
 */
/***=======[E N D   O F   F I L E]==============================================*/

