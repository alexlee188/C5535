/*
 * $$$MODULE_NAME pal_osTime.c
 *
 * $$$MODULE_DESC pal_osTime.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/** \file   BIOS_ostime.c
    \brief  BIOS OS time Source File

    This file implements additional OS abstraction services for BIOS.
    The following BIOS services are provided:

    1. time - GEt current systime

    (C) Copyright 2006, Texas Instruments, Inc

    \author     PSG
    \version    0.1 - Nagarjuna Kristam     - Created
 */
#include <psp_common.h>
#include <trc.h>
#include <log.h>
#include <prd.h>

#ifdef __TMS320C55X__
extern LOG_Obj trace;
#else
extern far LOG_Obj trace;
#endif // __TMS320C55X__

/* BIOS Get OS Ticks per mSecs */
Uint32 PAL_osGetCurrentTick(void)
{
    return PRD_getticks();
}

Bool PAL_osCheckTimeOut(Uint32 startValue,Uint32 timeout)
{
    Uint32 checkValue;
    Bool    retVal;
    checkValue = PAL_osGetCurrentTick();
    if ( checkValue < startValue )
    {
        checkValue = (((0xFFFFFFFFU) - startValue) + checkValue) + (1U) ;
    }
    else
    {
        checkValue = checkValue - startValue;
    }

    if (checkValue < timeout)
    {
        retVal  =   FALSE;
    }
    else
    {
        retVal  =   TRUE;
    }
    return  retVal;
}

