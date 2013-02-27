/*
 * $$$MODULE_NAME ddc_i2c.h
 *
 * $$$MODULE_DESC ddc_i2c.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/**
 *  \file   ddc_i2c.h
 *
 *  \brief  I2C Core header file
 *
 *  This file contains the interfaces, data types and symbolic definitions
 *  that are needed for the I2C device driver core.
 *
 *  (C) Copyright 2005, Texas Instruments, Inc
 *
 *  \author     PSG
 *
 *  \version    1.x - Anant Gole        - previous versions of interfaces
 *              2.0 - Nagarjuna Kristam - cleanup effort and MISRAC compliance
 *              3.0 - Nagarjuna Kristam - New implementation
 */

#ifndef _DDC_I2C_H_
#define _DDC_I2C_H_

#include <corazon.h>
#include <tistdtypes.h>
#include <pal_osSem.h>
#include <pal_osProtect.h>
#include <pal_ostime.h>
#include <pal_oswait.h>
#include <std.h>
#include <sem.h>
//#include <stdlib.h>
#include <psp_i2c.h>

#define PSP_I2C_NUM_INSTANCES           (CSL_I2C_PER_CNT)
/**< Max I2C driver instances                                                */
#define PSP_I2C_NUM_OPENS               (5u)/* Can be increased if needed    */
/**< Max I2C driver opens allowed. */
#define PSP_I2C_MIN_BUS_FREQ            (1000u)      /* 1 KHz                */
/**< Min output i2c bus frequencies */
#define PSP_I2C_MAX_BUS_FREQ            (400000u)    /* 400 KHz              */
/**< Max output i2c bus frequencies */
#define PSP_I2C_BUS_BUSY_TIMEOUT_CNT    (10000000u)  /* Depends on CPU speed */
/**< Bus Busy Timeout - if polled mode, this shall be a counter              */
#define PSP_I2C_BUS_BUSY_TIMEOUT_MSECS  (10u)        /* 10 mSec              */
/**< Bus Busy Timeout if interrupt/dma this will be in mSecs                 */
#define DDC_I2C_MIN_NUM_BITS            (2u)
/**< Min Number of Bits                                                      */
#define DDC_I2C_MAX_NUM_BITS            (8u)
/**< Max Number of Bits                                                      */
#define DDC_I2C_NUMBITS_MASK            (0x7u)
/**< Bit Mask for Number of Bits to be sent/received                         */
#define DDC_I2C_BUFFER_MAXLENGTH        (0xFFFFu)
/**< Maximum Buffer Length to be transfered                                  */
#define DDC_I2C_SLAVEADRR_10BIT         (0x3FFu)
/**< Slave Address Bit Mask to 10 Bit                                        */
#define DDC_I2C_RESET                   (0x0020u)
/**< Reset I2C Device                                                        */
#define DDC_I2C_VALUE_RESTORE           (0x0000D5FFu)
/**< Restore the Value                                                       */
#define DDC_ICIVR_INTCODE_ARDY          (0x00000004u)
/**< Register Access Ready Interrupt Enable Bit                              */
//#define DDC_I2C_MODULE_CLOCK_FREQ       (12u*1000000u)
#define DDC_I2C_MODULE_CLOCK_FREQ       (100u*1000000u) /* 100 MHz input clock to module */
/**< I2C Module clock frequency                                              */

/**
 *  PSC = 0, is not a valid option,
 *  so we have PSC value to be either 1 or 2
 *      if PSC = 1, peripheral clock is 12 MHz
 *      if PSC = 2, peripheral clock is 8 MHz
 * Also we are calculating clock for 50% Duty Cycle, hence CLKL = CLKH
 */
//#define DDC_I2C_PRESCALER_VALUE 2u
#define DDC_I2C_PRESCALER_VALUE 7u /* 100e6/(7+1) = 12.5e6 MHz */
/**< I2C Prescaler value                                                     */

/**
 * \brief State of I2C Driver instance
 *
 *  These enums are used to maintain the current state of the driver
 */
typedef enum
{
    I2C_DELETED = 0u,       /**< I2C Instance Deleted                        */
    I2C_CREATED,            /**< I2C Instance Created                        */
    I2C_OPENED,             /**< I2C Instance Open                           */
    I2C_CLOSED              /**< I2C Instance Closed                         */
} DDA_I2CState;

/**
 * \brief DDC I2C Object
 *
 * This structure basically holds the hardware configuration for each instance.
 * One such storage exists for each instance of the I2C peripheral
 */
typedef struct DDC_I2cObject
{
    /** SOC Related Details                                                  */
    Uint32                  intNum;
    /**< Interrupt number                                                    */

    /* I2C Peripheral details                                                */
    PSP_OpMode              mode;
    /**< Driver operating mode - polled, interrupt, dma                      */
    Uint32                  i2cOwnAddr;
    /**< Own address (7 or 10 bit)                                           */
    Uint32                  i2cBusFreq;
    /**< I2C Bus Frequency                                                   */
    Uint32                  numBits;
    /**< Number of bits/byte to be sent/received                             */
    Bool                    addressing;
    /**< 7bit/10bit Addressing mode                                          */
    Bool                    repeatMode;
    /**< Repeat Mode                                                         */
    Bool                    dlb;
    /**< Digital Loob Back (DLB) mode                                        */
    ioport CSL_I2cDrvRegs          *i2cRegs;
    /**< I2C peripheral register map                                         */

    /* Driver internal members                                               */
    Uint32                  instanceId;
    /**< Instance number                                                     */
    DDA_I2CState            state;
    /**< State of driver                                                     */
    Uint32                  numOpens;
    /**< Number of active opens                                              */
    Bool                    pendingState;
    /**< Shows whether io is in pending state or not                         */
    Bool                    Cancel_Pending_IO;
    /**< Shows whether IO has to cancel or not                               */

    /* Driver internal semaphore                                             */
    PAL_OsSemHandle         devBusySem;
    /**< Device busy semaphore handle                                        */
    PAL_OsSemHandle         completionSem;
    /**< Device busy semaphore handle                                        */

    /* Driver internal Transmission details                                  */
    Int                     currError;
    /**< Error code                                                          */
    Uint32                  currFlags;
    /**< Current Flags for read/write                                        */
    Uint16                  * currBuffer;
    /**< User buffer for read/write                                          */
    Uint32                  currBufferLen;
    /**< User buffer length                                                  */

    /* NOTE: Though slave mode is not supported by this driver,
     * incase of any addressed-as-slave condition, the driver can call
     * an application registered callback. This is provided to indicate
     * an unexpected I2C activity.
     */

    /* Driver hardware event callback details                                */
    PSP_I2cHwEventCallback  hwEventCallback;
    /**< callback when accessed as slave                                     */
    Ptr                     appSlvHandle;
    /**< App handle which will be provided while regsitering callback
         and will be returned with slave callback                            */
} DDC_I2cObject;

/**
 *  \brief I2C DDC Driver Object
 *
 */
typedef struct DDC_I2cDriverObject
{
    DDC_I2cObject*          pi2cInstHandle; /**< i2c Insatnce object handle  */
    PSP_I2cAppCallback      callBack;       /**< Callback from applications  */
    Ptr                     appHandle;
    /**< application handle which will be returned with callback             */
} DDC_I2cDriverObject;

/**
 * \brief I2C interrupt handler
 *
 * I2C interrupt handle function
 *
 * \param  i2cObj [IN]    I2C driver object
 * \return void
 */
void _DDC_i2cIntrHandler(DDC_I2cObject *i2cObj);

#endif  /*_DDC_I2C_H_ */

