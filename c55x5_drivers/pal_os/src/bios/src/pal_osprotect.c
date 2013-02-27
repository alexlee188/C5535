/*
 * $$$MODULE_NAME pal_osprotect.c
 *
 * $$$MODULE_DESC pal_osprotect.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/** \file   pal_osProtect.c
    \brief  OsPROTECT Services Source File for BIOS

    This file implements OS abstraction services for reentrancy protection.
    For interrupt protection, assembly code is used since BIOS does not provide
    a function that returns previous interrupt status (cpsr).

    (C) Copyright 2006, Texas Instruments, Inc

    \author     PSG

    \version    0.1 - Created       Kapil Bohra
 */
#include <std.h>
#include "pal_osprotect.h"
#include <hwi.h>
#include <tsk.h>


/** \brief PAL OS Protect Entry

    This function is called to start a critical section (from interrupts/scheduler)
    When PAL_OSPROTECT_INTERRUPT is called the returned mask should be passed
    to the corresponding PAL_osProtectExit function. PAL_OSPROTECT_SCHEDULER
    does not take/return any parameters.

    BIOS has constrains and calling constraints for the HWI_disable and
    TSK_disable functions. Refer to BIOS API reference guide for further
    details
*/
void PAL_osProtectEntry(Int level, Uint32* imask)
{
    if (NULL != imask)
    {
        if (level == PAL_OSPROTECT_INTERRUPT)
        {
            *imask = HWI_disable();
        }
        else
        {
            if (level == PAL_OSPROTECT_SCHEDULER)
            {
                TSK_disable();
            }
        }
    }
    return;
}


/** \brief PAL OS Protect Exit

    This function is called to exit from a critical section.
    When PAL_OSPROTECT_INTERRUPT is called the mask returned from the corresponding
    entry function should be passed.PAL_OSPROTECT_SCHEDULER does not take any
    parameter.

    BIOS has constrains and calling constraints for the HWI_restore and
    TSK_enable functions. Refer to BIOS API reference guide for further
    details
*/
void PAL_osProtectExit(Int level, Uint32 imask)
{
    if (level == PAL_OSPROTECT_INTERRUPT)
    {
        HWI_restore(imask);
    }
    else {
        if (level == PAL_OSPROTECT_SCHEDULER)
        {
            TSK_enable();
        }
    }
    return;
}


