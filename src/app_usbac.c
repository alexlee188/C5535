/*
 * $$$MODULE_NAME app_usbac.c
 *
 * $$$MODULE_DESC app_usbac.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#include <std.h>
#include <string.h>
#include "app_usb.h"
#include "csl_audioClass.h"
#include "csl_audioClassAux.h"
#include "app_globals.h"
#include "app_usbac_descs.h"
#include "i2s_sample.h"
#include "app_usbac.h"
#include "VC5505_CSL_BIOS_cfg.h"
#include "codec_config.h"
#include "sample_rate.h"


/* Define to send known data on record path to USB */
//#define SEND_KNOWN_DATA_TO_USB // debug

extern Uint32 recAudioCopyCount;
extern Uint32 sofIntCountRec;

extern CSL_UsbContext  gUsbContext;

CSL_AcInitStructApp AC_AppHandle;

Uint16 active_sample_rate;
Uint16 rx_pkt_size_16K_playback;

Uint16 pId = DEV_PID;
Uint16 vId = DEV_VID;

Uint32 usb_error = 0; // debug

/* Allocating Memory for Use by the Module */
#pragma DATA_ALIGN(ACAppBuffer, 4);
Uint32 ACAppBuffer[APP_USBAC_AC_APP_BUF_SIZE];

/* Buffer used to store the playback data sent from USB */
#pragma DATA_ALIGN(lbaBufferPbApp, 4);
Uint16 lbaBufferPbApp[APP_USBAC_LBA_BUF_PB_SIZE];

/* Buffer used to store the record data to send to USB */
#pragma DATA_ALIGN(lbaBufferRecApp, 4);
Uint16 lbaBufferRecApp[APP_USBAC_LBA_BUF_REC_SIZE];

/* Buffer used to store the HID Report data to send to USB */
#pragma DATA_ALIGN(lbaBufferHidReportApp, 4);
Uint16 lbaBufferHidReportApp[HID_REPORT_SIZE_WORDS+1];

CSL_AcRequestStruct USB_ReqTable[] =
{
  { CSL_AC_REQUEST_GET_STATUS             , AC_reqGetStatus },
  { CSL_AC_REQUEST_CLEAR_FEATURE          , AC_reqClearFeature },
  { CSL_AC_REQUEST_SET_FEATURE            , AC_reqSetFeature },
  { CSL_AC_REQUEST_SET_ADDRESS            , AC_reqSetAddress },
  { CSL_AC_REQUEST_GET_DESCRIPTOR         , AC_reqGetDescriptor },
  { CSL_AC_REQUEST_SET_DESCRIPTOR         , AC_reqUnknown },
  { CSL_AC_REQUEST_GET_CONFIGURATION      , AC_reqGetConfiguration },
  { CSL_AC_REQUEST_SET_CONFIGURATION      , AC_reqSetConfiguration },
  { CSL_AC_REQUEST_GET_MAX_LUN            , AC_reqGetMaxLUN },
  { CSL_AC_REQUEST_GET_INTERFACE          , AC_reqGetInterface },
  { CSL_AC_REQUEST_SET_INTERFACE          , AC_reqSetInterface },
  { CSL_AC_REQUEST_SYNC_FRAME             , AC_reqUnknown },
  { CSL_AC_REQUEST_GET_CUR         		  , AC_reqGetCurrent },
  { CSL_AC_REQUEST_GET_RANGE           	  , AC_reqGetRange },
  { CSL_AC_REQUEST_GET_MEM				  , AC_reqUnknown },
  { CSL_AC_REQUEST_SET_CUR         		  , AC_reqSetCurrent },
  { CSL_AC_REQUEST_SET_RANGE           	  , AC_reqSetRange },
  { CSL_AC_REQUEST_SET_MEM				  , AC_reqUnknown },
  { 0, NULL }
};

CSL_AcRequestStruct USB_ReqTableHID[] =
{
  { CSL_AC_REQUEST_HID_GET_REPORT         , AC_reqHidGetReport },
  { CSL_AC_REQUEST_HID_GET_IDLE           , AC_reqHidGetIdle },
  { CSL_AC_REQUEST_HID_GET_PROTOCOL       , AC_reqUnknown },
  { CSL_AC_REQUEST_HID_SET_REPORT         , AC_reqUnknown },
  { CSL_AC_REQUEST_HID_SET_IDLE           , AC_reqHidSetIdle },
  { CSL_AC_REQUEST_HID_SET_PROTOCOL       , AC_reqUnknown },
  { 0, NULL }
};

#ifdef SEND_KNOWN_DATA_TO_USB // debug
#define DATA_TO_USB_SIZE ( 32 )
Int16 data_to_usb_idx = 8;
Int16 data_to_usb[DATA_TO_USB_SIZE] = {
    -32760, -28664, -24568, -20472, -16376, -12280,  -8184,  -4088,      8,   4104,   8200,  12296,  16392,  20488,  24584,  28680, 
    -32760, -28664, -24568, -20472, -16376, -12280,  -8184,  -4088,      8,   4104,   8200,  12296,  16392,  20488,  24584,  28680
};
#endif // SEND_KNOWN_DATA_TO_USB

static void MSCTask(void);

/**
 *  \brief  Function to play the audio
 *
 *  \param  dataLength    - Length of the data to be sent to audio device (In words)
 *  \param  leftDataBuf   - Left data buffer pointer
 *  \param  rightDataBuf  - Right data buffer pointer
 *  \param  ptr           - Pointer to hold the palyback status
 *
 *  \return Media status
 */
Uint32 appPlayAudioCount = 0;
CSL_AcMediaStatus appPlayAudio(
    Uint16    dataLength,
    Uint16    *leftDataBuf,
    Uint16    *rightDataBuf,
    void      *ptr
)
{
    Uint16    *playBackActive;

	appPlayAudioCount++;

#ifdef C5535_EZDSP_DEMO
	int i, numSample;
	int *samplePtr;
	extern int bufferIn[1024];
	extern int bufferInIdx;
	// get the number of samples
	numSample = dataLength/2;
	samplePtr = (int*)leftDataBuf;
#endif

    // NOTE: leftDataBuf and rightDataBuf point to the same buffer

    playBackActive = (Uint16*)ptr;
    *playBackActive = TRUE;

    // the first byte is the number of bytes in the buffer followed by the first left audio sample
    leftDataBuf++;

#ifdef C5535_EZDSP_DEMO
	samplePtr++;
	for (i=0; i<numSample; i++)
	{
		if (bufferInIdx<256)
		{
			/* Take average of left and right channels. */
			//temp = (signed long)samplePtr[i*2] + (signed long)samplePtr[i*2+1];
			//temp >>= 1;    /* Divide by 2 to prevent overload at output */
			//bufferIn[bufferInIdx] = (int)temp;
			// copy the audio sample from the USB buffer into bufferIn (left channel only)
			bufferIn[bufferInIdx] = (int)samplePtr[i*2]*2;
			bufferInIdx++;
			// if the bufferIn is filled, then send a semaphore to the SpectrumDisplayTask
			if (bufferInIdx==256)
			{
				// send semaphore
				//SEM_post(&SEM_BufferInReady);
			}
		}
	}
#endif

	// make sure we have enough microframes for playback (at least (CODEC_OUTPUT_SZ_MSEC/2)*8 microframes) 
    // enable the playback
    if ((usb_play_start)&&(usb_play_mode==FALSE)&&(codec_output_buffer_sample>=(MAX_TXBUFF_SZ_DACSAMPS*CODEC_OUTPUT_SZ_MSEC/2)))
    {
        usb_play_mode = TRUE;
        EZDSP5535_LED_on(3);
        usb_play_start = FALSE;
        EZDSP5535_LED_off(2);
    }

    return(CSL_AC_MEDIACCESS_SUCCESS);
}

/**
 *  \brief  Function to record the audio
 *
 *  \param  dataLength    - Length of the data to be received audio device
 *  \param  leftDataBuf   - Left data buffer pointer
 *  \param  rightDataBuf  - Right data buffer pointer
 *  \param  ptr           - Pointer for future use
 *
 *  \return Media status
 */
Uint32 appRecordAudioCount = 0;
CSL_AcMediaStatus appRecordAudio(
    Uint16    dataLength,
    Uint16    *leftDataBuf,
    Uint16    *rightDataBuf,
    void      *ptr
)
{
#if 0 // move to send_USB_Output
#ifdef ENABLE_RECORD
    ///Int16 codec_input_sample_count;
    Uint16 i;

    //STS_add(&mySts1, dataLength); // debug

    /* Compute number of samples in circular buffer */
    ///codec_input_sample_count = codec_input_buffer_input_index - codec_input_buffer_output_index;
    ///if (codec_input_sample_count < 0)
    ///{
    ///    codec_input_sample_count += CODEC_INPUT_BUFFER_SIZE;
    ///}

    /* First word is length in bytes */
#ifdef SAMP_24BIT
    *leftDataBuf++ = 4*dataLength;
#else
    *leftDataBuf++ = 2*dataLength;
#endif

#ifndef SEND_KNOWN_DATA_TO_USB
     // check for underflow
    if (codec_input_sample_count >= dataLength)
    {
        // get the data from the circular buffer
#ifdef SAMP_24BIT
        for(i = 0; i < dataLength; i++)
        {
            *leftDataBuf++ = codec_input_buffer[codec_input_buffer_output_index++];
            *leftDataBuf++ = codec_input_buffer[codec_input_buffer_output_index++];

            if (codec_input_buffer_output_index >= CODEC_INPUT_BUFFER_SIZE)
            {
                codec_input_buffer_output_index = 0;
            }
        }
#else
        for(i = 0; i < dataLength; i++)
        {
            *leftDataBuf++ = codec_input_buffer[codec_input_buffer_output_index++];
            if (codec_input_buffer_output_index >= CODEC_INPUT_BUFFER_SIZE)
            {
                codec_input_buffer_output_index = 0;
            }
        }
#endif
    }
    else
    {
        //codec input underflow - send 0's
        for (i = 0; i < dataLength; i++)
        {
            *leftDataBuf++ = 0;
        }
    }

#else // SEND_KNOWN_DATA_TO_USB // debug
    for(i = 0; i < dataLength; i++)
    {
        *leftDataBuf++ = data_to_usb[data_to_usb_idx];
        data_to_usb_idx = (data_to_usb_idx + 1) & (DATA_TO_USB_SIZE-1);
        
        codec_input_buffer_output_index++;
        if (codec_input_buffer_output_index >= CODEC_INPUT_BUFFER_SIZE)
        {
            codec_input_buffer_output_index = 0;
        }
    }

#endif // SEND_KNOWN_DATA_TO_USB

#endif // ENABLE_RECORD

#endif // # if 0
	
	appRecordAudioCount++;

    return(CSL_AC_MEDIACCESS_SUCCESS);
}

/**
 *  \brief  Function to initialize the audio playback
 *
 *  \param  status    - Status of audio playback
 *  \param  ptr       - Pointer for future use
 *
 *  \return Media status
 */
CSL_Status appInitPlayAudio(
    Uint16    status,
    void      *ptr
)
{
	volatile Uint16 i;
	
	// start the USB uFrame counting
	usb_play_start = TRUE;
	EZDSP5535_LED_on(2);
	usb_play_mode = FALSE;
	EZDSP5535_LED_off(3);

    // reset codec output circular buffer
    //memset(codec_output_buffer, 0, 2*CODEC_OUTPUT_BUFFER_SIZE);
	for (i=0; i<CODEC_OUTPUT_BUFFER_SIZE; i++)
	{
		codec_output_buffer[i] = 0;
	}
    //codec_output_buffer_input_index = CODEC_OUTPUT_BUFFER_SIZE/2;
    codec_output_buffer_input_index = 0;
    codec_output_buffer_output_index = 0;
    codec_output_buffer_underflow = 0;
    codec_output_buffer_overflow = 0;
	codec_output_buffer_sample = 0;

    return CSL_SOK;
}

/**
 *  \brief  Function to initialize the audio record
 *
 *  \param  status    - Status of audio record
 *  \param  ptr       - Pointer for future use
 *
 *  \return Media status
 */
CSL_Status appInitRecordAudio(
    Uint16    status,
    void      *ptr
)
{
	volatile Uint16 i;
	
	// start the USB uFrame counting
	usb_rec_start = TRUE;
	usb_rec_mode = FALSE;  
    // reset codec input circular buffer
	for (i=0; i<CODEC_INPUT_BUFFER_SIZE; i++)
	{
		codec_input_buffer[i] = 0;
	}
    //codec_input_buffer_input_index = CODEC_INPUT_BUFFER_SIZE/2;
    codec_input_buffer_input_index = 0;
    codec_input_buffer_output_index = 0;
    codec_input_buffer_underflow = 0;
    codec_input_buffer_overflow = 0;
	codec_input_buffer_sample = 0;

	recAudioCopyCount = 0;
    sofIntCountRec = 0;

    return CSL_SOK;
}

/**
 *  \brief  Function to stop the audio playback
 *
 *  \param  status    - Status of audio playback
 *  \param  ptr       - Pointer for future use
 *
 *  \return Media status
 */
CSL_Status appStopPlayAudio(
    Uint16    status,
    void      *ptr
)
{
    usb_play_mode = FALSE;
    EZDSP5535_LED_off(3);
    usb_play_start = FALSE;
    EZDSP5535_LED_off(2);
    return CSL_SOK;
}

/**
 *  \brief  Function to stop the audio record
 *
 *  \param  status    - Status of audio record
 *  \param  ptr       - Pointer for future use
 *
 *  \return Media status
 */
CSL_Status appStopRecordAudio(
    Uint16    status,
    void      *ptr
)
{
    usb_rec_start = FALSE;
    usb_rec_mode = FALSE;

    return CSL_SOK;
}

/**
 *  \brief  Function to send HID report
 *
 *  \param  dataLength    - Length of the report data (16-bit words)
 *  \param  hidReport     - Report buffer pointer
 *
 *  \return CSL status
 */
CSL_Status appGetHidReport(
    Uint16 dataLength, // bytes 
    Uint16 *hidReport
)
{
    /* First word is length in bytes */
    *hidReport = dataLength;
    hidReport++;

    /* Get report data */
    memcpy(hidReport, gHidReport, (dataLength+1)/2);

    return CSL_SOK;
}

/**
 *  \brief  Function to get the media size
 *
 *  \param  lunNo    - Logical unit number
 *
 *  \return size of the media
 */
Uint32 AppGetMediaSize(
    Uint16 lunNo
)
{
    Uint32 size;

    size = 0;

    return(size);
}

/**
 *  \brief  Function to initialize the media
 *
 *  \param  lunNo    - Logical unit number
 *
 *  \return Media status
 */
CSL_AcMediaStatus AppMediaInit(
    Uint16 lunNo
)
{
    return CSL_AC_MEDIACCESS_SUCCESS;
}

/**
 *  \brief  Function to know the media status
 *
 *  \param  lunNo    - Logical unit number
 *
 *  \return Media status
 */
CSL_AcMediaStatus AppGetMediaStatus(
    Uint16    lunNo
)
{
    return CSL_AC_MEDIACCESS_SUCCESS;
}

/**
 *  \brief  Function to Eject media
 *
 *  \param  lunNo  - Logical unit number
 *
 *  \return Media status
 */
CSL_AcMediaStatus AppMediaEject(
    Uint16    lunNo
)
{
    return(CSL_AC_MEDIACCESS_SUCCESS);
}

/**
 *  \brief  Function to lock media
 *
 *  \param  lunNo   - Logical unit number
 *  \param  status  - Media lock status
 *
 *  \return Media status
 */
CSL_AcMediaStatus AppLockMedia(
    Uint16  lunNo, 
    CSL_AcMediaLockStatus   status
)
{
    return(CSL_AC_MEDIACCESS_SUCCESS);
}

/***********************************************************************
 * process the data received from the USB  - called by SWI_Process_USB_Input
 ***********************************************************************/
Uint32 swiProcessCountIn = 0;
void process_usb_input(void)
{
    pAcClassHandle      pAcClassHdl;
    CSL_AcObject        *pAcHandle;
    CSL_Status          status;

	swiProcessCountIn++;
    pAcClassHdl = AC_AppHandle.pAcObj;
    pAcHandle   = &pAcClassHdl->acHandle;

    // playback USB data (numPbSamps)
    status = pAcHandle->playAudio(numPbSamps, &pAcHandle->lbaBufferPb[0],
                                      &pAcHandle->lbaBufferPb[0], (void*)(&pAcHandle->playBackActive));
    if(status != CSL_AC_MEDIACCESS_SUCCESS)
    {
        LOG_printf(&trace, "ERROR: process_usb_input() failed:\n");
    }
}

/**
 *  \brief  Application call back function for control transactions
 *
 *  \param  None
 *
 *  \return None
 */
void appCtrlFxn(void)
{
  AC_Ctrl(AC_AppHandle.pAcObj);
}

/**
 *  \brief  Application call back function for isochronous transactions
 *
 *  \param  None
 *
 *  \return None
 */
void appIsoFxn(void)
{
   AC_Iso(AC_AppHandle.pAcObj);
}

/**
 *  \brief  Application call back function for HID transactions
 *
 *  \param  None
 *
 *  \return None
 */
void appHidFxn(void)
{
  AC_Hid(AC_AppHandle.pAcObj);
}

/**
 *  \brief  Wraper for USB Msc Task
 *
 *  \param  None
 *
 *  \return None
 */
void USBMSCTask()
{
    /* Call the USB Mass storage class task */
    MSCTask();
}

/**
 *  \brief  USB Msc task
 *
 *  \param  None
 *
 *  \return None
 */
Uint32 mscTaskNum = 0;
Uint32 msgMscCtlCount = 0;
Uint32 msgIsoInCount = 0;
static void MSCTask(void)
{
    CSL_UsbMscMsg        wMSCMsg;
    pUsbContext     pContext = &gUsbContext;
    volatile WORD     Msg;
    pUsbEpStatus         peps;

    //TSK_settime(TSK_self()); // statistic collection
    while(TRUE)
    {
        /* wait for mailbox to be posted */
        MBX_pend(&MBX_msc, &wMSCMsg, SYS_FOREVER);
        Msg = wMSCMsg;
        if(Msg == CSL_USB_MSG_MSC_TASK_EXIT)
            break;

		mscTaskNum++;

        switch(Msg)
        {
            case CSL_USB_MSG_MSC_CTL:
				msgMscCtlCount++;
                peps = &pContext->pEpStatus[CSL_USB_EP0];
                if(peps->hEventHandler)
                    (*peps->hEventHandler)();
                break;

            case CSL_USB_MSG_ISO_IN:
				msgIsoInCount++;
                peps = &pContext->pEpStatus[EP_NUM_REC];
                if(peps->hEventHandler)
                    (*peps->hEventHandler)();
                break;

            case CSL_USB_MSG_ISO_OUT:
                peps = &pContext->pEpStatus[EP_NUM_PLAY];
                if(peps->hEventHandler)
                    (*peps->hEventHandler)();
                break;

            case CSL_USB_MSG_HID_INT_IN:
                peps = &pContext->pEpStatus[EP_NUM_HID];
                if(peps->hEventHandler)
                    (*peps->hEventHandler)();
                break;

            default:
                break;
        }

        //TSK_deltatime(TSK_self()); // statistic collection
    }
    /* Ack for exit this task */
    SEM_post(&SEM_MUSBMSCTaskExited);

}

/* Intializes active sample rate and I2S Tx buffer size */
Int16 initSampleRate(
    Uint16 usbSampleRate, 
    Uint16 *pActiveSampleRate, 
    Uint16 *pI2sTxBuffSz
)
{
    Int16 status = APP_USBAC_SOK;

    switch (usbSampleRate)
    {
    case RATE_48_KHZ:
        *pActiveSampleRate = ACTIVE_SAMPLE_RATE_48KHZ;
        *pI2sTxBuffSz = 2*TXBUFF_SZ_I2SSAMPS_48KHZ; /* 2x for stereo */
        break;
    case RATE_96_KHZ:
        *pActiveSampleRate = ACTIVE_SAMPLE_RATE_96KHZ;
        *pI2sTxBuffSz = 2*TXBUFF_SZ_I2SSAMPS_96KHZ; /* 2x for stereo */
        break;
    default:
        status = APP_USBAC_INV_USBSR;
        break;
    }

    return status;
}
