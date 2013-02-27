/*
 * $$$MODULE_NAME pal_osprotect.h
 *
 * $$$MODULE_DESC pal_osprotect.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/** \file   pal_osprotect.h
    \brief  OsPROTECT Services Header File

    The OsPROTECT models various levels of reentrancy protection
    for use while writing critical sections in user code. critical
    sections are those parts of user code that needs to run atomically
    in some sense. Meaning, single threading is called for. However,
    the degree of protection sought by user varies based on nature
    of code he is writing.

    It is possible that for some regions of code, user needs ultimate
    degree of protection where all external interrupts are blocked,
    essentially locking out the CPU exclusively for the critical
    section of code. On the other hand user may wish to merely avoid
    thread or task switch from occuring inside said region of code,
    but he may wish to entertain ISRs to run if so required.

    Depending on the underlying OS, the number of levels of protection
    offered may vary. At the least, two basic levels of protection are
    supported --

    - PAL_OSPROTECT_INTERRUPTS - Mask interrupts globally. This has
      real-time implications and must be used with descretion.
      If blocking/unblocking of specific interrupt lines is desired,
      one is reffered to APIs listed in pal_sys.h file.

    - PAL_OSPROTECT_SCHEDULER - Only turns off Kernel scheduler
      completely, but still allows h/w interrupts from being serviced.

    Protection levels 0 to N (max positive Int) are platform specific


    (C) Copyright 2007, Texas Instruments, Inc

    \author     - PSG, TI
    \version    1.0
 */

#ifndef _PAL_OSPROTECT_H_
#define _PAL_OSPROTECT_H_

#include <psp_common.h>

/**
 * \defgroup PalOSProtect PAL OS Protect Interface
 *
 * PAL OS Protect Interface
 * \{
 */

/** \name PAL OS Protect Interface
 *  PAL OS Protect Interface
 * \{
 */

#define PAL_OSPROTECT_INTERRUPT (-1)
#define PAL_OSPROTECT_SCHEDULER (-2)

/**
 * \brief   PAL OS Protect Entry
 *
 *      This function saves the current state of protection in cookie
 *      variable passed by caller. It then applies the requested level
 *      of protection
 * \param   level  [IN] is numeric identifier of the desired degree of protection.
 * \param   cookie [IN] is memory location where current state of protection is
 *      saved for future use while restoring it via PAL_osProtectExit()
 * \note    user is not expected to interpret the cookie in any manner. It
 *      is intended for use in terminating the presently enforced
 *      protection via a matching PAL_osProtectExit() call discssed
 *      later in this file.
 * \return  None
 */
void PAL_osProtectEntry(Int level, Uint32* const cookie);

/**
 * \brief   PAL OS Protect Exit
 *
 *      This function undoes the protection enforced to original state
 *      as is specified by the cookie passed.
 * \param   level  [IN]is numeric identifier of the desired degree of protection.
 * \param   cookie [IN] is original state of protection at time when the
 *      corresponding PAL_osProtectEnter() was called.
 * \return  None
 */
void PAL_osProtectExit(Int level, Uint32 cookie);

/*\}*/
/*\}*/

#endif /* _PAL_OS_PROTECT_H_ */
