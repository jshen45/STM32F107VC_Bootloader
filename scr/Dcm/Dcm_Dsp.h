/*============================================================================*/
/** Copyright (C) 2009-2011, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  @file       <Dcm_Dsp.h>
 *  @brief      <Macro and function decalrations for Dcm Module>
 *  
 *  
 *  @author     <Gary Chen>
 *  @date       <2012-11-09>
 */
/*============================================================================*/
#ifndef DCM_DSP_H
#define DCM_DSP_H

/*=======[I N C L U D E S]====================================================*/
#include "Dcm.h"

extern uint8 u8DidNumber;
/*=======[I N T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
static uint32 Dcm_Get4Byte(const uint8* data);

static void Dcm_Set4Byte(uint8* destData,
                         const uint32 sourceData);
                         
static boolean Dcm_CheckSubFuncSession(const Dcm_SessionType * sessionTable);

static boolean Dcm_CheckMsgLength(const uint16 minlength,const uint16 receivedLength);
                                  
static boolean Dcm_CheckSecurityCondition(const Dcm_SecurityType * secLevTable);

static boolean Dcm_CheckSubFuncFind(const boolean find,const Dcm_BuffType * rxBuff);
                                    
static boolean Dcm_CheckConditionCorrect(const boolean condition);

static boolean Dcm_CheckReqOutOfRange(const boolean condition);

static void Dcm_RequestSeed(const Dcm_SecurityRowType * secTablePtr,const Dcm_BuffType * rxBuff,Dcm_BuffType * txBuff);
                            
static void Dcm_SendKey(const Dcm_SecurityRowType * secTablePtr,const Dcm_BuffType * rxBuff,Dcm_BuffType * txBuff);
static boolean Dsp_bLookupDID(uint16 u16DidNumber, const Dcm_DspDidType **cpstDidPtr);
static Dcm_NegativeResponseCodeType Dsp_ReadDidData(const Dcm_DspDidType *cpstDidPtr, PduInfoType *PduTxData);
/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
extern void Dsp_Init(void);
extern boolean Dsp_CheckSessionLevel(const Dcm_SessionType * sessionTable);
extern boolean Dsp_CheckSecurityLevel(const Dcm_SecurityType * secLevTable);
extern uint16 Dsp_ReadVIN(uint8 *readData);
void Dcm_RecvMsg10(const Dcm_BuffType * rxBuff,Dcm_BuffType * txBuff);

void Dcm_RecvMsg11(const Dcm_BuffType * rxBuff,Dcm_BuffType * txBuff);

void Dcm_RecvMsg22(const Dcm_BuffType * rxBuff,Dcm_BuffType * txBuff);
extern void Dsp_UdsReadDataByIdentifier(const Dcm_BuffType * rxBuff,Dcm_BuffType * txBuff);
void Dcm_RecvMsg2E(const Dcm_BuffType * rxBuff,Dcm_BuffType * txBuff);

void Dcm_RecvMsg27(const Dcm_BuffType * rxBuff,Dcm_BuffType * txBuff);

void Dcm_RecvMsg28(const Dcm_BuffType * rxBuff,Dcm_BuffType * txBuff);

void Dcm_RecvMsg34(const Dcm_BuffType * rxBuff,Dcm_BuffType * txBuff);

void Dcm_RecvMsg36(const Dcm_BuffType * rxBuff,Dcm_BuffType * txBuff);

void Dcm_RecvMsg37(const Dcm_BuffType * rxBuff,Dcm_BuffType * txBuff);

void Dcm_RecvMsg31(const Dcm_BuffType * rxBuff,Dcm_BuffType * txBuff);

void Dcm_CheckProgPreCondition(const Dcm_BuffType * rxBuff,Dcm_BuffType * txBuff);

void Dcm_CheckProgDependencies(const Dcm_BuffType * rxBuff,Dcm_BuffType * txBuff);

void Dcm_CheckMemory(const Dcm_BuffType * rxBuff,Dcm_BuffType * txBuff);

void Dcm_EraseMemory(const Dcm_BuffType * rxBuff,Dcm_BuffType * txBuff);

void Dcm_RecvMsg3E(const Dcm_BuffType * rxBuff,Dcm_BuffType * txBuff);

void Dcm_RecvMsg85(const Dcm_BuffType * rxBuff,Dcm_BuffType * txBuff);

void Dcm_Pending2E(const FL_ResultType errorCode,
                   const Dcm_BuffType * rxBuff,
                   Dcm_BuffType * txBuff);
                   
void Dcm_Pending36(const FL_ResultType errorCode,
                   const Dcm_BuffType * rxBuff,
                   Dcm_BuffType * txBuff);
                   
void Dcm_Pending31(const FL_ResultType errorCode,
                   const Dcm_BuffType * rxBuff,
                   Dcm_BuffType * txBuff);

Std_ReturnType vSecurityLevel_1_GetSeed (uint8 *seed);
Std_ReturnType vSecurityLevel_1_CompKey (uint8 *key);
Std_ReturnType vSecurityLevel_Locked_GetSeed (uint8 *seed);
Std_ReturnType vSecurityLevel_Locked_CompKey (uint8 *key);
#endif/* endof DCM_DSP_H */

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121109    Gary       Initial version
 */
/*=======[E N D   O F   F I L E]==============================================*/
