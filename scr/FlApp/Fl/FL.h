/*============================================================================*/
/** Copyright (C) 2009-2011, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  @file       <FL.h>
 *  @brief      <Macros,Types defines and function decalrations for Flash Driver 
 *              Module>
 *  
 *  @author     <Gary Chen>
 *  @date       <2012-12-27>
 */
/*============================================================================*/
#ifndef FLASHLOADER_H_
#define FLASHLOADER_H_

/*=======[I N C L U D E S]====================================================*/
#include "Common_Types.h"

//#include "Std_Types.h"
#include "FL_Cfg.h"

/*=======[M A C R O S]========================================================*/
/* FL module execute result */
#define FL_OK               0x00u
#define FL_FAILED           0x01u
#define FL_ERR_SEQUENCE     0x02u
#define FL_NO_FINGERPRINT   0x03u
#define FL_NO_FLASHDRIVER   0x04u
#define FL_ERR_ADDR_LENGTH  0x05u
#define FL_INVALID_DATA     0x06u

/*result values of flash driver routines*/
#define kFlashOk	0x0000u
#define kFlashFailed	0x0001u

/*=======[T Y P E   D E F I N I T I O N S]====================================*/
/** Standard return type for callback routines. */
typedef uint8 FL_ResultType;

/* 
** needed in the interface between flashloader runtime environment and security
** module. 
*/
typedef struct
{
    /* block start global address */
    const uint32 address;

    /* block length */
    const uint32 length;
    
    /* maxmun program cycle */
    const uint8 maxProgAttempt;
    
} FL_BlockDescriptorType;

/** Segment list infomation of the block */
typedef struct
{
    /* Start global address of the segment in flash */
    uint32 address;
    
    /* Length of the segment */
    uint32 length;
    
} FL_SegmentInfoType;

/** Needed in interface to the security module. */
typedef struct
{
    /* number of segment */
    uint8 nrOfSegments;

    /* segments infomation */
    FL_SegmentInfoType segmentInfo[FL_MAX_SEGMENTS];
    
} FL_SegmentListType;

/** Each logical block infomation in EEPROM. */
typedef struct
{
    boolean blkValid;
    
    uint16 blkProgAttempt;
    
    uint32 blkChecksum;
    
    uint8 fingerPrint[FL_FINGER_PRINT_LENGTH];
     
} FL_blockInfoType;

/** Totol block infomation in EEPROM. */
typedef struct
{
    FL_blockInfoType blockInfo[FL_NUM_LOGICAL_BLOCKS];
    
    /* NVM infomation checksum */
    uint32 infoChecksum;
    
} FL_NvmInfoType;

typedef struct 
{	
	/* logical target address */
	uint32 address;
	/* lenght information (in bytes) */
	uint32 length; 
	/* retrun value / error code: output parameters */
	uint16 errorCode;
	/* pointer to data buffer */
	uint8 *data;
} tFlashParam;

/*=======[E X T E R N A L   D A T A]==========================================*/
/* infomation witch shall be programmed to EEPROM */
extern FL_NvmInfoType FL_NvmInfo;
extern tFlashParam flashParamInfo;

/* logical block infomation that configered by user */

extern const FL_BlockDescriptorType FL_BlkInfo[FL_NUM_LOGICAL_BLOCKS];


/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
static uint8 FL_ErasePage(uint32 StartAddr,uint32 EndAddr);
static uint8 FL_ProgramWord(uint32 Address, uint32 Data);
static uint8 FL_FlashProgram(tFlashParam* flashParam);

extern void FL_InitState(void);

extern uint16 FL_ReadDid(uint8 *readData);

uint32 FL_ReadMemory(uint32 address, 
                     uint32 length,
                     uint8* data);
                     
extern uint8 FL_CheckProgPreCondition(uint8 * conditions);

extern FL_ResultType FL_WriteFingerPrint(const uint8 *data,
                                  const uint16 length);
                                  
FL_ResultType FL_CheckSumRoutine(const uint8 *checkSum);
											           
extern FL_ResultType FL_EraseRoutine(const uint8 blockIndex);

FL_ResultType FL_DownloadRequestValid(const uint32 startAdd, 
									  const uint32 length);
													             
FL_ResultType FL_FlashProgramRegion(const uint32 destAddr,
									const uint8 *sourceBuff,
									const uint32 length);
										                 
FL_ResultType FL_ExitTransferData(void);

boolean FL_ServiceFinished(FL_ResultType *error);

void FL_MainFunction(void);

#endif/* endof FL_H */

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121227    Gary       Initial version
 */
/*=======[E N D   O F   F I L E]==============================================*/

