/*
 * $$$MODULE_NAME pal_ostimer.h
 *
 * $$$MODULE_DESC pal_ostimer.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/** \file   pal_ostimer.h
    \brief  OsTIMER Services Header File

    This file declares OS abstraction services for TIMER.
    All services run in the context of the calling thread or program.
    OsTimer does not spawn a thread of its own to implement the APIs
    declared here. The registered callbacks are called from the Timer interrupt
    context, hence OS specific restrictions apply to what can be called from
    that callback function.

    (C) Copyright 2007, Texas Instruments, Inc

    \author     - PSG, TI
    \version    1.0
 */

#ifndef _PAL_OSTIMER_H_
#define _PAL_OSTIMER_H_

#include <psp_common.h>

/**
 * \defgroup PalOsTimerMain PAL OS TIMER Interface
 *
 * PAL OS TIMER Interface
 * @{
 */

/** Max size of the name given to the Timer */
#define PAL_OS_TIMER_NAME_SIZE  16u

/**
 * \brief Handle to a Timer object
 * This is updated by the Create API and is to be used
 * for subsequent Timer services
 */
typedef Ptr PAL_osTimerHandle;

/**
 * \brief Callback Parameters
 * The Timer callback function will be passed a pointer to
 * a structure of this datatype.
 */
typedef struct {
    /** Handle to the Timer which called this callback */
    PAL_osTimerHandle hTimer;
    /** Data which was passed to the Create API */
    Ptr cbData;
} PAL_osTimerCallbackParams;

/**
 * \brief Callback Function
 * This callback function will be called from the interrupt context
 * So corresponding OS limitations apply on what calls may be made
 * from within this callback function
 */
/*added const to remove misrac warning from storage drivers*/
typedef void (*PAL_osTimerCallback) (PAL_osTimerCallbackParams const *);

/**
 * \brief Timer operation mode
 */
typedef enum {
    /** ONESHOT = Run once and stop after calling callback function */
    PAL_OS_TIMER_MODE_ONESHOT,
    /**
     * PERIODIC = Run repeatedly with a time period as specified in
     *            the Create API.  At the end of each such time period
     *            call the callback function
     */
    PAL_OS_TIMER_MODE_PERIODIC,
    /** Invalide Mode - used by the driver during Delete API */
    PAL_OS_TIMER_MODE_INVALID
} PAL_osTimerMode;

/**
 * \brief Operational state of Timer
 * The value is the same as that returned by the OS call
 */
typedef enum {
    /** Timer is Non-operational */
    PAL_OS_TIMER_INACTIVE,
    /** Timer is operational */
    PAL_OS_TIMER_ACTIVE
} PAL_osTimerOperState;

/**
  * \brief Structure used to report Timer information
  */
typedef struct {
    /** Name of the Timer */
    Char name[PAL_OS_TIMER_NAME_SIZE];
    /** Operational State of the Timer */
    PAL_osTimerOperState operState;
    /** Number of milliseconds remaining before next activation */
    Uint mSecsRemaining;
} PAL_osTimerReport;

/*+-------------------------------------------------------------------------+*/
/**
 * \defgroup PalOsTimerCoreProcess Core Processing APIs
 *
 * PAL OS TIMER Core Processing APIs
 * @{
 */

/**
 * \brief Timer instance creation
 *
 * Use this API to create an instance of the timer.
 *
 * \param name          [IN]     Name of the Timer - for bookkeeping and debugging
 * \param mSecsTimeout  [IN]     Number of milliseconds before timeout occurs
 * \param mode          [IN]     ONESHOT / PERIODIC
 * \param cbFunc        [IN]     Callback function which will be
 *                               called after timeout
 * \param cbData        [IN]     User data which will be relayed to
 *                               the callback function cbFunc
 * \param hTimer        [IN/OUT] Pointer to Handle to Timer instance
 *
 * \return PAL_SOK or PAL Error code
 */
PAL_Result PAL_osTimerCreate (const char * const name, Uint mSecsTimeout, PAL_osTimerMode mode,
                            PAL_osTimerCallback cbFunc, Ptr cbData, PAL_osTimerHandle *const hTimer);



/**
 * \brief Timer instance deletion
 *
 * Use this API to delete an instance of the timer.
 *
 * \param hTimer            [IN]    Handle to Timer instance
 *
 * \return PAL_SOK or PAL Error code
 */
PAL_Result PAL_osTimerDelete (PAL_osTimerHandle hTimer);


/**
 * \brief Timer instance start
 *
 * Use this API to start the instance of the timer.
 *
 * \param hTimer            [IN]    Handle to Timer instance
 *
 * \return PAL_SOK or PAL Error code
 */
PAL_Result PAL_osTimerStart (PAL_osTimerHandle hTimer);

/**
 * \brief Timer instance stop
 *
 * Use this API to stop the instance of the timer
 * The Timer retains the value of the current tick count and will
 * resume counting from this current tick value when PAL_sysTimerStart()
 * is called again.
 *
 * \param hTimer            [IN]    Handle to Timer instance
 *
 * \return PAL_SOK or PAL Error code
 */
PAL_Result PAL_osTimerStop (PAL_osTimerHandle hTimer);

/* @} PalOsTimerCoreProcess */


/*+-------------------------------------------------------------------------+*/
/**
 * \defgroup PalOsTimerSuppleProcess Supplemental Processing APIs
 *
 * PAL OS TIMER Supplemental Processing APIs
 * @{
 */

/**
 * \brief Timer instance creation
 *
 * Use this API to create an instance of the timer.
 *
 * \param hTimer        [IN]    Pointer to Handle to Timer instance
 * \param newTimeout    [IN]    Number of milliseconds before timeout occurs
 * \param newMode       [IN]    ONESHOT / PERIODIC
 * \param newCbFunc     [IN]    Callback function which will be
 *                              called after timeout
 * \param newCbData     [IN]    User data which will be relayed to
 *                              the callback function cbFunc
 *
 * \return PAL_SOK or PAL Error code
 */
PAL_Result PAL_osTimerSetParams (PAL_osTimerHandle hTimer, Uint newTimeout, PAL_osTimerMode newMode,
                                PAL_osTimerCallback newCbFunc, Ptr newCbData);
/* @} PalOsTimerSuppleProcess */

/**
 * \brief Reporting function
 *
 * Use this API to get a report of an existing timer instance in the system
 *
 * \param hTimer    [IN]        Handle to Timer instance
 * \param report    [IN/OUT]    Reporting structure which will be updated by
 *                              this function
 *
 * \return PAL_SOK or PAL Error code
 */
PAL_Result PAL_osTimerGetReport (PAL_osTimerHandle hTimer, PAL_osTimerReport *const report);

/*@} PalOsTimerMain */

#endif /* _PAL_OSTIMER_H_ */
