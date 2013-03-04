/*
 * $$$MODULE_NAME i2s_sample.c
 *
 * $$$MODULE_DESC i2s_sample.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/**
 *  \file   i2s_sample.c
 *
 *  \brief  I2S application program
 *
 *  I2S sample test file which defines the interfaces for testing the
 *  uasability of the I2S driver.
 *
 *  (C) Copyright 2004, Texas Instruments, Inc
 *
 *  \author        PR Mistral
 *  \version    1.0   Initial implementation
 *  \version    1.1   Modified for review comments
 */

#include <string.h>
#include "psp_i2s.h"
#include "dda_i2s.h"
#include "psp_dma.h"
#include "dda_dma.h"
#include "csl_intc.h"    /* SampleBySample */
#include "app_globals.h"
#include "app_usb.h"
#include "app_usbac.h"
#include "app_usbac_descs.h"
#include "i2s_sample.h"
#include "gpio_control.h"

#include "VC5505_CSL_BIOS_cfg.h"
#include <clk.h>

#include "dbg_sdram.h"

/* I2S handles */
PSP_Handle       i2sHandleTx;
PSP_Handle       i2sHandleRx;

Uint16    fsError1 = 0;    /**< FSYNC gobal parameter                 */
Uint16    ouError1 = 0;    /**< under/over run global parameter    */
//Uint16 outwrap = 0;

DMA_ChanHandle   hDmaTxLeft;
DMA_ChanHandle   hDmaTxRight;
DMA_ChanHandle   hDmaRxLeft;
DMA_ChanHandle   hDmaRxRight;

#ifdef USE_TWO_CODEC
/* I2S handles */
PSP_Handle       i2sHandleTx2;
PSP_Handle       i2sHandleRx2;

DMA_ChanHandle   hDmaTxLeft2;
DMA_ChanHandle   hDmaTxRight2;
DMA_ChanHandle   hDmaRxLeft2;
DMA_ChanHandle   hDmaRxRight2;
#endif //USE_TWO_CODEC

#ifdef USE_THREE_CODEC
/* I2S handles */
PSP_Handle       i2sHandleTx2;
PSP_Handle       i2sHandleRx2;

DMA_ChanHandle   hDmaTxLeft2;
DMA_ChanHandle   hDmaTxRight2;
DMA_ChanHandle   hDmaRxLeft2;
DMA_ChanHandle   hDmaRxRight2;

PSP_Handle       i2sHandleTx3;
PSP_Handle       i2sHandleRx3;

DMA_ChanHandle   hDmaTxLeft3;
DMA_ChanHandle   hDmaTxRight3;
DMA_ChanHandle   hDmaRxLeft3;
DMA_ChanHandle   hDmaRxRight3;
#endif //USE_THREE_CODEC

#ifdef USE_FOUR_CODEC
/* I2S handles */
PSP_Handle       i2sHandleTx2;
PSP_Handle       i2sHandleRx2;

DMA_ChanHandle   hDmaTxLeft2;
DMA_ChanHandle   hDmaTxRight2;
DMA_ChanHandle   hDmaRxLeft2;
DMA_ChanHandle   hDmaRxRight2;

PSP_Handle       i2sHandleTx3;
PSP_Handle       i2sHandleRx3;

DMA_ChanHandle   hDmaTxLeft3;
DMA_ChanHandle   hDmaTxRight3;
DMA_ChanHandle   hDmaRxLeft3;
DMA_ChanHandle   hDmaRxRight3;

PSP_Handle       i2sHandleTx4;
PSP_Handle       i2sHandleRx4;

DMA_ChanHandle   hDmaTxLeft4;
DMA_ChanHandle   hDmaTxRight4;
DMA_ChanHandle   hDmaRxLeft4;
DMA_ChanHandle   hDmaRxRight4;
#endif //USE_FOUR_CODEC

#ifdef ENABLE_RECORD
/* Codec input ping/pong buffer (Left ch.) */
#pragma DATA_ALIGN(ping_pong_i2sRxLeftBuf, 4);
Uint16 ping_pong_i2sRxLeftBuf[2*I2S_RXBUFF_SZ]; /* 2x for ping/pong */

#ifdef USE_TWO_CODEC
#pragma DATA_ALIGN(ping_pong_i2sRxLeftBuf2, 4);
Uint16 ping_pong_i2sRxLeftBuf2[2*I2S_RXBUFF_SZ]; /* 2x 2nd for ping/pong */
#endif //USE_TWO_CODEC

#ifdef USE_THREE_CODEC
#pragma DATA_ALIGN(ping_pong_i2sRxLeftBu2, 4);
Uint16 ping_pong_i2sRxLeftBuf2[2*I2S_RXBUFF_SZ]; /* 2x 2nd for ping/pong */
#pragma DATA_ALIGN(ping_pong_i2sRxLeftBuf3, 4);
Uint16 ping_pong_i2sRxLeftBuf3[2*I2S_RXBUFF_SZ]; /* 2x 3rd for ping/pong */
#endif //USE_THREE_CODEC

#ifdef USE_FOUR_CODEC
#pragma DATA_ALIGN(ping_pong_i2sRxLeftBu2, 4);
Uint16 ping_pong_i2sRxLeftBuf2[2*I2S_RXBUFF_SZ]; /* 2x 2nd for ping/pong */
#pragma DATA_ALIGN(ping_pong_i2sRxLeftBuf3, 4);
Uint16 ping_pong_i2sRxLeftBuf3[2*I2S_RXBUFF_SZ]; /* 2x 3rd for ping/pong */
#pragma DATA_ALIGN(ping_pong_i2sRxLeftBuf4, 4);
Uint16 ping_pong_i2sRxLeftBuf4[2*I2S_RXBUFF_SZ]; /* 2x 4th for ping/pong */
#endif //USE_FOUR_CODEC

/* Codec input ping/pong buffer (Right ch.) */
#pragma DATA_ALIGN(ping_pong_i2sRxRightBuf, 4);
Uint16 ping_pong_i2sRxRightBuf[2*I2S_RXBUFF_SZ]; /* 2x for ping/pong */

#ifdef USE_TWO_CODEC
#pragma DATA_ALIGN(ping_pong_i2sRxRightBuf2, 4);
Uint16 ping_pong_i2sRxRightBuf2[2*I2S_RXBUFF_SZ]; /* 2x for 2nd ping/pong */
#endif //USE_TWO_CODEC

#ifdef USE_THREE_CODEC
#pragma DATA_ALIGN(ping_pong_i2sRxRightBuf2, 4);
Uint16 ping_pong_i2sRxRightBuf2[2*I2S_RXBUFF_SZ]; /* 2x for 2nd ping/pong */
#pragma DATA_ALIGN(ping_pong_i2sRxRightBuf3, 4);
Uint16 ping_pong_i2sRxRightBuf3[2*I2S_RXBUFF_SZ]; /* 2x for 3rd ping/pong */
#endif //USE_THREE_CODEC

#ifdef USE_FOUR_CODEC
#pragma DATA_ALIGN(ping_pong_i2sRxRightBuf2, 4);
Uint16 ping_pong_i2sRxRightBuf2[2*I2S_RXBUFF_SZ]; /* 2x for 2nd ping/pong */
#pragma DATA_ALIGN(ping_pong_i2sRxRightBuf3, 4);
Uint16 ping_pong_i2sRxRightBuf3[2*I2S_RXBUFF_SZ]; /* 2x for 3rd ping/pong */
#pragma DATA_ALIGN(ping_pong_i2sRxRightBuf4, 4);
Uint16 ping_pong_i2sRxRightBuf4[2*I2S_RXBUFF_SZ]; /* 2x for 4th ping/pong */
#endif //USE_FOUR_CODEC

Int16 left_rx_buf_sel = 0x0;
Int16 right_rx_buf_sel = 0x0;

// codec input buffer
Uint16 codec_input_buffer[CODEC_INPUT_BUFFER_SIZE];
//codec input buffer input index
Uint16 codec_input_buffer_input_index;
//codec input buffer output index
Uint16 codec_input_buffer_output_index;
Uint32 codec_input_buffer_overflow = 0;
Uint32 codec_input_buffer_underflow = 0;
Uint32 codec_input_buffer_sample = 0;

#endif  // ENABLE_RECORD

/* Codec output ping/pong buffer (Left ch.) */
/* NOTE: Left & Right interleaved channels for sample-by-sample playback */
//#pragma DATA_SECTION(ping_pong_i2sTxLeftBuf, ".ping_pong_i2sTxLeftBuf");
#pragma DATA_ALIGN(ping_pong_i2sTxLeftBuf, 4);
Int16 ping_pong_i2sTxLeftBuf[2*MAX_I2S_TXBUFF_SZ]; /* 2x for ping/pong */

#ifdef USE_TWO_CODEC
#pragma DATA_ALIGN(ping_pong_i2sTxLeftBuf2, 4);
Int16 ping_pong_i2sTxLeftBuf2[2*MAX_I2S_TXBUFF_SZ]; /* 2x for 2nd ping/pong */
#endif //USE_TWO_CODEC

#ifdef USE_THREE_CODEC
#pragma DATA_ALIGN(ping_pong_i2sTxLeftBuf2, 4);
Int16 ping_pong_i2sTxLeftBuf2[2*MAX_I2S_TXBUFF_SZ]; /* 2x for 2nd ping/pong */
#pragma DATA_ALIGN(ping_pong_i2sTxLeftBuf3, 4);
Int16 ping_pong_i2sTxLeftBuf3[2*MAX_I2S_TXBUFF_SZ]; /* 2x for 3rd ping/pong */
#endif //USE_THREE_CODEC

#ifdef USE_FOUR_CODEC
#pragma DATA_ALIGN(ping_pong_i2sTxLeftBuf2, 4);
Int16 ping_pong_i2sTxLeftBuf2[2*MAX_I2S_TXBUFF_SZ]; /* 2x for 2nd ping/pong */
#pragma DATA_ALIGN(ping_pong_i2sTxLeftBuf3, 4);
Int16 ping_pong_i2sTxLeftBuf3[2*MAX_I2S_TXBUFF_SZ]; /* 2x for 3rd ping/pong */
#pragma DATA_ALIGN(ping_pong_i2sTxLeftBuf4, 4);
Int16 ping_pong_i2sTxLeftBuf4[2*MAX_I2S_TXBUFF_SZ]; /* 2x for 4th ping/pong */
#endif //USE_FOUR_CODEC

/* Codec output ping/pong buffer (Right ch.) */
#pragma DATA_ALIGN(ping_pong_i2sTxRightBuf, 4);
Int16 ping_pong_i2sTxRightBuf[2*MAX_I2S_TXBUFF_SZ]; /* 2x for ping/pong */

#ifdef USE_TWO_CODEC
#pragma DATA_ALIGN(ping_pong_i2sTxRightBuf2, 4);
Int16 ping_pong_i2sTxRightBuf2[2*MAX_I2S_TXBUFF_SZ]; /* 2x for 2nd ping/pong */
#endif //USE_TWO_CODEC

#ifdef USE_THREE_CODEC
#pragma DATA_ALIGN(ping_pong_i2sTxRightBuf2, 4);
Int16 ping_pong_i2sTxRightBuf2[2*MAX_I2S_TXBUFF_SZ]; /* 2x for 2nd ping/pong */
#pragma DATA_ALIGN(ping_pong_i2sTxRightBu3, 4);
Int16 ping_pong_i2sTxRightBuf3[2*MAX_I2S_TXBUFF_SZ]; /* 2x for 3rd ping/pong */
#endif //USE_THREE_CODEC

#ifdef USE_FOUR_CODEC
#pragma DATA_ALIGN(ping_pong_i2sTxRightBuf2, 4);
Int16 ping_pong_i2sTxRightBuf2[2*MAX_I2S_TXBUFF_SZ]; /* 2x for 2nd ping/pong */
#pragma DATA_ALIGN(ping_pong_i2sTxRightBu3, 4);
Int16 ping_pong_i2sTxRightBuf3[2*MAX_I2S_TXBUFF_SZ]; /* 2x for 3rd ping/pong */
#pragma DATA_ALIGN(ping_pong_i2sTxRightBu4, 4);
Int16 ping_pong_i2sTxRightBuf4[2*MAX_I2S_TXBUFF_SZ]; /* 2x for 4th ping/pong */
#endif //USE_FOUR_CODEC

Int16 left_tx_buf_sel = 0x0;
Int16 right_tx_buf_sel = 0x0;

// codec output buffer
Uint16 codec_output_buffer[CODEC_OUTPUT_BUFFER_SIZE];
//codec input buffer input index
Uint16 codec_output_buffer_input_index;
//codec input buffer output index
Uint16 codec_output_buffer_output_index;
Uint32 codec_output_buffer_overflow = 0;
Uint32 codec_output_buffer_underflow = 0;
/* Run-time size of Tx ping/pong buffer */
Uint16 i2sTxBuffSz;
// sample counter for codec_output_buffer
Uint32 codec_output_buffer_sample = 0;

extern Uint32 dma_error;    /* SampleBySample */

/* Zeros I2S buffers for Playback */
static void zeroI2sBuf(
    Bool enableStereo, 
    Uint16 pingPongBufSz, 
    Int16 *pingPongI2sLeftBuf, 
    Int16 *pingPongI2sRightBuf
);

void i2sTxRxInit(void);
void i2sTxRxStart(void);

Int16 i2sPlayAudio(PSP_Handle        i2sHandle,
                   Uint32            *i2sNextTxLeftBuf,
                   Uint32            *i2sNextTxRightBuf
                   );

Int16 i2sReceiveData(PSP_Handle       i2sHandle,
                     Uint32           *i2sNextRxLeftBuf,
                     Uint32           *i2sNextRxRightBuf
                     );
Int16 i2sStopTransfer(PSP_Handle    i2sHandle);
void DDC_I2S_write(DDC_I2SHandle hI2s, Uint16 *writeBuff, Uint16 buffLen);

/* Initializes I2S and associated DMA channels for Playback and Record */
Int16 i2sInit(
    I2sInitPrms *pI2sInitPrms
)
{
    PSP_I2SOpMode opMode;
    PSP_I2SConfig i2sTxConfig;
    PSP_DMAConfig dmaTxConfig;
    PSP_I2SConfig i2sRxConfig;
    PSP_DMAConfig dmaRxConfig;

    if (pI2sInitPrms->enablePlayback == TRUE)
    {
        /* Initialize I2S instance for Playback */
        i2sTxConfig.dataformat  = PSP_I2S_DATAFORMAT_LJUST;
        i2sTxConfig.i2sMode     = PSP_I2S_SLAVE;
#ifdef SAMP_24BIT
        i2sTxConfig.word_len    = PSP_I2S_WORDLEN_24;
#else
        i2sTxConfig.word_len    = PSP_I2S_WORDLEN_16;
#endif
        //i2sTxConfig.signext     = PSP_I2S_SIGNEXT_ENABLE;
        i2sTxConfig.signext     = PSP_I2S_SIGNEXT_DISABLE;
        i2sTxConfig.datapack    = PSP_I2S_DATAPACK_DISABLE;
        i2sTxConfig.datadelay   = PSP_I2S_DATADELAY_ONEBIT;
        i2sTxConfig.clk_pol     = PSP_I2S_FALLING_EDGE;
        i2sTxConfig.fsync_pol   = PSP_I2S_FSPOL_LOW;
        i2sTxConfig.loopBack    = PSP_I2S_LOOPBACK_DISABLE;
        i2sTxConfig.datatype    = PSP_I2S_MONO_DISABLE;
        i2sTxConfig.fsdiv       = PSP_I2S_FSDIV32; /* not necessary for slave mode */
        i2sTxConfig.clkdiv      = PSP_I2S_CLKDIV2; /* not necessary for slave mode */

        if (pI2sInitPrms->sampleBySamplePb == TRUE)
        {
            opMode = PSP_I2S_INTERRUPT; /* change from PSP_DMA_INTERRUPT to PSP_I2S_INTERRUPT SampleBySample */
        }
        else
        {
            opMode = PSP_DMA_INTERRUPT;
        }

        i2sHandleTx = I2S_INIT(pI2sInitPrms->i2sPb, PSP_I2S_TRANSMIT, 
            PSP_I2S_CHAN_STEREO, opMode, &i2sTxConfig, NULL);
        if (i2sHandleTx == NULL)
        {
            return I2SSAMPLE_I2SINIT_PB_FAIL;
        }

        if (pI2sInitPrms->sampleBySamplePb == FALSE)
        {
            /* Initialize DMA channels for Playback */
            dmaTxConfig.pingPongMode    = TRUE;
            dmaTxConfig.autoMode        = PSP_DMA_AUTORELOAD_ENABLE;
            dmaTxConfig.burstLen        = PSP_DMA_TXBURST_1WORD;
            dmaTxConfig.chanDir         = PSP_DMA_WRITE;
            dmaTxConfig.trigger         = PSP_DMA_EVENT_TRIGGER;
            dmaTxConfig.trfType         = PSP_DMA_TRANSFER_IO_MEMORY;
            dmaTxConfig.dataLen         = 2*(2*MAX_I2S_TXBUFF_SZ); /* bytes */
            dmaTxConfig.srcAddr         = (Uint32)pI2sInitPrms->pingPongI2sTxLeftBuf;
            dmaTxConfig.callback        = I2S_DmaTxLChCallBack;

            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S0_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S0_I2STXLT0;
                break;
            case PSP_I2S_1:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S1_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S1_I2STXLT0;
                break;
            case PSP_I2S_2:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S2_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S2_I2STXLT0;
                break;
            case PSP_I2S_3:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S3_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S3_I2STXLT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for left channel data */
            hDmaTxLeft = I2S_DMA_INIT(i2sHandleTx, &dmaTxConfig);
            if (hDmaTxLeft == NULL)
            {
                return I2SSAMPLE_DMAINIT_PB_FAIL;
            }

            /* Request and configure a DMA channel for right channel data */
            dmaTxConfig.srcAddr   = (Uint32)pI2sInitPrms->pingPongI2sTxRightBuf;
            dmaTxConfig.callback  = I2S_DmaTxRChCallBack;

            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaTxConfig.destAddr    =  (Uint32)I2S0_I2STXRT0;
                break;
            case PSP_I2S_1:
                dmaTxConfig.destAddr    =  (Uint32)I2S1_I2STXRT0;
                break;
            case PSP_I2S_2:
                dmaTxConfig.destAddr    =  (Uint32)I2S2_I2STXRT0;
                break;
            case PSP_I2S_3:
                dmaTxConfig.destAddr    =  (Uint32)I2S3_I2STXRT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for right data */
            hDmaTxRight  =  I2S_DMA_INIT(i2sHandleTx, &dmaTxConfig);
            if (hDmaTxRight == NULL)
            {
                return I2SSAMPLE_DMAINIT_PB_FAIL;
            }
            }

            /* Zero buffers */
        zeroI2sBuf(pI2sInitPrms->enableStereoPb, 
            2*MAX_I2S_TXBUFF_SZ, 
            pI2sInitPrms->pingPongI2sTxLeftBuf, 
            pI2sInitPrms->pingPongI2sTxRightBuf);
    }

    if (pI2sInitPrms->enableRecord == TRUE)
    {
        /* Initialize I2S instance for Record */
        i2sRxConfig.dataformat   = PSP_I2S_DATAFORMAT_LJUST;
        i2sRxConfig.i2sMode      = PSP_I2S_SLAVE;
#ifdef SAMP_24BIT
        i2sRxConfig.word_len     = PSP_I2S_WORDLEN_24;
#else
        i2sRxConfig.word_len     = PSP_I2S_WORDLEN_16;
#endif
        //i2sRxConfig.signext      = PSP_I2S_SIGNEXT_ENABLE;
        i2sRxConfig.signext      = PSP_I2S_SIGNEXT_DISABLE;
        i2sRxConfig.datapack     = PSP_I2S_DATAPACK_DISABLE;
        i2sRxConfig.datadelay    = PSP_I2S_DATADELAY_ONEBIT;
        i2sRxConfig.clk_pol      = PSP_I2S_FALLING_EDGE;
        i2sRxConfig.fsync_pol    = PSP_I2S_FSPOL_LOW;
        i2sRxConfig.loopBack     = PSP_I2S_LOOPBACK_DISABLE;
        i2sRxConfig.datatype     = PSP_I2S_MONO_DISABLE;
        i2sRxConfig.fsdiv        = PSP_I2S_FSDIV32; /* not necessary for slave mode */
        i2sRxConfig.clkdiv       = PSP_I2S_CLKDIV2; /* not necessary for slave mode */

        i2sHandleRx = I2S_INIT(pI2sInitPrms->i2sRec, PSP_I2S_RECEIVE, 
            PSP_I2S_CHAN_STEREO, PSP_DMA_INTERRUPT, &i2sRxConfig, NULL);
        if (i2sHandleRx == NULL)
        {
            return I2SSAMPLE_I2SINIT_REC_FAIL;
        }

            /* Initialize DMA channels for Record */
        dmaRxConfig.pingPongMode    = TRUE;
        dmaRxConfig.autoMode        = PSP_DMA_AUTORELOAD_ENABLE;
            dmaRxConfig.burstLen        = PSP_DMA_TXBURST_1WORD;
            dmaRxConfig.chanDir         = PSP_DMA_READ;
            dmaRxConfig.trigger         = PSP_DMA_EVENT_TRIGGER;
            dmaRxConfig.trfType         = PSP_DMA_TRANSFER_IO_MEMORY;
        dmaRxConfig.dataLen         = 2*(2*I2S_RXBUFF_SZ); /* bytes */
        dmaRxConfig.destAddr        = (Uint32)pI2sInitPrms->pingPongI2sRxLeftBuf;
            dmaRxConfig.callback        = I2S_DmaRxLChCallBack;

            switch (pI2sInitPrms->i2sRec)
            {
            case PSP_I2S_0:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S0_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S0_I2SRXLT0;
                break;
            case PSP_I2S_1:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S1_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S1_I2SRXLT0;
                break;
            case PSP_I2S_2:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S2_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S2_I2SRXLT0;
                break;
            case PSP_I2S_3:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S3_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S3_I2SRXLT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for left channel data */
            hDmaRxLeft = I2S_DMA_INIT(i2sHandleRx, &dmaRxConfig);
            if (hDmaRxLeft == NULL)
            {
                return I2SSAMPLE_DMAINIT_REC_FAIL;
            }

            /* Request and configure a DMA channel for right channel data */
            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S0_I2SRXRT0;
                break;
            case PSP_I2S_1: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S1_I2SRXRT0;
                break;
            case PSP_I2S_2: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S2_I2SRXRT0;
                break;
            case PSP_I2S_3: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S3_I2SRXRT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

        dmaRxConfig.destAddr  =  (Uint32)pI2sInitPrms->pingPongI2sRxRightBuf;
            dmaRxConfig.callback = I2S_DmaRxRChCallBack;

            /* Request and configure a DMA channel for right data */
            hDmaRxRight = I2S_DMA_INIT(i2sHandleRx, &dmaRxConfig);
            if (hDmaRxRight == NULL)
            {
                return I2SSAMPLE_DMAINIT_REC_FAIL;
            }

        /* Zero buffers */
        zeroI2sBuf(pI2sInitPrms->enableStereoRec, 
            2*I2S_RXBUFF_SZ, 
            pI2sInitPrms->pingPongI2sRxLeftBuf, 
            pI2sInitPrms->pingPongI2sRxRightBuf);

    }
    //EZDSP5535_LED_on(0);
    return I2SSAMPLE_SOK;
}

#ifdef USE_TWO_CODEC
/* Initializes 2nd I2S and associated DMA channels for Playback and Record */
Int16 i2sInit2(
    I2sInitPrms *pI2sInitPrms
)
{
    PSP_I2SOpMode opMode;
    PSP_I2SConfig i2sTxConfig;
    PSP_DMAConfig dmaTxConfig;
    PSP_I2SConfig i2sRxConfig;
    PSP_DMAConfig dmaRxConfig;

    if (pI2sInitPrms->enablePlayback == TRUE)
    {
        /* Initialize I2S instance for Playback */
        i2sTxConfig.dataformat  = PSP_I2S_DATAFORMAT_LJUST;
        i2sTxConfig.i2sMode     = PSP_I2S_SLAVE;
#ifdef SAMP_24BIT
        i2sTxConfig.word_len    = PSP_I2S_WORDLEN_24;
#else
        i2sTxConfig.word_len    = PSP_I2S_WORDLEN_16;
#endif
        //i2sTxConfig.signext     = PSP_I2S_SIGNEXT_ENABLE;
        i2sTxConfig.signext     = PSP_I2S_SIGNEXT_DISABLE;
        i2sTxConfig.datapack    = PSP_I2S_DATAPACK_DISABLE;
        i2sTxConfig.datadelay   = PSP_I2S_DATADELAY_ONEBIT;
        i2sTxConfig.clk_pol     = PSP_I2S_FALLING_EDGE;
        i2sTxConfig.fsync_pol   = PSP_I2S_FSPOL_LOW;
        i2sTxConfig.loopBack    = PSP_I2S_LOOPBACK_DISABLE;
        i2sTxConfig.datatype    = PSP_I2S_MONO_DISABLE;
        i2sTxConfig.fsdiv       = PSP_I2S_FSDIV32; /* not necessary for slave mode */
        i2sTxConfig.clkdiv      = PSP_I2S_CLKDIV2; /* not necessary for slave mode */

        if (pI2sInitPrms->sampleBySamplePb == TRUE)
        {
            opMode = PSP_I2S_INTERRUPT; /* change from PSP_DMA_INTERRUPT to PSP_I2S_INTERRUPT SampleBySample */
        }
        else
        {
            opMode = PSP_DMA_INTERRUPT;
        }

        i2sHandleTx2 = I2S_INIT(pI2sInitPrms->i2sPb, PSP_I2S_TRANSMIT, 
            PSP_I2S_CHAN_STEREO, opMode, &i2sTxConfig, NULL);
        if (i2sHandleTx2 == NULL)
        {
            return I2SSAMPLE_I2SINIT_PB_FAIL;
        }

        if (pI2sInitPrms->sampleBySamplePb == FALSE)
        {
            /* Initialize DMA channels for Playback */
            dmaTxConfig.pingPongMode    = TRUE;
            dmaTxConfig.autoMode        = PSP_DMA_AUTORELOAD_ENABLE;
            dmaTxConfig.burstLen        = PSP_DMA_TXBURST_1WORD;
            dmaTxConfig.chanDir         = PSP_DMA_WRITE;
            dmaTxConfig.trigger         = PSP_DMA_EVENT_TRIGGER;
            dmaTxConfig.trfType         = PSP_DMA_TRANSFER_IO_MEMORY;
            dmaTxConfig.dataLen         = 2*(2*MAX_I2S_TXBUFF_SZ); /* bytes */
            dmaTxConfig.srcAddr         = (Uint32)pI2sInitPrms->pingPongI2sTxLeftBuf;
            dmaTxConfig.callback        = I2S_DmaTxLChCallBack2;

            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S0_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S0_I2STXLT0;
                break;
            case PSP_I2S_1:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S1_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S1_I2STXLT0;
                break;
            case PSP_I2S_2:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S2_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S2_I2STXLT0;
                break;
            case PSP_I2S_3:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S3_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S3_I2STXLT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for left channel data */
            hDmaTxLeft2 = I2S_DMA_INIT(i2sHandleTx2, &dmaTxConfig);
            if (hDmaTxLeft2 == NULL)
            {
                return I2SSAMPLE_DMAINIT_PB_FAIL;
            }

            /* Request and configure a DMA channel for right channel data */
            dmaTxConfig.srcAddr   = (Uint32)pI2sInitPrms->pingPongI2sTxRightBuf;
            dmaTxConfig.callback  = I2S_DmaTxRChCallBack2;

            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaTxConfig.destAddr    =  (Uint32)I2S0_I2STXRT0;
                break;
            case PSP_I2S_1:
                dmaTxConfig.destAddr    =  (Uint32)I2S1_I2STXRT0;
                break;
            case PSP_I2S_2:
                dmaTxConfig.destAddr    =  (Uint32)I2S2_I2STXRT0;
                break;
            case PSP_I2S_3:
                dmaTxConfig.destAddr    =  (Uint32)I2S3_I2STXRT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for right data */
            hDmaTxRight2  =  I2S_DMA_INIT(i2sHandleTx2, &dmaTxConfig);
            if (hDmaTxRight2 == NULL)
            {
                return I2SSAMPLE_DMAINIT_PB_FAIL;
            }
            }

            /* Zero buffers */
        zeroI2sBuf(pI2sInitPrms->enableStereoPb, 
            2*MAX_I2S_TXBUFF_SZ, 
            pI2sInitPrms->pingPongI2sTxLeftBuf, 
            pI2sInitPrms->pingPongI2sTxRightBuf);
    }

    if (pI2sInitPrms->enableRecord == TRUE)
    {
        /* Initialize I2S instance for Record */
        i2sRxConfig.dataformat   = PSP_I2S_DATAFORMAT_LJUST;
        i2sRxConfig.i2sMode      = PSP_I2S_SLAVE;
#ifdef SAMP_24BIT
        i2sRxConfig.word_len     = PSP_I2S_WORDLEN_24;
#else
        i2sRxConfig.word_len     = PSP_I2S_WORDLEN_16;
#endif
        //i2sRxConfig.signext      = PSP_I2S_SIGNEXT_ENABLE;
        i2sRxConfig.signext      = PSP_I2S_SIGNEXT_DISABLE;
        i2sRxConfig.datapack     = PSP_I2S_DATAPACK_DISABLE;
        i2sRxConfig.datadelay    = PSP_I2S_DATADELAY_ONEBIT;
        i2sRxConfig.clk_pol      = PSP_I2S_FALLING_EDGE;
        i2sRxConfig.fsync_pol    = PSP_I2S_FSPOL_LOW;
        i2sRxConfig.loopBack     = PSP_I2S_LOOPBACK_DISABLE;
        i2sRxConfig.datatype     = PSP_I2S_MONO_DISABLE;
        i2sRxConfig.fsdiv        = PSP_I2S_FSDIV32; /* not necessary for slave mode */
        i2sRxConfig.clkdiv       = PSP_I2S_CLKDIV2; /* not necessary for slave mode */

        i2sHandleRx2 = I2S_INIT(pI2sInitPrms->i2sRec, PSP_I2S_RECEIVE, 
            PSP_I2S_CHAN_STEREO, PSP_DMA_INTERRUPT, &i2sRxConfig, NULL);
        if (i2sHandleRx2 == NULL)
        {
            return I2SSAMPLE_I2SINIT_REC_FAIL;
        }

            /* Initialize DMA channels for Record */
        dmaRxConfig.pingPongMode    = TRUE;
        dmaRxConfig.autoMode        = PSP_DMA_AUTORELOAD_ENABLE;
            dmaRxConfig.burstLen        = PSP_DMA_TXBURST_1WORD;
            dmaRxConfig.chanDir         = PSP_DMA_READ;
            dmaRxConfig.trigger         = PSP_DMA_EVENT_TRIGGER;
            dmaRxConfig.trfType         = PSP_DMA_TRANSFER_IO_MEMORY;
        dmaRxConfig.dataLen         = 2*(2*I2S_RXBUFF_SZ); /* bytes */
        dmaRxConfig.destAddr        = (Uint32)pI2sInitPrms->pingPongI2sRxLeftBuf;
            dmaRxConfig.callback        = I2S_DmaRxLChCallBack2;

            switch (pI2sInitPrms->i2sRec)
            {
            case PSP_I2S_0:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S0_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S0_I2SRXLT0;
                break;
            case PSP_I2S_1:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S1_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S1_I2SRXLT0;
                break;
            case PSP_I2S_2:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S2_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S2_I2SRXLT0;
                break;
            case PSP_I2S_3:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S3_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S3_I2SRXLT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for left channel data */
            hDmaRxLeft2 = I2S_DMA_INIT(i2sHandleRx2, &dmaRxConfig);
            if (hDmaRxLeft2 == NULL)
            {
                return I2SSAMPLE_DMAINIT_REC_FAIL;
            }

            /* Request and configure a DMA channel for right channel data */
            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S0_I2SRXRT0;
                break;
            case PSP_I2S_1: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S1_I2SRXRT0;
                break;
            case PSP_I2S_2: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S2_I2SRXRT0;
                break;
            case PSP_I2S_3: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S3_I2SRXRT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

        dmaRxConfig.destAddr  =  (Uint32)pI2sInitPrms->pingPongI2sRxRightBuf;
            dmaRxConfig.callback = I2S_DmaRxRChCallBack2;

            /* Request and configure a DMA channel for right data */
            hDmaRxRight2 = I2S_DMA_INIT(i2sHandleRx2, &dmaRxConfig);
            if (hDmaRxRight2 == NULL)
            {
                return I2SSAMPLE_DMAINIT_REC_FAIL;
            }

        /* Zero buffers */
        zeroI2sBuf(pI2sInitPrms->enableStereoRec, 
            2*I2S_RXBUFF_SZ, 
            pI2sInitPrms->pingPongI2sRxLeftBuf, 
            pI2sInitPrms->pingPongI2sRxRightBuf);

    }

    return I2SSAMPLE_SOK;
}
#endif // USE_TWO_CODEC

#ifdef USE_THREE_CODEC
/* Initializes 2nd I2S and associated DMA channels for Playback and Record */
Int16 i2sInit2(
    I2sInitPrms *pI2sInitPrms
)
{
    PSP_I2SOpMode opMode;
    PSP_I2SConfig i2sTxConfig;
    PSP_DMAConfig dmaTxConfig;
    PSP_I2SConfig i2sRxConfig;
    PSP_DMAConfig dmaRxConfig;

    if (pI2sInitPrms->enablePlayback == TRUE)
    {
        /* Initialize I2S instance for Playback */
        i2sTxConfig.dataformat  = PSP_I2S_DATAFORMAT_LJUST;
        i2sTxConfig.i2sMode     = PSP_I2S_SLAVE;
#ifdef SAMP_24BIT
        i2sTxConfig.word_len    = PSP_I2S_WORDLEN_24;
#else
        i2sTxConfig.word_len    = PSP_I2S_WORDLEN_16;
#endif
        //i2sTxConfig.signext     = PSP_I2S_SIGNEXT_ENABLE;
        i2sTxConfig.signext     = PSP_I2S_SIGNEXT_DISABLE;
        i2sTxConfig.datapack    = PSP_I2S_DATAPACK_DISABLE;
        i2sTxConfig.datadelay   = PSP_I2S_DATADELAY_ONEBIT;
        i2sTxConfig.clk_pol     = PSP_I2S_FALLING_EDGE;
        i2sTxConfig.fsync_pol   = PSP_I2S_FSPOL_LOW;
        i2sTxConfig.loopBack    = PSP_I2S_LOOPBACK_DISABLE;
        i2sTxConfig.datatype    = PSP_I2S_MONO_DISABLE;
        i2sTxConfig.fsdiv       = PSP_I2S_FSDIV32; /* not necessary for slave mode */
        i2sTxConfig.clkdiv      = PSP_I2S_CLKDIV2; /* not necessary for slave mode */

        if (pI2sInitPrms->sampleBySamplePb == TRUE)
        {
            opMode = PSP_I2S_INTERRUPT; /* change from PSP_DMA_INTERRUPT to PSP_I2S_INTERRUPT SampleBySample */
        }
        else
        {
            opMode = PSP_DMA_INTERRUPT;
        }

        i2sHandleTx2 = I2S_INIT(pI2sInitPrms->i2sPb, PSP_I2S_TRANSMIT, 
            PSP_I2S_CHAN_STEREO, opMode, &i2sTxConfig, NULL);
        if (i2sHandleTx2 == NULL)
        {
            return I2SSAMPLE_I2SINIT_PB_FAIL;
        }

        if (pI2sInitPrms->sampleBySamplePb == FALSE)
        {
            /* Initialize DMA channels for Playback */
            dmaTxConfig.pingPongMode    = TRUE;
            dmaTxConfig.autoMode        = PSP_DMA_AUTORELOAD_ENABLE;
            dmaTxConfig.burstLen        = PSP_DMA_TXBURST_1WORD;
            dmaTxConfig.chanDir         = PSP_DMA_WRITE;
            dmaTxConfig.trigger         = PSP_DMA_EVENT_TRIGGER;
            dmaTxConfig.trfType         = PSP_DMA_TRANSFER_IO_MEMORY;
            dmaTxConfig.dataLen         = 2*(2*MAX_I2S_TXBUFF_SZ); /* bytes */
            dmaTxConfig.srcAddr         = (Uint32)pI2sInitPrms->pingPongI2sTxLeftBuf;
            dmaTxConfig.callback        = I2S_DmaTxLChCallBack2;

            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S0_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S0_I2STXLT0;
                break;
            case PSP_I2S_1:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S1_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S1_I2STXLT0;
                break;
            case PSP_I2S_2:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S2_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S2_I2STXLT0;
                break;
            case PSP_I2S_3:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S3_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S3_I2STXLT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for left channel data */
            hDmaTxLeft2 = I2S_DMA_INIT(i2sHandleTx2, &dmaTxConfig);
            if (hDmaTxLeft2 == NULL)
            {
                return I2SSAMPLE_DMAINIT_PB_FAIL;
            }

            /* Request and configure a DMA channel for right channel data */
            dmaTxConfig.srcAddr   = (Uint32)pI2sInitPrms->pingPongI2sTxRightBuf;
            dmaTxConfig.callback  = I2S_DmaTxRChCallBack2;

            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaTxConfig.destAddr    =  (Uint32)I2S0_I2STXRT0;
                break;
            case PSP_I2S_1:
                dmaTxConfig.destAddr    =  (Uint32)I2S1_I2STXRT0;
                break;
            case PSP_I2S_2:
                dmaTxConfig.destAddr    =  (Uint32)I2S2_I2STXRT0;
                break;
            case PSP_I2S_3:
                dmaTxConfig.destAddr    =  (Uint32)I2S3_I2STXRT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for right data */
            hDmaTxRight2  =  I2S_DMA_INIT(i2sHandleTx2, &dmaTxConfig);
            if (hDmaTxRight2 == NULL)
            {
                return I2SSAMPLE_DMAINIT_PB_FAIL;
            }
            }

            /* Zero buffers */
        zeroI2sBuf(pI2sInitPrms->enableStereoPb, 
            2*MAX_I2S_TXBUFF_SZ, 
            pI2sInitPrms->pingPongI2sTxLeftBuf, 
            pI2sInitPrms->pingPongI2sTxRightBuf);
    }

    if (pI2sInitPrms->enableRecord == TRUE)
    {
        /* Initialize I2S instance for Record */
        i2sRxConfig.dataformat   = PSP_I2S_DATAFORMAT_LJUST;
        i2sRxConfig.i2sMode      = PSP_I2S_SLAVE;
#ifdef SAMP_24BIT
        i2sRxConfig.word_len     = PSP_I2S_WORDLEN_24;
#else
        i2sRxConfig.word_len     = PSP_I2S_WORDLEN_16;
#endif
        //i2sRxConfig.signext      = PSP_I2S_SIGNEXT_ENABLE;
        i2sRxConfig.signext      = PSP_I2S_SIGNEXT_DISABLE;
        i2sRxConfig.datapack     = PSP_I2S_DATAPACK_DISABLE;
        i2sRxConfig.datadelay    = PSP_I2S_DATADELAY_ONEBIT;
        i2sRxConfig.clk_pol      = PSP_I2S_FALLING_EDGE;
        i2sRxConfig.fsync_pol    = PSP_I2S_FSPOL_LOW;
        i2sRxConfig.loopBack     = PSP_I2S_LOOPBACK_DISABLE;
        i2sRxConfig.datatype     = PSP_I2S_MONO_DISABLE;
        i2sRxConfig.fsdiv        = PSP_I2S_FSDIV32; /* not necessary for slave mode */
        i2sRxConfig.clkdiv       = PSP_I2S_CLKDIV2; /* not necessary for slave mode */

        i2sHandleRx2 = I2S_INIT(pI2sInitPrms->i2sRec, PSP_I2S_RECEIVE, 
            PSP_I2S_CHAN_STEREO, PSP_DMA_INTERRUPT, &i2sRxConfig, NULL);
        if (i2sHandleRx2 == NULL)
        {
            return I2SSAMPLE_I2SINIT_REC_FAIL;
        }

            /* Initialize DMA channels for Record */
        dmaRxConfig.pingPongMode    = TRUE;
        dmaRxConfig.autoMode        = PSP_DMA_AUTORELOAD_ENABLE;
            dmaRxConfig.burstLen        = PSP_DMA_TXBURST_1WORD;
            dmaRxConfig.chanDir         = PSP_DMA_READ;
            dmaRxConfig.trigger         = PSP_DMA_EVENT_TRIGGER;
            dmaRxConfig.trfType         = PSP_DMA_TRANSFER_IO_MEMORY;
        dmaRxConfig.dataLen         = 2*(2*I2S_RXBUFF_SZ); /* bytes */
        dmaRxConfig.destAddr        = (Uint32)pI2sInitPrms->pingPongI2sRxLeftBuf;
            dmaRxConfig.callback        = I2S_DmaRxLChCallBack2;

            switch (pI2sInitPrms->i2sRec)
            {
            case PSP_I2S_0:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S0_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S0_I2SRXLT0;
                break;
            case PSP_I2S_1:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S1_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S1_I2SRXLT0;
                break;
            case PSP_I2S_2:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S2_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S2_I2SRXLT0;
                break;
            case PSP_I2S_3:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S3_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S3_I2SRXLT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for left channel data */
            hDmaRxLeft2 = I2S_DMA_INIT(i2sHandleRx2, &dmaRxConfig);
            if (hDmaRxLeft2 == NULL)
            {
                return I2SSAMPLE_DMAINIT_REC_FAIL;
            }

            /* Request and configure a DMA channel for right channel data */
            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S0_I2SRXRT0;
                break;
            case PSP_I2S_1: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S1_I2SRXRT0;
                break;
            case PSP_I2S_2: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S2_I2SRXRT0;
                break;
            case PSP_I2S_3: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S3_I2SRXRT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

        dmaRxConfig.destAddr  =  (Uint32)pI2sInitPrms->pingPongI2sRxRightBuf;
            dmaRxConfig.callback = I2S_DmaRxRChCallBack2;

            /* Request and configure a DMA channel for right data */
            hDmaRxRight2 = I2S_DMA_INIT(i2sHandleRx2, &dmaRxConfig);
            if (hDmaRxRight2 == NULL)
            {
                return I2SSAMPLE_DMAINIT_REC_FAIL;
            }

        /* Zero buffers */
        zeroI2sBuf(pI2sInitPrms->enableStereoRec, 
            2*I2S_RXBUFF_SZ, 
            pI2sInitPrms->pingPongI2sRxLeftBuf, 
            pI2sInitPrms->pingPongI2sRxRightBuf);

    }

    return I2SSAMPLE_SOK;
}

/* Initializes rd I2S and associated DMA channels for Playback and Record */
Int16 i2sInit3(
    I2sInitPrms *pI2sInitPrms
)
{
    PSP_I2SOpMode opMode;
    PSP_I2SConfig i2sTxConfig;
    PSP_DMAConfig dmaTxConfig;
    PSP_I2SConfig i2sRxConfig;
    PSP_DMAConfig dmaRxConfig;

    if (pI2sInitPrms->enablePlayback == TRUE)
    {
        /* Initialize I2S instance for Playback */
        i2sTxConfig.dataformat  = PSP_I2S_DATAFORMAT_LJUST;
        i2sTxConfig.i2sMode     = PSP_I2S_SLAVE;
#ifdef SAMP_24BIT
        i2sTxConfig.word_len    = PSP_I2S_WORDLEN_24;
#else
        i2sTxConfig.word_len    = PSP_I2S_WORDLEN_16;
#endif
        //i2sTxConfig.signext     = PSP_I2S_SIGNEXT_ENABLE;
        i2sTxConfig.signext     = PSP_I2S_SIGNEXT_DISABLE;
        i2sTxConfig.datapack    = PSP_I2S_DATAPACK_DISABLE;
        i2sTxConfig.datadelay   = PSP_I2S_DATADELAY_ONEBIT;
        i2sTxConfig.clk_pol     = PSP_I2S_FALLING_EDGE;
        i2sTxConfig.fsync_pol   = PSP_I2S_FSPOL_LOW;
        i2sTxConfig.loopBack    = PSP_I2S_LOOPBACK_DISABLE;
        i2sTxConfig.datatype    = PSP_I2S_MONO_DISABLE;
        i2sTxConfig.fsdiv       = PSP_I2S_FSDIV32; /* not necessary for slave mode */
        i2sTxConfig.clkdiv      = PSP_I2S_CLKDIV2; /* not necessary for slave mode */

        if (pI2sInitPrms->sampleBySamplePb == TRUE)
        {
            opMode = PSP_I2S_INTERRUPT; /* change from PSP_DMA_INTERRUPT to PSP_I2S_INTERRUPT SampleBySample */
        }
        else
        {
            opMode = PSP_DMA_INTERRUPT;
        }

        i2sHandleTx3 = I2S_INIT(pI2sInitPrms->i2sPb, PSP_I2S_TRANSMIT, 
            PSP_I2S_CHAN_STEREO, opMode, &i2sTxConfig, NULL);
        if (i2sHandleTx3 == NULL)
        {
            return I2SSAMPLE_I2SINIT_PB_FAIL;
        }

        if (pI2sInitPrms->sampleBySamplePb == FALSE)
        {
            /* Initialize DMA channels for Playback */
            dmaTxConfig.pingPongMode    = TRUE;
            dmaTxConfig.autoMode        = PSP_DMA_AUTORELOAD_ENABLE;
            dmaTxConfig.burstLen        = PSP_DMA_TXBURST_1WORD;
            dmaTxConfig.chanDir         = PSP_DMA_WRITE;
            dmaTxConfig.trigger         = PSP_DMA_EVENT_TRIGGER;
            dmaTxConfig.trfType         = PSP_DMA_TRANSFER_IO_MEMORY;
            dmaTxConfig.dataLen         = 2*(2*MAX_I2S_TXBUFF_SZ); /* bytes */
            dmaTxConfig.srcAddr         = (Uint32)pI2sInitPrms->pingPongI2sTxLeftBuf;
            dmaTxConfig.callback        = I2S_DmaTxLChCallBack3;

            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S0_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S0_I2STXLT0;
                break;
            case PSP_I2S_1:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S1_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S1_I2STXLT0;
                break;
            case PSP_I2S_2:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S2_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S2_I2STXLT0;
                break;
            case PSP_I2S_3:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S3_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S3_I2STXLT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for left channel data */
            hDmaTxLeft3 = I2S_DMA_INIT(i2sHandleTx3, &dmaTxConfig);
            if (hDmaTxLeft3 == NULL)
            {
                return I2SSAMPLE_DMAINIT_PB_FAIL;
            }

            /* Request and configure a DMA channel for right channel data */
            dmaTxConfig.srcAddr   = (Uint32)pI2sInitPrms->pingPongI2sTxRightBuf;
            dmaTxConfig.callback  = I2S_DmaTxRChCallBack3;

            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaTxConfig.destAddr    =  (Uint32)I2S0_I2STXRT0;
                break;
            case PSP_I2S_1:
                dmaTxConfig.destAddr    =  (Uint32)I2S1_I2STXRT0;
                break;
            case PSP_I2S_2:
                dmaTxConfig.destAddr    =  (Uint32)I2S2_I2STXRT0;
                break;
            case PSP_I2S_3:
                dmaTxConfig.destAddr    =  (Uint32)I2S3_I2STXRT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for right data */
            hDmaTxRight3  =  I2S_DMA_INIT(i2sHandleTx3, &dmaTxConfig);
            if (hDmaTxRight3 == NULL)
            {
                return I2SSAMPLE_DMAINIT_PB_FAIL;
            }
            }

            /* Zero buffers */
        zeroI2sBuf(pI2sInitPrms->enableStereoPb, 
            2*MAX_I2S_TXBUFF_SZ, 
            pI2sInitPrms->pingPongI2sTxLeftBuf, 
            pI2sInitPrms->pingPongI2sTxRightBuf);
    }

    if (pI2sInitPrms->enableRecord == TRUE)
    {
        /* Initialize I2S instance for Record */
        i2sRxConfig.dataformat   = PSP_I2S_DATAFORMAT_LJUST;
        i2sRxConfig.i2sMode      = PSP_I2S_SLAVE;
#ifdef SAMP_24BIT
        i2sRxConfig.word_len     = PSP_I2S_WORDLEN_24;
#else
        i2sRxConfig.word_len     = PSP_I2S_WORDLEN_16;
#endif
        //i2sRxConfig.signext      = PSP_I2S_SIGNEXT_ENABLE;
        i2sRxConfig.signext      = PSP_I2S_SIGNEXT_DISABLE;
        i2sRxConfig.datapack     = PSP_I2S_DATAPACK_DISABLE;
        i2sRxConfig.datadelay    = PSP_I2S_DATADELAY_ONEBIT;
        i2sRxConfig.clk_pol      = PSP_I2S_FALLING_EDGE;
        i2sRxConfig.fsync_pol    = PSP_I2S_FSPOL_LOW;
        i2sRxConfig.loopBack     = PSP_I2S_LOOPBACK_DISABLE;
        i2sRxConfig.datatype     = PSP_I2S_MONO_DISABLE;
        i2sRxConfig.fsdiv        = PSP_I2S_FSDIV32; /* not necessary for slave mode */
        i2sRxConfig.clkdiv       = PSP_I2S_CLKDIV2; /* not necessary for slave mode */

        i2sHandleRx3 = I2S_INIT(pI2sInitPrms->i2sRec, PSP_I2S_RECEIVE, 
            PSP_I2S_CHAN_STEREO, PSP_DMA_INTERRUPT, &i2sRxConfig, NULL);
        if (i2sHandleRx3 == NULL)
        {
            return I2SSAMPLE_I2SINIT_REC_FAIL;
        }

            /* Initialize DMA channels for Record */
        dmaRxConfig.pingPongMode    = TRUE;
        dmaRxConfig.autoMode        = PSP_DMA_AUTORELOAD_ENABLE;
            dmaRxConfig.burstLen        = PSP_DMA_TXBURST_1WORD;
            dmaRxConfig.chanDir         = PSP_DMA_READ;
            dmaRxConfig.trigger         = PSP_DMA_EVENT_TRIGGER;
            dmaRxConfig.trfType         = PSP_DMA_TRANSFER_IO_MEMORY;
        dmaRxConfig.dataLen         = 2*(2*I2S_RXBUFF_SZ); /* bytes */
        dmaRxConfig.destAddr        = (Uint32)pI2sInitPrms->pingPongI2sRxLeftBuf;
            dmaRxConfig.callback        = I2S_DmaRxLChCallBack3;

            switch (pI2sInitPrms->i2sRec)
            {
            case PSP_I2S_0:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S0_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S0_I2SRXLT0;
                break;
            case PSP_I2S_1:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S1_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S1_I2SRXLT0;
                break;
            case PSP_I2S_2:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S2_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S2_I2SRXLT0;
                break;
            case PSP_I2S_3:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S3_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S3_I2SRXLT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for left channel data */
            hDmaRxLeft3 = I2S_DMA_INIT(i2sHandleRx3, &dmaRxConfig);
            if (hDmaRxLeft3 == NULL)
            {
                return I2SSAMPLE_DMAINIT_REC_FAIL;
            }

            /* Request and configure a DMA channel for right channel data */
            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S0_I2SRXRT0;
                break;
            case PSP_I2S_1: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S1_I2SRXRT0;
                break;
            case PSP_I2S_2: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S2_I2SRXRT0;
                break;
            case PSP_I2S_3: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S3_I2SRXRT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

        dmaRxConfig.destAddr  =  (Uint32)pI2sInitPrms->pingPongI2sRxRightBuf;
            dmaRxConfig.callback = I2S_DmaRxRChCallBack3;

            /* Request and configure a DMA channel for right data */
            hDmaRxRight3 = I2S_DMA_INIT(i2sHandleRx3, &dmaRxConfig);
            if (hDmaRxRight3 == NULL)
            {
                return I2SSAMPLE_DMAINIT_REC_FAIL;
            }

        /* Zero buffers */
        zeroI2sBuf(pI2sInitPrms->enableStereoRec, 
            2*I2S_RXBUFF_SZ, 
            pI2sInitPrms->pingPongI2sRxLeftBuf, 
            pI2sInitPrms->pingPongI2sRxRightBuf);

    }

    return I2SSAMPLE_SOK;
}
#endif // USE_THREE_CODEC

#ifdef USE_FOUR_CODEC
/* Initializes 2nd I2S and associated DMA channels for Playback and Record */
Int16 i2sInit2(
    I2sInitPrms *pI2sInitPrms
)
{
    PSP_I2SOpMode opMode;
    PSP_I2SConfig i2sTxConfig;
    PSP_DMAConfig dmaTxConfig;
    PSP_I2SConfig i2sRxConfig;
    PSP_DMAConfig dmaRxConfig;

    if (pI2sInitPrms->enablePlayback == TRUE)
    {
        /* Initialize I2S instance for Playback */
        i2sTxConfig.dataformat  = PSP_I2S_DATAFORMAT_LJUST;
        i2sTxConfig.i2sMode     = PSP_I2S_SLAVE;
#ifdef SAMP_24BIT
        i2sTxConfig.word_len    = PSP_I2S_WORDLEN_24;
#else
        i2sTxConfig.word_len    = PSP_I2S_WORDLEN_16;
#endif
        //i2sTxConfig.signext     = PSP_I2S_SIGNEXT_ENABLE;
        i2sTxConfig.signext     = PSP_I2S_SIGNEXT_DISABLE;
        i2sTxConfig.datapack    = PSP_I2S_DATAPACK_DISABLE;
        i2sTxConfig.datadelay   = PSP_I2S_DATADELAY_ONEBIT;
        i2sTxConfig.clk_pol     = PSP_I2S_FALLING_EDGE;
        i2sTxConfig.fsync_pol   = PSP_I2S_FSPOL_LOW;
        i2sTxConfig.loopBack    = PSP_I2S_LOOPBACK_DISABLE;
        i2sTxConfig.datatype    = PSP_I2S_MONO_DISABLE;
        i2sTxConfig.fsdiv       = PSP_I2S_FSDIV32; /* not necessary for slave mode */
        i2sTxConfig.clkdiv      = PSP_I2S_CLKDIV2; /* not necessary for slave mode */

        if (pI2sInitPrms->sampleBySamplePb == TRUE)
        {
            opMode = PSP_I2S_INTERRUPT; /* change from PSP_DMA_INTERRUPT to PSP_I2S_INTERRUPT SampleBySample */
        }
        else
        {
            opMode = PSP_DMA_INTERRUPT;
        }

        i2sHandleTx2 = I2S_INIT(pI2sInitPrms->i2sPb, PSP_I2S_TRANSMIT, 
            PSP_I2S_CHAN_STEREO, opMode, &i2sTxConfig, NULL);
        if (i2sHandleTx2 == NULL)
        {
            return I2SSAMPLE_I2SINIT_PB_FAIL;
        }

        if (pI2sInitPrms->sampleBySamplePb == FALSE)
        {
            /* Initialize DMA channels for Playback */
            dmaTxConfig.pingPongMode    = TRUE;
            dmaTxConfig.autoMode        = PSP_DMA_AUTORELOAD_ENABLE;
            dmaTxConfig.burstLen        = PSP_DMA_TXBURST_1WORD;
            dmaTxConfig.chanDir         = PSP_DMA_WRITE;
            dmaTxConfig.trigger         = PSP_DMA_EVENT_TRIGGER;
            dmaTxConfig.trfType         = PSP_DMA_TRANSFER_IO_MEMORY;
            dmaTxConfig.dataLen         = 2*(2*MAX_I2S_TXBUFF_SZ); /* bytes */
            dmaTxConfig.srcAddr         = (Uint32)pI2sInitPrms->pingPongI2sTxLeftBuf;
            dmaTxConfig.callback        = I2S_DmaTxLChCallBack2;

            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S0_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S0_I2STXLT0;
                break;
            case PSP_I2S_1:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S1_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S1_I2STXLT0;
                break;
            case PSP_I2S_2:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S2_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S2_I2STXLT0;
                break;
            case PSP_I2S_3:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S3_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S3_I2STXLT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for left channel data */
            hDmaTxLeft2 = I2S_DMA_INIT(i2sHandleTx2, &dmaTxConfig);
            if (hDmaTxLeft2 == NULL)
            {
                return I2SSAMPLE_DMAINIT_PB_FAIL;
            }

            /* Request and configure a DMA channel for right channel data */
            dmaTxConfig.srcAddr   = (Uint32)pI2sInitPrms->pingPongI2sTxRightBuf;
            dmaTxConfig.callback  = I2S_DmaTxRChCallBack2;

            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaTxConfig.destAddr    =  (Uint32)I2S0_I2STXRT0;
                break;
            case PSP_I2S_1:
                dmaTxConfig.destAddr    =  (Uint32)I2S1_I2STXRT0;
                break;
            case PSP_I2S_2:
                dmaTxConfig.destAddr    =  (Uint32)I2S2_I2STXRT0;
                break;
            case PSP_I2S_3:
                dmaTxConfig.destAddr    =  (Uint32)I2S3_I2STXRT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for right data */
            hDmaTxRight2  =  I2S_DMA_INIT(i2sHandleTx2, &dmaTxConfig);
            if (hDmaTxRight2 == NULL)
            {
                return I2SSAMPLE_DMAINIT_PB_FAIL;
            }
            }

            /* Zero buffers */
        zeroI2sBuf(pI2sInitPrms->enableStereoPb, 
            2*MAX_I2S_TXBUFF_SZ, 
            pI2sInitPrms->pingPongI2sTxLeftBuf, 
            pI2sInitPrms->pingPongI2sTxRightBuf);
    }

    if (pI2sInitPrms->enableRecord == TRUE)
    {
        /* Initialize I2S instance for Record */
        i2sRxConfig.dataformat   = PSP_I2S_DATAFORMAT_LJUST;
        i2sRxConfig.i2sMode      = PSP_I2S_SLAVE;
#ifdef SAMP_24BIT
        i2sRxConfig.word_len     = PSP_I2S_WORDLEN_24;
#else
        i2sRxConfig.word_len     = PSP_I2S_WORDLEN_16;
#endif
        //i2sRxConfig.signext      = PSP_I2S_SIGNEXT_ENABLE;
        i2sRxConfig.signext      = PSP_I2S_SIGNEXT_DISABLE;
        i2sRxConfig.datapack     = PSP_I2S_DATAPACK_DISABLE;
        i2sRxConfig.datadelay    = PSP_I2S_DATADELAY_ONEBIT;
        i2sRxConfig.clk_pol      = PSP_I2S_FALLING_EDGE;
        i2sRxConfig.fsync_pol    = PSP_I2S_FSPOL_LOW;
        i2sRxConfig.loopBack     = PSP_I2S_LOOPBACK_DISABLE;
        i2sRxConfig.datatype     = PSP_I2S_MONO_DISABLE;
        i2sRxConfig.fsdiv        = PSP_I2S_FSDIV32; /* not necessary for slave mode */
        i2sRxConfig.clkdiv       = PSP_I2S_CLKDIV2; /* not necessary for slave mode */

        i2sHandleRx2 = I2S_INIT(pI2sInitPrms->i2sRec, PSP_I2S_RECEIVE, 
            PSP_I2S_CHAN_STEREO, PSP_DMA_INTERRUPT, &i2sRxConfig, NULL);
        if (i2sHandleRx2 == NULL)
        {
            return I2SSAMPLE_I2SINIT_REC_FAIL;
        }

            /* Initialize DMA channels for Record */
        dmaRxConfig.pingPongMode    = TRUE;
        dmaRxConfig.autoMode        = PSP_DMA_AUTORELOAD_ENABLE;
            dmaRxConfig.burstLen        = PSP_DMA_TXBURST_1WORD;
            dmaRxConfig.chanDir         = PSP_DMA_READ;
            dmaRxConfig.trigger         = PSP_DMA_EVENT_TRIGGER;
            dmaRxConfig.trfType         = PSP_DMA_TRANSFER_IO_MEMORY;
        dmaRxConfig.dataLen         = 2*(2*I2S_RXBUFF_SZ); /* bytes */
        dmaRxConfig.destAddr        = (Uint32)pI2sInitPrms->pingPongI2sRxLeftBuf;
            dmaRxConfig.callback        = I2S_DmaRxLChCallBack2;

            switch (pI2sInitPrms->i2sRec)
            {
            case PSP_I2S_0:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S0_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S0_I2SRXLT0;
                break;
            case PSP_I2S_1:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S1_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S1_I2SRXLT0;
                break;
            case PSP_I2S_2:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S2_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S2_I2SRXLT0;
                break;
            case PSP_I2S_3:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S3_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S3_I2SRXLT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for left channel data */
            hDmaRxLeft2 = I2S_DMA_INIT(i2sHandleRx2, &dmaRxConfig);
            if (hDmaRxLeft2 == NULL)
            {
                return I2SSAMPLE_DMAINIT_REC_FAIL;
            }

            /* Request and configure a DMA channel for right channel data */
            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S0_I2SRXRT0;
                break;
            case PSP_I2S_1: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S1_I2SRXRT0;
                break;
            case PSP_I2S_2: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S2_I2SRXRT0;
                break;
            case PSP_I2S_3: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S3_I2SRXRT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

        dmaRxConfig.destAddr  =  (Uint32)pI2sInitPrms->pingPongI2sRxRightBuf;
            dmaRxConfig.callback = I2S_DmaRxRChCallBack2;

            /* Request and configure a DMA channel for right data */
            hDmaRxRight2 = I2S_DMA_INIT(i2sHandleRx2, &dmaRxConfig);
            if (hDmaRxRight2 == NULL)
            {
                return I2SSAMPLE_DMAINIT_REC_FAIL;
            }

        /* Zero buffers */
        zeroI2sBuf(pI2sInitPrms->enableStereoRec, 
            2*I2S_RXBUFF_SZ, 
            pI2sInitPrms->pingPongI2sRxLeftBuf, 
            pI2sInitPrms->pingPongI2sRxRightBuf);

    }

    return I2SSAMPLE_SOK;
}

/* Initializes 3rd I2S and associated DMA channels for Playback and Record */
Int16 i2sInit3(
    I2sInitPrms *pI2sInitPrms
)
{
    PSP_I2SOpMode opMode;
    PSP_I2SConfig i2sTxConfig;
    PSP_DMAConfig dmaTxConfig;
    PSP_I2SConfig i2sRxConfig;
    PSP_DMAConfig dmaRxConfig;

    if (pI2sInitPrms->enablePlayback == TRUE)
    {
        /* Initialize I2S instance for Playback */
        i2sTxConfig.dataformat  = PSP_I2S_DATAFORMAT_LJUST;
        i2sTxConfig.i2sMode     = PSP_I2S_SLAVE;
#ifdef SAMP_24BIT
        i2sTxConfig.word_len    = PSP_I2S_WORDLEN_24;
#else
        i2sTxConfig.word_len    = PSP_I2S_WORDLEN_16;
#endif
        //i2sTxConfig.signext     = PSP_I2S_SIGNEXT_ENABLE;
        i2sTxConfig.signext     = PSP_I2S_SIGNEXT_DISABLE;
        i2sTxConfig.datapack    = PSP_I2S_DATAPACK_DISABLE;
        i2sTxConfig.datadelay   = PSP_I2S_DATADELAY_ONEBIT;
        i2sTxConfig.clk_pol     = PSP_I2S_FALLING_EDGE;
        i2sTxConfig.fsync_pol   = PSP_I2S_FSPOL_LOW;
        i2sTxConfig.loopBack    = PSP_I2S_LOOPBACK_DISABLE;
        i2sTxConfig.datatype    = PSP_I2S_MONO_DISABLE;
        i2sTxConfig.fsdiv       = PSP_I2S_FSDIV32; /* not necessary for slave mode */
        i2sTxConfig.clkdiv      = PSP_I2S_CLKDIV2; /* not necessary for slave mode */

        if (pI2sInitPrms->sampleBySamplePb == TRUE)
        {
            opMode = PSP_I2S_INTERRUPT; /* change from PSP_DMA_INTERRUPT to PSP_I2S_INTERRUPT SampleBySample */
        }
        else
        {
            opMode = PSP_DMA_INTERRUPT;
        }

        i2sHandleTx3 = I2S_INIT(pI2sInitPrms->i2sPb, PSP_I2S_TRANSMIT, 
            PSP_I2S_CHAN_STEREO, opMode, &i2sTxConfig, NULL);
        if (i2sHandleTx3 == NULL)
        {
            return I2SSAMPLE_I2SINIT_PB_FAIL;
        }

        if (pI2sInitPrms->sampleBySamplePb == FALSE)
        {
            /* Initialize DMA channels for Playback */
            dmaTxConfig.pingPongMode    = TRUE;
            dmaTxConfig.autoMode        = PSP_DMA_AUTORELOAD_ENABLE;
            dmaTxConfig.burstLen        = PSP_DMA_TXBURST_1WORD;
            dmaTxConfig.chanDir         = PSP_DMA_WRITE;
            dmaTxConfig.trigger         = PSP_DMA_EVENT_TRIGGER;
            dmaTxConfig.trfType         = PSP_DMA_TRANSFER_IO_MEMORY;
            dmaTxConfig.dataLen         = 2*(2*MAX_I2S_TXBUFF_SZ); /* bytes */
            dmaTxConfig.srcAddr         = (Uint32)pI2sInitPrms->pingPongI2sTxLeftBuf;
            dmaTxConfig.callback        = I2S_DmaTxLChCallBack3;

            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S0_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S0_I2STXLT0;
                break;
            case PSP_I2S_1:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S1_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S1_I2STXLT0;
                break;
            case PSP_I2S_2:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S2_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S2_I2STXLT0;
                break;
            case PSP_I2S_3:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S3_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S3_I2STXLT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for left channel data */
            hDmaTxLeft3 = I2S_DMA_INIT(i2sHandleTx3, &dmaTxConfig);
            if (hDmaTxLeft3 == NULL)
            {
                return I2SSAMPLE_DMAINIT_PB_FAIL;
            }

            /* Request and configure a DMA channel for right channel data */
            dmaTxConfig.srcAddr   = (Uint32)pI2sInitPrms->pingPongI2sTxRightBuf;
            dmaTxConfig.callback  = I2S_DmaTxRChCallBack3;

            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaTxConfig.destAddr    =  (Uint32)I2S0_I2STXRT0;
                break;
            case PSP_I2S_1:
                dmaTxConfig.destAddr    =  (Uint32)I2S1_I2STXRT0;
                break;
            case PSP_I2S_2:
                dmaTxConfig.destAddr    =  (Uint32)I2S2_I2STXRT0;
                break;
            case PSP_I2S_3:
                dmaTxConfig.destAddr    =  (Uint32)I2S3_I2STXRT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for right data */
            hDmaTxRight3  =  I2S_DMA_INIT(i2sHandleTx3, &dmaTxConfig);
            if (hDmaTxRight3 == NULL)
            {
                return I2SSAMPLE_DMAINIT_PB_FAIL;
            }
            }

            /* Zero buffers */
        zeroI2sBuf(pI2sInitPrms->enableStereoPb, 
            2*MAX_I2S_TXBUFF_SZ, 
            pI2sInitPrms->pingPongI2sTxLeftBuf, 
            pI2sInitPrms->pingPongI2sTxRightBuf);
    }

    if (pI2sInitPrms->enableRecord == TRUE)
    {
        /* Initialize I2S instance for Record */
        i2sRxConfig.dataformat   = PSP_I2S_DATAFORMAT_LJUST;
        i2sRxConfig.i2sMode      = PSP_I2S_SLAVE;
#ifdef SAMP_24BIT
        i2sRxConfig.word_len     = PSP_I2S_WORDLEN_24;
#else
        i2sRxConfig.word_len     = PSP_I2S_WORDLEN_16;
#endif
        //i2sRxConfig.signext      = PSP_I2S_SIGNEXT_ENABLE;
        i2sRxConfig.signext      = PSP_I2S_SIGNEXT_DISABLE;
        i2sRxConfig.datapack     = PSP_I2S_DATAPACK_DISABLE;
        i2sRxConfig.datadelay    = PSP_I2S_DATADELAY_ONEBIT;
        i2sRxConfig.clk_pol      = PSP_I2S_FALLING_EDGE;
        i2sRxConfig.fsync_pol    = PSP_I2S_FSPOL_LOW;
        i2sRxConfig.loopBack     = PSP_I2S_LOOPBACK_DISABLE;
        i2sRxConfig.datatype     = PSP_I2S_MONO_DISABLE;
        i2sRxConfig.fsdiv        = PSP_I2S_FSDIV32; /* not necessary for slave mode */
        i2sRxConfig.clkdiv       = PSP_I2S_CLKDIV2; /* not necessary for slave mode */

        i2sHandleRx3 = I2S_INIT(pI2sInitPrms->i2sRec, PSP_I2S_RECEIVE, 
            PSP_I2S_CHAN_STEREO, PSP_DMA_INTERRUPT, &i2sRxConfig, NULL);
        if (i2sHandleRx3 == NULL)
        {
            return I2SSAMPLE_I2SINIT_REC_FAIL;
        }

            /* Initialize DMA channels for Record */
        dmaRxConfig.pingPongMode    = TRUE;
        dmaRxConfig.autoMode        = PSP_DMA_AUTORELOAD_ENABLE;
            dmaRxConfig.burstLen        = PSP_DMA_TXBURST_1WORD;
            dmaRxConfig.chanDir         = PSP_DMA_READ;
            dmaRxConfig.trigger         = PSP_DMA_EVENT_TRIGGER;
            dmaRxConfig.trfType         = PSP_DMA_TRANSFER_IO_MEMORY;
        dmaRxConfig.dataLen         = 2*(2*I2S_RXBUFF_SZ); /* bytes */
        dmaRxConfig.destAddr        = (Uint32)pI2sInitPrms->pingPongI2sRxLeftBuf;
            dmaRxConfig.callback        = I2S_DmaRxLChCallBack3;

            switch (pI2sInitPrms->i2sRec)
            {
            case PSP_I2S_0:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S0_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S0_I2SRXLT0;
                break;
            case PSP_I2S_1:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S1_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S1_I2SRXLT0;
                break;
            case PSP_I2S_2:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S2_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S2_I2SRXLT0;
                break;
            case PSP_I2S_3:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S3_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S3_I2SRXLT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for left channel data */
            hDmaRxLeft3 = I2S_DMA_INIT(i2sHandleRx3, &dmaRxConfig);
            if (hDmaRxLeft3 == NULL)
            {
                return I2SSAMPLE_DMAINIT_REC_FAIL;
            }

            /* Request and configure a DMA channel for right channel data */
            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S0_I2SRXRT0;
                break;
            case PSP_I2S_1: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S1_I2SRXRT0;
                break;
            case PSP_I2S_2: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S2_I2SRXRT0;
                break;
            case PSP_I2S_3: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S3_I2SRXRT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

        dmaRxConfig.destAddr  =  (Uint32)pI2sInitPrms->pingPongI2sRxRightBuf;
            dmaRxConfig.callback = I2S_DmaRxRChCallBack3;

            /* Request and configure a DMA channel for right data */
            hDmaRxRight3 = I2S_DMA_INIT(i2sHandleRx3, &dmaRxConfig);
            if (hDmaRxRight3 == NULL)
            {
                return I2SSAMPLE_DMAINIT_REC_FAIL;
            }

        /* Zero buffers */
        zeroI2sBuf(pI2sInitPrms->enableStereoRec, 
            2*I2S_RXBUFF_SZ, 
            pI2sInitPrms->pingPongI2sRxLeftBuf, 
            pI2sInitPrms->pingPongI2sRxRightBuf);

    }

    return I2SSAMPLE_SOK;
}

/* Initializes 4th I2S and associated DMA channels for Playback and Record */
Int16 i2sInit4(
    I2sInitPrms *pI2sInitPrms
)
{
    PSP_I2SOpMode opMode;
    PSP_I2SConfig i2sTxConfig;
    PSP_DMAConfig dmaTxConfig;
    PSP_I2SConfig i2sRxConfig;
    PSP_DMAConfig dmaRxConfig;

    if (pI2sInitPrms->enablePlayback == TRUE)
    {
        /* Initialize I2S instance for Playback */
        i2sTxConfig.dataformat  = PSP_I2S_DATAFORMAT_LJUST;
        i2sTxConfig.i2sMode     = PSP_I2S_SLAVE;
#ifdef SAMP_24BIT
        i2sTxConfig.word_len    = PSP_I2S_WORDLEN_24;
#else
        i2sTxConfig.word_len    = PSP_I2S_WORDLEN_16;
#endif
        //i2sTxConfig.signext     = PSP_I2S_SIGNEXT_ENABLE;
        i2sTxConfig.signext     = PSP_I2S_SIGNEXT_DISABLE;
        i2sTxConfig.datapack    = PSP_I2S_DATAPACK_DISABLE;
        i2sTxConfig.datadelay   = PSP_I2S_DATADELAY_ONEBIT;
        i2sTxConfig.clk_pol     = PSP_I2S_FALLING_EDGE;
        i2sTxConfig.fsync_pol   = PSP_I2S_FSPOL_LOW;
        i2sTxConfig.loopBack    = PSP_I2S_LOOPBACK_DISABLE;
        i2sTxConfig.datatype    = PSP_I2S_MONO_DISABLE;
        i2sTxConfig.fsdiv       = PSP_I2S_FSDIV32; /* not necessary for slave mode */
        i2sTxConfig.clkdiv      = PSP_I2S_CLKDIV2; /* not necessary for slave mode */

        if (pI2sInitPrms->sampleBySamplePb == TRUE)
        {
            opMode = PSP_I2S_INTERRUPT; /* change from PSP_DMA_INTERRUPT to PSP_I2S_INTERRUPT SampleBySample */
        }
        else
        {
            opMode = PSP_DMA_INTERRUPT;
        }

        i2sHandleTx4 = I2S_INIT(pI2sInitPrms->i2sPb, PSP_I2S_TRANSMIT, 
            PSP_I2S_CHAN_STEREO, opMode, &i2sTxConfig, NULL);
        if (i2sHandleTx4 == NULL)
        {
            return I2SSAMPLE_I2SINIT_PB_FAIL;
        }

        if (pI2sInitPrms->sampleBySamplePb == FALSE)
        {
            /* Initialize DMA channels for Playback */
            dmaTxConfig.pingPongMode    = TRUE;
            dmaTxConfig.autoMode        = PSP_DMA_AUTORELOAD_ENABLE;
            dmaTxConfig.burstLen        = PSP_DMA_TXBURST_1WORD;
            dmaTxConfig.chanDir         = PSP_DMA_WRITE;
            dmaTxConfig.trigger         = PSP_DMA_EVENT_TRIGGER;
            dmaTxConfig.trfType         = PSP_DMA_TRANSFER_IO_MEMORY;
            dmaTxConfig.dataLen         = 2*(2*MAX_I2S_TXBUFF_SZ); /* bytes */
            dmaTxConfig.srcAddr         = (Uint32)pI2sInitPrms->pingPongI2sTxLeftBuf;
            dmaTxConfig.callback        = I2S_DmaTxLChCallBack4;

            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S0_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S0_I2STXLT0;
                break;
            case PSP_I2S_1:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S1_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S1_I2STXLT0;
                break;
            case PSP_I2S_2:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S2_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S2_I2STXLT0;
                break;
            case PSP_I2S_3:
                dmaTxConfig.dmaEvt      =  PSP_DMA_EVT_I2S3_TX;
                dmaTxConfig.destAddr    =  (Uint32)I2S3_I2STXLT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for left channel data */
            hDmaTxLeft4 = I2S_DMA_INIT(i2sHandleTx4, &dmaTxConfig);
            if (hDmaTxLeft4 == NULL)
            {
                return I2SSAMPLE_DMAINIT_PB_FAIL;
            }

            /* Request and configure a DMA channel for right channel data */
            dmaTxConfig.srcAddr   = (Uint32)pI2sInitPrms->pingPongI2sTxRightBuf;
            dmaTxConfig.callback  = I2S_DmaTxRChCallBack4;

            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaTxConfig.destAddr    =  (Uint32)I2S0_I2STXRT0;
                break;
            case PSP_I2S_1:
                dmaTxConfig.destAddr    =  (Uint32)I2S1_I2STXRT0;
                break;
            case PSP_I2S_2:
                dmaTxConfig.destAddr    =  (Uint32)I2S2_I2STXRT0;
                break;
            case PSP_I2S_3:
                dmaTxConfig.destAddr    =  (Uint32)I2S3_I2STXRT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for right data */
            hDmaTxRight4  =  I2S_DMA_INIT(i2sHandleTx4, &dmaTxConfig);
            if (hDmaTxRight4 == NULL)
            {
                return I2SSAMPLE_DMAINIT_PB_FAIL;
            }
            }

            /* Zero buffers */
        zeroI2sBuf(pI2sInitPrms->enableStereoPb, 
            2*MAX_I2S_TXBUFF_SZ, 
            pI2sInitPrms->pingPongI2sTxLeftBuf, 
            pI2sInitPrms->pingPongI2sTxRightBuf);
    }

    if (pI2sInitPrms->enableRecord == TRUE)
    {
        /* Initialize I2S instance for Record */
        i2sRxConfig.dataformat   = PSP_I2S_DATAFORMAT_LJUST;
        i2sRxConfig.i2sMode      = PSP_I2S_SLAVE;
#ifdef SAMP_24BIT
        i2sRxConfig.word_len     = PSP_I2S_WORDLEN_24;
#else
        i2sRxConfig.word_len     = PSP_I2S_WORDLEN_16;
#endif
        //i2sRxConfig.signext      = PSP_I2S_SIGNEXT_ENABLE;
        i2sRxConfig.signext      = PSP_I2S_SIGNEXT_DISABLE;
        i2sRxConfig.datapack     = PSP_I2S_DATAPACK_DISABLE;
        i2sRxConfig.datadelay    = PSP_I2S_DATADELAY_ONEBIT;
        i2sRxConfig.clk_pol      = PSP_I2S_FALLING_EDGE;
        i2sRxConfig.fsync_pol    = PSP_I2S_FSPOL_LOW;
        i2sRxConfig.loopBack     = PSP_I2S_LOOPBACK_DISABLE;
        i2sRxConfig.datatype     = PSP_I2S_MONO_DISABLE;
        i2sRxConfig.fsdiv        = PSP_I2S_FSDIV32; /* not necessary for slave mode */
        i2sRxConfig.clkdiv       = PSP_I2S_CLKDIV2; /* not necessary for slave mode */

        i2sHandleRx4 = I2S_INIT(pI2sInitPrms->i2sRec, PSP_I2S_RECEIVE, 
            PSP_I2S_CHAN_STEREO, PSP_DMA_INTERRUPT, &i2sRxConfig, NULL);
        if (i2sHandleRx4 == NULL)
        {
            return I2SSAMPLE_I2SINIT_REC_FAIL;
        }

            /* Initialize DMA channels for Record */
        dmaRxConfig.pingPongMode    = TRUE;
        dmaRxConfig.autoMode        = PSP_DMA_AUTORELOAD_ENABLE;
            dmaRxConfig.burstLen        = PSP_DMA_TXBURST_1WORD;
            dmaRxConfig.chanDir         = PSP_DMA_READ;
            dmaRxConfig.trigger         = PSP_DMA_EVENT_TRIGGER;
            dmaRxConfig.trfType         = PSP_DMA_TRANSFER_IO_MEMORY;
        dmaRxConfig.dataLen         = 2*(2*I2S_RXBUFF_SZ); /* bytes */
        dmaRxConfig.destAddr        = (Uint32)pI2sInitPrms->pingPongI2sRxLeftBuf;
            dmaRxConfig.callback        = I2S_DmaRxLChCallBack4;

            switch (pI2sInitPrms->i2sRec)
            {
            case PSP_I2S_0:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S0_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S0_I2SRXLT0;
                break;
            case PSP_I2S_1:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S1_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S1_I2SRXLT0;
                break;
            case PSP_I2S_2:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S2_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S2_I2SRXLT0;
                break;
            case PSP_I2S_3:
                dmaRxConfig.dmaEvt      =  PSP_DMA_EVT_I2S3_RX;
                dmaRxConfig.srcAddr     =  (Uint32)I2S3_I2SRXLT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

            /* Request and configure a DMA channel for left channel data */
            hDmaRxLeft4 = I2S_DMA_INIT(i2sHandleRx4, &dmaRxConfig);
            if (hDmaRxLeft4 == NULL)
            {
                return I2SSAMPLE_DMAINIT_REC_FAIL;
            }

            /* Request and configure a DMA channel for right channel data */
            switch (pI2sInitPrms->i2sPb)
            {
            case PSP_I2S_0: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S0_I2SRXRT0;
                break;
            case PSP_I2S_1: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S1_I2SRXRT0;
                break;
            case PSP_I2S_2: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S2_I2SRXRT0;
                break;
            case PSP_I2S_3: 
                dmaRxConfig.srcAddr   =  (Uint32)I2S3_I2SRXRT0;
                break;
            default:
                return I2SSAMPLE_INV_PRMS;
            }

        dmaRxConfig.destAddr  =  (Uint32)pI2sInitPrms->pingPongI2sRxRightBuf;
            dmaRxConfig.callback = I2S_DmaRxRChCallBack4;

            /* Request and configure a DMA channel for right data */
            hDmaRxRight4 = I2S_DMA_INIT(i2sHandleRx4, &dmaRxConfig);
            if (hDmaRxRight4 == NULL)
            {
                return I2SSAMPLE_DMAINIT_REC_FAIL;
            }

        /* Zero buffers */
        zeroI2sBuf(pI2sInitPrms->enableStereoRec, 
            2*I2S_RXBUFF_SZ, 
            pI2sInitPrms->pingPongI2sRxLeftBuf, 
            pI2sInitPrms->pingPongI2sRxRightBuf);

    }

    return I2SSAMPLE_SOK;
}
#endif // USE_FOUR_CODEC

/* Zeros I2S buffers */
void zeroI2sBuf(
    Bool enableStereo, 
    Uint16 pingPongBufSz, 
    Int16 *pingPongI2sLeftBuf, 
    Int16 *pingPongI2sRightBuf
)
{
	volatile int i;

    /* Clear left channel */
    ///memset(pingPongI2sLeftBuf, 0, 2*pingPongBufSz);
	for (i=0; i<pingPongBufSz; i++)
		pingPongI2sLeftBuf[i] = 0;

    /* Clear right channel */
    if (enableStereo == TRUE)
    {
        ///memset(pingPongI2sRightBuf, 0, 2*pingPongBufSz);
		for (i=0; i<pingPongBufSz; i++)
			pingPongI2sRightBuf[i] = 0;
    }
}

/* Function to play an audio on I2S */
Int16 i2sPlayAudio(PSP_Handle        i2sHandle,
                   Uint32            *i2sNextTxLeftBuf,
                   Uint32            *i2sNextTxRightBuf
                  )
{
    Int16 status = PSP_E_INVAL_PARAM;

    if((i2sHandle != NULL)        &&
       (i2sNextTxLeftBuf != NULL) &&
       (i2sNextTxRightBuf != NULL))
    {
        status = I2S_TransmitData(i2sHandle, i2sNextTxLeftBuf, i2sNextTxRightBuf,
                                  hDmaTxLeft, hDmaTxRight);
    }

    return status;
}

/* Stops the I2S data transfer */
Int16 i2sStopTransfer(PSP_Handle    i2sHandle)
{
    Int16 status;

    if(i2sHandle != NULL)
    {
        /* Release the DMA channels */
        status   =  I2S_DMA_Deinit(hDmaTxLeft);
        status  |=  I2S_DMA_Deinit(hDmaTxRight);

        #ifdef ENABLE_RECORD
    //    status   =  I2S_DMA_Deinit(hDmaRxLeft);
    //    status  |=  I2S_DMA_Deinit(hDmaRxRight);
        #endif // ENABLE_RECORD

        /* Deinitialize the I2S instance */
    //    status  |=  I2S_DeInit(i2sHandle);
    }

    return status;
}

/* Resets codec output buffer */
void reset_codec_output_buffer(void)
{
	Uint16 i;

    //memset(ping_pong_i2sTxLeftBuf, 0, 4*MAX_I2S_TXBUFF_SZ); /* 2x for ping/pong */
    //memset(ping_pong_i2sTxRightBuf, 0, 2*MAX_I2S_TXBUFF_SZ); /* 2x for ping/pong */
    for (i=0; i<(2*MAX_I2S_TXBUFF_SZ); i++)
	{
		ping_pong_i2sTxLeftBuf[i] = 0;
		ping_pong_i2sTxRightBuf[i] = 0;
#ifdef USE_TWO_CODEC
		ping_pong_i2sTxLeftBuf2[i] = 0;
		ping_pong_i2sTxRightBuf2[i] = 0;
#endif //USE_TWO_CODEC
#ifdef USE_THREE_CODEC
		ping_pong_i2sTxLeftBuf2[i] = 0;
		ping_pong_i2sTxRightBuf2[i] = 0;
		ping_pong_i2sTxLeftBuf3[i] = 0;
		ping_pong_i2sTxRightBuf3[i] = 0;
#endif //USE_THREE_CODEC
#ifdef USE_FOUR_CODEC
		ping_pong_i2sTxLeftBuf2[i] = 0;
		ping_pong_i2sTxRightBuf2[i] = 0;
		ping_pong_i2sTxLeftBuf3[i] = 0;
		ping_pong_i2sTxRightBuf3[i] = 0;
		ping_pong_i2sTxLeftBuf4[i] = 0;
		ping_pong_i2sTxRightBuf4[i] = 0;
#endif //USE_FOUR_CODEC
	}
    //memset(codec_output_buffer, 0, 2*CODEC_OUTPUT_BUFFER_SIZE);
	for (i=0; i<CODEC_OUTPUT_BUFFER_SIZE; i++)
	{
		codec_output_buffer[i] = 0;
	}
    left_tx_buf_sel = 0x0;
    right_tx_buf_sel = 0x0;
	//codec_output_buffer_input_index  = CODEC_OUTPUT_BUFFER_SIZE/2;
	codec_output_buffer_input_index  = 0;
	codec_output_buffer_output_index = 0;
	codec_output_buffer_overflow = 0;
	codec_output_buffer_underflow = 0;
	i2sTxBuffSz = MAX_I2S_TXBUFF_SZ;
}

/* SampleBySample
 * copy of CSL I2S_transEnable function to work with DDC_I2SObj type handle
 ***********************************************************************/
void DDC_I2S_transEnable(DDC_I2SHandle    hI2s, Uint16 enableBit)
{
    ioport    CSL_I2sDrvRegs      *localregs;

    localregs =     hI2s->regs;
    //localregs->SCRL = 0x2A00;

    if(enableBit == TRUE)
    {
        /*  Enable the transmit and receive bit */
        CSL_FINST(localregs->SCRL, I2S_I2SSCTRL_ENABLE, SET);
    }
    else
    {
        /*  Disable the transmit and receive bit */
        CSL_FINST(localregs->SCRL, I2S_I2SSCTRL_ENABLE, CLEAR);
    }
}

/* SampleBySample
 * copy of CSL I2S_write function to work with DDC_I2SObj type handle
 ***********************************************************************/
void DDC_I2S_write(DDC_I2SHandle    hI2s,
                Uint16 *writeBuff, Uint16 buffLen)
{
    ioport    CSL_I2sDrvRegs      *localregs;
    Uint16    i2sIrStatus;

    if((NULL == writeBuff) || (0 == buffLen))
    {
        return;
    }

    localregs = hI2s->regs;

        while(buffLen > 0)
        {
            /* Copy data from local buffer to transmit register  */
            localregs->TRW0M = *writeBuff++;
            if(hI2s->chanType == PSP_I2S_CHAN_STEREO) //I2S_CHAN_STEREO)
            {
                localregs->TRW1M = *writeBuff++;
                buffLen -= 1;
            }
            buffLen -= 1;
        }
        // check for errors
        i2sIrStatus = localregs->IRL;

        if(i2sIrStatus & CSL_I2S_I2SINTFL_FERRFL_MASK)
        {
            fsError1++;
        }

        if(i2sIrStatus & CSL_I2S_I2SINTFL_OUERRFL_MASK)
        {
            ouError1++;
        }
}
