/*
 * $$$MODULE_NAME pal_oscache.h
 *
 * $$$MODULE_DESC pal_oscache.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/** \file   pal_oscache.h
    \brief  OsLIST Services Header File

    This file declares APIs for handling cache functionality.

    CONFIGURATION OPTIONS:
        1. Defining PSP_ARM_USE_WRITEBUFFER enables writebuffer functionality.

    \author     - PSG, TI
    \version    1.0 - Original from PSPF1.0
                1.1 - Added macro version (non-protected)
                1.2 - Converted macro version to functions so that return value can be obtained
                      Thus at this stage no macros supported now
                1.3 - Add a few more API's
 */

#ifndef __PAL_OS_CACHE_H__
#define __PAL_OS_CACHE_H__

#include "psp_common.h"

/**
 * \defgroup PalOSCache PAL OS Cache Interface
 *
 * PAL OS Cache Interface
 * \{
 */

/** \name PAL OS Cache Interface
 *  PAL OS Cache Interface
 * \{
 */

 typedef enum
 {
    PAL_osCache_NOT_DEFINED
 } PAL_osCacheMemAddrSpace;



/**
 * \brief   PAL OS Cache Invalidate
 *
 * This function invalidates the D cache.
 *
 *  NOTE : cache is NOT USED in this API
 *  \parm   cache         [IN]      Cache address space
 *  \param  mem_start_ptr [IN]      Starting adress of memory.
 *                                  Please note that this should be
 *                                  32 bytes alinged.
 *  \param  num_bytes     [IN]      length of buffer
 * \return  nil return value
 */
void PAL_osCacheInvalidate(PAL_osCacheMemAddrSpace  cache,
                           Uint32                   mem_start_ptr,
                           Uint32                   num_bytes);

/**
 * \brief   PAL OS Cache Flush
 *
 * This function flushes (cleans) the Cache
 *
 *  NOTE : cacheAddrSpace is not cared in this API
 *  \parm   cache  [IN]      Cache address space
 *  \param  mem_start_ptr [IN]  Starting adress of memory. Please note that this should be 32 bytes alinged.
 *  \param  num_bytes [IN]      length of buffer
 * \return  nil return value
 */
void PAL_osCacheFlush(PAL_osCacheMemAddrSpace   cache,
                      Uint32                    mem_start_ptr,
                      Uint32                    num_bytes);


/**
 * \brief   PAL OS Cache Flush and Invalidate
 *
 * This function flushes (cleans) and invalidates the Cache
 *
 *  NOTE : cacheAddrSpace is not cared in this API
 *  \parm   cache     [IN]      Cache address space
 *  \param  mem_start_ptr [IN]      Starting adress of memory. Please note that this should be 32 bytes alinged.
 *  \param  num_bytes [IN]         length of buffer
 * \return  nil
 */
void PAL_osCacheFlushAndInvalidate(PAL_osCacheMemAddrSpace  cache,
                                   Uint32                   mem_start_ptr,
                                   Uint32                   num_bytes);


/**
 * \brief   PAL OS Set Cache line Length
 *
 * This function sets the cache line
 *
 *  NOTE : cache is not cared in this API
 *  \parm   cache   [IN]      Cache address space
 *  \param  length  [IN]     Cache line Length
 * \return nil
 */
void PAL_osCacheSetCacheLineLength(PAL_osCacheMemAddrSpace  cache,
                                   Uint8                    length);

/**
 * \brief   PAL OS GEt Cache line Length
 *
 * This function returns the length of cache line
 *
 *  NOTE : cacheAddrSpace is not cared in this API
 *  \parm   cache   [IN]      Cache address space
 * \return  Cache Length
 */
Uint8 PAL_osCacheGetCacheLineLength(PAL_osCacheMemAddrSpace cache);


/**
 *  \brief Drain the write buffer
 *
 *  This function Drain the write buffer
 *
 *  NOTE : cache is not cared in this API
 *  \parm   cache      [IN]   Cache address space
 *  \param  waitForInt [IN]  if TRUE -- Drains the writebuffer and put processor in low
 *                                    power mode till next interrupts occurs.
 *                          if FALSE -- Drains the writebuffer and stops excuting further
 *                                  till buffer is drained.
 *  \return void
 */
void PSP_cacheDrainWriteBuffer(PAL_osCacheMemAddrSpace  cache,
                               Bool                     waitForInt);


/*\}*/

/*\}*/

#endif /* __PAL_OS_CACHE_H__ */
