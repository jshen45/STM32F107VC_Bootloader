/*============================================================================*/
/* Copyright (C) 2015-2016, Chengdu WeiTe Electronic Fuel CO.,LTD.
 *  
 *  All rights reserved. This software is WIT property. Duplication 
 *  or disclosure without WIT written authorization is prohibited.
 *  
 *  
 *  @file            :<Dcm_Dsd.h>
 *  @brief           :<Dcm for MPC5634> 
 *  @author          :<Mingdong.Yuan>
 *  @date            :<2016-8-11>
 *  @Current revision: $Revision: 1.0
 */
/*============================================================================*/

#ifndef DCM_DSD_H_
#define DCM_DSD_H_

/*************************************************************************
Packages inclusion
*************************************************************************/
#include "Common_Types.h"

/*************************************************************************
Declaration of functions
*************************************************************************/
/*=======[I N T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
static void ServiceHandle(void);
static void ServiceProcess(void);

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
extern void Dsd_Init(void);
extern void Dsd_MainFunction(void); 
#endif /* DCM_DSD_H_ */
