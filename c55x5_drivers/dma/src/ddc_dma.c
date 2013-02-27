/*
 * $$$MODULE_NAME ddc_dma.c
 *
 * $$$MODULE_DESC ddc_dma.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/**
 *  \file   ddc_dma.c
 *
 *  \brief  DMA DDC definitions.
 *
 *  This file contains the implementation of DDC of the DMA driver
 *  to utilize the services of the DMA device driver.
 *
 *   (C) Copyright 2004, Texas Instruments, Inc
 *
 *  \author     PR  Mistral
 *  \version    1.0 Initial implementation
 */

#include <std.h>
#include "ddc_dma.h"

DMA_ChannelObj    dmaChanObj[DMA_CHAN_OBJ_COUNT];
/**< DMA channel handle, one instance is created for each channel */

DMA_EngineObj     dmaEngObj[DMA_ENGINE_COUNT];
/**< DMA engine object. One instance for each engine */

DMA_ChanLookUp    dmaChanLookUp[DMA_REQDEV_COUNT]  =
                            {
                              PSP_DMA_I2S0    , 0               ,
                              PSP_DMA_MMC_SD0 , 0               ,
                              PSP_DMA_MMC_SD1 , 0               ,
                              PSP_DMA_UART    , DMA_CHAN_COUNT  ,
                              PSP_DMA_I2S2    , DMA_CHAN_COUNT  ,
                              PSP_DMA_I2C     , 2*DMA_CHAN_COUNT,
                              PSP_DMA_I2S3    , 2*DMA_CHAN_COUNT,
                              PSP_DMA_SAR     , 2*DMA_CHAN_COUNT,
                              PSP_DMA_I2S1    , 3*DMA_CHAN_COUNT,
                              PSP_DMA_NAND_CE0, 3*DMA_CHAN_COUNT,
                              PSP_DMA_NAND_CE1, 3*DMA_CHAN_COUNT,
                              PSP_DMA_EMIF    , 3*DMA_CHAN_COUNT,
                              PSP_DMA_MEMORY  , 0
                            };
/**< Lookup table which maps DMA channel No. with DMA requesting device Id's */

DMA_EvtLookUp   dmaEvtLookUp[DMA_EVT_COUNT]  =
                            {
                              PSP_DMA_EVT_I2S0_TX    , I2S0_TX_EVTVAL    ,
                              PSP_DMA_EVT_I2S0_RX    , I2S0_RX_EVTVAL    ,
                              PSP_DMA_EVT_I2S1_TX    , I2S1_TX_EVTVAL    ,
                              PSP_DMA_EVT_I2S1_RX    , I2S1_RX_EVTVAL    ,
                              PSP_DMA_EVT_I2S2_TX    , I2S2_TX_EVTVAL    ,
                              PSP_DMA_EVT_I2S2_RX    , I2S2_RX_EVTVAL    ,
                              PSP_DMA_EVT_I2S3_TX    , I2S3_TX_EVTVAL    ,
                              PSP_DMA_EVT_I2S3_RX    , I2S3_RX_EVTVAL    ,
                              PSP_DMA_EVT_MMC_SD0_TX , MMC_SD0_TX_EVTVAL ,
                              PSP_DMA_EVT_MMC_SD0_RX , MMC_SD0_RX_EVTVAL ,
                              PSP_DMA_EVT_MMC_SD1_TX , MMC_SD1_TX_EVTVAL ,
                              PSP_DMA_EVT_MMC_SD1_RX , MMC_SD1_RX_EVTVAL ,
                              PSP_DMA_EVT_TIMER0     , TIMER0_EVTVAL     ,
                              PSP_DMA_EVT_TIMER1     , TIMER1_EVTVAL     ,
                              PSP_DMA_EVT_TIMER2     , TIMER2_EVTVAL     ,
                              PSP_DMA_EVT_UART_TX    , UART_TX_EVTVAL    ,
                              PSP_DMA_EVT_UART_RX    , UART_RX_EVTVAL    ,
                              PSP_DMA_EVT_I2C_TX     , I2C_TX_EVTVAL     ,
                              PSP_DMA_EVT_I2C_RX     , I2C_RX_EVTVAL     ,
                              PSP_DMA_EVT_SAR_AD     , SAR_AD_EVTVAL     ,
                              PSP_DMA_EVT_NONE       , DMA_NO_EVTVAL
                            };
/**< Lookup table which maps DMA event values with DMA event Id's */

/**
 * \brief Opens the specified DMA instance
 *
 * This API is used to open a particular DMA channel.
 * This function creates handle to a particular DMA channel instance.
 *
 * \param    chanStartNum [IN]   Starting channel number from where requesting
 *                               device can access DMA the channels.
 *
 * \return   hDma     DMA channel handle.
 */
static DMA_ChanHandle DDC_DMAOpen(Uint16  chanStartNum);

/**
 * \brief Closes a particular DMA channel instance.
 *
 * This API is called to close a DMA channel instance.
 * This function resets all the hardware registers of a channel.
 *
 * \param    hDma [IN]    DMA channel handle.
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
static PSP_Result DDC_DMAClose(DMA_ChanHandle    hDma);

/**
 * \brief Sets the source and destination address
 *
 * This API is called to set the source and destination
 * address and transfer length of the given channel.
 *
 * \param   hDma [IN]    DMA channel handle.
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
static PSP_Result DDC_DMASetTransferParams(DMA_ChanHandle     hDma);

/**
 * \brief Sets the event source register
 *
 * This API is called to set the event source register when the
 * DMA is triggered by external hardware events.
 *
 * \param   hDma        [IN]    DMA channel handle.
 * \param   dmaEvtValue [IN]    DMA event value
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
static PSP_Result DDC_DMASetEvent(DMA_ChanHandle     hDma,
                                  Uint16             dmaEvtValue);

/**
 * \brief Initializes DMA driver.
 *
 * This API initializes all the DMA engine and channel objects.
 *
 * \return   None
 */
void DDC_DMADrvInit(void)
{
    Uint16 chanObjNum;

    /* Initialize DMA channel objects */
    for(chanObjNum=0; chanObjNum<DMA_CHAN_OBJ_COUNT; chanObjNum++)
    {
        dmaChanObj[chanObjNum].chanNum     =  chanObjNum;
        dmaChanObj[chanObjNum].isChanFree  =  TRUE;
    }

    /* Initialize the DMA engine objects */
    dmaEngObj[0].dmaRegs      =  CSL_DMA_REGS0;
    dmaEngObj[0].dmaEvtRegs   =  CSL_DMAEVTINT_REGS;

    dmaEngObj[1].dmaRegs      =  CSL_DMA_REGS1;
    dmaEngObj[1].dmaEvtRegs   =  CSL_DMAEVTINT_REGS;

    dmaEngObj[2].dmaRegs      =  CSL_DMA_REGS2;
    dmaEngObj[2].dmaEvtRegs   =  CSL_DMAEVTINT_REGS;

    dmaEngObj[3].dmaRegs      =  CSL_DMA_REGS3;
    dmaEngObj[3].dmaEvtRegs   =  CSL_DMAEVTINT_REGS;
}

/**
 * \brief Allocates a requested channel
 *
 * This API checks the availability of requested channel and allocates it
 * if available and returns the handle to that channel.
 *
 * \param    dmaReqDevId [IN]    Id of the device requesting DMA
 *
 * \return   hDma                DMA channel handle.
 */
DMA_ChanHandle DDC_DMARequestChan(PSP_DMAReqDevId   dmaReqDevId)
{
    DMA_ChanHandle    hDma;
    Uint16            looper;
    Uint16            chanNum;

    hDma = NULL;

    if(dmaReqDevId < PSP_DMA_REQDEVID_MAX)
    {
        /* Check for the given device Id in channel lookup table */
        for(looper=0; looper<PSP_DMA_REQDEVID_MAX; looper++)
        {
            if(dmaChanLookUp[looper].dmaReqDevId == dmaReqDevId)
            {
                break;
            }
        }

        if(looper != PSP_DMA_REQDEVID_MAX)
        {
            if(dmaChanLookUp[looper].dmaReqDevId == PSP_DMA_MEMORY)
            {
                for(chanNum=0; chanNum<DMA_CHAN_OBJ_COUNT; chanNum++)
                {
                    if(dmaChanObj[chanNum].isChanFree == TRUE)
                    {
                        dmaChanObj[chanNum].isChanFree  =  FALSE;
                        break;
                    }
                }

                if(chanNum < DMA_CHAN_OBJ_COUNT)
                {
                    hDma  =  &dmaChanObj[chanNum];
                }
            }
            else
            {
                hDma  =  DDC_DMAOpen(dmaChanLookUp[looper].chanStartNum);
            }
        }
    }
    return hDma;
}

/**
 * \brief Releases a channel
 *
 * This API releases a channel allocated to a device, so that
 * it can be used by other devices.
 *
 * \param    hDma [IN]    DMA instance handle.
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
PSP_Result DDC_DMAReleaseChan(DMA_ChanHandle    hDma)
{
    PSP_Result status;

    status  =  PSP_E_INVAL_PARAM;

    if((hDma != NULL) && (hDma->chanNum < DMA_CHAN_OBJ_COUNT))
    {
        status  =  DDC_DMAClose(hDma);

        hDma->isChanFree  =  TRUE;
        hDma              =  NULL;
    }
    return status;
}

/**
 * \brief Opens the specified DMA instance
 *
 * This API is used to open a particular DMA channel.
 * This function creates handle to a particular DMA channel instance.
 *
 * \param    chanStartNum [IN]   Starting channel number from where requesting
 *                               device can access DMA the channels.
 *
 * \return   hDma     DMA channel handle.
 */
static DMA_ChanHandle DDC_DMAOpen(Uint16  chanStartNum)
{
    DMA_ChanHandle    hDma;
    Uint16            chanNum;

    hDma    = NULL;
    chanNum = chanStartNum;

    if(chanStartNum < DMA_CHAN_OBJ_COUNT)
    {
        /* Check for the free channels among
         * four channels on a DMA engine
         */
        do
        {
            if(dmaChanObj[chanNum].isChanFree == TRUE)
            {
                dmaChanObj[chanNum].isChanFree  =  FALSE;
                break;
            }
            chanNum++;

        } while((chanNum - chanStartNum) != DMA_CHAN_COUNT);

        if((chanNum != chanStartNum)                    &&
          ((chanNum - chanStartNum) == DMA_CHAN_COUNT))
        {
            return NULL;
        }
    }

    hDma  =  &dmaChanObj[chanNum];
    return hDma;
}

/**
 * \brief Closes a particular DMA channel instance.
 *
 * This API is called to close a DMA channel instance.
 * This function resets all the hardware registers of a channel.
 *
 * \param    hDma [IN]    DMA channel handle.
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
static PSP_Result DDC_DMAClose(DMA_ChanHandle hDma)
{
    Uint16           chanNum;
    Uint16           dmaNum;
    DMA_EngineObj    *dmaObj;
    PSP_Result       status;

    status  =  PSP_E_INVAL_PARAM;

    if((hDma != NULL) && (hDma->chanNum < DMA_CHAN_OBJ_COUNT))
    {
        chanNum  =  hDma->chanNum;
        dmaNum   =  0;

        /* Based on the channel number provided, find out the
           DMA engine to be used
         */
        while(chanNum >= DMA_CHAN_COUNT)
        {
            chanNum  -=  DMA_CHAN_COUNT;
            dmaNum   +=  1;
        }

        dmaObj   =  &dmaEngObj[dmaNum];

        /* Reset transfer control, source and destination address
         * registers for the given channel
         */
        switch(chanNum)
        {
            case DMA_CHAN0:
                 CSL_FINS(dmaObj->dmaRegs->DMACH0SADR0,
                          DMA_DMACH0SADR0_LSWADDR,0x0);
                 CSL_FINS(dmaObj->dmaRegs->DMACH0SADR1,
                          DMA_DMACH0SADR1_MSWADDR,0x0);
                 CSL_FINS(dmaObj->dmaRegs->DMACH0DADR0,
                          DMA_DMACH0DADR0_LSWADDR,0x0);
                 CSL_FINS(dmaObj->dmaRegs->DMACH0DADR1,
                          DMA_DMACH0DADR1_MSWADDR,0x0);
                 CSL_FINS(dmaObj->dmaRegs->DMACH0TC0,
                          DMA_DMACH0TC0_TXLEN, 0x0);
                 dmaObj->dmaRegs->DMACH0TC1 &= (Uint16)0x0;
                 break;

            case DMA_CHAN1:
                 CSL_FINS(dmaObj->dmaRegs->DMACH1SADR0,
                          DMA_DMACH1SADR0_LSWADDR, 0x0);
                 CSL_FINS(dmaObj->dmaRegs->DMACH1SADR1,
                          DMA_DMACH1SADR1_MSWADDR, 0x0);
                 CSL_FINS(dmaObj->dmaRegs->DMACH1DADR0,
                          DMA_DMACH1DADR0_LSWADDR, 0x0);
                 CSL_FINS(dmaObj->dmaRegs->DMACH1DADR1,
                          DMA_DMACH1DADR1_MSWADDR, 0x0);
                 CSL_FINS(dmaObj->dmaRegs->DMACH1TC0,
                          DMA_DMACH1TC0_TXLEN, 0x0);
                 dmaObj->dmaRegs->DMACH1TC1 &= (Uint16)0x0;
                 break;

            case DMA_CHAN2:
                 CSL_FINS(dmaObj->dmaRegs->DMACH2SADR0,
                          DMA_DMACH2SADR0_LSWADDR, 0x0);
                 CSL_FINS(dmaObj->dmaRegs->DMACH2SADR1,
                          DMA_DMACH2SADR1_MSWADDR, 0x0);
                 CSL_FINS(dmaObj->dmaRegs->DMACH2DADR0,
                          DMA_DMACH2DADR0_LSWADDR, 0x0);
                 CSL_FINS(dmaObj->dmaRegs->DMACH2DADR1,
                          DMA_DMACH2DADR1_MSWADDR, 0x0);
                 CSL_FINS(dmaObj->dmaRegs->DMACH2TC0,
                          DMA_DMACH2TC0_TXLEN, 0x0);
                 dmaObj->dmaRegs->DMACH2TC1 &= (Uint16)0x0;
                 break;

            case DMA_CHAN3:
                 CSL_FINS(dmaObj->dmaRegs->DMACH3SADR0,
                          DMA_DMACH3SADR0_LSWADDR, 0x0);
                 CSL_FINS(dmaObj->dmaRegs->DMACH3SADR1,
                          DMA_DMACH3SADR1_MSWADDR, 0x0);
                 CSL_FINS(dmaObj->dmaRegs->DMACH3DADR0,
                          DMA_DMACH3DADR0_LSWADDR, 0x0);
                 CSL_FINS(dmaObj->dmaRegs->DMACH3DADR1,
                          DMA_DMACH3DADR1_MSWADDR, 0x0);
                 CSL_FINS(dmaObj->dmaRegs->DMACH3TC0,
                          DMA_DMACH3TC0_TXLEN, 0x0);
                 dmaObj->dmaRegs->DMACH3TC1 &= (Uint16)0x0;
                 break;

            default:
                 break;
        }
        /* Clear event source registers */
        status  =  DDC_DMASetEvent(hDma, 0x0);
    }

    return status;
}

/**
 * \brief Configures DMA hardware registers
 *
 * This API is called to configure the hardware registers of DMA.
 *
 * \param   hDma      [IN]    DMA channel handle.
 * \param   dmaConfig [IN]    DMA configuration structure
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
PSP_Result DDC_DMAConfigChan(DMA_ChanHandle     hDma,
                             PSP_DMAConfig      *dmaConfig)
{
    Uint16           chanNum;
    Uint16           dmaNum;
    Uint16           dmaTcRegValue;
    Uint16           dmaEvtValue;
    Uint16           looper;
    DMA_EngineObj    *dmaObj;
    PSP_Result       status;

    dmaTcRegValue  =  0x0;
    dmaEvtValue    =  0x0;
    status         =  PSP_SOK;

    if((hDma != NULL)                        &&
       (dmaConfig != NULL)                   &&
       (hDma->chanNum < DMA_CHAN_OBJ_COUNT))
    {
        chanNum  =  hDma->chanNum;
        dmaNum   =  0;

        /* Based on the channel number provided, find out the
           DMA engine to be used
         */
        while(chanNum >= DMA_CHAN_COUNT)
        {
            chanNum  -=  DMA_CHAN_COUNT;
            dmaNum   +=  1;
        }

        dmaObj  =  &dmaEngObj[dmaNum];

        /* Check the configuration parameters of DMA */
        /* Check auto reload mode parameter */
        if((dmaConfig->autoMode != PSP_DMA_AUTORELOAD_DISABLE) &&
           (dmaConfig->autoMode != PSP_DMA_AUTORELOAD_ENABLE))
        {
            status  =  PSP_E_INVAL_PARAM;
        }
        /* Check burst length parameter */
        if((dmaConfig->burstLen < PSP_DMA_TXBURST_1WORD) ||
           (dmaConfig->burstLen > PSP_DMA_TXBURST_16WORD))
        {
            status  =  PSP_E_INVAL_PARAM;
        }
        /* Check channel direction parameter */
        if((dmaConfig->chanDir != PSP_DMA_READ) &&
           (dmaConfig->chanDir != PSP_DMA_WRITE))
        {
            status  =  PSP_E_INVAL_PARAM;
        }
        /* Check DMA trigger parameter */
        if((dmaConfig->trigger != PSP_DMA_SOFTWARE_TRIGGER) &&
           (dmaConfig->trigger != PSP_DMA_EVENT_TRIGGER))
        {
            status  =  PSP_E_INVAL_PARAM;
        }
        /* Check transfer type parameter */
        if((dmaConfig->trfType != PSP_DMA_TRANSFER_IO_MEMORY) &&
           (dmaConfig->trfType != PSP_DMA_TRANSFER_MEMORY))
        {
            status  =  PSP_E_INVAL_PARAM;
        }
        /* Check data length parameter */
        if(dmaConfig->dataLen < DMA_MINTRFLEN)
        {
            status  =  PSP_E_INVAL_PARAM;
        }

        if(status == PSP_SOK)
        {
            hDma->chanDir       =  dmaConfig->chanDir;
            hDma->trigger       =  dmaConfig->trigger;
            hDma->trfType       =  dmaConfig->trfType;
            hDma->srcAddr       =  dmaConfig->srcAddr;
            hDma->destAddr      =  dmaConfig->destAddr;
            hDma->dataLen       =  dmaConfig->dataLen;
            hDma->callback      =  dmaConfig->callback;
            hDma->dataCallback  =  dmaConfig->dataCallback;

            /* Set the interrupt enable bit */
            CSL_FINS(dmaTcRegValue, DMA_DMACH0TC1_INTEN, 0x01);

            /* Set the auto reload mode bit */
            CSL_FINS(dmaTcRegValue, DMA_DMACH0TC1_AUTORELOAD,
                                    dmaConfig->autoMode);

                     /* Set reserved field */
            CSL_FINS(dmaTcRegValue, DMA_DMACH0TC1_RSV, 0);

            /* Set burst length of data to be transferred in each transfer */
            CSL_FINS(dmaTcRegValue, DMA_DMACH0TC1_BURSTMODE,
                     dmaConfig->burstLen);

            /* Set the ping-pong mode */
            CSL_FINS(dmaTcRegValue, DMA_DMACH0TC1_PINGPONGEN, 
                dmaConfig->pingPongMode);

            /* Set DMA event value */
            if(dmaConfig->trigger == PSP_DMA_EVENT_TRIGGER)
            {
                for(looper=0; looper<PSP_DMA_EVT_INVALID; looper++)
                {
                    /* Check for the given event Id in event lookup table */
                    if(dmaEvtLookUp[looper].dmaEvtId == dmaConfig->dmaEvt)
                    {
                        break;
                    }
                }

                /* Extract the event value if given event Id is valid */
                if(looper != PSP_DMA_EVT_INVALID)
                {
                    dmaEvtValue  =  dmaEvtLookUp[looper].dmaEvtValue;
                    status = DDC_DMASetEvent(hDma, dmaEvtValue);
                    /* Set sync mode bit */
                    CSL_FINS(dmaTcRegValue, DMA_DMACH0TC1_SYNC, 0x01);
                }
                else
                {
                    status  =  PSP_E_INVAL_PARAM;
                }
            }

            if(status == PSP_SOK)
            {
                switch(chanNum)
                {
                    case DMA_CHAN0:
                         /* Set the transfer control register value */
                         dmaObj->dmaRegs->DMACH0TC1 |= (Uint16)dmaTcRegValue;
                         break;

                    case DMA_CHAN1:
                         /* Set the transfer control register value */
                         dmaObj->dmaRegs->DMACH1TC1 |= (Uint16)dmaTcRegValue;
                         break;

                    case DMA_CHAN2:
                         /* Set the transfer control register value */
                         dmaObj->dmaRegs->DMACH2TC1 |= (Uint16)dmaTcRegValue;
                         break;

                    case DMA_CHAN3:
                         /* Set the transfer control register value */
                          dmaObj->dmaRegs->DMACH3TC1 |= (Uint16)dmaTcRegValue;
                          break;

                    default:
                         break;
                }
                /* set the src/dest address and transfer length */
                status  =  DDC_DMASetTransferParams(hDma);
            }
        }
    }
    else
    {
        status  =  PSP_E_INVAL_PARAM;
    }

    return status;
}

/**
 * \brief Sets the source and destination address
 *
 * This API is called to set the source and destination
 * address and transfer length of the given channel.
 *
 * \param   hDma [IN]    DMA channel handle.
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
static PSP_Result DDC_DMASetTransferParams(DMA_ChanHandle     hDma)
{
    Uint16           chanNum;
    Uint16           dmaNum;
    DMA_EngineObj    *dmaObj;
    Uint32           src_address;
    Uint32           dest_address;
    Uint16           src_addrMode;
    Uint16           dest_addrMode;
    PSP_Result       status;

    status  =  PSP_SOK;

    if((hDma != NULL) && (hDma->chanNum<DMA_CHAN_OBJ_COUNT))
    {
        chanNum  =  hDma->chanNum;
        dmaNum   =  0;

        /* Based on the channel number provided, find out the
           DMA engine to be used
         */
        while(chanNum >= DMA_CHAN_COUNT)
        {
            chanNum  -=  DMA_CHAN_COUNT;
            dmaNum   +=  1;
        }

        dmaObj   =  &dmaEngObj[dmaNum];

        if(status == PSP_SOK)
        {
            switch(hDma->trfType)
            {
                case PSP_DMA_TRANSFER_MEMORY:
                     dest_address = DDC_DMAAdjustAddrOffset(hDma->destAddr);
                     src_address  = DDC_DMAAdjustAddrOffset(hDma->srcAddr);
                     if((dest_address == hDma->destAddr)||
                        (src_address  == hDma->srcAddr))
                     {
                         status  =  PSP_E_INVAL_PARAM;
                     }
                     src_addrMode   =  (Uint16)(DMA_ADDR_MODE_INCREMENT);
                     dest_addrMode  =  (Uint16)(DMA_ADDR_MODE_INCREMENT);
                     break;

                case PSP_DMA_TRANSFER_IO_MEMORY:
                     /* Reading data from a peripheral */
                     if((hDma->chanDir == PSP_DMA_READ) )
                     {
                         dest_address = DDC_DMAAdjustAddrOffset(hDma->destAddr);
                         if(dest_address == hDma->destAddr)
                         {
                             status  =  PSP_E_INVAL_PARAM;
                         }
                         src_address  =  hDma->srcAddr;
                         /* Set the destination and source address modes.
                          * For read, src. is fixed and dest. is incremented.
                          */
                         src_addrMode   =  (Uint16)(DMA_ADDR_MODE_FIXED);
                         dest_addrMode  =  (Uint16)(DMA_ADDR_MODE_INCREMENT);
                     }
                     /* Writing data to a peripheral */
                     else if(hDma->chanDir == PSP_DMA_WRITE)
                     {
                         src_address = DDC_DMAAdjustAddrOffset(hDma->srcAddr);
                         if(src_address == hDma->srcAddr)
                         {
                             status  =  PSP_E_INVAL_PARAM;
                         }
                         dest_address  =  hDma->destAddr;
                         /* Set the destination and source address modes.
                          * For write, src is incremented and dest. is fixed.
                          */
                         src_addrMode   =  (Uint16)(DMA_ADDR_MODE_INCREMENT);
                         dest_addrMode  =  (Uint16)(DMA_ADDR_MODE_FIXED);
                     }
                     else
                     {
                         status  =  PSP_E_INVAL_PARAM;
                     }
                     break;

                default:
                     status  =  PSP_E_INVAL_PARAM;
                     break;
            }
        }

        if(status == PSP_SOK)
        {
            switch(chanNum)
            {
                case DMA_CHAN0:
                     /* Set the DMA source and destination addresses for CH0 */
                     CSL_FINS(dmaObj->dmaRegs->DMACH0SADR0,
                              DMA_DMACH0SADR0_LSWADDR, (src_address & 0xFFFF));
                     CSL_FINS(dmaObj->dmaRegs->DMACH0SADR1,
                              DMA_DMACH0SADR1_MSWADDR, (src_address >> 16));
                     CSL_FINS(dmaObj->dmaRegs->DMACH0DADR0,
                             DMA_DMACH0DADR0_LSWADDR, (dest_address & 0xFFFF));
                     CSL_FINS(dmaObj->dmaRegs->DMACH0DADR1,
                              DMA_DMACH0DADR1_MSWADDR, (dest_address >> 16));

                     /* Set the transfer length */
                     CSL_FINS(dmaObj->dmaRegs->DMACH0TC0,
                            DMA_DMACH0TC0_TXLEN, hDma->dataLen);

                     /* Set source and destination modes */
                     CSL_FINS(dmaObj->dmaRegs->DMACH0TC1,
                              DMA_DMACH0TC1_SRCADRMODE, src_addrMode);
                     CSL_FINS(dmaObj->dmaRegs->DMACH0TC1,
                              DMA_DMACH0TC1_DESTADRMODE, dest_addrMode);
                     break;

                case DMA_CHAN1:
                     /* Set the DMA source and destination addresses for CH1 */
                     CSL_FINS(dmaObj->dmaRegs->DMACH1SADR0,
                              DMA_DMACH1SADR0_LSWADDR, (src_address & 0xFFFF));
                     CSL_FINS(dmaObj->dmaRegs->DMACH1SADR1,
                              DMA_DMACH1SADR1_MSWADDR, (src_address >> 16));
                     CSL_FINS(dmaObj->dmaRegs->DMACH1DADR0,
                             DMA_DMACH1DADR0_LSWADDR, (dest_address & 0xFFFF));
                     CSL_FINS(dmaObj->dmaRegs->DMACH1DADR1,
                              DMA_DMACH1DADR1_MSWADDR, (dest_address >> 16));

                     /* Set the transfer length */
                     CSL_FINS(dmaObj->dmaRegs->DMACH1TC0,
                            DMA_DMACH1TC0_TXLEN, hDma->dataLen);

                     /* Set source and destination modes */
                     CSL_FINS(dmaObj->dmaRegs->DMACH1TC1,
                              DMA_DMACH1TC1_SRCADRMODE, src_addrMode);
                     CSL_FINS(dmaObj->dmaRegs->DMACH1TC1,
                             DMA_DMACH1TC1_DESTADRMODE, dest_addrMode);
                     break;

                case DMA_CHAN2:
                     /* Set the DMA source and destination addresses for CH2 */
                     CSL_FINS(dmaObj->dmaRegs->DMACH2SADR0,
                              DMA_DMACH2SADR0_LSWADDR, (src_address & 0xFFFF));
                     CSL_FINS(dmaObj->dmaRegs->DMACH2SADR1,
                              DMA_DMACH2SADR1_MSWADDR, (src_address >> 16));
                     CSL_FINS(dmaObj->dmaRegs->DMACH2DADR0,
                             DMA_DMACH2DADR0_LSWADDR, (dest_address & 0xFFFF));
                     CSL_FINS(dmaObj->dmaRegs->DMACH2DADR1,
                              DMA_DMACH2DADR1_MSWADDR, (dest_address >> 16));

                     /* Set the transfer length */
                     CSL_FINS(dmaObj->dmaRegs->DMACH2TC0,
                            DMA_DMACH2TC0_TXLEN, hDma->dataLen);

                     /* Set source and destination modes */
                     CSL_FINS(dmaObj->dmaRegs->DMACH2TC1,
                              DMA_DMACH2TC1_SRCADRMODE, src_addrMode);
                     CSL_FINS(dmaObj->dmaRegs->DMACH2TC1,
                              DMA_DMACH2TC1_DESTADRMODE, dest_addrMode);
                     break;

                case DMA_CHAN3:
                     /* Set the DMA source and destination addresses for CH3 */
                     CSL_FINS(dmaObj->dmaRegs->DMACH3SADR0,
                              DMA_DMACH3SADR0_LSWADDR, (src_address & 0xFFFF));
                     CSL_FINS(dmaObj->dmaRegs->DMACH3SADR1,
                              DMA_DMACH3SADR1_MSWADDR, (src_address >> 16));
                     CSL_FINS(dmaObj->dmaRegs->DMACH3DADR0,
                             DMA_DMACH3DADR0_LSWADDR, (dest_address & 0xFFFF));
                     CSL_FINS(dmaObj->dmaRegs->DMACH3DADR1,
                              DMA_DMACH3DADR1_MSWADDR, (dest_address >> 16));

                     /* Set the transfer length */
                     CSL_FINS(dmaObj->dmaRegs->DMACH3TC0,
                            DMA_DMACH3TC0_TXLEN, hDma->dataLen);

                     /* Set source and destination modes */
                     CSL_FINS(dmaObj->dmaRegs->DMACH3TC1,
                              DMA_DMACH3TC1_SRCADRMODE, src_addrMode);
                     CSL_FINS(dmaObj->dmaRegs->DMACH3TC1,
                              DMA_DMACH3TC1_DESTADRMODE, dest_addrMode);
                     break;

                default:
                     break;
            }
        }
    }
    else
    {
        status  =  PSP_E_INVAL_PARAM;
    }

    return status;
}

/**
 * \brief Sets the event source register
 *
 * This API is called to set the event source register when the
 * DMA is triggered by external hardware events.
 *
 * \param   hDma        [IN]    DMA channel handle.
 * \param   dmaEvtValue [IN]    DMA event value
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
static PSP_Result DDC_DMASetEvent(DMA_ChanHandle     hDma,
                                  Uint16             dmaEvtValue)
{
    DMA_EngineObj    *dmaObj;
    Uint16           chanNum;
    Uint16           dmaNum;
    PSP_Result       status;

    status  =  PSP_E_INVAL_PARAM;

    if((hDma != NULL) && (hDma->chanNum < DMA_CHAN_OBJ_COUNT))
    {
        chanNum  =  hDma->chanNum;
        dmaNum   =  0;

        /* Based on the channel number provided, find out the
           DMA engine to be used
         */
        while(chanNum >= DMA_CHAN_COUNT)
        {
            chanNum  -=  DMA_CHAN_COUNT;
            dmaNum   +=  1;
        }

        dmaObj   =  &dmaEngObj[dmaNum];

        /* Set the event source register */
        switch(chanNum)
        {
            case DMA_CHAN0:

                 switch(dmaNum)
                 {
                     case DMA_ENGINE0:
                          CSL_FINS(dmaObj->dmaEvtRegs->DMA0EVT0,
                                       DMA_DMA0EVT0_CH0EVENT, dmaEvtValue);
                          break;
                     case DMA_ENGINE1:
                          CSL_FINS(dmaObj->dmaEvtRegs->DMA1EVT0,
                                   DMA_DMA1EVT0_CH0EVENT, dmaEvtValue);
                          break;
                     case DMA_ENGINE2:
                          CSL_FINS(dmaObj->dmaEvtRegs->DMA2EVT0,
                                   DMA_DMA2EVT0_CH0EVENT, dmaEvtValue);
                          break;
                     case DMA_ENGINE3:
                          CSL_FINS(dmaObj->dmaEvtRegs->DMA3EVT0,
                                   DMA_DMA3EVT0_CH0EVENT, dmaEvtValue);
                          break;
                     default:
                          break;
                 }

                 break;

            case DMA_CHAN1:

                 switch(dmaNum)
                 {
                     case DMA_ENGINE0:
                          CSL_FINS(dmaObj->dmaEvtRegs->DMA0EVT0,
                                   DMA_DMA0EVT0_CH1EVENT, dmaEvtValue);
                          break;
                     case DMA_ENGINE1:
                          CSL_FINS(dmaObj->dmaEvtRegs->DMA1EVT0,
                                   DMA_DMA1EVT0_CH1EVENT, dmaEvtValue);
                          break;
                     case DMA_ENGINE2:
                          CSL_FINS(dmaObj->dmaEvtRegs->DMA2EVT0,
                                   DMA_DMA2EVT0_CH1EVENT, dmaEvtValue);
                          break;
                     case DMA_ENGINE3:
                          CSL_FINS(dmaObj->dmaEvtRegs->DMA3EVT0,
                                   DMA_DMA3EVT0_CH1EVENT, dmaEvtValue);
                          break;
                     default:
                          break;
                 }

                 break;

            case DMA_CHAN2:

                 switch(dmaNum)
                 {
                     case DMA_ENGINE0:
                          CSL_FINS(dmaObj->dmaEvtRegs->DMA0EVT1,
                                   DMA_DMA0EVT1_CH2EVENT, dmaEvtValue);
                          break;
                     case DMA_ENGINE1:
                          CSL_FINS(dmaObj->dmaEvtRegs->DMA1EVT1,
                                   DMA_DMA1EVT1_CH2EVENT, dmaEvtValue);
                          break;
                     case DMA_ENGINE2:
                          CSL_FINS(dmaObj->dmaEvtRegs->DMA2EVT1,
                                   DMA_DMA2EVT1_CH2EVENT, dmaEvtValue);
                          break;
                     case DMA_ENGINE3:
                          CSL_FINS(dmaObj->dmaEvtRegs->DMA3EVT1,
                                   DMA_DMA3EVT1_CH2EVENT, dmaEvtValue);
                          break;
                     default:
                          break;
                     }

                 break;

            case DMA_CHAN3:

                 switch(dmaNum)
                 {
                     case DMA_ENGINE0:
                          CSL_FINS(dmaObj->dmaEvtRegs->DMA0EVT1,
                                   DMA_DMA0EVT1_CH3EVENT, dmaEvtValue);
                          break;
                     case DMA_ENGINE1:
                          CSL_FINS(dmaObj->dmaEvtRegs->DMA1EVT1,
                                   DMA_DMA1EVT1_CH3EVENT, dmaEvtValue);
                          break;
                     case DMA_ENGINE2:
                          CSL_FINS(dmaObj->dmaEvtRegs->DMA2EVT1,
                                   DMA_DMA2EVT1_CH3EVENT, dmaEvtValue);
                          break;
                     case DMA_ENGINE3:
                          CSL_FINS(dmaObj->dmaEvtRegs->DMA3EVT1,
                                   DMA_DMA3EVT1_CH3EVENT, dmaEvtValue);
                          break;
                     default:
                          break;
                 }

                 break;

            default:
                 break;
        }
        status  =  PSP_SOK;
    }

    return status;
}

/**
 * \brief Routine to modify address according to DMA requirement
 *
 * This function modifies the source/destination address specified by the
 * application according to the requirement of DMA
 *
 *  \param      address     [IN]    address given by application
 *
 *  \return     modAddr - modified address
 */
Uint32 DDC_DMAAdjustAddrOffset(Uint32    address)
{
    Uint32 modAddr;

    /* DMA can access the DARAM/SARAM from address 0x10000/0x80000.
     * Add this base address to the given address
     */
    if((address >= DARAM_START_ADDR) && (address <= DARAM_END_ADDR))
    {
        modAddr  = (address << 0x01) + 0x10000;
    }
    else if((address >= SARAM_START_ADDR) && (address <= SARAM_END_ADDR))
    {
        modAddr  = (address << 0x01) + 0x80000;
    }
    else
    {
        modAddr  =  address;
    }

    return(modAddr);
}

/**
 * \brief Changes the source address
 *
 * This API is called to change the source address of any channel.
 * This API is used for supporting PING - PONG buffer configuration
 *
 * \param    hDma       [IN]    DMA channel handle.
 * \param    newSrcAddr [IN]    address of DMA new source data buffer
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
PSP_Result DDC_DMAChangeSrcAddr(DMA_ChanHandle    hDma,
                                Uint32            newSrcAddr)
{
    PSP_Result status;

    status = PSP_E_INVAL_PARAM;

    if(hDma != NULL)
    {
        hDma->srcAddr  =  newSrcAddr;
        status  =  DDC_DMASetTransferParams(hDma);
    }

    return status;
}

/**
 * \brief Changes the destination address
 *
 * This API is called to change the destination address of any channel.
 * This API is used for supporting PING - PONG buffer configuration
 *
 * \param    hDma        [IN]    DMA instance handle.
 * \param    newDestAddr [IN]    address of DMA new destination data buffer
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
PSP_Result DDC_DMAChangeDestAddr(DMA_ChanHandle    hDma,
                                 Uint32            newDestAddr)
{
    PSP_Result status;

    status = PSP_E_INVAL_PARAM;

    if(hDma != NULL)
    {
        hDma->destAddr  =  newDestAddr;
        status  =  DDC_DMASetTransferParams(hDma);
    }
    return status;
}

/**
 * \brief Starts DMA data transfer
 *
 * This function transfers data on given channel from
 * source data buffer to destination data buffer
 *
 * \param    hDma [IN]    DMA channel handle
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
PSP_Result DDC_DMAStartTransfer(DMA_ChanHandle    hDma)
{
    DMA_EngineObj     *dmaObj;
    Uint16            chanNum;
    Uint16            dmaNum;
    PSP_Result        status;

    status     =  PSP_E_INVAL_PARAM;

    if((hDma != NULL) && (hDma->chanNum < DMA_CHAN_OBJ_COUNT))
    {
        chanNum  =  hDma->chanNum;
        dmaNum   =  0;

        while(chanNum >= DMA_CHAN_COUNT)
        {
            chanNum  -=  DMA_CHAN_COUNT;
            dmaNum   +=  1;
        }

        dmaObj   =  &dmaEngObj[dmaNum];

        switch(chanNum)
        {
            case DMA_CHAN0:    /* Starts data transfer on channel 0 */
                    CSL_FINS(dmaObj->dmaRegs->DMACH0TC1,
                             DMA_DMACH0TC1_DMASTART, (Uint16)0x01);
                break;

            case DMA_CHAN1:    /* Starts data transfer on channel 1 */
                    CSL_FINS(dmaObj->dmaRegs->DMACH1TC1,
                             DMA_DMACH1TC1_DMASTART, (Uint16)0x01);
                break;

            case DMA_CHAN2:    /* Starts data transfer on channel 2 */
                    CSL_FINS(dmaObj->dmaRegs->DMACH2TC1,
                             DMA_DMACH2TC1_DMASTART, (Uint16)0x01);
                break;

            case DMA_CHAN3:    /* Starts data transfer on channel 3 */
                    CSL_FINS(dmaObj->dmaRegs->DMACH3TC1,
                             DMA_DMACH3TC1_DMASTART, (Uint16)0x01);
                break;

            default:
                break;
        }
        status  =  PSP_SOK;
    }

    return status;
}

/**
 * \brief Stops DMA data transfer
 *
 * This function stops data transfer on given channel.
 * This can be used to abort the data transfer in normal mode or to stop
 * the data transfer in Auto-reload mode.
 *
 * \param    hDma [IN]    DMA instance handle.
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
PSP_Result DDC_DMAStopTransfer(DMA_ChanHandle    hDma)
{
    DMA_EngineObj    *dmaObj;
    Uint16           chanNum;
    Uint16           dmaNum;
    PSP_Result       status;

    status  =  PSP_E_INVAL_PARAM;

    if((hDma != NULL) && (hDma->chanNum < DMA_CHAN_OBJ_COUNT))
    {
        chanNum  =  hDma->chanNum;
        dmaNum   =  0;

        /* Based on the channel number provided, find out the
           DMA engine to be used
         */
        while(chanNum >= DMA_CHAN_COUNT)
        {
            chanNum  -=  DMA_CHAN_COUNT;
            dmaNum   +=  1;
        }

        dmaObj   =  &dmaEngObj[dmaNum];

        switch(chanNum)
        {
            case DMA_CHAN0:    /* Stops data transfer on channel 0 */
                CSL_FINS(dmaObj->dmaRegs->DMACH0TC1,DMA_DMACH0TC1_DMASTART,0);
                break;
            case DMA_CHAN1:    /* Stops data transfer on channel 1 */
                CSL_FINS(dmaObj->dmaRegs->DMACH1TC1,DMA_DMACH1TC1_DMASTART,0);
                break;
            case DMA_CHAN2:    /* Stops data transfer on channel 2 */
                CSL_FINS(dmaObj->dmaRegs->DMACH2TC1,DMA_DMACH2TC1_DMASTART,0);
                break;
            case DMA_CHAN3:    /* Stops data transfer on channel 3 */
                CSL_FINS(dmaObj->dmaRegs->DMACH3TC1,DMA_DMACH3TC1_DMASTART,0);
                break;
            default:
                break;
        }
        status  =  PSP_SOK;
    }

    return status;
}

/**
 * \brief Interrupt service routine for dma
 *
 * DMA interrupts the CPU after transferring given length of data.
 * CPU dispatches the isr in which registered callback function is invoked
 *
 *  \return     void
 */
Uint32 dmaIsrCount = 0;
Uint32 dmaCallbackReqCount = 0;
Uint32 dmaCallbackCount = 0;
Uint32 dmaIsrCountArray[DMA_CHAN_OBJ_COUNT] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
void DMA_Isr(void)
{
    Uint16 ifrValue;
    Uint16 looper;

    /* Read and clear the IFR register of DMA */
    ifrValue = CSL_DMAEVTINT_REGS->DMAIFR;
    CSL_DMAEVTINT_REGS->DMAIFR = ifrValue;

    dmaIsrCount++;
    for(looper=0; looper<DMA_CHAN_OBJ_COUNT; looper++)
    {
        if((ifrValue>>looper)&0x01 == 0x01)
        {
            /* Clear the corresponding DMA IFR Register bit value */
            //CSL_DMAEVTINT_REGS->DMAIFR &= (0x01 << looper);
			
			dmaCallbackReqCount++;
			dmaIsrCountArray[looper]++;
            if(dmaChanObj[looper].callback != NULL)
            {
				dmaCallbackCount++;
                /* Invoke the callback function */
                dmaChanObj[looper].callback(PSP_DMA_TRANSFER_COMPLETE,
                                        dmaChanObj[looper].dataCallback);
            }
        }
    }
}

/**
 * \brief Change I2S source buffer address
 *
 * This function changes the DMA source address for I2S
 *
 * \param    hDma      [IN]    DMA instance handle.
 * \param    NextTxBuf [IN]    I2S next transmit buffer address
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
PSP_Result DMA_ChangeI2SSrcAddr(DMA_ChanHandle    hDma,
                                Uint32            NextTxBuf)
{
    DMA_EngineObj    *dmaObj;
    Uint16           chanNum;
    Uint16           dmaNum;
    PSP_Result       status;

    status  =  PSP_E_INVAL_PARAM;

    if((hDma != NULL) && (hDma->chanNum < DMA_CHAN_OBJ_COUNT))
    {
        chanNum  =  hDma->chanNum;
        dmaNum   =  0;

        while(chanNum >= DMA_CHAN_COUNT)
        {
            chanNum  -=  DMA_CHAN_COUNT;
            dmaNum   +=  1;
        }

        dmaObj   =  &dmaEngObj[dmaNum];

        switch(chanNum)
        {
            case DMA_CHAN0:
                    /* Set the DMA source address for CH0 */
                    CSL_FINS(dmaObj->dmaRegs->DMACH0SADR0,
                             DMA_DMACH0SADR0_LSWADDR, (NextTxBuf & 0xFFFF));
                    CSL_FINS(dmaObj->dmaRegs->DMACH0SADR1,
                             DMA_DMACH0SADR1_MSWADDR, (NextTxBuf >> 16));
                break;

            case DMA_CHAN1:
                    /* Set the DMA source address for CH1 */
                    CSL_FINS(dmaObj->dmaRegs->DMACH1SADR0,
                             DMA_DMACH1SADR0_LSWADDR, (NextTxBuf & 0xFFFF));
                    CSL_FINS(dmaObj->dmaRegs->DMACH1SADR1,
                             DMA_DMACH1SADR1_MSWADDR, (NextTxBuf >> 16));
                break;

            case DMA_CHAN2:
                    /* Set the DMA source address for CH2 */
                    CSL_FINS(dmaObj->dmaRegs->DMACH2SADR0,
                             DMA_DMACH2SADR0_LSWADDR, (NextTxBuf & 0xFFFF));
                    CSL_FINS(dmaObj->dmaRegs->DMACH2SADR1,
                             DMA_DMACH2SADR1_MSWADDR, (NextTxBuf >> 16));
                break;

            case DMA_CHAN3:
                    /* Set the DMA source address for CH3 */
                     CSL_FINS(dmaObj->dmaRegs->DMACH3SADR0,
                              DMA_DMACH3SADR0_LSWADDR, (NextTxBuf & 0xFFFF));
                     CSL_FINS(dmaObj->dmaRegs->DMACH3SADR1,
                              DMA_DMACH3SADR1_MSWADDR, (NextTxBuf >> 16));
                break;

            default:
                break;
        }
        status  =  PSP_SOK;
    }

    return status;
}

/**
 * \brief Change I2S destination buffer address
 *
 * This function changes the DMA destination address for I2S
 *
 * \param    hDma      [IN]    DMA instance handle.
 * \param    NextRxBuf [IN]    I2S next receive buffer address
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
PSP_Result DMA_ChangeI2SDestAddr(DMA_ChanHandle    hDma,
                                 Uint32            NextRxBuf)
{
    DMA_EngineObj    *dmaObj;
    Uint16           chanNum;
    Uint16           dmaNum;
    PSP_Result       status;

    status  =  PSP_E_INVAL_PARAM;

    if((hDma != NULL) && (hDma->chanNum < DMA_CHAN_OBJ_COUNT))
    {
        chanNum  =  hDma->chanNum;
        dmaNum   =  0;

        while(chanNum >= DMA_CHAN_COUNT)
        {
            chanNum  -=  DMA_CHAN_COUNT;
            dmaNum   +=  1;
        }

        dmaObj   =  &dmaEngObj[dmaNum];

        switch(chanNum)
        {
            case DMA_CHAN0:
                 /* Set the DMA destination address for CH0 */
                 CSL_FINS(dmaObj->dmaRegs->DMACH0DADR0,
                          DMA_DMACH0DADR0_LSWADDR, (NextRxBuf & 0xFFFF));
                 CSL_FINS(dmaObj->dmaRegs->DMACH0DADR1,
                          DMA_DMACH0DADR1_MSWADDR, (NextRxBuf >> 16));
                 break;

            case DMA_CHAN1:
                 /* Set the DMA destination address for CH1 */
                 CSL_FINS(dmaObj->dmaRegs->DMACH1DADR0,
                          DMA_DMACH1DADR0_LSWADDR, (NextRxBuf & 0xFFFF));
                 CSL_FINS(dmaObj->dmaRegs->DMACH1DADR1,
                          DMA_DMACH1DADR1_MSWADDR, (NextRxBuf >> 16));
                 break;

            case DMA_CHAN2:
                 /* Set the DMA destination address for CH2 */
                 CSL_FINS(dmaObj->dmaRegs->DMACH2DADR0,
                          DMA_DMACH2DADR0_LSWADDR, (NextRxBuf & 0xFFFF));
                 CSL_FINS(dmaObj->dmaRegs->DMACH2DADR1,
                          DMA_DMACH2DADR1_MSWADDR, (NextRxBuf >> 16));
                 break;

            case DMA_CHAN3:
                 /* Set the DMA destination address for CH3 */
                 CSL_FINS(dmaObj->dmaRegs->DMACH3DADR0,
                          DMA_DMACH3DADR0_LSWADDR, (NextRxBuf & 0xFFFF));
                 CSL_FINS(dmaObj->dmaRegs->DMACH3DADR1,
                          DMA_DMACH3DADR1_MSWADDR, (NextRxBuf >> 16));

                 break;

            default:
                break;
        }
        status  =  PSP_SOK;
    }

    return status;
}


/**
 * \brief Checks the DMA transfer completion
 *
 * This function checks whether the DMA data transfer on a particular
 * channel is complete or not
 *
 * \param    hDma      [IN]    DMA instance handle.
 *
 * \return   none
 */
void DDC_DMACheckTxComplete(DMA_ChanHandle    hDma)
{
    DMA_EngineObj    *dmaObj;
    Uint16           chanNum;
    Uint16           dmaNum;

    if((hDma != NULL) && (hDma->chanNum < DMA_CHAN_OBJ_COUNT))
    {
        chanNum  =  hDma->chanNum;
        dmaNum   =  0;

        while(chanNum >= DMA_CHAN_COUNT)
        {
            chanNum  -=  DMA_CHAN_COUNT;
            dmaNum   +=  1;
        }

        dmaObj   =  &dmaEngObj[dmaNum];

        switch(chanNum)
        {
            case DMA_CHAN0:
                 while(CSL_FEXT(dmaObj->dmaRegs->DMACH0TC1, DMA_DMACH0TC1_DMASTART) == TRUE);
                break;

            case DMA_CHAN1:
                 while(CSL_FEXT(dmaObj->dmaRegs->DMACH1TC1, DMA_DMACH1TC1_DMASTART) == TRUE);
                break;

            case DMA_CHAN2:
                 while(CSL_FEXT(dmaObj->dmaRegs->DMACH2TC1, DMA_DMACH2TC1_DMASTART) == TRUE);
                break;

            case DMA_CHAN3:
                 while(CSL_FEXT(dmaObj->dmaRegs->DMACH3TC1, DMA_DMACH3TC1_DMASTART) == TRUE);
                break;

            default:
                break;
        }
    }
}
