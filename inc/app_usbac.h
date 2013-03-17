/*
 * $$$MODULE_NAME app_usbac.h
 *
 * $$$MODULE_DESC app_usbac.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#ifndef _APP_USBAC_H_
#define _APP_USBAC_H_

#include <std.h>
#include "csl_audioClass.h"
#include "user_interface.h"

/* Status return codes */
#define APP_USBAC_SOK       ( 0 )
#define APP_USBAC_INV_USBSR ( 1 )   /* Invalid USB sample rate */

/* Nominal USB transaction sizes */
#define NOM_TRANS_SZ_16KHZ          ( 16 )
#define NOM_TRANS_SZ_32KHZ          ( 32 )
#define NOM_TRANS_SZ_44_1KHZ        ( 44 )
#define NOM_TRANS_SZ_48KHZ          ( 48 )
#define NOM_TRANS_SZ_96KHZ          ( 96 )
#define NOM_TRANS_SZ_192KHZ          ( 192 )

#define APP_USBAC_AC_APP_BUF_SIZE   ( 1000 )
#define APP_USBAC_LBA_BUF_PB_SIZE   ( 2*2*192 )
#define APP_USBAC_LBA_BUF_REC_SIZE  ( 2*2*192 )

enum
{
    ACTIVE_SAMPLE_RATE_16KHZ = 0,
    ACTIVE_SAMPLE_RATE_48KHZ,
    ACTIVE_SAMPLE_RATE_96KHZ,
    ACTIVE_SAMPLE_RATE_192KHZ
};

#define RX_PKT_SIZE_16K_PLAYBACK_MONO   (32) /* rx buffer size in bytes for 16 kHz mono playback  */
#define RX_PKT_SIZE_16K_PLAYBACK_STEREO (64) /* rx buffer size in bytes for 16 kHz stereo playback  */

extern CSL_AcInitStructApp AC_AppHandle;

extern Uint16 active_sample_rate;
extern Uint16 rx_pkt_size_16K_playback;

extern Uint16 pId;
extern Uint16 vId;

/* Allocating Memory for Use by the Module */
extern Uint32 ACAppBuffer[APP_USBAC_AC_APP_BUF_SIZE];

/* Buffer used to store the playback data sent from USB */
extern Uint16 lbaBufferPbApp[APP_USBAC_LBA_BUF_PB_SIZE];

/* Buffer used to store the record data to send to USB */
extern Uint16 lbaBufferRecApp[APP_USBAC_LBA_BUF_REC_SIZE];

extern Uint16 lbaBufferFbckApp[3];

/* Buffer used to store the HID Report data to send to USB */
extern Uint16 lbaBufferHidReportApp[HID_REPORT_SIZE_WORDS+1];

extern CSL_AcRequestStruct USB_ReqTable[];
extern CSL_AcRequestStruct USB_ReqTableHID[];

CSL_AcMediaStatus appPlayAudio(
    Uint16    dataType,
    Uint16    *leftDataBuf,
    Uint16    *rightDataBuf,
    void      *ptr
);

CSL_AcMediaStatus appRecordAudio(
    Uint16    dataType,
    Uint16    *leftDataBuf,
    Uint16    *rightDataBuf,
    void      *ptr
);

CSL_Status appInitPlayAudio(
    Uint16    status,
    void      *ptr
);

CSL_Status appInitRecordAudio(
    Uint16    status,
    void      *ptr
);

CSL_Status appStopPlayAudio(
    Uint16    status,
    void      *ptr
);

CSL_Status appStopRecordAudio(
    Uint16    status,
    void      *ptr
);

/* Gets HID report */
CSL_Status appGetHidReport(
    Uint16 dataLength, 
    Uint16 *hidReport
);

Uint32 AppGetMediaSize(
    Uint16 lunNo
);

CSL_AcMediaStatus AppMediaInit(
    Uint16 lunNo
);

CSL_AcMediaStatus AppGetMediaStatus (
    Uint16 lunNo
);

CSL_AcMediaStatus AppMediaEject(
    Uint16 lunNo
);

CSL_AcMediaStatus AppLockMedia(
    Uint16  lunNo, 
    CSL_AcMediaLockStatus   status
);

void appCtrlFxn(void);

void appIsoFxn(void);

void appHidFxn(void);

/* Intializes active sample rate and I2S Tx buffer size */
Int16 initSampleRate(
    Uint16 usbSampleRate, 
    Uint16 *pActiveSampleRate, 
    Uint16 *pI2sTxBuffSz
);


#endif /* _APP_USB_AC_H_ */
