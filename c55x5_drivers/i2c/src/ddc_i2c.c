/*
 * $$$MODULE_NAME ddc_i2c.c
 *
 * $$$MODULE_DESC ddc_i2c.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/**
 *  \file   ddc_i2c.c
 *
 *  \brief  I2C DDC implementation
 *
 *  This file contains the DDC implementation of the I2C driver to utilize
 *  the serivces of the I2C device driver.
 *
 *  (C) Copyright 2005, Texas Instruments, Inc
 *
 *  \author     PSG
 *
 *  \version    1.x - Anant Gole        - previous implementation
 *              2.0 - Nagarjuna Kristam - cleanup effort
 *              3.0 - Nagarjuna Kristam - new implementation
 *              4.0 - Pandith           - Porting to DA224
 *              5.0 - Ulagammal S       - staticSem used instead of Dynamic
 *                                        (defect 1 on I2C defect track sheet)
 *  \version    6.0 -    PR Mistral - Modified for ROM-ability
 */

#include "ddc_i2c.h"
#include "dda_i2c.h"

extern PSP_I2cConfig    *initConfig;
/**< I2C configuration structure */

extern SEM_Obj SEM_I2cDevBusy;
/**< Semaphore used for checking device busy */

DDC_I2cObject ddcI2CObj[PSP_I2C_NUM_INSTANCES];
/**< I2C DDC Object. One per instance. */

/* Note:PSP_I2C_NUM_INSTANCES & PSP_I2C_NUM_OPENS are defined in PSP_i2c.h */
DDC_I2cDriverObject ddcI2CDriver[PSP_I2C_NUM_INSTANCES][PSP_I2C_NUM_OPENS];
/**< I2C Driver object. One per instance per open */

/** I2C driver Transfer [internal to DDC where actual transfer happens]
 *
 *  This is the function in which a tuall data transfer happens.
 *  In this function it validates input parameters set the parameter values in
 *  to the registers and starts transmission
 *
 * \param       handle     [IN]     I2C Driver Instance Handle
 * \param       slaveAddr  [IN]     Slave address (only valid in Master mode)
 * \param       buffer     [IN]     Buffer (to read into or write from)
 * \param       bufLen     [IN]     Buffer length - number of bytes to transact
 * \param       flags      [IN]     Flags to specify mode of operation
 * \param       timeout    [IN]     Timeout for i2c transaction
 * \param       param      [IN]     Extra paramter for future usage
 * \param       xferActual [IN]     Gives number of bytes actually transferred
 *
 * \return      Interger Value > 0 - Number of bytes transferred.
 *
 */
static Int DDC_i2cTransfer (PSP_Handle handle,
                            Uint32     slaveAddr,
                            Uint16     *buffer,
                            Uint32     bufLen,
                            Uint32     flags,
                            Int32      timeout,
                            Ptr        param,
                            Ptr        xferActual);

/**
 *  \brief Wait for Bus busy
 *
 *  This function introduces delay in case of bus busy
 *
 *  \param  i2cObj       [IN]    I2C Driver Instance object
 *  \param  waitCount    [IN]    Wait time
 *  \param  sleepAllowed [IN]    Variable for deciding sleep allowed or not
 *
 *  \return On success returns PAL_SOK, else negative error code
 */
static Int i2cWaitForBB(const DDC_I2cObject *i2cObj,
                        Uint32 waitCount,
                        Uint32 sleepAllowed);

/* Function for getting the OS ticks, exposed to DDC by DDA */
extern Int  _DDA_i2cOsGetTicks(Int mSecs);

/**
 *  \brief Function for calculating clock settings
 *
 *  This function calulates clock frequency based on given
 *  prescaler and clock values
 *
 *  \param  obj   [IN]    I2C Driver instance object
 *  \param  psc   [IN]    Vlaue of the prescaler to derive clock.
 *  \param  clock [IN]    Input clock frequency
 *
 *  \return Derived clock frequency
 */
static Int i2cCalculateClockSettings( const DDC_I2cObject *obj , Uint32 *psc, Uint32 *clock) ;

/**
 *  \brief Function to cancel pending IO operations
 *
 *  This function cancels any pending IO operations
 *
 *  \param  hi2cObj   [IN]    I2C Driver instance object
 *
 *  \return void
 */
static void i2c_cancelIO(Ptr hi2cObj);

/**
 *  \brief Initialize a given I2C driver (instance)
 *
 *  Typically, software bookkeeping functions are performed in this call.
 *  Memory for device instance specific data structures may be allocated
 *  and initialized. Configuration information may be passed in the call
 *  and initialization based upon this information is done.
 *
 *  \param  instanceId [IN]     I2C instance number
 *  \param  initConfig [IN] Initialization configuration
 *
 *  \return PSP_SOK or PSP Error code
 */
PSP_Result  PSP_i2cCreate(Uint32 instanceId, const PSP_I2cConfig *initConfig)
{
    DDC_I2cObject   * i2cObj;
    PSP_Result      retVal;
    PAL_OsSemAttrs semAttrs = {PAL_OSSEM_TYPE_FIFO, NULL};

    retVal = PSP_SOK;

    /* Validate instance ID */
    if ( (instanceId >= PSP_I2C_NUM_INSTANCES) || (initConfig == (PSP_I2cConfig *)NULL) ) {
        retVal = PSP_E_INVAL_PARAM;
    }
    else {
        i2cObj = &ddcI2CObj[instanceId];

        if (i2cObj == (DDC_I2cObject *)NULL) {
            retVal = PSP_E_INVAL_STATE;
        }
        /* Check for only Polled Mode of Operation */
        else if ((initConfig->numBits < 2u) ||
            (initConfig->numBits > 8u)) {
                retVal = PSP_E_INVAL_PARAM;
        }
        else if ((initConfig->mode == PSP_OPMODE_INTERRUPT) ||
            (initConfig->mode == PSP_OPMODE_DMAINTERRUPT)) {
                retVal = PSP_E_INVAL_PARAM;
        }
        /* Check input/ output freq param limits */
        else if ((initConfig->i2cBusFreq < PSP_I2C_MIN_BUS_FREQ) ||
            (initConfig->i2cBusFreq > PSP_I2C_MAX_BUS_FREQ)) {
                retVal = PSP_E_INVAL_PARAM;
        }
        /* Check if repeatmode is to be enabled/disabled */
        else if (( initConfig->repeatMode != (Bool)TRUE) && (initConfig->repeatMode != FALSE)) {
            retVal = PSP_E_INVAL_PARAM;
        }
        /* Check if addressing is to be enabled/disabled */
        else if ((initConfig->addressing != FALSE)) {
            retVal = PSP_E_INVAL_PARAM;
        }
        /* Check if dlb is to be enabled/disabled */
        else {
            if ((initConfig->dlb != (Bool)TRUE) && (initConfig->dlb != FALSE)) {
                retVal = PSP_E_INVAL_PARAM;
            }
        }
    } /* retVal =0 */

    /* Copy init parameters */
    if (PSP_SOK == retVal) {
        /* Check the operation mode */
        if ((initConfig->mode == PSP_OPMODE_INTERRUPT) ) {
            i2cObj->mode = PSP_OPMODE_INTERRUPT;
        } else {
            i2cObj->mode = PSP_OPMODE_POLLED;
        }

        i2cObj->i2cOwnAddr          = initConfig->i2cOwnAddr;
        i2cObj->i2cBusFreq          = initConfig->i2cBusFreq;
        i2cObj->numBits             = initConfig->numBits;
        i2cObj->repeatMode          = initConfig->repeatMode;
        i2cObj->addressing          = initConfig->addressing;
        i2cObj->dlb                 = initConfig->dlb;


        /* Create internal semaphores */
      //  if (PAL_osSemCreate("I2C Dev Busy Sem", 1, &semAttrs, &i2cObj->devBusySem) != PAL_SOK) {
      //      retVal = PSP_E_RESOURCES;
      //  }

        if (i2cObj->mode != PSP_OPMODE_POLLED)
        {
            if (PAL_osSemCreate("I2C Completion Sem", 1, &semAttrs, &i2cObj->completionSem) != PAL_SOK) {
                PAL_osSemDelete(i2cObj->devBusySem);
                retVal = PSP_E_RESOURCES;
            }
            else {
                if (PAL_osSemTake(i2cObj->completionSem, -1) != PAL_SOK) {
                    PAL_osSemDelete(i2cObj->devBusySem);
                    PAL_osSemDelete(i2cObj->completionSem);
                    retVal = PSP_E_RESOURCES;
                }
            }
        }
    } /* if (0 == retVal) */

    /* set state to created */
    if (PSP_SOK == retVal) {
        i2cObj->numOpens = 0u;
        i2cObj->state = I2C_CREATED;
        i2cObj->instanceId = instanceId;
        retVal = PSP_SOK;
    }
    return (retVal);
}

/**
 *  \brief Delete a given I2C driver (instance)
 *
 *  The instance of I2C is deleted in software. If there are open handles
 *  then they are invalidated
 *
 *  \param  instanceId [IN]        I2C instance number
 *  \return PSP_SOK or PSP Error code
 */
PSP_Result PSP_i2cDelete(Uint32 instanceId)
{
    DDC_I2cObject *i2cObj;
    Int retVal;

    retVal = PSP_SOK;

    /* Validate instance ID */
    if (instanceId >= PSP_I2C_NUM_INSTANCES) {
            retVal = (PSP_E_INVAL_PARAM);
    }
    /* Check state of driver */
    else {
        i2cObj = &ddcI2CObj[instanceId];
        if (i2cObj == ((DDC_I2cObject *) NULL)) {
         retVal = (PSP_E_INVAL_PARAM);
        }
        else {
            if ((i2cObj->state != I2C_CREATED) && ((i2cObj->state != I2C_CLOSED))) {
                retVal = (PSP_E_INVAL_STATE);
            }
        }
    }

    if (PSP_SOK == retVal) {
        /* Delete driver semaphores */
        if (PAL_osSemDelete(i2cObj->devBusySem) != PAL_SOK) {
            retVal = (PSP_E_RESOURCES);
        } else {
            if (PSP_OPMODE_INTERRUPT == i2cObj->mode) {
                if (PAL_osSemDelete(i2cObj->completionSem) != PAL_SOK) {
                        retVal = (PSP_E_RESOURCES);
                }
            }
        }
    }

    if (PSP_SOK == retVal) {
        /* set state to deleted */
        i2cObj->numOpens = 0u;
        i2cObj->state = I2C_DELETED;
    }
    return (retVal);
}

/**
 *  \brief Open Instance of the I2C device
 *
 *  This function prepares the device hardware for data transfers and
 *  usage by the upper layer driver software. The DDA layer is expected
 *  to install the ISR and enable it only after the completion of this
 *  call. The function prepares the driver instance for data transfers
 *  and returns an handle to the driver instance.
 *
 * Note: The driver can be opened multiple times
 *  \param  instanceId [IN]    I2C instance number
 *  \param  callback   [IN]    Callback function for events notification
 *  \param  appHandle  [IN]    This appHandle will be provided by user and
 *                             will be returned along with call back
 *  \return PSP_Handle [OUT]   If successful positive Driver Instance Handle
 *                             else return the NULL pointer.
 */
PSP_Handle  PSP_i2cOpen(Uint32 instanceId, PSP_I2cAppCallback callback,
                                                                Ptr appHandle)
{
    DDC_I2cObject *i2cObj;
    DDC_I2cDriverObject   *i2cHandle;
    Uint32 psc, clk, cnt;
    Uint32 cookie;

    i2cHandle = NULL;

    /* Validate instance ID */
    if (instanceId < PSP_I2C_NUM_INSTANCES) {
        i2cObj = &ddcI2CObj[instanceId];
        if ( ((i2cObj != NULL) && (i2cObj->state != I2C_DELETED)) &&
             (i2cObj->numOpens < PSP_I2C_NUM_OPENS) )
        {
            /* Create i2c driver object */
            for (cnt=0; cnt < PSP_I2C_NUM_OPENS; cnt++)
            {
                    ddcI2CDriver[i2cObj->instanceId][cnt].callBack          = callback;
                    ddcI2CDriver[i2cObj->instanceId][cnt].appHandle         = appHandle;
                    ddcI2CDriver[i2cObj->instanceId][cnt].pi2cInstHandle    = i2cObj;
                    i2cHandle = &ddcI2CDriver[i2cObj->instanceId][cnt];
                    break;
            }
        }
    }

    if (i2cHandle != NULL) {
        /* Start of critical section */
        PAL_osProtectEntry(PAL_OSPROTECT_INTERRUPT, &cookie);

        /* Init hardware if first open */
        if (i2cObj->numOpens == 0)
        {
            i2cObj->instanceId = instanceId;
            switch (instanceId)
            {
                case 0:
                    i2cObj->i2cRegs = (CSL_I2cDrvRegs *) CSL_I2C_REGS;
                    break;

                default:
                    i2cHandle = NULL;
                    break;
            }

            if (i2cHandle != NULL) {
                /* Put the module in reset */
                i2cObj->i2cRegs->ICMDR = 0;

                if(i2cCalculateClockSettings(i2cObj , &psc, &clk) != 0 )
                {
                    i2cHandle = NULL;
                } else {
                    i2cObj->i2cRegs->ICPSC = psc;
                    i2cObj->i2cRegs->ICCLKH = clk;
                    i2cObj->i2cRegs->ICCLKL = clk;

                    /* Set own address and bits/byte */
                    i2cObj->i2cRegs->ICOAR = i2cObj->i2cOwnAddr;
                    i2cObj->i2cRegs->ICMDR = (i2cObj->numBits & 0x7u) | CSL_I2C_ICMDR_MST_MASK;

                    /* Enable/Disable Repeat,DLB and 7/10 bit addressing modes */
                    if ((Bool)TRUE == i2cObj->dlb) {
                        i2cObj->i2cRegs->ICMDR |= CSL_I2C_ICMDR_DLB_MASK;
                    } else  {
                        i2cObj->i2cRegs->ICMDR &= ~(CSL_I2C_ICMDR_DLB_MASK);
                    }

                    if ( ((Bool)TRUE == i2cObj->repeatMode) )   {
                        i2cObj->i2cRegs->ICMDR |= CSL_I2C_ICMDR_RM_MASK;
                    } else {
                        i2cObj->i2cRegs->ICMDR &= (~(CSL_I2C_ICMDR_RM_MASK));
                    }

                    if ((Bool)TRUE == i2cObj->addressing)   {
                        i2cObj->i2cRegs->ICMDR |= CSL_I2C_ICMDR_XA_MASK;
                    } else {
                        i2cObj->i2cRegs->ICMDR &= (~(CSL_I2C_ICMDR_XA_MASK));
                    }
                    /* Bring module out of reset */
                    i2cObj->i2cRegs->ICMDR |= CSL_I2C_ICMDR_IRS_MASK;

                    /* Set the backward compatibility mode off - this is critical to get
                     * interrupts correctly */
                    i2cObj->i2cRegs->ICEMDR = 0;
                } /* else retCode */
            } /* retVal = NULL in side if first oprn */
        } /* First open initialization */
        /* End of critical section */
        PAL_osProtectExit(PAL_OSPROTECT_INTERRUPT, cookie);
    } /* retVal = NULL before firstt open */

    if (i2cHandle != NULL) {
        /* Increment open count and return driver handle */
        i2cObj->numOpens = i2cObj->numOpens + 1u;

        /* Updated the driver state */
        i2cObj->state = I2C_OPENED;
    }

    return (PSP_Handle)i2cHandle;
}

/**
 *  \brief Close Instance of the I2C device
 *
 *  This function closes the device for data transfers and usage by the
 *  upper layer driver software. The hardware is programmed to stop/abort data
 *  transfer (depending upon the type of device and its specifics) and the
 *  device ISR is "disabled" by the upper layer driver software after the
 *  completion of this call. After the successful completion of this call, the
 *  device cannot perform any data IO.
 *
 *  \param  handle [IN]         I2C Driver Instance Handle
 *  \return PSP_SOK or PSP Error code
 */
PSP_Result PSP_i2cClose(PSP_Handle handle)
{
    DDC_I2cDriverObject   *pi2cHandle;
    DDC_I2cObject *i2cObj;
    Uint32 cookie;
    Int retVal;
    Int retCode;

    retVal  = PSP_SOK;
    retCode = 0;
    /* Check state of handle */
    if (handle == NULL) {
        retVal = (PSP_E_INVAL_PARAM);
    }
    else {
        pi2cHandle = (DDC_I2cDriverObject *)handle;
        if (pi2cHandle == ((DDC_I2cDriverObject *) NULL)) {
            retVal = (PSP_E_INVAL_PARAM);
        }
        else {
            i2cObj =  pi2cHandle->pi2cInstHandle;
            if (i2cObj == ((DDC_I2cObject *) NULL)) {
                retVal = (PSP_E_INVAL_PARAM);
            }
            else {
                /* Check for the validity of the I2C STATE */
                if ( i2cObj->state != I2C_OPENED ) {
                    retVal = (PSP_E_INVAL_STATE);
                }
            }
        }
    }

    if (retVal == PSP_SOK) {
        /*  Check for any IO is pending, if yes then cancel io */
        if ( i2cObj->pendingState == TRUE ){
                retCode = PSP_i2cIoctl(handle,PSP_I2C_IOCTL_CANCEL_PENDING_IO, NULL, NULL);
                if(retCode != PSP_SOK){
                    retVal = retCode;
                }
        }
        /* Start of Critical Section */
        PAL_osProtectEntry(PAL_OSPROTECT_INTERRUPT, &cookie);
        /* Decrease the Number of Opens */
        i2cObj->numOpens = i2cObj->numOpens - 1u;
        pi2cHandle->pi2cInstHandle = NULL;
        pi2cHandle = NULL;

        PAL_osProtectExit(PAL_OSPROTECT_INTERRUPT, cookie);
        if (i2cObj->numOpens == 0)
        {
            /* Put the device in reset and quit */
            i2cObj->i2cRegs->ICMDR = 0;
            i2cObj->state = I2C_CLOSED;
        }
    }/* retVal */
    return (retVal);
}

/**
 *  \brief Wait for Bus busy
 *
 *  This function introduces delay in case of bus busy
 *
 *  \param  i2cObj       [IN]    I2C Driver Instance object
 *  \param  waitCount    [IN]    Wait time
 *  \param  sleepAllowed [IN]    Variable for deciding sleep allowed or not
 *
 *  \return On success returns PAL_SOK, else negative error code
 */
static Int i2cWaitForBB(const DDC_I2cObject *i2cObj,
                        Uint32 waitCount,
                        Uint32 sleepAllowed)
{
    volatile Uint32 countdown;
    Int retVal;

    countdown = waitCount;
    retVal = 0;

    if (i2cObj == ((DDC_I2cObject *) NULL)) {
        retVal = PSP_E_INVAL_PARAM;
    }
    else {
        while (i2cObj->i2cRegs->ICSTR & CSL_I2C_ICSTR_BB_MASK)
        {
            if (countdown != 0) {
                if (sleepAllowed != 0) {
                    _DDA_i2cSleep(1u); /* Wait for 1 milliSeconds */
                }
            } else {
                retVal = -1;
                break;
            }
            countdown = countdown - 1u;
        }
    } /* retVal */
    return retVal;
}

/**
 *  \brief Read/Write (single transaction) using I2C
 *
 *  This function read/writes single transaction data using the I2C peripheral.
 *
 *  \param  handle    [IN]    I2C Driver Instance Handle
 *  \param  slaveAddr [IN]    Slave address (only valid in Master mode)
 *  \param  buffer    [IN]    Buffer (to read into or write from)
 *  \param  bufLen    [IN]    Buffer length - number of bytes to transact
 *  \param  flags     [IN]    Flags to specify mode of operation
 *  \param  timeout   [IN]    Timeout for i2c transaction
 *  \param  param     [IN]    Extra paramter for future usage
 *
 *  \return On success returns PAL_SOK, else negative error code
 */
Int PSP_i2cTransfer (PSP_Handle handle,
                     Uint32     slaveAddr,
                     Uint16     *buffer,
                     Uint32     bufLen,
                     Uint32     flags,
                     Int32      timeout,
                     Ptr        param)
{
    DDC_I2cDriverObject   *pi2cHandle;
    DDC_I2cObject *i2cObj;
    Int retVal;
    Int cnt;
    Int retCode;

    retVal  = 0;
    retCode = 0;
    cnt = 0;

    if (handle == NULL) {
        retVal = (PSP_E_INVAL_PARAM);
    }
    else {
        pi2cHandle = (DDC_I2cDriverObject *)handle;
        i2cObj =  pi2cHandle->pi2cInstHandle;
        if (i2cObj == ((DDC_I2cObject *) NULL)) {
            retVal = (PSP_E_INVAL_PARAM);
        }
    }

    if (retVal == 0) {
        /* Check input parameters */
        if ( ( (buffer == NULL) || (bufLen == 0) ) ||
                    (bufLen > DDC_I2C_BUFFER_MAXLENGTH) ) {
            retVal = (PSP_E_INVAL_PARAM);
        }

        else if(( (flags & PSP_I2C_READ) == 1u ) && ( (flags & PSP_I2C_WRITE) == 1u )) {
            retVal = (PSP_E_INVAL_PARAM);
        }
        /* Return error if aplication sets stop bit during repeat mode and bufLen > 0 */
        else if (i2cObj->repeatMode == TRUE) {
            if ( (bufLen > 0) && ((flags & PSP_I2C_STOP) != 0)) {
                retVal = (PSP_E_INVAL_PARAM);
            }
        }
        /* Protect transaction if not in polled mode */
        else {
                retCode = PAL_osSemTake(&SEM_I2cDevBusy,
                                SYS_FOREVER);// _DDA_i2cOsGetTicks((Int) timeout) );
                if (retCode != PAL_SOK)
                {
                    retVal = (retCode);
                }
            //  SEM_pend(&SEM_I2cDevBusy,SYS_FOREVER);
        }
    } /* retVal = 0 */

    if (retVal == 0) {
        /* call ddc transfer function */
        retCode = DDC_i2cTransfer (i2cObj,slaveAddr,buffer, bufLen,
                               flags, timeout , param, &cnt) ;
        if (retCode == 0) {
            retCode = PAL_SOK;
        }
           PAL_osSemGive(&SEM_I2cDevBusy);
         //  SEM_post(&SEM_I2cDevBusy);
        /* Mask off interrupts */
        i2cObj->i2cRegs->ICIMR = 0;

        retVal  = retCode;
    }
    return retVal ;
}

/**
 * \brief I2C interrupt handler
 *
 * I2C interrupt handle function
 *
 * \param  i2cObj [IN]    I2C driver object
 * \return void
 */
void _DDC_i2cIntrHandler(DDC_I2cObject * const i2cObj)
{
    volatile Uint32 intStatus;
    if (i2cObj == ((DDC_I2cObject *) NULL)) {
        return ;
    }

    intStatus = i2cObj->i2cRegs->ICIVR;
    while (intStatus != 0)
    {
        switch (intStatus) {
            case CSL_I2C_ICIVR_INTCODE_NONE:
            /* No interrupt - Ideally, control should not come here. */
                break;

            case CSL_I2C_ICIVR_INTCODE_AL:      /* Arbitration loss */
                i2cObj->currError = (Int)PSP_I2C_ARBITRATION_LOSS_ERR;
                PAL_osSemGive(i2cObj->completionSem);
                break;

            case CSL_I2C_ICIVR_INTCODE_NACK:    /* NACK interrupt */
                i2cObj->currError = PSP_I2C_NACK_ERR;
                PAL_osSemGive(i2cObj->completionSem);
                break;

            case CSL_I2C_ICIVR_INTCODE_RAR:
                /* Register Access ready */
                /* We do not need to handle this interrupt in this driver */
                /* Disable this interrupt enable bit */
                i2cObj->i2cRegs->ICIMR &= ~DDC_ICIVR_INTCODE_ARDY;
                break;

            case CSL_I2C_ICIVR_INTCODE_RDR:
                /* Receive Data ready */
                if (i2cObj->currBufferLen != 0) {
                    *i2cObj->currBuffer = (Uint16)(i2cObj->i2cRegs->ICDRR);
                    i2cObj->currBuffer = i2cObj->currBuffer + 1u;
                    i2cObj->currBufferLen = i2cObj->currBufferLen - 1u;
                    if (i2cObj->currBufferLen != 0) {
                        /* Check the Pending State */
                        if(TRUE == i2cObj->Cancel_Pending_IO) {
                            /* Cancel IO for Pending State */
                            i2c_cancelIO(i2cObj);
                            /* Error of Cancel IO */
                            i2cObj->currError = PSP_I2C_CANCEL_IO_ERROR;
                            /* Disable Receive interrupt */
                            i2cObj->i2cRegs->ICIMR &= ~CSL_I2C_ICIMR_ICRRDY_MASK;
                            i2cObj->Cancel_Pending_IO = FALSE;
                            /* Give Semaphore */
                            PAL_osSemGive(i2cObj->completionSem);
                        }
                    } else {
                         /* Disable Receive interrupt */
                        i2cObj->i2cRegs->ICIMR &= ~CSL_I2C_ICIMR_ICRRDY_MASK;
                        /* Give the Semaphore */
                        PAL_osSemGive(i2cObj->completionSem);
                    }
                }
                break;

            case CSL_I2C_ICIVR_INTCODE_TDR:
                /* Transmit Data ready */
                if (i2cObj->currBufferLen != 0) {
                        i2cObj->i2cRegs->ICDXR = *i2cObj->currBuffer;
                        i2cObj->currBuffer = i2cObj->currBuffer + 1u;
                        i2cObj->currBufferLen = i2cObj->currBufferLen - 1u;
                    if (i2cObj->currBufferLen != 0) {
                        /* Cancel IO for Pending IO State */
                        if(TRUE == i2cObj->Cancel_Pending_IO){
                            i2c_cancelIO(i2cObj);
                            i2cObj->currError = PSP_I2C_CANCEL_IO_ERROR;
                            i2cObj->i2cRegs->ICIMR &= ~CSL_I2C_ICIMR_ICXRDY_MASK;
                            i2cObj->Cancel_Pending_IO = FALSE;
                            /* Give Semaphore */
                            PAL_osSemGive(i2cObj->completionSem);
                        }
                    } else {
                         /* Disable Transmit interrupt */
                        i2cObj->i2cRegs->ICIMR &= ~CSL_I2C_ICIMR_ICXRDY_MASK;
                        PAL_osSemGive(i2cObj->completionSem);
                    }
                }
                break;

            case CSL_I2C_ICIVR_INTCODE_SCD:     /* Stop condition detected */
                /* We do not need to handle this interrupt in this driver */
                /* Disable this interrupt enable bit */
                i2cObj->i2cRegs->ICIMR &= ~CSL_I2C_ICIMR_SCD_MASK;
                break;

            case CSL_I2C_ICIVR_INTCODE_AAS:     /* Addressed as slave */
                /* Slave mode is not supported currently*/
                if (i2cObj->hwEventCallback != (PSP_I2cHwEventCallback)0) {
                    /* Callback function if registered by by the user */
                    i2cObj->hwEventCallback(i2cObj->appSlvHandle,0,0);
                }
                /* Disable Addressed as slave interrupt */
                i2cObj->i2cRegs->ICIMR &= ~(CSL_I2C_ICIMR_AAS_MASK);
                break;

            default:
                break;
        }
        intStatus = i2cObj->i2cRegs->ICIVR;
    }
}

/**
 *  \brief Read/Write multiple transactions using I2C
 *
 *  This function read/writes multiple transactions data using the I2C peripheral.
 *  Though nothing prevents the user from using multiple invokations of the
 *  single transaction transfer function, this function helps in making sure that
 *  all the transactions are completed in one go before another application gets
 *  hold of the i2c bus (from the applications perspective)
 *
 *  \param  handle  [IN]    I2C Driver Instance Handle
 *  \param  xfer    [IN]    I2C Transaction array - first element pointer
 *  \param  numXfer [IN]    Number of transactions to be done
 *  \param  param   [IN]    Extra paramter for future usage
 *  \param  timeout [IN]    Timeout for whole i2c transaction
 *  \return PSP_SOK or Transmission number where error occured
 */
PSP_Result  PSP_i2cTransferMultiple (PSP_Handle     handle,
                                     const PSP_I2cTransaction *xfer,
                                     Uint32         numXfer,
                                     Int32          timeout,
                                     Ptr            param)
{
    DDC_I2cDriverObject   *pi2cHandle;
    DDC_I2cObject *i2cObj;
    PSP_Result retCode;
    Int loop;
    Uint32 cnt;
    Int retVal;

    retCode = (PSP_Result)0;
    retVal = 0;

    /* Check the state of handle */
    if ( (handle == NULL) || (xfer == NULL) ) {
        retVal = (PSP_E_INVAL_PARAM);
    }
    else {
        pi2cHandle = (DDC_I2cDriverObject *)handle;
        i2cObj =  pi2cHandle->pi2cInstHandle;
        if (i2cObj == ((DDC_I2cObject *) NULL)) {
            retVal = (PSP_E_INVAL_PARAM);
        }
    }

    if (retVal == 0) {
        if(numXfer == 0)
        {
            retVal = (PSP_E_INVAL_PARAM);
        }
        if (retVal == 0) {
                retCode = PAL_osSemTake(i2cObj->devBusySem,
                                _DDA_i2cOsGetTicks((Int) timeout) );
                if (retCode != PAL_SOK) {
                    retVal = (retCode);
                }
        }
    }/* retVal  */

    if (retVal == 0) {
        for(loop = 0 ; loop < numXfer ; loop++)
        {
            retCode = DDC_i2cTransfer (i2cObj,
                                        xfer->slaveAddr,
                                        xfer->buffer,
                                        xfer->bufLen,
                                        xfer->flags,
                                        timeout,
                                        xfer->param,
                                        (Uint32 *) &cnt) ;
            if(retCode != 0)
            {
                retCode = loop ; /* failed transction number */
                break;
            }

            xfer = xfer + 1u;
            /* This delay is given to program the page or for some reason to avoid
               the driver from stopping/hanging*/
            _DDA_i2cSleep(1u);
        } /* for */

        PAL_osSemGive(i2cObj->devBusySem);

        /* Mask off interrupts */
        i2cObj->i2cRegs->ICIMR = 0;

    } /* retVal */
    if (retVal == 0) {
        retVal  = retCode;
    }
    return retVal;
}

/**
 *  \brief IO Control for I2C controller.
 *
 *  This function supports various IOCTLs for the I2C controller.
 *
 *  \param  handle [IN]    I2C Driver Instance Handle
 *  \param  cmd    [IN]    IOCTL command
 *  \param  cmdArg [IN]    Arguments, if any, for the command
 *  \param  param  [IN]    IOCTL specific parameter (driver specific).
 *
 *  \return PSP_SOK or PSP Error code.
 */
PSP_Result  PSP_i2cIoctl(PSP_Handle      handle,
                         PSP_I2cIoctlCmd cmd,
                         Ptr             cmdArg,
                         Ptr             param)
{
    PSP_Result              retVal;
    DDC_I2cDriverObject     *pi2cHandle;
    DDC_I2cObject           *i2cObj;
    Bool                    nack;
    Int                     *lcmdArg; /*  for tesing only */

    retVal = PSP_SOK;

    /* Check the state of handle */
    if (handle == NULL) {
        retVal = (PSP_E_INVAL_PARAM);
    }
    else {
        pi2cHandle = (DDC_I2cDriverObject *)handle;
        i2cObj =  pi2cHandle->pi2cInstHandle;
        if (i2cObj == ((DDC_I2cObject *) NULL)) {
            retVal = (PSP_E_INVAL_PARAM);
        }
    }

    if (retVal == PSP_SOK) {
        switch (cmd) {
            case PSP_I2C_IOCTLE_SET_RX_SLAVE_CB :
                if ( (NULL == cmdArg) && (NULL == param) ) {
                    retVal = PSP_E_INVAL_PARAM;
                } else {
                    i2cObj->hwEventCallback = *(PSP_I2cHwEventCallback *)cmdArg;
                    i2cObj->appSlvHandle = param;
                }
                break;
            case PSP_I2C_IOCTL_TESTS: /* testing only*/
                if (cmdArg == NULL) {
                    retVal = PSP_E_INVAL_PARAM;
                }
                if (retVal == PSP_SOK) {
                    lcmdArg = (Int*)cmdArg;
                    if ( lcmdArg == NULL) {
                        retVal = PSP_E_INVAL_PARAM;
                    }
                    else if ( i2cObj->i2cRegs->ICOAR    != lcmdArg[0] ) {
                        retVal = PSP_E_INVAL_PARAM;
                    }
                    else if ( i2cObj->addressing    != lcmdArg[1] ) {
                        retVal = PSP_E_INVAL_PARAM;
                    }
                    else if ( i2cObj->dlb           != lcmdArg[2] ) {
                        retVal = PSP_E_INVAL_PARAM;
                    }
                    else if ( i2cObj->repeatMode    != lcmdArg[3] ) {
                        retVal = PSP_E_INVAL_PARAM;
                    }
                    else {
                        if ( i2cObj->numBits        != lcmdArg[4] ) {
                            retVal = PSP_E_INVAL_PARAM;
                        }
                    }
                } /* retVal */
                break;
            case PSP_I2C_IOCTL_CANCEL_PENDING_IO:
                /* Start of Critical Section */
                /* CAlling this when no io is in transmission */
                if(i2cObj->pendingState == TRUE){
                    i2cObj->Cancel_Pending_IO = TRUE; /* to cancel io in ddc*/
                    while (i2cObj->pendingState != 0) {
                        _DDA_i2cSleep(1u); /* For testing task swithig only */
                    }
                } else {
                    retVal = PSP_E_INVAL_STATE;
                }/* if */
                break;
            case PSP_I2C_IOCTL_BIT_COUNT:
                if (cmdArg == ((Uint32 *) NULL)) {
                    retVal = PSP_E_INVAL_PARAM;
                } else {
                    /* Set the bit count value */
                    i2cObj->i2cRegs->ICMDR != ( *((Uint32 *)cmdArg)
                                                    & DDC_I2C_NUMBITS_MASK);
                }
                break;
            case PSP_I2C_IOCTL_NACK:
                if (cmdArg == ((Uint32 *) NULL)) {
                    retVal = (PSP_E_INVAL_PARAM);
                } else {
                    nack = (*((Bool*)cmdArg)) ;
                    if(nack == TRUE) {
                        /* set the nack mode, MSB of ICMDR*/
                        i2cObj->i2cRegs->ICMDR = i2cObj->i2cRegs->ICMDR | (0x8000u);
                    } else {
                        /* disable the nack mode, */
                        i2cObj->i2cRegs->ICMDR = i2cObj->i2cRegs->ICMDR & ~(0x8000u);
                    }
                }
                break;

            default:
                retVal = PSP_E_INVAL_PARAM;
                break;
        }
    } /* retVal */
    return retVal ;
}

/** I2C driver Transfer [internal to DDC where actual transfer happens]
 *
 *  This is the function in which a tuall data transfer happens.
 *  In this function it validates input parameters set the parameter values in
 *  to the registers and starts transmission
 *
 * \param       handle     [IN]     I2C Driver Instance Handle
 * \param       slaveAddr  [IN]     Slave address (only valid in Master mode)
 * \param       buffer     [IN]     Buffer (to read into or write from)
 * \param       bufLen     [IN]     Buffer length - number of bytes to transact
 * \param       flags      [IN]     Flags to specify mode of operation
 * \param       timeout    [IN]     Timeout for i2c transaction
 * \param       param      [IN]     Extra paramter for future usage
 * \param       xferActual [IN]     Gives number of bytes actually transferred
 *
 * \return      Interger Value > 0 - Number of bytes transferred.
 *
 */
static Int DDC_i2cTransfer (PSP_Handle handle,
                     Uint32     slaveAddr,
                     Uint16     *buffer,
                     Uint32     bufLen,
                     Uint32     flags,
                     Int32      timeout,
                     Ptr        param,
                     Ptr        xferActual)
{
    DDC_I2cObject   *i2cObj;
    Uint32          cnt;
    Uint32          icmdr;
    Uint32          status;
    PSP_Result      retCode;
    Uint32          cookie;
    Int             retVal;
    volatile Uint16    looper;
#ifdef BIOS_BUILD
    Uint32          timeCnt;
#endif

    cnt = 0;
    icmdr = 0;
    retCode = 0;
    retVal = 0;

    /* Check the state of handle */
    if (((handle == NULL) || (buffer == NULL)) || (xferActual == NULL)) {
        retVal = (PSP_E_INVAL_PARAM);
    }
    else {
        i2cObj =  (DDC_I2cObject *) handle;
        if (i2cObj == ((DDC_I2cObject *) NULL)) {
            retVal = (PSP_E_INVAL_PARAM);
        }
        /* Check user has not set both read and write flag at same time*/
        else if (((flags & PSP_I2C_READ) != 0) && ((flags & PSP_I2C_WRITE) != 0)) {
            retVal = (PSP_E_INVAL_PARAM);
        }
        else if ( ( (buffer == NULL) || (bufLen == 0) ) ||
                    (bufLen > DDC_I2C_BUFFER_MAXLENGTH) ) {
            retVal = (PSP_E_INVAL_PARAM);
        }
        else if (i2cObj->repeatMode == TRUE) {
            if ( (bufLen > 0) && ((flags & PSP_I2C_STOP) != 0)) {
                retVal = (PSP_E_INVAL_PARAM);
            }
        }
        else {
            /* Check for Bus Busy if not restarting */
            if (((flags & PSP_I2C_RESTART) == 0) && ((flags & PSP_I2C_IGNORE_BUS_BUSY) == 0)) {
                /* Loop on busy condition */
                if (i2cObj->mode == PSP_OPMODE_POLLED) {
                    retCode = i2cWaitForBB(i2cObj, PSP_I2C_BUS_BUSY_TIMEOUT_CNT, 0);
                    if ( retCode != 0 ) {
                        retVal = (PSP_I2C_BUS_BUSY_ERR);
                    }
                } else {
                    retCode = i2cWaitForBB(i2cObj, PSP_I2C_BUS_BUSY_TIMEOUT_MSECS, 1u);
                    if ( retCode != 0 ) {
                        retVal = (PSP_I2C_BUS_BUSY_ERR);
                    }
                }
                /* Enters this condition if i2c bus is busy */
            }
        } /* else */
    } /* retVal */

    if (retVal == 0) {
        /* Set count for data transfer and init mode control register */
        i2cObj->i2cRegs->ICCNT = (bufLen & DDC_I2C_BUFFER_MAXLENGTH);

        /* Check flags and set accordingly */
        if ((flags & PSP_I2C_MASTER)  != 0) {
            i2cObj->i2cRegs->ICMDR |= CSL_I2C_ICMDR_MST_MASK;
        }
        if ((flags & PSP_I2C_START)  != 0) {
            icmdr |= CSL_I2C_ICMDR_STT_MASK;
        }
        if ((flags & PSP_I2C_STOP) != 0) {
            icmdr |= CSL_I2C_ICMDR_STP_MASK;
        }
        if ((flags & PSP_I2C_WRITE) != 0) {
            i2cObj->i2cRegs->ICMDR |= CSL_I2C_ICMDR_TRX_MASK;
        }
        if ((flags & PSP_I2C_READ) != 0) {
            i2cObj->i2cRegs->ICMDR &= ~CSL_I2C_ICMDR_TRX_MASK;
        }

        /* Write slave address - restrict to 10 bits */
        i2cObj->i2cRegs->ICSAR = (((Uint)slaveAddr) & DDC_I2C_SLAVEADRR_10BIT);
        /* Check if restart required to set the bus busy */
        if ((flags & PSP_I2C_RESTART) != 0) {
            i2cObj->i2cRegs->ICSTR |= CSL_I2C_ICSTR_BB_MASK;
            icmdr |= CSL_I2C_ICMDR_STT_MASK;
        }
        /* To make transaction is in progress */
        i2cObj->pendingState = TRUE;
        /* If polled mode, wait for data transfer and completion,
         * else wait (on a semaphore) for transaction to complete */
        if (i2cObj->mode == PSP_OPMODE_POLLED)
        {
            i2cObj->i2cRegs->ICIMR = 0;        /* Mask all interrupts - :)
                                                * just to make sure */
            i2cObj->i2cRegs->ICMDR |= icmdr;    /* Start bit set here to
                                                * start transaction */
#ifdef BIOS_BUILD
            timeCnt = PAL_osGetCurrentTick();
#endif

            while (cnt < bufLen)
            {
                if ( retCode == 0 )
                {
                    if ((flags & PSP_I2C_WRITE) != 0)
                    {
                        /* Transmit */
                        if ((i2cObj->i2cRegs->ICSTR & CSL_I2C_ICSTR_ICXRDY_MASK) != 0) {
                            i2cObj->i2cRegs->ICDXR = (*buffer & 0x00FF); /* Transmit 8-bit data */
                            buffer = buffer + 1u;
                            cnt = cnt + 1u;
                        }

                    } else {
                        /* Receive */
                        if ((i2cObj->i2cRegs->ICSTR & CSL_I2C_ICIMR_ICRRDY_MASK) != 0) {
                            *buffer = (Uint8) i2cObj->i2cRegs->ICDRR;
                            buffer = buffer + 1u;
                            cnt = cnt + 1u;
                        }
                    }

                    /* Check for Arbitration loss, transmit underflow and
                     * receive overflow errors */
                    status = i2cObj->i2cRegs->ICSTR;
                    if ((status & CSL_I2C_ICSTR_AL_MASK) != 0) {
                        retCode = PSP_I2C_ARBITRATION_LOSS_ERR;
                    }
                    else if ((status & CSL_I2C_ICSTR_RSFULL_MASK) != 0) {
                        retCode = PSP_I2C_RECEIVE_OVERFLOW_ERR;
                    }
                    /* Check for NACK error
                     */
                    else{
                        if ((status & CSL_I2C_ICSTR_NACK_MASK) != 0){
                            retCode = PSP_I2C_NACK_ERR;
                         }
                    }
                }
                else
                {
                    if (retCode == PSP_SOK) {
                        retCode = PSP_E_TIMEOUT;
                    }
                    break;
                }
//                PAL_osWaitMsecs(1u);
                for(looper = 0;looper < 2000;looper++)
                {
                    asm("\tNOP;");
                }
            } /* While number of byte to be transmitted/received loop */
        } /* if polled mode */
        else
        {
            /* Set the current buffer params correctly */
            i2cObj->currError = 0;
            i2cObj->currFlags = flags;
            i2cObj->currBuffer = (buffer+cnt);
            i2cObj->currBufferLen = (bufLen-cnt);

            if (i2cObj->currBufferLen > 0) {
                /* Enable interrupts */
                i2cObj->i2cRegs->ICIMR = ( CSL_I2C_ICIMR_AL_MASK |
                                                /* Arbitration lost */
                                        CSL_I2C_ICIMR_NACK_MASK |
                                                /* NACK received/not received */
                                        CSL_I2C_ICIMR_AAS_MASK);
                                                /* Addressed as Slave */

            if ((flags & PSP_I2C_WRITE) != 0){
                    i2cObj->i2cRegs->ICIMR |= CSL_I2C_ICIMR_ICXRDY_MASK ;
                                                        /* Transmit ready */
                }else if((flags & PSP_I2C_READ) != 0){
                    i2cObj->i2cRegs->ICIMR |= CSL_I2C_ICIMR_ICRRDY_MASK ;
            }                                       /* Receive ready */

                /* Start I2C transaction */
                i2cObj->i2cRegs->ICIVR = 0; /* Clear IVR before doing a start */
                i2cObj->i2cRegs->ICMDR |= icmdr; /* Start bit set here to start
                                                 * transaction */

                /* Take completion semaphore */
                retCode = PAL_osSemTake(i2cObj->completionSem,
                                    _DDA_i2cOsGetTicks( timeout) );
                if (retCode != PAL_SOK) {
                    /* Entry of Critical Section */
                    PAL_osProtectEntry(PAL_OSPROTECT_INTERRUPT, &cookie);
                    cnt = bufLen - i2cObj->currBufferLen;
                    i2cObj->currBufferLen = 0 ;
                    i2cObj->currBuffer = NULL;
                    i2cObj->currError = PSP_E_TIMEOUT;
                    i2cObj->i2cRegs->ICIMR &= ~CSL_I2C_ICIMR_ICXRDY_MASK;
                                                    /* Disable Transmit interrupt */
                    i2cObj->i2cRegs->ICIMR &= ~CSL_I2C_ICIMR_ICRRDY_MASK;
                                                    /* Disable Receive interrupt */

                    /*wait till data is shifted out of shift register*/
                    if ((flags & PSP_I2C_WRITE) != 0) {
                        while (( (i2cObj->i2cRegs->ICSTR) & (CSL_I2C_ICSTR_ICXRDY_MASK) ) == 0) {
                        }
                    }
                    /*Put the module in reset mode*/
                    i2cObj->i2cRegs->ICMDR &= ~( DDC_I2C_RESET);
                    /*Bring the module out of reset*/
                    i2cObj->i2cRegs->ICMDR |=  DDC_I2C_RESET;
                    /* Exit of Critical Section */
                    PAL_osProtectExit(PAL_OSPROTECT_INTERRUPT, cookie);
                } else {
                    /* Return error code (if error) or number of bytes transmitted */
                    retCode = i2cObj->currError;
                    /* Disable the interrupts once it comes out of ISR */
                    i2cObj->i2cRegs->ICIMR = 0;
                }
            }
            /* Return error code (if error) or number of bytes transmitted */
            cnt = bufLen - i2cObj->currBufferLen;
        }
#ifdef BIOS_BUILD
        if (1u == bufLen) {
            timeCnt = PAL_osGetCurrentTick();
            /* This Delay is given to ensure whether there is check for any errors */
             while(FALSE == PAL_osCheckTimeOut(timeCnt,1)){
            }
        }
#endif
        /* Check for Arbitration loss, transmit underflow and
         * receive overflow errors( this checking is for the reason that updating
         * ICSTR depends on the clock frequency and speed of slave */
        if ((i2cObj->i2cRegs->ICSTR & CSL_I2C_ICSTR_AL_MASK) != 0) {
            retCode = PSP_I2C_ARBITRATION_LOSS_ERR;
        } else if ((i2cObj->i2cRegs->ICSTR & CSL_I2C_ICSTR_RSFULL_MASK) != 0) {
            retCode = PSP_I2C_RECEIVE_OVERFLOW_ERR;
        }
        /* Check for NACK error
         */
        else {
            if ((i2cObj->i2cRegs->ICSTR & CSL_I2C_ICSTR_NACK_MASK) != 0) {
                retCode = PSP_I2C_NACK_ERR;
            }
        }
        if ( retCode != 0) {
            i2c_cancelIO(i2cObj);
            retVal = retCode;
        }
        *(Uint32 *)xferActual = cnt;
        i2cObj->pendingState=FALSE;
    } /* retVal */
    return retVal;
}


/* NOTE: I2C Clock divider programming info
 * As per I2C specs the following formulas provide prescalar and low/high
 * divider values
 *
 * input clk --> PSC Div -----------> ICCL/H Div --> output clock
 *                       module clk
 *
 * output clk = module clk / (PSC + 1) [ (ICCL + d) + (ICCH + d) ]
 *
 * Thus for Duty cycle,
 * ICCL = ICCH = clk = input clk / ( (psc +1) * output clk * 2 ) - d;
 *
 * where if PSC == 0, d = 7,
 *       if PSC == 1, d = 6
 *       if PSC > 1 , d = 5
 */
/**
 *  \brief Function for calculating clock settings
 *
 *  This function calulates clock frequency based on given
 *  prescaler and clock values
 *
 *  \param  obj   [IN]    I2C Driver instance object
 *  \param  psc   [IN]    Vlaue of the prescaler to derive clock.
 *  \param  clock [IN]    Input clock frequency
 *
 *  \return Derived clock frequency
 */
static Int i2cCalculateClockSettings(const DDC_I2cObject *obj,
                                     Uint32 *psc,
                                     Uint32 *clock)
{
    Uint32 prescalar;
    Uint32 D;
    Int retVal;

    prescalar = DDC_I2C_PRESCALER_VALUE;
    retVal = 0;

    if ( ( (obj == NULL) || (psc == (Uint32 *)NULL) ) ||
                    ( (clock == (Uint32 *)NULL) || (obj == NULL) ) ) {
        retVal = (PSP_E_INVAL_PARAM);
    }
    else {
        switch (prescalar) {
            case 1:
                D = 6u;
                break;

            case 2:
                D = 5u;
                break;

            default:
                //prescalar = 2u;
                D = 5u;
        }

        if (obj->dlb == TRUE) {
            D = 5u;
            prescalar = 2u;
        }

        *clock = (Uint32) ((DDC_I2C_MODULE_CLOCK_FREQ / ( (prescalar +1u) * obj->i2cBusFreq * 2u) ) - (D)) ;
        *psc = prescalar;
    }
    return retVal;
}

/**
 *  \brief Function to cancel pending IO operations
 *
 *  This function cancels any pending IO operations
 *
 *  \param  hi2cObj   [IN]    I2C Driver instance object
 *
 *  \return void
 */
static void i2c_cancelIO(Ptr hi2cObj)
{
    Uint32 psc,clkh,clkl,oar,icmdr; /* local variables to store register contents */
    DDC_I2cObject * i2cObj;

    i2cObj = NULL;

    if (hi2cObj == NULL) {
        return;
    }
    else {
        i2cObj = (DDC_I2cObject * )hi2cObj;
        /* set start,stop and transmit bits to zero and restore values of other bits */
        icmdr = i2cObj->i2cRegs->ICMDR & DDC_I2C_VALUE_RESTORE;
        psc   = i2cObj->i2cRegs->ICPSC;
        clkh  = i2cObj->i2cRegs->ICCLKH;
        clkl  = i2cObj->i2cRegs->ICCLKL;
        oar   = i2cObj->i2cRegs->ICOAR;

        /* Put the module in reset */
        i2cObj->i2cRegs->ICMDR = 0;

        /* Update the Cancel IO related parameters in i2cObject */
        i2cObj->pendingState = FALSE;

        /* Load the Counter register with zero value */
        i2cObj->i2cRegs->ICCNT = 0;
        /* Restore the value from temporary variables to register */
        i2cObj->i2cRegs->ICPSC   = psc;
        i2cObj->i2cRegs->ICCLKH  = clkh;
        i2cObj->i2cRegs->ICCLKL  = clkl;
        i2cObj->i2cRegs->ICOAR   = oar;
        /* Bring module out of reset */
        i2cObj->i2cRegs->ICMDR   = icmdr;
        /* Set the backward compatibility mode off -
         * this is critical to get interrupts correctly */
        i2cObj->i2cRegs->ICEMDR = 0;
    }
}

/* End of file */
