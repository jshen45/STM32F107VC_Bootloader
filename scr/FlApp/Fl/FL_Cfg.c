/*============================================================================*/
/** Copyright (C) 2009-2011, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  @file       <FL_Cfg.c>
 *  @brief      <Flash Loader Configuration >
 *               describe the block infomation.
 *  
 *  @author     <Gary Chen>
 *  @date       <2012-12-27>
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/

#include "FL.h"

/*=======[E X T E R N A L   D A T A]==========================================*/
const FL_BlockDescriptorType FL_BlkInfo[FL_NUM_LOGICAL_BLOCKS] =
{
    /* start global address,length,    maxmum attempt */ 
    /* @type:uint32 range:0x0000C000~0xFFFFFFFF note:NONE */
                            /* @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE */
                                       /* @type:uint8 range:0x00~0xFF note:NONE */ 
	 {0x08010000uL,              0x28000uL,0xFFu},  //192k
};

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121227    Gary       Initial version
 */
/*=======[E N D   O F   F I L E]==============================================*/

