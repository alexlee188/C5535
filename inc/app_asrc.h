/*
 * $$$MODULE_NAME app_asrc.h
 *
 * $$$MODULE_DESC app_asrc.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#ifndef _APP_ASRC_H_
#define _APP_ASRC_H_

#include "psp_i2s.h"
#include "dda_dma.h"
#include "app_usb.h"
#include "app_usbac.h"

#define ENABLE_PB_ASRC /* define to enable Playback ASRC */

/* App ASRC return codes */
/* Global return codes */
#define AASRC_SOK                   ( 0 )   /* indicates successful return status */
#define AASRC_INV_PRM               ( 1 )   /* indicates invalid parameter to function */
/* Combine ASRC output return codes */
#define AASRC_OUT_FIFO_UND          ( 2 )   /* indicates ASRC output FIFO underflow */
/* Pitch calculation return codes */
#define AASRC_PCALC_INV_PPBUF       ( 3 )   /* indicates invalid ping/pong address for Left Channel Tx DMA -- spans 64K word boundary */

/* Playback ASRC input FIFO size */
#define PB_ASRC_IN_FIFO_SZ          ( ASRC_DEF_IN_FIFO_SZ )

/* Combine ASRC output buffer formats */
/* Stereo output stream is interleaved: left samp, right samp, left samp, right samp, ...
Only Left output buffer pointer is pass as input parameter to function. */
#define CMBASRC_OUTFMT_INT          ( 0 )
/* Stereo output stream is split buffer: left block, right block, ...
Only Left output buffer pointer is pass as input parameter to function. */
#define CMBASRC_OUTFMT_SBUF         ( 1 )
/* Output stream is unpacked (mono or stereo). 
Used for direct output to DMA buffers with I2S in unpacked mode.
For stereo, Left and Right output buffer pointers must be passed as input parameters to function. */
#define CMBASRC_OUTFMT_UNPK         ( 2 )

extern Uint32 gUsbPbSampRateHz; /* current USB playback sampling rate */
extern Uint32 gDacSampRateHz;   /* current DAC sampling rate */

/* Pitch calculation variables */
extern Uint16 dmaChanNum;           /* DMA channel number for Left Channel Tx DMA (0-15) */
extern Uint32 dmaBaseAddr;          /* 16 LSBs Left Channel Tx DMA source base address */
extern Uint16 dmaFrameLenSamps;     /* DMA frame length in samples */
extern volatile ioport Uint16 *dmaStartAddrReg0; /* I/O space address of Left Channel Tx DMA Source Start Address register */
extern volatile ioport Uint16 *dmaStartAddrReg1; /* I/O space address of Left Channel Tx DMA Source Start Address register */
extern Int16 dmaCXferCnt;           /* current count of completed DMA transfers between SOF's for Left Channel Tx DMA */
extern Uint16 numCDmaXfer;          /* number of completed DMA transfers between SOF's for Left Channel Tx DMA */
extern Uint32 dmaCurAddr;           /* 16 LSBs Left Channel Tx DMA source address */
extern Bool firstSof;               /* indicates whether to initiate DMA sample count computation */
extern Uint16 dmaCurCnt;            /* DMA transfer count in frame for current SOF */
extern Uint16 dmaPrevCnt;           /* DMA transfer count in frame for previous SOF */

extern Uint16 i2sRxSampCnt;         /* Rx sample count */

/* Initializes pitch calculation */
Int16 initPitchCalc(
    DMA_ChanHandle hDmaTxLeft
);

/* Latches current DMA transfer state */
void latchDmaXferState(void);

/* Compute DMA transfer count since last SOF */
Uint16 computeDmaXferCnt(void);

/* Calculates pitch */
void calcPitch(
    Uint16 dmaSampCnt
);

#endif /* _APP_ASRC_H_ */
