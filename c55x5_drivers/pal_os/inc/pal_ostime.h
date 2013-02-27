/*
 * $$$MODULE_NAME pal_ostime.h
 *
 * $$$MODULE_DESC pal_ostime.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/** \file   pal_ostime.h
    \brief  PAL OS Service Header File

    This file declares additional OS abstraction services for PrOS. The following
    PrOS services are provided:

    1. Get systime, current tick and check timeout.

    (C) Copyright 2007, Texas Instruments, Inc

    \author     - PSG, TI
    \version    1.0 - Nagarjuna Kristam     -   Created
 */

#ifndef _PAL_OSTIME_H
#define _PAL_OSTIME_H

#include <psp_common.h>


/**
 * \defgroup PrOS_OsServ PAL OS Time Interface
 *
 * PAL OS TIME INTERFACE
 * @{
 */

/**
 * \brief   PAL OS Get Current System Time
 *
 *      This checks the start value against the timeout to
 *      determine whether a time out has happened or not
 * \param   startValue[IN] Start time
 * \param   timeout[IN]     Timeout value
 * \return  Bool Whether timed-out or not
 */
Bool PAL_osCheckTimeOut(Uint32 startValue,Int32 timeout);

/**
 * \brief   PAL OS Get Current Tick
 *
 *      This returns the current time in seconds
 * \param   None
 * \return  Uint32 Tick in seconds
 */
Uint32 PAL_osGetCurrentTick(void);
/*@}*/

#endif /* _PAL_OSTIME_H */
