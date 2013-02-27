/*
 * $$$MODULE_NAME pal_osmem.h
 *
 * $$$MODULE_DESC pal_osmem.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/** \file   pal_osmem.h
    \brief  OsMEM Services Header File

    This file declares OS abstraction services for variable length heap
    memory management operations. The PAL module implementing these APIs
    is called OsMEM

    (C) Copyright 2007, Texas Instruments, Inc

    \author     - PSG, TI
    \version    1.0
 */

#ifndef _PAL_OSMEM_H_
#define _PAL_OSMEM_H_

#include <psp_common.h>
#ifndef BIOS_BUILD
#include "itron.h"      /* Include TRON header to get types.h and kernel.h in PrOS */
#endif

/**
 * \defgroup PALOSMem PAL OS Memory Interface
 *
 * PAL OS Memory Interface
 * @{
 */

/** \name PAL OS Memory Interface
 *  PAL OS Memory Interface
 * @{
 */

/** \name PAL OS Memory Cache Alignment Size
 *  PAL OS Memory Cache Alignment Size
 * @{
 */
#define PAL_OS_MEM_CACHE_LINE_SIZE      32

/**
 * \brief PAL OS Memory Init Config
 *
 * Specifies type of memory
 */
typedef struct
{
    Uint32  palOsMemSegId;              /**< Memory seg id for prOS */
    Uint32 memId;                       /**< Memory seg id for user */
} PAL_OsMemInitConfig;

/**
 * \brief PAL OS Memory Address Space
 *
 * Specifies type of memory address space
 */
typedef enum
{
    PAL_OSMEM_ADDR_PRG      = 0,    /**< Program only address space */
    PAL_OSMEM_ADDR_DAT      = 1,    /**< Data only address */
    PAL_OSMEM_ADDR_IO       = 2,    /**< I/O only space address */
    PAL_OSMEM_ADDR_PRGDAT   = 3,    /**< Unified prog-data */
    PAL_OSMEM_ADDR_PRGIO    = 4,    /**< Unified prog-io */
    PAL_OSMEM_ADDR_DATIO    = 5,    /**< Unified data-io */
    PAL_OSMEM_ADDR_UNIFIED  = 6,    /**< Homogeneous, unified prog/data/io memory */
    PAL_OSMEM_ADDR_SPECIAL  = 7     /**< Special or un-classified address range */
} PAL_OsMemAddrSpace;

/**
 * \brief PAL OS Memory attributes
 *
 * Memory attributes
 */
typedef struct
{
  PAL_OsMemAddrSpace addrSpace;
} PAL_OsMemAttrs;

/**
 * \brief PAL OS Memory Report Attributes
 *
 * PAL OS Memory Report attributes
 */
typedef struct
{
  Int segBytesSz;           /**< Segment size in bytes */
  Int segBytesUsed;         /**< Number of bytes allocated */
  Int numBufs;              /**< Number of buffers allocated */
  Int maxLengthFree;        /**< Max contigious bytes free */
  Int maxLengthAllocated;   /**< Max contigious bytes allocated */
} PAL_OsMemReport;

/**
 * \brief PAL OS MEM Init
 *
 * This is an idempotent function that must be called ahead of
 * calling any other OsMEM services. It initializes OsMEM internal
 * data structures (ex: segment tables) and does any book-keep
 * necessary to implement the published services of OsMEM.
 *
 * \param   param [IN]  is an arbitrary void* data type used to pass platform
 *                      specific initialization information for OsMEM. This can
 *                      be used to extend OsMEM configurability to decisions made
 *                      at run-time. This is added for future extensions only.
 *
 * \return              PAL_SOK if successful else a suitable error code.
 */

PAL_Result PAL_osMemInit(Ptr param) ;

/**
 * \brief PAL OS Memory Segment Define
 *
 * This function defines a segment of memory at specified
 * start address with given attributes. Once defined, user can allocate
 * variable length memory buffers at desired address alignments
 * via the PAL_osMemAlloc() call.
 *
 * \param   name [IN]       Name of memory segment being defined
 * \param   startAddr [IN]  The start address (byte granular) of memory segment
 * \param   numBytes [IN]   The length in bytes of the memory segment
 * \param   attrs [IN]      Optional memory attributes characterizing the
 *                          region of memory being defined. Ex: Program or
 *                          Data memory etc.,
 * \param   segId [OUT]     Location to recieve the numeric identifier of the
 *                          just defined memory segment.
 * \return  PAL_SOK if successful, else a suitable error code
 */
PAL_Result PAL_osMemSegDefine(const char* name,
                              Uint32 startAddr,
                              Uint32 numBytes,
                              PAL_OsMemAttrs * const attrs,
                              Uint32 * const segId);

/**
 * \brief PAL OS Memory Segment Undefine
 *
 * This function undefines specified segment of memory. Once
 * undefined, the region is no longer available for PAL_osMemAlloc().
 *
 * \param   segId [IN] Identifier of the memory segment being undefined.
 * \return  PAL_SOK if successful, else a suitable error code
 */
PAL_Result PAL_osMemSegUndefine(Uint32 segId);

/**
 * \brief PAL OS Memory Alloc
 *
 * This function allocates specified length of memory from
 * the given memory segment.
 * \note The memory segment must already be defined before calling this API.
 * \note Alignment = 0 or 1 results is NO special alignments being done. A
 * value of 2 forces the buffer to start at even byte boundary
 *
 * \param   segId [IN]      Identifier of the hosting memory segment
 * \param   numBytes [IN]   The length in bytes of buffer being allocated
 * \param   alignment [IN]  A power-of-2 alignment constraint specifier.
 *                          If non-zero, OsMEM will allocate specified length
 *                          of memory ensuring that it is aligned to an
 *                          address boundary as specified.
 * \param   memAddr [OUT]   Address of just allocated memory buffer
 * \return  PAL_SOK if successful, else a suitable error code
 */
PAL_Result PAL_osMemAlloc(Uint32 segId,
                          Uint32 numBytes,
                          Uint16 alignment,
                          Ptr * const memAddr);

/**
 * \brief PAL OS Memory Free
 *
 * This function counters the PAL_osMemAlloc() call in that it frees
 * the specified memory buffer and returns it to the hosting memory
 * segment for others to allocate.
 * \note    The memory segment must already be defined before calling this API.
 *
 * \param   segId [IN]      Identifier of the hosting memory segment
 * \param   memAddr [IN]    The start address of memory buffer being freed.
 * \param   numBytes [IN]   The length in bytes of buffer being freed.
 * \return  PAL_SOK if successful, else a suitable error code
 */
PAL_Result PAL_osMemFree(Uint32 segId, Ptr memAddr, Uint32 numBytes);

/**
 * \brief PAL OS Memory Copy
 *
 * This function copies specified number of bytes from give source
 * address to given destination address
 * \note The user is responsible of making sure that adequate free memory
 * is indeed available at the specified destination address.
 *
 * \param   dest [IN/OUT]   Address of destination buffer
 * \param   src [IN]        Address of buffer to copy data from
 * \param   numBytes [IN]   The number of contigious bytes to copy
 * \return  PAL_SOK if successful, else a suitable error code
 */
PAL_Result PAL_osMemCopy(Ptr dest, const Ptr src, Uint32 numBytes);

/**
 * \brief PAL OS Memory Set
 *
 * This function stamps the specified region of memory with
 * the given bit pattern (fill character).
 *
 * \param   memAddr [IN/OUT]    Address of memory region to be stamped with
 *                              fill character
 * \param   fillVal [IN]        Address of buffer to copy data from
 * \param   numBytes [IN]       The number of contigious bytes to fill
 * \return  PAL_SOK if successful, else a suitable error code
 */
PAL_Result PAL_osMemSet(Ptr memAddr, Char fillVal, Uint32 numBytes);

/**
 * \brief PAL OS Memory Lock
 *
 * This function locks out the specified region of memory from
 * any page-swap operations effected by OS virtual memory manager.
 * Once locked, the region of memory will stay resident untill
 * its unlocked via PAL_osMemUnlock().
 *
 * \param   memAddr [IN]    Address of memory region to be locked from VM opserations
 * \param   byteLen [IN]    Length of memory region in bytes
 * \param   cookie [OUT]    Space to recieve an arbitrary platform specific data
 *                          associated with just performed locking. User is not
 *                          expected to interpret it any way. It is intended to
 *                          be passed as-is during matching unlocking operation
 *                          via PAL_osMemUnlock().
 * \return  PAL_SOK if successful, else a suitable error code
 */
PAL_Result PAL_osMemLock(Ptr memAddr,
                         Uint32 byteLen,
                         Uint32 *const cookie);

/**
 * \brief PAL OS Memory Un-Lock
 *
 * This function counter to PAL_osMemLock() in that it unlocks the
 * specified region of memory thereby exposing it to any swaps
 * performed by OS virtual memory manager.
 *
 * \param   memAddr [IN]    Address of memory to be unlocked for VM operations
 * \param   byteLen [IN]    Length of memory region in bytes
 * \param   cookie [IN]     Platform specific data obtained when the specific
 *                          memory region was locked by a corresponding
 *                          PAL_osMemLock() function
 * \return  PAL_SOK if successful, else a suitable error code
 */
PAL_Result PAL_osMemUnlock(Ptr memAddr,
                           Uint32 byteLen,
                           Uint32 * const cookie);

/**
 * \brief PAL OS Memory Virtual To Physical
 *
 * This function is relevant in platforms where OS is running
 * a virtual memory manager. It inputs a logical or virtual
 * memory address (as seen by application) and returns the
 * corresponding physical memory address (as seen by the h/w
 * devices).
 * \note A macro equivalent of this API will also be available
 *
 * \param   virtAddress [IN] The virtual memory address
 * \return  A 32bit unsigned physical memory address.
 */
Uint32 PAL_osMemVirt2Phy(Ptr virtAddress);

/**
 * \brief PAL OS Memory Physical To Virtual
 *
 * This function is relevant in platforms where OS is running
 * a virtual memory manager. It inputs a physical memory
 * address (as seen by the h/w devices) and returns the corresponding
 * virtual memory address (as seen by the application).
 * \note A macro equivalent of this API will also be available
 *
 * \param   phyAddress [IN] The physical memory address
 * \return  Virtual memory address (unadorned void* type)
 */
Ptr PAL_osMemPhy2Virt(Uint32 phyAddress);



/**
 * \brief PAL OS Memory Report
 *
 * This function reports assorted usage statistics information
 * regarding the specified memory segment
 *
 * \param   segId [IN]      Identifier of memory segment to report statistics
 * \param   report [IN/OUT] Location where information must be reported. If
 *                          NULL, structure is not filled.
 * \param   buf [IN/OUT]    String buffer where a text formatted report will
 *                          be printed. If NULL, no text reporting is done
 * \return  PAL_SOK if successful, else a suitable error code.
 */
PAL_Result PAL_osMemReport(const Uint32 segId, PAL_OsMemReport * const report, Char* const buf);

/*@}*/
/*@}*/

#endif /* _PAL_OSMEM_H_ */
