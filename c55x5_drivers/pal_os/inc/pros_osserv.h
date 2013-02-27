/*
 * $$$MODULE_NAME pros_osserv.h
 *
 * $$$MODULE_DESC pros_osserv.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/** \file   pros_osserv.h
    \brief  PrOS OS Service Header File

    This file declares additional OS abstraction services for PrOS. The following
    PrOS services are provided:

    1. Task - Create, Delete, Sleep
    2. Define Timer service

    (C) Copyright 2007, Texas Instruments, Inc

    \author     - PSG, TI
    \version    1.0
 */
/**
 * \defgroup PrOS_OsServ PrOS OS Service Interface
 *
 * PrOS OS Service Interface
 * @{
 */

#ifndef _PROS_OSSERV_H_
#define _PROS_OSSERV_H_

#include <psp_common.h>
#include <itron.h>




/**
 * \brief PrOS Get OS Ticks per MilliSeconds
 *
 * This function returns the OS ticks for given milliSeconds.
 *
 * NOTE: This function will round off the number of ticks if the number of mSecs
 * \param   mSecs [IN] milliSeconds input
 * \return  OS ticks equivalent to given milliSeconds
 */
Uint32 PrOS_OsGetOsTicksPerMSecs(Uint32 mSecs);

/**
 * \brief PrOS Get MilliSeconds per OS Tick
 *
 * This function returns the number of milliSeconds per one OS Tick
 *
 * \param   None
 * \return  ticksPerMsecs is the number of OS ticks per mSecs
 */
Uint32 PrOS_OsGetMSecsPerOsTick(void);

/**
 * \brief PrOS Set MilliSeconds per OS Tick
 *
 * This function sets the number of milliseconds of time per OS Tick
 *
 * Note: The number of ticks per mSecs is set via the PrOS mechanism of
 * TIC_NUME and TIC_DENO only This function does not change that in any way
 * and is provided so that the baseport can call this functionto set the
 * internal variable so that applications or PAL OS can get the number of
 * milliSeconds that each OS tick corrosponds to.
 *
 * \param   mSecsPerTick is the number of OS ticks per mSecs
 * \return  None
 */
void PrOS_OsSetMSecsPerOsTick(Uint32 mSecsPerTick);

/**
 * \brief PrOS Set Task memory Pool Id
 *
 * This function sets the default Task Memory Pool Id
 *
 * \param   taskPoolId
 * \return  None
 */
void PrOS_OsSetTaskPoolId (ID taskPoolId);

/**
 * \brief PrOS Task Create
 *
 * This function creates a PrOS task by allocating specified stack memory from
 * the default memory pool and activates the task.
 *
 * \param   taskPrio [IN] is the task priority
 * \param   taskFunc [IN] is the task function
 * \param   taskExtraInfo [IN] is extra parameter that can be stored with task
 * \param   taskActivate [IN] is TRUE if task to be activated immediately
 * \param   taskStackSize [IN] is the stack size for the task
 * \return  Positive task ID or negative Error value returned by acre_tsk() PrOS call
 */
Int32 PrOS_OsCreateTask(Int16   taskPrio,
                        Ptr     taskFunc,
                        Ptr     taskExtraInfo,
                        Bool    taskActivate,
                        Uint16  taskStackSize);

/**
 * \brief PrOS Task Activate (Start))
 *
 * This function activates a dormant task
 *
 * \param   taskId [IN] is the taskId created with ..CreateTask()
 * \return  E_OK if successful, else error code returned by act_tsk() PrOS call
 */
Int32 PrOS_OsActivateTask(Int32 taskId);


/**
 * \brief PrOS Task De-Activate
 *
 * This function de-activates a task and moves it to dormant state
 *
 * \param   taskId [IN] is the taskId created with ..CreateTask()
 * \return  E_OK if successful, else error code returned by ter_tsk() PrOS call
 */
Int32 PrOS_OsDeActivateTask(Int32 taskId);


/**
 * \brief PrOS Task Delete (Kill)
 *
 * This function deletes the earlier created PrOS task and frees the stack memory
 * In order to find out the original stack memory space, this implementation is
 * aware of the tcb structure of PrOS where the original stack pointer is stored.
 * THUS THIS FUNCTION IS AWARE AND DEPENDENT UPON PrOS INTERNALS.
 *
 * Note: It is assumed that this function is dormant before being called to delete.
 *
 * \param   taskId [IN] is the taskId created with ..CreateTask()
 * \return  E_OK if successful, else error code returned by del_tsk() PrOS call
 */
Int32 PrOS_OsDeleteTask(Int32 taskId);

/*@}*/

#endif /* __PROS_OS_SERV_H__ */
