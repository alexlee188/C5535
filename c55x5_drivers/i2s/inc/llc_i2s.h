/*
 * $$$MODULE_NAME llc_i2s.h
 *
 * $$$MODULE_DESC llc_i2s.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/**
 *  \file   llc_i2s.h
 *
 *  \brief  header file for I2S LLC implementation
 *
 *  This file contains prototypes,datatypes required for the low
 *  level controller implementation of I2S.
 *
 *  (C) Copyright 2006, Texas Instruments, Inc
 *
 *  \author     PR Mistral
 *  \version    1.0 initial implementation
 *  \version    1.1 Modified for review comments
 */

#ifndef _LLC_I2S_H_
#define _LLC_I2S_H_

#include <tistdtypes.h>
#include <corazon.h>
#include <cslr_i2s_001.h>

/* NOTE: This was not defined in corazon.h, Remove here once defined there  */
#define I2S_PER_CNT       (4) /**< Number of I2S peripherals on corazon */

/* Declaration of enum variables */
/**
 * \brief I2S instance number
 *
 */
typedef enum
{
     I2S_0,                  /**< I2S device instance number 0              */
     I2S_1,                  /**< I2S device instance number 1              */
     I2S_2,                  /**< I2S device instance number 2              */
     I2S_3,                  /**< I2S device instance number 3              */
     I2S_INVALID             /**< Invalid I2S device instance number        */
} I2S_INSTANCE;

/**
 * \brief  I2S data packing bit
 *
 */
typedef enum
{
    I2S_DATAPACK_DISABLE ,   /**< No data packing                           */
    I2S_DATAPACK_ENABLE      /**< Data is packed depending on word length   */
} I2S_DATAPACK_TYPE;

/**
 * \brief  I2S sign extension bit
 *
 */
typedef enum
{
    I2S_SIGNEXT_ENABLE ,     /**< Data is sign extended                     */
    I2S_SIGNEXT_DISABLE      /**< No sign extension for data                */
} I2S_SIGNEXT_TYPE;

/**
 * \brief  I2S transmission mode bit(MASTER/SLAVE)
 *
 */
typedef enum
{
    I2S_TXMODE_SLAVE ,       /**< Sets the I2S transmission mode to slave   */
    I2S_TXMODE_MASTER        /**< Sets the I2S transmission mode to master  */
} I2S_TXMODE_TYPE;

/**
 * \brief  I2S Transmission data type(Mono/Stereo)
 *
 */
typedef enum
{
    I2S_MONO_ENABLE ,        /**< Data to be transmitted is mono            */
    I2S_MONO_DISABLE         /**< Data to be transmitted is stereo          */
} I2S_MONO_TYPE;

/**
 * \brief  I2S data word length- Bits per word
 *
 */
typedef enum
{
    I2S_DATATX_WORDLEN_8,    /**< Number of bits in each word of data is 8  */
    I2S_DATATX_WORDLEN_10,   /**< Number of bits in each word of data is 10 */
    I2S_DATATX_WORDLEN_12,   /**< Number of bits in each word of data is 12 */
    I2S_DATATX_WORDLEN_14,   /**< Number of bits in each word of data is 14 */
    I2S_DATATX_WORDLEN_16,   /**< Number of bits in each word of data is 16 */
    I2S_DATATX_WORDLEN_18,   /**< Number of bits in each word of data is 18 */
    I2S_DATATX_WORDLEN_20,   /**< Number of bits in each word of data is 20 */
    I2S_DATATX_WORDLEN_24,   /**< Number of bits in each word of data is 24 */
    I2S_DATATX_WORDLEN_32    /**< Number of bits in each word of data is 32 */
} I2S_DATATX_WORDLEN_TYPE;

/**
 * \brief  Polarity of I2S clock signal
 *
 */
typedef enum
{
    I2S_TX_CLKPOL_FALLING,   /**< Tx on the falling edge of the clk signal  */
    I2S_TX_CLKPOL_RISING     /**< Tx on rising edge of the clk signal       */
} I2S_TX_CLKPOL_TYPE;

/**
 * \brief  Polarity of I2S frame sync signal
 *
 */
typedef enum
{
    I2S_FSPOL_LOW,           /**< Left chan is transmitted when fs is low   */
    I2S_FSPOL_HIGH           /**< Left chan is transmitted when fs is high  */
} I2S_FSPOL_TYPE;

/**
 * \brief  Data transmission delay
 *
 */
typedef enum
{
    I2S_DATADELAY_ONEBIT ,   /**< One bit delay in data tranmission         */
    I2S_DATADELAY_TWOBIT     /**< Two bits delay in data tranmission        */
} I2S_DATADELAY_TYPE;

/**
 * \brief I2S data transfer format
 *
 */
typedef enum
{
    I2S_DATAFORMAT_LJUST ,   /**< Data format is left justified             */
    I2S_DATAFORMAT_DSP       /**< Data format is DSP                        */
} I2S_DATAFORMAT_TYPE;

/**
 * \brief  I2S Serializer frame sync divider
 *
 */
typedef enum
{
    I2S_FSDIV4,              /**< FS Divide ratio is 4                      */
    I2S_FSDIV8,              /**< FS Divide ratio is 8                      */
    I2S_FSDIV16,             /**< FS Divide ratio is 16                     */
    I2S_FSDIV32,             /**< FS Divide ratio is 32                     */
    I2S_FSDIV64,             /**< FS Divide ratio is 64                     */
    I2S_FSDIV128,            /**< FS Divide ratio is 128                    */
    I2S_FSDIV_SLAVE          /**< FS is from external source for slave mode */
} I2S_FSDIV_TYPE;

/**
 * \brief  I2S Serializer clock divider.
 *
 */
typedef enum
{
    I2S_CLKDIV2,             /**< CLK Divide ratio is 2                     */
    I2S_CLKDIV4,             /**< CLK Divide ratio is 4                     */
    I2S_CLKDIV8,             /**< CLK Divide ratio is 8                     */
    I2S_CLKDIV16,            /**< CLK Divide ratio is 16                    */
    I2S_CLKDIV32,            /**< CLK Divide ratio is 32                    */
    I2S_CLKDIV64,            /**< CLK Divide ratio is 64                    */
    I2S_CLKDIV128,           /**< CLK Divide ratio is 128                   */
    I2S_CLKDIV256,           /**< CLK Divide ratio is 256                   */
    I2S_CLKDIV_SLAVE         /**< CLK is from external source in slave mode */
} I2S_CLKDIV_TYPE;

/**
 * \brief  Interrupt Register(IR) interrupt set
 *
 */
typedef enum
{
    I2S_IR_ORUR_SET,         /**< IR or/ur error flag bit is set to '1'     */
    I2S_IR_FERR_SET,         /**< IR Fsync error flag bit is set to '1'     */
    I2S_IR_RCV0_SET,         /**< IR rx interrupt0 flag bit is set to '1'   */
    I2S_IR_RCV1_SET,         /**< IR rx interrupt1 flag bit is set to '1'   */
    I2S_IR_XMIT0_SET,        /**< IR tx interrupt0 flag bit is set to '1'   */
    I2S_IR_XMIT1_SET         /**< IR tx interrupt1 flag bit is set to '1'   */
} I2S_IR_SETBIT_TYPE;

/**
 * \brief  Interrupt Register(IR) interrupt reset
 *
 */
typedef enum
{
    I2S_IR_ORUR_RESET,       /**< IR or/ur error flag bit is set to '0'     */
    I2S_IR_FERR_RESET,       /**< IR Fsync error flag bit is set to '0'     */
    I2S_IR_RCV0_RESET,       /**< IR rx interrupr0 flag bit is set to '0'   */
    I2S_IR_RCV1_RESET,       /**< IR rx interrupr1 flag bit is set to '0'   */
    I2S_IR_XMIT0_RESET,      /**< IR tx interrupt0 flag bit is set to '0'   */
    I2S_IR_XMIT1_RESET       /**< IR tx interrupr1 flag bit is set to '0'   */
} I2S_IR_RESETBIT_TYPE;

/**
 * \brief  Interrupt CPU Mask Register(ICMR) interrupt bit enable
 *
 */
typedef enum
{
    I2S_ICMR_ORUR_ENABLE,    /**< I2S overrun/underrun errors are enabled  */
    I2S_ICMR_FERR_ENABLE,    /**< I2S Framesync errors are enabled         */
    I2S_ICMR_RCV0_ENABLE,    /**< I2S receive0 interrupts are enabled      */
    I2S_ICMR_RCV1_ENABLE,    /**< I2S receive1 interrupts are enabled      */
    I2S_ICMR_XMIT0_ENABLE,   /**< I2S transmit0 interrupts are enabled     */
    I2S_ICMR_XMIT1_ENABLE    /**< I2S transmit1 interrupts are enabled     */
} I2S_ICMR_ENABLEBIT_TYPE;

/**
 * \brief  Interrupt CPU Mask Register(ICMR) interrupt bit disable
 *
 */
typedef enum
{
    I2S_ICMR_ORUR_DISABLE,   /**< I2S overrun/underrun errors are disabled */
    I2S_ICMR_FERR_DISABLE,   /**< I2S Framesync errors are disabled        */
    I2S_ICMR_RCV0_DISABLE,   /**< I2S receive0 interrupts are disabled     */
    I2S_ICMR_RCV1_DISABLE,   /**< I2S receive1 interrupts are disabled     */
    I2S_ICMR_XMIT0_DISABLE,  /**< I2S transmit0 interrupts are disabled    */
    I2S_ICMR_XMIT1_DISABLE   /**< I2S transmit1 interrupts are disabled    */
} I2S_ICMR_DISABLEBIT_TYPE;

/* Function prototypes */
/**
 * \brief   Enables the I2S serializer
 *
 * This function enables the I2S transmission or reception
 *
 * \param regs [IN]
 *
 * \returns    void
 */
void LLC_I2SEnable(ioport CSL_I2sDrvRegs    *regs);
/**
 * \brief    Disables the I2S serializer
 *
 * This function disables the I2S transmission or reception
 *
 * \param regs [IN]
 *
 * \returns    void
 */
void LLC_I2SDisable(ioport CSL_I2sDrvRegs    *regs);
/**
 * \brief   Sets the I2S data sign
 *
 * This function sets the sign extension of the I2S data
 *
 * \param regs     [IN]
 * \param eSignExt [IN]
 *
 * \returns        void
 */
void LLC_I2SSetSignExt(ioport CSL_I2sDrvRegs    *regs,
                       I2S_SIGNEXT_TYPE      eSignExt);
/**
 * \brief   Configures the I2S in loopback mode
 *
 * This function  enables the I2S digital loopback mode
 *
 * \param regs [IN]
 *
 * \returns    void
 */
void LLC_I2SEnableLoopBack(ioport CSL_I2sDrvRegs    *regs);
/**
 * \brief      Configures the I2S into normal mode
 *
 * This function  enables the I2S digital loopback mode
 *
 * \param regs [IN]
 *
 * \returns    void
 */
void LLC_I2SDisableLoopBack(ioport CSL_I2sDrvRegs    *regs);
/**
 * \brief  Enables or disables the data packing
 *
 * This sets or resets the data packing bit of I2S.
 *
 * \param regs      [IN]
 * \param eDataPack [IN]
 *
 * \return          void
 */
void LLC_I2SSetDataPacking(ioport CSL_I2sDrvRegs    *regs,
                           I2S_DATAPACK_TYPE     eDataPack);
/**
 * \brief  Configures the I2S transmission mode
 *
 * This function configures the I2S in master mode or slave mode
 *
 * \param regs    [IN]
 * \param eTxMode [IN]
 *
 * \returns       void
 */
void LLC_I2SSetTransmitMode(ioport CSL_I2sDrvRegs    *regs,
                            I2S_TXMODE_TYPE       eTxMode);
/**
 * \brief   Sets the I2S data type (Mono/Stereo)
 *
 * This functions sets the type of the I2S audio data i.e stereo ot mono
 *
 * \param regs      [IN]
 * \param eDataType [IN]
 *
 * \returns         void
 */
void LLC_I2SSetMono(ioport CSL_I2sDrvRegs    *regs,
                    I2S_MONO_TYPE         eMonoData);
/**
 * \brief  Sets the I2S transmission word length,
 *
 * This function sets I2s transmission word length.
 * I2S supports word lengths of 8,10,12,14,16,18,20,24 or 32 bits.
 *
 * \param regs        [IN]
 * \param eWordLength [IN]
 *
 * \returns           void
 */
void LLC_I2SSetWordLength(ioport CSL_I2sDrvRegs          *regs,
                          I2S_DATATX_WORDLEN_TYPE     eWordLength);
/**
 * \brief  Sets the I2S transmission clock polarity.
 *
 *  If ClockPol = 1,
 *  Receive data sampled on falling edge of CLKx pin
 *  Transmit data outputted on raising edge of CLKx pin.
 *  If ClockPol = 0,
 *  Receive data sampled on raising edge of CLKx pin
 *  Transmit data outputted on falling edge of CLKx pin
 *
 * \param regs    [IN]
 * \param eClkPol [IN]
 *
 * \returns    void
 */
void LLC_I2SSetClkPol(ioport CSL_I2sDrvRegs    *regs,
                      I2S_TX_CLKPOL_TYPE    eClkPol);
/**
 * \brief      Sets the I2S frame sync polarity.
 *
 * This function sets the frame sync polarity to low or high.
 *
 * \param regs   [IN]
 * \param eFsPol [IN]
 *
 * \returns      void
 */
void LLC_I2SSetFsPol(ioport CSL_I2sDrvRegs    *regs,
                     I2S_FSPOL_TYPE        eFsPol);
/**
 * \brief  Sets the I2S Data transmission delay,
 *
 * This function sets the I2S data delay to one bit or two bits.
 *
 * \param regs       [IN]
 * \param eDataDelay [IN]
 *
 * \returns          void
 */
void LLC_I2SSetDataDelay(ioport CSL_I2sDrvRegs     *regs,
                         I2S_DATADELAY_TYPE     eDataDelay);
/**
 * \brief      Sets the I2S data format.
 *
 * This function sets the I2S data format to left-justified or dsp format.
 *
 * \param regs        [IN]
 * \param eDataFormat [IN]
 *
 * \returns           void
 */
void LLC_I2SSetDataFormat(ioport CSL_I2sDrvRegs     *regs,
                          I2S_DATAFORMAT_TYPE    eDataFormat);
/**
 * \brief  Sets the serializer frame sync divider value.
 *
 * This value is used as the divide-down number of the internally
 * generated frame sync to the serializer.
 *
 * \param regs   [IN]
 * \param eFsdiv [IN]
 *
 * \returns          void
 */
void LLC_I2SSetFsdiv(ioport CSL_I2sDrvRegs    *regs,
                     I2S_FSDIV_TYPE        eFsdiv);
/**
 * \brief      Sets the serializer clock divider value.
 *
 * This value is used as the divide-down number of the internally
 * generated clock to the serializer.
 *
 * \param regs    [IN]
 * \param eClkdiv [IN]
 *
 * \returns       void
 */
void LLC_I2SSetClkdiv(ioport CSL_I2sDrvRegs    *regs,
                      I2S_CLKDIV_TYPE       eClkdiv);
/**
 * \brief   Resets interrupt bits of interrupt register
 *
 * This function resets all the I2S interrupts that are flagged in IR.
 *
 * \param regs [IN]
 *
 * \returns    void
 */
void LLC_I2SResetIR(ioport CSL_I2sDrvRegs    *regs);
/**
 * \brief   Enables interrupts
 *
 * This function enables I2S interrupts by setting interrupt mask register bits
 *
 * \param regs           [IN]
 * \param eICMREnableBit [IN]
 *
 * \returns              void
 */
void LLC_I2SEnableICMR(ioport CSL_I2sDrvRegs         *regs,
                       I2S_ICMR_ENABLEBIT_TYPE    eICMREnableBit);
/**
 * \brief      Disables interrupts
 *
 * This function disables I2S interrupts by resetting IMR bits.
 *
 * \param regs            [IN]
 * \param eICMRDisableBit [IN]
 *
 * \returns               void
 */
void LLC_I2SDisableICMR(ioport CSL_I2sDrvRegs          *regs,
                        I2S_ICMR_DISABLEBIT_TYPE    eICMRDisableBit);
/**
 * \brief    Reset all I2S regesters
 *
 * This function resets all the registers of I2S serializer
 *
 * \param regs [IN]
 *
 * \returns    void
 */
void LLC_ResetAll(ioport CSL_I2sDrvRegs    *regs);

#endif /*_LLC_I2S_H_ */
