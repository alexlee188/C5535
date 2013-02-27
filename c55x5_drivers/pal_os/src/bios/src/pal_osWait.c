/*
 * $$$MODULE_NAME pal_osWait.c
 *
 * $$$MODULE_DESC pal_osWait.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/** \file   pal_osWait.c
    \brief  OsWAIT Services Source File

    This file implements OS abstraction services for programmed interval-waits.

    (C) Copyright 2006, Texas Instruments, Inc

    \author     PSG

    \version    0.1     Created                                     Kapil Bohra
 */

#include <std.h>
#include <tsk.h>
#include <pal_osWait.h>
#include <clk.h>


/* Function to wait for OS Ticks */
PAL_Result PAL_osWaitMsecs(Uint32 mSecs)
{
    Uint32 ticksForSleeping = mSecs;
    TSK_sleep(ticksForSleeping);
    return (PAL_SOK);
}



