/*
 * $$$MODULE_NAME dda_dma.h
 *
 * $$$MODULE_DESC dda_dma.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/**
 *  \mainpage PSP DMA Driver
 *
 *  \section Introduction
 *
 *  \subsection xxx Purpose and Scope
 *  The purpose of this document is to explain the DMA driver on the PSP
 *  Framework architecture.
 *
 *  \subsection aaa DMA Usage and Working
 *  DMA(Direct Memory Access) is used to free off the CPU from
 *  bulky data transfers.
 *  \n DMA transfers the given length of data provided the source address,
 *  destination address and transfer control registers are configured properly.
 *  CPU initiates the DMA transfer and DMA interrupts the CPU after completing
 *  the data transfer.\n DMA can be used in two modes: Non-auto reload mode
 *  in which dma transfers given length of data and stops the data transfer.
 *  Auto reload mode in which after transferring the given length of data
 *  DMA reloads same source and destination address and restarts the transfer.
 *  Maximum length of data that DMA can transfer in one request is fixed
 *  and depends on DMA hardware design.
 *
 *  \subsection bbb Application Interfaces
 *  DMA driver is providing five APIs for normal operation.
 *  \n DMA_RequestChan(), DMA_ConfigChan(), DMA_StartTransfer(),
 *  DMA_StopTransfer() and DMA_ReleaseChan().
 *
 *  To transfer some data on DMA
 *  \n 1.Request for the DMA channel using DMA_RequestChan() API.
 *  Channel handle is returned by this API if channel is available.
 *  Modules requesting for DMA needs to pass its device Id to this API.
 *  \n Note: Refer psp_dma.h file for the device Ids.
 *
 *  \n 2.After getting valid handle, Configure the channel using
 *  DMA_ConfigChan() API. Application needs to fill the data structure
 *  that contains all configuration parameters and give it to this API.
 *  \n Note: Refer psp_dma.h file for configuration parameters.
 *
 *  \n 3.After configuring the channel driver can start the transfer using
 *  DMA_StartTransfer() API. After calling this API application should wait
 *  for an event to occur(Ex:pend on semaphore).Once the transfer is completed
 *  application gets a call from DMA Isr through application callback function.
 *  In the callback function make this event to happen(Ex:Release semaphore).
 *
 *  \n 4.After completing the transfer application must release the channel
 *  using DMA_ReleaseChan(). This API must be called after completing all the
 *  transfers, Otherwise that particular channel cannot be used
 *  any other modules until the system is restarted.
 *
 *  \n 5.DMA_StopTransfer() API can be used to abort the data transfer in
 *  normal mode or to stop the data transfer in auto-reload mode.
 *
 *  \n 6.Two other APIs DMA_ChangeSrcAddr(), DMA_ChangeDestAddr() are provided
 *  to change source address and destination address respectively to support
 *  Ping-Pong buffer configuration.
 *
 *  \subsection ccc Driver Scope
 *  DMA driver supports transfers between CPU memory and IO peripherals.
 *  It can also support the transfers with in the memory.
 *  The driver can support transfer on four DMA engines of the corazon and
 *  four channels on each engine independently and simultaneously.
 *  It can support up to 64k bytes of data transfer on each channel.
 *
 */

/**
 *  \file   dda_dma.h
 *
 *  \brief  header file for DMA DDA implementation
 *
 *  This file contains prototypes, data types required for
 *  DDA implementation of DMA device driver.
 *
 *  (C) Copyright 2005, Texas Instruments, Inc
 *
 *  \author    PR  Mistral
 *  \version   1.0 initial implementation
 */

#ifndef _DDA_DMA_H_
#define _DDA_DMA_H_

#include "ddc_dma.h"

/* Macros for DMA interrupt register addresses */
#define DMA_IFR_ADDR             (0x1C30)/**< DMA IFR address                */
#define DMA_IMR_ADDR             (0x1C31)/**< DMA IMR register address       */
#define DMA_IFR_RESET_VAL        (0xFFFF)/**< DMA IFR register reset val     */
#define DMA_IMR_VAL              (0xFFFF)/**< DMA IMR register enable value  */

/**
 * \brief initializes the CPU hardware registers of DMA
 *
 * This API must be called only once from the system init function
 * to initialize CPU registers for bringing the DMA out of idle
 * and to enable interrupts.
 *
 * Caution !!!! This function has to be called only once.
 *
 * \return   void
 */
void DMA_HwInit(void);

/**
 * \brief Initializes DMA driver.
 *
 * This API initializes all the DMA engine and channel objects.
 * This API assigns base address for hardware register pointers and
 * initializes channel related information.
 * This API must be called only once at the time of system initialization.
 *
 * Caution !!!! This function has to be called only once.
 *
 * \return   None
 */
void DMA_DrvInit(void);

/**
 * \brief Allocates a requested channel
 *
 * This API checks the availability of requested channel and allocates it
 * if available and returns the handle to that channel.
 * This API checks for one free channel out of four channels that are
 * accessible for a particular IO peripheral.
 * Only one channel can be requested in one call.
 *
 * \param    dmaReqDevId [IN]    Id of the device requesting DMA
 *
 * \return   hDma                DMA channel handle.
 */
DMA_ChanHandle DMA_RequestChan(PSP_DMAReqDevId    dmaReqDevId);

/**
 * \brief Releases a channel
 *
 * This API releases a channel allocated to a device, so that
 * it can be used by other devices.No further transfers are allowed on the
 * released channel until it is allocated again. Only one channel can be
 * released in one call.
 *
 * NOTE: This API 'must' be called after completing all the DMA transfers
 *       to make the channel available for other modules.
 *
 * \param    hDma     [IN]    DMA channel handle.
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
PSP_Result DMA_ReleaseChan(DMA_ChanHandle    hDma);

/**
 * \brief Configures the DMA channel
 *
 * This API configures the DMA channels with given configuration parameters
 * This API is called by the application after getting the valid handle
 * to the requested channel.
 *
 * \param    hDma      [IN]    DMA instance handle.
 * \param    dmaConfig [IN]    DMA configuration structure
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
PSP_Result DMA_ConfigChan(DMA_ChanHandle    hDma,
                          PSP_DMAConfig     *dmaConfig);

/**
 * \brief Starts DMA data transfer
 *
 * This function starts data transfer on a given DMA channel.
 * Transfer length of bytes are transferred in each call in
 * non-auto reload mode. This API just initiates the data transfer and
 * returns back to the application. Completion of the data transfer is
 * intimated to the application through application callback function.
 *
 * \param    hDma     [IN]    DMA channel handle
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
PSP_Result DMA_StartTransfer(DMA_ChanHandle    hDma);

/**
 * \brief Stops DMA data transfer
 *
 * This function stops data transfer on given channel.
 * This can be used to abort the data transfer in normal mode or to stop
 * the data transfer in Auto-reload mode.
 *
 * \param    hDma     [IN]    DMA channel handle.
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
PSP_Result DMA_StopTransfer(DMA_ChanHandle    hDma);

/**
 * \brief Routine to modify address for DMA offset
 *
 * This function modifies the source/destination address specified by the
 * application according to the requirement of DMA
 *
 *  \param      address [IN]    Address given by application
 *
 *  \return     modAddr - modified address
 */
Uint32 DMA_AdjustAddrOffset(Uint32    address);

/**
 * \brief Changes the source address
 *
 * This API is called to change the source address of any channel.
 * This API is used for supporting PING - PONG buffer configuration
 *
 * \param    hDma       [IN]    DMA channel handle.
 * \param    newSrcAddr [IN]    New source address
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
PSP_Result DMA_ChangeSrcAddr(DMA_ChanHandle    hDma,
                             Uint32            newSrcAddr);

/**
 * \brief Changes the destination address
 *
 * This API is called to change the destination address of any channel.
 * This API is used for supporting PING - PONG buffer configuration
 *
 * \param    hDma        [IN]    DMA instance handle.
 * \param    newDestAddr [IN]    New destination address
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
PSP_Result DMA_ChangeDestAddr(DMA_ChanHandle    hDma,
                              Uint32            newDestAddr);

#endif    /* _DDA_DMA_H_ */
