/*
 * $$$MODULE_NAME pal_osCache.c
 *
 * $$$MODULE_DESC pal_osCache.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/** \file   pal_osinvalidate.c
    \brief  Cache Services Source File for BIOS

    This file implements Cache functionalities.

    (C) Copyright 2006, Texas Instruments, Inc
    \author     PSG

    \version    0.1 - Created       Kapil Bohra
    \version    0.2 - Modified      Kapil Bohra
 */

#include <std.h>
#include <bcache.h>
#include <pal_oscache.h>

#define CACHE_WAIT          (TRUE)
#define PALOS_CACHE_DEBUG   0       /* Set to 1 to enable printf for debugging */

/** \brief Defines the cache line length
 *
 *  Used for storing the MAX of L1D/L2 Cache
 */
static Uint8 palosCacheLineLength = 0;


#if PALOS_CACHE_DEBUG
static __inline Int checkCacheLine (Uint32 mem_start_ptr)
{
    Int value = 0;

    if((mem_start_ptr & (palosCacheLineLength - 1)) !=  0)
    {
        value = 1;
    }

    return value;
}
#endif


/**
 *  \brief Clean the Cache
 *
 *  This function clean the D Cache in MVA way.
 *
 *  \param  cache           [IN]        Not used
 *  \param  mem_start_ptr   [IN]        Starting adress of memory. Please note
 *                                      that this should be 32 bytes alinged.
 *  \param  num_bytes       [IN]        Length of buffer
 */
void PAL_osCacheFlush(PAL_osCacheMemAddrSpace   cache,
                        Uint32                  mem_start_ptr ,
                        Uint32                  num_bytes)
{
    /* To avoid compiler warning */
    cache = cache;
#if PALOS_CACHE_DEBUG
    if (checkCacheLine(mem_start_ptr)) {
        printf("\r\n -- FLUSHING UNALIGNED BUFFERS --");
    }
#endif
    BCACHE_wb ((Ptr)mem_start_ptr, num_bytes, CACHE_WAIT);
}


/**
 * \brief   PAL OS Cache Invalidate
 *
 * This function invalidates the D cache.
 *
 *  NOTE : cacheAddrSpace is not cared in this API
 * \parm   cacheAddrSpace   [IN]        Cache address space
 * \param  mem_start_ptr    [IN]        Starting adress of memory. Please note that
 *                                      this should be 32 bytes alinged.
 * \param  num_bytes        [IN]        Length of buffer
 * \return  nil return value
 */
void PAL_osCacheInvalidate(PAL_osCacheMemAddrSpace  cache,
                           Uint32                   mem_start_ptr,
                           Uint32                   num_bytes)
{
    /* To avoid compiler warning */
    cache = cache;
#if PALOS_CACHE_DEBUG
    if (checkCacheLine(mem_start_ptr)) {
        printf("\r\n -- INVALIDATING UNALIGNED BUFFERS --");
    }
#endif
    BCACHE_inv((Ptr)mem_start_ptr, num_bytes, CACHE_WAIT);
}


 /**
 *  \brief invalidate the Cache
 *
 *  This function invalidate the D Cache in MVA way
 *
 *  \param  mem_start_ptr   [IN]        Starting adress of memory. Please note
 *                                      that this should be 32 bytes alinged.
 *  \param  len             [IN]        Length of buffer
 */
void PAL_osCacheFlushAndInvalidate(PAL_osCacheMemAddrSpace  cache,
                                    Uint32 mem_start_ptr,
                                    Uint32 num_bytes)
{
    /* To avoid compiler warning */
    cache = cache;
#if PALOS_CACHE_DEBUG
    if (checkCacheLine(mem_start_ptr)) {
        printf("\r\n -- FLUSH AND INVALIDATING UNALIGNED BUFFERS --");
    }
#endif
    BCACHE_wbInv ((Ptr)mem_start_ptr, num_bytes, CACHE_WAIT);
}


/**
 * \brief   PAL OS Set Cache line Length
 *
 *
 * \parm   cacheAddrSpace   [IN]    Not Used
 * \param  Cache            [IN]    Not Used
 *
 * Cache line length is calculated using the BIOS APIs. This API should be
 * called before calling other PAL_osCache APIs.
 *
 * \return  nil return value
 */
void PAL_osCacheSetCacheLineLength(PAL_osCacheMemAddrSpace  cache,
                                   Uint8                    length)
{
    BCACHE_Size size;

    /* L1D/L2 cache can have different cache line size. So get the
       cache line sizes for both the caches and set the MAX of these two */
    BCACHE_getSize(&size);

    if (size.l1dsize > size.l2size) {
        palosCacheLineLength = (Uint8)size.l1dsize;
    } else {
        palosCacheLineLength = (Uint8)size.l2size;
    }
}



/**
 * \brief   PAL OS GEt Cache line Length
 *
 * Returns the current Cache Line size (bigger of L1D or L2 Cache line size)
 *
 * \parm   cacheAddrSpace[IN]      Cache address space (not used)
 *
 * \return  Cache Length
 */
Uint8 PAL_osCacheGetCacheLineLength(PAL_osCacheMemAddrSpace cache)
{

    return (palosCacheLineLength);
}


/**
 *  \brief Drain the write buffer
 *
 *  Function does nothing. Implemented for consistance.
 *
 *  NOTE : cacheAddrSpace is not cared in this API
 *  \parm   cacheAddrSpace[IN]      Cache address space
 *  \param  waitForInt [IN]        if TRUE -- Drains the writebuffer and put processor in low
 *                                            power mode till next interrupts occurs.
 *                                 if FALSE -- Drains the writebuffer and stops excuting further
 *                                            till buffer is drained.
 */
void PSP_cacheDrainWriteBuffer(PAL_osCacheMemAddrSpace  cache,
                               Bool                     waitForInt)
{
    /* Does Nothing */
    /* Added to remove warnings */
    cache = cache;
    waitForInt = waitForInt;

}
