/*
 * $$$MODULE_NAME dda_i2s.c
 *
 * $$$MODULE_DESC dda_i2s.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/**
 *  \file   dda_i2s.c
 *
 *  \brief  DDA of I2S Interface Driver file
 *
 *  This file implements the DDA interface of I2S driver functions.
 *
 *  (C) Copyright 2006, Texas Instruments, Inc
 *
 *  \author     PR Mistral.
 *  \version    1.0
 *  \version    1.1     PR Mistral - modified for code review comments
 */

#include "dda_i2s.h"

/**
 *  \brief Opens I2S instance
 *
 *  Function to open the specified I2S driver instance.
 *  This API is called by the driver code from init function
 *  to open a particular I2S instance. Opens the I2S instance if
 *  it is available and parameters passed by application are valid.
 *
 *  \param i2sInstance  [IN]  I2S instance number
 *  \param i2sChanDir   [IN]  Channel direction Read/Write
 *  \param chanType     [IN]  Channel type(MONO/ STEREO)
 *  \param opMode       [IN]  Mode of operation(polled/interrupt)
 *  \param callback     [IN]  Callback argument
 *
 *  \return PSP_Handle - if successful, else NULL
 */
static PSP_Handle DDA_I2SOpen(PSP_I2SInstanceId      i2sInstance,
                              PSP_I2SChanDir         i2sChanDir,
                              PSP_I2SChanType        chanType,
                              PSP_I2SOpMode          opMode,
                              PSP_I2SAppCallBack     callback);

/**
 *  \brief Closes I2S instance
 *
 *  Function to close the specified I2S driver instance.
 *  This API is called by the driver code from deinit function
 *  to close one I2S instance.
 *
 *  \param i2sHandle    [IN]  The handle to the I2S driver
 *
 *  \return PSP_SOK - if successful else suitable error code.
 */
static Int16 DDA_I2SClose(PSP_Handle    i2sHandle);

DDA_I2SObj  DDA_I2SInfo[I2S_PER_CNT];
/**< Peripheral Instance DDA Handle */

/**
 *  \brief I2S Initialization function.
 *
 *  Function to initialize the I2S driver.
 *  This function is called from the application to initialize and open
 *  a particular I2S instance. One call from application for each instance.
 *  This API initializes and opens one I2S instance and returns the handle
 *  for that instance.
 *
 *  \param i2sInstance  [IN]  I2S instance of I2S.
 *  \param i2sChanDir   [IN]  Channel direction Read/Write
 *  \param chanType     [IN]  Channel type(MONO/ STEREO)
 *  \param opMode       [IN]  Mode of operation(polled/interrupt)
 *  \param i2sConfig    [IN]  I2S Config parameters
 *  \param callback     [IN]  Callback argument
 *
 *  \return PSP_Handle - successful
 *          NULL       - failure
 */
PSP_Handle I2S_INIT(PSP_I2SInstanceId      i2sInstance,
                    PSP_I2SChanDir         i2sChanDir,
                    PSP_I2SChanType        chanType,
                    PSP_I2SOpMode          opMode,
                    PSP_I2SConfig          *i2sConfig,
                    PSP_I2SAppCallBack     callback)
{
    PSP_Handle    i2sHandle;
    Int16         status;
    Int16         i2sNum;

    i2sHandle = NULL;
    status = PSP_SOK;
    i2sNum = i2sInstance;

    /* Check whether the operating mode parameter is valid or not */
    if((i2sConfig->i2sMode != PSP_I2S_MASTER) &&
       (i2sConfig->i2sMode != PSP_I2S_SLAVE))
    {
        return NULL;
    }

#if 0
    /* Check whether the wordlength parameter is valid or not */
    if(i2sConfig->word_len != PSP_I2S_WORDLEN_16)
    {
        return NULL;
    }
#endif	

    /* Call open function to open I2S device */
    i2sHandle  =  DDA_I2SOpen(i2sInstance, i2sChanDir, chanType,
                              opMode, callback);
    /* Check whether the device is being opened successfully or not */
    if(i2sHandle == NULL )
    {
        return NULL;
    }

#if 1 // this call resets all I2S => more than one call to I2S_INIT() will reset all I2S modules
    /* Call HwInit function to make the I2S device active */
    DDA_I2SHWInit();
#endif

    /* Configure the hardware registers of the device */
    status = DDC_I2SHwSetup(i2sHandle, i2sConfig);
    /* Check whether the hardware configuration is successful or not */
    if(status != PSP_SOK)
    {
        /* Close the Opened i2S if hardware configuration fails */
        DDC_I2SClose(i2sHandle);
        DDA_I2SInfo[i2sNum].i2sHandle = NULL;
        DDA_I2SInfo[i2sNum].i2sNum    = -1;
        return NULL;
    }
    return i2sHandle;
}

/**
 *  \brief Deintialises I2S device
 *
 *  Function to shutdown a I2S driver instance.
 *  Application code calls this API to deinitialise the I2S instance.
 *  After this function call all the I2S registers are reset and no further
 *  transmission can be done on that particular instance.
 *
 *  CAUTION: This API must be called after completing all the data tarnsfers
 *           for proper operation of the I2S. Next I2S init will fail for that
 *           particular instance if it is not deinitialized properly.
 *
 *  \param i2sHandle    [IN]  I2S device handle
 *
 *  \return PSP_SOK
 */
Int16 I2S_DeInit(PSP_Handle    i2sHandle)
{
    Int16   status;

    status = PSP_E_INVAL_PARAM;

    if(i2sHandle != NULL)
    {
        status = DDA_I2SClose(i2sHandle);
    }

    return status;
}

/**
 *  \brief Initializes CPU registers of I2S
 *
 *  Function to initialize the I2S hardware.
 *  This function initializes the CPU registers to bring the I2S out of idle
 *  and to enable the peripheral interrupts.
 *  This API is called from the system init function.
 *  NOTE: This API must be called only once during system initialization
 *
 *  \return   void
 */
void DDA_I2SHWInit(void)
{
    Uint16 i;

    /* In PCGCR, set I2S0 CG,I2S1 CG,I2S2 CG and I2S3 CG  to 0
     * for bringing the I2S modules out of idle */
    CSL_FINS((*IDLE_PCGCRL_ADDR), IDLE_PCGCRL_IIS0_IDLE, 0);
    CSL_FINS((*IDLE_PCGCRL_ADDR), IDLE_PCGCRL_IIS1_IDLE, 0);
    CSL_FINS((*IDLE_PCGCRL_ADDR), IDLE_PCGCRL_IIS2_IDLE, 0);
    CSL_FINS((*IDLE_PCGCRL_ADDR), IDLE_PCGCRL_IIS3_IDLE, 0);

    /* Configure the Peripheral Software Reset Counter Register */
    CSL_FINS((*PER_RSTCOUNT_ADDR), PER_RSTCOUNT, 2);

    /* Configure the Peripheral Reset Control Register */
    CSL_FINS((*PER_RESET_ADDR), PER_RESET_RESETEN_MMCSD0_MMCSD1_IIS0_IIS1, 1);
    CSL_FINS((*PER_RESET_ADDR), PER_RESET_RESETEN_LCD_IIS2_IIS3_UART_SPI, 1);

    /* Give some delay after reset */
    for (i=0; i<100; i++);

    /* Set the serial & parallel port field of the external bus selection
     * register to mux different I2S signals to serial/parallel port
     * which controls clock to I2S */
}

/**
 *  \brief Opens I2S instance
 *
 *  Function to open the specified I2S driver instance.
 *  This API is called by the driver code from init function
 *  to open a particular I2S instance. Opens the I2S instance if
 *  it is available and parameters passed by application are valid.
 *
 *  \param i2sInstance  [IN]  I2S instance number
 *  \param i2sChanDir   [IN]  Channel direction Read/Write
 *  \param chanType     [IN]  Channel type(MONO/ STEREO)
 *  \param opMode       [IN]  Mode of operation(polled/interrupt)
 *  \param callback     [IN]  Callback argument
 *
 *  \return PSP_Handle - if successful, else NULL
 */
static PSP_Handle DDA_I2SOpen(PSP_I2SInstanceId      i2sInstance,
                              PSP_I2SChanDir         i2sChanDir,
                              PSP_I2SChanType        chanType,
                              PSP_I2SOpMode          opMode,
                              PSP_I2SAppCallBack     callback)
{
    Int16    i2sNum;
    Int16 status;

    status = PSP_SOK;

    /* Check whether the parameters are valid or not */
    if((i2sChanDir != PSP_I2S_RECEIVE) &&
       (i2sChanDir != PSP_I2S_TRANSMIT))
    {
        return NULL;
    }
    if((chanType != PSP_I2S_CHAN_MONO) &&
       (chanType != PSP_I2S_CHAN_STEREO))
    {
        return NULL;
    }
    if((opMode != PSP_DMA_INTERRUPT) &&
       (opMode != PSP_DMA_POLLED) &&
       (opMode != PSP_I2S_INTERRUPT) &&
       (opMode != PSP_I2S_POLLED) )
    {
        return NULL;
    }

    /* Check the given I2S instance is valid or not */
    else if((i2sInstance >= PSP_I2S_0) && (i2sInstance <= PSP_I2S_3))
    {
        i2sNum = i2sInstance;
    }
    /* Retun error if parameter passed is invalid */
    else
    {
        return NULL;
    }

    /* Open a particular instance of I2S */
    status = DDC_I2SOpen((I2S_INSTANCE)i2sNum, chanType,
                          i2sChanDir, opMode, callback,
                          &DDA_I2SInfo[i2sNum].i2sHandle);
    DDA_I2SInfo[i2sNum].i2sNum = i2sNum;
    if(status != PSP_SOK)
    {
        return NULL;
    }

    return (PSP_Handle)DDA_I2SInfo[i2sNum].i2sHandle;
}

/**
 *  \brief Closes I2S instance
 *
 *  Function to close the specified I2S driver instance.
 *  This API is called by the driver code from deinit function
 *  to close one I2S instance.
 *
 *  \param i2sHandle    [IN]  The handle to the I2S driver
 *
 *  \return PSP_SOK - if successful else suitable error code.
 */
static Int16 DDA_I2SClose(PSP_Handle    i2sHandle)
{
    Int16       status;

    status = PSP_E_INVAL_PARAM;

    if(i2sHandle != NULL)
    {
        status = DDC_I2SClose(i2sHandle);
     /* DeSelectt the serial & parallel port field of the external bus selection
     * register to mux different I2S signals to serial/parallel port
     * which controls clock to I2S */
        CSL_FINS((*PERIPHSEL0_ADDR),PERIPHSEL0_SEL_SERIALPORT0, 0x0);
        CSL_FINS((*PERIPHSEL0_ADDR),PERIPHSEL0_SEL_SERIALPORT1, 0x0);
        CSL_FINS((*PERIPHSEL0_ADDR),PERIPHSEL0_SEL_PARALLELPORT, 0x0);
    }

    return status;
}

/**
 *  \brief Dma init function
 *
 *  Function to initiate the DMA for data transfer.
 *  This API is called by the application to initialize DMA HW.
 *
 *  \param i2sHandle  [IN]  I2S device handle
 *  \param dmaConfig  [IN]  DMA configuration structure
 *
 *  \return    hDma - dma channel handle
 */
DMA_ChanHandle I2S_DMA_INIT(PSP_Handle       i2sHandle,
                            PSP_DMAConfig    *dmaConfig)
{
    DMA_ChanHandle    hDma;

    hDma = NULL;

    if((i2sHandle != NULL) && (dmaConfig != NULL))
    {
        hDma  =  DDC_I2SDMAInit(i2sHandle, dmaConfig);
    }

    return hDma;
}

/**
 *  \brief Dma deinit function
 *
 *  Function to reinitialize a dma channel
 *  This API is called by the application to deinitialise a DMA channel
 *
 *  \param hDma  [IN]  dma channel handle
 *
 *  \return PSP_SOK - if successful or else suitable error code
 */
Int16 I2S_DMA_Deinit(DMA_ChanHandle    hDma)
{
    Int16    status;

    status  =  PSP_E_INVAL_PARAM;

    if(hDma != NULL)
    {
        status  =  DDC_I2SDMADeinit(hDma);
    }

    return status;
}

/**
 *  \brief Transmits I2S data through DMA
 *
 *  Function to initiate I2S transmission
 *  This API is called by the application to transmit I2S data using DMA
 *
 *  \param i2sHandle      [IN]  The handle to the I2S instance
 *  \param nextTxLeftBuf  [IN]  Pointer to next left transmit buffer
 *  \param nextTxRightBuf [IN]  Pointer to next right transmit buffer
 *  \param hDmaTxLeft     [IN]  Dma handle for left transmit channel
 *  \param hDmaTxRight    [IN]  Dma handle for right transmit channel
 *
 *  \return PSP_SOK - if successful else suitable error code.
 */
Int16 I2S_TransmitData(PSP_Handle        i2sHandle,
                       Uint32            *nextTxLeftBuf,
                       Uint32            *nextTxRightBuf,
                       DMA_ChanHandle    hDmaTxLeft,
                       DMA_ChanHandle    hDmaTxRight)
{
    Int16 status;

    status = PSP_E_INVAL_PARAM;

    if(i2sHandle != NULL)
    {
        status  =  DDC_I2SDMAWrite(i2sHandle,
                                   nextTxLeftBuf,
                                   nextTxRightBuf,
                                   hDmaTxLeft,
                                   hDmaTxRight);
    }

    return status;
}

/**
 *  \brief Receives I2S data through DMA
 *
 *  Function to initiate I2S reception
 *  This API is called from the application to receive I2S data using DMA
 *
 *  \param i2sHandle      [IN]  The handle to the I2S instance
 *  \param nextRxLeftBuf  [IN]  Pointer to next left receive buffer
 *  \param nextRxRightBuf [IN]  Pointer to next right receive buffer
 *  \param hDmaRxLeft     [IN]  Dma handle for left receive channel
 *  \param hDmaRxRight    [IN]  Dma handle for right receive channel
 *
 *  \return PSP_SOK - if successful else suitable error code.
 */
Int16 I2S_ReceiveData(PSP_Handle        i2sHandle,
                      Uint32            *nextRxLeftBuf,
                      Uint32            *nextRxRightBuf,
                      DMA_ChanHandle    hDmaRxLeft,
                      DMA_ChanHandle    hDmaRxRight)
{
    Int16 status;

    status = PSP_E_INVAL_PARAM;

    if(i2sHandle != NULL)
    {
        status  =  DDC_I2SDMARead(i2sHandle,
                                  nextRxLeftBuf,
                                  nextRxRightBuf,
                                  hDmaRxLeft,
                                  hDmaRxRight);
    }

    return status;

}

/**
 *  \brief Transmits I2S data through DMA
 *
 *  Function to initiate DMA transmission
 *  This API is called by the application to transmit I2S data using DMA
 *
 *  \param i2sHandle    [IN]  The handle to the I2S instance
 *  \param txBuffer     [IN]  Pointer to transmit data buffer.
 *  \param buffLen      [IN]  Transmit data length
 *
 *  \return PSP_SOK - if successful else suitable error code.
 */
Int16 I2S_DMA_ISR_OUTPUT(PSP_Handle    i2sHandle,
                         Uint16        *txBuffer,
                         Uint16        buffLen)
{
    Int16 status;

    status  =  PSP_E_INVAL_PARAM;

    if((i2sHandle != NULL) && (txBuffer != NULL) && (buffLen > 0))
    {
        status  =  PSP_SOK;
    }
    return status;
}

/**
 *  \brief Receives I2S data through DMA
 *
 *  Function to initiate I2S reception
 *  This API is called from the application to receive I2S data using DMA
 *
 *  \param i2sHandle    [IN]  The handle to the I2S instance
 *  \param rxBuffer     [IN]  Pointer to receive data buffer.
 *  \param buffLen      [IN]  Receive data length
 *
 *  \return PSP_SOK - if successful else suitable error code.
 */
Int16 I2S_DMA_ISR_INPUT(PSP_Handle    i2sHandle,
                        Uint16*       rxBuffer,
                        Uint16        buffLen)
{
    Int16 status;

    status = PSP_E_INVAL_PARAM;

    if((i2sHandle != NULL) && (rxBuffer != NULL) && (buffLen > 0))
    {
        status = PSP_SOK;
    }
    return status;
}
