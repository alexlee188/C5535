/*
 * $$$MODULE_NAME pal_os.h
 *
 * $$$MODULE_DESC pal_os.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/** \file   pal_os.h
    \brief  OS Abstraction Header File

    This file provides visibility to OS abstraction APIs by including
    only the configured service modules interface files.

    (C) Copyright 2007, Texas Instruments, Inc

    \author     - PSG, TI
    \version    1.0 - Original PSPF header + added generic error codes
                1.1 - Added Timeout error code
                1.2 - Added Timer Services
 */

#ifndef _PAL_OS_H_
#define _PAL_OS_H_

#include "pal_osCfg.h"

#ifdef PAL_INCLUDE_OSMEM
#include <pal_osMem.h>          /* OsMEM Services */
#endif /* PAL_INCLUDE_OSMEM */

#ifdef PAL_INCLUDE_OSBUF
#include <pal_osBuf.h>          /* OsBUF Services */
#endif /* PAL_INCLUDE_OSBUF */

#ifdef PAL_INCLUDE_OSSEM
#include <pal_osSem.h>          /* OsSEM Services */
#endif /* PAL_INCLUDE_OSSEM */

#ifdef PAL_INCLUDE_OSMUTEX
#include <pal_osMutex.h>        /* OsMUTEX Services */
#endif /* PAL_INCLUDE_OSMUTEX */

#ifdef PAL_INCLUDE_OSWAIT
#include <pal_osWait.h>         /* OsWAIT Services */
#endif /* PAL_INCLUDE_OSWAIT */

#ifdef PAL_INCLUDE_OSLIST
#include <pal_osList.h>         /* OsLIST Services */
#endif /* PAL_INCLUDE_OSLIST */

#ifdef PAL_INCLUDE_OSPROTECT
#include <pal_osProtect.h>      /* OsPROTECT Services */
#endif /* PAL_INCLUDE_OSPROTECT */

#ifdef PAL_INCLUDE_OSCACHE
#include <pal_osCache.h>      /* OsCACHE Services */
#endif /* PAL_INCLUDE_OSCACHE */

#ifdef PAL_INCLUDE_OSTIMER
#include <pal_osTimer.h>      /* OsTIMER Services */
#endif /* PAL_INCLUDE_OSTIMER */

/* Added:
 * PAL OS module type to be placed in section D of the final "ERROR CODE"
 * as described in pal_defs.h
 */
#define PAL_OS_COMMON_ERR   (0)
#define PAL_OSMEM_ERR       (1)
#define PAL_OSBUF_ERR       (2)
#define PAL_OSSEM_ERR       (3)
#define PAL_OSMUTEX_ERR     (4)
#define PAL_OSWAIT_ERR      (5)
#define PAL_OSLIST_ERR      (6)
#define PAL_OSPROTECT_ERR   (7)
#define PAL_OSTIMER_ERR     (8)

/* Common error codes for ALL PAL OS modules */
#define PAL_OS_COMMON_ERROR_CREATE(x)   (PAL_ERROR(PAL_CRITICAL_ERROR, PAL_OS_COMMON_ERR, 0, (x)))

/* Invalid parameter passed to the function error */
#define PAL_OS_ERROR_INVALID_PARAM      (PAL_OS_COMMON_ERROR_CREATE(1))

/* Feature not supported error */
#define PAL_OS_ERROR_NOT_SUPPORTED      (PAL_OS_COMMON_ERROR_CREATE(2))

/* No resources available error */
#define PAL_OS_ERROR_NO_RESOURCES       (PAL_OS_COMMON_ERROR_CREATE(3))

/* Timeout from the OS call */
#define PAL_OS_ERROR_TIMEOUT            (PAL_OS_COMMON_ERROR_CREATE(4))

/* OS specific error - not classified, not passed on to the caller */
#define PAL_OS_ERROR_OS_SPECFIC         (PAL_OS_COMMON_ERROR_CREATE(5))


/* Default (memory) segment Id - Many of the modules (like OSSEM, OSBUF), depend upon a
 * segment id to be passed in the API's. The macro below defines a default segment Id
 * that can be used in these API's
 */


/* Default Pool ID - Used when segId=0 is passed for memory alloc/free
 * This macro should be updated if the default pool id is changed
 */
#define PAL_OSMEM_DEFAULT_SEGID         1
#define PAL_OSMEM_NOCACHE_SEGID         2
#define PAL_OSMEM_DMA_ALIGNED_SEGID     3

#endif /* _PAL_OS_H_ */
