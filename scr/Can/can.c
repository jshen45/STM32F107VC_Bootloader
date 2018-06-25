/**
  ******************************************************************************
  * @file    can.c
  * $Author: wdluo $
  * $Revision: 17 $
  * $Date:: 2012-07-06 11:16:48 +0800 #$
  * @brief   CAN总线收发相关函数.
  ******************************************************************************
  * @attention
  *
  *<h3><center>&copy; Copyright 2009-2012, ViewTool</center>
  *<center><a href="http:\\www.viewtool.com">http://www.viewtool.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Common_Types.h"
#include "Can.h"
#include "CanIf_Cfg.h"
#include "CanIf.h"
#include "CanTp.h"
//#include "Can_Cfg.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
EXPORTED CanRxMsg RxMessage;
EXPORTED volatile uint8_t CanRxMsgFlag=0;//接收到CAN数据后的标志
/* Private variables ---------------------------------------------------------*/
/**=======[E X T E R N A L   D A T A]==========================================*/
EXPORTED Can_GlobalType Can_Global;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  CAN时钟配置表，CLK=36MHz 
  */
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq,   6,     // 1M     36/(1+3+2)/6 =1M  
// CAN_SJW_1tq, CAN_BS1_4tq , CAN_BS2_3tq,   5,     // 900K   36/(1+4+3)/5 =0.9  
// CAN_SJW_1tq, CAN_BS1_5tq , CAN_BS2_3tq,   5,     // 800K   36/(1+5+3)/5 =0.8  
// CAN_SJW_1tq, CAN_BS1_6tq , CAN_BS2_3tq,   6,     // 600K   36/(1+6+3)/6=0.6  
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq,  12,     // 500K     
// CAN_SJW_1tq, CAN_BS1_5tq , CAN_BS2_3tq,  10,     // 400K     
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq,  20,     // 300K   36/(1+3+2)/20  
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq,  24,     // 250K     
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq,  30,     // 200K    
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq,  40,     // 150K   36/(1+3+2)/40  
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq,  48,     // 125K   36/(1+3+2)/48  
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq,  60,     // 100K     
// CAN_SJW_1tq, CAN_BS1_4tq , CAN_BS2_3tq,  50,     //  90K   36/(1+4+3)/50 =0.09  
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq,  75,     //  80K    
// CAN_SJW_1tq, CAN_BS1_6tq , CAN_BS2_3tq,  60,     //  60K    
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq, 120,     //  50K    
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq, 150,     //  40K   
// CAN_SJW_1tq, CAN_BS1_6tq , CAN_BS2_3tq, 120,     //  30K    
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq, 300,     //  20K       
// CAN_SJW_1tq, CAN_BS1_3tq , CAN_BS2_2tq, 600,     //  10K  
// CAN_SJW_2tq, CAN_BS1_6tq , CAN_BS2_4tq, 600,     //  5K   36/(2+6+4)/600=5K   
// CAN_SJW_2tq, CAN_BS1_6tq , CAN_BS2_4tq, 1000,    //  3K   36/(2+6+4)/1000  
// CAN_SJW_2tq, CAN_BS1_10tq, CAN_BS2_6tq, 1000     //  2K   36/(2+10+6)/1000=2K   
/**
  * @brief  CAN引脚配置
  * @param  None
  * @retval None
  */
void CAN_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* CAN Periph clock enable */
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO ,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO ,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	/* Configure CAN pin: RX */
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;
	//GPIO_Init(GPIOB, &GPIO_InitStructure);   
	GPIO_Init(GPIOD, &GPIO_InitStructure);   
	/* Configure CAN pin: TX */
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	//GPIO_Init(GPIOB, &GPIO_InitStructure);	     
	GPIO_Init(GPIOD, &GPIO_InitStructure);	 
	GPIO_PinRemapConfig(GPIO_Remap2_CAN1 , ENABLE);

	//AFIO->MAPR |= 0X00406000;              
}
/**
  * @brief  CAN接收中断配置
  * @param  None
  * @retval None
  */
void CAN_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable CAN1 RX0 interrupt IRQ channel */
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;//USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
/**
  * @brief  初始化CAN
  * @param  None
  * @retval None
  */
void CAN_Configuration(const Can_ConfigType* Can_Config)
{
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	  /* save config */
  	Can_Global.Config = Can_Config;

	CAN_NVIC_Configuration();
	CAN_GPIO_Configuration();
	/* CAN register init */
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;//ENABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = ENABLE;//DISABLE;//;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	/*CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;//配置波特率为1M
	CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
	CAN_InitStructure.CAN_Prescaler = 4;*/
	CAN_InitStructure.CAN_SJW = CAN_SJW_2tq;//配置波特率为500k
	CAN_InitStructure.CAN_BS1 = CAN_BS1_2tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_1tq;
	CAN_InitStructure.CAN_Prescaler = 4;

	if (CAN_Init(CAN1,&CAN_InitStructure) == CANINITFAILED)
	{
		//printf("Initialize CAN failed!\n\r");
	}

	//设置CAN接收过滤器
	CAN_FilterInitStructure.CAN_FilterNumber=0;//过滤器0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;//屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;//32bit模式
	//以下4个为0表示接收任何数据
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//使能过滤器
	CAN_FilterInit(&CAN_FilterInitStructure);
	//使能接收中断
	CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);	
}

void CAN_ConfigFilter(uint8_t can_addr)
{
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	//设置CAN接收过滤器
	CAN_FilterInitStructure.CAN_FilterNumber=0;//过滤器0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;//屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;//32bit模式
	//以下4个为0表示接收任何数据
	CAN_FilterInitStructure.CAN_FilterIdHigh=can_addr<<9;
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xFE00;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//使能过滤器
	CAN_FilterInit(&CAN_FilterInitStructure);	
}
/**
  * @brief  发送一帧CAN数据
  * @param  None
  * @retval None
  */
uint8_t CAN_WriteData(CanTxMsg *TxMessage)
{
	uint8_t TransmitMailbox;   
	uint32_t	TimeOut=0;
	
	TransmitMailbox = CAN_Transmit(CAN1,TxMessage);
	while(CAN_TransmitStatus(CAN1,TransmitMailbox) != CAN_TxStatus_Ok)
	{
		TimeOut++;
		if(TimeOut > 10000000)
		{
			//printf("Send data Time out,Data send failed!\n\r");
			return 1;
		}
	}
	return 0;
}

EXPORTED Can_ReturnType Can_Write( Can_HwHandleType Hth, const Can_PduType* PduInfo ) 
{
	Can_ReturnType Can_transmitRet;
	CanTxMsg TxMessage;
	uint16 pduIndex;
	uint8 u8Index;

	TxMessage.DLC = 0;
	TxMessage.ExtId = 0;
	TxMessage.StdId = 0x00;
	TxMessage.RTR = CAN_RTR_DATA;

	Can_transmitRet = CAN_NOT_OK;

	for (pduIndex = 0u; pduIndex < CAN_TX_PDU_NUM; pduIndex++)
	{
		if (PduInfo->swPduHandle == CanIf_TxPduConfigData[pduIndex].CanIf_TxPduId)
		{
			TxMessage.DLC   = PduInfo->length;
			if(PduInfo->id > 0x7FF)
			{
				TxMessage.ExtId = PduInfo->id;
				TxMessage.IDE   = CAN_ID_EXT;
			}
			else 
			{
				TxMessage.StdId = PduInfo->id;
				TxMessage.IDE   = CAN_ID_STD;
			}

			for(u8Index=0;u8Index<TxMessage.DLC;u8Index++)
			{
				TxMessage.Data[u8Index]  = PduInfo->sdu[u8Index];
			}
			CAN_WriteData(&TxMessage);
			Can_transmitRet = CAN_OK;
		}
	}
	return Can_transmitRet;
}

EXPORTED void CAN_ReceiveData(uint8 u8ControllerID)
{
	const Can_ControllerConfigType *CanHwConfig;
	const Can_HardwareObjectType   *HohObj;
	Can_HwType  Can_Mailbox;
	PduInfoType Can_ReceivePdu;
	uint8		 RxData[8u];
	uint8  		data_count;
	uint8  		length;
	
	CanHwConfig          = CAN_GET_CONTROLLER_CONFIG(u8ControllerID);
    HohObj               = CanHwConfig->Can_Hoh;

	do
	{	
		if (HohObj->CanObjectType == CAN_OBJECT_TYPE_RECEIVE)
		{
			Can_Mailbox.Hoh = HohObj->CanObjectId; 
			Can_Mailbox.ControllerId = u8ControllerID;

			break;
		}
		HohObj ++;
	}while(HohObj->Can_EOL != TRUE);

	length = RxMessage.DLC;
    for (data_count = 0u; data_count < length; data_count++)
    {
       RxData[data_count] = RxMessage.Data[data_count];
    }

    /* get the data */
    Can_ReceivePdu.sduDataPtr = RxData;
	/* get the length */
    Can_ReceivePdu.sduLength = (uint16)length;
	
	CanIf_RxIndication(&Can_Mailbox,&Can_ReceivePdu);
}

/*******************************************************************************/
/***
 * @brief			:Can_FindBuffer,find the buffer for rx or tx.  
 * 
 * Service ID		: <Can_FindBuffer>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/*******************************************************************************/
STATIC uint32 Can_FindBuffer(uint32 buffer_mask) 
{
    boolean exit_Flag       = FALSE;
    uint32  compare_lastbit = 0x00000001uL;
    uint8   count           = 0u;

    while ((count < 32u) && (exit_Flag == FALSE))
    {
        if(((uint32)(buffer_mask >> count) & compare_lastbit) == 0u)
        {
         /* if this bit is zero, then count next */
         count ++;
        }
        else
        {
         exit_Flag = TRUE;
        }
    }
    return count;
}

/*******************************************************************************/
/***
 * @brief			: The Can Commication Main Fcuntion.  
 *                   
 * Service ID		: <Can_MainFunc>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>    
 */
/*******************************************************************************/
EXPORTED void Can_MainFunction(void)
{
	uint8 pduIndex;
    uint32 Can_mbMask;
    uint8 Can_mbNr;   
	uint32 Can_mbMask_Low = 0x00000200uL; 

	for (pduIndex = 0u; pduIndex < CAN_TX_PDU_NUM-2; pduIndex++)
    {
        Can_mbMask = (Can_mbMask_Low & Can_TxPdu[pduIndex].Can_TxMailBoxMask);

        while(Can_mbMask != 0u)
        {
            Can_mbNr = (uint8)Can_FindBuffer(Can_mbMask);
         #if 0
            /* Clear interrupt and mark txBox as Free*/
            if (Can_mbNr >= 32u) 
            {
                Can_Ptr->ifrh |= (uint32) (1u << (Can_mbNr - 32u));
            } 
            else 
            {
                Can_Ptr->ifrl |= (uint32)(1u << Can_mbNr);
            }                
            #endif
		    /* callback */
			CanTp_TxConfirmation(Can_TxPdu[pduIndex].pduId);
            
            Can_mbMask &= (uint32)(~(1u << Can_mbNr));

        }

    }

	//CanTp_TxConfirmation(0x01);
	CAN_RxMainFunction();
}

EXPORTED void CAN_RxMainFunction(void)
{
	if(CanRxMsgFlag == CAN_RECEIVE_FLAG)
	{
		CanRxMsgFlag = CAN_RECEIVE_CLEARED;
		CAN_ReceiveData(CAN_CONTROLLER_A);
	}
}
/**
  * @brief  CAN接收中断处理函数
  * @param  None
  * @retval None
  */
//void USB_LP_CAN1_RX0_IRQHandler(void)
void CAN1_RX0_IRQHandler(void)
{
	CanRxMsg ReceiveData;
	
	CAN_Receive(CAN1,CAN_FIFO0, &ReceiveData);
	RxMessage = ReceiveData; 
	RxMessage.DLC = ReceiveData.DLC;
	RxMessage.StdId = ReceiveData.StdId;
	
	CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);
	CanRxMsgFlag = CAN_RECEIVE_FLAG;
}

void CAN1_RX1_IRQHandler(void)
{
	CAN_Receive(CAN1,CAN_FIFO1, &RxMessage);
	CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);
	CanRxMsgFlag = CAN_RECEIVE_FLAG;
}


/*********************************END OF FILE**********************************/
