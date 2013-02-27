/*
 * $$$MODULE_NAME psp_dma.h
 *
 * $$$MODULE_DESC psp_dma.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/**
 *    \file    psp_dma.h
 *
 *    \brief    DMA interface definitions
 *
 *    This file contains the data types and symbolic definitions
 *    that are needed by the application to utilize the services of the DMA
 *    device driver on DA225 board.
 *
 *    (C) Copyright 2006, Texas Instruments, Inc
 *
 *    \author        PR Mistral
 *
 *    \version    0.1     Initial implementation
 *
 */

#ifndef _PSP_DMA_H_
#define _PSP_DMA_H_

#include <psp_common.h>
#include <tistdtypes.h>

/* Enumerated variables for DMA transfer control register */
/**
 * \brief DMA Auto reload bit
 *
 * If this bit is enabled, DMA auto reloads source and destination addresses
 * and restarts the data transfer.
 */
typedef enum
{
    PSP_DMA_AUTORELOAD_DISABLE,   /**< Auto reload mode is disabled          */
    PSP_DMA_AUTORELOAD_ENABLE     /**< Auto reload mode is enabled           */
} PSP_DMAAutoRealoadMode;

/**
 * \brief DMA destination data size bit
 *
 * Size of single unit of destination data buffer(Uint8,16 or 32 )
 */
typedef enum
{
    PSP_DMA_DEST_DATASIZE_32,     /**< Destination data is 32-bits           */
    PSP_DMA_DEST_DATASIZE_16,     /**< Destination data is 16-bits           */
    PSP_DMA_DEST_DATASIZE_8       /**< Destination data is 8-bits            */
} PSP_DMADestdataSize;

/**
 * \brief DMA burst mode bit
 *
 * This represents the number of words to be transferred by a channel
 * before giving the DMA FIFO to next channel
 */
typedef enum
{
    PSP_DMA_TXBURST_1WORD,        /**< One 32-bit transfer:1-word burst      */
    PSP_DMA_TXBURST_2WORD,        /**< Two 32-bit transfer:2-word burst      */
    PSP_DMA_TXBURST_4WORD,        /**< Four 32-bit transfer:4-word burst     */
    PSP_DMA_TXBURST_8WORD,        /**< Eight 32-bit transfer:8-word burst    */
    PSP_DMA_TXBURST_16WORD        /**< Sixteen 32-bit transfer:16-word burst */
} PSP_DMATxBurstLen;

/**
 * \brief DMA source data size bit
 *
 * Size of single unit of source data buffer(Uint8,16 or 32 )
 */
typedef enum
{
    PSP_DMA_SRC_DATASIZE_32,      /**< Source data is 32-bits                */
    PSP_DMA_SRC_DATASIZE_16,      /**< Source data is 16-bits                */
    PSP_DMA_SRC_DATASIZE_8        /**< Source data is 8-bits                 */
} PSP_DMASrcDataSize;

/**
 * \brief DMA transfer status parameter
 *
 * Gives the status of the data transfer. This is passed to the application
 * after completing the data transfer through callback.
 */
typedef enum
{
    PSP_DMA_TRANSFER_COMPLETE,    /**< Transfer is completed successfully    */
    PSP_DMA_TRANSFER_ERROR        /**< Error in data transfer                */
} PSP_DMATransferStatus;

/**
 * \brief DMA event Id
 *
 * This represents the event by which DMA transfer is being triggered
 */
typedef enum
{
    PSP_DMA_EVT_I2S0_TX = 0,      /**< Tx of I2S0 data event triggers DMA    */
    PSP_DMA_EVT_I2S0_RX,          /**< Rx of I2S0 data event triggers DMA    */
    PSP_DMA_EVT_I2S1_TX,          /**< Tx of I2S1 data event triggers DMA    */
    PSP_DMA_EVT_I2S1_RX,          /**< Rx of I2S1 data event triggers DMA    */
    PSP_DMA_EVT_I2S2_TX,          /**< Tx of I2S2 data event triggers DMA    */
    PSP_DMA_EVT_I2S2_RX,          /**< Rx of I2S2 data event triggers DMA    */
    PSP_DMA_EVT_I2S3_TX,          /**< Tx of I2S3 data event triggers DMA    */
    PSP_DMA_EVT_I2S3_RX,          /**< Rx of I2S3 data event triggers DMA    */
    PSP_DMA_EVT_MMC_SD0_TX,       /**< MMC/SD0 TX event triggers DMA         */
    PSP_DMA_EVT_MMC_SD0_RX,       /**< MMC/SD0 RX event triggers DMA         */
    PSP_DMA_EVT_MMC_SD1_TX,       /**< MMC/SD1 TX event triggers DMA         */
    PSP_DMA_EVT_MMC_SD1_RX,       /**< MMC/SD1 RX event triggers DMA         */
    PSP_DMA_EVT_TIMER0,           /**< timer0 event triggers DMA             */
    PSP_DMA_EVT_TIMER1,           /**< timer1 event triggers DMA             */
    PSP_DMA_EVT_TIMER2,           /**< timer2 event triggers DMA             */
    PSP_DMA_EVT_UART_TX,          /**< Tx of UART data event triggers DMA    */
    PSP_DMA_EVT_UART_RX,          /**< Rx of UART data event triggers DMA    */
    PSP_DMA_EVT_I2C_TX,           /**< Tx of I2C data event triggers DMA     */
    PSP_DMA_EVT_I2C_RX,           /**< Rx  of I2C data event triggers DMA    */
    PSP_DMA_EVT_SAR_AD,           /**< SAR A/D event triggers DMA            */
    PSP_DMA_EVT_NONE,             /**< No DMA event(SW trig./mem to mem)     */
    PSP_DMA_EVT_INVALID           /**< Invalid event Id for error checking   */
} PSP_DMAEvtType;

/**
 * \brief DMA channel direction
 *
 * This is provided to the application to give information about DMA
 * operation. DMA read or DMA write.
 */
typedef enum
{
    PSP_DMA_READ,                 /**< Reading data from a device            */
    PSP_DMA_WRITE                 /**< Writing data to a device              */
} PSP_DMAChanDir;

/**
 * \brief DMA data transfer type
 *
 * This is provided to the application to give information about DMA
 * transfer type is between IO and memory, or memory and memory
 */
typedef enum
{
    PSP_DMA_TRANSFER_IO_MEMORY,   /**< Data transfer is b/w IO & memory      */
    PSP_DMA_TRANSFER_MEMORY       /**< Data transfer is b/w memory to memory */
} PSP_DMATransferType;

/**
 * \brief DMA request device Id
 *
 * Id of the device that is seeking the service from DMA driver.
 * Based on this Id only DMA driver decides which free channel to allocate
 */
typedef enum
{
    PSP_DMA_I2S0 = 0,             /**< I2S0 request for DMA                  */
    PSP_DMA_MMC_SD0,              /**< MMC_SD0 request for DMA               */
    PSP_DMA_MMC_SD1,              /**< MMC_SD1 request for DMA               */
    PSP_DMA_UART,                 /**< UART request for DMA                  */
    PSP_DMA_I2S2,                 /**< I2S2 request for DMA                  */
    PSP_DMA_I2C,                  /**< I2C request for DMA                   */
    PSP_DMA_I2S3,                 /**< I2S3 request for DMA                  */
    PSP_DMA_SAR,                  /**< SAR request for DMA                   */
    PSP_DMA_I2S1,                 /**< I2S1 request for DMA                  */
    PSP_DMA_NAND_CE0,             /**< NAND CE0 request for DMA              */
    PSP_DMA_NAND_CE1,             /**< NAND CE1 request for DMA              */
    PSP_DMA_EMIF,                 /**< EMIF request for DMA                  */
    PSP_DMA_MEMORY,                  /**< Dev Id for memory to memory transfer  */
    PSP_DMA_REQDEVID_MAX          /**< Invalid device ID                     */
} PSP_DMAReqDevId;

/**
 * \brief Type of the DMA triggering
 *
 * This provides the information about DMA triggering.
 * DMA transfer can be triggered by software or hardware event
 */
typedef enum
{
    PSP_DMA_SOFTWARE_TRIGGER,     /**< Triggering by software instruction    */
    PSP_DMA_EVENT_TRIGGER         /**< Triggering by hardware event          */
} PSP_DMATriggerType;

/**
 * \brief DMA application callback function
 *
 * Application callback pointer which is used to call application callback
 * function after completing the data transfer.
 *
 * \param   status[IN]         Status of the DMA data transfer
 * \param   dataCallback[IN]   DMA data callback pointer
 * \return  none
 */
typedef void (*dmaCallBack)(PSP_DMATransferStatus status,
                            void *dataCallback);

/**
 * \brief DMA configuration structure
 *
 * Contains configuration parameters for DMA hardware registers.
 * This has to be filled by the drivers using DMA.
 */
typedef struct _PSP_DMAConfig
{
    Bool                     pingPongMode; /**< Ping/pong mode               */
    PSP_DMAAutoRealoadMode   autoMode;     /**< Auto reloadmode bit          */
    //PSP_DMADestdataSize      destDataSize; /**< Size of the dest. data buff  */
    PSP_DMATxBurstLen        burstLen;     /**< Length of the transmit burst */
    //PSP_DMASrcDataSize       srcDataSize;  /**< Size of the src data buffer  */
    PSP_DMAEvtType           dmaEvt;       /**< Type of the DMA event        */
    PSP_DMAChanDir           chanDir;      /**< Channel direction - Rd/Wr    */
    PSP_DMATriggerType       trigger;      /**< Type of DMA triggering HW/SW */
    PSP_DMATransferType      trfType;      /**< Type of the data transfer    */
    Uint32                   srcAddr;      /**< Src. data buffer address     */
    Uint32                   destAddr;     /**< Dest. data buffer address    */
    Uint16                   dataLen;      /**< Length of data to transfer
                                                in number of bytes           */
    dmaCallBack              callback;     /**< Application callback pointer */
    void                     *dataCallback;/**< Application data callback ptr*/
} PSP_DMAConfig;

#endif  /* _PSP_DMA_H_ */
