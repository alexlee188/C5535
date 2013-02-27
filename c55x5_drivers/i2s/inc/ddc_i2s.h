/*
 * $$$MODULE_NAME ddc_i2s.h
 *
 * $$$MODULE_DESC ddc_i2s.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/**
 *  \file   ddc_i2s.h
 *
 *  \brief I2S driver header file
 *
 *  This file defines the various data structures used by I2S driver core.
 *
 *  (C) Copyright 2006, Texas Instruments, Inc
 *
 *  \author     PR Mistral
 *  \version    1.0
 *  \version    1.1     PR Mistral - modified for code review comments
 */

#ifndef _DDC_I2S_H_
#define _DDC_I2S_H_

#include "llc_i2s.h"
#include "psp_i2s.h"
#include <dda_dma.h>

/**
 *   \brief DDC object structure
 *
 *   This contains the object LLC structure
 */
typedef struct
{
    I2S_INSTANCE          i2sNum;       /**< Instance Id                     */
    Uint16                configured;   /**< Track device conf. DONE         */
    ioport CSL_I2sDrvRegs    *regs;        /**< Pointer to I2S HW registers     */
    PSP_I2SChanDir        chanDir;      /**< Direction of I2S channel RD/WR  */
    PSP_I2SChanType       chanType;     /**< Type of the audio channel       */
    PSP_I2SAppCallBack    applCallBack; /**< Application callback pointer    */
    PSP_I2SOpMode         opMode;       /**< Mode of the data trans(I2S/DMA) */
    PSP_DMAReqDevId       i2sDmaId;     /**< I2S DMA instance Id             */
} DDC_I2SObj;

/**
 *   \brief DDC object structure pointer
 *
 */
typedef DDC_I2SObj *DDC_I2SHandle;

/**
 *  \brief I2S hardware structure
 *
 *  This contains all the hardware configuration enums of the I2S
 *  This structure is used to pass the register bit values to LLC functions
 */
typedef struct
{
    I2S_TXMODE_TYPE         eTxMode;    /**< I2S transfer mode bit           */
    I2S_FSDIV_TYPE          eFsdiv;     /**< I2S frame sync divider value    */
    I2S_CLKDIV_TYPE         eClkdiv;    /**< I2S clock divider value         */
    I2S_DATATX_WORDLEN_TYPE eWordLength;/**< I2S word length bit             */
    I2S_TX_CLKPOL_TYPE      eClkPol;    /**< I2S clock polarity bit          */
    I2S_FSPOL_TYPE          eFsPol;     /**< I2S frame sync polarity bit     */
    I2S_DATAFORMAT_TYPE     eDataFormat;/**< I2S data format bit             */
    I2S_DATADELAY_TYPE      eDataDelay; /**< I2S data delay bit              */
    I2S_DATAPACK_TYPE       eDataPack;  /**< I2S data pack bit               */
    I2S_MONO_TYPE           eDataType;  /**< I2S audio data type             */
    I2S_SIGNEXT_TYPE        eSignExt;   /**< I2S sign extension bit          */
} DDC_I2S_HwSetup;

/**
 *  \brief Opens I2S instance
 *
 *  Function to open the specified I2S device instance.
 *  This API is called by the driver code from DDA open function
 *
 *  \param eI2SNum    [IN]  I2S instance
 *  \param chType     [IN]  Channel type(MONO/ STEREO)
 *  \param i2sChanDir [IN]  Channel direction Read/Write
 *  \param opMode     [IN]  Mode of operation(polled/interrupt)
 *  \param callback   [IN]  Callback argument
 *  \param i2sHandle  [OUT] Pointer to the I2S instance handle
 *
 *  \return PSP_SOK - Successful, desired operation is done.
 *          PSP_E_INVAL_PARAM - I2S instance is INVALID.
 */
Int16 DDC_I2SOpen(I2S_INSTANCE           eI2SNum,
                  PSP_I2SChanType        chType,
                  PSP_I2SChanDir         i2sChanDir,
                  PSP_I2SOpMode          opMode,
                  PSP_I2SAppCallBack     callback,
                  PSP_Handle             *i2sHandle);
/**
 *  \brief Closes I2S instance
 *
 *  Function to close the specified I2S device instance.
 *  This API is called by driver code from DDA close function
 *
 *  \param i2sHandle  [IN]  The handle to the I2S instance
 *  \return PSP_SOK - Successful, desired operation is done.
 *          PSP_E_INVAL_PARAM - Handle pass is INVALID.
 */
Int16 DDC_I2SClose(PSP_Handle    i2sHandle);

/**
 *  \brief Configures I2S hardware
 *
 *  Function to setup the specified I2S device hardware registers.
 *  This API is called by driver code from DDA init function
 *
 *  \param i2sHandle     [IN]  The handle to the I2S instance
 *  \param i2sConfig     [IN]  Pointer to the i2s config structure.
 *
 *  \return PSP_SOK - Successful, desired operation is done.
 *          PSP_E_INVAL_PARAM - Handle pass is INVALID.
 */
Int16 DDC_I2SHwSetup(PSP_Handle       i2sHandle,
                     PSP_I2SConfig    *i2sConfig);

/**
 *  \brief DMA init function
 *
 *  Function to initiate the DMA for data transfer.
 *  This API is called by the driver code from DDA DMA init function
 *  to request and configure one DMA channel.
 *
 *  \param i2sHandle  [IN]  The handle to the I2S instance
 *  \param dmaConfig  [IN]  DMA configuration structure
 *
 *  \return   hDma  - DMA channel handle
 */
DMA_ChanHandle DDC_I2SDMAInit(PSP_Handle       i2sHandle,
                              PSP_DMAConfig    *dmaConfig);

/**
 *  \brief DMA deinit function
 *
 *  Function to deinitialise the DMA channel
 *
 *  \param hDma   [IN]  Handle to the DMA channel
 *
 *  \return PSP_SOK - if successful else suitable error code.
 */
Int16 DDC_I2SDMADeinit(DMA_ChanHandle    hDma);

/**
 *  \brief DMA data write
 *
 *  Function to write the data from DMA to I2S.
 *  This API is called by the driver code from DDA to transmit
 *  I2S data using DMA.
 *
 *  \param i2sHandle      [IN]  The handle to the I2S instance
 *  \param nextTxLeftBuf  [IN]  Pointer to next left transmit buffer
 *  \param nextTxRightBuf [IN]  Pointer to next right transmit buffer
 *  \param hDmaTxLeft     [IN]  Dma handle for left transmit channel
 *  \param hDmaTxRight    [IN]  Dma handle for right transmit channel
 *
 *  \return PSP_SOK - Successful, else suitable error code
 *
 */
Int16 DDC_I2SDMAWrite(PSP_Handle        i2sHandle,
                      Uint32            *nextTxLeftBuf,
                      Uint32            *nextTxRightBuf,
                      DMA_ChanHandle    hDmaTxLeft,
                      DMA_ChanHandle    hDmaTxRight);

/**
 *  \brief DMA data read.
 *
 *  Function to receive the data from I2S using DMA.
 *  This API is called by the driver code from DDA to receive
 *  I2S data using DMA.
 *
 *  \param i2sHandle      [IN]  Handle to the I2S instance
 *  \param nextRxLeftBuf  [IN]  Pointer to next left receive buffer
 *  \param nextRxRightBuf [IN]  Pointer to next right receive buffer
 *  \param hDmaRxLeft     [IN]  Dma handle for left receive channel
 *  \param hDmaRxRight    [IN]  Dma handle for right receive channel
 *
 *  \return PSP_SOK -  if successful, else suitable error code
 */
Int16 DDC_I2SDMARead(PSP_Handle        i2sHandle,
                     Uint32            *nextRxLeftBuf,
                     Uint32            *nextRxRightBuf,
                     DMA_ChanHandle    hDmaRxLeft,
                     DMA_ChanHandle    hDmaRxRight);

/**
 *   \brief Call back function for DMA transmit complete
 *
 *  This function will be called when DMA transmit completes.
 *  This function changes the source address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return   void
 *
 */
void I2S_DmaTxLChCallBack(
    PSP_DMATransferStatus dmaStatus,
    void *dataCallback
);

/**
 *   \brief Call back function for DMA transmit complete
 *
 *  This function will be called when DMA transmit completes.
 *  This function changes the source address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return   void
 *
 */
void I2S_DmaTxRChCallBack(
    PSP_DMATransferStatus dmaStatus,
    void *dataCallback
);

/**
 *   \brief Call back function for DMA left channel receive complete
 *
 *  This function will be called when DMA receive completes
 *  This function changes the destination address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return             void
 *
 */
void I2S_DmaRxLChCallBack(
    PSP_DMATransferStatus    dmaStatus,
    void    *dataCallback
);

/**
 *   \brief Call back function for DMA right channel receive complete
 *
 *  This function will be called when DMA receive completes
 *  This function changes the destination address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return             void
 *
 */
void I2S_DmaRxRChCallBack(
    PSP_DMATransferStatus    dmaStatus,
    void    *dataCallback
);

#ifdef USE_TWO_CODEC
/**
 *   \brief Call back function for DMA transmit complete
 *
 *  This function will be called when DMA transmit completes.
 *  This function changes the source address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return   void
 *
 */
void I2S_DmaTxLChCallBack2(
    PSP_DMATransferStatus dmaStatus,
    void *dataCallback
);

/**
 *   \brief Call back function for DMA transmit complete
 *
 *  This function will be called when DMA transmit completes.
 *  This function changes the source address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return   void
 *
 */
void I2S_DmaTxRChCallBack2(
    PSP_DMATransferStatus dmaStatus,
    void *dataCallback
);

/**
 *   \brief Call back function for DMA left channel receive complete
 *
 *  This function will be called when DMA receive completes
 *  This function changes the destination address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return             void
 *
 */
void I2S_DmaRxLChCallBack2(
    PSP_DMATransferStatus    dmaStatus,
    void    *dataCallback
);

/**
 *   \brief Call back function for DMA right channel receive complete
 *
 *  This function will be called when DMA receive completes
 *  This function changes the destination address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return             void
 *
 */
void I2S_DmaRxRChCallBack2(
    PSP_DMATransferStatus    dmaStatus,
    void    *dataCallback
);
#endif // USE_TWO_CODEC

#ifdef USE_THREE_CODEC
/**
 *   \brief Call back function for DMA transmit complete
 *
 *  This function will be called when DMA transmit completes.
 *  This function changes the source address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return   void
 *
 */
void I2S_DmaTxLChCallBack2(
    PSP_DMATransferStatus dmaStatus,
    void *dataCallback
);

/**
 *   \brief Call back function for DMA transmit complete
 *
 *  This function will be called when DMA transmit completes.
 *  This function changes the source address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return   void
 *
 */
void I2S_DmaTxRChCallBack2(
    PSP_DMATransferStatus dmaStatus,
    void *dataCallback
);

/**
 *   \brief Call back function for DMA left channel receive complete
 *
 *  This function will be called when DMA receive completes
 *  This function changes the destination address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return             void
 *
 */
void I2S_DmaRxLChCallBack2(
    PSP_DMATransferStatus    dmaStatus,
    void    *dataCallback
);

/**
 *   \brief Call back function for DMA right channel receive complete
 *
 *  This function will be called when DMA receive completes
 *  This function changes the destination address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return             void
 *
 */
void I2S_DmaRxRChCallBack2(
    PSP_DMATransferStatus    dmaStatus,
    void    *dataCallback
);

/**
 *   \brief Call back function for DMA transmit complete
 *
 *  This function will be called when DMA transmit completes.
 *  This function changes the source address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return   void
 *
 */
void I2S_DmaTxLChCallBack3(
    PSP_DMATransferStatus dmaStatus,
    void *dataCallback
);

/**
 *   \brief Call back function for DMA transmit complete
 *
 *  This function will be called when DMA transmit completes.
 *  This function changes the source address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return   void
 *
 */
void I2S_DmaTxRChCallBack3(
    PSP_DMATransferStatus dmaStatus,
    void *dataCallback
);

/**
 *   \brief Call back function for DMA left channel receive complete
 *
 *  This function will be called when DMA receive completes
 *  This function changes the destination address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return             void
 *
 */
void I2S_DmaRxLChCallBack3(
    PSP_DMATransferStatus    dmaStatus,
    void    *dataCallback
);

/**
 *   \brief Call back function for DMA right channel receive complete
 *
 *  This function will be called when DMA receive completes
 *  This function changes the destination address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return             void
 *
 */
void I2S_DmaRxRChCallBack3(
    PSP_DMATransferStatus    dmaStatus,
    void    *dataCallback
);
#endif // USE_THREE_CODEC

#ifdef USE_FOUR_CODEC
/**
 *   \brief Call back function for DMA transmit complete
 *
 *  This function will be called when DMA transmit completes.
 *  This function changes the source address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return   void
 *
 */
void I2S_DmaTxLChCallBack2(
    PSP_DMATransferStatus dmaStatus,
    void *dataCallback
);

/**
 *   \brief Call back function for DMA transmit complete
 *
 *  This function will be called when DMA transmit completes.
 *  This function changes the source address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return   void
 *
 */
void I2S_DmaTxRChCallBack2(
    PSP_DMATransferStatus dmaStatus,
    void *dataCallback
);

/**
 *   \brief Call back function for DMA left channel receive complete
 *
 *  This function will be called when DMA receive completes
 *  This function changes the destination address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return             void
 *
 */
void I2S_DmaRxLChCallBack2(
    PSP_DMATransferStatus    dmaStatus,
    void    *dataCallback
);

/**
 *   \brief Call back function for DMA right channel receive complete
 *
 *  This function will be called when DMA receive completes
 *  This function changes the destination address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return             void
 *
 */
void I2S_DmaRxRChCallBack2(
    PSP_DMATransferStatus    dmaStatus,
    void    *dataCallback
);

/**
 *   \brief Call back function for DMA transmit complete
 *
 *  This function will be called when DMA transmit completes.
 *  This function changes the source address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return   void
 *
 */
void I2S_DmaTxLChCallBack3(
    PSP_DMATransferStatus dmaStatus,
    void *dataCallback
);

/**
 *   \brief Call back function for DMA transmit complete
 *
 *  This function will be called when DMA transmit completes.
 *  This function changes the source address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return   void
 *
 */
void I2S_DmaTxRChCallBack3(
    PSP_DMATransferStatus dmaStatus,
    void *dataCallback
);

/**
 *   \brief Call back function for DMA left channel receive complete
 *
 *  This function will be called when DMA receive completes
 *  This function changes the destination address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return             void
 *
 */
void I2S_DmaRxLChCallBack3(
    PSP_DMATransferStatus    dmaStatus,
    void    *dataCallback
);

/**
 *   \brief Call back function for DMA right channel receive complete
 *
 *  This function will be called when DMA receive completes
 *  This function changes the destination address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return             void
 *
 */
void I2S_DmaRxRChCallBack3(
    PSP_DMATransferStatus    dmaStatus,
    void    *dataCallback
);

/**
 *   \brief Call back function for DMA transmit complete
 *
 *  This function will be called when DMA transmit completes.
 *  This function changes the source address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return   void
 *
 */
void I2S_DmaTxLChCallBack4(
    PSP_DMATransferStatus dmaStatus,
    void *dataCallback
);

/**
 *   \brief Call back function for DMA transmit complete
 *
 *  This function will be called when DMA transmit completes.
 *  This function changes the source address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return   void
 *
 */
void I2S_DmaTxRChCallBack4(
    PSP_DMATransferStatus dmaStatus,
    void *dataCallback
);

/**
 *   \brief Call back function for DMA left channel receive complete
 *
 *  This function will be called when DMA receive completes
 *  This function changes the destination address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return             void
 *
 */
void I2S_DmaRxLChCallBack4(
    PSP_DMATransferStatus    dmaStatus,
    void    *dataCallback
);

/**
 *   \brief Call back function for DMA right channel receive complete
 *
 *  This function will be called when DMA receive completes
 *  This function changes the destination address of the DMA channel and
 *  restarts the DMA transfer.
 *
 *   \param dmaStatus    [IN]    Status of the DMA transfer
 *   \param dataCallback [IN]    I2S handle
 *
 *   \return             void
 *
 */
void I2S_DmaRxRChCallBack4(
    PSP_DMATransferStatus    dmaStatus,
    void    *dataCallback
);
#endif // USE_FOUR_CODEC

#endif    /* _DDC_I2S_H_ */

