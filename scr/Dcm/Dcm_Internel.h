/*============================================================================*/
/** Copyright (C) 2009-2011, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  @file       <Dcm_Internel.h>
 *  @brief      <Macro and function decalrations for Dcm Module>
 *  
 *  
 *  @author     <Gary Chen>
 *  @date       <2012-11-09>
 */
/*============================================================================*/
#ifndef DCM_INTERNEL_H
#define DCM_INTERNEL_H

/*=======[I N C L U D E S]====================================================*/
#include "Dcm_Cfg.h"
#include "Dcm_Types.h"

/*=======[E X T E R N A L   D A T A]==========================================*/
/** use static memory for service */
extern uint8	Dcm_MainRxBuff[DCM_RX_BUF_SIZE];
extern uint8	Dcm_MainTxBuff[DCM_TX_BUF_SIZE];

#endif/* endof DCM_INTERNEL_H */

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121109    Gary       Initial version
 */
/*=======[E N D   O F   F I L E]==============================================*/
