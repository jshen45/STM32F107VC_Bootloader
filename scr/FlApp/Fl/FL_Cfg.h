/*============================================================================*/
/** Copyright (C) 2009-2011, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  @file       <FL_Cfg.h>
 *  @brief      <Flash Loader Configuration file>
 *              Contained Macros of Mode,Timer and Address.
 *  @author     <Gary Chen>
 *  @date       <2012-12-27>
 */
/*============================================================================*/
#ifndef FL_CFG_H
#define FL_CFG_H

/*=======[M A C R O S]========================================================*/
/** Sleep time count:if configered 0,then no sleep function. */
/* @type:uint16 range:0~65535 note:unit ms */
#define FL_SLEEP_TIMER  0

/** Totol num of program blocks. */
/* @type:uint8 range:1~255 note:reference to num of FL_BlkInfo */
#define FL_NUM_LOGICAL_BLOCKS   (uint8)1u

/** Maxmum of segment in one block. */
/* @type:uint8 range:1~255 note:NONE */
#define FL_MAX_SEGMENTS (uint8)1u

/** Value for fill gap,if configered 0xFF,then not excute. */
/* @type:uint8 range:0x00~0xFF note:NONE */
#define FL_GAP_FILL_VALUE   0xFFu

/* @type:define range:NONE note:auto generate */
#if (FL_GAP_FILL_VALUE == 0xFFu)
#define FL_USE_GAP_FILL FALSE
#else
#define  FL_USE_GAP_FILL TRUE
#endif

/** Start address of flash driver in RAM. LOCAL address */
/* @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE */
#define FL_DEV_BASE_ADDRESS 0x40011000uL

/** Length of flash driver in RAM. */
/* @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE */
#define FL_DEV_SIZE 0x2000uL

/** timer delay in bootloader when ECU is startup */
/* @type:uint16 range:0~65535 note:unit ms */
#define FL_MODE_STAY_TIME    50

/** Length of finger print infomation */
/* @type:uint16 range:1~65535 note:NONE */
#define FL_FINGER_PRINT_LENGTH  17u

#ifndef 	BL_UPDATE
/*
** not standard config parameter
*/
/** bootloader infomation global address in NVM */
/* @type:uint32 range:NONE note:auto generate */
#define FL_NVM_INFO_ADDRESS  0x00080000uL

#else

#define FL_NVM_INFO_ADDRESS  0x00008000uL

#endif

/** code flash size that should program once time */
/* @type:uint32 range:NONE note:auto generate */
#define FL_FLASH_ALIGN_SIZE    (uint32)0x00000010uL

#endif/* endof FL_CFG_H */

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121227    Gary       Initial version
 */
/*=======[E N D   O F   F I L E]==============================================*/

