/*
 * $$$MODULE_NAME app_asrc.c
 *
 * $$$MODULE_DESC app_asrc.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#include <std.h>
#include <string.h>
#include "data_types.h"
#include "app_globals.h"
#include "dda_dma.h"
#include "app_usb.h"
#include "app_usbac.h"
#include "app_asrc.h"
#include "VC5505_CSL_BIOS_cfg.h"

Uint32 gUsbPbSampRateHz;    /* current USB playback sampling rate */
Uint32 gDacSampRateHz;      /* current DAC sampling rate */

/* Pitch calculation variables */
Uint16 dmaChanNum;          /* DMA channel number for Left Channel Tx DMA (0-15) */
Uint32 dmaBaseAddr;         /* 16 LSBs Left Channel Tx DMA source base address */
Uint16 dmaFrameLenSamps;    /* DMA frame length in samples */
volatile ioport Uint16 *dmaStartAddrReg0; /* I/O space address of Left Channel Tx DMA Source Start Address register */
volatile ioport Uint16 *dmaStartAddrReg1; /* I/O space address of Left Channel Tx DMA Source Start Address register */
Int16 dmaCXferCnt;          /* current count of completed DMA transfers between SOF's for Left Channel Tx DMA */
Uint16 numCDmaXfer;         /* number of completed DMA transfers between SOF's for Left Channel Tx DMA */
Uint32 dmaCurAddr;          /* 16 LSBs Left Channel Tx DMA source address -- byte address */
Bool firstSof;              /* indicates whether to initiate DMA sample count computation */
Uint16 dmaCurCnt;           /* DMA transfer count in frame for current SOF */
Uint16 dmaPrevCnt;          /* DMA transfer count in frame for previous SOF */
Uint16 i2sRxSampCnt;        /* Rx sample count */

/* Initializes pitch calculation */
Int16 initPitchCalc(
    DMA_ChanHandle hDmaTxLeft
)
{
    Uint16 dmaFrameLenWords;
    Uint32 pingPongBaseAddr, pingPongEndAddr;
    Uint16 dmaEngNum, dmaEngChanNum;
    CSL_DmaDrvRegsOvly dmaRegs;

    /* Compute DMA frame length in 16-bit words */
    /* Divide by 2: 2x for bytes */
    dmaFrameLenWords = (Uint16)(hDmaTxLeft->dataLen>>1);

    /* Check DMA ping/pong buffer contained in 64K data page */
    pingPongBaseAddr = (Uint32)hDmaTxLeft->srcAddr;
    pingPongEndAddr = pingPongBaseAddr + dmaFrameLenWords - 1;
    if ((pingPongBaseAddr & 0xF0000) == (pingPongEndAddr & 0xF0000))
    {
        /* Compute DMA frame length in samples */
        /* Divide by 4: 2x for unpacked, 2x for ping/pong, 
        i.e. assumes UNPACKED I2S mode and DMA HW PING/PONG */
        dmaFrameLenSamps = dmaFrameLenWords>>2;

        /* Get 16 LSBs of DMA ping/pong buffer */
        dmaBaseAddr = hDmaTxLeft->srcAddr;

        dmaEngNum = hDmaTxLeft->chanNum/DMA_ENGINE_COUNT;
        switch(dmaEngNum)
        {
        case DMA_ENGINE0:
            dmaRegs = CSL_DMA_REGS0;
            break;
        case DMA_ENGINE1:
            dmaRegs = CSL_DMA_REGS1;
            break;
        case DMA_ENGINE2:
            dmaRegs = CSL_DMA_REGS2;
            break;
        case DMA_ENGINE3:
            dmaRegs = CSL_DMA_REGS3;
            break;
        }

        dmaEngChanNum = hDmaTxLeft->chanNum - dmaEngNum*DMA_ENGINE_COUNT;
        switch (dmaEngChanNum)
        {
        case DMA_CHAN0:
            dmaStartAddrReg0 = 
                (volatile ioport Uint16 *)&dmaRegs->DMACH0SADR0;
            dmaStartAddrReg1 = 
                (volatile ioport Uint16 *)&dmaRegs->DMACH0SADR1;
            break;
        case DMA_CHAN1:
            dmaStartAddrReg0 = 
                (volatile ioport Uint16 *)&dmaRegs->DMACH1SADR0;
            dmaStartAddrReg1 = 
                (volatile ioport Uint16 *)&dmaRegs->DMACH1SADR1;
            break;
        case DMA_CHAN2:
            dmaStartAddrReg0 = 
                (volatile ioport Uint16 *)&dmaRegs->DMACH2SADR0;
            dmaStartAddrReg1 = 
                (volatile ioport Uint16 *)&dmaRegs->DMACH2SADR1;
            break;
        case DMA_CHAN3:
            dmaStartAddrReg0 = 
                (volatile ioport Uint16 *)&dmaRegs->DMACH3SADR0;
            dmaStartAddrReg1 = 
                (volatile ioport Uint16 *)&dmaRegs->DMACH3SADR1;
            break;
        }

        dmaChanNum = hDmaTxLeft->chanNum; /* init. DMA channel number, 0-15 */
        dmaCXferCnt = 0; /* init. DMA completed transfer count */
        firstSof = TRUE;
    }
    else
    {
        return AASRC_PCALC_INV_PPBUF; /* error -- ping/pong buffer spans 64K word boundary */
    }

    return AASRC_SOK;
}

/* Latches current DMA transfer state */
void latchDmaXferState(void)
{
    Uint16 ifrValue;

    /* Sample DMA source address */
    dmaCurAddr = *dmaStartAddrReg1;
    dmaCurAddr <<= 16;
    dmaCurAddr |= *dmaStartAddrReg0;
    /* Latch and clear DMA completed transfer count */
    numCDmaXfer = dmaCXferCnt;
    dmaCXferCnt = 0;

    ifrValue = CSL_DMAEVTINT_REGS->DMAIFR;
    if (((ifrValue>>dmaChanNum)&0x1) == 0x1)
    {
        /* Resample DMA source address */
	    dmaCurAddr = *dmaStartAddrReg1;
	    dmaCurAddr <<= 16;
	    dmaCurAddr |= *dmaStartAddrReg0;
        /* Adjust DMA completed transfer count */
        numCDmaXfer += 1;
        /* Ajust DMA completed transfer count so current 
        completed transfer not counted more than once */
        dmaCXferCnt = -1;
    }
}

/* Compute DMA transfer count since last SOF */
Uint16 computeDmaXferCnt(void)
{
    Uint16 sampCnt;

    if (firstSof == FALSE)
    {           
        dmaPrevCnt = dmaCurCnt;

        /* Compute current DMA transer count in frame */
        dmaCurCnt = ((dmaCurAddr-0x80000)>>1)-dmaBaseAddr; /* current address /2 since byte address */
        dmaCurCnt = dmaCurCnt>>1; /* count /2 since unpacked */
        if (dmaCurCnt >= dmaFrameLenSamps)
        {
            dmaCurCnt -= dmaFrameLenSamps;
        }

        /* Compute sample count since last SOF */
        sampCnt = numCDmaXfer*dmaFrameLenSamps - dmaPrevCnt + dmaCurCnt;
    }
    else
    {
        /* Compute current DMA transer count in frame */
        dmaCurCnt = ((dmaCurAddr-0x80000)>>1)-dmaBaseAddr; /* current address /2 since byte address */
        dmaCurCnt = dmaCurCnt>>1; /* count /2 since unpacked */
        if (dmaCurCnt >= dmaFrameLenSamps)
        {
            dmaCurCnt -= dmaFrameLenSamps;
        }

        sampCnt = dmaFrameLenSamps/16;

        firstSof = FALSE;
    }

    //STS_add(&mySts1, sampCnt); // debug

    return sampCnt;
}

