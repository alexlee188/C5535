/*
 * $$$MODULE_NAME pal_osSem.c
 *
 * $$$MODULE_DESC pal_osSem.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/** \file   pal_osSem.c
 *   \brief  OsSem Services Source File for BIOS
 *
 *  This file implements OS abstraction services for counting semaphores.
 *  All services run in the context of the calling thread or program.
 *  OsSEM does not spawn a thread of its own to implement the APIs
 *  declared here.
 *
 *  (C) Copyright 2006, Texas Instruments, Inc
 *
 *  \author     NK
 *  \version    0.1 - Created
 *
 */
#include <std.h>
#include <psp_common.h>
#include <pal_osSem.h>
#include <sem.h>

/* To create a semaphore */
PAL_Result PAL_osSemCreate(const char * name,
                           Int32 initVal,
                           const PAL_OsSemAttrs *attrs,
                           PAL_OsSemHandle * const hSem)
{
    SEM_Attrs   temp; /* used for passing the 'name' parameter to SEM_create */
    PAL_Result semCreateResult = PAL_SOK;

    if(NULL == hSem)
    {
        semCreateResult =   PSP_E_RESOURCES;
    }
    else
    {
        temp.name = (String) name;
        *hSem = (PAL_OsSemHandle)SEM_create(initVal, &temp);
        if ( (*hSem) == NULL )
        {
            semCreateResult = PSP_E_RESOURCES;
        }
    }
    return semCreateResult;
}

/* To Delete a semaphore */
PAL_Result PAL_osSemDelete(PAL_OsSemHandle hSem)
{
    PAL_Result semDeleteResult = PAL_SOK;
    SEM_delete(hSem);
    return semDeleteResult;
}

/* To take a semaphore */
PAL_Result PAL_osSemTake(PAL_OsSemHandle hSem, Int32 mSecTimeout)
{
    PAL_Result semTakeResult = PAL_SOK;
    Bool        semPendResult;
    semPendResult = SEM_pend(hSem, mSecTimeout);
    if ( semPendResult == FALSE )
    {
        semTakeResult = PSP_E_RESOURCES;
    }
    else
    {
        semTakeResult = PAL_SOK;
    }
    semTakeResult = PAL_SOK;
    return semTakeResult;
}

/* To give a semaphore */
PAL_Result PAL_osSemGive(PAL_OsSemHandle hSem)
{
    PAL_Result semGiveResult = PAL_SOK;
    SEM_post(hSem);
    return semGiveResult;
}


