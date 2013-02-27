/*
 * $$$MODULE_NAME ddc_dma.h
 *
 * $$$MODULE_DESC ddc_dma.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/**
 *  \file   ddc_dma.h
 *
 *  \brief  header file for DMA DDC implementation
 *
 *  This file contains prototypes, data types required for
 *  DDC implementation of DMA device driver.
 *
 *  (C) Copyright 2005, Texas Instruments, Inc
 *
 *  \author     PR  Mistral
 *  \version    1.0 Initial implementation
 */

#ifndef _DDC_DMA_H_
#define _DDC_DMA_H_

#include <psp_dma.h>
#include <tistdtypes.h>
#include <corazon.h>
#include <cslr_dma_001.h>
#include <cslr_dmaevtint_001.h>

#define DMA_ENGINE_COUNT            (CSL_DMA_PER_CNT)
/**< Number of DMA devices on the chip                                       */
#define DMA_CHAN_COUNT              (4)
/**< Number of channels on each DMA device                                   */
#define DMA_CHAN_OBJ_COUNT          (DMA_ENGINE_COUNT * DMA_CHAN_COUNT)
/**< Total number of DMA channel objects                                     */
#define DMA_REQDEV_COUNT            (13)
/**< Number of devices that can use DMA                                      */
#define DMA_EVT_COUNT               (21)
/**< Number of hardware events on DMA                                        */
#define DMA_ADDR_MODE_FIXED         (0x02)   /**< Value for fixed addr mode  */
#define DMA_ADDR_MODE_INCREMENT     (0x00)   /**< Value for incr. addr mode  */
#define DMA_MINTRFLEN               (0x0004) /**< Min.legth of data transfer */
/* SARAM and DARAM address ranges on CORAZON */
#define DARAM_START_ADDR            (0x00060)/**< DARAM starting address     */
#define DARAM_END_ADDR              (0x07FFF)/**< DARAM ending address       */
#define SARAM_START_ADDR            (0x08000)/**< SARAM starting address     */
#define SARAM_END_ADDR              (0x27FFF)/**< SARAM ending address       */

/* Macros for DMA event values */
#define DMA_NO_EVTVAL               (0x0000) /**< No event for DMA(default)  */
#define I2S0_TX_EVTVAL              (0x0001) /**< I2S0 tx event value        */
#define I2S0_RX_EVTVAL              (0x0002) /**< I2S0 rx event value        */
#define I2S1_TX_EVTVAL              (0x0001) /**< I2S1 tx event value        */
#define I2S1_RX_EVTVAL              (0x0002) /**< I2S1 rx event value        */
#define I2S2_TX_EVTVAL              (0x0001) /**< I2S2 tx event value        */
#define I2S2_RX_EVTVAL              (0x0002) /**< I2S2 rx event value        */
#define I2S3_TX_EVTVAL              (0x0004) /**< I2S3 tx event value        */
#define I2S3_RX_EVTVAL              (0x0005) /**< I2S3 rx event value        */
#define MMC_SD0_TX_EVTVAL           (0x0005) /**< MMCSD0 tx event value      */
#define MMC_SD0_RX_EVTVAL           (0x0006) /**< MMCSD0 rx event value      */
#define MMC_SD1_TX_EVTVAL           (0x0007) /**< MMCSD1 tx event value      */
#define MMC_SD1_RX_EVTVAL           (0x0008) /**< MMCSD1 rx event value      */
#define TIMER0_EVTVAL               (0x000C) /**< Timer0 event value         */
#define TIMER1_EVTVAL               (0x000D) /**< Timer1 event value         */
#define TIMER2_EVTVAL               (0x000E) /**< Timer2 event value         */
#define UART_TX_EVTVAL              (0x0005) /**< UART tx event value        */
#define UART_RX_EVTVAL              (0x0006) /**< UART rx event value        */
#define I2C_TX_EVTVAL               (0x0001) /**< I2C tx event value         */
#define I2C_RX_EVTVAL               (0x0002) /**< I2C rx event value         */
#define SAR_AD_EVTVAL               (0x0003) /**< SAR A/D event value        */

/**
 * \brief DMA engine number
 *
 * Enumeration for the DMA engine numbers
 */
typedef enum _DMAEngineId
{
    DMA_ENGINE0 = 0,                    /**< DMA engine instance 0           */
    DMA_ENGINE1,                        /**< DMA engine instance 1           */
    DMA_ENGINE2,                        /**< DMA engine instance 2           */
    DMA_ENGINE3                         /**< DMA engine instance 3           */
} DMAEngineId;

/**
 * \brief DMA channel number
 *
 * Enumeration for the DMA channel numbers on each DMA engine
 */
typedef enum _DMAChanNum
{
    DMA_CHAN0 = 0,                      /**< DMA channel number 0            */
    DMA_CHAN1,                          /**< DMA channel number 1            */
    DMA_CHAN2,                          /**< DMA channel number 2            */
    DMA_CHAN3                           /**< DMA channel number 3            */
} DMAChanNum;

/**
 * \brief DMA engine object
 *
 * Holds information about the DMA engine hardware register pointers
 */
typedef struct _DMA_EngineObj
{
    CSL_DmaDrvRegsOvly        dmaRegs;     /**< register overlay struct pointer */
    CSL_DmaEvtIntRegsOvly  dmaEvtRegs;  /**< Event reg. overlay struct ptr   */
} DMA_EngineObj;

/**
 * \brief DMA channel object
 *
 * Contains the data of a specific DMA channel
 */
typedef struct _DMA_ChannelObj
{
    Uint16                chanNum;      /**< DMA channel number              */
    Bool                  isChanFree;   /**< Indicates Availability of chan  */
    PSP_DMAChanDir        chanDir;      /**< Channel direction - Rd/Wr       */
    PSP_DMATriggerType    trigger;      /**< Type of DMA triggering HW/SW    */
    PSP_DMATransferType   trfType;      /**< Type of the data transfer       */
    Uint32                srcAddr;      /**< Src. data buffer address        */
    Uint32                destAddr;     /**< Dest. data buffer address       */
    Uint16                dataLen;      /**< Length of data to transfer in
                                             number of words                 */
    dmaCallBack           callback;     /**< Application callback pointer    */
    void                  *dataCallback;/**< Application data callback ptr   */
} DMA_ChannelObj;

typedef DMA_ChannelObj  *DMA_ChanHandle;

/**
 * \brief Lookup table for requesting device Id
 *
 * Gives the mapping of DMA engine numbers with DMA requesting device Id's
 */
typedef struct _DMA_ChanLookUp
{
    PSP_DMAReqDevId       dmaReqDevId;  /**< Id of device requesting DMA     */
    Uint16                chanStartNum; /**< DMA channel starting number     */
} DMA_ChanLookUp;

/**
 * \brief Lookup table for DMA events
 *
 * Gives the mapping of DMA event values with DMA event Id's
 */
typedef struct _DMA_EvtLookUp
{
    PSP_DMAEvtType        dmaEvtId;     /**< Event Id which triggers DMA     */
    Uint16                dmaEvtValue;  /**< Register value of the DMA event */
} DMA_EvtLookUp;

/* Prototype declarations for DMA DDC APIs */
/**
 * \brief Initializes DMA driver.
 *
 * This API initializes all the DMA engine and channel objects.
 *
 * \return   None
 */
void DDC_DMADrvInit(void);

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
DMA_ChanHandle DDC_DMARequestChan(PSP_DMAReqDevId   dmaReqDevId);

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
PSP_Result DDC_DMAReleaseChan(DMA_ChanHandle hDma);

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
                             PSP_DMAConfig      *dmaConfig);

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
Uint32 DDC_DMAAdjustAddrOffset(Uint32    address);

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
                                Uint32            newSrcAddr);

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
                                 Uint32            newDestAddr);

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
PSP_Result DDC_DMAStartTransfer(DMA_ChanHandle    hDma);

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
PSP_Result DDC_DMAStopTransfer(DMA_ChanHandle    hDma);

/**
 * \brief Interrupt service routine for dma
 *
 * DMA interrupts the CPU after transferring given length of data.
 * CPU dispatches the isr in which registered callback function is invoked
 *
 *  \return     void
 */
void DMA_Isr(void);

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
                                Uint32            NextTxBuf);

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
                                 Uint32            NextRxBuf);

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
void DDC_DMACheckTxComplete(DMA_ChanHandle    hDma);

#endif    /* _DDC_DMA_H_ */
