/*============================================================================*/
/** Copyright (C) 2009-2011, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  @file       <Dcm_Types.h>
 *  @brief      <Macro and function decalrations for Dcm Module>
 *  
 *  
 *  @author     <Gary Chen>
 *  @date       <2012-11-09>
 */
/*============================================================================*/
#ifndef DCM_TYPES_H
#define DCM_TYPES_H

/*=======[I N C L U D E S]====================================================*/
#include "SecM.h"
#include "Dcm_Cfg.h"
#include "Rte_Dcm_Type.h"

/*=======[T Y P E   D E F I N I T I O N S]====================================*/
/** service function type */
typedef void (*Dcm_ServiceFct) (const Dcm_BuffType * rxBuff,Dcm_BuffType * txBuff);

/** service pending function type */
typedef void (*Dcm_PendingFct) (const FL_ResultType errorCode,\
                                const Dcm_BuffType * rxBuff,\
                                Dcm_BuffType * txBuff);

/** service table parameter config */
typedef struct
{ 
    const uint8 SID;                  /* service Id */
    const boolean funcAddrSupportted; /* if function address is supportted */    
    const Dcm_SessionType sessionSupp[DCM_SESSION_SUPPORT_NUM];/* supportted session mode */
    const Dcm_ServiceFct serviceFct;  /* service function */
    const Dcm_PendingFct pendingFct;  /* service pending function */
}Dcm_ServiceTableType;

/** 0x10 service parameter config */
typedef struct
{
	/* session mode type */
    const Dcm_SessionType sessionType;
    
    /* supportted session mode */
    const Dcm_SessionType sessionSupp[DCM_SESSION_SUPPORT_NUM];
    
    /* supportted security level */
    const Dcm_SecurityType securitySupp[DCM_SECURITY_SUPPORT_NUM]; 
}Dcm_SessionRowType;

/** 0x11 service parameter config */
typedef struct
{
	/* reset mode type */
    const Dcm_ResetType resetType;
    
    /* supportted session mode */
    const Dcm_SessionType sessionSupp[DCM_SESSION_SUPPORT_NUM];
    
    /* supportted security level */
    const Dcm_SecurityType securitySupp[DCM_SECURITY_SUPPORT_NUM]; 
}Dcm_ResetRowType;


#if(DCM_READDID_NUM > 0)
/** read data function type */
typedef uint16 (*Dcm_ReadDataFct) (uint8 *readData);

/** 0x22 service parameter config */
typedef struct
{
	/* read data Id */
    const uint16 DID;
    
    /* supportted security level */
    const Dcm_SecurityType securitySupp[DCM_SECURITY_SUPPORT_NUM]; 
    
    /* read data funtion */ 
    const Dcm_ReadDataFct readDataFct;
}Dcm_ReadDidRowType;
#endif

/** generate seed function type */
typedef SecM_StatusType (*Dcm_GenerateSeedFct)(SecM_SeedType *seed);

/** compare key function type */
typedef SecM_StatusType (*Dcm_CompareKeyFct)(SecM_KeyType key,SecM_SeedType seed);

/** 0x27 service parameter config */
typedef struct
{
	/* request seed subfunction Id */
    const uint8 reqSeedId;
    
    /* send key subfunction Id */
    const uint8 sendKeyId;
    
    /* security access level */
    const Dcm_SecurityType secAccessLevel;
    
    /* supportted session mode */
    const Dcm_SessionType sessionSupp[DCM_SESSION_SUPPORT_NUM];
    
    /* supportted security level */
    const Dcm_SecurityType securitySupp[DCM_SECURITY_SUPPORT_NUM];
    
    /* generate seed function */
    const Dcm_GenerateSeedFct generateSeed;
    
    /* compare key function */
    const Dcm_CompareKeyFct compareKey;
}Dcm_SecurityRowType;

#if (DCM_COM_CONTROL_NUM > 0)
/** 0x28 service parameter config */
typedef struct
{
	 /* communication control type */
    const Dcm_ComControlType controlType;
    
    /* supportted session mode */
    const Dcm_SessionType sessionSupp[DCM_SESSION_SUPPORT_NUM];
    
    /* supportted security level */
    const Dcm_SecurityType securitySupp[DCM_SECURITY_SUPPORT_NUM];
}Dcm_ComControlRowType;
#endif

/** write data function type */
typedef FL_ResultType (*Dcm_WriteDataFct) (const uint8 *data,const uint16 length);

/* 0x2E service parameter config */
typedef struct
{
	 /* write data Id */
    const uint16 DID;
    
    /* write length */
    const uint16 dataLength;
    
    /* supportted security level */
    const Dcm_SecurityType securitySupp[DCM_SECURITY_SUPPORT_NUM];  
    
    /* write data funtion */
    const Dcm_WriteDataFct writeDataFct;
}Dcm_WriteDidRowType;

/** routine control function type */
typedef void (*Dcm_RoutineControlFct) (const Dcm_BuffType * rxBuff,Dcm_BuffType * txBuff);

/** 0x31 service parameter config */
typedef struct
{
	 /* routine Id */
    const uint16 routineId;
    
    /* routine option length */
    const uint8 optionLength;
    
    /* if function address supportted */
    const boolean funcAddrSupp;
    
    /* supportted session mode */
    const Dcm_SessionType sessionSupp[DCM_SESSION_SUPPORT_NUM];
    
    /* supportted security level */
    const Dcm_SecurityType securitySupp[DCM_SECURITY_SUPPORT_NUM];
    
    /* routine control funtion */      
    const Dcm_RoutineControlFct routineControl;
}Dcm_RoutineControlRowType;

/** 0x34 0x36 0x37 service parameter config */
typedef struct
{
	 /* supportted security level */
    const Dcm_SecurityType securitySupp[DCM_SECURITY_SUPPORT_NUM];  
}Dcm_DownloadRowType;

#if (DCM_DTC_SET_NUM > 0)
/** 0x85 service parameter config */
typedef struct
{
	 /* DTC setting type */
    const Dcm_DTCSettingType DTCSet;
    
    /* supportted session mode */
    const Dcm_SessionType sessionSupp[DCM_SESSION_SUPPORT_NUM];
    
    /* supportted security level */
    const Dcm_SecurityType securitySupp[DCM_SECURITY_SUPPORT_NUM];
}Dcm_DTCSettingRowType;
#endif

// SecurityAccess_<LEVEL>
typedef Std_ReturnType (*Dcm_CallbackGetSeedFncType)(uint8 *seed);
typedef Std_ReturnType (*Dcm_CallbackCompareKeyFncType)(uint8 *key);

// CallBackDCMRequestServices
typedef Std_ReturnType (*Dcm_CallbackStartProtocolFncType)(Dcm_ProtocolType protocolID);
typedef Std_ReturnType (*Dcm_CallbackStopProtocolFncType)(Dcm_ProtocolType protocolID);

// DidServices_<DID>
typedef uint16 (*Dcm_CallbackReadDataFncType)(uint8 *data);
typedef Std_ReturnType (*Dcm_CallbackWriteDataFncType)(const uint8 *data, const uint16 dataLength);

/* Autosar DCM v4.2.2, 10.3.9 DcmDsdService Container */
typedef struct 
{
	uint8						 DcmDsdSidTabServiceId;				/* @ECUC_Dcm_00735 :Identifier of the service. */ 
	/*boolean				 DcmDsdSidTabSubfuncAvail;*/		/* @ECUC_Dcm_00737 :Information about whether the service has subfunctions or not. 
																					This parameter is used for the handling of the "suppressPosRspMsgIndicationBit" as defined in ISO 14229-1 */ 	
   const boolean         funcAddrSupportted;             /* if function address is supportted */
   const Dcm_SessionType sessionSupp[DCM_SESSION_SUPPORT_NUM];/* supportted session mode */
   const Dcm_ServiceFct  serviceFct;                          /* service function */
   const Dcm_PendingFct  pendingFct;                          /* service pending function */
	boolean					 Arc_EOL;
}Dcm_DsdServiceType;

/* Autosar DCM v4.2.2, 10.3.8 DcmDsdServiceTable Container */
typedef struct 
{
	uint8						      DcmDsdSidTabId; /*@ECUC_Dcm_00736:Due to using possibly more service tables, 
	                                             the unique DcmDsdSidTabId can be used to identify them. */ 
	/*Autosar DCM v4.2.2, 10.3.9 DcmDsdService Container*/
#if 0																
	const Dcm_DsdServiceType	*DcmDsdService; /*@ECUC_Dcm_00689: This container contains the configuration (DSD parameters) for a Service. */ 
#endif
	const Dcm_ServiceTableType *DcmDsdService;
	boolean						   Arc_EOL;
}Dcm_DsdServiceTableType; 

/* Autosar DCM v4.2.2, 10.3.5 DcmDsd Container */
typedef struct
{
	const Dcm_DsdServiceTableType *DsdServiceTable; /*@ECUC_Dcm_00688:This container contains the configuration (DSD parameters) 
	                                                  for a Service Identifier Table. (1..256)*/
}Dcm_DsdType;

/* Autosar DCM v4.2.2, 10.3.54 DcmDspDidControl Container */
typedef struct 
{
	Dcm_SesCtrlType	DcmDspSessionLevel;			    /*@req ECUC_Dcm_00765:subFunction value of the DiagnosticSession.*/
	uint16			   DcmDspSessionP2ServerMax;	    /*@req ECUC_Dcm_00766:session value for P2ServerMax in seconds (per Session).*/	
	uint16			   DcmDspSessionP2StarServerMax;	 /*@req ECUC_Dcm_00768: session value for P2*ServerMax in seconds (per Session)*/
	uint16 				DcmDspSessionS3Server;
	boolean			   Arc_EOL;
}Dcm_DspSessionRowType; /** @req DCM072 */

// 10.2.42
typedef struct 
{
	Dcm_SecLevelType			DcmDspSecurityLevel;			      /*@req ECUC_Dcm_00754:Value of Security level.*/
	uint16						DcmDspSecurityDelayTimeOnBoot;	/*@req ECUC_Dcm_00726:Value of the delay timer in case of 'power on' in seconds.*/
	uint8							DcmDspSecurityNumAttDelay;		   /*@req ECUC_Dcm_00762:Number of failed security accesses after which the delay time is activated */
	uint16						DcmDspSecurityDelayTime;		   /*@req ECUC_Dcm_00757:Delay time after failed security access in seconds.*/
	uint8							DspSecurityADRSize;					/*@req ECUC_Dcm_00725:Size in bytes of the AccessDataRecord used in GetSeed. */
	uint8							DcmDspSecuritySeedSize;				/*@req ECUC_Dcm_00755:size of the security seed (in Bytes). */
	uint8							DcmDspSecurityKeySize;				/*@req ECUC_Dcm_00760:size of the security key (in Bytes).*/
	Dcm_CallbackGetSeedFncType		DcmDspSecurityGetSeedFnc;	/*@req ECUC_Dcm_00968:Callout function name used to request a seed.*/
	Dcm_CallbackCompareKeyFncType	DcmDspSecurityCompareKeyFnc;/*@req ECUC_Dcm_00969:Function name to request the result of a key comparison.*/
	boolean							Arc_EOL;
}Dcm_DspSecurityRowType; /** @req DCM073 */

/* Autosar DCM v4.2.2, 10.3.54 DcmDspDidControl Container */
typedef struct 
{
	const Dcm_DspSessionRowType				**DcmDspDidControlSessionRef;			 /** @req ECUC_Dcm_00621 */
	const Dcm_DspSecurityRowType			   **DcmDspDidControlSecurityLevelRef;	 /** @req ECUC_Dcm_00620 */
#if 0
	const Dcm_DspDidControlRecordSizesType	*DspDidFreezeCurrentState;			/* @req ECUC_Dcm_00624 */
	const Dcm_DspDidControlRecordSizesType	*DspDidResetToDefault;				/* @req ECUC_Dcm_00623 */
	const Dcm_DspDidControlRecordSizesType	*DspDidShortTermAdjustment;		/* @req ECUC_Dcm_00625 */
#endif
} Dcm_DspDidControlType; 

// 10.2.27
typedef struct 
{
	const Dcm_DspSessionRowType		**DcmDspDidReadSessionRef;			// (1..*)	/** @req DCM615 */
	const Dcm_DspSecurityRowType	   **DcmDspDidReadSecurityLevelRef;	// (1..*)	/** @req DCM614 */
}Dcm_DspDidReadType; 

// 10.2.28
typedef struct 
{
	const Dcm_DspSessionRowType		**DcmDspDidWriteSessionRef;		// (1..*)	/** @req DCM618 */
	const Dcm_DspSecurityRowType	   **DcmDspDidWriteSecurityLevelRef;	// (1..*)	/** @req DCM617 */
}Dcm_DspDidWriteType; 

/* Autosar DCM v4.2.2, 10.3.53 DcmDspDidInfo Container */
typedef struct 
{
	//boolean					DcmDspDidDynamicallyDefined;	/* @req ECUC_Dcm_00612:Indicates if this DID can be dynamically defined */
	// Containers
	const Dcm_DspDidReadType		*DcmDspDidRead;	// (0..1)
	const Dcm_DspDidWriteType		*DcmDspDidWrite;	// (0..1)
	const Dcm_DspDidControlType		*DcmDspDidControl;	/*@req ECUC_Dcm_00619:This container contains the configuration (parameters) of the DID control. */ 
}Dcm_DspDidInfoType; 

/* Autosar DCM v4.2.2, 10.3.34 DcmDspDid Container */
typedef struct 
{
	const boolean								DcmDspDidUsed;					   /* @req ECUC_Dcm_00805:Allow to activate or deactivate the usage of a DID */ 
	const uint16								DcmDspDidIdentifier;				/* @req ECUC_Dcm_00602:2 byte Identifier of the DID */
	const uint16								DcmDspDidSize;
	Dcm_CallbackReadDataFncType			DcmDspDidReadDataFnc;
	Dcm_CallbackWriteDataFncType			DcmDspDidWriteDataFnc;
   const Dcm_SecurityType              securitySupp[DCM_SECURITY_SUPPORT_NUM];  /* supportted security level */
	const Dcm_SessionType               sessionSupp[DCM_SESSION_SUPPORT_NUM];    /* supportted session mode */
#if 0	
	const Dcm_DspDidInfoType				*DcmDspDidInfoRef;			   /* @req ECUC_Dcm_00604:This container contains the configuration (parameters) of the DID's Info */
#endif
	boolean										Arc_EOL;
}Dcm_DspDidType; 

/*Autosar DCM v4.2.2, 10.3.63 DcmDspPid Container */
typedef struct 
{
	boolean						DcmDspPidUsed;		   /* @req ECUC_Dcm_00806:Allow to activate or deactivate the usage of a PID */
	uint8							DcmDspPidIdentifier;	/* @req ECUC_Dcm_00627:1 byte Identifier of the PID */
	uint8							DcmDspPidSize; 		/* @req ECUC_Dcm_00870:Length of a PID in byte(s). */
#if 0
	const Dcm_DspPidDataType DcmDspPidData;
	const Dcm_DspPidSupportInfoType DcmDspPidSupportInfo;
#endif
}Dcm_DspPidType;

/*Autosar DCM v4.2.2, 10.3.26 DcmDsp Container */
typedef struct 
{
	uint8								DcmDspMaxDidToRead; /*@req ECUC_Dcm_00638:Indicates the maximum allowed DIDs in a single "ReadDataByIdentifier" request. */
	const Dcm_DspDidType			*DcmDspDid;	        /*@req ECUC_Dcm_00601:This container contains the configuration (parameters) of the DID.*/
	const Dcm_DspDidInfoType	*DcmDspDidInfo;     /*@req ECUC_Dcm_00607:This container contains the configuration (parameters) of the DID's Info.*/
	const Dcm_DspPidType			*DcmDspPid;			  /*@req ECUC_Dcm_00626:This container defines the availability of a PID to the DCM. */
#if 0
	const Dcm_DspSecurityType	*DcmDspSecurity;	  /*req ECUC_Dcm_00764:This container contains the configuration ( DSP parameter) for security level configuration */	
#endif
}Dcm_DspType;

/*Autosar DCM v4.2.2, 10.3.13 DcmDslBuffer Container */
typedef struct 
{
	uint16						DcmDslBufferSize;	/*@req ECUC_Dcm_00738:Size of the diagnostic buffer in bytes.*/
} Dcm_DslBufferType; 

/*Autosar DCM v4.2.2, 10.3.14 DcmDslCallbackDCMRequestService Container */
typedef struct 
{
	Dcm_CallbackStartProtocolFncType	   StartProtocol;
	Dcm_CallbackStopProtocolFncType		StopProtocol;
	boolean								      Arc_EOL;
} Dcm_DslCallbackDCMRequestServiceType;

/*Autosar DCM v4.2.2, 10.3.15 DcmDslDiagResp Container */
typedef struct 
{
#if 0
	boolean	DcmDslDiagRespOnSecondDeclinedRequest;	/*@req ECUC_Dcm_00914*/
#endif
	uint8	DcmDslDiagRespMaxNumRespPend;	            /*@req ECUC_Dcm_00693:Maximum number of negative responses with response code 0x78. */
} Dcm_DslDiagRespType;

/*Autosar DCM v4.2.2, 10.3.20 DcmDslProtocolRx Container */
typedef struct  
{
	const Dcm_ProtocolAddrType		DcmDslProtocolRxAddrType;	/*@req ECUC_Dcm_00710:Selects the addressing type of the reception channel. */ 
	const PduIdType					DcmDslProtocolRxPduId;		/*@req ECUC_Dcm_00687:Identifier of the PDU that is used for this reception channel. */
   const PduInfoType             DcmDslProtocolRxPduRef;    /*@req ECUC_Dcm_00770:Reference to a Pdu in EcuC that is used for this reception channel. */ 
	const boolean						Arc_EOL;
}Dcm_DslProtocolRxType;

/*Autosar DCM v4.2.2, 10.3.21 DcmDslProtocolTx Container */
typedef struct 
{
	const PduIdType				DcmDslTxConfirmationPduId;	 /*@req ECUC_Dcm_00864:Identifier of the PDU that is used by the lower level module for transmission 
																			confirmation of responses on this channel.*/
	const PduInfoType          DcmDslProtocolTxPduRef;  /*@req ECUC_Dcm_00772:Reference to a Pdu in EcuC that is used for this transmission channel.*/																			
	const boolean					Arc_EOL;
}Dcm_DslProtocolTxType;

/*Autosar DCM v4.2.2, 10.3.19 DcmDslMainConnection Container */
typedef struct 
{ 
	const uint8				DcmDslProtocolRxTesterSourceAddr; /*@req ECUC_Dcm_00826:Source address of the tester which uses this connection for diagnostic communication. */ 
#if 0
	const Dcm_DslPeriodicTransmissionType	*DslPeriodicTransmissionConRef;	// (0..1)
	const Dcm_DslResponseOnEventType		*DslROEConnectionRef;			// (0..*)
#endif
	// Containers
	const Dcm_DslProtocolRxType				*DslProtocolRx;					// (1..*) Remove?
	const Dcm_DslProtocolTxType				*DslProtocolTx;					// (1)
}Dcm_DslMainConnectionType;


/*Autosar DCM v4.2.2, 10.3.18 DcmDslConnection Container */
typedef struct  
{
	const Dcm_DslMainConnectionType			*DcmDslMainConnection; /*@req ECUC_Dcm_00706:This container contains the configuration for a main connection of a diagnostic protocol.*/
#if 0
	const Dcm_DslPeriodicTransmissionType	*DslPeriodicTransmission;	// (0..1)
	const Dcm_DslResponseOnEventType		*DslResponseOnEvent;		// (0..1)
#endif	
	boolean 								Arc_EOL;
}Dcm_DslConnectionType;

typedef struct 
{
	const uint16			TimStrP2ServerMax;		// (1)
	const uint16			TimStrP2StarServerMax;	// (1)
	const uint16 			TimStrS3Server;			// (1)
	const boolean	Arc_EOL;
} Dcm_DslProtocolTimingRowType;

/*Autosar DCM v4.2.2, 10.3.17 DcmDslProtocolRow Container */
typedef struct 
{ 
#if 0
	boolean										DcmDslProtocolIsParallelExecutab; 
	uint16										DcmDslProtocolPreemptTimeout; 
	uint8									   	DcmDslProtocolPriority; 
	Dcm_ProtocolTransTypeType				DcmDslProtocolTransType; 
	Dcm_DslRunTimeProtocolParametersType *DcmDslRunTimeProtocolParameters; 
#endif
	Dcm_ProtocolType							DcmDslProtocolID;            /*@req ECUC_Dcm_00696:The diagnostic protocol type for the DCM DSL protocol that is being configured. */ 
	const Dcm_DslBufferType					*DcmDslProtocolRxBufferRef ; /*@req ECUC_Dcm_00701: used for diagnostic request reception for the protocol.*/
	const Dcm_DslBufferType					*DcmDslProtocolTxBufferRef ; /*@req ECUC_Dcm_00704: used for diagnostic response transmission for the protocol.*/
	const Dcm_DsdServiceTableType			*DcmDslProtocolSIDTable;     /*@req ECUC_Dcm_00702: used for diagnostic request processing for this protocol.*/
	const Dcm_DslConnectionType			*DcmDslConnection;  /*@req ECUC_Dcm_00706: This container contains the configuration for a main connection of a diagnostic protocol.*/
	const Dcm_DslProtocolTimingRowType	*DcmDslProtocolTimeLimit;    /*This structure is not follow the Autosar DCM v4.2.2*/
	boolean										Arc_EOL;
}Dcm_DslProtocolRowType;

/*Autosar DCM v4.2.2, 10.3.16 DcmDslProtocol Container */
typedef struct 
{
	const Dcm_DslProtocolRowType	*DcmDslProtocolRow;	/*@req ECUC_Dcm_00695:This container contains the configuration of one particular diagnostic protocol used in Dcm.*/
}Dcm_DslProtocolType;

/*Autosar DCM v4.2.2, 10.3.12 DcmDsl Container */
typedef struct 
{
	const Dcm_DslBufferType						*DcmDslBuffer;		/*@req ECUC_Dcm_00739:This container contains the configuration of a diagnostic buffer. */ 
	/*const Dcm_DslCallbackDCMRequestServiceType	*DcmDslCallbackDCMRequestService;	@req ECUC_Dcm_00679 */ 
	const Dcm_DslDiagRespType					*DcmDslDiagResp;	/*@req ECUC_Dcm_00691: */
	const Dcm_DslProtocolType					*DcmDslProtocol;	/*@req ECUC_Dcm_00694:This container contains the configurations of the diagnostic protocols used in Dcm. */ 
}Dcm_DslType;

/*Autosar DCM v4.2.2, 10.3.3 DcmConfigSet Container */
typedef struct 
{
	const Dcm_DspType *DcmDsp;      /* These parameters apply to Diagnostic Service Processing. There will always 
	                                   be one set of these parameters per Dcm. */
	const Dcm_DsdType *DcmDsd;      /* These parameters configure the Diagnostic Service Dispatcher submodule. */
	const Dcm_DslType *DcmDsl;      /* These parameters configure the Diagnostic Session Layer submodule. */
#if 0	
	const Dcm_PageBufferType     *DcmPageBufferCfg;               /* This container contains the configuration (parameters) for Page Buffer handling */
	const Dcm_ProcessingConditionsType *DcmProcessingConditions;  /*This container contains the configuration for mode arbitration functionality of the Dcm*/
#endif
}Dcm_ConfigSetType;

/*Autosar DCM v4.2.2, 10.3.105 DcmGeneral Container */
typedef struct 
{
	const boolean DcmDDDIDStorage;   /*@ECUC_Dcm_00971:This configuration switch defines, whether DDDID definition is stored non-volatile or not. */
	const boolean DcmDevErrorDetect; /*@ECUC_Dcm_00823:Switches the Default Error Tracer (Det) detection and notification ON or OFF.*/ 
	const boolean DcmRespondAllRequest; /*@ECUC_Dcm_00600:If set to FALSE the Dcm will not respond to diagnostic request that 
                                         contains a service ID which is in the range from 0x40 to 0x7F or in the range from 0xC0 to 0xFF (Response IDs). */  
	const boolean DcmVersionInfoApi;    /*@ECUC_Dcm_00821:Preprocessor switch to enable or disable the output Version info of the functionality. */
}Dcm_GeneralType;

/*Autosar DCM v4.2.2, 10.3.2 Dcm Containers */
typedef struct 
{
	const Dcm_ConfigSetType *DcmConfigSet; 
	const Dcm_GeneralType   *DcmGeneral; 
}Dcm_ConfigType;


/*=======[E X T E R N A L   D A T A]==========================================*/
extern const Dcm_ConfigType Dcm_Config;
extern const Dcm_DslProtocolRowType DcmDslProtocolRowList;

extern void Dcm_Init(const Dcm_ConfigType *ConfigPtr);

extern const Dcm_ServiceTableType Dcm_ServiceTable[DCM_SERVICE_NUM];

extern const Dcm_SessionRowType Dcm_SessionRow[DCM_SESSION_NUM];

extern const Dcm_ResetRowType Dcm_ResetRow[DCM_RESET_NUM];

#if(DCM_READDID_NUM > 0)
extern const Dcm_ReadDidRowType Dcm_ReadDidRow[DCM_READDID_NUM];
#endif

extern const Dcm_SecurityRowType Dcm_SecurityRow[DCM_SECURITY_NUM];

#if (DCM_COM_CONTROL_NUM > 0)
extern const Dcm_ComControlRowType Dcm_ComControlRow[DCM_COM_CONTROL_NUM];
#endif

extern const Dcm_WriteDidRowType Dcm_WriteDidRow[DCM_WRITEDID_NUM];

extern const Dcm_RoutineControlRowType Dcm_RoutineControlRow[DCM_ROUTINE_CONTROL_NUM];

extern const Dcm_DownloadRowType Dcm_DownloadRow;

#if (DCM_DTC_SET_NUM > 0)
extern const Dcm_DTCSettingRowType Dcm_DTCSettingRow[DCM_DTC_SET_NUM];
#endif

#endif/* endof DCM_TYPES_H */

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121109    Gary       Initial version
 */
/*=======[E N D   O F   F I L E]==============================================*/
