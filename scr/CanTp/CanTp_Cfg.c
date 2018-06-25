/*============================================================================*/
/*** Copyright (C) 2009-2011, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  
 *  @file       <CanTp.c>
 *  @brief      <canTp configuration file>
 *  
 *  
 *  @author     <Tommy Yu>
 *  @date       <2012-12-28>
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/

#include "CanTp.h"
#include "CanTp_Cfg.h"


/*=======[E X T E R N A L   D A T A]==========================================*/
const CanTp_GeneralType CanTpGeneralConfig =
{
	/* .main_function_period = */ 10,
};

//NSa
const CanTp_NSaType CanTpNSaConfig_RX_vEcuC_Pdu_1 = 
{
	/* .CanTpNSa = */ (uint32)0,
};

//NTa
const CanTp_NTaType CanTpNTaConfig_RX_vEcuC_Pdu_1 = 
{
	/* .CanTpNTa = */ (uint32)0,
};

//NSa
const CanTp_NSaType CanTpNSaConfig_TX_vEcuC_Pdu_1 = 
{
	/* .CanTpNSa = */ (uint32)0,
};

//NTa
const CanTp_NTaType CanTpNTaConfig_TX_vEcuC_Pdu_1 = 
{
	/* .CanTpNTa = */ (uint32)0,
};

const CanTp_NSduType CanTpNSduConfigList[] =
{
	{
		/* .direction = */ ISO15765_RECEIVE,
		/* .listItemType = */ CANTP_NOT_LAST_ENTRY,
		{
			/* .configData.CanTpTxNSdu.CanTp_rxDcmId = */0xCCu,
			/* .configData.CanTpRxNSdu.CanTp_FcPduId = */ 0x01u,
			/* .configData.CanTpRxNSdu.CanIf_FcPduId = */ 0x11u,   /* Relate with CanIf_RxPduConfigType.CanIf_RxPduId */
			/* .configData.CanTpRxNSdu.CanTpAddressingFormant = */ CANTP_STANDARD,//CANTP_EXTENDED,
			/* .configData.CanTpRxNSdu.CanTpBs = */ //250,//4,
			/* .configData.CanTpRxNSdu.CanTpNar = */ //250,
			/* .configData.CanTpRxNSdu.CanTpNbr = */ //250,
			/* .configData.CanTpRxNSdu.CanTpNcr = */ //250,
			
			/* .configData.CanTpRxNSdu.CanTpBs = */ 0xFF,
			/* .configData.CanTpRxNSdu.CanTpNar = */ 1000,
			/* .configData.CanTpRxNSdu.CanTpNbr = */ 1000,
			/* .configData.CanTpRxNSdu.CanTpNcr = */ 1000,
			
			/* .configData.CanTpRxNSdu.CanTpRxChannel = */ CANTP_RX_vEcuC_Pdu_1,
			/* .configData.CanTpRxNSdu.CanTpRxDI = */ 8,
			/* .configData.CanTpRxNSdu.CanTpRxPaddingActivation = */ CANTP_OFF,
			/* .configData.CanTpRxNSdu.CanTpRxTaType = */ CANTP_PHYSICAL,
			/* .configData.CanTpRxNSdu.CanTpWftMax = */ 5,
			/* .configData.CanTpRxNSdu.CanTpSTmin = */ 0,
			/* .configData.CanTpRxNSdu.CanTpNSa = */ &CanTpNSaConfig_RX_vEcuC_Pdu_1,
			/* .configData.CanTpRxNSdu.CanTpNTa = */ &CanTpNTaConfig_RX_vEcuC_Pdu_1,
		}
	},
	{
		/* .direction = */ ISO15765_TRANSMIT,
		/* .listItemType = */ CANTP_END_OF_LIST,
		{
			/* .configData.CanTpTxNSdu.CanTp_txDcmId = */0xAAu,
			/* .configData.CanTpTxNSdu.CanTp_FcPduId = */ 0x11u,
			/* .configData.CanTpTxNSdu.CanIf_PduId = */ 0x01u,  /* Relate with CanIf_TxPduConfigType.CanIf_TxPduId */
			/* .configData.CanTpTxNSdu.CanTpAddressingMode = */ CANTP_STANDARD,//CANTP_EXTENDED,
         /* .configData.CanTpTxNSdu.Reserved_CanTpBs = */ 0xFF,
			/* .configData.CanTpTxNSdu.CanTpNas = */ 250,
			/* .configData.CanTpTxNSdu.CanTpNbs = */ 250,
			/* .configData.CanTpTxNSdu.CanTpNcs = */ 250,
			/* .configData.CanTpTxNSdu.CanTpTxChannel = */ CANTP_TX_vEcuC_Pdu_1,
			/* .configData.CanTpTxNSdu.CanTpTxDI = */ 8,
			/* .configData.CanTpTxNSdu.CanTpTxPaddingActivation = */ CANTP_OFF,
			/* .configData.CanTpTxNSdu.CanTpTxTaType = */ CANTP_PHYSICAL,
			/* .configData.CanTpTxNSdu.Reserved_CanTpWftMax = */ 0xFF,
			/* .configData.CanTpTxNSdu.Reserved_CanTpSTmin = */ 0xFFFF,
			/* .configData.CanTpTxNSdu.CanTpNSa = */ &CanTpNSaConfig_TX_vEcuC_Pdu_1,
			/* .configData.CanTpTxNSdu.CanTpNTa = */ &CanTpNTaConfig_TX_vEcuC_Pdu_1,
		}
	},
};

const CanTp_RxIdType CanTp_RxIdList[] = 
{
	/* RX_vEcuC_Pdu_1 */
	{
		/* .CanTpAddressingMode =  */CANTP_EXTENDED,
		/* .CanTpNSduIndex =  */0,
		/* .CanTpReferringTxIndex = */ 1,
	},
	/* TX_vEcuC_Pdu_1 */
	{
		/* .CanTpAddressingMode =  */CANTP_EXTENDED,
		/* .CanTpNSduIndex =  */1,
		/* .CanTpReferringTxIndex = */ 0xFFFF,
	},
};

extern const CanTp_ConfigType CanTpConfig =
{
	/* .CanTpGeneral 	= */ &CanTpGeneralConfig,
	/* .CanTpNSduList = */	 CanTpNSduConfigList,
	/* .CanTpRxIdList = */   CanTp_RxIdList
};

/***=======[R E V I S I O N   H I S T O R Y]====================================*/
/**** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121228    Tommy Yu    Initial version
 * 
 */
/***=======[E N D   O F   F I L E]==============================================*/


