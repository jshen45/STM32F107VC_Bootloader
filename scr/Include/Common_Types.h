/*============================================================================*/
/** Copyright (C) 2009-2011, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  
 *  @file       <Std_Types.h>
 *  @brief      <standard type define>
 *  
 *  <Detailed description of the file(multi-line)>
 *  
 *  @author     <Gary Chen>
 *  @date       <2012-09-09>
 */
/*============================================================================*/
#include <stdlib.h>
 #include <string.h> 
 
#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

/*************************************************************************
Declaration of macros
*************************************************************************/
#define EXPORTED  /* empty */
#define LOCAL     static

#define COM_U8_POS_DATA0                0x00
#define COM_U8_POS_DATA1                0x01
#define COM_U8_POS_DATA2                0x02
#define COM_U8_POS_DATA3                0x03
#define COM_U8_POS_DATA4                0x04
#define COM_U8_POS_DATA5                0x05
#define COM_U8_POS_DATA6                0x06
#define COM_U8_POS_DATA7                0x07
#define COM_U8_POS_DATA8                0x08

#define COM_U8_SHIFT_1_BIT          	 0x01
#define COM_U8_SHIFT_2_BIT          	 0x02
#define COM_U8_SHIFT_4_BIT          	 0x04
#define COM_U8_SHIFT_6_BIT          	 0x06
#define COM_U8_SHIFT_8_BIT          	 0x08
#define COM_U8_SHIFT_16_BIT         	 0x10
#define COM_U8_SHIFT_24_BIT         	 0x18

/*************************************************************************
Declaration of types
*************************************************************************/
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned long       uint32;
typedef unsigned char       boolean;

typedef uint8               Std_ReturnType;
typedef uint8               NotifResultType;

typedef unsigned char       BOOL_T;

typedef uint16	PduIdType;
typedef uint16	PduLengthType;

typedef struct 
{
	uint8 *sduDataPtr;
	PduLengthType sduLength;	
} PduInfoType;

typedef struct 
{
	PduInfoType sduData;
	PduIdType   u16PduId;	
} PduType;

typedef enum
{
	BUFREQ_OK = 0u,
	BUFREQ_E_NOT_OK,
	BUFREQ_E_BUSY,
	BUFREQ_E_OVFL
} BufReq_RetType;


/*************************************************************************
Declaration of constant data
*************************************************************************/
/*
#ifndef TRUE
#define	TRUE	(1)
#endif

#ifndef FALSE
#define FALSE	(0)
#endif
*/
#ifndef NULLPTR
#define NULLPTR	((void *)0)
#endif

#ifndef KUC_NULL
#define  KUC_NULL ((uint8)0x00)
#endif

#ifndef KUL_NULL
#define  KUL_NULL ((uint32)0x00)
#endif

#define E_OK     0x00
#define E_NOT_OK 0x01

#define STATIC static


#define B_FALSE   ((BOOL_T)0)
#define B_TRUE    ((BOOL_T)!0)

/* followed item is for NotifResultType */
#define  NTFRSLT_OK                    (NotifResultType)(0x00u)
#define  NTFRSLT_E_NOT_OK              (NotifResultType)(0x01u)
#define  NTFRSLT_E_TIMEOUT_A           (NotifResultType)(0x02u)
#define  NTFRSLT_E_TIMEOUT_BS          (NotifResultType)(0x03u)
#define  NTFRSLT_E_TIMEOUT_CR          (NotifResultType)(0x04u)
#define  NTFRSLT_E_WRONG_SN            (NotifResultType)(0x05u)
#define  NTFRSLT_E_INVALID_FS          (NotifResultType)(0x06u)
#define  NTFRSLT_E_UNEXP_PDU           (NotifResultType)(0x07u)
#define  NTFRSLT_E_WFT_OVRN            (NotifResultType)(0x08u)
#define  NTFRSLT_E_NO_BUFFER           (NotifResultType)(0x09u)
#define  NTFRSLT_E_CANCELATION_OK      (NotifResultType)(0x0Au)
#define  NTFRSLT_E_CANCELATION_NOT_OK  (NotifResultType)(0x0Bu)


#endif/* endof STD_TYPES_H */

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20120909    Gary       Initial version
 */
/*=======[E N D   O F   F I L E]==============================================*/

