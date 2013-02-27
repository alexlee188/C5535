/*
 * $$$MODULE_NAME pal_osbuf.h
 *
 * $$$MODULE_DESC pal_osbuf.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/** \file   pal_osbuf.h
    \brief  OsBUF Services Header File

    This file declares OS abstraction services for fixed length buffer
    memory management operations. The PAL module implementing these APIs
    is called OsBUF.

    The fixed size buffer pool is carved out of a predefined variable
    length memory segment constructed using OsBUF. With OsBUF, user
    can partition his memory such that each partition has a specified
    number of buffer chunks, each of which is exactly the same size.

    A given memory segment can host more than one buffer pool each of
    which is defined for a distinct fixed size buffer. Example, one
    could have 2 buffer pools in a given memory segment, one with buffers
    of length 512 bytes and other with buffers of 1024 bytes length.

    (C) Copyright 2007, Texas Instruments, Inc

    \author     - PSG, TI
    \version    1.0
 */

#ifndef __PAL_OSBUF_H__
#define __PAL_OSBUF_H__

#include "psp_common.h"

/**
 * \defgroup PalOSBuf PAL OS Buffer Interface
 *
 * PAL OS Buffer Interface
 * @{
 */

/** \name PAL OS Buffer Interface
 *  PAL OS Buffer Interface
 * @{
 */

/**
 * \brief   PAL OS Buffer Init
 *
 *      This is an idempotent function that must be called ahead of
 *      calling any other OsBUF services. It initializes OsBUF internal
 *      data structures (ex: segment tables) and does any book-keep
 *      necessary to implement the published services of OsBUF.
 * \param   param [IN] is an arbitrary void* data type used to pass platform
 *      specific initialization information for OsBUF. This can be used
 *      to extend OsBUF configurability to decisions made at run-time.
 *      This is added for future extensions only.
 * \return  PAL_SOK if successful else a suitable error code.
 */
PAL_Result PAL_osBufInit(Ptr param);

typedef struct
{
  Int   alignment; /* power-of-2 byte address boundary */
} PAL_OsBufAttrs;

/**
 * \brief   PAL OS Buffer Pool Define
 *
 *      This function defines a pool of fixed size buffers within
 *      the specified memory segment with given attributes.
 *      Once defined, user can acquire and release buffers from
 *      this pool via PAL_osBufAlloc() and PAL_osBufFree() respectively.
 * \note    The memory segment identified by segId must already exist before
 *      calling PAL_osBufPoolDefine()
 *
 * \param   name [IN] is name of buffer pool being defined
 * \param   segId [IN] is the hosting memory segment identifier for the buffer pool.
 * \param   maxBufs [IN] is the max number of buffers to be constructed in the pool
 * \param   bytesPerBuf [IN] is size of each buffer in bytes
 * \param   attrs is optional buffer pool attributes to be applied while creating
 *      buffer pool. Ex: Address alignment constraints for individual buffers
 * \param   pBufPoolId [OUT] is location to recieve the numeric identifier of the
 *      just defined fixed size buffer pool.
 * \return  PAL_SOK if successful, else a suitable error code
 */
PAL_Result PAL_osBufPoolDefine(const char* const name,
                Uint32 segId,
                Uint maxBufs,
                Uint32 bytesPerBuf,
                const PAL_OsBufAttrs * const attrs,
                Uint32 * const pBufPoolId);

/**
 * \brief   PAL OS Buffer Pool Undefine
 *
 *      This function undefines specified buffer pool. Once
 *      undefined, the pool is no longer available for PAL_osBufAlloc().
 * \param   bufPoolId [IN] is identifier of the buffer pool being undefined.
 * \return  PAL_SOK if successful, else a suitable error code
 */
PAL_Result PAL_osBufPoolUndefine(Uint32 bufPoolId);

/**
 * \brief   PAL OS Buffer Alloc
 *
 *      This function acquires a buffer from the specified buffer pool
 * \param   bufPoolId [IN] is identifier of the buffer pool
 * \param   bufAddr [OUT] Address of just acquired fixed size buffer
 * \note    Each buffer has alignment enforced during buffer pool creation itself.
 * \return  PAL_SOK if successful, else a suitable error code
 */
PAL_Result PAL_osBufAlloc(const Uint32 bufPoolId, Ptr * const bufAddr);

/**
 * \brief   PAL OS Buffer Free
 *
 *      This function counters the PAL_osBufAlloc() call in that it returns
 *      the specified fixed size buffer back to the buffer pool
 * \param   bufPoolId [IN] is identifier of the buffer pool.
 * \param   bufAddr [IN] is the start address of buffer being freed.
 * \return  PAL_SOK if successful, else a suitable error code
 */
PAL_Result PAL_osBufFree(Uint32 bufPoolId, Ptr bufAddr);

/**
 * \brief Buffer Report Data Structure
 *
 * Data structure for Bufer Report service
 */
typedef struct
{
  Uint32 bufBytesSz;        /* Buffer size in bytes */
  Uint32 poolBytesSz;       /* Buffer pool size in bytes */
  Uint   maxBufs;           /* max number of buffers in pool */
  Uint   bufsUsed;      /* number of buffers allocated */
} PAL_OsBufReport;
/**
 * \brief   PAL OS Buffer Report
 *
 *      This function reports assorted usage statistics information
 *      regarding the specified buffer pool
 * \param   bufPoolId [IN] identifier of buffer pool to report statistics on
 * \param   report [IN/OUT] location where information must be reported
 *      If NULL, structure is not filled.
 * \param   buf [IN/OUT] string buffer where a text formatted report will
 *      be printed. If NULL, no text style reporting is done.
 * \return  PAL_SOK if successful, else a suitable error code.
 */
PAL_Result PAL_osBufReport(const Uint32 segId, PAL_OsBufReport * const report, Char * const buf);

/*@}*/
/*@}*/

#endif /* _PAL_OSBUF_H_ */
