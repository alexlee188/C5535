/*
 * $$$MODULE_NAME gpio_control.h
 *
 * $$$MODULE_DESC gpio_control.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#ifndef _GPIO_CONTROL_H_
#define _GPIO_CONTROL_H_

#include <std.h>
#include "csl_gpio.h"

/* Status return codes */
#define GPIOCTRL_SOK        ( 0 )
#define GPIOCTRL_OPEN_FAIL  ( 1 )
#define GPIOCTRL_RESET_FAIL ( 2 )
#define GPIOCTRL_CFG_FAIL   ( 3 )
#define GPIOCTRL_WRITE_FAIL ( 4 )

extern CSL_GpioObj gGpioObj;
extern CSL_GpioObj *hGpio;

// Debug
extern Int16 gpio02State;
extern Int16 gpio04State;

/* Configures the GPIO module */
Int16 gpioInit(
    Uint32 ioDir, 
    Uint32 intEn, 
    Uint32 intEdg
);

Int16 GpioWrite02(
    Uint16 gpioOutputValue
);

Int16 GpioWrite04(
    Uint16 gpioOutputValue
);

#endif /* _GPIO_CONTROL_H_ */
