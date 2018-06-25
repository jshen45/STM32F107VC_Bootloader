/*============================================================================*/
/** Copyright (C) 2009-2011, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  @file       <Cal.h>
 *  @brief      < function decalrations for Cal>
 *  
 *  @author     <Gary Chen>
 *  @date       <2012-12-27>
 */
/*============================================================================*/
#ifndef CAL_H
#define CAL_H

/*=======[I N C L U D E S]====================================================*/

//#include "Std_Types.h"
#include "Common_Types.h"


/*=======[E X T E R N A L   D A T A]==========================================*/
extern const uint32 Cal_Crc32Tab[256];

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
void Cal_Crc32Init(uint32 *curCrc);

void Cal_ComputeCrc32(uint32 *curCrc, const uint8 *buf, const uint32 size);

void Cal_CrcFinalize(uint32 *curCrc);
    
#endif/* endof CAL_H */

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121227    Gary       Initial version
 */
/*=======[E N D   O F   F I L E]==============================================*/
