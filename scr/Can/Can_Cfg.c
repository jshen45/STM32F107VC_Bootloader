/*=======[I N C L U D E S]====================================================*/
#include "Can.h"
//#include "Can_Cfg.h"

/** Can transmit Pdu configuration*/
const CanTxPduType Can_TxPdu[CAN_TX_PDU_NUM] = 
{
	{
        /* tx pduId */
        /* @type:uint16 range:0x0000~0xFFFF note:auto generate */
        0x01u,
        
        /* canId */
        /* @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE */
		//0x18DA2F00u,
		CAN_TXID,
		/* controller idx*/
		CAN_CONTROLLER_A,
		/* standardFrame or not */
        /* @type:boolean range:TRUE,FALSE note:NONE */
		FALSE,
		
        /* Can_TxMailBoxMask,buffer 1-31 can be selected*/
        /* @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE */
        0x00000200uL,  

	},
};
