/*============================================================================*/
/* Copyright (C) 2016-2017, Chengdu WeiTe Electronic Fuel CO.,LTD..
 *  
 *  All rights reserved. This software is WIT property. Duplication 
 *  or disclosure without WIT written authorization is prohibited.
 *
 *  @file       <FL.c>
 *  @brief      <His Flash Loader >
 *  
 *  < The Code process checksum,erase and program for bootloader project>
 *
 *  @author     <Mingdong.Yuan>
 *  @date       <2018-1-24>

 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "FL.h"
#include "main.h"
#include "SecM.h"

/*=======[M A C R O S]========================================================*/
#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)
 #define PAGE_SIZE                         (0x400)    /* 1 Kbyte */
 #define FLASH_SIZE                        (0x20000)  /* 128 KBytes */
#elif defined STM32F10X_CL
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x40000)  /* 256 KBytes */
#elif defined STM32F10X_HD
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x80000)  /* 512 KBytes */
#elif defined STM32F10X_XL
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x100000) /* 1 MByte */
#else 
 #error "Please select first the STM32 device to be used (in stm32f10x.h)"    
#endif  

/** maxmun length of program buffer */
#define FL_PROGRAM_WORD_SIZE	0x20uL
#define FL_PROGRAM_SIZE	(FL_PROGRAM_WORD_SIZE * 0x04uL)

/*=======[T Y P E   D E F I N I T I O N S]====================================*/
/** flashloader job status */
typedef enum
{
    FL_JOB_IDLE,
    
    FL_JOB_ERASING,
    
    FL_JOB_PROGRAMMING,
    
    FL_JOB_CHECKING
    
} FL_ActiveJobType;

/** flashloader download step */
typedef enum
{
    FL_REQUEST_STEP,
    
    FL_TRANSFER_STEP,
    
    FL_EXIT_TRANSFER_STEP,
    
    FL_CHECKSUM_STEP
    
} FL_DownloadStepType;

/** flashloader status infomation */
typedef struct
{
    /* flag if fingerprint has written */
    boolean fingerPrintWritten;
    
    /* flag if fingerprint buffer */
    uint8 fingerPrint[FL_FINGER_PRINT_LENGTH];
    
    /* error code for flash active job */
    FL_ResultType errorCode;
    
    /* flag if current block is erased */
    boolean blockErased;
    
    /* current process block index */
    uint8 blockIndex;
    
    /* current procees start address */
    uint32 startAddr;
    
    /* current procees length */
    uint32 length;
    
    /* current procees buffer point,point to buffer supplied from DCM */
    const uint8 *dataBuff;
    
    /* segment list of current procees block */
    FL_SegmentListType segmentList;
    
    /* flashloader download step */
    FL_DownloadStepType downloadStep;
    
    /* current job status */
    FL_ActiveJobType activeJob;
    
} FL_DownloadStateType;

/*=======[I N T E R N A L   D A T A]==========================================*/
/** flashloader status infomation */
STATIC FL_DownloadStateType downloadStatus;

/** flashloader program align size,transfered address may not aligned */
STATIC uint32 alignedProgramSize;

/** flashloader program buffer */
STATIC uint32 programData[FL_PROGRAM_WORD_SIZE];

/** flashloader program length */
STATIC uint32 programLength;

/*=======[E X T E R N A L   D A T A]==========================================*/
/** NVM infomation witch include bootloader infomation,read from EEPROM */
FL_NvmInfoType FL_NvmInfo;
EXPORTED tFlashParam flashParamInfo;

/*=======[F U N C T I O N   I M P L E M E N T A T I O N S]====================*/
/******************************************************************************/
/**
 * @brief               <flashloader module initialize>
 * 
 * <initialize download status> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <NON Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
EXPORTED void FL_InitState(void)
{
    /* fingerprint is not written */
    downloadStatus.fingerPrintWritten = B_FALSE;
    
    /* current block is not erased */
    downloadStatus.blockErased = B_FALSE;
    
    /* download step is download request 0x34 */
    downloadStatus.downloadStep = FL_REQUEST_STEP;
    
    /* current active job is idle */
    downloadStatus.activeJob = FL_JOB_IDLE;

    return;
}

/******************************************************************************/
/**
 * @brief               <get aligned size by address>
 * 
 * <code flash align size is 0x08,data falsh align size is 0x02,this mean align
 *  size if the length that should program once time> .
 * @param[in]           <addr (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <uint32>    
 */
/******************************************************************************/
STATIC uint32 FL_GetAlignSize(void)
{
	/* flash minmum write size is 8 byte */     
    return FL_FLASH_ALIGN_SIZE;
}

/******************************************************************************/
/**
 * @brief               <read data by identifier for 0x22>
 * 
 * <read data by identifier for 0x22> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <readData (OUT)>
 * @param[in/out]       <NONE>
 * @return              <uint16>    
 */
/******************************************************************************/
uint16 FL_ReadDid(uint8 *readData)
{
    uint16 readLength = 0x00uL;
    uint8 readBlock = 0x00u;
    
    /* read finger print from all blocks */
    for (readBlock = 0; readBlock < FL_NUM_LOGICAL_BLOCKS; readBlock ++)
    {
        memcpy(readData, (const uint8*)FL_NvmInfo.blockInfo[readBlock].fingerPrint,FL_FINGER_PRINT_LENGTH);
        readLength += FL_FINGER_PRINT_LENGTH;
        readData += FL_FINGER_PRINT_LENGTH;
    }
    
    return readLength;
}

/******************************************************************************/
/**
 * @brief               <read memory for checksum>
 * 
 * <Needed by the security module to read only those memory areas that are in 
 *  the scope of the flashloader (e.g. not erase/write protected, not RAM, ...)> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <address (IN), length (IN)>
 * @param[out]          <data (OUT)>
 * @param[in/out]       <NONE>
 * @return              <uint32>    
 */
/******************************************************************************/
uint32 FL_ReadMemory(uint32 address, uint32 length, uint8 * data)
{
    uint32 readLength = 0x00u;
    uint8 curBlockIndex = 0;
    
    /* read data from flash block */
    for (curBlockIndex = 0; 
         curBlockIndex < FL_NUM_LOGICAL_BLOCKS; 
         curBlockIndex ++)
    {
        /* check if address is in range of logical blocks */
        if ((address >= FL_BlkInfo[curBlockIndex].address) && 
            (address < (FL_BlkInfo[curBlockIndex].address + 
                        FL_BlkInfo[curBlockIndex].length)))
        {
            while ((length > 0) && (address < (FL_BlkInfo[curBlockIndex].address + FL_BlkInfo[curBlockIndex].length)))
            {
                /* read data from global address */
                *data = *(uint8 *)address;
                data ++;
                address ++;
                length --;
                readLength ++;
            }
        }
    }
 
    return readLength;    
}

/******************************************************************************/
/**
 * @brief               <0x2E service write fingerprint>
 * 
 * <save fingerprint to internel buffer,fingerprint is written to EPROM when 
 *  request erase routine control.> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <NON Reentrant>
 * @param[in]           <data (IN), length (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>    
 */
/******************************************************************************/
FL_ResultType FL_WriteFingerPrint(const uint8 *data,const uint16 length)										 
{
    FL_ResultType ret = FL_OK;

    /* check if fingprint length is correct */ 
    if (FL_FINGER_PRINT_LENGTH == length)
    {
        /* save the BlockFingerPrint info*/
        memcpy(downloadStatus.fingerPrint, data, FL_FINGER_PRINT_LENGTH);
        /* set the download  state of the BlockFingerPrint */
        downloadStatus.fingerPrintWritten = B_TRUE;
        
        ret = FL_OK;
    }
    else
    {
        ret = FL_FAILED;
    }
 
    return ret;
}

/******************************************************************************/
/**
 * @brief               <get service status>
 * 
 * <supply active job status for service witch can not response with in 50ms,
 *  and send pending> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <NON Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <error (OUT)>
 * @param[in/out]       <NONE>
 * @return              <boolean>    
 */
/******************************************************************************/
boolean FL_ServiceFinished(FL_ResultType *error)
{
    boolean ret = FALSE;
    
    /* check if service is finished */
    if (FL_JOB_IDLE == downloadStatus.activeJob)
    {
        *error = downloadStatus.errorCode;
        ret = TRUE;
    }
    else
    {
        *error = FL_OK;
        ret = FALSE;
    }
    
    return ret;
}

/******************************************************************************/
/**
 * @brief               <active job erase>
 * 
 * <erase the current logical block witch requested by 0x31 service> .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>    
 */
/******************************************************************************/
STATIC FL_ResultType FL_Erasing(void)
{
    FL_ResultType ret = FL_FAILED;
    
    /* set flash driver input parameter */
    flashParamInfo.address = FL_BlkInfo[downloadStatus.blockIndex].address;
    flashParamInfo.length = FL_BlkInfo[downloadStatus.blockIndex].length;
    				
    /* erase the flash of the requested block*/
    ret = FL_ErasePage(flashParamInfo.address,flashParamInfo.address + flashParamInfo.length);
    
    /* check if erase success */
    if (ret == FL_OK)
    {
        /*set the block erased */
        downloadStatus.blockErased = B_TRUE;
    }
    else
    {
        ret = FL_FAILED;
    }

    return (ret);
}

/**
  * @brief  擦出用户程序储存区数据
  * @param  StartAddr 起始地址
	* @param  EndAddr 结束地址
  * @retval 数据擦出状态，0-擦出数据成功，1-擦出数据失败
  */
STATIC uint8 FL_ErasePage(uint32 StartAddr,uint32 EndAddr)
{
	uint32 i;
	FLASH_Status FLASHStatus = FLASH_COMPLETE;
	FLASH_Unlock();
	for(i=StartAddr;i<=EndAddr;i+=PAGE_SIZE)
	{
		FLASHStatus = FLASH_ErasePage(i);
		if(FLASHStatus != FLASH_COMPLETE)
		{
			FLASH_Lock();
			return	1;	
		}
	}
	FLASH_Lock();
	return 0;
}

/******************************************************************************/
/**
 * @brief               <0x31 check program pre-condition>
 * 
 * <0x31 check program pre-condition .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <NON Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <conditions (OUT)>
 * @param[in/out]       <NONE>
 * @return              <uint8>    
 */
/******************************************************************************/
EXPORTED uint8 FL_CheckProgPreCondition(uint8 * conditions)										 
{
    *conditions = 0x00u;
    
    return 0x00u;
}

/******************************************************************************/
/**
 * @brief               <0x31 service routine erase>
 * 
 * <erase routine for  logical block> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <NON Reentrant>
 * @param[in]           <blockIndex (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>    
 */
/******************************************************************************/
EXPORTED FL_ResultType FL_EraseRoutine(const uint8 blockIndex)
{
    FL_ResultType ret = FL_OK;

    /* check if finger print is written */
    if (B_FALSE == downloadStatus.fingerPrintWritten)
    {
        ret = FL_NO_FINGERPRINT;
    }
    else
    {
		 /* check download step sequence */
		 if (FL_REQUEST_STEP != downloadStatus.downloadStep)
		 {
			 ret = FL_ERR_SEQUENCE;
		 }
    }
    
    /* check the conditions of erase */
    if (FL_OK == ret)
    {
        /* check the erase block index and the pragram attempt*/
        if (blockIndex < FL_NUM_LOGICAL_BLOCKS)
        {
            if (((FL_NvmInfo.blockInfo[blockIndex].blkProgAttempt < 
                FL_BlkInfo[blockIndex].maxProgAttempt) ||
                (0x00u == FL_BlkInfo[blockIndex].maxProgAttempt)))
            {
                /* set current block is valid */
                FL_NvmInfo.blockInfo[blockIndex].blkValid = B_FALSE;
                
                /* increment program attempt counter of current block */
                FL_NvmInfo.blockInfo[blockIndex].blkProgAttempt ++;
                
                /* store fingerprint of current block */           
                /* change and initialize status of the programmed block */
                downloadStatus.segmentList.nrOfSegments = 0x00u;
                downloadStatus.blockIndex = blockIndex;
                downloadStatus.blockErased = B_FALSE;
                downloadStatus.errorCode = FL_OK;
                
                /* set the download active state to erase */
                downloadStatus.activeJob = FL_JOB_ERASING;
            }
            else
            {
                ret = FL_FAILED;
            }
        }
        else
        {
            ret = FL_INVALID_DATA;
        }
    }
    
    if (ret != FL_OK)
    {
        /* initialize the flash download state */
        FL_InitState();
    }
    
    return ret;
}

/******************************************************************************/
/**
 * @brief               <0x31 service routine checksum>
 * 
 * <checksum routine for flash driver or logical blocks,only for current 
 *  download address> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <NON Reentrant>
 * @param[in]           <checkSum (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>    
 */
/******************************************************************************/
EXPORTED FL_ResultType FL_CheckSumRoutine(const uint8 *checkSum)
{
    
    FL_ResultType ret = FL_OK;
    
    /* record checksum buffer address */
    downloadStatus.dataBuff = checkSum;
    
    /* check if download step is checksum step */
    if (FL_CHECKSUM_STEP == downloadStatus.downloadStep)
    {
        /* set the download active state to checksum */
        downloadStatus.activeJob = FL_JOB_CHECKING;
        downloadStatus.errorCode = FL_OK;
    }
    else
    {
        ret = FL_ERR_SEQUENCE;
        
        /* initialize the flash download state */
        FL_InitState();
    }
    
    return ret;
}

/******************************************************************************/
/**
 * @brief               <check segment address>
 * 
 * <check if the transfered address is in current block,and if the address is 
 *  increased by segment> .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>    
 */
/******************************************************************************/
STATIC FL_ResultType FL_CheckDownloadSegment(void)
{
    FL_ResultType ret = FL_OK;
    
    /* 
    ** check if block is erased,if current num of segment is less than maxmun,
    ** if address if in current block.
    */
    if (B_FALSE == downloadStatus.blockErased)
    {
        ret = FL_ERR_SEQUENCE;
    }
    else
    {
        if ((downloadStatus.segmentList.nrOfSegments < FL_MAX_SEGMENTS) &&
            (downloadStatus.startAddr >= 
             FL_BlkInfo[downloadStatus.blockIndex].address) &&
            ((downloadStatus.startAddr + downloadStatus.length) <= 
             (FL_BlkInfo[downloadStatus.blockIndex].address + 
              FL_BlkInfo[downloadStatus.blockIndex].length)))
        {
            FL_SegmentInfoType * curSegment;
            
            /* get current segment num */
            uint8 segmentIndex = downloadStatus.segmentList.nrOfSegments;
            
            /* check if segment address is increase */
            if (segmentIndex > 0x00u)
            {
                uint32 alignSize = FL_GetAlignSize() - 1;
                 
                curSegment = &downloadStatus.segmentList.segmentInfo[segmentIndex - 1];
                
                /* check if download address is in front segment range */
                if (downloadStatus.startAddr < (curSegment->address + curSegment->length))
                {
                    ret = FL_ERR_ADDR_LENGTH; 
                }
            }
            
            if (FL_OK == ret)
            {
                /* set the flash download info */	
                curSegment = &downloadStatus.segmentList.segmentInfo[segmentIndex];
                downloadStatus.segmentList.nrOfSegments ++;
                curSegment->address = downloadStatus.startAddr;
                curSegment->length = downloadStatus.length;
            }		
        }
        else
        {
            ret = FL_ERR_ADDR_LENGTH;
        }
    }
    
    return ret;
}

/******************************************************************************/
/**
 * @brief               <0x34 service download request>
 * 
 * <download request for current logical block> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <NON Reentrant>
 * @param[in]           <startAdd (IN), length (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>    
 */
/******************************************************************************/
EXPORTED FL_ResultType FL_DownloadRequestValid(const uint32 startAdd,const uint32 length)
{
    FL_ResultType ret = FL_OK;
    
    /* save the parameter used for active job use */
    downloadStatus.startAddr = startAdd;    
    downloadStatus.length = length;

    /* check if finger print is written */
    if (B_FALSE == downloadStatus.fingerPrintWritten)
    {
        ret = FL_NO_FINGERPRINT;
    }
    else
    {
        /* check download step sequence */
        if ((FL_REQUEST_STEP != downloadStatus.downloadStep) && 
            (FL_CHECKSUM_STEP != downloadStatus.downloadStep))
        {
            ret = FL_ERR_SEQUENCE;
        }
    }
    
    /* check the condition for download request */
    if (FL_OK == ret)
    {
		 /* check if download address is in correct range */
		 ret = FL_CheckDownloadSegment();

		 if (FL_OK == ret)
		 { 
			 /* if address is not aligned ,save the align size */
			 uint32 alignSize = FL_GetAlignSize() - 1;

			 alignedProgramSize = downloadStatus.startAddr & alignSize;

			 /* set the download step*/
			 downloadStatus.downloadStep = FL_TRANSFER_STEP;

			 /* initialize the program buffer */
			 memset((uint8 *)&programData[0],(uint8)FL_GAP_FILL_VALUE,(uint32)FL_PROGRAM_SIZE);    
		 }
    }
    
    if (ret != FL_OK)
    {
        /* initialize the flash download state */
        FL_InitState();
    }
    
    downloadStatus.activeJob = FL_JOB_IDLE;
    
    return ret;
}

/******************************************************************************/
/**
 * @brief               <0x36 service download data>
 * 
 * <download data for current logical block> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <NON Reentrant>
 * @param[in]           <destAddr (IN), sourceBuff (IN), length (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>    
 */
/******************************************************************************/
FL_ResultType FL_FlashProgramRegion(const uint32 destAddr,const uint8 *sourceBuff,const uint32 length)
{
    FL_ResultType ret = FL_OK;
    
    /* change local address to global address*/
    uint32 globalAddr = destAddr;
    
    /* check the conditions of the program*/
    if (FL_TRANSFER_STEP != downloadStatus.downloadStep)
    {
        ret = FL_ERR_SEQUENCE;
    }
    else
    {
        if ((downloadStatus.startAddr != globalAddr) || (downloadStatus.length < length))
        {
            ret = FL_ERR_ADDR_LENGTH;
        }
    }
    
    if (FL_OK == ret)
    {
       /* save parameter for program active job */
       downloadStatus.dataBuff = sourceBuff;
       programLength = length;
      
       /* set the download active state to program*/
       downloadStatus.activeJob = FL_JOB_PROGRAMMING;
       downloadStatus.errorCode = FL_OK;
    }

    if (ret != FL_OK)
    {
        /* initialize the flash download state */
        FL_InitState();
    }
    
    return ret;
}

/******************************************************************************/
/**
 * @brief               <program data>
 * 
 * <program the aligned data transfered by 0x36 service request > .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>    
 */
/******************************************************************************/
STATIC FL_ResultType FL_ProgrammingData(void)
{
    FL_ResultType ret = FL_OK;
    uint8 *programDataPtr;
	
    /* check the program length and program status */
    while ((programLength > 0) && (FL_OK == ret))
    {
        /* check if the program size is more than maxmun size of program buffer */
        if ((programLength + alignedProgramSize) >= FL_PROGRAM_SIZE)
        {
            /* get the download datas */
	     	programDataPtr = (uint8 *)&programData[0];
            memcpy(&programDataPtr[alignedProgramSize],downloadStatus.dataBuff,FL_PROGRAM_SIZE - alignedProgramSize);
					              
            /* index the databuff point in tranfered buffer */
            downloadStatus.dataBuff += FL_PROGRAM_SIZE - alignedProgramSize;
            
            /* index the totle program length */
            programLength -= FL_PROGRAM_SIZE - alignedProgramSize;
            
            /* set the flash driver input parameter */		
            flashParamInfo.address = downloadStatus.startAddr - alignedProgramSize;
            flashParamInfo.length = (uint32)FL_PROGRAM_SIZE;
            flashParamInfo.data = (uint8 *)&programData[0];
            
            /* program the data */
			FL_FlashProgram(&flashParamInfo);
            
            /* index the start address and length that record in 0x34 service */
            downloadStatus.startAddr += FL_PROGRAM_SIZE - alignedProgramSize;
            downloadStatus.length -= FL_PROGRAM_SIZE - alignedProgramSize; 
            
            alignedProgramSize = 0;
            
            /* check if program success */
            if (flashParamInfo.errorCode != kFlashOk)
            {
                ret = FL_FAILED;
            }
        }
        else
        {
            /* set the last datas for write of current service 0x36 */
	     	programDataPtr = (uint8 *)&programData[0];
            memcpy(&programDataPtr[alignedProgramSize], downloadStatus.dataBuff,(uint32)programLength);
        					      
        	/* index the start address and length that record in 0x34 service */			      
            downloadStatus.startAddr += programLength;
            downloadStatus.length -= programLength; 
            
            /* save the data length witch has not program */
            alignedProgramSize += programLength;
            
            /* end of current service 0x36 */
            programLength = 0; 
        }
    }
    
    return ret;
}

/******************************************************************************/
/**
 * @brief               <program aligned data>
 * 
 * <when the last 0x36 service request is received, the not aligned data should
 *  be program,and the pad address if filled for default vaule > .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>    
 */
/******************************************************************************/
STATIC FL_ResultType FL_ProgrammingAlignData(void)
{
    FL_ResultType ret = FL_OK;
    
    /* set the download step*/
    downloadStatus.downloadStep = FL_EXIT_TRANSFER_STEP;
    
    /* check if there is same no aligned data that needed programmed */
    if (alignedProgramSize > 0)
    {
        /* the last 0x36 */
        uint32 alignSize;
        uint8 *programDataPtr = (uint8 *)&programData[0];
			
        flashParamInfo.address = downloadStatus.startAddr - alignedProgramSize;
        
        alignSize = FL_GetAlignSize();
        
        /* fill pad for the left data */
        memset(&programDataPtr[alignedProgramSize],FL_GAP_FILL_VALUE,FL_PROGRAM_SIZE - alignedProgramSize);
				            
        /* set the flash download info */		
        flashParamInfo.data = (uint8 *)&programData[0];
        
        /* set the alignedProgramSize to multi of flash phrase */
        if ((alignedProgramSize & (alignSize - 1)) > 0)
        {
            alignedProgramSize &= ~(alignSize - 1);
            alignedProgramSize += alignSize;
        }
        
        flashParamInfo.length = alignedProgramSize;
        alignedProgramSize = 0;
        
        /* write the last 0x36 aligned data */
        FL_FlashProgram(&flashParamInfo);
		
        if (flashParamInfo.errorCode != kFlashOk)
        {
            ret = FL_FAILED;
        }
    }
    
    return ret;
}

/******************************************************************************/
/**
 * @brief               <active job program>
 * 
 * <program the data transfered by 0x36 service request > .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>    
 */
/******************************************************************************/
STATIC FL_ResultType FL_Programming(void)
{
    FL_ResultType ret = FL_OK;
    
    /* program buffer aligned data */
    ret = FL_ProgrammingData();
    
    /* check if the last not aligned data should be programmed */
    if ((downloadStatus.length == 0x00uL) && 
        (FL_OK == ret))
    {
        /* program the not aligned data */
        ret = FL_ProgrammingAlignData();
    }
    
    return ret;
}

/**
  * @brief  将程序数据写入指定Flash地址
  * @param  Address 起始地址
	* @param  Data 准备写入的数据
  * @retval 数据写入状态，0-写入数据成功，1-写入数据失败
  */
STATIC uint8 FL_ProgramWord(uint32 Address, uint32 Data)
{
	FLASH_Status FLASHStatus = FLASH_COMPLETE;
	FLASH_Unlock();
	FLASHStatus = FLASH_ProgramWord(Address,Data);
	if(FLASHStatus!=FLASH_COMPLETE)
	{
		FLASH_Lock();
		return	1;	
	}
	else
	{
		FLASH_Lock();
		return	0;	
	}
}

/*******************************************************************************
Function Name : BlFlash_FlashProgram
Engineer      : b01802
Date          : Feb-08-2013
Parameters    : NONE
Modifies      : NONE
Returns       : NONE
Notes         : Receive data and program data to flash. 
Issues        : NONE
*******************************************************************************/
STATIC uint8 FL_FlashProgram(tFlashParam* flashParam)
{
	uint32 u32EndAddress;
	uint8  u8ReturnValue;
	uint8  u8ReturnCode;
	uint32 u32DestAddress;  /* destination address */
	uint32 u32Source; 		/* source address for program and verify */
	uint32 u32AddressTemp;
	uint8  *pDataTemp;

	u32AddressTemp = flashParam->address;
	pDataTemp = flashParam->data;
	u32DestAddress = 0x00uL;
    flashParam->errorCode = kFlashOk;
    u32EndAddress = flashParam->address + flashParam->length;
	  
   while ((flashParam->length > 0) && (flashParam->address < u32EndAddress) && (kFlashOk == flashParam->errorCode))
   {                         		
	   /* Prepare data for programming */
	   /*The following can't run correct ,HardFault_Handler() will happened ,so in STM32 use the temp variable
	   u32DestAddress = flashParam->address;
	   u32Source = (uint32)flashParam->data;
	   */
	   u32DestAddress = u32AddressTemp;
	   u32Source = *(uint32 *)pDataTemp;

	   /*Flash program driver,different platform needs relevant driver*/
	   u8ReturnValue = FL_ProgramWord(u32DestAddress,u32Source);

      if ( 0 != u8ReturnValue )
      {
		  flashParam->errorCode = kFlashFailed; 
      }
	  else
	  {
	  	  flashParam->errorCode = kFlashOk;
	  }

	  /* write program words */
      /*flashParam->data += 0x04u;
        flashParam->address += 0x04uL;
	  */
      u32AddressTemp += 0x04uL;
	  pDataTemp +=0x04;
      flashParam->length -= 0x04uL;
   }
   
   u8ReturnCode = u8ReturnValue;
   return(u8ReturnCode);
}

/******************************************************************************/
/**
 * @brief               <fill pad>
 * 
 * <fill the pad between segments of current block> .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>    
 */
/******************************************************************************/
STATIC FL_ResultType FL_FillGap(void)
{
    FL_ResultType ret = FL_OK;
    uint8 segmentIndex = 0;
    uint32 startAddress = FL_BlkInfo[downloadStatus.blockIndex].address;
    uint32 gapLength;
    uint32 alignSize = FL_GetAlignSize();
	
    /* set the download datas with FL_GAP_FILL_VALUE */
    memset((uint8 *)&programData[0], (uint8)FL_GAP_FILL_VALUE, (uint32)FL_PROGRAM_SIZE); 
    
    flashParamInfo.data = (uint8 *)&programData[0];
    
    while ((segmentIndex <= downloadStatus.segmentList.nrOfSegments) &&
           (FL_OK == ret))
    {
        /* find length ofthe gap in the segment*/
        if (segmentIndex < downloadStatus.segmentList.nrOfSegments)
        {
            gapLength = downloadStatus.segmentList.segmentInfo[segmentIndex].address
					              - startAddress;
        }
        else
        {
            gapLength = (FL_BlkInfo[downloadStatus.blockIndex].address + 
							           FL_BlkInfo[downloadStatus.blockIndex].length) - 
							          startAddress;
        }
        
        gapLength &= ~(alignSize - 1);
        
        /* set the flash download address of gap */
        flashParamInfo.address = startAddress;
        
        while ((gapLength > 0) && (FL_OK == ret))
        {
            if (gapLength >= FL_PROGRAM_SIZE)
            {
                /* set the download length */
                flashParamInfo.length = FL_PROGRAM_SIZE;
                
                /* update the gap length*/
                gapLength -= FL_PROGRAM_SIZE;
            }
            else
            {
                /* the last gap*/
                flashParamInfo.length = gapLength;
                gapLength = 0;
            }
            
            /* write the flash of the FlashParam for gap*/
            FL_FlashProgram(&flashParamInfo);	
            		
            flashParamInfo.address += flashParamInfo.length;
            
            /* check if program pad success */
            if (flashParamInfo.errorCode != kFlashOk)
            {
                ret = FL_FAILED;
            }
        }
        
        if (segmentIndex < downloadStatus.segmentList.nrOfSegments)
        {
            /* set the next start address */
            startAddress = downloadStatus.segmentList.segmentInfo[segmentIndex].address + 
							             downloadStatus.segmentList.segmentInfo[segmentIndex].length;
							          
            if ((startAddress & (alignSize - 1)) > 0)
            {
                startAddress &= ~(alignSize - 1);
                startAddress += alignSize;
            }
        }
        
        segmentIndex ++;
    }
    
    return ret;
}

/******************************************************************************/
/**
 * @brief               <active job checksum>
 * 
 * <active checksum that request by 0x31 service> .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>    
 */
/******************************************************************************/
STATIC FL_ResultType FL_CheckSuming(void)
{
    FL_ResultType ret = FL_OK;
    SecM_StatusType secMStatus;
    SecM_VerifyParamType verifyParam;
    
    /* set verification API input parameter */
    verifyParam.segmentList = &downloadStatus.segmentList;
    verifyParam.verificationData = downloadStatus.dataBuff;
    
    /* CRC compute and verification */
    secMStatus = SecM_Verification(&verifyParam);
    
    /* set block not erased */
    downloadStatus.blockErased = FALSE;
    
    /* check if CRC if correct */
    if (SECM_OK == secMStatus)
    {
		 /* set current block is valid */
		 FL_NvmInfo.blockInfo[downloadStatus.blockIndex].blkValid = TRUE;

		 /* save computed CRC to NVM if CRC success */
		 FL_NvmInfo.blockInfo[downloadStatus.blockIndex].blkChecksum = verifyParam.crcTotle;
		                                               
		 /* fill the gap if configered */
		 ret = FL_FillGap();

		 if(FL_OK == ret)
		 /* check if EEPROM UPDATE failed */
		 if(ret != FL_OK)
		 {
		    FL_NvmInfo.blockInfo[downloadStatus.blockIndex].blkValid = FALSE;
		 }
    }
    else
    {
        ret = FL_FAILED;
    }
    
    /* reset download step */
    downloadStatus.downloadStep = FL_REQUEST_STEP;
    
    return ret;
}

/******************************************************************************/
/**
 * @brief               <0x37 service download finish>
 * 
 * <download finish for current logical block> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <NON Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>    
 */
/******************************************************************************/
EXPORTED FL_ResultType FL_ExitTransferData(void)
{
    FL_ResultType ret = FL_OK;
    
    if (FL_EXIT_TRANSFER_STEP == downloadStatus.downloadStep)
    {
        /* set the download step*/
        downloadStatus.downloadStep = FL_CHECKSUM_STEP;
        downloadStatus.activeJob = FL_JOB_IDLE;
    }
    else
    {
        /* initialize the flash download state */
        ret = FL_ERR_SEQUENCE;
        FL_InitState();
    }
    
    return ret;
}

/******************************************************************************/
/**
 * @brief               <flash main function for active job>
 * 
 * <flash main function for active job,process checksum,erase and program> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
void FL_MainFunction(void)
{
    switch (downloadStatus.activeJob)
    {	
        case FL_JOB_ERASING:
            /* do the flash erase*/
            downloadStatus.errorCode = FL_Erasing();
            break;

        case FL_JOB_PROGRAMMING:
            /* do the flash program*/
            downloadStatus.errorCode = FL_Programming();
            break;

        case FL_JOB_CHECKING:
            /* do the flash checksum*/
            downloadStatus.errorCode = FL_CheckSuming();
            break;
			
        default:
            break;
    }
	
    if (downloadStatus.errorCode != FL_OK)
    {
        /* initialize the flash download state */
        FL_InitState();
    }
    
    downloadStatus.activeJob = FL_JOB_IDLE;
    
    return;
}

