/*
 * $$$MODULE_NAME dda_i2c.c
 *
 * $$$MODULE_DESC dda_i2c.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/**
 *  \file   dda_i2c.c
 *
 *  \brief  i2c Driver Adaptation for BIOS
 *
 *  This file contains the DDA implementation of the I2C driver to utilize the
 *  serivces of the I2C device driver.
 *
 *  (C) Copyright 2005, Texas Instruments, Inc
 *
 *  \author     Pandith
 *
 *  \version    1.0 -    Initial Version
 *  \version    1.1 -    PR Mistral - Modified for ROM-ability
 */

#include <dda_i2c.h>

/**
 * \brief Function to Initialize the I2C Peripheral
 *
 * This function creates an I2C object, opens and initializes an I2C
 * hardware instance.
 *
 * \param    I2cOwnAddr [IN]    I2C own address
 * \param    I2cBusFreq [IN]    I2C operating frequency
 *
 * \return   handle to I2C instance if the open is successful
 *           else NULL
 */
PSP_Handle I2C_Init(Uint32 I2cOwnAddr, Uint32 I2cBusFreq)
{
    Int             status;
    PSP_I2cConfig   InitConfig;
    PSP_Handle      i2c_handle;
    Uint32          InstanceId;

    /* Initialise DDC
     * The following function will create an instance of i2c
     */
     InstanceId  = 0;
     i2c_handle  = NULL;

     InitConfig.mode = PSP_OPMODE_POLLED;
     InitConfig.i2cOwnAddr =  I2cOwnAddr;
     InitConfig.numBits = 8u;
     InitConfig.i2cBusFreq = I2cBusFreq;
     InitConfig.addressing = FALSE;
     InitConfig.repeatMode = FALSE;
     InitConfig.dlb = FALSE;

    status = PSP_i2cCreate(InstanceId, ( const PSP_I2cConfig *)&InitConfig);
    if(status != PAL_SOK)
    {
        status = PSP_E_DRIVER_INIT;
    }
    else
    {
        /* Initialise DDC
         * The following function prepares the driver instance for data transfers
         */
        i2c_handle = PSP_i2cOpen(InstanceId, NULL, NULL);
        if(i2c_handle == NULL)
        {
            i2c_handle = NULL;
        }
    }
    return i2c_handle;
}

/**
 * \brief Function to perform write using I2C peripheral.
 *
 * This function writes the data of length given by Count to I2C slave device
 * from the given data buffer.
 *
 * \param  i2c_handle   [IN]    I2C instance handle
 * \param  I2cSlaveAddr [IN]    Address of the slave device
 * \param  Count        [IN]    Length of the data to write
 * \param  buffer       [IN]    Address of the data buffer
 */
Uint16 I2C_Write(PSP_Handle i2c_handle,
                 Uint16     I2cSlaveAddr,
                 Uint16     Count,
                 Uint16     *buffer)
{
    PSP_I2cDataParam dataparam;
    PSP_I2cDataParam *pdataparam;
    Int status;

    pdataparam = &dataparam;

    /* start transmission */
    pdataparam->i2cTrans.slaveAddr = I2cSlaveAddr;
    pdataparam->i2cTrans.buffer = buffer;
    pdataparam->i2cTrans.bufLen = Count;
    pdataparam->i2cTrans.flags = PSP_I2C_DEFAULT_WRITE;
    pdataparam->timeout = -1;
    status = PSP_i2cTransfer(i2c_handle,
                            pdataparam->i2cTrans.slaveAddr,
                            pdataparam->i2cTrans.buffer,
                            pdataparam->i2cTrans.bufLen,
                            pdataparam->i2cTrans.flags,
                            pdataparam->timeout,
                            NULL);
    return status;

}

/**
 * \brief Function to read data using I2C peripheral.
 *
 * This function reads the data of length given by Count from I2C slave device
 * to the given data buffer.
 *
 * \param  i2c_handle   [IN]    I2C instance handle
 * \param  I2cSlaveAddr [IN]    Address of the slave device
 * \param  Count        [IN]    Length of the data to read
 * \param  Addr         [IN]    Memory start address
 * \param  buffer       [IN]    Address of the data buffer
 */
Uint16 I2C_Read(PSP_Handle i2c_handle,
                Uint16     I2cSlaveAddr,
                Uint16     Count,
                Uint16     Addr,
                Uint16     *buffer)
{
    PSP_I2cDataParam dataparam, *pdataparam;
    Int status, delay;
    Uint16 ByteAddr[2];

    pdataparam = &dataparam;

    /* 16-bit address */
    //ByteAddr[0] = ( ( Addr & 0xFF00 ) >> 8); /*  Byte Addr. (A15 - A8)
    //ByteAddr[1] = ( Addr & 0x00FF );         /*  Byte Addr. (A7 - A0)
    /* 8-bit address */
    ByteAddr[0] = ( Addr & 0x00FF );         /*  Byte Addr. (A7 - A0) */

    /* Dummy write slave address and byte addresses
     * of the location it wishes to read.
     */
    pdataparam->i2cTrans.slaveAddr = I2cSlaveAddr;
    pdataparam->i2cTrans.buffer = ByteAddr;
    ///pdataparam->i2cTrans.bufLen = 2; /* 16-bit address */
    pdataparam->i2cTrans.bufLen = 1; /* 8-bit address */
    pdataparam->i2cTrans.flags = PSP_I2C_DEFAULT_WRITE ;
    pdataparam->timeout = -1;
    status = PSP_i2cTransfer(i2c_handle,
                            pdataparam->i2cTrans.slaveAddr,
                            pdataparam->i2cTrans.buffer,
                            pdataparam->i2cTrans.bufLen,
                            pdataparam->i2cTrans.flags,
                            pdataparam->timeout,
                            NULL);
    if(status != PAL_SOK)
    {
        status = PAL_SINPROGRESS;
    }
    else
    {
        /* After Dummy write, Read N bytes from EEPROM */
        pdataparam->i2cTrans.slaveAddr = I2cSlaveAddr;
        pdataparam->i2cTrans.buffer = buffer;
        pdataparam->i2cTrans.bufLen = Count;
        pdataparam->i2cTrans.flags = PSP_I2C_DEFAULT_READ;
        pdataparam->timeout = -1;

        for(delay = 0; delay < 10000; delay++);

        status = PSP_i2cTransfer(i2c_handle,
                                pdataparam->i2cTrans.slaveAddr,
                                pdataparam->i2cTrans.buffer,
                                pdataparam->i2cTrans.bufLen,
                                pdataparam->i2cTrans.flags,
                                pdataparam->timeout,
                                NULL);
    }

    return status;
}

/**
 * \brief I2C sleep function
 *
 * Function called when the DDC needs to sleep for a specified mSecs
 *
 * \param   mSecs [IN]    Sleep time in milliseconds
 * \return  void
 */
void _DDA_i2cSleep(Uint32 mSecs)
{
    PAL_osWaitMsecs(mSecs);
}

/**
 * \brief Get OS ticks
 *
 * Function exposed by DDA, to DDC, to get the OS ticks
 *
 * \param   mSecs [IN]    Time in millisecs
 */
Int  _DDA_i2cOsGetTicks(Int mSecs)
{
    Int retVal = -1;
    if ( -1 != mSecs ) {
        retVal = mSecs;
        if (retVal == -1) {
            retVal = -1;
        }
    }
    return retVal;
}

/* End of file */
