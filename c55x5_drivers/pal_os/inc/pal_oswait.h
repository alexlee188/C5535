/*
 * $$$MODULE_NAME pal_oswait.h
 *
 * $$$MODULE_DESC pal_oswait.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/** \file   pal_oswait.h
    \brief  OsWAIT Services Header File

    This file declares OS abstraction services for programmed interval-waits.

    (C) Copyright 2007, Texas Instruments, Inc

    \author     - PSG, TI
    \version    1.0
 */

#ifndef _PAL_OSWAIT_H_
#define _PAL_OSWAIT_H_

#include <psp_common.h>

/**
 * \defgroup PalOSWait PAL OS Wait Interface
 *
 * PAL OS Wait Interface
 * \{
 */

/** \name PAL OS Wait Interface
 *  PAL OS Wait Interface
 * \{
 */

/**
 * \brief   PAL_osWaitMsecs()
 *
 *      This function leverages OS implemented "wait" to delay
 *      further execution of current thread for specified milliseconds
 *      period of time.
 * \note    The currently running thread might be preempted and placed
 *      on scheduler's wait queue for the specified duration of time.
 *      If a more effecient (w/o context switch overhead) wait is required
 *      at finer time granularity (order of microseconds), please use
 *      SysWAIT services defined in pal_sys.h file
 * \note    The function will do any milliseconds-to-ticks conversion
 *      as appropriate for implementing the wait using underlying
 *      OS supported APIs.
 * \param   mSecs [IN] is the duration in milliseconds to wait for
 * \return  PAL_SOK if succesful else a suitable error code
 */
PAL_Result PAL_osWaitMsecs(Uint32 mSecs);

/**
 * \brief   PAL_osWaitTicks()
 *
 *      This function leverages OS implemented "wait" to delay
 *      further execution of current thread for specified number
 *      of operating system ticks.
 * \note    The currently running thread might be preempted and placed
 *      on scheduler's wait queue for the specified duration of time.
 *      If a more effecient (w/o context switch overhead) wait is required
 *      at finer time granularity (order of microseconds), please use
 *      SysWAIT services defined in pal_sys.h file
 * \param   ticks [IN] is the number of operating system ticks to wait for
 * \return  PAL_SOK if succesful else a suitable error code
 */
PAL_Result PAL_osWaitTicks(Uint32 ticks);

/*\}*/
/*\}*/

#endif /* _PAL_OSWAIT_H_ */
