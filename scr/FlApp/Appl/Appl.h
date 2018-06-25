/*============================================================================*/
/* Copyright (C) 2016-2017, Chengdu WeiTe Electronic Fuel CO.,LTD..
 *  
 *  All rights reserved. This software is WIT property. Duplication 
 *  or disclosure without WIT written authorization is prohibited.
 *  
 *  @file       <Appl.h>
 *  @brief      <App Loader>
 *  
 *  <provide the environment of the Bootloader peoject>
 *  
 *  @author     <Mingdong.Yuan>
 *  @date       <2018-1-24>
 */
/*============================================================================*/

#ifndef APPL_H
#define APPL_H

/*=======[I N C L U D E S]====================================================*/
#include "Common_Types.h"

/*=======[M A C R O S]========================================================*/
/** Value indicate an external programming request. */
#define FL_EXT_PROG_REQUEST_RECEIVED    (uint8)0x2Au

/** Value indicate an update of the application software. */
#define FL_APPL_UPDATED 0xD5u

/** The physical memory location of boot request flag. LOCAL address*/
/* @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE */
#define FL_BOOT_MODE    0x2000FFF1uL

/** The physical memory location of application software update flag. LOCAL address*/
/* @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE */
#define FL_APPL_UPDATE  0x2000FFF2uL

/** Application RCHW address */
/* @type:uint32 range:0x0000C000~0xFFFFFFFF note:NONE */
#define FL_APP_RCHW_ADDR     0x08020000uL

/*************************************************************************
EXPORTED variables declaration
*************************************************************************/
extern boolean APPL_b500usTimerFlag;
extern boolean APPL_b1msTimerFlag;
extern boolean APPL_b5msTimerFlag;
extern boolean APPL_b10msTimerFlag;
extern uint32 APPL_u32TimerCounter;
extern uint32 ecuBootStayTimer;

/*=======[I N T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
static void Appl_GotoAppSW(uint32 Addr);
static void Appl_InitNvmInfo(void);

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
extern void Appl_EcuStartup (void);
extern void Appl_EcuInitialize(void);
extern Std_ReturnType Appl_CheckConsistency (void);
extern void Appl_BootStayTimer(void);
extern void Appl_EcuReset(void);
extern void Appl_LedTest(void);

#endif/* endof APPL_H */

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121227    Gary       Initial version
 */
/*=======[E N D   O F   F I L E]==============================================*/

