/*
 * $$$MODULE_NAME dda_i2c.h
 *
 * $$$MODULE_DESC dda_i2c.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/**
 *  \mainpage PSP I2C Driver
 *
 *  \section Introduction
 *
 *  \subsection xxx Purpose and Scope
 *  The purpose of this document is to explain the I2C driver on the PSP
 *  Framework architecture.
 *
 *  \subsection aaa I2C Usage and Working
 *  The multi-master I2C peripheral provides an interface between the
 *  DSP subsystem and I2C-bus compatible devices that connect via the
 *  I2C serial bus. External components attached to the I2C bus can serially
 *  transmit/receive up to 8-bit data to/from the DSP subsystem through the
 *  two-wire I2C interface. The I2C peripheral supports any slave or master
 *  I2C compatible device.
 *
 *  \subsection bbb Application Interfaces
 *  I2C driver provides three interfaces for data transfer between processor
 *  and I2C devices.I2C_Init(), I2C_Write(), I2C_Read().
 *
 *  \n I2C_Init() Initializes the I2C device. I2C own address and bus frequency
 *  are passed to this API. I2C operating frequency is derived from this given
 *  frequency value.
 *
 *  \n I2C_Write() Writes the data to I2C device. Address of the
 *  I2C slave device and data buffer are passed to this API.First two bytes of
 *  the data buffer contains the starting address of the slave device to
 *  write the data.
 *
 *  \n I2C_Read() reads data from I2C slave device.
 *  Address of the I2C slave device, data buffer and staring address of
 *  slave device to read the data are passed to this API.
 *
 *  \subsection ccc Driver Scope
 *  Driver supports only master mode of operation.
 *  It operates only in polled mode.
 *
 */

/**
 *  \file   dda_i2c.h
 *
 *  \brief  i2c Driver Adaptation for BIOS
 *
 *  This file contains the interfaces, data types and symbolic definitions
 *  that are needed by the application to utilize the serivces of the I2C
 *  device driver.
 *
 *  (C) Copyright 2005, Texas Instruments, Inc
 *
 *  \author     Pandith
 *
 *  \version    1.0 -    Initial Version
 */

#ifndef _DDA_I2C_H_
#define _DDA_I2C_H_

#include <psp_i2c.h>
#include <ddc_i2c.h>

/* DDA function prototypes */
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
PSP_Handle I2C_Init(Uint32 I2cOwnAddr, Uint32 I2cBusFreq);

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
                 Uint16     *buffer);

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
                Uint16     *buffer);

/**
 * \brief I2C sleep function
 *
 * Function called when the DDC needs to sleep for a specified mSecs
 *
 * \param   mSecs [IN]    Sleep time in milliseconds
 * \return  void
 */
void _DDA_i2cSleep(Uint32 mSecs);

/**
 * \brief Get OS ticks
 *
 * Function exposed by DDA, to DDC, to get the OS ticks
 *
 * \param   mSecs [IN]    Time in millisecs
 */
Int  _DDA_i2cOsGetTicks(Int mSecs);

/**
 * \brief Registers I2C interrupt handler
 *
 * Function used to register the interrupt handler
 *
 * \param   instNum [IN]    I2C instance number
 * \param   intNum  [IN]    Interrupt number
 * \return  void
 */
void _DDA_i2cRegisterIntrHandler(Uint32 instNum, Uint32 intNum);

/**
 * \brief Unregisters I2C interrupt handler
 *
 * Function used to un-register the interrupt handler
 *
 * \param   instNum [IN]    I2C instance number
 * \param   intNum  [IN]    Interrupt number
 * \return  void
 */
void _DDA_i2cUnregisterIntrHandler(Uint32 instNum, Uint32 intNum);

#endif  /*_DDA_I2C_H_ */
