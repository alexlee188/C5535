/*
 * $$$MODULE_NAME pal_osmutex.h
 *
 * $$$MODULE_DESC pal_osmutex.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/** \file   pal_osmutex.h
    \brief  OsMUTEX Services Header File

    This file declares OS abstraction services for mutually exclusive
    locks or binary semaphores. All services run in the context of the
    calling thread or program. OsMUTEX does not spawn a thread of its own
    to implement the APIs declared here.

    (C) Copyright 2007, Texas Instruments, Inc

    \author     - PSG, TI
    \version    1.0
 */

#ifndef __PAL_OSMUTEX_H__
#define __PAL_OSMUTEX_H__

#include <psp_common.h>

/**
 * \defgroup PalOSMutex PAL OS Mutex Interface
 *
 * PAL OS Mutex Interface
 * \{
 */

/** \name PAL OS Mutex Interface
 *  PAL OS Mutex Interface
 * \{
 */

/* Blocking call without timeout */
#define PAL_OSMUTEX_NO_TIMEOUT    (-1)

/**
 * \brief   PAL OS Mutex Init
 *
 *      This is an idempotent function that must be called ahead of
 *      calling any other OsMUTEX services. It initializes OsMUTEX internal
 *      data structures and does any book-keep necessary to implement
 *      the published services of OsMUTEX.
 * \param   param [IN] is an arbitrary void* data type used to pass platform
 *      specific initialization information for OsMUTEX. This can be used
 *      to extend OsMUTEX configurability to decisions made at run-time.
 *      This is added for future extensions only.
 * \return  PAL_SOK if successful else a suitable error code.
 */
PAL_Result PAL_osMutexInit(Ptr param);

/**
 * \brief Mutex Attributes
 *
 * Attributes specified during Mutex creation
 */

typedef enum
{
  PAL_OSMUTEX_TYPE_FIFO = 0,
  PAL_OSMUTEX_TYPE_PRIORITY = 1
} PAL_OsMutexType;

typedef Ptr PAL_OsMutexHandle;

typedef struct
{
  PAL_OsMutexType  type;
  Uint32           memSegId;  /**< identifier of memory segment for mutex variables */
} PAL_OsMutexAttrs;

/**
 * \brief   PAL OS Mutex Create
 *
 *      This function creates a mutex with specified attributes
 * \param   name [IN] is char string name of the mutex
 * \param   attrs [IN] is the mutex attributes ex: memory segment from
 *      where this particular mutex object must be allocated from.
 * \param   hMutex [OUT] is location to recieve the handle to just created
 *      mutex object.
 * \return  PAL_SOK if succesful, else a suitable error code.
 */
PAL_Result PAL_osMutexCreate(const Char* name,
                           const PAL_OsMutexAttrs * const attrs,
                           PAL_OsMutexHandle * const hMutex);

/**
 * \brief   PAL OS Mutex Delete
 *
 *      This function deletes or removes the specified mutex
 *      from the system. Associated dynamically allocated memory
 *      if any is also freed up.
 * \param   hMutex [IN] handle to the mutex to be deleted
 * \return  PAL_SOK if succesful else a suitable error code
 */
PAL_Result PAL_osMutexDelete(PAL_OsMutexHandle hMutex);

/**
 * \brief   PAL OS Mutex Lock
 *
 *      This function acquires a lock on the given Mutex. If the
 *      specified mutex is already locked by someone else, the calling
 *      thread will block on this function for a maximum duration of
 *      mSecTimeout. If the mutex continues to be unavailable even
 *      after waiting for specified duration, the function will return
 *      with a suitable error code.
 * \param   hMutex [IN] is the handle of the specified mutex
 * \param   mSecTimeout [IN] is wait time in milliseconds
 * \return  PAL_SOK if successful else a suitable error code
 */
PAL_Result PAL_osMutexLock(PAL_OsMutexHandle hMutex, Int32 mSecTimeout);

/**
 * \brief   PAL OS Mutex Unlock
 *
 *      This function releases an already acquired mutex lock
 * \param   hMutex [IN] is the handle of the specified mutex
 * \return  PAL_SOK if successful else a suitable error code
 */
PAL_Result PAL_osMutexUnlock(PAL_OsMutexHandle hMutex);

/**
 * \brief Mutex Report Data Structure
 *
 * Data structure for Mutex Report service
 */
typedef struct
{
    Char* name;             /**< Name of mutex lock */
    Bool  free;             /**< =FALSE if locked state */
    PAL_OsMutexAttrs attrs; /**< Mutex Attribues */
} PAL_OsMutexReport;

/**
 * \brief   PAL OS Mutex Report
 *
 *      This function reports assorted usage statistics information
 *      regarding the specified mutex
 * \param   hMutex [IN] handle of mutex to be reported on.
 * \param   report [IN/OUT] location where mutex information must be reported
 *      If NULL, structure is not filled.
 * \param   buf [IN/OUT] string buffer where a text formatted report will
 *      be printed for the mutex. If NULL, no text style reporting is done.
 * \return  PAL_SOK if successful, else a suitable error code.
 */
PAL_Result PAL_osMutexReport(const PAL_OsMutexHandle hMutex, PAL_OsMutexReport * const report, Char* const buf);

/*\}*/
/*\}*/

#endif /* _PAL_OSMUTEX_H_ */
