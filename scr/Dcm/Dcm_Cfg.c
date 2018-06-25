/*============================================================================*/
/** Copyright (C) 2009-2011, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  @file       <Dcm_Cfg.c>
 *  @brief      <UDS Service - ISO14229>
 *  
 *  <This Diagnostic Communication Manager file contained UDS services
 *   which used for bootloader project>
 *  @author     <Gary Chen>
 *  @date       <2012-11-09>
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "Dcm_Types.h"
#include "Dcm_Dsp.h"
#include "Dcm_Dsl.h"
#include "Dcm_Internel.h"

/*=======[E X T E R N A L   D A T A]==========================================*/
const Dcm_ServiceTableType Dcm_ServiceTable[DCM_SERVICE_NUM] = 
{
    /* SID, funcAddrSupport,  sessionSupp,      ServiceFct,  pendingFct */
    /* @type:uint8 range:0x00~0xFF note:select unique service Id */
             /* @type:boolean range:TRUE,FALSE note:NONE */
                               /* @type:uint8 range:session value note:select defined session value */
                                                    /* @type:API range:NONE note:auto generate */
                                                                   /* @type:API range:NONE note:auto generate */
    {
    	M_U8_CMNSRV_START_SESSION_SID,  									 /* SID: DiagnosticSessionControl (10 hex) service */
		TRUE,             													 /* funcAddrSupport :TRUE or FALSE*/
		{DCM_SESSION_DEFAULT,DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND},    /*sessionSupp:select defined session value */
		&Dcm_RecvMsg10,														 /*API*/
		NULLPTR
	},
    {
    	M_U8_CMNSRV_ECU_RESET_SID,  										 /* SID: ECUReset (11 hex) service */  
		TRUE,             													 /* funcAddrSupport :TRUE or FALSE*/             
		{DCM_SESSION_DEFAULT,DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND},    /*sessionSupp:select defined session value */ 
		&Dcm_RecvMsg11,														 /*API*/
		NULLPTR
	},
    {
    	M_U8_CMNSRV_READ_DATA_SID,  										 /* SID: ReadDataByIdentifier (22 hex) service */  
		FALSE,             													 /* funcAddrSupport :TRUE or FALSE*/            
		{DCM_SESSION_DEFAULT,DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND},    /*sessionSupp:select defined session value */ 
		&Dsp_UdsReadDataByIdentifier,//&Dcm_RecvMsg22						 /*API*/
		NULLPTR
	},
    {
    	M_U8_CMNSRV_SECURITY_ACCESS_SID,  									 /* SID: SecurityAccess (27 hex) service */  
		FALSE,             													 /* funcAddrSupport :TRUE or FALSE*/            
		{DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND,DCM_SESSION_EXTEND},     /*sessionSupp:select defined session value */ 
		&Dcm_RecvMsg27,														 /*API*/
		NULLPTR
	},
    {
    	M_U8_CMNSRV_TRANSMIT_CONTROL_SID,  									 /* SID: CommunicationControl (28 hex) service */  
		TRUE,             													 /* funcAddrSupport :TRUE or FALSE*/             
		{DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND,DCM_SESSION_EXTEND},     /*sessionSupp:select defined session value */ 
		&Dcm_RecvMsg28,														 /*API*/
		NULLPTR
	},
    {
    	M_U8_CMNSRV_WRITE_DATA_SID,   										 /* SID: WriteDataByIdentifier (2E hex) service */ 
		FALSE,             													 /* funcAddrSupport :TRUE or FALSE*/            
		{DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND,DCM_SESSION_EXTEND},     /*sessionSupp:select defined session value */ 
		&Dcm_RecvMsg2E,														 /*API*/
		&Dcm_Pending2E
	},
    {
    	M_U8_CMNSRV_ROUTINE_CONTROL_SID,  									 /* SID: RoutineControl (31 hex) service */  
		TRUE,             													 /* funcAddrSupport :TRUE or FALSE*/             
		{DCM_SESSION_DEFAULT,DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND},    /*sessionSupp:select defined session value */ 
		&Dcm_RecvMsg31,														 /*API*/
		&Dcm_Pending31
	},
    {
    	M_U8_CMNSRV_REQUEST_DOWNLOAD_SID,  									 /* SID: RequestDownload (34 hex) service */ 
		FALSE,             													 /* funcAddrSupport :TRUE or FALSE*/            
		{DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND,DCM_SESSION_EXTEND}, 
		&Dcm_RecvMsg34,														 /*API*/
		NULLPTR
	},
    {
    	M_U8_CMNSRV_TRANSFER_DATA_SID,  									 /* SID: TransferData (36 hex) service */  
		FALSE,             													 /* funcAddrSupport :TRUE or FALSE*/            
		{DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND,DCM_SESSION_EXTEND},     /*sessionSupp:select defined session value */ 
		&Dcm_RecvMsg36,														 /*API*/
		&Dcm_Pending36
	},
    {
    	M_u8_CMNSRV_REQUEST_TRANSFER_EXIT,  								 /* SID: RequestTransferExit (37 hex) service */  
		FALSE,             													 /* funcAddrSupport :TRUE or FALSE*/            
		{DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND,DCM_SESSION_EXTEND},     /*sessionSupp:select defined session value */ 
		&Dcm_RecvMsg37,														 /*API*/
		NULLPTR
	},
    {
    	M_U8_CMNSRV_TESTER_PRESENT_SID,  									 /* SID: TesterPresent (3E hex) service */  
		TRUE,             													 /* funcAddrSupport :TRUE or FALSE*/             
		{DCM_SESSION_DEFAULT,DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND},    /*sessionSupp:select defined session value */ 
		&Dcm_RecvMsg3E,														 /*API*/
		NULLPTR
	},
    {
    	M_U8_CMNSRV_CONTROL_DTC_SETTING,  									 /* SID: ControlDTCSetting (85 hex) service */  
		TRUE,             													 /* funcAddrSupport :TRUE or FALSE*/             
		{DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND,DCM_SESSION_EXTEND},     /*sessionSupp:select defined session value */ 
		&Dcm_RecvMsg85,														 /*API*/
		NULLPTR
	},
};

const Dcm_SessionRowType Dcm_SessionRow[DCM_SESSION_NUM] =
{
    {
    	DCM_SESSION_DEFAULT,    											/*sessionType:select unique session value */ 
		{DCM_SESSION_DEFAULT,DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND},	/*sessionSupp:select defined session value */
		{DCM_SECURITY_LOCKED,DCM_SECURITY_LEV1,DCM_SECURITY_LEV1}			/*securitySupp:select defined security value*/
	},
    {
    	DCM_SESSION_PROGRAMMING,											/*sessionType:select unique session value */
		{DCM_SESSION_DEFAULT,DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND},	/*sessionSupp:select defined session value */
		{DCM_SECURITY_LOCKED,DCM_SECURITY_LEV1,DCM_SECURITY_LEV1}			/*securitySupp:select defined security value*/
	},
    {
    	DCM_SESSION_EXTEND,     											/*sessionType:select unique session value */
		{DCM_SESSION_DEFAULT,DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND},	/*sessionSupp:select defined session value */
		{DCM_SECURITY_LOCKED,DCM_SECURITY_LEV1,DCM_SECURITY_LEV1}			/*securitySupp:select defined security value*/
	},
};

const Dcm_ResetRowType Dcm_ResetRow[DCM_RESET_NUM] =
{
    {
    	DCM_HARD_RESET,        												/*sub-function :resetType */
		{DCM_SESSION_DEFAULT,DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND},	/*sessionSupp:select defined session value */
		{DCM_SECURITY_LOCKED,DCM_SECURITY_LEV1,DCM_SECURITY_LEV1}			/*securitySupp:select defined security value*/
	},
};

#if(DCM_READDID_NUM > 0)
const Dcm_ReadDidRowType Dcm_ReadDidRow[DCM_READDID_NUM] = 
{
    {
	    0xF15Bu,															/* DID, securitySupp,readDataFct, @type:uint16 range:0x0000~0xFFFF note:NONE */  
		{DCM_SECURITY_LOCKED,DCM_SECURITY_LEV1,DCM_SECURITY_LEV1}, 		 	/* @type:uint8 range:security value note:select defined security value */
		&FL_ReadDid															/* @type:API range:NONE note:input APIname */
	},
};
#endif

const Dcm_SecurityRowType Dcm_SecurityRow[DCM_SECURITY_NUM] = 
{
	{
		M_U8_CMNSRV_SECURITY_LEVEL1_SEED,    								/* seedId:type:uint8 range:0x00~0xFF */
		M_U8_CMNSRV_SECURITY_LEVEL1_KEY,     								/* keyId:type:uint8 range:0x00~0xFF  */
		DCM_SECURITY_LEV1,  					 						 	/* secAccessLevel:type:define range:DCM_SECURITY_LEV1~DCM_SECURITY_LEV2 */
		{DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND,DCM_SESSION_EXTEND},	/* sessionSupp :type:uint8 range:session value note:select defined session value*/
		{DCM_SECURITY_LOCKED,DCM_SECURITY_LEV1,DCM_SECURITY_LEV1},      	/* securitySupp:type:uint8 range:security value note:select defined security value */
		&SecM_GenerateSeed,													/* type:API range:NONE note:input APIname */
		&SecM_CompareKey   													/* type:API range:NONE note:input APIname */
	},
	{
		M_U8_CMNSRV_SECURITY_LEVEL2_SEED,    								/* seedId:type:uint8 range:0x00~0xFF */
		M_U8_CMNSRV_SECURITY_LEVEL2_KEY,     								/* keyId:type:uint8 range:0x00~0xFF  */
		DCM_SECURITY_LEV2,						  							/* secAccessLevel:type:define range:DCM_SECURITY_LEV1~DCM_SECURITY_LEV2 */
		{DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND,DCM_SESSION_EXTEND},	/* sessionSupp :type:uint8 range:session value note:select defined session value*/
		{DCM_SECURITY_LOCKED,DCM_SECURITY_LEV2,DCM_SECURITY_LEV2},      	/* securitySupp:type:uint8 range:security value note:select defined security value */
		&SecM_SecurityAccess_Level2Seed,									/* @type:API range:NONE note:input APIname */
		&SecM_SecurityAccess_Level2Key  									/* @type:API range:NONE note:input APIname */
	},
};

#if (DCM_COM_CONTROL_NUM > 0)
const Dcm_ComControlRowType Dcm_ComControlRow[DCM_COM_CONTROL_NUM] = 
{
    {
    	0x00u,        														/* controlType */
		{DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND,DCM_SESSION_EXTEND},	/*sessionSupp:select defined session value */
		{DCM_SECURITY_LOCKED,DCM_SECURITY_LEV1,DCM_SECURITY_LEV1}			/*securitySupp:select defined security value*/
	},
    {
    	0x01u,        														/* controlType */        
		{DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND,DCM_SESSION_EXTEND},	/*sessionSupp:select defined session value */
		{DCM_SECURITY_LOCKED,DCM_SECURITY_LEV1,DCM_SECURITY_LEV1}			/*securitySupp:select defined security value*/
	},
};
#endif

const Dcm_WriteDidRowType Dcm_WriteDidRow[DCM_WRITEDID_NUM] = 
{
    {
    	DCM_FINGER_PRINT_DATA, 										/*DID*/
		0x11u,      												/*dataLength*/
		{DCM_SECURITY_LEV1,DCM_SECURITY_LEV1,DCM_SECURITY_LEV1},	/*securitySupp:select defined security value*/
		&FL_WriteFingerPrint										/*input API name*/
	},
};

const Dcm_RoutineControlRowType Dcm_RoutineControlRow[DCM_ROUTINE_CONTROL_NUM] = 
{
    {
    	0x0203u,    														/* routineId:select unique routine Id*/
		0x00u,         														/*optionLength*/
		TRUE,        														/*funcAddrSupp*/
		{DCM_SESSION_DEFAULT,DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND},	/*sessionSupp:select defined session value */
		{DCM_SECURITY_LOCKED,DCM_SECURITY_LEV1,DCM_SECURITY_LEV1},			/*securitySupp:select defined security value*/
		&Dcm_CheckProgPreCondition											/*routineControl function*/
	},
    {
    	0xFF00u,    														/* routineId:select unique routine Id*/
		0x02u,         														/*optionLength*/
		FALSE,       														/*funcAddrSupp*/
		{DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND,DCM_SESSION_EXTEND},	/*sessionSupp:select defined session value */
		{DCM_SECURITY_LEV1,DCM_SECURITY_LEV1,DCM_SECURITY_LEV1},			/*securitySupp:select defined security value*/
		&Dcm_EraseMemory													/*routineControl function*/
	},
    {
    	0x0202u,     														/* routineId:select unique routine Id*/   
		0x04u,         														/*optionLength*/         
		FALSE,       														/*funcAddrSupp*/       
		{DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND,DCM_SESSION_EXTEND},	/*sessionSupp:select defined session value */
		{DCM_SECURITY_LEV1,DCM_SECURITY_LEV1,DCM_SECURITY_LEV1},			/*securitySupp:select defined security value*/
		&Dcm_CheckMemory													/*routineControl function*/
	},
    {
    	0xFF01u,     														/* routineId:select unique routine Id*/       
		0x00u,         														/*optionLength*/                  
		FALSE,       														/*funcAddrSupp*/           
		{DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND,DCM_SESSION_EXTEND},	/*sessionSupp:select defined session value */
		{DCM_SECURITY_LEV1,DCM_SECURITY_LEV1,DCM_SECURITY_LEV1},			/*securitySupp:select defined security value*/
		&Dcm_CheckProgDependencies											/*routineControl function*/
	},
};

const Dcm_DownloadRowType Dcm_DownloadRow = 
{
    /* securitySupp */
    /* @type:uint8 range:security value note:select defined security value */
    {DCM_SECURITY_LEV1,DCM_SECURITY_LEV1,DCM_SECURITY_LEV1},
};

#if (DCM_DTC_SET_NUM > 0)
const Dcm_DTCSettingRowType Dcm_DTCSettingRow[DCM_DTC_SET_NUM] = 
{
    {
	    0x01u,    															/*DTCSet*/
		{DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND,DCM_SESSION_EXTEND},	/*sessionSupp:select defined session value*/
		{DCM_SECURITY_LOCKED,DCM_SECURITY_LEV1,DCM_SECURITY_LEV1}			/*securitySupp:select defined security value*/
	},
    {
    	0x02u,    															/*DTCSet*/
		{DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND,DCM_SESSION_EXTEND},	/*sessionSupp:select defined session value*/
		{DCM_SECURITY_LOCKED,DCM_SECURITY_LEV1,DCM_SECURITY_LEV1}			/*securitySupp:select defined security value*/
	},
};
#endif

const Dcm_DspDidType DspDidList[] = 
{ 
	{ 
		/* DcmDspDidUsed = */ B_TRUE,
		/* DcmDspDidIdentifier = */ 0xF15Bu,
		/* DcmDspDidSize = */0xFFu,
		/* DcmDspDidReadDataFnc = */ &FL_ReadDid,
		/* DcmDspDidWriteDataFnc = */ NULL,
		/* securitySupp[DCM_SECURITY_SUPPORT_NUM] = */ {DCM_SECURITY_LOCKED,DCM_SECURITY_LEV1,DCM_SECURITY_LEV1},
		/* sessionSupp[DCM_SESSION_SUPPORT_NUM] = */   {DCM_SESSION_DEFAULT,DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND},
		/* Arc_EOL = */ FALSE
	},
	{ 
		/* DcmDspDidUsed = */ B_TRUE,
		/* DcmDspDidIdentifier = */ 0xF190u,
		/* DcmDspDidSize = */0xFFu,
		/* DcmDspDidReadDataFnc = */ &Dsp_ReadVIN,
		/* DcmDspDidWriteDataFnc = */ NULL,
		/* securitySupp[DCM_SECURITY_SUPPORT_NUM] = */ {DCM_SECURITY_LOCKED,DCM_SECURITY_LEV1,DCM_SECURITY_LEV1},
		/* sessionSupp[DCM_SESSION_SUPPORT_NUM] = */   {DCM_SESSION_DEFAULT,DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND},
		/* Arc_EOL = */ FALSE
	},
	{ 
		/* DcmDspDidUsed = */ B_TRUE,
		/* DspDidIdentifier = */ 0xF15Au,
		/* DcmDspDidSize = */0x11u,
		/* DcmDspDidReadDataFnc = */ NULL,
		/* DcmDspDidWriteDataFnc = */ &FL_WriteFingerPrint,
		/* securitySupp[DCM_SECURITY_SUPPORT_NUM] = */ {DCM_SECURITY_LEV1,DCM_SECURITY_LEV1,DCM_SECURITY_LEV1},
		/* sessionSupp[DCM_SESSION_SUPPORT_NUM] = */   {DCM_SESSION_PROGRAMMING,DCM_SESSION_EXTEND,DCM_SESSION_EXTEND},
		/* Arc_EOL = */ FALSE
	},
	{ 
		/* DcmDspDidUsed = */ B_FALSE,
		/* DspDidIdentifier = */ 0xFFFF,
		/* DcmDspDidSize = */0xFFu,
		/* DcmDspDidReadDataFnc = */ NULL,
		/* DcmDspDidWriteDataFnc = */ NULL,
		/* securitySupp[DCM_SECURITY_SUPPORT_NUM] = */ NULL,
		/* sessionSupp[DCM_SESSION_SUPPORT_NUM] = */NULL,
		/* Arc_EOL = */ TRUE
	}
};

const Dcm_DspSessionRowType DspSessionList[] = 
{
	{ 
		/* vSession_default */
		/* DcmDspSessionLevel = */ 1,
		/* DcmDspSessionP2ServerMax = */ 40,
		/* DcmDspSessionP2StarServerMax = */ 4000,
		/* DcmDspSessionS3Server*/ 5000,
		/* Arc_EOL = */ B_FALSE
	},
	{ 
	   /* vSession_program */
		/* DcmDspSessionLevel = */ 2,
		/* DcmDspSessionP2ServerMax = */ 40,
		/* DcmDspSessionP2StarServerMax = */ 4000,
		/* DcmDspSessionS3Server*/ 5000,
		/* Arc_EOL = */ B_FALSE
	},
	{ 
	   /* vSession_extended */
		/* DcmDspSessionLevel = */ 3,
		/* DcmDspSessionP2ServerMax = */ 40,
		/* DcmDspSessionP2StarServerMax = */ 4000,
		/* DcmDspSessionS3Server*/ 5000,
		/* Arc_EOL = */ B_FALSE
	},

	{ 
		//Dummy Session For EOL
		/* DcmDspSessionLevel = */ 0xFF,
		/* DcmDspSessionP2ServerMax = */ 0xFF,
		/* DcmDspSessionP2StarServerMax = */ 0xFF,
		/* DcmDspSessionS3Server*/ 0,
		/* Arc_EOL = */ B_TRUE
	},
};

const Dcm_DspSecurityRowType DspSecurityList[] = 
{
	{ 
		/* vSecurityLevel_Locked */
		/* DcmDspSecurityLevel = */ DCM_SEC_LEV_LOCKED,
		/* DcmDspSecurityDelayTimeOnBoot = */ 0,          /* Value is not configurable */
		/* DcmDspSecurityNumAttDelay = */ 0, 	           /* Value is not configurable */
		/* DcmDspSecurityDelayTime = */ 0, 	              /* Value is not configurable */
		/* DcmDspSecurityADRSize = */ 0,
		/* DcmDspSecuritySeedSize = */ 2,
		/* DcmDspSecurityKeySize = */ 2,
		/* GetSeed = */ vSecurityLevel_Locked_GetSeed,
		/* CompareKey = */ vSecurityLevel_Locked_CompKey,
		/* Arc_EOL = */ B_FALSE
	},
	{ 
	   /* vSecurityLevel_1 */
		/* DcmDspSecurityLevel = */ DCM_SEC_LEV_L1,
		/* DcmDspSecurityDelayTimeOnBoot = */ 0,          /* Value is not configurable */
		/* DcmDspSecurityNumAttDelay = */ 0, 	           /* Value is not configurable */
		/* DcmDspSecurityDelayTime = */ 0, 	              /* Value is not configurable */
		/* DcmDspSecurityADRSize = */ 0,
		/* DcmDspSecuritySeedSize = */ 2,
		/* DcmDspSecurityKeySize = */ 2,
		/* GetSeed = */ vSecurityLevel_1_GetSeed,
		/* CompareKey = */ vSecurityLevel_1_CompKey,
		/* Arc_EOL = */ B_FALSE
	},

	{ 
		// Dummy For EOL
		/* DcmDspSecurityLevel = */ 0xFF,
		/* DcmDspSecurityDelayTimeOnBoot = */ 0,/* Value is not configurable */
		/* DcmDspSecurityNumAttDelay = */ 0, 	/* Value is not configurable */
		/* DcmDspSecurityDelayTime = */ 0, 	/* Value is not configurable */
		/* DcmDspSecurityADRSize = */ 0xFF,
		/* DcmDspSecuritySeedSize = */ 0xFF,
		/* DcmDspSecurityKeySize = */ 0xFF,
		/* GetSeed = */ NULLPTR,
		/* CompareKey = */ NULLPTR,
		/* Arc_EOL = */ B_TRUE
}
};

const Dcm_DspSessionRowType *vDcmDidInfo_1_read_sessionRefList[] = 
{
	&DspSessionList[DCM_SESSION_DEFAULT_INDEX],//vSession_default
	&DspSessionList[DCM_SESSION_EXTENDED_INDEX],//vSession_extended
	&DspSessionList[DCM_SESSION_EOL_INDEX]
};

const Dcm_DspSecurityRowType *vDcmDidInfo_1_read_securityRefList[] = 
{
	&DspSecurityList[DCM_SEC_LEV_LOCKED],      //vSecurityLock
	&DspSecurityList[DCM_SEC_LEV_L1],          //vSecurityLevel_1
	&DspSecurityList[DCM_SECURITY_EOL_INDEX]
};

const Dcm_DspDidReadType vDcmDidInfo_1_didRead = 
{
	/* DcmDspDidReadSessionRef = */ vDcmDidInfo_1_read_sessionRefList,
	/* DcmDspDidReadSecurityLevelRef = */ vDcmDidInfo_1_read_securityRefList
};

const Dcm_DspSessionRowType *vDcmDidInfo_1_write_sessionRefList[] = 
{
	&DspSessionList[DCM_SESSION_EXTENDED_INDEX], //vSession_default
	&DspSessionList[DCM_SESSION_EOL_INDEX]
};

const Dcm_DspSecurityRowType *vDcmDidInfo_1_write_securityRefList[] = 
{
	&DspSecurityList[DCM_SEC_LEV_L1],          //vSecurityLevel_1
	&DspSecurityList[DCM_SECURITY_EOL_INDEX]
};

const Dcm_DspDidWriteType vDcmDidInfo_1_didWrite = 
{
   /* DspDidReadSessionRef = */ vDcmDidInfo_1_write_sessionRefList,
   /* DspDidReadSecurityLevelRef = */ vDcmDidInfo_1_write_securityRefList
};

const Dcm_DspSessionRowType *vDcmDidInfo_1_control_sessionRefList[] = 
{
	&DspSessionList[DCM_SESSION_DEFAULT_INDEX],//vSession_default
	&DspSessionList[DCM_SESSION_EOL_INDEX]
};

const Dcm_DspSecurityRowType *vDcmDidInfo_1_control_securityRefList[] =
{
	&DspSecurityList[DCM_SEC_LEV_L1],//vSecurityLevel_1
	&DspSecurityList[DCM_SECURITY_EOL_INDEX]
};

const Dcm_DspDidControlType vDcmDidInfo_1_didControl = 
{
    /* DspDidReadSessionRef = */ vDcmDidInfo_1_control_sessionRefList,
    /* DspDidReadSecurityLevelRef = */ vDcmDidInfo_1_control_securityRefList,
};

const Dcm_DslDiagRespType DiagResp = 
{
    /* DcmDslDiagRespMaxNumRespPend = */ 10
};

uint8 vDcmBuffer_1_RX[512];
const Dcm_DslProtocolRxType DcmDslProtocolRxList[] = 
{
	{
		// vProtocol_1->vConnection0->vRxChannel0
		/* DslProtocolAddrType = */ DCM_PROTOCOL_PHYSICAL_ADDR_TYPE,
		/* DcmDslProtocolRxPduId = */ PDUR_RX_vEcuC_Pdu_1,
      /* DcmDslProtocolRxPduRef = */
		{ 
			/* pduInfo */
			/* SduDataPtr = */ vDcmBuffer_1_RX,
			/* SduLength = */ 512,
		},
		/* Arc_EOL = */ TRUE
	}
};

uint8 vDcmBuffer_1_TX[512];
const Dcm_DslProtocolTxType DcmDslProtocolTxList[] = 
{
	{
		// vProtocol_1->vConnection0->vTxChannel0
		/* DcmDslTxConfirmationPduId = */ PDUR_TX_vEcuC_Pdu_1,
		/* DcmDslProtocolTxPduRef = */
		{ 
			/* pduInfo */
			/* SduDataPtr = */ vDcmBuffer_1_TX,
			/* SduLength = */ 512,
		},
		/* Arc_EOL = */ TRUE
	}
};

const Dcm_DslMainConnectionType DcmDslMainConnectionList[] = 
{
	{
		//vProtocol_1->vConnection0
		/* DcmDslProtocolRxTesterSourceAddr = */ 0x00,
		/* DslProtocolRx = */ &DcmDslProtocolRxList[0],						
		/* DslProtocolTx = */ &DcmDslProtocolTxList[0]
	}
};

const Dcm_DslConnectionType DslConnectionList[] = 
{
	{
		//vProtocol_1->vConnection0
		/* DcmDslMainConnection = */ &DcmDslMainConnectionList[0],
		/* Arc_EOL = */ TRUE
	}
};

const Dcm_DslBufferType DcmDslBufferList[] = 
{
	{
		/*RxBuffer*/
		/*DcmDslBufferSize = */ 512
	},
	{
		/*TxBuffer*/
		/*DcmDslBufferSize = */ 512
	}
};

const Dcm_DsdServiceTableType DsdServiceTable[] = 
{	
	{ 
		/* vServiceTable_1 */
		/* DsdSidTabId = */ 0,
		/* DsdService = */ Dcm_ServiceTable,
		/* Arc_EOL = */ TRUE
	}
};

const Dcm_DslProtocolTimingRowType ProtocolTimingList[] = 
{
	{
		//vTiming_1
		/* TimStrP2ServerMax = */ 5000,
		/* TimStrP2StarServerMax = */ 40,		
		/* TimStrS3Server = */ 5000,
		/* Arc_EOL = */ FALSE
	},
};

extern const Dcm_DslProtocolRowType DcmDslProtocolRowList = 
{
	/* DcmDslProtocolID = */ DCM_UDS_ON_CAN,
   /* DcmDslProtocolRxBufferRef = */ &DcmDslBufferList[0],//vDcmBuffer_1_RX
	/* DcmDslProtocolTxBufferRef = */ &DcmDslBufferList[1],//vDcmBuffer_1_TX
	/* DcmDslProtocolSIDTable = */ DsdServiceTable,
	/* DcmDslConnection = */DslConnectionList,
	/* DcmDslProtocolTimeLimit*/ ProtocolTimingList,
	/* Arc_EOL = */ TRUE
};

const Dcm_DslProtocolType DslProtocol = 
{
    /* DcmDslProtocolRow = */ &DcmDslProtocolRowList
};

const Dcm_DspDidInfoType DspDidInfoList[] = 
{
	{ 
		 /* DcmDspDidDynamicllyDefined = */ //FALSE,
		 /* DcmDspDidRead = */ &vDcmDidInfo_1_didRead,
		 /* DcmDspDidWrite = */ &vDcmDidInfo_1_didWrite,
		 /* DcmDspDidControl = */ &vDcmDidInfo_1_didControl,
	}, 
};

const Dcm_DslCallbackDCMRequestServiceType DCMRequestServiceList[] = 
{
	{ 
		// vRequestService_1
		/* StartProtocol = */ vRequestService_1_Start,
		/* StopProtocol = */ vRequestService_1_Stop,
		/* Arc_EOL = */ TRUE
	}
};

const Dcm_DspType DcmDsp = 
{
    /* DcmDspMaxDidToRead = */ 0xFF,	/* To be define */
    /* DcmDspDid = */ DspDidList,
    /* DcmDspDidInfo = */ DspDidInfoList,
    /* DcmDspPid = */ NULL,
};

const Dcm_DsdType DcmDsd = 
{
    /* DcmDsdServiceTable = */ DsdServiceTable,  
};

const Dcm_DslType DcmDsl = 
{
    /* DcmDslBuffer = */ DcmDslBufferList,
    /* DcmDslDiagResp = */ &DiagResp,
    /* DcmDslProtocol = */ &DslProtocol,
};

const Dcm_ConfigSetType DcmConfigSet =
{
	/* DcmDsp = */ &DcmDsp,
	/* DcmDsd = */ &DcmDsd,
	/* DcmDsl = */ &DcmDsl
};

const Dcm_GeneralType DcmGeneral = 
{
	/* DcmDDDIDStorage = */ FALSE,
	/* DcmDevErrorDetect = */FALSE,
	/* DcmRespondAllRequest = */TRUE,
	/* DcmVersionInfoApi = */FALSE
};

extern const Dcm_ConfigType Dcm_Config =
{
	/* DcmConfigSet = */ &DcmConfigSet,
	/* DcmGeneral   = */ &DcmGeneral
};

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121109    Gary       Initial version
 */
/*=======[E N D   O F   F I L E]==============================================*/
