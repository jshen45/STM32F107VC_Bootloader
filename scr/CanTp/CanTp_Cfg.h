/*============================================================================*/
/*** Copyright (C) 2009-2011, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  
 *  @file       <CanTp_Cfg.h>
 *  @brief      <The CanTp Configration file>
 *  
 *  
 *  @author     <Tommy Yu>
 *  @date       <2012-12-28>
 */
/*============================================================================*/
  
#ifndef CANTP_CFG_H
#define CANTP_CFG_H

/* to get the GPT_PERIOD_TIME */
//#include "Gpt_Cfg.h"

/*=======[M A C R O S]========================================================*/

/** Minimum time the sender shall wait between transmissions of two N-PDU */ 
/* @type:uint8 range:0~255 note:uint-ms */
#define ISO15765_ST_MIN     ((uint8)10u)	

/** N_As timeout for transmission of any CAN frame */
/* @type:uint8 range:0~255 note:uint-ms */
#define ISO15765_N_As       ((uint16)1000u)

/** Timeout for transmission of a CAN frame (ms) */	
/* @type:uint8 range:0~255 note:uint-ms */
#define ISO15765_N_Ar       ((uint16)1000u)

/** N_Bs timeout */	
/* @type:uint8 range:0~255 note:uint-ms */
#define ISO15765_N_Bs       ((uint16)1000u)

/** Time out for consecutive frames (ms) */	
/* @type:uint8 range:0~255 note:uint-ms */
#define ISO15765_N_Cr       ((uint16)1000u)

/* @type:define range:GPT_PERIOD_TIME note:auto generate */
//#define CANTP_MAIN_TICK GPT_PERIOD_TIME
#define CANTP_MAIN_TICK     5


/** padding value */	
/* @type:uint8 range:0x00~0xFF note:NONE */
#define CANTP_PADDING_VALUE (0x55u)	

/* @type:NONE range:NONE note:auto generate */
#define PADDING_ON

/* @type:uint8 range:1~255 note:reference to num of CanTpNSduConfigList.CanTpTxNSdu */
#define CANTP_TX_CHANNEL_NUM 	1u

/* @type:uint8 range:1~255 note:reference to num of CanTpNSduConfigList.CanTpRxNSdu */
#define CANTP_RX_CHANNEL_NUM 	1u

#define CANTP_NSDU_CONFIG_LIST_SIZE 	((uint8)2u)

/*CanTp Addressing Formant*/
#define 	CANTP_ADD_EXTENDED     	((uint8)0u)
#define 	CANTP_ADD_STANDARD     	((uint8)1u)

/*CanTp State*/
#define 	CANTP_OFF          	((uint8)0u)
#define 	CANTP_ON           	((uint8)1u)

/*CanTp  target address Type*/
#define 	CANTP_FUNCTIONAL 		((uint8)0u)
#define 	CANTP_PHYSICAL   		((uint8)1u)

/*CanTp Direction Type*/
#define 	ISO15765_TRANSMIT		((uint8)0u)
#define 	ISO15765_RECEIVE 		((uint8)1u)

/*CanTp List Item Type*/
#define 	CANTP_NOT_LAST_ENTRY ((uint8)0u)
#define 	CANTP_END_OF_LIST		((uint8)1u)

/* CanTp Rx NSdu Id */
#define CANTP_RX_vEcuC_Pdu_1 ((uint8)0u)

/* CanTp Tx NSdu Id */
#define CANTP_TX_vEcuC_Pdu_1 ((uint8)1u)
/*************************************************************************
Declaration of types
*************************************************************************/
typedef struct 
{
	uint32 CanTpMainFunctionPeriod;   /* @req ECUC_CanTp_00240 */
}CanTp_GeneralType;                  /* @req ECUC_CanTp_00278 */

typedef struct 
{
	const uint32 CanTpNSa;     /* @req ECUC_CanTp_00254 */
}CanTp_NSaType;         /* @req ECUC_CanTp_00253 */

typedef struct 
{
	const uint32 CanTpNTa;     /* @req ECUC_CanTp_00255 */ 
}CanTp_NTaType;         /* @req ECUC_CanTp_00139 */

typedef struct 
{
	const PduIdType rxDcmId;
	const PduIdType CanTp_FcPduId;  /* When recieving this Pdu this conf can be used.*/
	const PduIdType CanIf_FcPduId;  /* The CanIf PDU index.*/
	const uint8     CanTpAddressingFormant;	/* @req ECUC_CanTp_00281 */
	const uint8     CanTpBs;        /* @req ECUC_CanTp_00276 , Sets the maximum number of messages of N-PDUs before flow control. */
	const uint16    CanTpNar;       /* @req ECUC_CanTp_00277 , Timeout for transmission of a CAN frame (ms). */
	const uint16    CanTpNbr;       /* @req ECUC_CanTp_00245 */
	const uint16    CanTpNcr;       /* @req ECUC_CanTp_00279 , Time out for consecutive frames (ms). */
	const uint8     CanTpRxChannel; /* Connection to runtime variable index. */
	const uint16    CanTpRxDI;      /* Data length code for of this RxNsdu. */
	const uint8     CanTpRxPaddingActivation; /* @req ECUC_CanTp_00249 , Enable use of padding. */
	const uint8     CanTpRxTaType;  /* @req ECUC_CanTp_00250 , Functional or physical addressing. */
	const uint8     CanTpWftMax;    /* @req ECUC_CanTp_00251 , Max number FC wait that can be transmitted consecutively. */
	const uint16    CanTpSTmin;     /* @req ECUC_CanTp_00252 , Minimum time the sender shall wait between transmissions of two N-PDU. */
	/*const uint32							CanTpNSduRef ** req: CanTp241. This is PDU id - typeless enum. */
	const CanTp_NSaType *CanTpNSa;  /* @req ECUC_CanTp_00253 , This parameter contains the transport protocol source address value. */
	const CanTp_NTaType *CanTpNTa;  /* @req ECUC_CanTp_00139 , This parameter contains the transport protocol target address value. */
	//CanTp_RxNPduType  *CanTpRxNPdu;
	//CanTp_TxFcNPduType *CanTpTxFcNPdu;
	//const PduIdType CanTpRxPduId;

}CanTp_RxNSduType; /** @req ECUC_CanTp_00137 */

typedef struct 
{
	const PduIdType txDcmId;
	const PduIdType CanTp_FcPduId;
	const PduIdType CanIf_PduId;         /* The CanIf index.*/
	const uint8     CanTpAddressingMode; /* @req ECUC_CanTp_00262, CanTpTxAddressingFormat  */
	const uint8     Reserved_CanTpBs;    /* to make this type same as CanTp_RxNSduType */
	const uint16    CanTpNas;            /* @req CANTP263 */ /* N_As timeout for transmission of any CAN frame. */
	const uint16    CanTpNbs; 				 /* @req CANTP264 */ /* N_Bs timeout of transmission until reception of next Flow Control. */
	const uint16    CanTpNcs;            /* @req CANTP265 */ /* N_Bs timeout of transmission until reception of next Flow Control. */
	const uint8	    CanTpTxChannel;      /* Link to the TX connection channel. */
	const uint16    CanTpTxDI;           /* Data length code for of this TxNsdu. */
	const uint8     CanTpTxPaddingActivation; /* @req ECUC_CanTp_00269 , Enable use of padding. */
	const uint8     CanTpTxTaType;       /* @req ECUC_CanTp_00270 , Functional or physical addressing. */
	const uint8     Reserved_CanTpWftMax; /* to make this type same as CanTp_RxNSduType */
	const uint16    Reserved_CanTpSTmin;  /* to make this type same as CanTp_RxNSduType */
	/*const uint32						CanTpNSduRef ** req: ECUC_CanTp_00261 . This is PDU id - typeless enum. */
	const CanTp_NSaType *CanTpNSa;       /* @req ECUC_CanTp_00253 , This parameter contains the transport protocol source address value. */
	const CanTp_NTaType *CanTpNTa;		 /* @req ECUC_CanTp_00139 , This parameter contains the transport protocol target address value. */
	//CanTp_RxFcNPduType *CanTpRxFcNPdu;
	//CanTp_TxNPduType   *CanTpTxNPdu;
	//PduIdType CanTpTxPduId;

}CanTp_TxNSduType; /* @req ECUC_CanTp_00138 */


typedef struct 
{
	const uint8 CanTpDirection;
	const uint8 CanTpListItemType;
	union
	{
		const CanTp_RxNSduType 	CanTpRxNSdu;   /* @req ECUC_CanTp_00137 */ 
		const CanTp_TxNSduType 	CanTpTxNSdu;   /* @req ECUC_CanTp_00138 */ 
	} ConfigData;
}CanTp_NSduType;

typedef struct 
{
	const uint8     CanTpAddressingMode;
	const PduIdType CanTpNSduIndex;
	const PduIdType CanTpReferringTxIndex;
}CanTp_RxIdType;

/* Top level config container for CANTP implementation. */
typedef struct 
{
	const CanTp_GeneralType *CanTpGeneral;   /*  10.2.3, General configuration paramters for the CANTP module. */
	const CanTp_NSduType    *CanTpNSduList;
	const CanTp_RxIdType    *CanTpRxIdList;
}CanTp_ConfigType;

extern const CanTp_ConfigType CanTpConfig;
#endif


/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121228    Tommy Yu     Initial version
 * 
 */
/*=======[E N D   O F   F I L E]==============================================*/

