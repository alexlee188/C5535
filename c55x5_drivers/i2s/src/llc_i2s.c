/*
 * $$$MODULE_NAME llc_i2s.c
 *
 * $$$MODULE_DESC llc_i2s.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/**
 * \file   llc_i2s.c
 *
 * \brief  I2S LLC definitions.
 *
 * This file contains the I2S LLC definitions, that are used by the DDC.
 *
 * (C) Copyright 2006, Texas Instruments, Inc
 *
 * \author        PR Mistral
 * \version    0.1
 */

#include "llc_i2s.h"

//NOTE: All the LLC APIs are called from the DDC functions by the driver code

/**
 * \brief   Enables the I2S serializer
 *
 * This function enables the I2S transmission or reception
 *
 * \param regs [IN]
 *
 * \returns    void
 */
void LLC_I2SEnable(ioport CSL_I2sDrvRegs    *regs)
{
    regs->SCRL |= (Uint16)(CSL_I2S_SCRL_ENABLE_MASK);
}

/**
 * \brief    Disables the I2S serializer
 *
 * This function disables the I2S transmission or reception
 *
 * \param regs [IN]
 *
 * \returns    void
 */
void LLC_I2SDisable(ioport CSL_I2sDrvRegs    *regs)
{
    regs->SCRL &= (Uint16)(~CSL_I2S_SCRL_ENABLE_MASK);
}

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
                       I2S_SIGNEXT_TYPE      eSignExt)
{
    switch(eSignExt)
    {
        case I2S_SIGNEXT_ENABLE:     /* Data is sign extended      */
            regs->SCRL |= (Uint16)(CSL_I2S_SCRL_SIGNEXT_MASK);
            break;
        case I2S_SIGNEXT_DISABLE:    /* No sign extension for data */
            regs->SCRL &= (Uint16)(~CSL_I2S_SCRL_SIGNEXT_MASK);
            break;
        /* By default sign extension is disabled */
        default:
            regs->SCRL &= (Uint16)(~CSL_I2S_SCRL_SIGNEXT_MASK);
            break;
    }
}

/**
 * \brief   Configures the I2S in loopback mode
 *
 * This function  enables the I2S digital loopback mode
 *
 * \param regs [IN]
 *
 * \returns    void
 */
void LLC_I2SEnableLoopBack(ioport CSL_I2sDrvRegs    *regs)
{
    regs->SCRL |= (Uint16)(CSL_I2S_SCRL_LOOPBACK_MASK);
}

/**
 * \brief      Configures the I2S into normal mode
 *
 * This function  enables the I2S digital loopback mode
 *
 * \param regs [IN]
 *
 * \returns    void
 */
void LLC_I2SDisableLoopBack(ioport CSL_I2sDrvRegs    *regs)
{
    regs->SCRL &= (Uint16)(~CSL_I2S_SCRL_LOOPBACK_MASK);
}

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
                           I2S_DATAPACK_TYPE     eDataPack)
{
    switch(eDataPack)
    {
        case I2S_DATAPACK_ENABLE:   /* Data is packed  */
            regs->SCRL |= (Uint16)(CSL_I2S_SCRL_PACK_MASK);
            break;
        case I2S_DATAPACK_DISABLE:  /* No data packing */
            regs->SCRL &= (Uint16)(~CSL_I2S_SCRL_PACK_MASK);
            break;
        /* By default data packing is enabled */
        default:
            regs->SCRL |= (Uint16)(CSL_I2S_SCRL_PACK_MASK);
            break;
    }
}

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
                            I2S_TXMODE_TYPE       eTxMode)
{
    switch(eTxMode)
    {
        case I2S_TXMODE_SLAVE:  /* Sets the I2S transmission mode to slave  */
            regs->SCRL &= (Uint16)(~CSL_I2S_SCRL_MODE_MASK);
            break;
        case I2S_TXMODE_MASTER: /* Sets the I2S transmission mode to master */
            regs->SCRL |= (Uint16)(CSL_I2S_SCRL_MODE_MASK);
            break;
        /* By default mode can be set to Master mode */
        default:
            regs->SCRL |= (Uint16)(CSL_I2S_SCRL_MODE_MASK);
            break;
    }
}

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
                    I2S_MONO_TYPE         eDataType)
{
    switch(eDataType)
    {
        case I2S_MONO_ENABLE:  /* Data to be transmitted is of type mono   */
            regs->SCRL |= (Uint16)(CSL_I2S_SCRL_MONO_MASK);
            break;
        case I2S_MONO_DISABLE: /* Data to be transmitted is of type stereo */
            regs->SCRL &= (Uint16)(~CSL_I2S_SCRL_MONO_MASK);
            break;
        /* By we set the data type to stereo */
        default    :
            regs->SCRL &= (Uint16)(~CSL_I2S_SCRL_MONO_MASK);
            break;
    }
}

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
void LLC_I2SSetWordLength(ioport CSL_I2sDrvRegs         *regs,
                          I2S_DATATX_WORDLEN_TYPE    eWordLength)
{
    regs->SCRL &= (Uint16)(~CSL_I2S_SCRL_WORDLENGTH_MASK);
    switch(eWordLength)
    {
        case I2S_DATATX_WORDLEN_8:    /* 8 bits in each word   */
            regs->SCRL |= (Uint16)(0x0000 << CSL_I2S_SCRL_WORDLENGTH_SHIFT);
            break;
        case I2S_DATATX_WORDLEN_10:   /* 10 bits in each word  */
            regs->SCRL |= (Uint16)(0x0001 << CSL_I2S_SCRL_WORDLENGTH_SHIFT);
            break;
        case I2S_DATATX_WORDLEN_12:   /* 12 bits in each word  */
            regs->SCRL |= (Uint16)(0x0002 << CSL_I2S_SCRL_WORDLENGTH_SHIFT);
            break;
        case I2S_DATATX_WORDLEN_14:   /* 14 bits in each word  */
            regs->SCRL |= (Uint16)(0x0003 << CSL_I2S_SCRL_WORDLENGTH_SHIFT);
            break;
        case I2S_DATATX_WORDLEN_16:   /* 16 bits in each word  */
            regs->SCRL |= (Uint16)(0x0004 << CSL_I2S_SCRL_WORDLENGTH_SHIFT);
            break;
        case I2S_DATATX_WORDLEN_18:   /* 18 bits in each word  */
            regs->SCRL |= (Uint16)(0x0005 << CSL_I2S_SCRL_WORDLENGTH_SHIFT);
            break;
        case I2S_DATATX_WORDLEN_20:   /* 20 bits in each word  */
            regs->SCRL |= (Uint16)(0x0006 << CSL_I2S_SCRL_WORDLENGTH_SHIFT);
            break;
        case I2S_DATATX_WORDLEN_24:   /* 24 bits in each word  */
            regs->SCRL |= (Uint16)(0x0007 << CSL_I2S_SCRL_WORDLENGTH_SHIFT);
            break;
        case I2S_DATATX_WORDLEN_32:   /* 32 bits in each word  */
            regs->SCRL |= (Uint16)(0x0008 << CSL_I2S_SCRL_WORDLENGTH_SHIFT);
            break;
         //Default word length is 16 bits
        default:
            regs->SCRL |= (Uint16)(0x0004 << CSL_I2S_SCRL_WORDLENGTH_SHIFT);
            break;
    }
}

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
void LLC_I2SSetClkPol(ioport CSL_I2sDrvRegs     *regs,
                      I2S_TX_CLKPOL_TYPE     eClkPol)
{
    switch(eClkPol)
    {
        /* Data is transmitted on the falling edge of the clock signal */
        case I2S_TX_CLKPOL_FALLING:
            regs->SCRL &= (Uint16)(~CSL_I2S_SCRL_CLOCKPOL_MASK);
            break;
        /* Data is transmitted on the rising edge of the clock signal  */
        case I2S_TX_CLKPOL_RISING:
            regs->SCRL |= (Uint16)(CSL_I2S_SCRL_CLOCKPOL_MASK);
            break;
        /* By default clock polarity is set to rising edge */
        default:
            regs->SCRL |= (Uint16)(CSL_I2S_SCRL_CLOCKPOL_MASK);
            break;
    }

}

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
                     I2S_FSPOL_TYPE        eFsPol)
{
    switch(eFsPol)
    {
        case I2S_FSPOL_LOW:  /* Left channel is transmitted when fs is low */
            regs->SCRL &= (Uint16)(~CSL_I2S_SCRL_FSPOL_MASK);
            break;
        case I2S_FSPOL_HIGH: /* Left channel is transmitted when fs is high */
            regs->SCRL |= (Uint16)(CSL_I2S_SCRL_FSPOL_MASK);
            break;
        /* By default  fs polarity is set to low */
        default:
            regs->SCRL &= (Uint16)(~CSL_I2S_SCRL_FSPOL_MASK);
            break;
    }

}

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
void LLC_I2SSetDataDelay(ioport CSL_I2sDrvRegs    *regs,
                         I2S_DATADELAY_TYPE    eDataDelay)
{
    switch(eDataDelay)
    {
        case I2S_DATADELAY_ONEBIT:    /* One bit delay in data tranmission  */
            regs->SCRL &= (Uint16)(~CSL_I2S_SCRL_DATADELAY_MASK);
            break;
         case I2S_DATADELAY_TWOBIT:   /* Two bits delay in data tranmission */
             regs->SCRL |= (Uint16)(CSL_I2S_SCRL_DATADELAY_MASK);
             break;
         /* By default data delay is set to one bit */
         default:
             regs->SCRL &= (Uint16)(~CSL_I2S_SCRL_DATADELAY_MASK);
             break;
     }
}

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
void LLC_I2SSetDataFormat(ioport CSL_I2sDrvRegs      *regs,
                          I2S_DATAFORMAT_TYPE     eDataFormat)
{
    switch(eDataFormat)
    {
        case I2S_DATAFORMAT_LJUST:      /* Data format is left-justified */
            regs->SCRL &= (Uint16)(~CSL_I2S_SCRL_FORMAT_MASK);
            break;
        case I2S_DATAFORMAT_DSP :       /* Data format is DSP            */
            regs->SCRL |= (Uint16)(CSL_I2S_SCRL_FORMAT_MASK);
            break;
        /* By default data format is set to left-justified */
        default :
            regs->SCRL &= (Uint16)(~CSL_I2S_SCRL_FORMAT_MASK);
            break;
    }
}

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
                     I2S_FSDIV_TYPE        eFsdiv)
{
    /* Reset the value of fsdiv */
    regs->SRGRL  &= (Uint16)(~CSL_I2S_SRGRL_FSDIV_MASK);
    switch(eFsdiv)
    {
        case I2S_FSDIV4:             /*   FS Divide ratio is 4  */
            regs->SRGRL |= (Uint16)(0x0000 << CSL_I2S_SRGRL_FSDIV_SHIFT);
            break;
        case I2S_FSDIV8:             /*   FS Divide ratio is 8  */
            regs->SRGRL |= (Uint16)(0x0001 << CSL_I2S_SRGRL_FSDIV_SHIFT);
            break;
        case I2S_FSDIV16:            /*   FS Divide ratio is 16 */
            regs->SRGRL |= (Uint16)(0x0002 << CSL_I2S_SRGRL_FSDIV_SHIFT);
            break;
        case I2S_FSDIV32:            /*   FS Divide ratio is 32 */
            regs->SRGRL |= (Uint16)(0x0003 << CSL_I2S_SRGRL_FSDIV_SHIFT);
            break;
        case I2S_FSDIV64:            /*   FS Divide ratio is 64 */
            regs->SRGRL |= (Uint16)(0x0004 << CSL_I2S_SRGRL_FSDIV_SHIFT);
            break;
        case I2S_FSDIV128:           /*  FS Divide ratio is 128 */
            regs->SRGRL |= (Uint16)(0x0005 << CSL_I2S_SRGRL_FSDIV_SHIFT);
            break;
        case I2S_FSDIV_SLAVE:
            regs->SRGRL &= (Uint16)(CSL_I2S_SRGRL_FSDIV_RESETVAL);
            break;
        default:
            break;
    }
}

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
                      I2S_CLKDIV_TYPE       eClkdiv)
{
    /* Reset the value of clkdiv */
    regs->SRGRL &= (Uint16)(~CSL_I2S_SRGRL_CLOCKDIV_MASK);

    switch(eClkdiv)
    {
        case I2S_CLKDIV2:        /*    CLK Divide ratio is 2.   */
            regs->SRGRL |= (Uint16)(0x0000 << CSL_I2S_SRGRL_CLOCKDIV_SHIFT );
            break;
        case I2S_CLKDIV4:        /*    CLK Divide ratio is 4.   */
            regs->SRGRL |= (Uint16)(0x0001 << CSL_I2S_SRGRL_CLOCKDIV_SHIFT );
            break;
        case I2S_CLKDIV8:        /*    CLK Divide ratio is 8.   */
            regs->SRGRL |= (Uint16)(0x0002 << CSL_I2S_SRGRL_CLOCKDIV_SHIFT );
            break;
        case I2S_CLKDIV16:       /*    CLK Divide ratio is 16.  */
            regs->SRGRL |= (Uint16)(0x0003 << CSL_I2S_SRGRL_CLOCKDIV_SHIFT );
            break;
        case I2S_CLKDIV32:       /*    CLK Divide ratio is 32.  */
            regs->SRGRL |= (Uint16)(0x0004 << CSL_I2S_SRGRL_CLOCKDIV_SHIFT );
            break;
        case I2S_CLKDIV64:       /*    CLK Divide ratio is 64.  */
            regs->SRGRL |= (Uint16)(0x0005 << CSL_I2S_SRGRL_CLOCKDIV_SHIFT );
            break;
        case I2S_CLKDIV128:      /*    CLK Divide ratio is 128. */
            regs->SRGRL |= (Uint16)(0x0006 << CSL_I2S_SRGRL_CLOCKDIV_SHIFT );
            break;
        case I2S_CLKDIV256:      /*    CLK Divide ratio is 256. */
            regs->SRGRL |= (Uint16)(0x0007 << CSL_I2S_SRGRL_CLOCKDIV_SHIFT );
            break;
        case I2S_CLKDIV_SLAVE:
            regs->SRGRL &= (Uint16)(~CSL_I2S_SRGRL_RESETVAL);
            break;
        default:
            break;
    }
}

/**
 * \brief   Resets interrupt bits of interrupt register
 *
 * This function resets all the I2S interrupts that are flagged in IR.
 *
 * \param regs [IN]
 *
 * \returns    void
 */
void LLC_I2SResetIR(ioport CSL_I2sDrvRegs    *regs)
{
    regs->IRL &= (Uint16)(CSL_I2S_IRL_RESETVAL);
}

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
void LLC_I2SEnableICMR(ioport CSL_I2sDrvRegs    *regs,
                       I2S_ICMR_ENABLEBIT_TYPE    eICMREnableBit)
{
    switch(eICMREnableBit)
    {
        case I2S_ICMR_ORUR_ENABLE: /* I2S or/ur errors are enabled         */
            regs->ICMRL |= (Uint16)(CSL_I2S_ICMRL_O_U_MASK);
            break;
        case I2S_ICMR_FERR_ENABLE: /* I2S Framesync errors are enabled     */
            regs->ICMRL |= (Uint16)(CSL_I2S_ICMRL_FERR_MASK );
            break;
        case I2S_ICMR_RCV0_ENABLE: /* I2S receive0 interrupts are enabled  */
            regs->ICMRL |= (Uint16)(CSL_I2S_ICMRL_RCV0_MASK);
            break;
        case I2S_ICMR_RCV1_ENABLE: /* I2S receive1 interrupts are enabled  */
            regs->ICMRL |= (Uint16)(CSL_I2S_ICMRL_RCV1_MASK);
            break;
        case I2S_ICMR_XMIT0_ENABLE:/* I2S transmit0 interrupts are enabled */
            regs->ICMRL |= (Uint16)(CSL_I2S_ICMRL_XMIT0_MASK);
            break;
        case I2S_ICMR_XMIT1_ENABLE:/* I2S transmit1 interrupts are enabled */
            regs->ICMRL |= (Uint16)(CSL_I2S_ICMRL_XMIT1_MASK);
            break;
        default:
            break;
    }
}

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
                        I2S_ICMR_DISABLEBIT_TYPE    eICMRDisableBit)
{
    switch(eICMRDisableBit)
    {
        case I2S_ICMR_ORUR_DISABLE: /* I2S or/ur errors are disabled         */
            regs->ICMRL &= (Uint16)(~CSL_I2S_ICMRL_O_U_MASK);
            break;
        case I2S_ICMR_FERR_DISABLE: /* I2S Framesync errors are disabled     */
            regs->ICMRL &= (Uint16)(~CSL_I2S_ICMRL_FERR_MASK );
            break;
        case I2S_ICMR_RCV0_DISABLE: /* I2S receive0 interrupts are disabled  */
            regs->ICMRL &= (Uint16)(~CSL_I2S_ICMRL_RCV0_MASK);
            break;
        case I2S_ICMR_RCV1_DISABLE: /* I2S receive1 interrupts are disabled  */
            regs->ICMRL &= (Uint16)(~CSL_I2S_ICMRL_RCV1_MASK);
            break;
        case I2S_ICMR_XMIT0_DISABLE:/* I2S transmit0 interrupts are disabled */
            regs->ICMRL &= (Uint16)(~CSL_I2S_ICMRL_XMIT0_MASK);
            break;
        case I2S_ICMR_XMIT1_DISABLE:/* I2S transmit1 interrupts are disabled */
            regs->ICMRL &= (Uint16)(~CSL_I2S_ICMRL_XMIT1_MASK);
            break;
        default:
            break;
    }
}

/**
 * \brief    Reset all I2S regesters
 *
 * This function resets all the registers of I2S serializer
 *
 * \param regs [IN]
 *
 * \returns    void
 */
void LLC_ResetAll(ioport CSL_I2sDrvRegs  *regs)
{
    regs->SCRL     &=  (Uint16)(CSL_I2S_SCRL_RESETVAL);
    regs->SCRM     &=  (Uint16)(CSL_I2S_SCRM_RESETVAL);
    regs->SRGRL    &=  (Uint16)(CSL_I2S_SRGRL_RESETVAL);
    regs->SRGRM    &=  (Uint16)(CSL_I2S_SRGRM_RESETVAL);
    regs->IRL      &=  (Uint16)(CSL_I2S_IRL_RESETVAL);
    regs->IRM      &=  (Uint16)(CSL_I2S_IRM_RESETVAL);
    regs->ICMRL    &=  (Uint16)(CSL_I2S_ICMRL_RESETVAL);
    regs->ICMRM    &=  (Uint16)(CSL_I2S_ICMRM_RESETVAL);
    regs->TRW0L    &=  (Uint16)(CSL_I2S_TRW0L_RESETVAL);
    regs->TRW0M    &=  (Uint16)(CSL_I2S_TRW0M_RESETVAL);
    regs->TRW1L    &=  (Uint16)(CSL_I2S_TRW1L_RESETVAL);
    regs->TRW1M    &=  (Uint16)(CSL_I2S_TRW1M_RESETVAL);
    regs->RRW0L    &=  (Uint16)(CSL_I2S_RRW0L_RESETVAL);
    regs->RRW0M    &=  (Uint16)(CSL_I2S_RRW0M_RESETVAL);
    regs->RRW1L    &=  (Uint16)(CSL_I2S_RRW1L_RESETVAL);
    regs->RRW1M    &=  (Uint16)(CSL_I2S_RRW1M_RESETVAL);
}
