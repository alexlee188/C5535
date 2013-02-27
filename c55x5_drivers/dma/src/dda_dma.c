/*
 * $$$MODULE_NAME dda_dma.c
 *
 * $$$MODULE_DESC dda_dma.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/**
 *  \file   dda_dma.c
 *
 *  \brief  DDA of DMA Interface Driver file
 *
 *  This file contains the implementation of DDA of the DMA driver
 *  to utilize the services of the DMA device driver.
 *
 *  (C) Copyright 2006, Texas Instruments, Inc
 *
 *  \author     PR Mistral.
 *  \version    1.0
 */

#include "dda_dma.h"

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
void DMA_HwInit(void)
{
    ioport volatile unsigned int *io_ptr;
    Uint16 i;

    /* In PCGCR LSW, set DMA0 CG to 0 for bringing DMA0 module out of idle */
    CSL_FINS((*IDLE_PCGCRL_ADDR), IDLE_PCGCRL_DMA0_IDLE, 0);

    /* In PCGCR MSW, set DMA1 CG, DMA2 CG and DMA3 CG  to 0
     * for bringing the  DMA1, DMA2, DMA3 modules out of idle */
    CSL_FINS((*IDLE_PCGCRM_ADDR), IDLE_PCGCRM_DMA1_CG, 0);
    CSL_FINS((*IDLE_PCGCRM_ADDR), IDLE_PCGCRM_DMA2_CG, 0);
    CSL_FINS((*IDLE_PCGCRM_ADDR), IDLE_PCGCRM_DMA3_CG, 0);

    /* Configure the Peripheral Software Reset Counter Register */
    CSL_FINS((*PER_RSTCOUNT_ADDR), PER_RSTCOUNT, 2);

    /* Configure the Peripheral Reset Control Register */
    CSL_FINS((*PER_RESET_ADDR), PER_RESET_RESETEN_DMA, 1);

    /* Give some delay after reset */
    for (i=0; i<100; i++);

    /* Clear the IFR of DMA */
    io_ptr    =  (ioport volatile unsigned int*)(DMA_IFR_ADDR);
    *io_ptr   =  DMA_IFR_RESET_VAL;

    /* Enable interrupts for DMA module */
    CSL_FINS((*CPU_IER0_ADDR), CPUIER0_DMAINT_ENABLE, 1);
    io_ptr     =  (ioport volatile unsigned int*)(DMA_IMR_ADDR);
    *io_ptr    =  DMA_IMR_VAL;
}

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
void DMA_DrvInit(void)
{
    DDC_DMADrvInit();
}

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
DMA_ChanHandle DMA_RequestChan(PSP_DMAReqDevId dmaReqDevId)
{
    DMA_ChanHandle hDma;

    hDma  =  NULL;

    /* Check whether the given request is valid or not */
    if(dmaReqDevId < PSP_DMA_REQDEVID_MAX)
    {
        hDma  =  DDC_DMARequestChan(dmaReqDevId);
    }
    return hDma;
}

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
PSP_Result DMA_ReleaseChan(DMA_ChanHandle    hDma)
{
    PSP_Result status;

    status = PSP_E_INVAL_PARAM;

    if(hDma != NULL)
    {
        status  =  DDC_DMAReleaseChan(hDma);
    }
    return status;
}

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
                          PSP_DMAConfig     *dmaConfig)
{
    PSP_Result status;

    status = PSP_E_INVAL_PARAM;

    if((hDma != NULL) && (dmaConfig != NULL))
    {
        /* Call the DDC function to configure DMA channel */
        status  =  DDC_DMAConfigChan(hDma, dmaConfig);
    }
    return status;
}

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
PSP_Result DMA_StartTransfer(DMA_ChanHandle    hDma)
{
    PSP_Result status;

    status = PSP_E_INVAL_PARAM;

    if(hDma != NULL)
    {
        /* Call the DDC function to start data transfer */
        status  =  DDC_DMAStartTransfer(hDma);
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
 * \param    hDma     [IN]    DMA channel handle.
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
PSP_Result DMA_StopTransfer(DMA_ChanHandle    hDma)
{
    PSP_Result status;

    status = PSP_E_INVAL_PARAM;

    if(hDma != NULL)
    {
        /* Call the DDC function to stop data transfer */
        status  =  DDC_DMAStopTransfer(hDma);
    }
    return status;
}

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
Uint32 DMA_AdjustAddrOffset(Uint32    address)
{
    Uint32 modAddr;

    /* Call DDC API for modifying the address */
    modAddr  =  DDC_DMAAdjustAddrOffset(address);

    return(modAddr);
}

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
                             Uint32            newSrcAddr)

{
    PSP_Result status;

    status = PSP_E_INVAL_PARAM;

    if(hDma != NULL)
    {
        /* Call the DDC function to change source address */
        status  =  DDC_DMAChangeSrcAddr(hDma, newSrcAddr);
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
 * \param    newDestAddr [IN]    New destination address
 *
 * \return   PSP_SOK - If operation is successful
 *                     else suitable error.
 */
PSP_Result DMA_ChangeDestAddr(DMA_ChanHandle    hDma,
                              Uint32            newDestAddr)
{
    PSP_Result status;

    status = PSP_E_INVAL_PARAM;

    if(hDma != NULL)
    {
        /* Call the DDC function to change destination address */
        status  =  DDC_DMAChangeDestAddr(hDma, newDestAddr);
    }
    return status;
}
